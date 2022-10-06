//Firmware for MiniTruss remote lab experiment
// Single controller firmware with state machine for reading and writing.

//Author: David P. Reid
//dprydereid@gmail.com
// 17/03/22

// Update for new LinearServo load control
// 02/05/22
// dprydereid@gmail.com

// Added new WAIT state and READ state now only measures 1 gauge every loop of the state so as not to remain in read state for too long, reducing responsiveness.
// 08/06/22
// dprydereid@gmail.com

// Add a hardware limit switch state and updated load cell pin
// 01/09/22
// dprydereid@gmail.com

// Updated with secure flash storage of calibration data using https://github.com/timdrysdale/nanoflash 
//14/09/22

// Because of slightly different behaviour on different truss setups, have updated the resetting of the truss so that it performs a reset here on startup, but that
//"zeroing" on the UI is really just setting to zero position rather than performing the reset function - which is only really to be used for a hard reset on power
// loss.

// 06/10/22 Added the ability to place hardware back in original startup state -> allows updating of calibration data without power cycle.

// IMPORT LIBRARIES
#include "HX711.h"
#include "LinearServo.h"
#include "ArduinoJson-v6.9.1.h"
#include <FlashStorage.h>

//JSON serialization
#define COMMAND_SIZE 192  //originally 64 - increased for length of cal data + uuid authorization https://arduinojson.org/v6/assistant
StaticJsonDocument<COMMAND_SIZE> doc;
char command[COMMAND_SIZE];

//Linear actuator VARIABLES
#define DRIVE 12
//#define SDIR 15
//#define SPUL 16

int currentPos = 0;     //the position of the servo as output from LinearServo library
int moveToPos = 0;      //the position the servo should move to between 0 (full retraction) to 100 (full extension)
int max_position = 20;  //% extension of linear actuator
LinearServo servo = LinearServo(DRIVE, max_position);

unsigned long move_interval = 5000L;    //this is updated depending on the move distance
unsigned long step_interval = 100L;     //at 5V takes approx. 100ms to move 1 position
unsigned long enter_move_time = millis();    //the time at which the move state started

//LIMIT SWITCHES
#define limitSwitch 11
volatile bool limitReached = false;
unsigned long lastDebounceTime = 0;  // the last time the interrupt was triggered
unsigned long debounceDelay = 50000;    // micro-seconds the debounce time; 
int soft_limit = 15;        //need to have a soft limit which the servo returns to if it hits the hard limit switch

//GAUGE READINGS
const int numGauges = 7;    //6 gauges + load cell
float data[numGauges] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};


//GAUGE SETUP
const int SCK_PIN = 2;  //Common CLOCK pin
const int GAUGE_0_DT = 9; //DATA pins, //LOAD CELL
const int GAUGE_1_DT = 3; //TRUSS MEMBER 1
const int GAUGE_2_DT = 4; //TRUSS MEMBER 2
const int GAUGE_3_DT = 5; //TRUSS MEMBER 3
const int GAUGE_4_DT = 6; //TRUSS MEMBER 4
const int GAUGE_5_DT = 7; //TRUSS MEMBER 5
const int GAUGE_6_DT = 8; //TRUSS MEMBER 6 

int read_index = 0;   //holds a flag for which gauge to read on next loop of state machine.

const int pins[numGauges] = {GAUGE_0_DT, GAUGE_1_DT, GAUGE_2_DT, GAUGE_3_DT, GAUGE_4_DT, GAUGE_5_DT, GAUGE_6_DT};

HX711 gauge_0;
HX711 gauge_1;
HX711 gauge_2;
HX711 gauge_3;
HX711 gauge_4;
HX711 gauge_5;
HX711 gauge_6;

//these will need to be calibrated for new truss setup
// OLD values for large truss
//const float scale_load = -15184;
//const float scale_factor_1 = 4050;
//const float scale_factor_2 = scale_factor_1*1.030;
//const float scale_factor_3 = scale_factor_1*0.977;
//const float scale_factor_4 = scale_factor_1*0.947;
//const float scale_factor_5 = scale_factor_1*0.818;
//const float scale_factor_6 = scale_factor_1*0.924;

//these are the average values of the first 8 trusses. The specific calibration data for each truss is loaded at startup.
float scale_load = -371.8;
float scale_factor_1 = 69.84;
float scale_factor_2 = 99.27;
float scale_factor_3 = 34.44;
float scale_factor_4 = 178.82;
float scale_factor_5 = 67.30;
float scale_factor_6 = 47.86;


