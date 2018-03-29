/*
    Copyright (c) 2018 Macrogiga Electronics Co., Ltd.

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

#ifdef BLE_PAIR_SUPPORT

#define AES_HW_SUPPORT  /*AES Hardware implemntation support*/

#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"


u8 Password_wr[9] = {0}; //max 8 digit password
u8 LockFlag = 1;

extern void att_ErrorFd_eCode(u8 pdu_type, u8 attOpcode, u16 attHd , u8 errorCode);
extern  unsigned char StartEncryption;

u8 CanNotifyFlag = 0;

/// Characteristic Properties Bit
#define ATT_CHAR_PROP_RD                            0x02
#define ATT_CHAR_PROP_W_NORSP                       0x04
#define ATT_CHAR_PROP_W                             0x08
#define ATT_CHAR_PROP_NTF                           0x10
#define ATT_CHAR_PROP_IND                           0x20
#define GATT_PRIMARY_SERVICE_UUID                   0x2800

#define TYPE_CHAR      0x2803
#define TYPE_CFG       0x2902
#define TYPE_INFO      0x2901
#define TYPE_xRpRef    0x2907
#define TYPE_RpRef     0x2908
#define TYPE_INC       0x2802
#define UUID16_FORMAT  0xff


#define SOFTWARE_INFO "SV3.3.0"
#define MANU_INFO     "MacroGiga Bluetooth"
#define DeviceInfo    "MG-Lock"  /*max len is 24 bytes*/

u16 cur_notifyhandle = 0x14;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);

u8* getDeviceInfoData(u8* len)
{    
    *len = sizeof(DeviceInfo);
    return (u8*)DeviceInfo;
}

/**********************************************************************************
                 *****DataBase****

01 - 06  GAP (Primary service) 0x1800
  03:04  name
07 - 0f  Device Info (Primary service) 0x180a
  0a:0b  firmware version
  0e:0f  software version
  hid service (Primary service) ...
************************************************************************************/

typedef struct ble_character16{
    u16 type16;          //type2
    u16 handle_rec;      //handle
    u8  characterInfo[5];//property1 - handle2 - uuid2
    u8  uuid128_idx;     //0xff means uuid16,other is idx of uuid128
}BLE_CHAR;

typedef struct ble_UUID128{
    u8  uuid128[16];//uuid128 string: little endian
}BLE_UUID128;

//
///STEP0:Character declare
//
const BLE_CHAR AttCharList[] = {
// ======  gatt =====  Do NOT Change!!
    {TYPE_CHAR,0x0003, {ATT_CHAR_PROP_RD, 0x04,0, 0x00,0x2a}, UUID16_FORMAT},//name
    //05-06 reserved
// ======  device info =====    Do NOT Change if using the default!!!  
    {TYPE_CHAR,0x0008, {ATT_CHAR_PROP_RD, 0x09,0, 0x29,0x2a}, UUID16_FORMAT},//manufacture
    {TYPE_CHAR,0x000a, {ATT_CHAR_PROP_RD, 0x0b,0, 0x26,0x2a}, UUID16_FORMAT},//firmware version
    {TYPE_CHAR,0x000c, {ATT_CHAR_PROP_RD, 0x0d,0, 0x50,0x2a}, UUID16_FORMAT},//PnPID
    {TYPE_CHAR,0x000e, {ATT_CHAR_PROP_RD, 0x0f,0, 0x28,0x2a}, UUID16_FORMAT},//sw version

// ======  scan prameter service  ======
    {TYPE_CHAR,0x0011, {ATT_CHAR_PROP_W_NORSP,              0x12,0, 0xf1,0xff}, UUID16_FORMAT},//unlock command
    {TYPE_CHAR,0x0013, {ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF, 0x14,0, 0xf2,0xff}, UUID16_FORMAT},//lock state
    {TYPE_CFG, 0x15,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W}//cfg
};

const BLE_UUID128 AttUuid128List[] = {
    {0}, //empty
};

u8 GetCharListDim(void)
{
    return sizeof(AttCharList)/sizeof(AttCharList[0]);
}

