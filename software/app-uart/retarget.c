#include <stdio.h>
#include <string.h>
#include "HAL_conf.h"
#include "HAL_device.h"
#include "mg_api.h"
#include "bsp.h"

extern char GetConnectedStatus(void);
extern void ChangeBaudRate(void);

#define MS1793_UART_VERSION_STRING  "IND:Ver1.0\n"


#ifdef USE_AT_CMD
//uart stream buffer, should > MAX_AT_CMD_BUF_SIZE for ATCMD
    #define MAX_SIZE 1500
#else  
    #define MAX_SIZE 500
#endif
u8 txBuf[MAX_SIZE]={0}, rxBuf[MAX_SIZE]={0};

static u16 RxCont = 0;
static u16 PosW = 0, txLen = 0;
unsigned int RxTimeout = 0;
unsigned int TxTimeout = 0;

extern volatile unsigned int SysTick_Count;
extern u32 BaudRate;
extern unsigned char SleepStop;


int fputc(int ch, FILE *f)
{
    UART_SendData(UART1, (uint16_t) ch);

    while(1)
    {
        if(UART_GetITStatus(UART1, UART_IT_TXIEN))
        {
             UART_ClearITPendingBit(UART1, UART_IT_TXIEN);
             break;
        }
    }

    return ch;
}

int fgetc(FILE *f)
{
    while(1)
    {
        if(UART_GetITStatus(UART1, UART_IT_RXIEN))
        {
            UART_ClearITPendingBit(UART1, UART_IT_RXIEN);  //
            break;
        }
    }
    
    return (int)UART_ReceiveData(UART1);
}

void UART1_IRQHandler(void)                	//串口1中断服务程序
{
    if(UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET)  //接收中断
    {
        UART_ClearITPendingBit(UART1,UART_IT_RXIEN);
        rxBuf[RxCont]=UART_ReceiveData(UART1);
        RxTimeout = SysTick_Count + 1000;
        RxCont++;
        if(RxCont >= MAX_SIZE)
        {
            RxCont = 0;
        }
    }
    if(UART_GetITStatus(UART1, UART_IT_TXIEN) != RESET)
    {
        UART_ClearITPendingBit(UART1,UART_IT_TXIEN);
        
        TxTimeout = SysTick_Count + (20000/BaudRate);

        if (PosW < txLen)
        {
            UART_SendData(UART1,txBuf[PosW++]);
        }
        else
        {
            UART_ITConfig(UART1, UART_IT_TXIEN, DISABLE);
        }

        if (PosW >= txLen){
            txLen = 0;
            PosW = 0;
        }
    }
}

//used by app-uart and app-uart-cmd
void moduleOutData(u8*data, u16 len) //api
{
    u16 i;

    if ((txLen+len)<MAX_SIZE)//buff not overflow
    {
        for (i=0;i<len;i++)
        {
            txBuf[txLen+i] = *(data+i);
        }
        txLen += len;
    }
}


//#define TO_HIGH_CASE   
#define comrxbuf_wr_pos RxCont
u16 comrxbuf_rd_pos = 0; //init, com rx buffer

///////////////FIFO proc for AT cmd///////////////
#ifdef USE_AT_CMD
//500 Byte
#define MAX_SEND_LEN  500
#define MAX_AT_CMD_BUF_SIZE (19+2*MAX_SEND_LEN)
u8 AtCmdBuf[MAX_AT_CMD_BUF_SIZE];
u16 AtCmdBufDataSize=0;

void updateDeviceInfoData(u8* name, u8 len);
void ble_set_interval(unsigned short interval/*unit= 0.625ms*/);//160==>100ms



//AT+SETNAME=abcdefg
void atcmd_SetName(u8* parameter,u16 len)
{   
    if(len <= 8){
        moduleOutData((u8*)"IND:ERR\n",8);
        return;
    }
    
    updateDeviceInfoData(parameter+8,len-8);
    moduleOutData((u8*)"IND:OK\n",7);
}

//AT+SETINTERVAL=160
void atcmd_SetAdvInt(u8* parameter,u16 len)
{
    int v = 0;
    unsigned char i;
    
    if(len <= 12){
        moduleOutData((u8*)"IND:ERR\n",8);
        return;
    }
    
    for(i = 12 ; i < len ; i ++)
    {
        v *= 10;
        v += (parameter[i] - '0');
    }
    
    if(v > 3200)v = 3200;
    
    ble_set_interval(v);
    
    moduleOutData((u8*)"IND:OK\n",7);
}

