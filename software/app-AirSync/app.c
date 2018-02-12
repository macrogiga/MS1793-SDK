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

#ifndef BLE_PAIR_SUPPORT //default cfg

#include <string.h>
#include <stdlib.h>
#include "mg_api.h"
#include "bsp.h"
#include "epb_MmBp.h"



extern unsigned char *ble_mac_addr;
extern unsigned char AdvDat[];

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


#define BUFFER_SIZE  200
static unsigned char TxBuff[BUFFER_SIZE];
static unsigned char RxBuff[BUFFER_SIZE];


#define SOFTWARE_INFO "SV2.1.2"
#define MANU_INFO     "MacroGiga Bluetooth"
char DeviceInfo[] =  "WeChatBle";  /*max len is 24 bytes*/

u16 cur_notifyhandle = 0x12;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);

u8* getDeviceInfoData(u8* len)
{
    *len = sizeof(DeviceInfo);
    return (u8*)DeviceInfo;
}

//void updateDeviceInfoData(u8* name, u8 len)
//{
//    memcpy(DeviceInfo,name, len);
//    
//    ble_set_name(name,len);
//}

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
    {TYPE_CHAR,0x0003, {ATT_CHAR_PROP_RD, 0x04,0, 0x00,0x2a}, UUID16_FORMAT},//name
    //05-06 reserved
// ======  device info =====    Do NOT Change if using the default!!!  
    {TYPE_CHAR,0x0008, {ATT_CHAR_PROP_RD, 0x09,0, 0x29,0x2a}, UUID16_FORMAT},//manufacture
    {TYPE_CHAR,0x000a, {ATT_CHAR_PROP_RD, 0x0b,0, 0x26,0x2a}, UUID16_FORMAT},//firmware version
    {TYPE_CHAR,0x000e, {ATT_CHAR_PROP_RD, 0x0f,0, 0x28,0x2a}, UUID16_FORMAT},//sw version
    
// ======  User service or other services added here =====  User defined  
    {TYPE_CHAR,0x0011, {ATT_CHAR_PROP_IND, 0x12,0, 0xc8,0xfe}, UUID16_FORMAT },
    {TYPE_CFG, 0x0013, {ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W}}, //cfg
    {TYPE_CHAR,0x0014, {ATT_CHAR_PROP_W, 0x15,0, 0xc7,0xfe}, UUID16_FORMAT },
	{TYPE_CHAR,0x0016, {ATT_CHAR_PROP_RD, 0x17,0, 0xc9,0xfe}, UUID16_FORMAT }
};

const BLE_UUID128 AttUuid128List[] = {0
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
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x07))//hard code for device info service
    {
        //apply user defined (device info)service example
        u8 t[] = {0xa,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0xf,(u8*)(t),2);
        return;
    }
    
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x10)) //usr's service
    {
        u8 t[] = {0xe7,0xfe};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x10,0x17,(u8*)(t),2);
        return;
    }
    //other service added here if any
    //to do....

    ///error handle
    att_notFd( pdu_type, attOpcode, st_hd );
}

static unsigned char Step=0,StepBak=0;
///STEP2:data coming
///write response, data coming....
void ser_write_rsp(u8 pdu_type/*reserved*/, u8 attOpcode/*reserved*/, 
                   u16 att_hd, u8* attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{
    static u16 offset=0;
    u16 Command,Len;
    AuthResponse *response1;
    SendDataResponse *response2;
    InitResponse *response3;
    RecvDataPush *push1;
    SwitchViewPush *push2;
    SwitchBackgroudPush *push3;
    
    switch(att_hd)
    {
        case 0x15://Tx

            if (valueLen_w)
            {
                memcpy(RxBuff+offset,attValue,valueLen_w);
                
                if ((RxBuff[2]<<8 | RxBuff[3]) ==  (offset + valueLen_w))
                {
                    offset = 0;
                }
                else//非完整帧
                {
                    offset += valueLen_w;
                }
                
                if ((0 == offset)&&(0xfe == RxBuff[0])&&(0x01 == RxBuff[1]))
                {
                    Len = (RxBuff[2]<<8 | RxBuff[3]);
                    Command = (RxBuff[4]<<8 | RxBuff[5]);
                    switch (Command)
                    {
                        case ECI_resp_auth:  //20001:
                            response1 = epb_unpack_auth_response(RxBuff+8, Len-8);
                            epb_unpack_auth_response_free(response1);
                            break;
                        
                        case ECI_resp_sendData:  //20002:
                            response2 = epb_unpack_send_data_response(RxBuff+8, Len-8);
                            epb_unpack_send_data_response_free(response2);
                            break;
                        
                        case ECI_resp_init: //20003:
                            response3 = epb_unpack_init_response(RxBuff+8, Len-8);
                            epb_unpack_init_response_free(response3);
                            break;
                        
                        case ECI_push_recvData:  //30001:
                            push1 = epb_unpack_recv_data_push(RxBuff+8, Len-8);
                            epb_unpack_recv_data_push_free(push1);
                            break;
                        
                        case ECI_push_switchView:  //30002:
                            push2 = epb_unpack_switch_view_push(RxBuff+8, Len-8);
                            epb_unpack_switch_view_push_free(push2);
                            break;
                        
                        case ECI_push_switchBackgroud:  //30003:
                            push3 = epb_unpack_switch_backgroud_push(RxBuff+8, Len-8);
                            epb_unpack_switch_backgroud_push_free(push3);
                            break;
                        
                        default:
                            break;
                    }
                    Step++;
                }
                
            }
            ser_write_rsp_pkt(pdu_type);
            break;

        case 0x13://cfg  
            Step = 1;
            ser_write_rsp_pkt(pdu_type);  /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/      
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
        
        case 0x0f://SOFTWARE_INFO
            att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(SOFTWARE_INFO), sizeof(SOFTWARE_INFO)-1);
            break;
        
        case 0x13://cfg
            {
                u8 t[2] = {0,0};
                att_server_rd( pdu_type, attOpcode, attHandle, t, 2);
            }
            break;
        
        case 0x17:
			att_server_rd( pdu_type, attOpcode, attHandle, ble_mac_addr, 6);
            break;
        
        default:
            att_notFd( pdu_type, attOpcode, attHandle );/*the default response, also for the purpose of error robust */
            break;
    }               
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
void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP,u16 offset)
{
}

