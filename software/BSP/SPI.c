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
#include "includes.h"
#include "SPI.h"
#include "platform.h"

void mg_writeReg(unsigned char reg, const unsigned char value)
{
	SPI_CS_Enable;
	
	SPI_WriteRead(reg | 0x20,1);
	SPI_WriteRead(value,1);
	
	SPI_CS_Disable;
}

void mg_readReg(unsigned char reg, unsigned char *value)
{
	SPI_CS_Enable;
	
	SPI_WriteRead(reg,1);
	*value = SPI_WriteRead(0xff,0);
	
	SPI_CS_Disable;
}
void mg_activate(unsigned char value)
{
	SPI_CS_Enable;
	
	SPI_WriteRead(ACTIVATE,1);
	SPI_WriteRead(value,1);
	
	SPI_CS_Disable;
}
unsigned char mg_writeBuf(unsigned char reg, unsigned char const *pBuf, unsigned char len)
{
	unsigned char result;
	unsigned char i;
	
	SPI_CS_Enable;
	
	SPI_WriteRead(reg | 0x20,1);	
	
	for (i=0;i<len;i++)
	{
		result = SPI_WriteRead(*pBuf++,1);
	}
	
	SPI_CS_Disable;
	
	return result;
}

unsigned char mg_readBuf(unsigned char reg, unsigned char *pBuf, unsigned char len)
{
	unsigned char result;
	unsigned char i;
	
	SPI_CS_Enable;
	
	result = SPI_WriteRead(reg,1);
	
	for (i=0;i<len;i++)
	{
		*(pBuf+i) = SPI_WriteRead(0xff,0);
	}
	
	SPI_CS_Disable;
	
	return result;
}
