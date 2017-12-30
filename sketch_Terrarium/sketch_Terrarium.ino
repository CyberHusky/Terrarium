/*
============== Hello! ==============
This is the project that could help you to control watering flowers on OLED display. Point of project is code realization, that you could put your Arduino board in sleeping mode and awake it by pressing the button. I’ve used 10000mAh battery on Arduino UNO, that powered my project all week. If you use another board (Arduino nano or mini) it will powered project more long.

What you required?
- Arduino UNO (or another arduino board);
- OLED display 0,96’’  I2C 128x64 (or another);
- 4 soil humidity sensor;
- 1 button.

Main scheme presented in fritzing file. 

Tested on Arduino UNO on Arduino v1.6.


--- CyberHusky ---

*/

#include <avr/sleep.h>
#include <OLED_I2C.h>

OLED  myOLED(SDA, SCL, 8);

extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

// -------------------------------------------
// ==================  VARIABLES ==================
// -------------------------------------------

int Plant1, Plant2, Plant3, Plant4;
float mean_water;
float proc_water_buf;
int proc_water;
String Plant1_info, Plant2_info, Plant3_info, Plant4_info;
long int delay_time = 3000; // delay time between reading the sensors
String ver = "v1.3.1";

int wakePin = 2; // pin used for waking up
int sleepStatus = 0; // variable to store a request for sleep
int count = 0; // counter




// -------------------------------------------
// ==================  SETUP ==================
// -------------------------------------------
void setup() {

  myOLED.begin();
  myOLED.setFont(SmallFont);

  Serial.begin(9600);

  pinMode(A0, INPUT); // Pins for read sensor data
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  pinMode(3, OUTPUT); // Pins for power
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);

  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);;
  
  pinMode(wakePin, INPUT);

  attachInterrupt(digitalPinToInterrupt(wakePin), wakeUpNow, HIGH); // use interrupt 0 (pin 2) and run function
                                      // wakeUpNow when pin 2 gets LOW 




//  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
//  sleep_enable();
//  sleep_cpu (); 
 
}


void sleepNow()         // here we put the arduino to sleep
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we
     * choose the according
     * sleep mode: SLEEP_MODE_PWR_DOWN
     *
     */  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin

    //sleep_cpu ();
   
    attachInterrupt(digitalPinToInterrupt(wakePin),wakeUpNow, HIGH); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

    //loop();

    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(digitalPinToInterrupt(wakePin));      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
 
}

void wakeUpNow()        // here the interrupt is handled after wakeup
{
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
  
}
// -------------------------------------------
// ==================  LOOP ==================
// -------------------------------------------
void loop() {
count++;

digitalWrite(3, HIGH);
delay(100); // we need to stop for a while, becouse analog pin need time for start working
Plant1 = analogRead(A0);
Serial.println(Plant1);

digitalWrite(5, HIGH);
delay(100); // we need to stop for a while, becouse analog pin need time for start working
Plant2 = analogRead(A1);
Serial.println(Plant2);

digitalWrite(6, HIGH);
delay(100); // we need to stop for a while, becouse analog pin need time for start working
Plant3 = analogRead(A2);
Serial.println(Plant3);

digitalWrite(9, HIGH);
delay(100); // we need to stop for a while, becouse analog pin need time for start working
Plant4 = analogRead(A3);
Serial.println(Plant4);


digitalWrite(3, LOW);
digitalWrite(5, LOW);
digitalWrite(6, LOW);
digitalWrite(9, LOW);


// ---------------------

mean_water = (Plant1 + Plant2 + Plant3 + Plant4)/4; // mean of water saturation
proc_water_buf = round (( (1024-mean_water) / 1024)*100); // procent of water saturation
proc_water = proc_water_buf; // Take fixed part of result
Serial.println(proc_water);

Plant1_info = Plants_info(Plant1,"1"); // function that make an String information about plant water level
Plant2_info = Plants_info(Plant2,"2");
Plant3_info = Plants_info(Plant3,"3");
Plant4_info = Plants_info(Plant4,"4");

myOLED.clrScr();

myOLED.print({"Water level " + ver}, CENTER, 0);
myOLED.print(Plant1_info, LEFT, 16); 
myOLED.print(Plant2_info, LEFT, 26);
myOLED.print(Plant3_info, LEFT, 36);
myOLED.print(Plant4_info, LEFT, 46);
myOLED.print({"Water saturation: "+ String(proc_water, DEC)+"%"}, LEFT, 56);
myOLED.update();

//delay(delay_time);
delay(1000);

Serial.println(count);
 if (count >= 5) {
      Serial.println("Timer: Entering Sleep mode");
      myOLED.clrScr();
      myOLED.update();
      delay(delay_time);     // this delay is needed, the sleep
                      //function will provoke a Serial error otherwise!!
      count = 0;

      sleepNow();     // sleep function called here
  }


//sleepNow();

}




// -------------------------------------------
// ==================  FUNCTIONS ==================
// -------------------------------------------

String Plants_info(int Plant_level, String Plant_num){ // plant water level function
String info;

if (Plant_level > 1 && Plant_level < 400){
  info = {"Plant "+Plant_num+" : ~~~~~~~"};
}
else if (Plant_level >= 400 && Plant_level < 450){
  info = {"Plant "+Plant_num+" : ~~~~~~"};
}
else if (Plant_level >= 450 && Plant_level < 500){
  info = {"Plant "+Plant_num+" : ~~~~~"};
}
else if (Plant_level >= 500 && Plant_level < 550){
  info = {"Plant "+Plant_num+" : ~~~~"};
}
else if (Plant_level >= 550 && Plant_level < 600){
  info = {"Plant "+Plant_num+" : ~~~"};
}
else if (Plant_level >= 600 && Plant_level < 650){
  info = {"Plant "+Plant_num+" : ~~"};
}
else if (Plant_level >= 650 && Plant_level < 700){
  info = {"Plant "+Plant_num+" : ~"};
}
else if (Plant_level >= 700){
  info = {"Plant "+Plant_num+" : need water!!!"};
}
else if (Plant_level <= 1){
  info = {"Sensor "+Plant_num+" isn't work!"};  
}


  return info;
}