//本回调函数可用于蓝牙模块端主动发送数据之用，协议栈会在系统允许的时候（异步）回调本函数，不得阻塞！！
void gatt_user_send_notify_data_callback(void)
{
    //to do if any ...
    //add user sending data notify operation ....
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

//void LED_ONOFF(unsigned char onFlag);//module led indicator

void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
    //[IsConnectedFlag] indicates the connection status
    
    LED_ONOFF(!IsConnectedFlag);

    if (IsConnectedFlag != gConnectedFlag)
    {
        gConnectedFlag = IsConnectedFlag;
        
        if (0 == gConnectedFlag)
        {
            Step = 0;
            StepBak = 0;
        }
    }
}

//uint8_t md5_type_id[16] = {0x79, 0xC1, 0x92, 0xD6, 0xE7, 0xEA, 0x6B, 0xB9, 0x7E, 0x1C, 0x2C, 0x44, 0xD3, 0xAB, 0x92, 0x43};
//uint8_t device_key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
//char *device_name = "WeChat_Device";
//const char *device_id = "Dev_001";
static char *timezone = "UTC+8";
static char *language = "zh-CN";


static unsigned short pack_auth_request(uint8_t *buf)
{
    int len = 0;
    AuthRequest *request = (AuthRequest *)malloc(sizeof(AuthRequest));
    
    request->has_md5_device_type_and_device_id = 0;
    request->proto_version = 0x010000;
    request->auth_proto = 1;
    request->auth_method = EAM_macNoEncrypt;

    request->has_mac_address = 1;
    request->mac_address.data = AdvDat + 11;
    request->mac_address.len = 6;

    request->has_aes_sign = 0;
    request->has_time_zone = true;
    request->time_zone.str = timezone;
    request->time_zone.len = strlen(timezone);

    request->has_language = true;
    request->language.str = language;
    request->language.len = strlen(language);

    request->has_device_name = true;
    request->device_name.str = DeviceInfo;//device_name;
    request->device_name.len = sizeof(DeviceInfo);

    len = epb_auth_request_pack_size(request);
    epb_pack_auth_request(request, buf, len);

	free(request);
    return len;
}

static unsigned short pack_init_request(uint8_t *buf)
{
    int len = 0;
    InitRequest *request = (InitRequest *)malloc(sizeof(InitRequest));
    
    request->has_resp_field_filter = 0;
    request->has_challenge = 0;

    len = epb_init_request_pack_size(request);
    epb_pack_init_request(request, buf, len);

    free(request);
    return len;
}

static unsigned short pack_senddata_request(uint8_t *buf)
{
    int len = 0;
    SendDataRequest *request = (SendDataRequest *)malloc(sizeof(SendDataRequest));
    
    request->data.data = 0;
    request->data.len = 0;
    request->has_type = 0;

    len = epb_send_data_request_pack_size(request);
    epb_pack_send_data_request(request, buf, len);

    free(request);
    return len;
}

void UsrProcCallback(void) //porting api
{
    u16 i = 0;
    u16 TxLen = 0;
    
    IWDG_ReloadCounter();
    
    if(GetConnectedStatus() && (Step < 4) && (Step != StepBak))
    {
        memset(TxBuff,0,BUFFER_SIZE);
        switch (Step)
        {
            case 1://auth
                TxLen = pack_auth_request(TxBuff+8)+8;
                TxBuff[4] = 0x27;
                TxBuff[5] = 0x11;
                break;
            case 2://init
                TxLen = pack_init_request(TxBuff+8)+8;
                TxBuff[4] = 0x27;
                TxBuff[5] = 0x13;
                break;
            case 3://send data
                TxLen = pack_senddata_request(TxBuff+8)+8;
                TxBuff[4] = 0x27;
                TxBuff[5] = 0x12;
                break;
            default:
                break;
        }
        TxBuff[0] = 0xfe;
        TxBuff[1] = 0x01;
        TxBuff[2] = TxLen>>8;
        TxBuff[3] = TxLen;
        
        TxBuff[6] = 0x00;
        TxBuff[7] = Step;
        
        for (i=0;i<TxLen;i+=20)
        {
            sconn_indicationdata(TxBuff+i,20);
        }    
        StepBak = Step;
    }
}
#endif
