# DengoPStoSwitch
 電車でGO!PS用マスコンを電車でGO!はしろう山手線(Switch)で使うための方法とプログラムです

 Raspberry Pi Picoを使ってSwitch用のコントローラーになりすまし、Raspberry Pi Picoでマスコンの状態を読み取ることで可能にします。

# 必要なパーツ

- Raspberry Pi Pico RP2040

# ビルド済みファームウェア
[こちら](https://github.com/kusaanko/DengoPStoSwitch/releases/latest)

# ピン配置
以下のように接続してください。プルアップ抵抗は不要です。

- PS pin 1 > Raspberry Pi Pico pin 11
- PS pin 2 > Raspberry Pi Pico pin 10
- PS pin 4 > Raspberry Pi Pico GND(どのピンでも可)
- PS pin 5 > Raspberry Pi Pico 3.3V
- PS pin 6 > Raspberry Pi Pico pin 5
- PS pin 7 > Raspberry Pi Pico pin 4

# ゲーム設定
オプションより、専用コントローラー設定をオンにしてください。これにより、2ハンドルタイプでも操作できるようになります。また、そのままだとブレーキのノッチ数が合いません。

# 操作方法
SELECTボタンの状態によりSTART,A,B,Cの機能が変わるというシステムになっています。

SELECT1回押し - +  
Raspberry Pi Pico上のボタン - ホーム

## SELECTを押していない状態
START - X  
A - Y  
B - B  
C - A  

## SELECTを押した状態
START - 上  
A - 左  
B - 下  
C - 右  

# ビルド手順

```bash
git clone https://github.com/kusaanko/DengoPStoSwitch
cd DengoPStoSwitch
sudo ./setup.sh
./setup_cmake.sh
cd build
make
```
完成したファイルは`build/dengo2switch.uf2`です。

Linuxでビルドしてください。WSLでも正常に動作します。

# ピン配置を変える
ファームウェアのビルドが必要になります。

ps2.h
```c
#define PS2_PIN_DATA 11
#define PS2_PIN_COMMAND 10
#define PS2_PIN_ATTENTION 5
#define PS2_PIN_CLOCK 4
```
ここの数字をRaspberry Pi Picoのピン番号に変更してください。
1番ピンがDATA、2番ピンがCOMMAND、6番ピンがATTENTION、7番ピンがCLOCKです。

# 参考
https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_hid_composite
https://gist.github.com/jwiki95/86dcf36103ce799b3e262ed7b4245da6
https://github.com/celclow/SwitchControlLibrary/blob/master/src/SwitchControlLibrary.h
https://github.com/madsci1016/Arduino-PS2X/blob/master/PS2X_lib/PS2X_lib.h
https://store.curiousinventor.com/guides/PS2