/*
 * Lampu Tepuk — 3 lampu dikontrol pake jumlah tepukan
 *
 * Hardware:
 *   - Arduino Uno
 *   - Modul sensor suara (KY-038 / LM393), pake pin DO (digital)
 *   - 3 relay (modul relay 4 channel / 3 modul 1 channel)
 *
 * Wiring:
 *   Sensor +   -> 5V     Sensor G   -> GND     Sensor DO -> D2
 *   Relay VCC  -> 5V     Relay GND  -> GND
 *   Relay IN1  -> D7     Relay IN2  -> D8      Relay IN3 -> D9
 *
 * Cara kerja: tepuk beberapa kali beruntun, terus diam sebentar.
 * Jumlah tepukan nentuin perintahnya:
 *   1x -> cuma lampu 1 nyala
 *   2x -> cuma lampu 2 nyala
 *   3x -> cuma lampu 3 nyala
 *   4x -> semua lampu nyala
 *   5x -> semua lampu mati
 *   lebih dari 5x -> diabaikan
 */

// ===== Pin =====
const uint8_t PIN_SENSOR = 2;
const uint8_t PIN_RELAY[] = {7, 8, 9};
const uint8_t JUMLAH_LAMPU = sizeof(PIN_RELAY) / sizeof(PIN_RELAY[0]);
const uint8_t PIN_LED = LED_BUILTIN;  // indikator tepukan kedeteksi

// ===== Setting (sesuaiin sama modul lu) =====
const bool SENSOR_AKTIF_HIGH = true;  // KY-038 umumnya DO = HIGH pas ada suara. Kalau kebalik, ganti false
const bool RELAY_AKTIF_LOW   = true;  // kebanyakan modul relay aktif LOW. Kalau kebalik, ganti false

// ===== Timing (ms) =====
const unsigned long JEDA_MIN_TEPUK = 200;  // suara lebih rapat dari ini dianggap gema tepukan yang sama
const unsigned long WAKTU_SELESAI  = 1000;  // diam selama ini abis tepukan terakhir -> hitungan dieksekusi

uint8_t hitungTepuk = 0;
unsigned long waktuTepukTerakhir = 0;
bool sensorSebelumnya = false;

bool sensorAktif() {
  bool level = digitalRead(PIN_SENSOR) == HIGH;
  return SENSOR_AKTIF_HIGH ? level : !level;
}

void setRelay(uint8_t i, bool nyala) {
  bool levelPin = RELAY_AKTIF_LOW ? !nyala : nyala;
  digitalWrite(PIN_RELAY[i], levelPin ? HIGH : LOW);
}

// bit 0 = lampu 1, bit 1 = lampu 2, bit 2 = lampu 3
void setLampu(uint8_t pola) {
  for (uint8_t i = 0; i < JUMLAH_LAMPU; i++) {
    setRelay(i, pola & (1 << i));
  }
  Serial.print(F(">>> Lampu: "));
  for (uint8_t i = 0; i < JUMLAH_LAMPU; i++) {
    Serial.print(i + 1);
    Serial.print(pola & (1 << i) ? F("=NYALA ") : F("=mati "));
  }
  Serial.println();
}

void jalankanPerintah(uint8_t jumlah) {
  Serial.print(F("Total tepukan: "));
  Serial.println(jumlah);
  switch (jumlah) {
    case 1: setLampu(0b001); break;  // lampu 1 aja
    case 2: setLampu(0b010); break;  // lampu 2 aja
    case 3: setLampu(0b100); break;  // lampu 3 aja
    case 4: setLampu(0b111); break;  // semua nyala
    case 5: setLampu(0b000); break;  // semua mati
    default: Serial.println(F("Kebanyakan tepukan, diabaikan."));
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SENSOR, INPUT);
  pinMode(PIN_LED, OUTPUT);
  for (uint8_t i = 0; i < JUMLAH_LAMPU; i++) {
    pinMode(PIN_RELAY[i], OUTPUT);
    setRelay(i, false);  // pastiin semua lampu mati pas baru nyala
  }
  delay(500);  // kasih waktu sensor stabil
  Serial.println(F("Lampu tepuk siap. 1-3x = lampu 1-3, 4x = semua nyala, 5x = semua mati."));
}

void loop() {
  unsigned long sekarang = millis();
  bool aktif = sensorAktif();
  bool rising = aktif && !sensorSebelumnya;  // baru aja kedeteksi suara
  sensorSebelumnya = aktif;

  // Udah diam cukup lama -> eksekusi sesuai jumlah tepukan
  if (hitungTepuk > 0 && sekarang - waktuTepukTerakhir > WAKTU_SELESAI) {
    jalankanPerintah(hitungTepuk);
    hitungTepuk = 0;
  }

  if (!rising) return;
  if (hitungTepuk > 0 && sekarang - waktuTepukTerakhir < JEDA_MIN_TEPUK) return;  // gema

  hitungTepuk++;
  waktuTepukTerakhir = sekarang;
  Serial.print(F("Tepukan ke-"));
  Serial.println(hitungTepuk);

  digitalWrite(PIN_LED, HIGH);
  delay(20);
  digitalWrite(PIN_LED, LOW);
}