//////////////////////////////////////////////////////////////////////////
///STEP1:Service declare
// read by type request handle, primary service declare implementation
void att_server_rdByGrType( u8 pdu_type, u8 attOpcode, u16 st_hd, u16 end_hd, u16 att_type )
{
 //!!!!!!!!  hard code for gap and gatt, make sure here is 100% matched with database:[AttCharList] !!!!!!!!!
    if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd == 1))//hard code for device info service
    {
        //att_server_rdByGrTypeRspDeviceInfo(pdu_type);//using the default device info service
        //GAP Device Name
        u8 t[] = {0x00,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x1,0x6,(u8*)(t),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x07)) //usr's service
    {
        //apply user defined (device info)service example
        u8 t[] = {0xa,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0xf,(u8*)(t),2);
        return;
    }
    
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x10)) //usr's service
    {
        u8 hid[2] = {0xf0,0xff};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x10,0x15,(u8*)(hid),2);
        return;
    }
    ///error handle
    att_ErrorFd_eCode(pdu_type, attOpcode, st_hd, 0x0A); //ATT_ERR_ATTR_NOT_FOUND
}

///STEP2:data coming
///write response, data coming....
void ser_write_rsp(u8 pdu_type/*reserved*/, u8 attOpcode/*reserved*/, 
                   u16 att_hd, u8* attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{
    switch(att_hd)
    {
        case 0x12://unlock
            if (StartEncryption)
            {
                if (valueLen_w < 9)
                {
                    Password_wr[0] = valueLen_w;
                    memcpy(&Password_wr[1], attValue, valueLen_w);
                }
            }else{
                Password_wr[0] = 0;
            }
        case 0x15://cfg
            ser_write_rsp_pkt(pdu_type); /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/
            break;
        
        default:
            att_ErrorFd_eCode(pdu_type, attOpcode, 0x0000, 0x06);//ATT_ERR_UNSUPPORTED_REQ
            break;
    }
 }

 ///STEP2.1:Queued Writes data if any
void ser_prepare_write(u16 handle, u8* attValue, u16 attValueLen, u16 att_offset)//user's call back api 
{
    //queued data:offset + data(size)
    //when ser_execute_write() is invoked, means end of queue write.
    
    //to do    
}
 
void ser_execute_write(void)//user's call back api 
{
    //end of queued writes  
    //to do...    
}

///STEP3:Read data
//// read response
void server_rd_rsp(u8 attOpcode, u16 attHandle, u8 pdu_type)
{
    u8  d_len;
    u8* ble_name = getDeviceInfoData(&d_len);
    
    switch(attHandle) //hard code
    {
        case 0x04: //GAP name
            att_server_rd( pdu_type, attOpcode, attHandle, ble_name, d_len);
            break;
        
        case 0x09: //MANU_INFO
            //att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(MANU_INFO), sizeof(MANU_INFO)-1);
            att_server_rd( pdu_type, attOpcode, attHandle, get_ble_version(), strlen((const char*)get_ble_version())); //ble lib build version
            break;
        
        case 0x0b: //FIRMWARE_INFO
        {
            //do NOT modify this code!!!
            att_server_rd( pdu_type, attOpcode, attHandle, GetFirmwareInfo(),strlen((const char*)GetFirmwareInfo()));
            break;
        }
        
        case 0x0d: //PnPID
        {
            u8 t[7] = {0,0,0,0,0,0,0};
            att_server_rd( pdu_type, attOpcode, attHandle, t, 7);
            break;
        }
        
        case 0x0f: //SOFTWARE_INFO
            att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(SOFTWARE_INFO), sizeof(SOFTWARE_INFO)-1);
            break;
        
        case 0x14://lock state
            att_server_rd( pdu_type, attOpcode, attHandle, &LockFlag, 1);
            break;

        case 0x15://cfg
        {
            u8 t[2] = {0,0};
            att_server_rd( pdu_type, attOpcode, attHandle, t, 2);
            break;
        }

        default:
            att_ErrorFd_eCode(pdu_type, attOpcode, 0x0000, 0x06); //ATT_ERR_UNSUPPORTED_REQ
            break;
    }
}

void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP,u16 offset)
{
}

//return 1 means found
int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
                            unsigned short uuid16,   
                            unsigned short* hd_start_r,unsigned short* hd_end_r)
{
// example
    if((uuid16 == 0xfff0) && (hd_start <= 0x10))// MUST keep match with the information save in function  att_server_rdByGrType(...)
    {
        *hd_start_r = 0x10;
        *hd_end_r = 0x15;
        return 1;
    }
    
    return 0;
}