u8 Hex2Int(unsigned char C)
{
    if(C <= '9')
    {
        return C - '0';
    }
    else if(C <= 'F')
    {
        return C - 'A' + 10;
    }
    else
    {
        return C - 'a' + 10;
    }
}

//AT+BLESEND=N,0x123456abef
u8 send_data[MAX_SEND_LEN] = {0};
u16 SentNum = 0, SendLen = 0;
void atcmd_SendData(u8* parameter, u16 len)
{
//    u8* data = send_data;
    u16 data_len = 0,i;
    u16 len_datalen = 0;
    
    //I am here check the data format
    if(len <= 8){
        moduleOutData((u8*)"IND:ERR\n",8);
        return;
    }
    
    if(0 == GetConnectedStatus()){
        moduleOutData((u8*)"IND:ERR\n",8);
        return;
    }
    
    parameter += 8;//move to N
    while(parameter[0] != ',')
    {
        data_len *= 10;
        data_len += (parameter[0] - '0');
        
        parameter ++;
        len_datalen ++;
    }
    
    if(data_len > MAX_SEND_LEN){
        moduleOutData((u8*)"IND:ERR\n",8);
        return;
    }
    
    parameter += 3; //move 0x

    
#if 0    
    for(i = 0 ; i < data_len ; i ++) //strict data & datalen check
    {
        send_data[i] = ((Hex2Int(parameter[0]) << 4) | Hex2Int(parameter[1]));
        parameter += 2;
    }
#else
    len_datalen = (len - 11 - len_datalen)/2;

    SendLen += data_len;
    if (SendLen > MAX_SEND_LEN)
        SendLen -= MAX_SEND_LEN;
    
    for(i = 0 ; i < len_datalen ; i++) 
    {
        send_data[(SentNum+i)%MAX_SEND_LEN] = ((Hex2Int(parameter[0]) << 4) | Hex2Int(parameter[1]));
        parameter += 2;
    }
    for(i=len_datalen; i<data_len; i++) //data pad with 0
    {
        send_data[(SentNum+i)%MAX_SEND_LEN] = 0x30;
    }
#endif
    
    //ble send data [data, data_len]
    //send = sconn_notifydata(data,data_len); //pls check the connect status if necessary
    
    moduleOutData((u8*)"IND:OK\n",7);
}

static void at_blesend(void)
{
    if (!GetConnectedStatus())
    {
        SentNum = SendLen = 0;
    }
    
    if(SendLen > SentNum)
    {
        SentNum += sconn_notifydata(send_data+SentNum, (((SendLen - SentNum) > 20)? 20:(SendLen - SentNum)));
        if(SentNum == SendLen) SentNum = SendLen = 0;
    }
    else if(SendLen < SentNum)
    {
        SentNum += sconn_notifydata(send_data+SentNum, (((MAX_SEND_LEN - SentNum) > 20)? 20:(MAX_SEND_LEN - SentNum)));
    }
    
    SentNum %= MAX_SEND_LEN;
}

//AT+SETADVFLAG=x		x=0/1, 0 disable adv, 1 enable adv
void atcmd_SetAdvFlag(u8* parameter,u16 len)
{
    if(len <= 11){
        moduleOutData((u8*)"IND:ERR\n",8);
        return;
    }
    
    parameter += 11;//move to x
    
    if('0' == parameter[0]) //diable adv
    {
        ble_set_adv_enableFlag(0);
    }
    else
    {
        ble_set_adv_enableFlag(1);
    }
    
    moduleOutData((u8*)"IND:OK\n",7);
}

//AT+DISCON, disconnect the connection is any
void atcmd_DisconnecteBle(u8* parameter,u16 len)
{
    if(len != 6){
        moduleOutData((u8*)"IND:ERR\n",8);
        return;
    }
    ble_disconnect();
    moduleOutData((u8*)"IND:OK\n",7);
}

