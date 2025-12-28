#include <DHT11.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>
#include <DS3231.h>

//defines
DHT11 dht11(2);
#define SCREEN_W 128
#define SCREEN_H 32
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)
#define DO_PIN 7

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, OLED_RESET);
Adafruit_BMP280 bmp; 
RTClib myRTC;

class STM{
  private:
    int temp;
    int humidity;
    float temp_flt;
    float pressure;
    bool daylight;
    DateTime now;
  public:
    
    STM(int temp_, int humidity_);
    STM(int temp_, int humidity_, float temp_flt_, float pressure_);
    STM(DateTime now_);
    STM(int temp_, int humidity_, float temp_flt_, float pressure_, DateTime now_);
    STM(int temp_, int humidity_, float temp_flt_, float pressure_, DateTime now_, bool daylight_);
    void afisare_consola();
    void afisare_display();
};

String format(int v){
  return (v < 10) ? "0" + String(v) : String(v);
}

void STM::afisare_consola()
{
  String timp = format(now.hour()) + ":"+ format(now.minute())+":"+format(now.second());
  String zi = ((daylight) ? "Night" : "Day");

  Serial.print("{");
  Serial.print("\"time\":\""); Serial.print(timp); Serial.print("\",");
  Serial.print("\"daylight\":\""); Serial.print(zi); Serial.print("\",");
  Serial.print("\"temp_int\":"); Serial.print(temp); Serial.print(",");
  Serial.print("\"humidity\":"); Serial.print(humidity); Serial.print(",");
  Serial.print("\"temp_float\":"); Serial.print(temp_flt); Serial.print(",");
  Serial.print("\"pressure\":"); Serial.print(pressure);
  Serial.println("}");
}

void STM::afisare_display()
{
  String temperatura = "Temperatura: " + String(temp_flt) + "C";
  String umiditate = "Umiditate: " + String(humidity) + "%";
  String time_day = format(now.hour()) + ":"+ format(now.minute()) + " | " + ((daylight) ? "Night" : "Day");//day / night de facut ceas + daylight sensor 
  String presiune = "Pressure: " + String(pressure) + " Pa";
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0); 
  display.setTextColor(SSD1306_WHITE); 
  display.println(time_day);
  display.println(temperatura);
  display.println(umiditate);
  display.println(presiune);
  display.display();
}

STM::STM(DateTime now_)
{
  now = now_;
}

STM::STM(int temp_, int humidity_)
{
  temp = temp_;
  humidity = humidity_;
}

STM::STM(int temp_, int humidity_, float temp_flt_, float pressure_)
{
  temp = temp_;
  humidity = humidity_;
  temp_flt = temp_flt_;
  pressure = pressure_;

}

STM::STM(int temp_, int humidity_, float temp_flt_, float pressure_, DateTime now_)
{
  temp = temp_;
  humidity = humidity_;
  temp_flt = temp_flt_;
  pressure = pressure_;
  now = now_;
}

STM::STM(int temp_, int humidity_, float temp_flt_, float pressure_, DateTime now_, bool daylight_)
{
  temp = temp_;
  humidity = humidity_;
  temp_flt = temp_flt_;
  pressure = pressure_;
  now = now_;
  daylight = daylight_;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(DO_PIN,INPUT);
  //Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
   
  }
  display.clearDisplay();

  unsigned status;
  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
  // put your main code here, to run repeatedly:
  int temp_int, humidity;
  int results = dht11.readTemperatureHumidity(temp_int, humidity);
  STM p = STM(temp_int, humidity, bmp.readTemperature(), bmp.readPressure(), myRTC.now(), digitalRead( DO_PIN ) == 1);
  if(results == 0)
  {
    p.afisare_consola();
    p.afisare_display();
  }
  else
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0); 
    display.setTextColor(SSD1306_WHITE); 
    display.println("error");
    display.display();
    Serial.println(DHT11::getErrorString(results));
  }
 
  delay(2000);
}
