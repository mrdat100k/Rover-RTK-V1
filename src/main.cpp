#include <Arduino.h>
#include "BluetoothSerial.h"
#include "Zed_F9P.h"
#include "Oled.h"
#include "Battery.h"

#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_ADDR   0x3C
#define ADC_PIN     34
#define LCD_WAIT    75

#define SERIAL_SIZE_BUFFER 20000 //Khai báo buffer lớn
uint8_t r_buffer[SERIAL_SIZE_BUFFER]; //buffer for reading Zed-F9P
uint8_t w_buffer[SERIAL_SIZE_BUFFER]; //buffer for writing Zed-F9P

uint8_t unit_MAC_address[6]; //MAC address in BT broadcast
char device_name[20];     //the serial string that is broadcast
char NMEA_buffer[120];    //Buffer for NMEA processing
ZED_F9P ZF9P(UART2, NMEA_buffer, sizeof (NMEA_buffer));
BluetoothSerial Serial_BT;
OLED display(OLED_WIDTH, OLED_HEIGHT);
battery Battery(ADC_PIN);

void beginBT();
void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
bool startBluetooth();
void F9PSerialWriteTask(void *e);
void F9PSerialReadTask (void *e);
void OLED_Print();

void setup()
{
  Serial.begin (9600);    //begin uart0 with baud speed 9600
  Serial.setRxBufferSize(SERIAL_SIZE_BUFFER);
  Serial.setTimeout(1);

  Wire.begin ();          //begin i2c port of esp32
  ZF9P.begin(115200);     //begin uart and i2c for transmission with zed-fp9
  ZF9P.setRxBufferSize(SERIAL_SIZE_BUFFER);
  ZF9P.setTimeout(1);
  

  beginBT();      //begin bluetooth serial
  
  if (ZF9P.config_GNSS ())
  {
    Serial.println ("Successfully Configure");
    ZF9P.saveConfiguration();
  }

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop()
{

  OLED_Print();

}


void beginBT()
{
  //Get unit MAC address
  esp_read_mac(unit_MAC_address, ESP_MAC_WIFI_STA);
  unit_MAC_address[5] += 2; //Convert MAC address to Bluetooth MAC (add 2): https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system.html#mac-address

  Serial_BT.register_callback(btCallback);
  if (startBluetooth() == false)
  {
    Serial.println("An error occurred initializing Bluetooth");
  }

}

//Call back for when BT connection event happens (connected/disconnect)
//Used for updating the bluetoothState state machine
void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    Serial.println("Client Connected");
  }

  if (event == ESP_SPP_CLOSE_EVT ) {
    Serial.println("Client disconnected");
  }
}


bool startBluetooth()
{
  sprintf(device_name,"IMET RTK rover -%02X%02X", unit_MAC_address[4], unit_MAC_address[5]);
  if (Serial_BT.begin(device_name) == false)
  {
    return (false);
  }
  Serial.print("Bluetooth broadcasting as: ");
  Serial.println(device_name);
  //Start task for handling incomming and outgoing BT bytes to/from Zed-F9p
  xTaskCreate(F9PSerialReadTask, "F9Read", 10000, NULL, 0, NULL);
  xTaskCreate(F9PSerialWriteTask, "F9Write", 10000, NULL, 0, NULL);

  Serial_BT.setTimeout(1);

  return (true);
}

//If the phone has any new data (NTRIP RTCM, etc), read it in over Bluetooth and pass along to ZED
//Task for writing to the GNSS receiver
void F9PSerialWriteTask(void *e)
{
  while (true)
  {
    //Receive corrections from either the ESP32 USB or bluetooth
    //and write to the GPS
    if (Serial.available())
      {
        auto s = Serial.readBytes(w_buffer, SERIAL_SIZE_BUFFER);
        ZF9P.write(w_buffer, s);
      }

    if (Serial_BT.available())
    {
      while (Serial_BT.available())
      {
        auto s = Serial_BT.readBytes(w_buffer, SERIAL_SIZE_BUFFER);
        ZF9P.write(w_buffer, s);
      }
    }

    taskYIELD();
  }
}

//If the ZED has any new NMEA data, pass it out over Bluetooth
//Task for reading data from the GNSS receiver.
void F9PSerialReadTask(void *e)
{
  while (true)
  {
    if (ZF9P.available())
    {
      auto s = ZF9P.readBytes(r_buffer, SERIAL_SIZE_BUFFER);
      //debug
      //Serial.printf("Buffer length = %d\r\n", s);
      //Pass the stream to NMEA processor
      uint16_t iter;
      for (iter = 0; iter < s; iter++)
      {
        ZF9P.processData(r_buffer[iter]);
      }
      //end
    if (Serial_BT.connected())
      {
        Serial_BT.write(r_buffer, s);
        //Serial.write (r_buffer,s);
      }
    }

    taskYIELD();
  }
}


void OLED_Print()
{

  display.clearDisplay();

  display.print_battery_level(Battery.get_battery_level());

  display.setCursor(0, 0);
  display.print(ZF9P.getHour() + 7);
  display.print(":");
  uint8_t minute = ZF9P.getMinute();
  if (minute < 10)
  {
    display.print("0");
    display.print(minute);
  }
  else 
  {
    display.print(minute);
  }
  display.print(":");
  uint8_t second = ZF9P.getSecond();
  if (second < 10)
  {
    display.print("0");
    display.print(second);
  }
  else
  {
    display.print(second);
  }
  delay(LCD_WAIT);

  display.setCursor(60, 0);
  display.print(ZF9P.getDay());
  display.print("/");
  display.print(ZF9P.getMonth());
  display.print("/");
  display.print(ZF9P.getYear());
  delay(LCD_WAIT);

  display.setCursor(0, 8);
  display.print("Lat: ");
  display.print(ZF9P.getLatitude() / 10000000.0, 7);
  delay(LCD_WAIT);

  display.setCursor(0, 16);
  display.print("Lon: ");
  display.print(ZF9P.getLongitude() / 10000000.0, 7);
  delay(LCD_WAIT);

  display.setCursor(0, 24);
  display.print("Alt: ");
  display.print(ZF9P.getAltitude() / 1000.0, 3);
  display.print("m");
  delay(LCD_WAIT);

  display.setCursor(0, 32);
  display.print("Status: ");
  display.print_GNSS_status(ZF9P.getFixType(), ZF9P.pubx_get_posMode());
  delay(LCD_WAIT);

  display.setCursor(0, 40);
  display.print("Number of Sat: ");
  display.print(ZF9P.getSIV());
  delay(LCD_WAIT);

  // display.setCursor(0, 48);
  // display.print("PDOP: ");
  // display.print(ZF9P.getPDOP() / 100.0, 2);
  // delay(LCD_WAIT);

  display.setCursor(0, 48);
  display.print("2D Accuracy: ");
  display.print(ZF9P.getHorizontalAccuracy() / 100.0, 1);
  display.print("cm");
  delay(LCD_WAIT);

  display.setCursor(0, 56);
  display.print_bluetooth_paired_status(Serial_BT.connected());
  delay(LCD_WAIT);

  display.display();

  delay(LCD_WAIT);

}
