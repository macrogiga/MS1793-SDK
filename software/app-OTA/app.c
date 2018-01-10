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



#include <string.h>
#include "HAL_conf.h"
#include "mg_api.h"
#include "bsp.h"



/// Characteristic Properties Bit
#define ATT_CHAR_PROP_RD                            0x02
#define ATT_CHAR_PROP_W_NORSP                       0x04
#define ATT_CHAR_PROP_W                             0x08
#define ATT_CHAR_PROP_NTF                			0x10
#define ATT_CHAR_PROP_IND                           0x20 
#define GATT_PRIMARY_SERVICE_UUID                   0x2800

#define TYPE_CHAR      0x2803
#define TYPE_CFG       0x2902
#define TYPE_INFO      0x2901
#define TYPE_xRpRef    0x2907
#define TYPE_RpRef     0x2908
#define TYPE_INC       0x2802
#define UUID16_FORMAT  0xff


#define SOFTWARE_INFO "V1.0"
#define MANU_INFO     "MacroGiga"
char DeviceInfo[10] =  "MG-BLE-OTA";  /*max len is 24 bytes*/


u16 cur_notifyhandle = 0x14;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);

u8* getDeviceInfoData(u8* len)
{    
    *len = sizeof(DeviceInfo);
    return (u8*)DeviceInfo;
}

void updateDeviceInfoData(u8* name, u8 len)
{
    memcpy(DeviceInfo,name, len);
    
    ble_set_name(name,len);
}

/**********************************************************************************
                 *****DataBase****

01 - 06  GAP (Primary service) 0x1800
  03:04  name
07 - 0f  Device Info (Primary service) 0x180a
  0a:0b  firmware version
  0e:0f  software version
10 - 19  LED service (Primary service) 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
  11:12  6E400003-B5A3-F393-E0A9-E50E24DCCA9E(0x04)  RxNotify
  13     cfg
  14:15  6E400002-B5A3-F393-E0A9-E50E24DCCA9E(0x0C)  Tx
  16     cfg
  17:18  6E400004-B5A3-F393-E0A9-E50E24DCCA9E(0x0A)  BaudRate
  19     0x2901  info
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
    {TYPE_CHAR,0x03,ATT_CHAR_PROP_RD, 0x04,0,0x00,0x2a,UUID16_FORMAT},//name
    //05-06 reserved
// ======  device info =====    Do NOT Change if using the default!!!  
    {TYPE_CHAR,0x08,ATT_CHAR_PROP_RD, 0x09,0,0x29,0x2a,UUID16_FORMAT},//manufacture
    {TYPE_CHAR,0x0a,ATT_CHAR_PROP_RD, 0x0b,0,0x26,0x2a,UUID16_FORMAT},//firmware version
    {TYPE_CHAR,0x0e,ATT_CHAR_PROP_RD, 0x0f,0,0x28,0x2a,UUID16_FORMAT},//sw version
    {TYPE_CHAR,0x10,ATT_CHAR_PROP_RD, 0x11,0,0x23,0x2a,UUID16_FORMAT},//system ID
    
    {TYPE_CHAR,0x13,ATT_CHAR_PROP_W|ATT_CHAR_PROP_RD,                   0x14,0, 0,0, 3/*uuid128-idx3*/ },//OTA
    {TYPE_INFO, 0x15,ATT_CHAR_PROP_RD},//description,"Mg OTA"
};

const BLE_UUID128 AttUuid128List[] = {
    {0x10,0x19,0x0d,0xc,0xb,0xa,9,8,7,6,5,4,3,2,1,0}, //idx0,little endian, service uuid
    {0x11,0x19,0x0d,0xc,0xb,0xa,9,8,7,6,5,4,3,2,1,0}, //idx1,little endian, character status uuid
    {0x12,0x19,0x0d,0xc,0xb,0xa,9,8,7,6,5,4,3,2,1,0}, //idx2,little endian, character cmd uuid
    {0x13,0x19,0x0d,0xc,0xb,0xa,9,8,7,6,5,4,3,2,1,0}, //idx3,little endian, character OTA uuid

};

u8 GetCharListDim(void)
{
    return sizeof(AttCharList)/sizeof(AttCharList[0]);
}