//本回调函数可用于蓝牙模块端主动发送数据之用，协议栈会在系统允许的时候（异步）回调本函数，不得阻塞！！
void gatt_user_send_notify_data_callback(void)
{
    static u8 LockFlagBak = 1;
    
    if (LockFlagBak != LockFlag)
    {
        LockFlagBak = LockFlag;
        
        sconn_notifydata(&LockFlagBak,1);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
u8* getsoftwareversion(void)
{
    return (u8*)SOFTWARE_INFO;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void updateDeviceInfoData(u8* name, u8 len){} //not needed, just for compile's happy


static unsigned char gConnectedFlag=0;
char GetConnectedStatus(void)
{
    return gConnectedFlag;
}

//---- MindMotion HardWare AES implemenation -----
//unsigned char aes_encrypt_HW(unsigned char *painText128bitBE,unsigned char *key128bitBE); //porting api, returns zero means not supported
unsigned char aes_encrypt_HW(unsigned char *_data, unsigned char *_key)
{
    unsigned int tmp;

#ifndef AES_HW_SUPPORT    //HW AES NOT supported
    return 0; //not supported

#else                     //HW AES supported

    AES->CR = 0x00;
    AES->CR|=0x03<<20;

    AES->KEYR3 = (u32)(_key[0] << 24)|(u32)(_key[1] << 16)|(u32)(_key[2] << 8)|(u32)(_key[3] << 0);
    AES->KEYR2 = (u32)(_key[4] << 24)|(u32)(_key[5] << 16)|(u32)(_key[6] << 8)|(u32)(_key[7] << 0);
    AES->KEYR1 = (u32)(_key[8] << 24)|(u32)(_key[9] << 16)|(u32)(_key[10] << 8)|(u32)(_key[11] << 0);
    AES->KEYR0 = (u32)(_key[12] << 24)|(u32)(_key[13] << 16)|(u32)(_key[14] << 8)|(u32)(_key[15] << 0);	

    AES->CR |= 0x01;  //start encryption

    AES->DINR = (u32)(_data[0] << 24)|(u32)(_data[1] << 16)|(u32)(_data[2] << 8)|(u32)(_data[3] << 0);
    AES->DINR = (u32)(_data[4] << 24)|(u32)(_data[5] << 16)|(u32)(_data[6] << 8)|(u32)(_data[7] << 0);
    AES->DINR = (u32)(_data[8] << 24)|(u32)(_data[9] << 16)|(u32)(_data[10] << 8)|(u32)(_data[11] << 0);
    AES->DINR = (u32)(_data[12] << 24)|(u32)(_data[13] << 16)|(u32)(_data[14] << 8)|(u32)(_data[15] << 0);	

    //查询模式
    while(1)
    {
        if((AES->SR & 0x01) )
        {
            AES->CR |=  0x1<<7; //clear ccf flag
            break;
        }
    }
//    _text[0] = AES->DOUTR;  //encrypted output data
//    _text[1] = AES->DOUTR;
//    _text[2] = AES->DOUTR;
//    _text[3] = AES->DOUTR;
    
    tmp = AES->DOUTR;  //encrypted output data0
    _data[0] = tmp >> 24;
    _data[1] = tmp >> 16;
    _data[2] = tmp >>  8;
    _data[3] = tmp;
    _data += 4;
    
    tmp = AES->DOUTR;  //encrypted output data1
    _data[0] = tmp >> 24;
    _data[1] = tmp >> 16;
    _data[2] = tmp >>  8;
    _data[3] = tmp;
    _data += 4;
    
    tmp = AES->DOUTR;  //encrypted output data2
    _data[0] = tmp >> 24;
    _data[1] = tmp >> 16;
    _data[2] = tmp >>  8;
    _data[3] = tmp;
    _data += 4;
    
    tmp = AES->DOUTR;  //encrypted output data3
    _data[0] = tmp >> 24;
    _data[1] = tmp >> 16;
    _data[2] = tmp >>  8;
    _data[3] = tmp;

    return 1;// HW supported value
#endif
}


void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
    //enable/disable ase clock if any
#ifdef AES_HW_SUPPORT
    if(IsConnectedFlag)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_AES, ENABLE); //AES CLK enable
    }
    else
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_AES, DISABLE); //AES CLK enable
    }
#endif
    
    if(!IsConnectedFlag)
    CanNotifyFlag = IsConnectedFlag; //disconnected, so can NOT notify data

    //[IsConnectedFlag] indicates the connection status
    gConnectedFlag = IsConnectedFlag;
}

unsigned int StandbyTimeout = 0; 
void UsrProcCallback(void) //porting api
{
    static unsigned char led_flash = 0;
    
    IWDG_ReloadCounter();
    StandbyTimeout ++;
    
    if(gConnectedFlag){//connected
        StandbyTimeout = 0;
        LED_ONOFF(1);
    }else{
        led_flash ++;
        LED_ONOFF(!(led_flash%10)); //flash every 10 interval
    }
}

#endif
