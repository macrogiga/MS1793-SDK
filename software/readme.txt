****MS1793EVB1.1 IO resources****
========SPI========
CSN: GPB8
MOSI:GPB5
MISO:GPB4
SCK: GPB3

IRQ: GPA12/GPA4(jumper)

-----led/huancaiGRB-----
GPA8

-----RGB led---------
R - GPA4
G - GPA11
B - GPA7

-----DHT11 temperature/humidity sensor
GPA7

----Buzzer---------
GPA7


1====OLED/DHT11 demo==========================
OLED display(H64 W128)  IIC(GPA9/10)

DHT11 GPA7

*under interupt running mode, IIC cannot use IRQ mode
*for OLED display and dht read, low power can only use SLEEP mode


============================================


****MS1793EVB1.0 IO resources****

========SPI========
CSN: GPB8
MOSI:GPB5
MISO:GPB4
SCK: GPB3

IRQ: GPA12

-----module led indicator-----
GPA8

======ADC=========
GPA3
GPA4
PGA7

======PWM=========
GPA9
GPA10
GPA11
GPA4
GPA7





****sample application****

1====UART数据透传应用==============
TX:GPA9  
RX:GPA10
RTS:GPA11
CTS:GPD2


2====LED-V2BJF======================
GPA11  LED_G
GPA4   LED_R 
GPA7   LED_B


3====ARGUN==================================
2分钟无连接进入休眠状态
PA0 wakeup mcu from standby with rising edge


4====HID 自拍器==============================
2分钟无连接进入休眠状态
使用手机原生蓝牙进行连接,不需要app
PA0 按键自拍


5===ADC demo==================================
2分钟无连接进入休眠状态
配合nRF toolbox app里面的HRM demo
连接后调节PA3口对应的可变电阻器


6===OTA demo==========================

7====LED-V2BJF-WS28111=====================
GPA8  OUT to WS2811(1.7us)


8====AirSync=============================
微信硬件接入


============================================