HX711 gauges[numGauges] = {gauge_0, gauge_1, gauge_2, gauge_3, gauge_4, gauge_5, gauge_6};


//TIMING FOR GAUGE WRITING
unsigned long timeInterval = 1000;    //write out gauge readings with a period no smaller than 1s
unsigned long previousTime = 0;

//TIMING FOR WAIT STATE
unsigned long waitInterval = 2000;    //this changes depending on what is being waited for.
unsigned long waitStartTime = 0;

//Additional variables for new Truss PCB design - Imogen Heard 18/08/22
#define OUTPUT_ENABLE A1   // Must be set high if using Logic Level Shifters

// SECURE CALIBRATION VARIABLES ============================

#define FLASH_WRITES_MAX 3
#define SECRET_LEN_MAX 99
#define SCALE_FACTOR_LEN 7

// calibration value indices in cal array
#define LOAD_CELL 0
#define MEMBER_1 1
#define MEMBER_2 2
#define MEMBER_3 3
#define MEMBER_4 4
#define MEMBER_5 5
#define MEMBER_6 6

// Create a structure that stores the cal data
typedef struct {
  boolean secure;          // Set this true when secret is first written 
  boolean valid;           // Set this true when calibration data is first written
  char secret[SECRET_LEN_MAX];  // Secret string for authorising calibration updates (typically a uuid of 36 chars in form 8-4-4-4-12)
  int  writes;             // Count number of remaining writes we'll permit
  // You can change the values below here to suit your experiment
  float scale_factor[SCALE_FACTOR_LEN];   // Scale factors for some experiment or other
} Calibration;

// Create a global "Calibration" variable and call it cal
Calibration cal;

// Reserve a portion of flash memory to store a "Calibration" and
// call it "cal_store".
FlashStorage(cal_store, Calibration);

/**
 * Defines the valid states for the state machine
 * 
 */
typedef enum
{
  STATE_STANDBY = 0,        //no drive to motor, no reading of gauges
  STATE_READ = 1,           //reads each gauge
  STATE_MOVE = 2,           //allows stepper motor to move to new position
  STATE_ZERO = 3,           //zeroes the position of the servo
  STATE_TARE_GAUGES = 4,           //tares (zeroes) the gauge readings
  STATE_TARE_LOAD = 5,      //tares the load force gauge
  STATE_TARE_ALL = 6,      //tares both the gauges and load cell
  STATE_GAUGE_RESET = 7,    //resets all gauges
  STATE_WAIT = 8,    //a wait state to allow functions like taring to complete before returning to READ state.
  STATE_CALIBRATE = 9,
  STATE_STARTUP = 10,        // anything to do with starting up the experiment
  STATE_SETSECRET = 11,     // await authorisation setup before accepting calibration data
  STATE_SETCAL = 12,       // await a calibration data before beginning normal operation of the experiment
  STATE_LOADCAL = 13,      // update calibration
  
} StateType;

//state Machine function prototypes
//these are the functions that run whilst in each respective state.
void Sm_State_Standby(void);
void Sm_State_Read(void);
void Sm_State_Move(void);
void Sm_State_Zero(void);
void Sm_State_Tare_Gauges(void);
void Sm_State_Tare_Load(void);
void Sm_State_Tare_All(void);
void Sm_State_Gauge_Reset(void);
void Sm_State_Wait(void);
void Sm_State_Calibrate(void);
void Sm_State_StartUp(void);
void Sm_State_SetSecret(void);
void Sm_State_SetCal(void);
void Sm_State_LoadCal(void);

// cal function prototypes (here because readSerialJSON has to come after StateType definition)
bool cal_is_secure();
bool cal_is_valid();
void startTimer(int);
void report_cal();
void cal_set_secret(const char *);
void cal_set_values(StaticJsonDocument<COMMAND_SIZE>);
StateType readSerialJSON(StateType); //has to come after type definition

/**
 * Type definition used to define the state
 */
 
typedef struct
{
  StateType State; /**< Defines the command */
  void (*func)(void); /**< Defines the function to run */
} StateMachineType;

/**
 * A table that defines the valid states of the state machine and
 * the function that should be executed for each state
 */
