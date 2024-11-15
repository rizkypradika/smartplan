#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi objek LCD dengan dimensi 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Definisikan pin sensor kelembaban tanah
#define SOIL_MOISTURE_PIN A0

// Nilai kalibrasi (sesuaikan setelah melakukan kalibrasi)
#define DRY_VALUE 0      // Nilai saat tanah kering
#define WET_VALUE 1023   // Nilai saat tanah basah

void setup() {
  // Inisialisasi komunikasi I2C pada pin D1 (SDA) dan D2 (SCL)
  Wire.begin(D1, D2);

  lcd.begin();           // Inisialisasi LCD
  lcd.backlight();       // Nyalakan backlight

  // Inisialisasi komunikasi Serial
  Serial.begin(9600);    // Atur baud rate ke 9600 bps

  lcd.setCursor(0, 0);   // Set kursor di kolom 0, baris 0
  lcd.print("Halo, Wemos!");
  delay(2000);           // Tampilkan pesan awal selama 2 detik
  lcd.clear();           // Bersihkan layar
}

void loop() {
  // Baca data sensor kelembaban tanah
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

  // Konversi nilai analog ke persentase (menggunakan nilai kalibrasi)
  int soilMoisturePercent = map(soilMoistureValue, DRY_VALUE, WET_VALUE, 0, 100);

  // Pastikan persentase tetap dalam rentang 0-100%
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);


  // Tampilkan data pada Serial Monitor
  Serial.print("Kelembaban Tanah: ");
  Serial.print(soilMoisturePercent);
  Serial.println(" %");

  delay(1000); // Tunggu 1 detik sebelum membaca lagi
}
