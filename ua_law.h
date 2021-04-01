// PCM A-law Companding Algorithms in ANSI C

// ============== ADVANTAGE ==============
// A-law is good for low amplitude signals 
// which is the case for night time monitoring 

// ============== DISADVANTAGE ==============
// A-law has significantly smaller range than u-law

// We will mainly be looking at compressing the signal

#ifndef UA_LAW_H
#define UA_LAW_H

#include <iostream>

using namespace std; 

class UA_CODEC
{
    protected:
    public:
    
    int8_t MuLaw_Encode(int16_t number)
    {
    const uint16_t MULAW_MAX = 0x1FFF;
    const uint16_t MULAW_BIAS = 33;
    uint16_t mask = 0x1000;
    uint8_t sign = 0;
    uint8_t position = 12;
    uint8_t lsb = 0;
    if (number < 0)
    {
        number = -number;
        sign = 0x80;
    }
    number += MULAW_BIAS;
    if (number > MULAW_MAX)
    {
        number = MULAW_MAX;
    }
    for (; ((number & mask) != mask && position >= 5); mask >>= 1, position--)
            ;
    lsb = (number >> (position - 4)) & 0x0f;
    return (~(sign | ((position - 5) << 4) | lsb));
    }

    int16_t MuLaw_Decode(int8_t number)
    {
    const uint16_t MULAW_BIAS = 33;
    uint8_t sign = 0, position = 0;
    int16_t decoded = 0;
    number = ~number;
    if (number & 0x80)
    {
        number &= ~(1 << 7);
        sign = -1;
    }
    position = ((number & 0xF0) >> 4) + 5;
    decoded = ((1 << position) | ((number & 0x0F) << (position - 4))
                | (1 << (position - 5))) - MULAW_BIAS;
    return (sign == 0) ? (decoded) : (-(decoded));
    }

    int8_t ALaw_Encode(int16_t number) // Compresses 50% for 16-bit to 8-bits
    {
        const uint16_t ALAW_MAX = 0xFFF; //8-bit max value
        uint16_t mask = 0x800;
        uint8_t sign = 0;
        uint8_t position = 11;
        uint8_t lsb = 0;
        if(number<0)                // First, the number is verified if it is negative
        {                           // If the number is negative, it WILL BE made positive
            number = -number;       // and the sign variable (default 0) will contain 0x80
            sign = 0x80;            // (So when it is OR operated, the coded result will determine it's sign)
        }  
        if(number>ALAW_MAX)         // Since the A-law algorithm considers number from 0x000 to 0xFFF
        {                           // if a number is bigger than 0xFFF, it will automatically make it equal to 0xFFF
            number = ALAW_MAX;      // This is done to avoidd problems
        }
        for(;((number & mask) != mask && position >= 5); mask >>= 1, position--);
        lsb = (number>>((position == 4) ? (1) : (position - 4))) & 0x0f;

        return(sign | ((position - 4) << 4) | lsb)^0x55;
    }
    /*
        * Description:
        *  Decodes an 8-bit unsigned integer using the A-Law.
        * Parameters:
        *  number - the number who will be decoded
        * Returns:
        *  The decoded number
    */
    int16_t ALaw_Decode(int8_t number)
    {
        uint8_t sign = 0x00;
        uint8_t position = 0;
        int16_t decoded = 0;
        number^=0x55;
        if(number&0x80){
            number&=~(1<<7);
            sign = -1;
        }
        position = ((number & 0xF0) >>4) + 4;
        if(position!=4){
            decoded = ((1<<position)|((number&0x0F)<<(position-4))
                        |(1<<(position-5)));
        }
        else{
            decoded = (number<<1)|1;
        }
        return (sign==0)?(decoded):(-decoded);
    }
};

#endif