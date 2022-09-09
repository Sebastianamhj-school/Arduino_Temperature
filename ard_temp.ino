#include <Wire.h>

/// Blue LED
const int BLED = 2;
/// Yellow LED
const int YLED = 3;
/// Red LED
const int RLED = 4;
/// DS1621 Address
const byte ADDRESS = 0x48;

/// Setup the program
void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(BLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(RLED, OUTPUT);
  tempSetup(ADDRESS);
}

/// Loops through, reading the temperature from the DS1621
/// and the input value from the potentiometer, to set a desired temperature
/// afterwards displays the values on the serial and LED's.
/// It have a 0.5sec delay, if it was any longer it would give too much lag to the
/// potentiometer, and there is no need for it to be any lower.
void loop() {
  int tempInputRaw = potentiometerReader(10);
  
  double currentInputTemp = (double)tempInputRaw/10;
  double currentTemp = tempRead(ADDRESS);
  
  Serial.print("Temp: ");
  Serial.print(currentTemp);
  Serial.print("\t");
  Serial.print("Input: ");
  Serial.println(currentInputTemp);
  setLights(currentTemp, currentInputTemp);
  
  delay(500);
}

/// Reads the potentiometer on the port A0, and returns a value to the nearest 5
/// @param  tempBuffer  set the buffer value for the potentiometer so it doesn't fluctuates.
/// @return returns a value to the nearest 5
int potentiometerReader(int tempBuffer){
  static int lastReading;
  int reader = analogRead(A0);
  
  if(reader > lastReading + tempBuffer || reader < lastReading - tempBuffer ){
    lastReading = reader;
    return (map(lastReading, 0, 1023, 0, 300)/5) * 5;
  }
}

/// Setup the temperature sensor DS1621
/// @param  address The byte address of the DS1621.
void tempSetup(byte address){
  Wire.beginTransmission(address);
  Wire.write(0xAC);
  Wire.write(0);
  Wire.beginTransmission(address);
  Wire.write(0xEE);
  Wire.endTransmission();
}

/// Reads the temperate from the DS1621.
/// @param  address The byte address of the DS1621.
/// @return returns the temperature that the DS1621 measures.
double tempRead(int address){
  Wire.beginTransmission (address);
  Wire.write (0xAA);
  Wire.endTransmission(false);
  Wire.requestFrom (address, 2);
  
  uint8_t temp_msb = Wire.read();
  uint8_t temp_lsb = Wire.read();
  
  double temp = (double)temp_msb;
  
  if(temp_lsb)
    temp += 0.5;
  delay(100);

  return temp;
}

/// Handle the LED depending on temperature and the wanted temperature.
/// @param  currentTemp The temperature read from the DS1621.
/// @param  desiredTemp The desired temperature set from the potetiometer.
void setLights(int currentTemp, int desiredTemp){
  if(currentTemp > desiredTemp){
    digitalWrite(BLED, LOW);
    digitalWrite(YLED, LOW);
    digitalWrite(RLED, HIGH);
  }
  else if(currentTemp < desiredTemp){
    digitalWrite(BLED, HIGH);
    digitalWrite(YLED, LOW);
    digitalWrite(RLED, LOW);
  }
  else {
    digitalWrite(BLED, LOW);
    digitalWrite(YLED, HIGH);
    digitalWrite(RLED, LOW);
  }
}
