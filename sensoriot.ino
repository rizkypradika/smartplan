#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h> // For ESP8266
#include <DHT.h>

// Inisialisasi objek LCD dengan dimensi 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Firebase credentials
#define FIREBASE_HOST "https://deteksikelembabandansuhu-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "zRWgHIYrsMFk1xy38ztSu7DYlJoLTIeeNFlpErFn"
#define WIFI_SSID "SKK-STUDENT"
#define WIFI_PASSWORD "sistemkomputerkontrol"

// Definisikan pin data DHT22 dan tipe sensor
#define DHTPIN D5
#define DHTTYPE DHT22

// Inisialisasi sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Definisikan pin sensor kelembaban tanah
#define SOIL_MOISTURE_PIN A0

// Nilai kalibrasi (sesuaikan setelah melakukan kalibrasi)
#define DRY_VALUE 0 // Nilai saat tanah kering
#define WET_VALUE 1023 // Nilai saat tanah basah

// Inisialisasi objek FirebaseData
FirebaseData firebaseData;

void setup() {
  // Inisialisasi komunikasi I2C pada pin D6 (SDA) dan D7 (SCL)
  Wire.begin(D1, D2);

  lcd.begin();
  lcd.backlight(); // Nyalakan backlight

  dht.begin(); // Inisialisasi sensor DHT

  lcd.setCursor(0, 0); // Set kursor di kolom 0, baris 0
  lcd.print("Halo, Wemos!");
  delay(2000); // Tampilkan pesan awal selama 2 detik
  lcd.clear(); // Bersihkan layar

  // Hubungkan ke Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Inisialisasi Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Baca data sensor DHT (kelembaban udara dan suhu)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Cek jika pembacaan data dari DHT berhasil
  if (isnan(h) || isnan(t)) {
    lcd.setCursor(0, 0);
    lcd.print("Failed to read");
    lcd.setCursor(0, 1);
    lcd.print("from DHT sensor!");
    delay(2000);
    return;
  }

  // Baca data sensor kelembaban tanah
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

  // Konversi nilai analog ke persentase (menggunakan nilai kalibrasi)
  int soilMoisturePercent = map(soilMoistureValue, DRY_VALUE, WET_VALUE, 0, 100);

  // Pastikan persentase tetap dalam rentang 0-100%
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

  // Tampilkan data kelembaban tanah pada LCD
  lcd.setCursor(0, 0);
  lcd.print("Kelembaban T: ");
  lcd.print(soilMoisturePercent);
  lcd.print(" %");

  // Tampilkan data kelembaban udara pada LCD
  lcd.setCursor(0, 1);
  lcd.print("Kelembaban U: ");
  lcd.print(h);
  lcd.print(" %");

  // Tampilkan data suhu pada LCD
  lcd.setCursor(0, 2);
  lcd.print("Suhu: ");
  lcd.print(t);
  lcd.print(" C");

  delay(2000); // Tunggu 2 detik sebelum pembacaan berikutnya

  // Push data ke Firebase
  if (Firebase.pushString(firebaseData, "/DHT22/Humidity", String(h))) {
    Serial.println("Humidity pushed successfully");
  } else {
    Serial.println("Failed to push humidity: " + firebaseData.errorReason());
  }

  if (Firebase.pushString(firebaseData, "/DHT22/Temperature", String(t))) {
    Serial.println("Temperature pushed successfully");
  } else {
    Serial.println("Failed to push temperature: " + firebaseData.errorReason());
  }
}
