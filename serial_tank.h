#ifndef SERIAL_TANK_H
#define SERIAL_TANK_H

//input digital 
#define INLET_VALVE     0X00
#define OUTLET_VALVE    0X01
// sensors digital
#define HIGH_FLOAT      0X10
#define LOW_FLOAT       0X11
//sensor analog
#define VOLUME          0X30

#define   ENABLE       0X01
#define   DISABLE      0X00



void init_serial_tank(void);
unsigned int volume(void);
void enable_inlet(void);
void disable_inlet(void);
void enable_outlet(void);
void disable_outlet(void);

#endif