StateMachineType StateMachine[] =
{
  {STATE_STANDBY, Sm_State_Standby},
  {STATE_READ, Sm_State_Read},
  {STATE_MOVE, Sm_State_Move},
  {STATE_ZERO, Sm_State_Zero},
  {STATE_TARE_GAUGES, Sm_State_Tare_Gauges},
  {STATE_TARE_LOAD, Sm_State_Tare_Load},
  {STATE_TARE_ALL, Sm_State_Tare_All},
  {STATE_GAUGE_RESET, Sm_State_Gauge_Reset},
  {STATE_WAIT, Sm_State_Wait},
  {STATE_CALIBRATE, Sm_State_Calibrate},
  {STATE_STARTUP,     Sm_State_StartUp},
  {STATE_SETSECRET,   Sm_State_SetSecret}, 
  {STATE_SETCAL,      Sm_State_SetCal},
  {STATE_LOADCAL,     Sm_State_LoadCal}, 

};
 
int NUM_STATES = 14;

/**
 * Stores the current state of the state machine
 */
 
StateType SmState = STATE_STARTUP;    //STATE_GAUGE_RESET will need to be reached before correct functioning

//TESTING ONLY
//unsigned long reportInterval = 1000;    //write out cal data
//unsigned long previousReport = 0;


//DEFINE STATE MACHINE FUNCTIONS================================================================

