/*
    Copyright (c) 2017 Macrogiga Electronics Co., Ltd.

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
*/

#ifndef _MG_API_H_
#define _MG_API_H_

//TX power
#define TXPWR_0DBM 0x43
#define TXPWR_3DBM 0x48


///////////////////////////lib provided APIs//////////////////////////////////////

//Function: radio_initBle
//Parameters: txpwr - input, txpower;   addr - output, BLE device address
//return: None
void radio_initBle(unsigned char txpwr, unsigned char** addr/*out*/);  //init ble mode, should be called first after spi initialization

//Function: radio_standby
//this function is to set rf to standby mode, I ~ 3uA
//Parameters: none
//return: None
void radio_standby(void);

//Function: radio_setXtal
//this function is to config the params of xtal
//Parameters: xoib:0~f, xocc:0
//return: None
void radio_setXtal(unsigned char xoib, unsigned char xocc);

//Function: ble_run
//Parameters: interv_adv - advertise packet interval, unit 0.625ms
//return: None
//Remarks: never reurn!!!
void ble_run(unsigned short interv_adv);

//Function: ble_set_adv_data
//Parameters: adv - advertise packet payload; len - payload len
//return: None
void ble_set_adv_data(unsigned char* adv, unsigned char len);

//Function: ble_set_adv_rsp_data
//Parameters: rsp - advertise response payload; len - payload len
//return: None
void ble_set_adv_rsp_data(unsigned char* rsp, unsigned char len);

//Function: ble_set_name
//this function IS available when using default scan response data
//Parameters: name - device name; len - name len
//return: None
void ble_set_name(unsigned char* name,unsigned char len);

//Function: ble_set_adv_type
//Parameters: type - advertisement type, 0-adv_ind, 2-adv_nonconn_ind. default 0
//return: None
void ble_set_adv_type(unsigned char type);

//Function: ble_set_interval
//Parameters: interval - advertisement interval, unit 0.625ms
//return: None
void ble_set_interval(unsigned short interval);

//Function: ble_set_adv_enableFlag
//this function is to enable/disable ble adv
//Parameters: sEnableFlag - 0 to disable adv, 1 to enable adv
//return: None
void ble_set_adv_enableFlag(char sEnableFlag);

//Function: ble_disconnect
//this function is to disconnected the ble connection
//Parameters: none
//return: None
void ble_disconnect(void);

unsigned char* GetFirmwareInfo(void); //such as "FVxx.2.0.2mmx"
unsigned char* get_ble_version(void); //such as "MG_BLE_LIB_V1.0"
unsigned char GetRssiData(void); //only valid after receive a packet

void att_notFd(unsigned char pdu_type, unsigned char attOpcode, unsigned short attHd );
void att_server_rdByGrTypeRspDeviceInfo(unsigned char pdu_type);
void att_server_rdByGrTypeRspPrimaryService(unsigned char pdu_type, unsigned short start_hd, unsigned short end_hd, unsigned char*uuid, unsigned char uuidlen);
void att_server_rd( unsigned char pdu_type, unsigned char attOpcode, unsigned short att_hd, unsigned char* attValue, unsigned char datalen );

void ser_write_rsp_pkt(unsigned char pdu_type);

unsigned char sconn_notifydata(unsigned char* data, unsigned char len);//returns data size has been sent, ******* user's safe API *********

//API, returning of zero means error found
//follows parameters defined in LL_CONNECTION_PARAM_REQ
unsigned char sconn_UpdateConnParaReq(unsigned short IntervalMin, unsigned short IntervalMax, unsigned char  PreferredPeriodicity); /*unit of 1.25 ms*/
//simple api
unsigned char sconn_UpdateConnParaReqS(unsigned short PreferConInterval/*unit of 1.25 ms*/); //API, return non-zero means error found
unsigned short sconn_GetConnInterval(void);//get current used interval in the unit of 1.25ms

//Get current (or the latest) connected master device's MAC
//returns mac(6 Bytes, Little-Endian format) and the type(MacType, 0 means public type, others mean random type)
unsigned char* GetMasterDeviceMac(unsigned char* MacType);

///////////////////////////PAIR APIs/////////////////////////////////
void SetLePinCode(unsigned char *PinCode/*6 0~9 digitals*/);

//Get current connected device's long term KEY's info(EDIV)
//returns u8* EDivData /*2 Bytes*/ (encrypted)
//        newFlag: 1 means new paired device's info, 0 means old paired device's info.
//Remarks: 
//  1. This function shall be invoked when [StartEncryption == 1].
//  2. This function is ONLY supported in pairing cases.
unsigned char* GetLTKInfo(unsigned char* newFlag);


