/*

Name:   FED3WAN.h

Function:
    FED3WAN definitions to transmit data

Copyright:
    See accompanying LICENSE file for copyright and license information.

Author:
    Dhinesh Kumar Pitchai, MCCI Corporation   August 2021

*/

#ifndef _FED3WAN_h_
# define _FED3WAN_h_

#pragma once

#include "FED3.h"                                       //Include the FED3 library
#include <SoftwareSerial.h>                             //Include SoftwareSerial library
#include <stdint.h>

#define HEADER_LENGTH   0x4

// set A0 pin of 4610 as Serial RX pin
SoftwareSerial mySerial(-1, A0); // RX, TX

static constexpr unsigned kT35 = 5;    

class FED3WAN:FED3 {
    public:
    enum class SerialMessageOffset : uint8_t
        {
        ID,
        ADDR_HI,
        ADDR_LO,
        BYTE_CNT,
        };

    /**
     * @enum MESSAGE
     * @brief
     * Indices to datagram frame positions
     */
    enum MESSAGE
        {
        ID          = int(SerialMessageOffset::ID),           //!< Index of ID field
        ADD_HI      = int(SerialMessageOffset::ADDR_HI),       //!< Index of Address high byte
        ADD_LO      = int(SerialMessageOffset::ADDR_LO),       //!< Index of Address low byte
        BYTE_CNT    = int(SerialMessageOffset::BYTE_CNT),     //!< Index of byte counter
        };

    uint8_t buf[32];    // this sets the largest buffer size
    uint8_t *p;
    uint16_t u16timeOut;
    uint32_t u32time, u32timeOut;
    uint8_t au8Buffer[32];
    uint8_t u8BufferSize;
    uint16_t u16OutCnt;

    void begin(void);
    void run(FED3 *fed3);
    
    private:
    uint16_t calcCRC(uint8_t u8length);
    void sendTxBuffer(void);
    void put(uint8_t c);
    void put2(uint32_t v);
    void put2(int32_t v);
    void put2sf(float v);
    void put2uf(float v);
    void put2u(int32_t v);
    void put4u(uint32_t v);
    void putV(float V);

    uint8_t *getp(void)
        {
        return this->p;
        }

    std::size_t getn(void)
        {
        return this->p - this->buf;
        }

    uint8_t *getbase(void)
        {
        return this->buf;
        }
    };

uint16_t FED3WAN::calcCRC(uint8_t u8length)
    {
    unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    for (unsigned char i = 0; i < u8length; i++)
        {
        temp = temp ^ au8Buffer[i];
        for (unsigned char j = 1; j <= 8; j++)
            {
            flag = temp & 0x0001;
            temp >>=1;
            if (flag)
                temp ^= 0xA001;
            }
        }
    // Reverse byte order.
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF;
    // the returned value is already swapped
    // crcLo byte is first & crcHi byte is last
    return temp;
    }

void FED3WAN::sendTxBuffer()
    {
    uint8_t i = 0;

    // append CRC to message
    uint16_t u16crc = calcCRC( u8BufferSize );
    au8Buffer[ u8BufferSize ] = u16crc >> 8;
    u8BufferSize++;
    au8Buffer[ u8BufferSize ] = u16crc & 0x00ff;
    u8BufferSize++;

    // wait for things to settle.
    delayMicroseconds(100);

    // transfer buffer to serial line
    mySerial.write( au8Buffer, u8BufferSize );

    //mySerial.drainWrite();

    mySerial.drainRead();

    u8BufferSize = 0;

    // set time-out for host
    u32timeOut = millis() + (uint32_t) this->u16timeOut;

    // increase message counter
    u16OutCnt++;
    }

void FED3WAN::begin()
    {
    mySerial.begin(115200);
    u16OutCnt = 0;
    }

void FED3WAN::put(uint8_t c)
    {
    if (this->p < this->buf + sizeof(this->buf))
        *this->p++ = c;
    }

