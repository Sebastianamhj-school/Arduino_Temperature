#include <Wire.h>

#define BLED 2        // Blue LED
#define YLED 3        // Yellow LED
#define RLED 4        // Red LED
#define ADDRESS 0x48  // DS1621 Address

int lastAnalogTemp = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(BLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(RLED, OUTPUT);
  tempSetup(ADDRESS);
}

void loop() {
  int tempInputRaw = analogTempReader(10);
  int tempInputMapped = (map(tempInputRaw, 0, 1023, 0, 300)/5)*5;  /// Round the reading value to the nearest 5,
  
  double currentInputTemp = (double)tempInputMapped/10;
  double currentTemp = tempRead(ADDRESS);
  
  Serial.print("Temp: ");
  Serial.print(currentTemp);
  Serial.print("\t");
  Serial.print("Input: ");
  Serial.println(currentInputTemp);
  setLights(currentTemp, currentInputTemp);
  
  delay(500);
}

/// Reads the potentiometer, to get the wanted tempeture
int analogTempReader(int tempBuffer){
  int reader = analogRead(A0);
  if(reader > lastAnalogTemp + tempBuffer || reader < lastAnalogTemp - tempBuffer ){
    lastAnalogTemp = reader;
    return lastAnalogTemp;
  }
}

/// Setup the temperature sensor DS1621
void tempSetup(byte address){
  Wire.beginTransmission(address);
  Wire.write(0xAC);                   /// Access Config
  Wire.write(0);
  Wire.beginTransmission(address);
  Wire.write(0xEE);                   /// Start Convert
  Wire.endTransmission();
}

/// Reads the temperate from the DS1621.
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
void setLights(int currentTemp, int wantedTemp){
  if(currentTemp > wantedTemp){
    digitalWrite(BLED, LOW);
    digitalWrite(YLED, LOW);
    digitalWrite(RLED, HIGH);
  }
  else if(currentTemp < wantedTemp){
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
