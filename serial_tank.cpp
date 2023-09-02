#include "serial_tank.h"
#include "Arduino.h"
#include "main.h"

unsigned int volume_value;
unsigned int valueh, valuel;

void init_serial_tank(void){
  Serial.begin(19200);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}

unsigned int volume(void){
 //sending command to request volumme of water
 Serial.write(VOLUME);
 //waiting for serial data
 while(!Serial.available());
 //reading higher byte
 valueh=Serial.read();
 while(!Serial.available());
 //reading lower byte
 valuel=Serial.read(); 
 volume_value= (valueh << 8) | valuel;
 return volume_value;
}
 void enable_inlet(void){
   // sequence to enable the inlet valve, send pin number, send enable command
   Serial.write(INLET_VALVE);
   Serial.write(ENABLE);
 }
 void disable_inlet(void){
  //sequence to disable the inlet valve, send pin number, send enable command
  Serial.write(INLET_VALVE);
   Serial.write(DISABLE);
 }
 void enable_outlet(void){
  //sequence to enable the outlet valve, send pin number, send enable command
   Serial.write(OUTLET_VALVE);
   Serial.write(ENABLE);
 }
 void disable_outlet(void){
  //sequence to disable the outlet valve, send pin number, send enable command
  Serial.write(OUTLET_VALVE);
   Serial.write(DISABLE);
 }