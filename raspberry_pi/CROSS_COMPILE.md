\# Raspberry Pi için ARM64 çapraz derleme



Program, Windows üzerindeki WSL Ubuntu ortamında derlenmiş,

Raspberry Pi 4 üzerinde çalıştırılarak doğrulanmıştır.



Kullanılan çapraz derleyici: aarch64-linux-gnu-gcc 15.2.0

Hedef sistem: Raspberry Pi OS 64-bit, libgpiod 2.2.1



Aşağıdaki IP adresi ve kullanıcı adı hedef Raspberry Pi'ye göre

değiştirilmelidir. Raspberry Pi ile laptop aynı ağda olmalıdır.



\## 1. Ubuntu üzerinde araçların kurulması



```bash

sudo apt update

sudo apt install -y gcc-aarch64-linux-gnu rsync file openssh-client

```



\## 2. Kaynak kodunun alınması



Aşağıdaki işlemler laptopun Ubuntu terminalinde yapılır.



```bash

mkdir -p \~/rpi-cross

cd \~/rpi-cross

scp -r kaan@192.168.1.108:/home/kaan/lab1\_gpio .

```



\## 3. Hedef sistem dosyalarının alınması



Raspberry Pi'de libgpiod-dev, build-essential ve rsync kurulu olmalıdır.

Bu adımlar yeni bir sysroot klasörü için uygulanır.



```bash

mkdir -p sysroot/usr/include sysroot/usr/lib/aarch64-linux-gnu



rsync -az kaan@192.168.1.108:/usr/include/ sysroot/usr/include/



rsync -az kaan@192.168.1.108:/usr/lib/aarch64-linux-gnu/ \\

&#x20; sysroot/usr/lib/aarch64-linux-gnu/



ln -s usr/lib sysroot/lib



ln -s aarch64-linux-gnu/ld-linux-aarch64.so.1 \\

&#x20; sysroot/lib/ld-linux-aarch64.so.1

```



\## 4. Çapraz derleme



```bash

aarch64-linux-gnu-gcc \\

&#x20; --sysroot="$PWD/sysroot" \\

&#x20; -B"$PWD/sysroot/usr/lib/aarch64-linux-gnu/" \\

&#x20; -L"$PWD/sysroot/usr/lib/aarch64-linux-gnu/" \\

&#x20; -Wl,-rpath-link,"$PWD/sysroot/usr/lib/aarch64-linux-gnu/" \\

&#x20; -Wall -Wextra -O2 \\

&#x20; lab1\_gpio/led.c -o lab1\_gpio/led\_cross \\

&#x20; -lgpiod



file lab1\_gpio/led\_cross

```



Doğrulanan dosya türü: ELF 64-bit, ARM aarch64.



\## 5. Raspberry Pi üzerinde çalıştırma



Laptopun Ubuntu terminalinde:



```bash

scp lab1\_gpio/led\_cross kaan@192.168.1.108:/home/kaan/lab1\_gpio/

ssh kaan@192.168.1.108

```



SSH bağlantısından sonra Raspberry Pi terminalinde:



```bash

cd \~/lab1\_gpio

chmod +x led\_cross

./led\_cross | tee cross\_test.txt

```



LED beş kez yanıp sönmüş ve 100.000 periyot için 0,252068 saniye

ölçülmüştür. Yazılımla hesaplanan ortalama frekans 396.718,53 Hz'dir.



sysroot klasörü ve üretilen çalıştırılabilir dosyalar depoya dahil edilmez.