//AT+LOWPOWER=x		x=0/1/2
void atcmd_LowPower(u8* parameter,u16 len)
{
    if(len <= 9){
        moduleOutData((u8*)"IND:ERR\n",8);
        return;
    }
        
    parameter += 9;//move to x
    
    if (0x31 == parameter[0])
    {
        SleepStop = 1;
    }
    else if (0x32 == parameter[0])
    {
        SleepStop = 2;
    }
    else
    {
        SleepStop = 0;
    }
    
    moduleOutData((u8*)"IND:OK\n",7);
}
unsigned char WaitSetBaud=0;
//AT+SETBAUD
void atcmd_SetBaud(u8* parameter,u16 len)
{
    int v = 0;
    unsigned char i;
    
    if(len <= 8){
        moduleOutData((u8*)"IND:ERR\n",8);
        return;
    }
    
    for(i = 8 ; i < len ; i ++)
    {
        v *= 10;
        v += (parameter[i] - '0');
    }
    
    if (BaudRate != v)
    {
        BaudRate = v;
        WaitSetBaud = 1;
    }else{
        moduleOutData((u8*)"IND:OK\n",7);
    }

}

void atcmd_Minfo(u8* parameter,u16 len);
void atcmd_Help(u8* parameter,u16 len);
    
#define MAX_AT_CMD_NAME_SIZE 16
typedef void (*ATCMDFUNC)(u8* cmd/*NULL ended, leading with the cmd NAME string, checking usage*/,u16 len);    
typedef struct _tagATCMD
{
    ATCMDFUNC func;
    u8 name[MAX_AT_CMD_NAME_SIZE]; //max len is 11 bytes
}AT_CMD_FUNC;

//function list
AT_CMD_FUNC at_func_list[]=
{
    {atcmd_SetName,"SETNAME="},
    {atcmd_SetAdvInt,"SETINTERVAL="},
    {atcmd_SendData,"BLESEND="},
    {atcmd_LowPower,"LOWPOWER="},
    {atcmd_SetBaud,"SETBAUD="},
    {atcmd_SetAdvFlag,"SETADVFLAG="},
    {atcmd_DisconnecteBle,"DISCON"},
    {atcmd_Minfo,"MINFO"},
    {atcmd_Help,"HELP"},
};


u8 IsExactCmdInclude(u8* data, const u8* cmd)
{
    u8 i,len = strlen((const char*)cmd);
    
    for(i = 0 ; i < len ; i ++)
    {        
        if(cmd[i] != data[i])
        {
            return 0;
        }
    }
    
    return 1;
}

#define at_cmd_num  (sizeof(at_func_list)/sizeof(at_func_list[0]))
    
//AT+MINFO
void atcmd_Minfo(u8* parameter,u16 len)
{    
    moduleOutData((u8*)"IND:OK\n",7);
    
    moduleOutData((u8*)MS1793_UART_VERSION_STRING,sizeof(MS1793_UART_VERSION_STRING)-1);    
    
    moduleOutData((u8*)"IND:CON=",8);
    
    if(GetConnectedStatus())
    {
        moduleOutData((u8*)"1\n",2);
    }
    else
    {
        moduleOutData((u8*)"0\n",2);
    }
}

//AT+HELP
void atcmd_Help(u8* parameter,u16 len)
{
    u8 i,templen;
    u8 name[20]="AT+";
    
    moduleOutData((u8*)"IND:OK\n",7);
    
    moduleOutData((u8*)MS1793_UART_VERSION_STRING,sizeof(MS1793_UART_VERSION_STRING)-1);
    
    for(i = 0; i < at_cmd_num ; i ++)
    {
        strcpy((char*)name+3,((char*)at_func_list[i].name));
        templen = strlen((char*)name);
        name[templen] = 0X0A;
        moduleOutData((u8*)name,templen+1); //NULL terminated format
    }
}

void AtCmdParser(u8* cmd, u16 len)
{
    u8 i;    
    
    for(i = 0 ; i < at_cmd_num ; i++)
    {
        if(IsExactCmdInclude(cmd,at_func_list[i].name))
        {
            //found
            at_func_list[i].func(cmd/*including the CMD name*/,len);
            
            return;
        }
    }
#if UART_DBG_EN
    printf("IND:ERROR CMD not supported.\n");
#endif
    moduleOutData((u8*)"IND:ERR\n",8);
}