//TRANSITION: STATE_STANDBY -> STATE_STANDBY
void Sm_State_Standby(void){

//TESTING
//  if(millis() - previousReport >= reportInterval){
//    report_cal_loaded();
//    previousReport = millis();
//  }
  
  SmState = STATE_STANDBY;
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ READ ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_READ -> STATE_READ
// State Read reads the next gauge and stores the data for reporting.
// Remains in read state until user makes the change.
void Sm_State_Read(void){

  if(millis() - previousTime >= timeInterval){
  
   //for(int i=0; i< numGauges; i++){
      //if(gaugeScales[i].wait_ready_timeout(100)){
      if(gauges[read_index].is_ready()){
        
        data[read_index] = gauges[read_index].get_units(10);       //what is the best number of readings to take?
        
      } 

      //delay(10);    //necessary?
   //}

    read_index = (read_index + 1) % numGauges;    //move to next read_index and loop back to 0 after numGauges reached
    report();
    previousTime = millis();
    
  }
  
   SmState = STATE_READ;
  
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ MOVE ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_MOVE -> STATE_READ
//Remains in move state for an appropriate time to complete the move. There is no feedback on position from servo so need to base on time.
//This blocks gauge reading, but high servo speed and slow update of gauges should make this fine.
void Sm_State_Move(void){

  // can only run servo in move state if the lower limit has not been activated
  if(!limitReached)
  {
    currentPos = servo.update();
   
   if(millis() - enter_move_time >= move_interval){
    
      SmState = STATE_READ;
    
   } 
   else
   {
    
      SmState = STATE_MOVE;
    
   }
  } 
  else 
  {
    
    SmState = STATE_READ;
    
  }
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ZERO ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_ZERO -> STATE_READ
//
void Sm_State_Zero(void){
  
  servo.zero();

  currentPos = 0;
  
  waitStartTime = millis();
  waitInterval = 3000;

  limitReached = false;
  SmState = STATE_WAIT;
 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ TARE GAUGES++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_TARE_GAUGES -> STATE_WAIT
void Sm_State_Tare_Gauges(void){
  
  tareGauges();

  waitStartTime = millis();
  waitInterval = 2000;

  SmState = STATE_WAIT;
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ TARE LOAD ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_TARE_LOAD -> STATE_WAIT
void Sm_State_Tare_Load(void){
  
  tareLoad();

  waitStartTime = millis();
  waitInterval = 1000;
  
  SmState = STATE_WAIT;
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ TARE ALL ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_TARE_ALL -> STATE_WAIT
void Sm_State_Tare_All(void){
  
  tareAll();

  waitStartTime = millis();
  waitInterval = 2000;
  
  SmState = STATE_WAIT;
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ GAUGE RESET ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_GAUGE_RESET -> STATE_READ
void Sm_State_Gauge_Reset(void){

  resetGauges();

  waitStartTime = millis();
  waitInterval = 3000;
  
  SmState = STATE_WAIT;
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ WAIT ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_WAIT -> STATE_READ
void Sm_State_Wait(void){

  if(millis() - waitStartTime < waitInterval){

      SmState = STATE_WAIT;
    
  } else {

      SmState = STATE_READ;
    
  }
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ CALIBRATE ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_WAIT -> STATE_READ
void Sm_State_Calibrate(void){

  calibrate();

  waitStartTime = millis();
  waitInterval = 3000;

  SmState = STATE_WAIT;
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ STATE_STARTUP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_STARTUP -> STATE_SECURECAL (if calibration is secure and valid)
void Sm_State_StartUp(void){

    // do any other start up tasks required before checking/loading calibration here
    
    SmState = STATE_SETSECRET;

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ STATE_SETSECRET ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_SETSECRET -> STATE_SETCAL (if cal is secure)
void Sm_State_SetSecret(void){

  if (cal_is_secure()) {
    SmState = STATE_SETCAL; //we let startup state dictate progress through essential tasks
    } else{
      SmState = STATE_SETSECRET;
    }

   
  
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ STATE_SETCAL ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_SETCAL -> STATE_LOADCAL (if cal is set)
void Sm_State_SetCal(void){

  if (cal_is_valid()) {        //IS THIS RIGHT?
      SmState = STATE_LOADCAL;
    } else{
      SmState = STATE_SETCAL;
    }

  
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ STATE_LOADCAL ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TRANSITION: STATE_LOADCAL -> STATE_STANDBY (if cal is set)
void Sm_State_LoadCal(void){

  if(load_cal()){
    SmState = STATE_GAUGE_RESET;      //this should be the final version
    //SmState = STATE_STANDBY;      //FOR TESTING
  } else{
    SmState = STATE_LOADCAL;
  }

  

  
  
}





//STATE MACHINE RUN FUNCTION
void Sm_Run(void)
{
  if (SmState < NUM_STATES)
  {
    SmState = readSerialJSON(SmState);      
    (*StateMachine[SmState].func)();        //reads the current state and then runs the associated function
    
  }
  else{
    Serial.println("Exception in State Machine");
  }
  
}



void setup() {

  pinMode(limitSwitch, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(limitSwitch), doLimit, FALLING);    // lower limit hardware switch will trigger method doLimitLower on press

  pinMode(OUTPUT_ENABLE, OUTPUT);
  digitalWrite(OUTPUT_ENABLE, HIGH);

  previousTime = millis();

  // zero the servo only on startup
  servo.zero();
  delay(2000);    //give it some time to reset

  
  //Serial communication for sending data -> RPi -> Server
  Serial.begin(57600);
  while(!Serial);

  //resetGauges();    //starts in gauge reset state rather than running here.

}

void loop() {

  if(limitReached){
    
    SmState = STATE_ZERO;
    (*StateMachine[SmState].func)();
    
  } 
  else
  {
    
    Sm_Run();
    
  }

  

}


StateType readSerialJSON(StateType SmState){
  if(Serial.available() > 0)
  {

    Serial.readBytesUntil(10, command, COMMAND_SIZE);
    deserializeJson(doc, command);
    
    const char* get = doc["get"];
    
    if(strcmp(get, "cal")==0) {
      report_cal();
      return SmState; // force {"get":cal"} to be a stand-alone command that can't be combined with set
    } else if(strcmp(get, "cal_load")==0) {
      report_cal_loaded();
      return SmState; 
    }

    const char* set = doc["set"];
    
    if(strcmp(set, "position")==0)
    {
  
        float new_position = doc["to"];
        
        if(new_position >= 0 && new_position <= max_position)
        {
          moveToPos = new_position;
          servo.updateMoveTo(moveToPos);
          move_interval = abs(moveToPos - currentPos)*step_interval;
        } 
        else
        {
          Serial.println("Outside position range");
        }
     
  } 
    else if(strcmp(set, "mode")==0)
    {
      
      const char* new_mode = doc["to"];

        if(strcmp(new_mode, "standby") == 0)
        {
          SmState = STATE_STANDBY;
          reportState(STATE_STANDBY);//necessary?
        } 
        else if(strcmp(new_mode, "move") == 0)
        {
          SmState = STATE_MOVE;
          enter_move_time = millis();
          reportState(STATE_MOVE);//necessary?
        }
        else if(strcmp(new_mode, "zero") == 0)
        {
          SmState = STATE_ZERO;
          reportState(STATE_ZERO);//necessary?
        }
        else if(strcmp(new_mode, "tare") == 0)
        {
          SmState = STATE_TARE_GAUGES;
          reportState(STATE_TARE_GAUGES);//necessary?
        }
        else if(strcmp(new_mode, "tare_load") == 0)
        {
          SmState = STATE_TARE_LOAD;
          reportState(STATE_TARE_LOAD);   //necessary?
        }
        else if(strcmp(new_mode, "tare_all") == 0)
        {
          SmState = STATE_TARE_ALL;
          reportState(STATE_TARE_ALL);   //necessary?
        }
        else if(strcmp(new_mode, "gauge_reset") == 0)
        {
          SmState = STATE_GAUGE_RESET;
          reportState(STATE_GAUGE_RESET);//necessary?
        }
        else if(strcmp(new_mode, "calibrate") == 0)
        {
          SmState = STATE_CALIBRATE;
          reportState(STATE_CALIBRATE);//necessary?
        }
        
    }  
    else if(strcmp(set, "secret")==0)
    {
        const char* secret = doc["to"]; 
        cal_set_secret(secret); // this function can be safely called many times because it ignores all but the first non-empty secret 
    } 
    else if(strcmp(set, "cal")==0)
    {
     cal_set_values(doc);
    }
    else if(strcmp(set, "state")==0){
      
       const char* new_state = doc["to"];
       
       if(strcmp(new_state, "STATE_STARTUP") == 0){
          
          SmState = STATE_STARTUP;
       
       }
       
    }
    
  }
      return SmState;     //return whatever state it changed to or maintain the state.
 } 


void doLimit(void){

//  lastDebounceTime = micros();
  limitReached = true;

//  SmState = STATE_ZERO;
//  reportState(SmState);
    
}

void report(){
  Serial.print("{\"load_cell\":");
  Serial.print(data[0]);
  
  for(int i=1;i<numGauges;i++){
    Serial.print(",\"gauge_");
    Serial.print(i);
    Serial.print("\":");
    Serial.print(data[i]);
  }

  Serial.print(",\"state\":");
  Serial.print(SmState);
  Serial.print(",\"pos\":");
  Serial.print(currentPos);
  
  Serial.println("}");
 
}

void reportState(int state){
  Serial.print("{\"state\":");
  Serial.print(state);
  Serial.println("}");
}

void report_cal(){
  Serial.print("{\"report\":\"cal\",\"secure\":");
  Serial.print(cal.secure);
  Serial.print(",\"valid\":");
  Serial.print(cal.valid);
  Serial.print(",\"writes_left\":");
  Serial.print(cal.writes);  
  Serial.print(",\"sf_load\":"); 
  Serial.print(cal.scale_factor[LOAD_CELL]);
  Serial.print(",\"sf_m1\":"); 
  Serial.print(cal.scale_factor[MEMBER_1]); 
  Serial.print(",\"sf_m2\":"); 
  Serial.print(cal.scale_factor[MEMBER_2]);  
  Serial.print(",\"sf_m3\":"); 
  Serial.print(cal.scale_factor[MEMBER_3]); 
  Serial.print(",\"sf_m4\":"); 
  Serial.print(cal.scale_factor[MEMBER_4]);      
  Serial.print(",\"sf_m5\":"); 
  Serial.print(cal.scale_factor[MEMBER_5]); 
  Serial.print(",\"sf_m6\":"); 
  Serial.print(cal.scale_factor[MEMBER_6]);    
  Serial.print(cal.valid); 
  Serial.println("}");
 
}

void report_cal_loaded(){
  Serial.print("{\"report\":\"cal\"");  
  Serial.print(",\"sf_load\":"); 
  Serial.print(scale_load);
  Serial.print(",\"sf_m1\":"); 
  Serial.print(scale_factor_1); 
  Serial.print(",\"sf_m2\":"); 
  Serial.print(scale_factor_2);  
  Serial.print(",\"sf_m3\":"); 
  Serial.print(scale_factor_3); 
  Serial.print(",\"sf_m4\":"); 
  Serial.print(scale_factor_4);      
  Serial.print(",\"sf_m5\":"); 
  Serial.print(scale_factor_5); 
  Serial.print(",\"sf_m6\":"); 
  Serial.print(scale_factor_6);     
  Serial.println("}");
 
}

bool cal_is_secure(){

    cal = cal_store.read();
  
    return cal.secure;
    
  }

bool cal_is_valid(){

    cal = cal_store.read();
 
    return (cal.secure && cal.valid);
    
  }

bool load_cal(){

    cal = cal_store.read();

    if(cal.secure && cal.valid){
        scale_load = cal.scale_factor[LOAD_CELL];
        scale_factor_1 = cal.scale_factor[MEMBER_1];
        scale_factor_2 = cal.scale_factor[MEMBER_2];
        scale_factor_3 = cal.scale_factor[MEMBER_3];
        scale_factor_4 = cal.scale_factor[MEMBER_4];
        scale_factor_5 = cal.scale_factor[MEMBER_5];
        scale_factor_6 = cal.scale_factor[MEMBER_6];
        Serial.println("{\"log\":\"load\",\"cal\":\"loaded\"}"); 
        return true;
    } 
    else
    {
      return false;
    }
    
    
  }

// set the secret for authorising changes to calibration data
void cal_set_secret(const char *secret){

    if (secret[0] == '\0') { 
      return; //empty string
    }
  
    cal = cal_store.read();
    
    if (cal.secure){ // only set secret once
       Serial.println("{\"error\":\"secret already set\"}");
    } else {
        strncpy(cal.secret, secret, (sizeof cal.secret) - 1);
        cal.secure = true;
        cal.writes = FLASH_WRITES_MAX;
        cal_store.write(cal);
        Serial.println("{\"log\":\"secret\",\"is\":\"set\"}"); 
      }
  }

// set the calibration values
void cal_set_values(StaticJsonDocument<COMMAND_SIZE> doc){
  
      cal = cal_store.read();

      if (!cal.secure) {
        Serial.println("{\"error\":\"cal secret not set\"}");
        return; // don't set values before setting authorisation (prevent rogue writes)
      }

      if (cal.writes <= 0) {
        Serial.println("{\"error\":\"no more cal writes permitted - reflash firmware to reset counter\"}");
        return; // prevent writes if remaining write count has reached zero
      }
      
      const char* secret =  doc["auth"];

      
      if (!(strcmp(cal.secret, secret)==0)) {
          Serial.println("{\"error\":\"wrong secret\"}");
          return; // don't set values if auth code does not match secret
        } 

      JsonArray values = doc["to"];

      if (values.size() != SCALE_FACTOR_LEN) { 
        Serial.print("{\"error\":\"wrong number of values in cal array\",");
        Serial.print("\"want\":");
        Serial.print(SCALE_FACTOR_LEN);
        Serial.print(",\"have\":");
        Serial.print(values.size());
        Serial.println("}");
        return; // don't set cal values if wrong number 
        } //size ok

        cal.writes -= 1;
        cal.valid = true;
                        
        Serial.print("{\"log\":\"cal\",\"is\":\"ok\",\"values\":[");
        for (int i=0; i<SCALE_FACTOR_LEN; i++) {
            cal.scale_factor[i]= values[i];
            Serial.print(cal.scale_factor[i]);
            if (i<(SCALE_FACTOR_LEN-1)){
               Serial.print(",");
            } 
            else 
            {
               Serial.print("],\"writes_remaining\":");
               Serial.print(cal.writes);
               Serial.println("}");
            }
          } // for
        
        cal_store.write(cal);
}


void initialiseGauges(){
  for(int i=0;i<numGauges;i++){
    gauges[i].begin(pins[i], SCK_PIN);
  }
}

void setGain(int gain){
  for(int i=0;i<numGauges;i++){
    gauges[i].set_gain(gain);
  }
}

//Just tares the load cell
void tareLoad(){
  data[0] = 0.0;
  gauges[0].tare();
}

//tares all gauge scales, but not load cell
void tareGauges(){
  for(int i=1;i<numGauges;i++){
    data[i] = 0.0;       //set the stored data value to 0
    gauges[i].tare();
   }
}

//tares all gauges, including load cell
void tareAll(){
  for(int i=0;i<numGauges;i++){
    data[i] = 0.0;       //set the stored data value to 0
    gauges[i].tare();
   }
}

void resetGauges(){
  initialiseGauges();
  setGain(128);
 
  gauges[0].set_scale(scale_load);   //OUTPUTS force in newtons
  gauges[1].set_scale(scale_factor_1);          //member 1, calibrated with truss member 1  -> outputs strain in micro-strain
  gauges[2].set_scale(scale_factor_2);          //member 2
  gauges[3].set_scale(scale_factor_3);          //member 3
  gauges[4].set_scale(scale_factor_4);          //member 4
  gauges[5].set_scale(scale_factor_5);          //member 5
  gauges[6].set_scale(scale_factor_6);          //member 6

  
  tareAll();

}

void calibrate(){

  for(int i=0;i<numGauges;i++){
    gauges[i].set_scale();
    gauges[i].tare();
   }

}
