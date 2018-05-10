/*
============== Hello! ==============
This is the project that could help you to control watering flowers on OLED display. 
This is the fork of original project, that include RGB diod for indicate of plants water.

What you required?
- Arduino UNO (or another arduino board);
- OLED display 0,96’’  I2C 128x64 (or another);
- 4 soil humidity sensor;
- RGB diod.

Main scheme presented in fritzing file. 

Tested on Arduino UNO on Arduino v1.6.


--- CyberHusky ---

*/

#include <OLED_I2C.h>

OLED myOLED(SDA, SCL, 8);

extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

// -------------------------------------------
// ==================  VARIABLES =============
// -------------------------------------------

int Plant1, Plant2, Plant3, Plant4;
int proc_water;
float mean_water;
float proc_water_buf;
String Plant1_info, Plant2_info, Plant3_info, Plant4_info;
String ver = "v1.4.0";
long int delay_time = 10000; // delay time between reading the sensors


int wakePin		  = 2; // pin used for waking up
int sleepStatus = 0; // variable to store a request for sleep
size_t count 		= 0; // counter


struct RGB{
  int REDpin    = 13;
  int GREENpin  = 12;
  int BLUEpin   = 11;

  void setRed(){
    digitalWrite(REDpin,   HIGH);
    digitalWrite(GREENpin, LOW);
    digitalWrite(BLUEpin,  LOW);
  }
  void setBlue(){
    digitalWrite(REDpin,   LOW);
    digitalWrite(GREENpin, LOW);
    digitalWrite(BLUEpin,  HIGH);
  }
  void setGreen(){
    digitalWrite(REDpin,   LOW);
    digitalWrite(GREENpin, HIGH);
    digitalWrite(BLUEpin,  LOW);
  }
  
}RGB_diod;



// -------------------------------------------
// ==================  SETUP =================
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
						          // Pin for diod
	pinMode(RGB_diod.BLUEpin,  OUTPUT); // Blue
	pinMode(RGB_diod.GREENpin, OUTPUT); // Green
	pinMode(RGB_diod.REDpin,   OUTPUT); // Red
	  
}


// -------------------------------------------
// ==================  LOOP ==================
// -------------------------------------------
void loop() {
	count++;

	digitalWrite(3, HIGH);
	delay(100); // we need to stop for a while, becouse analog pin need time for start working
	Plant1 = analogRead(A0);
  Serial.print("Plant1: ");
	Serial.println(Plant1);

	digitalWrite(5, HIGH);
	delay(100); // we need to stop for a while, becouse analog pin need time for start working
	Plant2 = analogRead(A1);
  Serial.print("Plant2: ");
	Serial.println(Plant2);

	digitalWrite(6, HIGH);
	delay(100); // we need to stop for a while, becouse analog pin need time for start working
	Plant3 = analogRead(A2);
  Serial.print("Plant3: ");
	Serial.println(Plant3);

	digitalWrite(9, HIGH);
	delay(100); // we need to stop for a while, becouse analog pin need time for start working
	Plant4 = analogRead(A3);
  Serial.print("Plant4: ");
	Serial.println(Plant4);


	digitalWrite(3, LOW);
	digitalWrite(5, LOW);
	digitalWrite(6, LOW);
	digitalWrite(9, LOW);


// ---------------------

	mean_water = (Plant1 + Plant2 + Plant3 + Plant4)/4; // mean of water saturation
	proc_water_buf = round (( (1024-mean_water) / 1024)*100); // procent of water saturation
	proc_water = proc_water_buf; // Take fixed part of result
	Serial.print(proc_water);
  Serial.println("%");

	Plant1_info = Plants_info(Plant1, "1"); // function that make an String information about plant water level
	Plant2_info = Plants_info(Plant2, "2");
	Plant3_info = Plants_info(Plant3, "3");
	Plant4_info = Plants_info(Plant4, "4");

	myOLED.clrScr();

	myOLED.print({"Water level " + ver}, CENTER, 0);
	myOLED.print(Plant1_info, LEFT, 16); 
	myOLED.print(Plant2_info, LEFT, 26);
	myOLED.print(Plant3_info, LEFT, 36);
	myOLED.print(Plant4_info, LEFT, 46);
	myOLED.print({ "Water saturation: " + String(proc_water, DEC) + "%"}, LEFT, 56);
	myOLED.update();

  // ----------diod-------
  if(proc_water < 60)
    PlantSOS(RGB_diod.REDpin, RGB_diod.GREENpin, RGB_diod.BLUEpin);
  else if (proc_water >= 60 && proc_water < 65)
    RGB_diod.setBlue();
  else if (proc_water >= 65)
    RGB_diod.setGreen();

  // ---------------------
  
  delay(delay_time);  // delay time - between reading the sensors
}




// -------------------------------------------
// ==================  FUNCTIONS ==================
// -------------------------------------------

// Plant SOS signal
void PlantSOS(int red_pin, int green_pin, int blue_pin){
  digitalWrite(green_pin, LOW);
  digitalWrite(blue_pin,  LOW);
  
  for (int i = 0; i < 3; i++) 
  {
    digitalWrite(red_pin, HIGH);
    delay(250);        
    digitalWrite(red_pin, LOW); 
    delay(100);         
   }
   delay(300);
   for (int i = 0; i < 3; i++) 
   {
    digitalWrite(red_pin, HIGH);
    delay(500);        
    digitalWrite(red_pin, LOW); 
    delay(100);         
   }
   delay(300);
   for (int i = 0; i < 3; i++) 
   {
    digitalWrite(red_pin, HIGH);
    delay(250);       
    digitalWrite(red_pin, LOW); 
    delay(100);        
   }
  delay(1000); // ждем 1 секунд
  Serial.println( "Plants make the SOS signal!!!" );
}


// Plant water level function
String Plants_info(int Plant_level, String Plant_num){ 
	String info;

	if (Plant_level > 1 && Plant_level < 400)
		info = { "Plant " + Plant_num + " : ~~~~~~~" };

	else if (Plant_level >= 400 && Plant_level < 450)
		info = { "Plant " + Plant_num + " : ~~~~~~" };

	else if (Plant_level >= 450 && Plant_level < 500)
		info = { "Plant " + Plant_num + " : ~~~~~" };

	else if (Plant_level >= 500 && Plant_level < 550)
		info = { "Plant " + Plant_num + " : ~~~~" };

	else if (Plant_level >= 550 && Plant_level < 600)
		info = { "Plant " + Plant_num + " : ~~~" };

	else if (Plant_level >= 600 && Plant_level < 650)
		info = { "Plant " + Plant_num + " : ~~" };

	else if (Plant_level >= 650 && Plant_level < 700)
		info = { "Plant " + Plant_num + " : ~" };

	else if (Plant_level >= 700)
		info = { "Plant " + Plant_num + " : need water!!!" };

	else if (Plant_level <= 1)
		info = { "Sensor " + Plant_num + " isn't work!" };  



	return info;
}
