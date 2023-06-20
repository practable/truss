#include <Arduino.h>
#include "HX711.h"
#include <buttonObject.h>  // Does not work with the HX711 libary! :(
#include <autoDelay.h>
#include <ledObject.h>

/*

How to calibrate your load cell
Call set_scale() with no parameter.
Call tare() with no parameter.
Place a known weight on the scale and call get_units(10).
Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale().
Adjust the parameter in step 4 until you get an accurate reading.


*/

// HX711 circuit wiring
const int LOADCELL_DO0_PIN = 3;
const int LOADCELL_DO1_PIN = 4;
const int LOADCELL_DO2_PIN = 5;
const int LOADCELL_DO3_PIN = 6;
const int LOADCELL_DO4_PIN = 7;
const int LOADCELL_DO5_PIN = 8;
const int LOADCELL_DO6_PIN = 9;
const int LOADCELL_SCK_PIN = 2;

int sensorPinArray[7] = { 3, 4, 5, 6, 7, 8, 9 };

HX711 sensorArray[7];

#define LIMIT_SWITCH 11
#define LED_PIN 13
#define ENABLE_PIN A1

ledObject led(LED_PIN);

//buttonObject tareButton(LIMIT_SWITCH, BUTTON_PULL_LOW);

autoDelay sampleDelay;

void testChannel(int channelNo) {
  Serial.print("Testing Channel: ");
  Serial.println(channelNo);
  Serial.print("  read: ");
  Serial.println(sensorArray[channelNo].read());  // print a raw reading from the ADC
  delay(500);
  Serial.print("  read average: ");
  Serial.println(sensorArray[channelNo].read_average(20));  // print the average of 20 readings from the ADC
  delay(500);
  Serial.print("  get value: ");
  Serial.println(sensorArray[channelNo].get_value(5));  // print the average of 5 readings from the ADC minus the tare weight
  delay(500);
  Serial.print("  get units: ");
  Serial.println(sensorArray[channelNo].get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight divided by the SCALE parameter
  delay(500);
}



void tareScale(int channel = 0) {
  Serial.println("Tare in 5 seconds... remove any weights from the sensorArray[i].");
  countdown(5);
  Serial.println("Taring...");
  countdown(5);
  sensorArray[channel].tare();  // Step 2
  Serial.println("Tare Complete..");
  delay(500);
}

void countdown(int from) {
  for (int i = from; i > 0; i--) {
    Serial.print(i);
    Serial.print("...");
    delay(1000);
  }
  Serial.println("0");
}



float scale_val;

void calibrateChannel(int channel, float known_weight) {
  Serial.print("Calibrating Sensor Number: ");
  Serial.println(channel);
  if (sensorArray[channel].is_ready()) {
    sensorArray[channel].set_scale();  // Step 1
    tareScale(channel);
    Serial.println("Now Calibrating Scale");
    Serial.println("Place a known weight on the scale...");  // Known weight in this case is 1kg, however the strain value this should result in ~373n strain or 0.373u strain.
    countdown(5);
    Serial.println("Taking Test Measurements");
    countdown(5);
    long reading = sensorArray[channel].get_units(10);  // Step 3
    Serial.print("Test Reading: ");
    Serial.println(reading);
    delay(500);
    //Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale(). "TO your known weight" - what ambiguous wording.
    // Assuming calculation is reading/X = known_weight or reading/known_weight = SCALE
    // Alternative: reading * scale = known_weight. therefore known_weight/reading = scale

    scale_val = float(reading) / known_weight;
    Serial.print("scale_val: ");
    Serial.println(scale_val);
    sensorArray[channel].set_scale(scale_val);
  } else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}








// Untested
void autoScale(int channel, float target_val = 0, float range = 50.0) {
  float init_scale = scale_val;  // save the init value incase autoscale fails
  bool scale_set = false;
  while (!scale_set) {
    Serial.print("Setting Scale, scale_val: ");
    Serial.println(scale_val);
    sensorArray[channel].set_scale(scale_val);
    long sensor_read = sensorArray[channel].get_units(2);
    if (scale_val < 0.1 && scale_val > -0.1) {
      Serial.print("Scale autoScale Failed, Reverting to: ");
      Serial.println(init_scale);
      scale_val = init_scale;
      scale_set = true;
      return;
    } else if (sensor_read - target_val > range) {
      scale_val = scale_val - 0.01;
      Serial.println("reading high");
    } else if (sensor_read - target_val < -1 * range) {
      scale_val = scale_val + 0.01;
      Serial.println("reading low");
    } else {
      scale_set = true;
      Serial.print("Scale in Range, scale_val: ");
      Serial.println(scale_val);
      return;
    }
  }
}



#define CURRENT_CHANNEL 0
//#define KNOWN_WEIGHT 373.0  // In expected nano strain
#define KNOWN_WEIGHT_N 10.0
#define TARGET_RANGE 25.0

void setup() {
  pinMode(ENABLE_PIN, OUTPUT);  // Output Enable for Logic Level Conversion
  digitalWrite(ENABLE_PIN, HIGH);
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  led.begin(true);

  pinMode(LIMIT_SWITCH, INPUT);

  Serial.println("\n\nHX711 Demo");

  Serial.println("Initializing the scale");

  for (int i = 0; i < 8; i++) {
    sensorArray[i].begin(sensorPinArray[i], LOADCELL_SCK_PIN);  // NOTE THIS STOPS BUTTON LIBARY FROM WORKING
  }

  Serial.println("Before setting up the scale:");
  testChannel(CURRENT_CHANNEL);  // Test channel although SCALE parameter and tare weight are not set
  calibrateChannel(CURRENT_CHANNEL, KNOWN_WEIGHT_N);
  Serial.println("After setting up the scale:");
  testChannel(CURRENT_CHANNEL);  // Retest channel now scale and tare are set
                                 //  autoScale(CURRENT_CHANNEL, KNOWN_WEIGHT, TARGET_RANGE);
  Serial.println("Test Completed, Taring Empty Scale");
  tareScale(CURRENT_CHANNEL);
  Serial.println("Setup Complete. Start Data Aquisition");
  led.callBlink(6, 100, 200);
}





void loop() {

  int tareButton = digitalRead(LIMIT_SWITCH);
  if (tareButton == 0) {
    Serial.println("Tare Button Pressed");
    tareScale(CURRENT_CHANNEL);  // this is blocking at the moment, so LED will flash when tare is complete
    led.callBlink(6, 100, 200);
  }


  if (sampleDelay.millisDelay(500)) {
  //  Serial.print("Raw Read: ");
   // float sensor_raw;
   // sensor_raw = sensorArray[CURRENT_CHANNEL].read();
   // Serial.print(sensor_raw);  // print a raw reading from the ADC
   // Serial.print(" | scale: ");
   // Serial.print(scale_val);
    Serial.print("Force: ");
    Serial.print(sensorArray[CURRENT_CHANNEL].get_units(), 4);
    // Serial.print("u | average: ");
    Serial.print(" N");
    // Serial.print(sensorArray[CURRENT_CHANNEL].get_units(10), 4);
    Serial.println();
  }
  led.performBlink();
}

