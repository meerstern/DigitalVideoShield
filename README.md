# DigitalVideoSheild
Digital Video Sheild for Arduino. The board enables Arduino to connect HDMI/DVI displays.

## 概要 
  * SPIからDVI出力(HDMIコネクタ)するArduinoシールドです。  
  * FTDI製ビデオエンジンBT816とDVIトランスミッタTFP410を搭載しています。
  
## 特徴 
  * Arduinoから任意の文字列や図形、画像をDVI/HDMIディスプレイに描画することができます。  
  * 1MBのバッファRAMを搭載しているため、滑らかに表示の切り替えが可能です。
  * バッファIC搭載のSPI通信ポートは5V耐圧のため、ArduinoUNOにも対応しています。  
  * 電源はArduinoピンから5Vおよび3.3Vを使用します。  
  * 最大解像度は800x600です。  
  * PCLK最大60MHz、24bitフルカラー(RGB888)に対応しています。  
  * 128MbitのNOR Flash W25Q128を搭載しており、フォントや画像といった任意のデータを保存可能です。  
  * microSDスロットを搭載しています。  
  * BT816のGPIOやタッチパネルポート等のIOは基板上のテストポイントからアクセス可能です。  
  
  
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
※上記の信号ポートは5V耐圧  

## 表示例
　サンプルデモコード(ビデオエンジンBT816の内蔵アニメーション)を800x480のディスプレイに表示した例  
 
 <img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/img3.JPG" width="360">

　サンプルデモコード(ビデオエンジンBT816の内蔵アニメーション)を800x480出力でHDディスプレイに表示した例  
 
 <img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/IPS235.jpg" width="360">
 
 Co-Processorエンジンによる内蔵Widgetsを800x480のディスプレイに表示した例  
 
 <img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/img4.JPG" width="360">
 ※内蔵Widgetsはテキスト、ボタン、時計、プログレスバー、スライダー、トグルボタンなどが内蔵されています。  
   
   数行のC/C++コードの記述で任意の表示をすることができます。     
   ボタン、スライダー等の入力イベントを使用する場合は別途、抵抗膜方式のパネルと配線が必要です。  



## 注意点
  * Arduinoピンヘッダは別売です。    
  * 映像出力コネクタはHDMIコネクタですが、出力はDVI出力となります。  
  * HDCP機能、HDMIコネクタを介した音声出力、ハイビジョン出力には対応していません。  
  * 全てのHDMI/DVI端子を有するディスプレイでの表示は保証できません。  
  * 一般的なディスプレイの場合は低解像度入力で自動的に拡大表示されますが、ディスプレイによって正しく表示されない場合があります。  
  
機能詳細については[BT816][1]を参照ください。  

## 販売サイト
  * [こちらのサイト][2]で販売しております。

<img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/DVS_1.JPG" width="360">


<img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/DVS_2.JPG" width="360">


<img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/DVS_arduino.JPG" width="360">


<img src="https://github.com/meerstern/DigitalVideoSheild/blob/master/DVS_nucleo.JPG" width="360">


[1]: https://www.ftdichip.com/Products/ICs/BT815_6.html "*1"

[2]: https://www.kadenken.com/shopdetail/000000000884/ct150/page1/order/ "*2"


MIT Lisense
