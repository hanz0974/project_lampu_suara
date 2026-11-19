# Lampu Tepuk

3 lampu dikontrol pake jumlah tepukan. Arduino Uno + sensor suara + 3 relay.

Tepuk beberapa kali beruntun, terus diam ±1 detik. Jumlah tepukannya nentuin perintah:

| Tepukan | Hasil |
|---------|-------|
| 1x      | cuma lampu 1 nyala |
| 2x      | cuma lampu 2 nyala |
| 3x      | cuma lampu 3 nyala |
| 4x      | semua lampu nyala |
| 5x      | semua lampu mati |
| >5x     | diabaikan |

## Komponen
- Arduino Uno / Nano
- Modul sensor suara (KY-038 / LM393), yang ada pin **DO**
- Modul relay 4 channel 5V (atau 3 modul relay 1 channel)
- Kabel jumper

## Wiring

| Modul        | Pin modul | Arduino |
|--------------|-----------|---------|
| Sensor suara | VCC       | 5V      |
| Sensor suara | GND       | GND     |
| Sensor suara | DO        | D2      |
| Relay        | VCC       | 5V      |
| Relay        | GND       | GND     |
| Relay        | IN1       | D7      |
| Relay        | IN2       | D8      |
| Relay        | IN3       | D9      |

Tiap lampu AC disambung lewat terminal **COM** + **NO** relay-nya masing-masing (fasa ke COM, NO ke lampu, netral langsung ke lampu). Fasa dari colokan boleh dicabangin ke COM ketiga relay.

> ⚠️ Listrik 220V bahaya. Cabut colokan pas ngerakit, dan tutup terminal relay biar nggak kesentuh. Testing pake LED dulu aja.

## Upload

```bash
# Cek port board
arduino-cli board list

# Compile + upload (ganti port-nya sesuai hasil board list)
arduino-cli compile --fqbn arduino:avr:uno lampu_suara
arduino-cli upload  --fqbn arduino:avr:uno -p /dev/ttyACM0 lampu_suara

# Liat log
arduino-cli monitor -p /dev/ttyACM0 -c baudrate=9600
```

Kalau pake Nano clone: `--fqbn arduino:avr:nano:cpu=atmega328old`.
Kalau upload ditolak (permission denied): `sudo usermod -aG dialout $USER`, terus logout-login.

## Kalibrasi sensor
1. Buka serial monitor.
2. Puter **potensiometer** (sekrup biru) di modul sensor sampe LED DO-nya **mati pas hening** dan **nyala pas ditepuk**.
3. Tepuk sekali — harusnya muncul `Tepukan ke-1`, `Total tepukan: 1`, `>>> Lampu: 1=NYALA 2=mati 3=mati`.

## Setting di sketch
| Konstanta           | Default | Keterangan |
|---------------------|---------|------------|
| `SENSOR_AKTIF_HIGH` | true    | Ganti `false` kalau sensor lu kebalik (DO LOW pas ada suara) |
| `RELAY_AKTIF_LOW`   | true    | Ganti `false` kalau pake LED langsung / relay aktif HIGH |
| `PIN_RELAY`         | {7, 8, 9} | Pin relay; tambah/kurangin isinya buat ubah jumlah lampu |
| `JEDA_MIN_TEPUK`    | 200 ms  | Suara lebih rapat dari ini dianggap gema |
| `WAKTU_SELESAI`     | 1000 ms | Lama diam abis tepukan terakhir sebelum perintah dijalankan |

## Troubleshooting
- **Lampu kebalik (nyala semua pas Arduino baru nyala)** → ganti `RELAY_AKTIF_LOW`.
- **Ke-trigger terus / random** → sensor terlalu sensitif, puter potensio ke arah sebaliknya.
- **Tepuk nggak kedeteksi** → naikin sensitivitas, atau cek `SENSOR_AKTIF_HIGH`.
- **Satu tepuk kebaca dua** → naikin `JEDA_MIN_TEPUK` (misal 250–300).
- **Hitungan kepotong pas tepuk pelan-pelan** → naikin `WAKTU_SELESAI` (misal 900–1000).
