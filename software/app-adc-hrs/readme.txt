ADC demo
==========================

demo App: nRF ToolBox  
          HRM

ADC: 12bit, range 0 - 4095
HRM data: 0 - 511
Batt ind: 0 - 100

===========================
ADC: PA3


I2C demo(master)
===========================
I2C_SCL:PA9
I2C_SDA:PA10
查询方式请使能宏定义USE_I2C
中端方式请使能宏定义USE_I2C和USE_I2CIRQ

请根据需要在函数IIC_Init()中修改slave设备地址


*如果作为I2C slave, 需要增加一个io口通知主设备进行数据的读取
