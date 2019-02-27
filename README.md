# DigitalVideoSheild
Digital Video Sheild for Arduino. The board enables Arduino to connect HDMI/DVI displays.

## 概要 
  * SPIからDVI出力(HDMIコネクタ)するArduinoシールドです。  
  * FTDI製ビデオエンジンBT816とDVIトランスミッタTFP410を搭載しています。
  
## 特徴 
  * Arduinoから任意の文字列や図形、画像をDVI/HDMIディスプレイに描画することができます。  
  * 1MBのバッファRAMを搭載しているため、滑らかに表示の切り替えが可能です。
  * バッファIC搭載しているため、5V耐圧でArduinoUNOにも対応しています。  
  * 最大解像度は800x600です。  
  * 128MbitのNOR Flash W25Q128を搭載しており、フォントや画像といった任意のデータを保存可能です。  
  * microSDスロットを搭載しています。  
  
## ピン配列

| Port | Arduino | 
|:-----------|:------------|
| BT816 CS | D7pin |
| BT816 SCK | D13pin |
| BT816 MOSI  | D11pin |
| BT816 MISO | D12pin |
| BT816 PD  | D3pin |
| BT816 INT | D4pin |
| microSD CS | D5pin |
| microSD SCK | D13pin |※BT816と共通
| microSD MOSI | D11pin |※BT816と共通
| microSD MISO | D12pin |※BT816と共通

※SPIはmicroSDとBT816で共通

## 表示例
　サンプルデモコード(ビデオエンジンBT816の内蔵アニメーション)を800x600のHDMIディスプレイに表示した例  
 
 <img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/img3.JPG" width="360">

## 注意点
  * Arduinoピンヘッダは別売です。    
  * 映像出力コネクタはHDMIコネクタですが、出力はDVI出力となります。
  * HDCP、HDMIコネクタを介した音声出力、ハイビジョン出力には対応していません。  
  * 全てのHDMI/DVI端子を有するディスプレイでの表示は保証できません。  
  
機能詳細については[BT816][1]を参照ください。  


<img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/img1.jpg" width="360">

<img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/img2.jpg" width="360">

[1]: https://www.ftdichip.com/Products/ICs/BT815_6.html "*1"

MIT Lisense
