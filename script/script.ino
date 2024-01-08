//Include Libraries
#include <Wire.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/RTDBHelper.h"
#include <dht.h>
#include <WiFiManager.h>
// End Include Libraries


//Define Firebase URL And Secret
#define DATABASE_URL "iot-farm-ffe52-default-rtdb.asia-southeast1.firebasedatabase.app"  //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "PJowHANQ6mFaTyO8JweSXJ0wckLUUNpghCjRsxQG"
//End Define Firebase URL And Secret

//Define Relay
#define _ch1 0 // untuk menentukan channel relay 1
#define _ch2 1 // untuk menentukan channel relay 2
#define Relay1 D4 // Relay 1 berada pada pin D4
#define Relay2 D5 // Relay 2 berada pada pin D5
//End Define Relay

//Define Timer Untuk Sistem Otomatis Waktu Pagi Siang Dan Sore
#define _jam_start_pagi 0 //berguna untuk menentukan jam mulai pagi berada pada array index 0
#define _jam_start_siang 0 //berguna untuk menentukan jam mulai siang berada pada array index 0
#define _jam_start_sore 0 //berguna untuk menentukan jam mulai sore berada pada array index 0
#define _menit_start_pagi 1 //berguna untuk menentukan menit mulai pagi berada pada array index 1
#define _menit_start_siang 1 //berguna untuk menentukan menit mulai siang berada pada array index 1
#define _menit_start_sore 1 //berguna untuk menentukan menit mulai sore berada pada array index 1
#define _detik_start_pagi 2 //berguna untuk menentukan detik mulai pagi berada pada array index 2
#define _detik_start_siang 2 //berguna untuk menentukan detik mulai siang berada pada array index 2
#define _detik_start_sore 2 //berguna untuk menentukan detik mulai sore berada pada array index 2
//End Define Timer Untuk Sistem Otomatis

//Define Timer Untuk Sistem Otomatis Waktu pagi Siang Dan Sore
#define _jam_stop_pagi 0 //berguna untuk menentukan jam stop pagi berada pada array index 0
#define _jam_stop_siang 0 //berguna untuk menentukan jam stop siang berada pada array index 0
#define _jam_stop_sore 0 //berguna untuk menentukan jam stop sore berada pada array index 0
#define _menit_stop_pagi 1 //berguna untuk menentukan menit stop pagi berada pada array index 1
#define _menit_stop_siang 1 //berguna untuk menentukan menit stop siang berada pada array index 1
#define _menit_stop_sore 1 //berguna untuk menentukan menit stop sore berada pada array index 1
#define _detik_stop_pagi 2 //berguna untuk menentukan detik stop pagi berada pada array index 2
#define _detik_stop_siang 2 //berguna untuk menentukan detik stop siang berada pada array index 2
#define _detik_stop_sore 2 //berguna untuk menentukan detik stop sore berada pada array index 2
//End Define Timer Untuk Sistem Otomatis

//Define Timer Untuk Sistem Otomatis Waktu pagi Siang Dan Sore
#define _jam_stop_siang 0 //berguna untuk menentukan jam stop siang berada pada array index 0
#define _jam_stop_sore 0 //berguna untuk menentukan jam stop sore berada pada array index 0
#define _menit_stop_pagi 1 //berguna untuk menentukan menit stop pagi berada pada array index 1
#define _menit_stop_siang 1 //berguna untuk menentukan menit stop siang berada pada array index 1
#define _menit_stop_sore 1 //berguna untuk menentukan menit stop sore berada pada array index 1
#define _detik_stop_pagi 2 //berguna untuk menentukan detik stop pagi berada pada array index 2
#define _detik_stop_siang 2 //berguna untuk menentukan detik stop siang berada pada array index 2
#define _detik_stop_sore 2 //berguna untuk menentukan detik stop sore berada pada array index 2
//End Define Timer Untuk Sistem Otomatis