///////////////////////////LED application APIs/////////////////////////////////
void Led_getInfo(unsigned char* data);
void SetLEDLum(int r, int g, int b, int L); //rgb[0~255], L[0~100,101] 101 means not used 
void UpdateLEDValueFading(unsigned char flag_fade); //1-fading, 0-now


///////////////////////////OTA APIs/////////////////////////////////
//return: 
// OTA_OK             0
// OTA_SN_ERROR       1
// OTA_CHECKSUM_ERROR 2
// OTA_FORMAT_ERROR   3
// OTA_UNKNOWN_ERROR  255
unsigned char OTA_Proc(unsigned char *data, unsigned short len);


///////////////////////////interrupt running mode APIs/////////////////////////////////
//Function: SetBleIntRunningMode
//this function SHOULD be invoked before init the ble
//Parameters: None
//return: None
void SetBleIntRunningMode(void);

//Function: ble_run_interrupt_start
//   this function SHOULD be invoked to start the interrupt running mode in the main routine.
//   ble_run(0) function SHOULD ONLY be invoked in the ble irq interrupt service subroutine.
//Parameters:  interv_adv - advertise packet interval, unit 0.625ms
//return: None
void ble_run_interrupt_start(unsigned short interv_adv);

//Function: ble_nMsRoutine
//this function SHOULD be invoked every 1ms tick, one can invoke this function inside the systick routine
//Parameters: None
//return: None
void ble_nMsRoutine(void);

//Function: ble_run_interrupt_McuCanSleep
//this function CAN be invoked at main task, one can invoke this function to detect the BLE status then goto MCU sleep.
//Before doing so, one SHOULD configure the BLE irq down wakeup pin's function enabled.  
//Parameters: None
//return: None-zero means MCU can enter into stop/sleep mode.
//example:
//void IrqMcuGotoSleepAndWakeup(void)
//{
//    if(ble_run_interrupt_McuCanSleep())
//    {
//        //to do MCU sleep and wakeup steps
//    }
//}
unsigned char ble_run_interrupt_McuCanSleep(void);


///////////////////////////test/debug APIs/////////////////////////////////
//Parameters: freq - input, 0~80, center frequency(2400+freq)MHz, txpwr - input, 0x20~0x4A, txpower
//return: None
void test_carrier(unsigned char freq, unsigned char txpwr);

void SetFixAdvChannel(unsigned char isFixCh37Flag);

void set_mg_ble_dbg_flag(unsigned char EnableFlag);

unsigned char* GetMgBleStateInfo(int* StateInfoSize/*Output*/);


///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////app implemented/porting functions//////////////////////////////
//unsigned int GetSysTickCount(void);
//void DisableEnvINT(void);
//void EnableEnvINT(void);
//void SysClk48to8(void);
//void SysClk8to48(void);
//char IsIrqEnabled(void);
//void SPI_CS_Enable_(void);
//void SPI_CS_Disable_(void);
//unsigned char SPI_WriteRead(unsigned char SendData,unsigned char WriteFlag);
//unsigned char SPI_WriteBuf(unsigned char reg, unsigned char const *pBuf, unsigned char len);//start from lib version 2.2.0
//unsigned char SPI_ReadBuf(unsigned char reg, unsigned char *pBuf, unsigned char len);//start from lib version 2.2.0
    
//void gatt_user_send_notify_data_callback(void);
//void ser_prepare_write(unsigned short handle, unsigned char* attValue, unsigned short attValueLen, unsigned short att_offset);
//void ser_execute_write(void);
//void att_server_rdByGrType( unsigned char pdu_type, unsigned char attOpcode, unsigned short st_hd, unsigned short end_hd, unsigned short att_type );
//void ser_write_rsp(unsigned char pdu_type/*reserved*/, unsigned char attOpcode/*reserved*/, 
//                   unsigned short att_hd, unsigned char* attValue/*app data pointer*/, unsigned char valueLen_w/*app data size*/);
//void server_rd_rsp(unsigned char attOpcode, unsigned short attHandle, unsigned char pdu_type);
//int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
//                            unsigned short uuid16,   
//                            unsigned short* hd_start_r,unsigned short* hd_end_r);

//void ConnectStausUpdate(unsigned char IsConnectedFlag);
//unsigned char* getDeviceInfoData(unsigned char* len);
//void UsrProcCallback(void);

//void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP,u16 offset);
//unsigned char aes_encrypt_HW(unsigned char *painText128bitBE,unsigned char *key128bitBE); //porting api, returns zero means not supported

//void UpdateLEDValueAll(void);

//void OtaSystemReboot(void);
//void WriteFlashE2PROM(u8* data, u16 len, u32 pos, u8 flag); //4 bytes aligned
//u32 GetOtaAddr(void);


//void McuGotoSleepAndWakeup(void);


#endif
