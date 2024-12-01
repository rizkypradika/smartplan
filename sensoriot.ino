#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <FirebaseESP8266.h>  // Pastikan library Firebase ESP8266 yang benar digunakan

// Inisialisasi objek LCD dengan dimensi 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4); 

// Definisikan pin data DHT22 dan tipe sensor
#define DHTPIN D5     
#define DHTTYPE DHT22   

// Pin untuk sensor kelembaban tanah
#define SOIL_MOISTURE_PIN A0

// Nilai kalibrasi sensor kelembaban tanah
#define DRY_VALUE 0 // Nilai saat tanah kering (kalibrasi)
#define WET_VALUE 1023 // Nilai saat tanah basah (kalibrasi)

// Data Firebase
#define FIREBASE_HOST "https://deteksikelembabandansuhu-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "zRWgHIYrsMFk1xy38ztSu7DYlJoLTIeeNFlpErFn"
#define WIFI_SSID "KOST PUTRI BIDADARI 4"
#define WIFI_PASSWORD "12341234"

// Inisialisasi sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Inisialisasi Firebase
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Koneksi WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected, IP Address: ");
  Serial.println(WiFi.localIP());

  // Konfigurasi Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Mulai Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Inisialisasi komunikasi I2C pada pin D1 (SDA) dan D2 (SCL)
  Wire.begin(D1, D2);

  lcd.begin();        // Inisialisasi LCD
  lcd.backlight();    // Nyalakan backlight

  lcd.setCursor(0, 0); // Set kursor di kolom 0, baris 0
  lcd.print("Halo, Wemos!");
  delay(2000);        // Tampilkan pesan awal selama 2 detik
  lcd.clear();        // Bersihkan layar
}

void loop() {
  // Baca data dari sensor DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Baca data dari sensor kelembaban tanah (soil moisture)
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

  // Konversi nilai analog ke persentase (0-100%) menggunakan nilai kalibrasi
  int soilMoisturePercent = map(soilMoistureValue, DRY_VALUE, WET_VALUE, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100); // Batasi antara 0-100%

  // Debug output
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %  Temperature: ");
  Serial.print(t);
  Serial.print(" C  Soil Moisture: ");
  Serial.print(soilMoisturePercent);
  Serial.println(" %");

  // Cek jika pembacaan data DHT22 berhasil
  if (isnan(h) || isnan(t)) {
    lcd.setCursor(0, 0);
    lcd.print("Failed to read");
    lcd.print(" from DHT sensor!");
    delay(2000);
    return;
  }

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

  // Kirim data ke Firebase
  if (Firebase.setFloat(firebaseData, "/Ardianti/Humidity", h)) {
    Serial.println("Humidity data sent successfully");
  } else {
    Serial.print("Failed to send humidity data: ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/Ardianti/Temperature", t)) {
    Serial.println("Temperature data sent successfully");
  } else {
    Serial.print("Failed to send temperature data: ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setInt(firebaseData, "/Ardianti/SoilMoisture", soilMoisturePercent)) {
    Serial.println("Soil moisture data sent successfully");
  } else {
    Serial.print("Failed to send soil moisture data: ");
    Serial.println(firebaseData.errorReason());
  }
}