#define _selected_relay 0 //Relay yang dipilih agar ketika timer sesuai maka relay akan menyala
#define sensor D3 //Define Sensor DHT
dht DHT; //Initialize DHT
const int SoilSensor = A0; //Define Soil Sensor berada pada pin A0

FirebaseData fbdo; //Declare FirebaseData object to fbdo
FirebaseAuth auth; //Declare FirebaseAuth object to auth
FirebaseConfig config; //Declare FirebaseConfig object to config

const long utcOffsetSecond = 3600 * 7; //Declare offset time in seconds to JAKARTA
WiFiUDP ntpUDP; //Declare an object ntpUDP
NTPClient WaktuClient(ntpUDP, "id.pool.ntp.org", utcOffsetSecond); //Declare an NTPClient to WaktuClient
LiquidCrystal_I2C lcd(0x27, 16, 2); //Initialize LCD

int val[2]; //Set Array for Relay State
int selected_relay[1]; //Set Array for Selected Relay, 0 = Relay 1, 1 = Relay 2
int start_pagi[3]; //Set Array for Start Time Pagi, Karena Jumlah Array 3
int start_siang[3]; //Set Array for Start Time Siang, Karena Jumlah Array 3
int start_sore[3]; //Set Array for Start Time Sore, Karena Jumlah Array 3
int stop_sore[3]; //Set Array for Stop Time Sore, Karena Jumlah Array 3
int stop_siang[3]; //Set Array for Stop Time Siang, Karena Jumlah Array 3
int stop_pagi[3]; //Set Array for Stop Time Pagi, Karena Jumlah Array 3

void setup() {
  Serial.begin(9600); //initiale Serial Baudrate to 9600
  lcd.begin(); //Initialize LCD
  lcd.setCursor(1, 0); //Set cursor to 1,0, for 16x2 LCD
  lcd.print("SELAMAT DATANG"); //Print "SELAMAT DATANG"
  lcd.setCursor(1, 1); //Set cursor to 1,1, for 16x2 LCD
  lcd.print("DI AUTO FARM"); //Print "DI AUTO FARM"
  delay(5000); //Delay 5 detik
  lcd.clear(); //Clear LCD
  WiFi.mode(WIFI_STA); //Set WiFi Mode to WIFI_STA
  WiFiManager wm; //Initialize WiFiManager
  lcd.setCursor(0, 0); //Set cursor to 0,0, for 16x2 LCD
  lcd.print("Connecting Wifi.."); //Print "Connecting Wifi.."
  lcd.clear(); //Clear LCD
  lcd.setCursor(0, 0); //Set cursor to 0,0, for 16x2 LCD
  lcd.print("WiFi SIPENYO"); //Print "WiFi SIPENYO"
  lcd.setCursor(0, 1); //Set cursor to 0,1, for 16x2 LCD
  lcd.print("Pass:12345678"); //Print "Pass:12345678"
  bool res; //Declare boolean, Untuk menampung WiFi Manager
  res = wm.autoConnect("SIPENYO", "12345678"); //AutoConnect to "SIPENYO" and Password "12345678"

  
  if (res) { //Jika res = true
    lcd.clear(); //Clear LCD
    lcd.setCursor(0, 0); //Set cursor to 0,0, for 16x2 LCD
    lcd.print("WiFi Terhubung"); //Print "WiFi Terhubung"
  }
  delay(2000); //Delay 2 detik

  config.database_url = DATABASE_URL; //initialize Firebase config
  config.signer.tokens.legacy_token = DATABASE_SECRET; //initialize Firebase config

  Firebase.reconnectWiFi(true); //set reconnectWiFi to true

  Firebase.begin(&config, &auth); //Initialize Firebase
  pinMode(Relay1, OUTPUT); //Set Relay1 to OUTPUT
  pinMode(Relay2, OUTPUT); //Set Relay2 to OUTPUT
  WaktuClient.begin(); //Initialize WaktuClient agar waktu bisa berjalan
}

