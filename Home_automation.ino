/*********************
Title         :   Home automation using blynk
Description   :   To control light's brigntness with brightness,monitor temperature , monitor water level in the tank through blynk app
Pheripherals  :   Arduino UNO , Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
 *********************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL3zFZpbvAW"
#define BLYNK_TEMPLATE_NAME "Home automation"
#define BLYNK_AUTH_TOKEN "WRDbH2_59H1qPYn1otUQaJtxIPsc6fK5"


// Comment this out to disable prints
//#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

char auth[] = BLYNK_AUTH_TOKEN;
bool heater_sw, inlet_sw, outlet_sw;
unsigned int tank_volume;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// This function is called every time the Virtual Pin 0 state changes
//To turn ON and OFF cooler based virtual PIN value/
BLYNK_WRITE(COOLER_V_PIN) {
  heater_sw = param.asInt();
  //if cooler button is ON on mobile application , then turn on the cooler
  if (heater_sw) {
    cooler_control(ON);
    lcd.setCursor(7, 0);
    lcd.print("CO_LR_ON");
  } else {
    cooler_control(OFF);
    lcd.setCursor(7, 0);
    lcd.print("CO_LR_OFF");
  }
}

//To turn ON and OFF heater based virtual PIN value/
BLYNK_WRITE(HEATER_V_PIN) {
  int value = param.asInt();
  // if heater button is ON on mobile application, then turn on the heater
  if (value) {
    heater_control(ON);
    lcd.setCursor(7, 0);
    lcd.print("HT_R_ON");
  } else {
    heater_control(OFF);
    lcd.setCursor(7, 0);
    lcd.print("HT_R_OFF");
  }
}
//To turn ON and OFF inlet vale based virtual PIN value/
BLYNK_WRITE(INLET_V_PIN) {
  inlet_sw = param.asInt();
  // if inlet valve button at logic high then turn ON the inlet valve else OFF
  if (inlet_sw) {
    enable_inlet();
    // to print the status of valve on CLCD
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_ON");
  } else {
    disable_inlet();
    // to print the status of valve on CLCD
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_OFF");
  }
}
//To turn ON and OFF outlet value based virtual switch value/
BLYNK_WRITE(OUTLET_V_PIN) {
  outlet_sw = param.asInt();
  // if outlet valve button at logic high then turn ON outlet valve else OFF
  if (outlet_sw) {
    enable_outlet();
    // to print the status of valve on CLCD
    lcd.setCursor(7, 1);
    lcd.print("OT_FL_ON");
  } else {
    disable_outlet();
    // to print the status of valve on CLCD
    lcd.setCursor(7, 1);
    lcd.print("OT_FL_OFF");
  }
}


/* To display temperature and water volume as gauge on the Blynk App*/
void update_temperature_reading() {
  Blynk.virtualWrite(TEMPERATURE_GAUGE, read_temperature());
  Blynk.virtualWrite(WATER_VOL_GAUGE, volume() );
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
}

//To turn off the heater if the temperature raises above 35 deg C/
void handle_temp(void) {
  // compare temperature with 35 and check heater is on
  if ((read_temperature() > float(35)) && (heater_sw = ON)) {
    // to turn off the heater
    heater_sw = 0;
    heater_control(OFF);

    // display notification on CLCD
    lcd.setCursor(7, 0);
    lcd.print("HT_R_OFF");

    // to display notification on bylnk
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Temperature is above 35 degree celsius\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, " Turning OFF the heater\n");

    // to reflect OFF on the heater button
    Blynk.virtualWrite(HEATER_V_PIN, OFF);
  }
}

//To control water volume above 2000ltrs/
void handle_tank(void) {
  // compare the volume of water with 2000ltr and also check the status of the inlet valve
  if ((tank_volume < 2000) && (inlet_sw == OFF)) {
    //enable inlet valve and print the status on CLCD
    enable_inlet();
    // to  print the status of valve on CLCD
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_ON");
    inlet_sw = ON;

    // update inlet button status on blynk app as ON
    Blynk.virtualWrite(INLET_V_PIN, ON);

    // print the notification on virtual terminal
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, " Water volume is less than 2000 ltrs\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, " Turning ON the inlet valve\n");
  }

  // check if tank is full then turn off the inlet valve
  if ((tank_volume == 3000) && (inlet_sw == ON)) {
    // disable inlet valve and print the status on CLCD
    disable_inlet();
    // to  print the status of valve on CLCD
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_OFF");
    inlet_sw = OFF;

    // update inlet button status on blynk app as OFF
    Blynk.virtualWrite(INLET_V_PIN, OFF);

    // print the notification on virtual terminal
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water level is full\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, " Turning OFF the inlet valve\n");
  }
}


void setup(void) {
  Blynk.begin(auth);
  // intialize the LCD
  lcd.init();
  // turn the backlight
  lcd.backlight();
  // clear the CLCD
  lcd.clear();
  // cursor to the home
  lcd.home();
  // intialize the temperature system
  init_temperature_system();
  lcd.setCursor(0, 0);
  lcd.print("T=");

  // set cursor to second line to display volume  of water
  lcd.setCursor(0, 1);
  lcd.print("v=");

  // intialising serial tank
  init_serial_tank();

  // update temperature on the every half/0.5 seconds
  timer.setInterval(500L, update_temperature_reading);
}

void loop(void) {
  // control the brightness of garden light using LDR sensor
  brightness_control();

  // just getting upto 2 decimal value hence following modifications
  String temperature;
  temperature = String(read_temperature(), 2);

  // display the temperature on CLCD
  lcd.setCursor(2, 0);
  lcd.print(temperature);

  // display volume
  tank_volume = volume();
  lcd.setCursor(2, 1);
  lcd.print(tank_volume);

  // to check the threshold temperature and controlling heater
  handle_temp();

  // to monitor the volume  of water and if less than 2000ltr turn ON the inlet valve
  handle_tank();

  //  to run the blynk app
  Blynk.run();
  timer.run();
}