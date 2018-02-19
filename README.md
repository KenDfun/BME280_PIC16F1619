# BME280_PIC16F1619
これはBoschの温度、湿度、気圧センサBME280をPIC16F1619でコントロールするプログラムです。マイコンボードはPIC16F1619が標準で搭載されているCuriosityボードを使用しています。Curiosityボード上からI2C信号を取り出してBME280からデータを取り出してUartでデータを出力しています。  
  
  ![bme280_pic](https://user-images.githubusercontent.com/2620156/36353638-33c4390a-150c-11e8-958b-d502b9601d54.gif)


## MCCの設定
Microchip Code Configurator(MCC)で基本設定を行っています。MCCはEUSARTとTimer1を使用していてMSSPのライブラリは使用していません。使用していない理由は使いにくいのと無駄にメモリを使用していたためです。

| RAM size | ROM size|
|:----|----------|
|229|4992|

### PIN設定
下図のMCC設定画面を参照してください。  
![pin asign](https://user-images.githubusercontent.com/2620156/36339272-06efcbc2-1405-11e8-9d09-4a7464d4cbc9.png)

## BME280_driver
このフォルダー以下にBoschのサイトからダウンロードしたAPIをおいています。基本は変更していませんが、bme280_defs.hがXC8ではコンパイルが通らなかったのでキャスト変換マクロを変更しています。
```
#if !defined(UINT8_C) && !defined(INT8_C)
#define INT8_C(x)       (int8_t)(x)
#define UINT8_C(x)      (uint8_t)(x)
#endif
```

## その他
すでにBME280_PICというレポジトリを作っていますがこれはPIC16F15344という新しいチップで作成したのですがメモリ容量が少なく86%もROMを消費してしまいました。このため作り直しました。