void FED3WAN::put2(uint32_t v)
    {
    if (v > 0xFFFF)
        v = 0xFFFF;
    this->put((uint8_t) (v >> 8));
    this->put((uint8_t) v);
    }

void FED3WAN::put2(int32_t v)
    {
    if (v < -0x8000)
        v = -0x8000;
    else if (v > 0x7FFF)
        v = 0x7FFF;
    this->put((uint8_t)(v >> 8));
    this->put((uint8_t)v);
    }

void FED3WAN::put2sf(float v)
    {
    int32_t iv;

    if (v > 32766.5f)
        iv = 0x7fff;
    else if (v < -32767.5f)
        iv = -0x8000;
    else
        iv = (int32_t)(v + 0.5f);

    this->put2(iv);
    }

void FED3WAN::put2uf(float v)
    {
    uint32_t iv;

    if (v > 65535.5f)
        iv = 0xffff;
    else if (v < 0.5f)
        iv = 0;
    else
        iv = (uint32_t)(v + 0.5f);

    this->put2(iv);
    }

void FED3WAN::put2u(int32_t v)
    {
    if (v < 0)
        v = 0;
    else if (v > 0xFFFF)
        v = 0xFFFF;
    this->put2((uint32_t) v);
    }

// put a 4-byte unsigned integer.
void FED3WAN::put4u(uint32_t v)
    {
    put((uint8_t)(v >> 24));
    put((uint8_t)(v >> 16));
    put((uint8_t)(v >> 8));
    put((uint8_t)v);
    }

void FED3WAN::putV(float V)
    {
    this->put2sf(V * 4096.0f + 0.5f);
    }

void FED3WAN::run(FED3 *fed3)
    {
    this->p = this->buf;

    this->putV(fed3->measuredvbat);
    this->put4u(fed3->numMotorTurns+1);
    
    this->put2u(fed3->FR);

    uint8_t event_active;
    uint16_t pokeTime;
    uint16_t retrievalTime;
    uint16_t event_with_time;

    if (fed3->Event == "Left")
            {
            pokeTime = fed3->leftInterval / 4;
            pokeTime &= 0x3FFF;
            event_active = 1;
            event_with_time = (pokeTime << 2) | event_active;
            }
    else if (fed3->Event == "Right")
            {
            pokeTime = fed3->rightInterval / 4;
            pokeTime &= 0x3FFF;
            event_active = 2;
            event_with_time = (pokeTime << 2) | event_active;
            }
    else if (fed3->Event == "Pellet")
            {
            retrievalTime = fed3->retInterval / 4;
            retrievalTime &= 0x3FFF;
            event_active = 3;
            event_with_time = (retrievalTime << 2) | event_active;
            }
    else
            {
            pokeTime = 0;
            retrievalTime = 0;
            event_active = 0;
            }

    this->put2u(event_with_time);
    this->put4u(fed3->LeftCount);
    this->put4u(fed3->RightCount);
    this->put4u(fed3->PelletCount);
    this->put2u(fed3->BlockPelletCount);

    auto pMessage = this->getbase();
    auto nMessage = this->getn();
    
    au8Buffer[ unsigned(SerialMessageOffset::ID) ] = 0x01;
    au8Buffer[ unsigned(SerialMessageOffset::ADDR_HI) ] = 0x00;
    au8Buffer[ unsigned(SerialMessageOffset::ADDR_LO) ] = 0x02;
    au8Buffer[ unsigned(SerialMessageOffset::BYTE_CNT) ] = nMessage;

    uint8_t nIndex;

    for (nIndex = 0; nIndex < nMessage; nIndex++)
            {
            au8Buffer[nIndex + HEADER_LENGTH] = (uint8_t) *(pMessage++);
            }

    u8BufferSize = nIndex + HEADER_LENGTH;
    
    this->sendTxBuffer();
    this->u32time = millis() + kT35;
    while ((int32_t)(millis() - this->u32time) < 0);
    }
    
    #endif  /* _FED3WAN_h_ */