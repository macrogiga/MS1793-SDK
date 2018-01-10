/*
    Copyright (c) 2015 Macrogiga Electronics Co., Ltd.

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
#include "iwdg.h"


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


#define SOFTWARE_INFO "SV2.1.2"
#define MANU_INFO     "MacroGiga Bluetooth"
char DeviceInfo[6] =  "ARGUN3";  /*max len is 24 bytes*/

u16 cur_notifyhandle = 0x17;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);

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
    
	 
	{TYPE_CHAR,0x11,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF, 0x12,0, 0xF1,0xff, UUID16_FORMAT}, //gun
	{TYPE_CFG, 0x13,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W},//cfg
	{TYPE_CHAR,0x14,ATT_CHAR_PROP_RD, 0x15,0, 0xF2,0xff, UUID16_FORMAT},
	{TYPE_CHAR,0x16,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF, 0x17,0, 0xF3,0xff, UUID16_FORMAT},
	{TYPE_CFG, 0x18,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W},//cfg
	{TYPE_CHAR,0x19,ATT_CHAR_PROP_RD, 0x1A,0, 0xF4,0xff, UUID16_FORMAT},
	{TYPE_CHAR,0x1B,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W, 0x1C,0, 0xF5,0xff, UUID16_FORMAT},

};

const BLE_UUID128 AttUuid128List[] = {
    /*for supporting the android app [nRF UART V2.0], one SHOULD using the 0x9e,0xca,0xdc.... uuid128*/
    {0x6f,0x38,0xA1,0x38,0xAD,0x82,0x35,0x86,0xA0,0x43,0x13,0x5C,0X47,0X1E,0x5D,0x9E}, //idx0,little endian, service uuid
    {0x6B,0x66,0x6c,0x08,0x0A,0x57,0x8E,0x83,0x99,0x4E,0xA7,0xF7,0XBF,0X50,0xDD,0xE3}, //idx1,little endian, RxNotify
    {0x36,0xFE,0x2E,0xE7,0x09,0x24,0x4F,0xB7,0x91,0x40,0x61,0xD9,0X7A,0X6C,0xE8,0x92}, //idx2,little endian, Tx
	{0x3B,0xDE,0xC4,0xED,0xD4,0x75,0x3B,0x91,0xEB,0x47,0x2D,0x2E,0X08,0X76,0x7F,0x34}
};

const BLE_UUID128 AttUuid16List[][3] = {
  {0xf4,0xff},
  {0xE1,0xff},
  {0xf0,0xff},
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
    
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x07))//hard code for device info service
    {
        //apply user defined (device info)service example
        u8 t[] = {0xa,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0xf,(u8*)(t),2);
        return;
    }
    
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x10)) //usr's service
    {
        //att_server_rdByGrTypeRspPrimaryService(pdu_type,0x10,0x19,(u8*)(AttUuid128List[0].uuid128),16);
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x10,0x1C,(u8*)(AttUuid16List[2]),2);
		
		//
		//
        return;
    }
/*	else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x14)) //usr's service
	{
		att_server_rdByGrTypeRspPrimaryService(pdu_type,0x14,0x19,(u8*)(AttUuid16List[1]),2);
		return;
	}
	else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x1a)) //usr's service
	{
		att_server_rdByGrTypeRspPrimaryService(pdu_type,0x1A,0x21,(u8*)(AttUuid16List[2]),2);
		return;
	}
	else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x22)) //usr's service
	{
		att_server_rdByGrTypeRspPrimaryService(pdu_type,0x22,0x28,(u8*)(AttUuid128List[0].uuid128),16);
		return;
	}*/
    //other service added here if any
    //to do....

    ///error handle
    att_notFd( pdu_type, attOpcode, st_hd );
}
unsigned char AuthReq=0;