//Start Void Loop
void loop() {
  WaktuClient.update(); //Update WaktuClient
  DHT.read11(sensor); //Read DHT11
  const char delimiter = ':'; //Set delimiter to ":" unutuk fungsi splitStringToInt
  int tokens[3]; //Set Array for Token
  int tokenCount; //Set Token Count

  Firebase.RTDB.getInt(&fbdo, "/relay/channel1/state"); //Get Firebase Data from Firebase Realtime Database, /relay/channel1/state
  val[_ch1] = fbdo.intData(); //Set val[_ch1] to fbdo.intData(), mengisi variabel val[2] diatas
  Firebase.RTDB.getInt(&fbdo, "/relay/channel2/state"); //Get Firebase Data from Firebase Realtime Database, /relay/channel2/state
  val[_ch2] = fbdo.intData(); //Set val[_ch2] to fbdo.intData(), mengisi variabel val[2] diatas

  Firebase.RTDB.getString(&fbdo, "/timer/pagi/start_pagi"); //Get Firebase Data from Firebase Realtime Database, /timer/pagi/start_pagi
  splitStringToInt(fbdo.stringData(), delimiter, tokens, tokenCount); //fungsi Split String to Int, berguna untuk mengelompokkan string berdasarkan delimiter

  for (int i = 0; i < tokenCount; i++) { //Looping for tokenCount
    start_pagi[_jam_start_pagi] = tokens[0]; //Set start_pagi[_jam_start_pagi] to tokens[0]
    start_pagi[_menit_start_pagi] = tokens[1]; //Set start_pagi[_menit_start_pagi] to tokens[1]
    start_pagi[_detik_start_pagi] = tokens[2]; //Set start_pagi[_detik_start_pagi] to tokens[2]
  }

  Firebase.RTDB.getString(&fbdo, "/timer/pagi/stop_pagi"); //Get Firebase Data from Firebase Realtime Database, /timer/pagi/stop_pagi
  splitStringToInt(fbdo.stringData(), delimiter, tokens, tokenCount); //fungsi Split String to Int, berguna untuk mengelompokkan string berdasarkan delimiter

  for (int i = 0; i < tokenCount; i++) { //Looping for tokenCount
    stop_pagi[_jam_stop_pagi] = tokens[0]; //Set stop_pagi[_jam_stop_pagi] to tokens[0]
    stop_pagi[_menit_stop_pagi] = tokens[1]; //Set stop_pagi[_menit_stop_pagi] to tokens[1]
    stop_pagi[_detik_stop_pagi] = tokens[2]; //Set stop_pagi[_detik_stop_pagi] to tokens[2]
  }

  Firebase.RTDB.getString(&fbdo, "/timer/siang/start_siang"); //Get Firebase Data from Firebase Realtime Database, /timer/siang/start_siang
  splitStringToInt(fbdo.stringData(), delimiter, tokens, tokenCount); //fungsi Split String to Int, berguna untuk mengelompokkan string berdasarkan delimiter

  for (int i = 0; i < tokenCount; i++) { //Looping for tokenCount
    start_siang[_jam_start_siang] = tokens[0]; //Set start_siang[_jam_start_siang] to tokens[0]
    start_siang[_menit_start_siang] = tokens[1]; //Set start_siang[_menit_start_siang] to tokens[1]
    start_siang[_detik_start_siang] = tokens[2]; //Set start_siang[_detik_start_siang] to tokens[2]
  }

  Firebase.RTDB.getString(&fbdo, "/timer/siang/stop_siang"); //Get Firebase Data from Firebase Realtime Database, /timer/siang/stop_siang
  splitStringToInt(fbdo.stringData(), delimiter, tokens, tokenCount); //fungsi Split String to Int, berguna untuk mengelompokkan string berdasarkan delimiter

  for (int i = 0; i < tokenCount; i++) { //Looping for tokenCount
    stop_siang[_jam_stop_siang] = tokens[0]; //Set stop_siang[_jam_stop_siang] to tokens[0]
    stop_siang[_menit_stop_siang] = tokens[1]; //Set stop_siang[_menit_stop_siang] to tokens[1]
    stop_siang[_detik_stop_siang] = tokens[2]; //Set stop_siang[_detik_stop_siang] to tokens[2]
  }

  Firebase.RTDB.getString(&fbdo, "/timer/sore/start_sore"); //Get Firebase Data from Firebase Realtime Database, /timer/sore/start_sore
  splitStringToInt(fbdo.stringData(), delimiter, tokens, tokenCount); //fungsi Split String to Int, berguna untuk mengelompokkan string berdasarkan delimiter

  for (int i = 0; i < tokenCount; i++) { //Looping for tokenCount
    start_sore[_jam_start_sore] = tokens[0]; //Set start_sore[_jam_start_sore] to tokens[0]
    start_sore[_menit_start_sore] = tokens[1]; //Set start_sore[_menit_start_sore] to tokens[1]
    start_sore[_detik_start_sore] = tokens[2]; //Set start_sore[_detik_start_sore] to tokens[2]
  }

  Firebase.RTDB.getString(&fbdo, "/timer/sore/stop_sore"); //Get Firebase Data from Firebase Realtime Database, /timer/sore/stop_sore
  splitStringToInt(fbdo.stringData(), delimiter, tokens, tokenCount); //fungsi Split String to Int, berguna untuk mengelompokkan string berdasarkan delimiter

  for (int i = 0; i < tokenCount; i++) { //Looping for tokenCount
    stop_sore[_jam_stop_sore] = tokens[0]; //Set stop_sore[_jam_stop_sore] to tokens[0]
    stop_sore[_menit_stop_sore] = tokens[1]; //Set stop_sore[_menit_stop_sore] to tokens[1]
    stop_sore[_detik_stop_sore] = tokens[2]; //Set stop_sore[_detik_stop_sore] to tokens[2]
  }


  Firebase.RTDB.getInt(&fbdo, "/relay/selectedRelay/state"); //Get Firebase Data from Firebase Realtime Database, /relay/selectedRelay/state
  selected_relay[_selected_relay] = fbdo.intData(); //Set selected_relay[_selected_relay] to fbdo.intData()



  if (val[_ch1] > 0) { //jika variable val[_ch1] > 0 maka
    digitalWrite(Relay1, LOW); //relay 1 nyala
  } else { //jika tidak
    digitalWrite(Relay1, HIGH); //relay 1 mati
  }

  if (val[_ch2] > 0) { //jika variable val[_ch2] > 0
    digitalWrite(Relay2, LOW); //relay 2 nyala
  } else { //jika tidak
    digitalWrite(Relay2, HIGH); //relay 2 mati
  }

  float kelembabanTanah; //set variable kelembabanTanah
  int hasilPembacaan = analogRead(SoilSensor); //fungsi membaca sensor kelembaban tanah pada pin A0/Analog0
  kelembabanTanah = (100 - ((hasilPembacaan / 1023.00) * 100)); //rumus menghitung kelembaban tanah
  Firebase.RTDB.setInt(&fbdo, "/soilMoisture/val", kelembabanTanah); //Set Firebase Realtime Database, /soilMoisture/val


  if (WaktuClient.getHours() == start_pagi[_jam_start_pagi] && WaktuClient.getMinutes() == start_pagi[_menit_start_pagi] && WaktuClient.getSeconds() == start_pagi[_detik_start_pagi]) { //jika jam, menit, dan detik sesuai dengan jam, menit, dan detik pada start_pagi
    if (selected_relay[_selected_relay] > 0) { //jika selected_relay[_selected_relay] > 0 maka
      if (kelembabanTanah <= 50) { //jika kelembabanTanah <= 50 maka
        digitalWrite(Relay1, LOW); //relay 1 nyala
        Firebase.RTDB.setInt(&fbdo, "/relay/channel1/state", 1); //Set Firebase Realtime Database, /relay/channel1/state
        int stop_mili = (stop_pagi[_jam_stop_pagi] * 3600UL + stop_pagi[_menit_stop_pagi] * 60UL + stop_pagi[_menit_stop_pagi]) * 1000UL; //menghitung millisecond
        int start_mili = (start_pagi[_jam_start_pagi] * 3600UL + start_pagi[_menit_start_pagi] * 60UL + start_pagi[_menit_start_pagi]) * 1000UL; //menghitung millisecond
        int Delayed = stop_mili - start_mili;// menghitung delay 
        delay(Delayed); //delay sesuai variabe delayed
        digitalWrite(Relay1, HIGH); //relay 1 mati
        Firebase.RTDB.setInt(&fbdo, "/relay/channel1/state", 0); //Set Firebase Realtime Database, /relay/channel1/state
      }
    } else {
      if (kelembabanTanah <= 50) { //jika kelembabanTanah <= 50 maka
        digitalWrite(Relay2, LOW); //relay 2 nyala
        Firebase.RTDB.setInt(&fbdo, "/relay/channel2/state", 1); //Set Firebase Realtime Database, /relay/channel2/state
        int stop_mili = (stop_pagi[_jam_stop_pagi] * 3600UL + stop_pagi[_menit_stop_pagi] * 60UL + stop_pagi[_menit_stop_pagi]) * 1000UL; //menghitung millisecond
        int start_mili = (start_pagi[_jam_start_pagi] * 3600UL + start_pagi[_menit_start_pagi] * 60UL + start_pagi[_menit_start_pagi]) * 1000UL; //menghitung millisecond
        int Delayed = stop_mili - start_mili; //menghitung delay
        delay(Delayed); //delay sesuai variabe delayed
        digitalWrite(Relay2, HIGH); //relay 2 mati
        Firebase.RTDB.setInt(&fbdo, "/relay/channel2/state", 0); //Set Firebase Realtime Database, /relay/channel2/state
      }
    }
  }

  if (WaktuClient.getHours() == start_siang[_jam_start_siang] && WaktuClient.getMinutes() == start_siang[_menit_start_siang] && WaktuClient.getSeconds() == start_siang[_detik_start_siang]) { //cek jam, menit, detik apakah sama dengan data di firebas
    if (selected_relay[_selected_relay] > 0) { //jika relay yang dipilih > 0
      if (kelembabanTanah <= 50) { //jika value dari sensor kelembaban tanah kurang dari 50 maka
        digitalWrite(Relay1, LOW); //relay 1 nyala
        Firebase.RTDB.setInt(&fbdo, "/relay/channel1/state", 1); //set relay 1 state
        int stop_mili = (stop_pagi[_jam_stop_siang] * 3600UL + stop_siang[_menit_stop_siang] * 60UL + stop_siang[_menit_stop_siang]) * 1000UL; //convert jam, menit, detik to mili
        int start_mili = (start_siang[_jam_start_siang] * 3600UL + start_siang[_menit_start_siang] * 60UL + start_siang[_menit_start_siang]) * 1000UL; //convert jam, menit, detik to mili
        int Delayed = stop_mili - start_mili; //menghitung delay
        delay(Delayed);//delay sesuai variabel Delayed
        digitalWrite(Relay1, HIGH); //relay 1 mati
        Firebase.RTDB.setInt(&fbdo, "/relay/channel1/state", 0); //set relay 1 state
      }
    } else {
      if (kelembabanTanah <= 50) { //jika value dari sensor kelembaban tanah kurang dari 50 maka
        digitalWrite(Relay2, LOW); //relay 2 nyala
        Firebase.RTDB.setInt(&fbdo, "/relay/channel2/state", 1); //set relay 2 state
        int stop_mili = (stop_siang[_jam_stop_siang] * 3600UL + stop_siang[_menit_stop_siang] * 60UL + stop_siang[_menit_stop_siang]) * 1000UL; //convert jam, menit, detik to mili
        int start_mili = (start_siang[_jam_start_siang] * 3600UL + start_siang[_menit_start_siang] * 60UL + start_siang[_menit_start_siang]) * 1000UL; //convert jam, menit, detik to mili
        int Delayed = stop_mili - start_mili; //menghitung delay
        delay(Delayed); //delay sesuai variable delayed
        digitalWrite(Relay2, HIGH); //relay 2 mati
        Firebase.RTDB.setInt(&fbdo, "/relay/channel2/state", 0);
      }
    }
  }

  if (WaktuClient.getHours() == start_sore[_jam_start_sore] && WaktuClient.getMinutes() == start_sore[_menit_start_sore] && WaktuClient.getSeconds() == start_sore[_detik_start_sore]) { //cek jam, menit, detik apakah sama dengan data di firebas
    if (selected_relay[_selected_relay] > 0) { //jika value dari selected relay lebih dari 0 maka
      if (kelembabanTanah <= 50) { //jika value dari sensor kelembaban tanah kurang dari 50 maka
        digitalWrite(Relay1, LOW); //menyalakan relay 1
        Firebase.RTDB.setInt(&fbdo, "/relay/channel1/state", 1); //send relay state to firebase, relay 1
        int stop_mili = (stop_sore[_jam_stop_sore] * 3600UL + stop_sore[_menit_stop_sore] * 60UL + stop_sore[_menit_stop_sore]) * 1000UL; //convert jam,menit,detik ke mili
        int start_mili = (start_sore[_jam_start_sore] * 3600UL + start_sore[_menit_start_sore] * 60UL + start_sore[_menit_start_sore]) * 1000UL; //convert jam,menit,detik ke mili
        int Delayed = stop_mili - start_mili; //menghitung delay
        delay(Delayed); //delay sesuai valriable delayed
        digitalWrite(Relay1, HIGH); //mematikan relay 1
        Firebase.RTDB.setInt(&fbdo, "/relay/channel1/state", 0); //send relay state to firebase, relay 1
      }
    } else {
      if (kelembabanTanah <= 50) { //jika value dari sensor kelembaban tanah kurang dari 50 maka
        digitalWrite(Relay2, LOW); //menyalakan relay 2
        Firebase.RTDB.setInt(&fbdo, "/relay/channel2/state", 1); //send relay state to firebase, relay 2
        int stop_mili = (stop_sore[_jam_stop_sore] * 3600UL + stop_sore[_menit_stop_sore] * 60UL + stop_sore[_menit_stop_sore]) * 1000UL; //convert jam,menit,detik ke mili
        int start_mili = (start_sore[_jam_start_sore] * 3600UL + start_sore[_menit_start_sore] * 60UL + start_sore[_menit_start_sore]) * 1000UL; //convert jam,menit,detik ke mili
        int Delayed = stop_mili - start_mili;
        delay(Delayed); //delay hasil dari variabel stop_mili di kurangi variable start_mili
        digitalWrite(Relay2, HIGH); //mematikan relay 2
        Firebase.RTDB.setInt(&fbdo, "/relay/channel2/state", 0); //send relay state to firebase, relay 2
      }
    }
  }

  Firebase.RTDB.setInt(&fbdo, "/dht/temperature/val", DHT.temperature); //send temperature to firebase from DHT
  Firebase.RTDB.setInt(&fbdo, "/dht/humidity/val", DHT.humidity); //send humidity to firebase from DHT
 Serial.print(DHT.temperature);
}


// Function to split a string into an array of integers
void splitStringToInt(const String &input, char delimiter, int integers[], int &integerCount) {
  char charArray[input.length() + 1]; //menambahkan 1 karakter
  input.toCharArray(charArray, sizeof(charArray)); //convert string to char

  char *token = strtok(charArray, &delimiter); //memecah string berdasarkan delimiter
  integerCount = 0; //set integerCount = 0

  while (token != NULL) { //looping
    integers[integerCount++] = atoi(token); //convert token to integer

    token = strtok(NULL, &delimiter); //memecah string berdasarkan delimiter
  }
}

//Sydah Selesai 
