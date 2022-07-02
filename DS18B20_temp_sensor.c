#include "DS18B20_temp_sensor.h"


#define ONE_WIRE_BUS 2 // Data wire is plugged into pin 2 on the Arduino
#define RESOLUTION 12 // Resolution to choose: 12bit -> 0,0625; 11bit -> 0,125; 10bit -> 0,25; 9bit -> 0,5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
// Data wire is connected to pin ONE_WIRE_BUS and with a pullup 4.7k resistor to 5V

LiquidCrystal_I2C lcd(0x27,16,2);

DeviceAddress sensor0 = {0,0,0,0,0,0,0,0}, sensor1 = {0,0,0,0,0,0,0,0};


float insideTemp;
float outsideTemp;
int upperTempBorder = 150;
int lowerTempBorder = -50;
float getResolution() {return float(256)/pow(2, sensors.getResolution());}

float oldInsideTemp = 23., oldOutsideTemp = 23.;

signed int start, end;
byte lcdDegree[8] = {
  0b00010,
  0b00101,
  0b00010,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte up[8] = {
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte down[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10001,
  0b01010,
  0b00100
};

void setup(void){
	Serial.begin(9600);

	sensors.begin();
	sensors.setResolution(RESOLUTION);
	sensors.getAddress(sensor0, 0);
	sensors.getAddress(sensor1, 1);

	Serial.println("Digital Temperature");
	Serial.print("Resolution: ");
	Serial.print(getResolution(), 4);
	Serial.println("\n sensor0: "); for(uint8_t i=0; i<8; i++) Serial.print(sensor0[i], HEX);
	Serial.println("\n sensor1: "); for(uint8_t i=0; i<8; i++) Serial.print(sensor1[i], HEX);
	Serial.println("");

	Wire.begin();

	lcd.init();
	lcd.clear();
	lcd.backlight();
	lcd.setCursor(0,1);		lcd.print("inside");
	lcd.setCursor(9,1);		lcd.print("outside");
	lcd.createChar(0, lcdDegree); // create a new custom character
	lcd.createChar(1, up); // create a new custom character
	lcd.createChar(2, down); // create a new custom character
}

void loop(void){
	start = millis();

	{ // show dot while requesting temperatures
		lcd.setCursor(7, 1); lcd.print('.');
		sensors.requestTemperatures();
		sensors.isConnected(sensor0) ? insideTemp  = sensors.getTempC(sensor0) : insideTemp  = -100;
		sensors.isConnected(sensor1) ? outsideTemp = sensors.getTempC(sensor1) : outsideTemp = -100;
		lcd.setCursor(7, 1); lcd.print(' ');
	}

	{
		lcd.setCursor(0, 0);
		if(insideTemp  > lowerTempBorder && insideTemp  < upperTempBorder) {
			lcd.print(String(((insideTemp +oldInsideTemp )/2),1)); lcd.write((byte)0); lcd.print("C");
		} else lcd.print("ERROR0");
		if(oldInsideTemp < insideTemp) { lcd.write((byte)1 ); } else
		if(oldInsideTemp > insideTemp) { lcd.write((byte)2 ); } else {}
	}
	{
		lcd.setCursor(9,0);
		if(oldOutsideTemp < outsideTemp) { lcd.write((byte)1 ); } else
		if(oldOutsideTemp > outsideTemp) { lcd.write((byte)2 ); } else {}
		lcd.setCursor(10,0);
		if(outsideTemp > lowerTempBorder && outsideTemp < upperTempBorder) {
			lcd.print(String(((outsideTemp+oldOutsideTemp)/2),1)); lcd.write((byte)0); lcd.print("C");
		} else lcd.print("ERROR1");
	}

	Serial.println("");
	Serial.println("Temperature of  inside is: "
			+ (insideTemp  > lowerTempBorder
					? String(	( (insideTemp +oldInsideTemp ) /2) ,1 )
					: String(insideTemp))
			+ "C");
	Serial.println("Temperature of outside is: "
			+ (outsideTemp > lowerTempBorder
					? String(((outsideTemp+oldOutsideTemp)/2),1)
					: String(insideTemp))
			+ "C");

	if(insideTemp  > lowerTempBorder && insideTemp  < upperTempBorder) oldInsideTemp  = insideTemp;
	if(outsideTemp > lowerTempBorder && outsideTemp < upperTempBorder) oldOutsideTemp = outsideTemp;

	end = millis();
	delay( 1000 - ((end - start) < 0 ? 1000 : (end - start)) );
}


int pow(int x,int n){
    int number = 1;
    int i = 0;

    do{
    	number *= x;
    	i++;
    } while(i != n);

    return(number);
}