void moduleOutData(u8*data, u8 len); //api

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
    
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x7))//hard code for device info service
    {
        u8 t[] = {0xa,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0x11,(u8*)(t),2);
        return;
    }
	 else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x12)) //OTA service
    {
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x12,0x15,(u8*)(AttUuid128List[0].uuid128),16);
        
        //
        //
        return;
    }
    //other service added here if any
    //to do....

    ///error handle
    att_notFd( pdu_type, attOpcode, st_hd );
}


///STEP2:data coming
///write response, data coming....
void ser_write_rsp(u8 pdu_type/*reserved*/, u8 attOpcode/*reserved*/, 
                   u16 att_hd, u8* attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{
 
    switch(att_hd)
    {
        case 0x14://OTA
            OTA_Proc(attValue, valueLen_w); //rsp followed
            ser_write_rsp_pkt(pdu_type);
            break;
        default:
            att_notFd( pdu_type, attOpcode, att_hd );	/*the default response, also for the purpose of error robust */
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
//	u8 tt[16] = {0x47,0x3f,0xa9,0xc1,0x47,0x80,0x14,0x90,0x3a,0xbc,0x57,0x7b,0x7d,0xc0,0x4d,0x8f};
    u8  d_len;
    u8* ble_name = getDeviceInfoData(&d_len);
    
    switch(attHandle) //hard code
    {
        case 0x04: //GAP name
            att_server_rd( pdu_type, attOpcode, attHandle, ble_name, d_len);
            break;
        
        case 0x09: //MANU_INFO
            att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(MANU_INFO), sizeof(MANU_INFO)-1);
            //att_server_rd( pdu_type, attOpcode, attHandle, get_ble_version(), strlen((const char*)get_ble_version())); //ble lib build version
            break;
        
        case 0x0b: //FIRMWARE_INFO
            //do NOT modify this code!!!
            att_server_rd( pdu_type, attOpcode, attHandle, GetFirmwareInfo(), strlen((const char*)GetFirmwareInfo()));
            break;
        
        case 0x0f://SOFTWARE_INFO
            att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(SOFTWARE_INFO), sizeof(SOFTWARE_INFO)-1);
            break;
        
        case 0x11://system ID
            {
                const u8 SystemID[8] = {0x12,0x34,0x56,0xff,0xfe,0x9c,0xbb,0xde};
                att_server_rd( pdu_type, attOpcode, attHandle, (u8 *)SystemID, 8);
            }
            break;
        case 0x15: //ota description
            #define MG_OTA "Mg OTA"
            att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(MG_OTA), sizeof(MG_OTA)-1);
            break;
        default:
            att_notFd( pdu_type, attOpcode, attHandle );/*the default response, also for the purpose of error robust */
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
//    if((uuid16 == 0x1812) && (hd_start <= 0x19))// MUST keep match with the information save in function  att_server_rdByGrType(...)
//    {
//        *hd_start_r = 0x19;
//        *hd_end_r = 0x2a;
//        return 1;
//    }
    
    return 0;
}
//本回调函数可用于蓝牙模块端主动发送数据之用，协议栈会在系统允许的时候（异步）回调本函数，不得阻塞！！
void gatt_user_send_notify_data_callback(void)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
u8* getsoftwareversion(void)
{
    return (u8*)SOFTWARE_INFO;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

static unsigned char gConnectedFlag=0;
char GetConnectedStatus(void)
{
    return gConnectedFlag;
}


void UsrProcCallback(void)
{
    u8 static led_flash = 0;
    
    if(gConnectedFlag == 0){
        LED_ONOFF((led_flash+1)&0x1);
        //led_flash ++;
    }
    else{
        LED_ONOFF(0);
    }
    
}

void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
    //[IsConnectedFlag] indicates the connection status
    
    //LED_ONOFF(IsConnectedFlag);
    
    if (IsConnectedFlag != gConnectedFlag)
    {
        gConnectedFlag = IsConnectedFlag;
    }
}

#ifdef BLE_PAIR_SUPPORT
unsigned char aes_encrypt_HW(unsigned char *_data, unsigned char *_key)
{
    return 0; //not supported
}
#endif
