

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