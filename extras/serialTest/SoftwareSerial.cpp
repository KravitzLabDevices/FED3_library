/*
  SoftwareSerial.cpp - library for Arduino M0/M0 pro
  Copyright (c) 2016 Arduino Srl. All rights reserved.
  Written by Chiara Ruggeri (chiara@arduino.org)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
  Enjoy!
*/ 
 
 
#include <Arduino.h>
#include "SoftwareSerial.h"
#include <variant.h>

// Constructor
SoftwareSerial::SoftwareSerial(uint8_t transmitPin, bool inverse_logic /* = false */) : 
  _tx_delay(0),
  _buffer_overflow(false),
  _inverse_logic(inverse_logic)
{   
  _transmitPin = transmitPin;
}

// Destructor
SoftwareSerial::~SoftwareSerial()
{
  end();
}

void SoftwareSerial::setTX(uint8_t tx)
{
  // First write, then set output. If we do this the other way around,
  // the pin would be output low for a short while before switching to
  // output high. Now, it is input with pullup for a short while, which
  // is fine. With inverse logic, either order is fine.
  digitalWrite(tx, _inverse_logic ? LOW : HIGH);
  pinMode(tx, OUTPUT);
  _transmitBitMask = digitalPinToBitMask(tx);
  PortGroup * port = digitalPinToPort(tx);
  _transmitPortRegister = (volatile PORT_OUT_Type*)portOutputRegister(port);
 
}

// Do not set above 57600 baud, else you will run into issues with data loss
void SoftwareSerial::begin(long speed)
{   
    setTX(_transmitPin);
    // calculates delay between bits in microseconds
  uint32_t bit_delay = (float(1)/speed)*1000000;
 
  _tx_delay = bit_delay;
}

size_t SoftwareSerial::write(uint8_t b)
{
  if (_tx_delay == 0) {
    setWriteError();
    return 0;
  }

  // By declaring these as local variables, the compiler will put them
  // in registers before entering the critical timing sections below, 
  // which makes it a lot easier to verify the cycle timings
  volatile PORT_OUT_Type *reg = _transmitPortRegister;
  uint32_t reg_mask = _transmitBitMask;
  uint32_t inv_mask = ~_transmitBitMask;
  bool inv = _inverse_logic;
  uint16_t delay = _tx_delay;
  
  if (inv)
    b = ~b;

  // Write the start bit
  if (inv)
    reg->reg |= reg_mask;
  else
    reg->reg &= inv_mask;

  delayMicroseconds(delay);


  // Write each of the 8 bits
  for (uint8_t i = 8; i > 0; --i)
  {
    if (b & 1) // choose bit
      reg->reg |= reg_mask; // send 1
    else
      reg->reg &= inv_mask; // send 0

    delayMicroseconds(delay); 
    b >>= 1;
  }

  // restore pin to natural state
  if (inv)
    reg->reg &= inv_mask;
  else
    reg->reg |= reg_mask;
  
  delayMicroseconds(delay);  
  
  return 1;
}

//
//  RX functions, functionality removed but functions themselves remain
//  so that this library is interchangable with standard serial library
//

void SoftwareSerial::end()
{
}
  
// returns error code, as there is nothing to read
int SoftwareSerial::read()
{
  return -1;
}  

// returns 0 to indicate nothing is available to read
int SoftwareSerial::available()
{
  return 0;
}

void SoftwareSerial::flush()
{
  //no tx buffering
}

// returns error code, as there is nothing to read
int SoftwareSerial::peek()
{
  return -1;
}