///STEP2:data coming
///write response, data coming....
void ser_write_rsp(u8 pdu_type/*reserved*/, u8 attOpcode/*reserved*/, 
                   u16 att_hd, u8* attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{

    switch(att_hd)
    {
        case 0x13://Tx
        case 0x18://cmd
		case 0x1C://cfg 
			if (0x1c == att_hd)
			{
				AuthReq = 1;
			}
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
	u8 tt1[16] = {0xE9,0x58,0xBA,0x39,0x58,0x0A,0x88,0x36,0x40,0xDD,0x5C,0x81,0x71,0x47,0x9C,0xF5};
	u8 tt2[16] = {0xEB,0x0B,0x56,0x65,0xB5,0x63,0x0C,0x48,0x66,0x81,0x29,0x96,0xB9,0x6C,0x24,0xCE};
	u8 tt4[16] = {0x4E,0x6F,0x47,0xD4,0xAE,0xE2,0x6C,0xBD,0xA7,0x88,0xFF,0x09,0xE1,0x6B,0x1B,0x1D};
    
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
        case 0x19://cfg
        case 0x21://cfg
            {
                u8 t[2] = {0,0};
                att_server_rd( pdu_type, attOpcode, attHandle, t, 2);
            }
            break;
        case 0x12:
            att_server_rd( pdu_type, attOpcode, attHandle, tt1, 16);
            break;
        case 0x15:
            att_server_rd( pdu_type, attOpcode, attHandle, tt2, 16);
            break;
        case 0x1A:
            att_server_rd( pdu_type, attOpcode, attHandle, tt4, 16);
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

static unsigned char KeyLastSta=0;
const unsigned char KeyData[2][16]={
 // {0xaf,0xd3,0x94,0x87,0xa4,0xac,0xbe,0xd6,0x10,0x9f,0xa9,0x50,0xb9,0x46,0x4f,0x75},	//press
 // {0x9c,0x3f,0x3a,0x14,0xaf,0xd8,0xa3,0x99,0x98,0xb1,0xb3,0x54,0x64,0xcc,0x13,0x8b}	//undo
/*	{0x71,0x9b,0x0e,0x5e,0x39,0xf6,0xf1,0x04,0x62,0x6c,0x9a,0x33,0xff,0xf1,0x24,0xf3},
	{0x3d,0x3a,0x3b,0xa9,0xfc,0xca,0x6b,0x78,0xb0,0x4c,0x3f,0xd4,0x9f,0x43,0xda,0xd0},*/
	{0x88,0x8a,0xf4,0x6e,0x85,0x55,0xe4,0x34,0xa6,0x91,0x57,0xba,0xbd,0xce,0xc4,0xab},
	{0xe9,0x21,0x55,0xde,0xa9,0x40,0x22,0x67,0x15,0x3d,0x8c,0x59,0x72,0x39,0xa7,0xbe},
};


const unsigned char AuthRsp[16]={
	0x45,0xbf,0x3a,0x48,0xda,0x3e,0x07,0xb9,0x75,0x01,0x41,0xf9,0xb6,0xdc,0xc5,0xa9
};
//本回调函数可用于蓝牙模块端主动发送数据之用，协议栈会在系统允许的时候（异步）回调本函数，不得阻塞！！
void gatt_user_send_notify_data_callback(void)
{
	if (1 == AuthReq)
	{
		AuthReq = 0;
		cur_notifyhandle = 0x12;
		sconn_notifydata((unsigned char *)AuthRsp,16);
	}
	else
	{
		cur_notifyhandle = 0x17;
		//add user sending data notify operation ....
		if (GPIO_ReadInputData(GPIOA) & 0x01)//press
		{
			if (KeyLastSta)
			{
				sconn_notifydata((unsigned char *)KeyData[1],16);
			}
			KeyLastSta = 0;
		}
		else//release
		{
			if (0 == KeyLastSta)
			{
				sconn_notifydata((unsigned char *)KeyData[0],16);
			}
			
			KeyLastSta = 1;
		}
	}
}

extern unsigned char *ble_mac_addr;

/*const */unsigned char ARGUNAdvDat0[]=
{
	0x02,0x01,0x06,
	0x03,0x02,0xf0,0xff,
						/*			Address			*/
	0x0b,0xff,0x04,0x01,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x0a,0x09,0x41,0x52,0x47,0x75,0x6e,0x47,0x61,0x6d,0x65};
const unsigned char ARGUNRspDat0[]=
{
0	};
	
const unsigned char ARGUNAdvDat1[]=
{
	0x02,0x01,0x06,
	0x03,0x02,0xe0,0xff,
	0x07,0x09,0x41,0x52,0x47,0x55,0x4e,0x34};
const unsigned char ARGUNRspDat1[]=
{
	0x0b,0xff,0x5b,0x37,0xc1,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

static unsigned char gConnectedFlag=0;
unsigned int StandbyTimeout = 0;
void UsrProcCallback(void) //porting api
{
    static unsigned char SwitchNum=0;
    unsigned char i;
    
    Write_Iwdg_RL();
    StandbyTimeout ++;
    
    if (gConnectedFlag)
    {
        SwitchNum = 0;
        StandbyTimeout = 0;
    }
    else
    {
        SwitchNum++;
        if (1 == SwitchNum)
        {
            ble_set_adv_data((unsigned char *)ARGUNAdvDat1,sizeof(ARGUNAdvDat1));
            ble_set_adv_rsp_data((unsigned char *)ARGUNRspDat1,sizeof(ARGUNRspDat1));
        }
        else if (11 == SwitchNum)
        {
            for (i=0;i<6;i++)
            {
                ARGUNAdvDat0[16-i] = *(ble_mac_addr+i);
            }
            ble_set_adv_data((unsigned char *)ARGUNAdvDat0,sizeof(ARGUNAdvDat0));
            ble_set_adv_rsp_data((unsigned char *)ARGUNRspDat0,0/*sizeof(ARGUNRspDat0)*/);
        }
        else if (20 <= SwitchNum)
        {
            SwitchNum = 0;
        }

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
u8* getsoftwareversion(void)
{
    return (u8*)SOFTWARE_INFO;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

char GetConnectedStatus(void)
{
    return gConnectedFlag;
}

void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
    //[IsConnectedFlag] indicates the connection status
    
    LED_ONOFF(!IsConnectedFlag);

    if (IsConnectedFlag != gConnectedFlag)
    {
        gConnectedFlag = IsConnectedFlag;
    }
}