void AtCmdPreParser(u8* cmd, u16 len)
{
    if(!IsExactCmdInclude(cmd, (const u8*)"AT+")) //AT+MINFO
    {
#ifdef UART_DBG_EN
        printf("IND:ERROR CMD format error.\n");
#endif
        moduleOutData((u8*)"IND:ERR\n",8);
        return; //cmd error
    }
    
    AtCmdParser(cmd+3,len-3);
}

void CheckAtCmdInfo(void) //main entrance
{
    while(comrxbuf_wr_pos != comrxbuf_rd_pos) //not empty
    {
        //has data
        AtCmdBuf[AtCmdBufDataSize++] = rxBuf[comrxbuf_rd_pos];
        
        if(AtCmdBufDataSize >= MAX_AT_CMD_BUF_SIZE)//error found
        {
            AtCmdBufDataSize = 0;//I just reset the position, drop the cmd
        }
        
        if((RxTimeout < SysTick_Count)||(rxBuf[comrxbuf_rd_pos] == 0x0a) || (rxBuf[comrxbuf_rd_pos] == 0x0d))        
        {
            if(AtCmdBufDataSize == 1)
            {
                AtCmdBufDataSize = 0;
            }
            else//found one cmd
            {
                AtCmdBuf[AtCmdBufDataSize-1] = '\0';
                AtCmdPreParser(AtCmdBuf,AtCmdBufDataSize-1);
                
                AtCmdBufDataSize = 0;//move to next cmd if any
            }
        }
        
        comrxbuf_rd_pos ++;
        comrxbuf_rd_pos %= MAX_SIZE; //com buff len
    }
}
#endif //USE_AT_CMD


static void CheckComPortInData(void) //at cmd NOT supported
{
    u16 send;
    
    if(comrxbuf_wr_pos != comrxbuf_rd_pos)//not empty
    {
        if(!GetConnectedStatus())
        {
            comrxbuf_rd_pos = comrxbuf_wr_pos; //empty the buffer if any
        }
        else //connected
        {
            if(comrxbuf_wr_pos > comrxbuf_rd_pos)
            {
                send = sconn_notifydata(rxBuf+comrxbuf_rd_pos, ((comrxbuf_wr_pos - comrxbuf_rd_pos)>255)?255:(comrxbuf_wr_pos - comrxbuf_rd_pos));
                comrxbuf_rd_pos += send;
            }
            else 
            {
                send = sconn_notifydata(rxBuf+comrxbuf_rd_pos, ((MAX_SIZE - comrxbuf_rd_pos) > 255)?255:(MAX_SIZE - comrxbuf_rd_pos));
                comrxbuf_rd_pos += send;
                comrxbuf_rd_pos %= MAX_SIZE;
            }
        }
    }
}

void UsrProcCallback(void) //porting api
{
//    u16 conn_interv = 0;
    static u16 counter = 0; 
    
    IWDG_ReloadCounter();

    counter ++;
    LED_ONOFF(counter & 0x1);
    
#if 0
    if(GetConnectedStatus()){
        counter ++;
        if(counter == 200){
            conn_interv = sconn_GetConnInterval();
            if(conn_interv > 24){//24*1.25=30ms
                SIG_ConnParaUpdateReq(0x0006, 0x0010, 0x0000, 1000);
            }
            counter = 0;
        }
    }else{
        counter = 0;
    }
#endif

#ifdef USE_AT_CMD
    CheckAtCmdInfo();
    at_blesend();
#else //AT CMD not supported
    CheckComPortInData();
#endif
    if ((2 != SleepStop)||(!(GPIO_ReadInputData(GPIOA) & 0x0010)))//CTS low
    {
        if ((txLen) && (0 == PosW))
        {
            UART_ITConfig(UART1, UART_IT_TXIEN, ENABLE);
            UART_SendData(UART1, txBuf[PosW++]);
            
            TxTimeout = SysTick_Count + (20000/BaudRate);
        }
    }
    if ((SleepStop == 2)&&(RxTimeout < SysTick_Count))
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_11);
        RxTimeout = SysTick_Count + (20000/BaudRate);
    }
    
#ifdef USE_AT_CMD
    if (WaitSetBaud)
    {
        WaitSetBaud++;
        
        if (WaitSetBaud > 2)
        {
            ChangeBaudRate();
            WaitSetBaud = 0;
            moduleOutData((u8*)"IND:OK\n",7);
        }
    }
#endif
}
