****MS1793 IO resources****

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
PGA11
GPA4
PGA7





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

==============================================
