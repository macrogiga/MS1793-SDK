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
中断方式请使能宏定义USE_I2C和USE_I2CIRQ

请根据需要在函数IIC_Init()中修改slave设备地址


*如果作为I2C slave, 需要增加一个io口通知主设备进行数据的读取



standby adv demo
===========================
上电发完成一次广播后进入standby,使用IWDG触发复位.
这个模式广播功耗最低,根据功耗需求来调整IWDG的时长.
使用备份寄存器来区分第一次上电


