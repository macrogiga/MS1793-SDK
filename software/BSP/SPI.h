#ifndef __SPI_H__
#define __SPI_H__

#define ACTIVATE     0x50
void mg_activate(unsigned char value);
void mg_writeReg(unsigned char reg, const unsigned char value);
void mg_readReg(unsigned char reg, unsigned char *value);
unsigned char mg_writeBuf(unsigned char reg, unsigned char const *pBuf, unsigned char len);
unsigned char mg_readBuf(unsigned char reg, unsigned char *pBuf, unsigned char len);

#endif
