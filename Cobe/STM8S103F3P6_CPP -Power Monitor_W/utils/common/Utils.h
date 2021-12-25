

/*
* INA219.h
*
*  Created on: 4 Apr 2021
*      Author: Amit Chaudhary
*/

#ifndef UTILS_H
#define UTILS_H

#include "System.h"

namespace Utils
{

// reverses a string 'str' of length 'len'

  
  /* Function to calculate x raised to the power y */
static int power(int x, unsigned int y)
{
    if (y == 0)
        return 1;
    else if (y%2 == 0)
        return power(x, y/2)*power(x, y/2);
    else
        return x*power(x, y/2)*power(x, y/2);
}

static void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
 
 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
static int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}
    
// Generates a string from intiger and add a filler byte i the end
// filler byte can be '\n' or null character
static int intToStr(int x, char str[], int d, char filler)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = filler;
    return i;
}

// Converts a floating point number to string.
static int ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);
    
    if(i == 0) res[i++] = '0';

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * power(10.0, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
	return i + afterpoint + 1 ;
}

// Converts a floating point number to string.
static int ftoa(float n, char *res, int afterpoint, char filler)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);
    
    if(i == 0) res[i++] = '0';

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * power(10.0, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint,filler);
    }
	return i + afterpoint + 1 ;
}


/* Extended version of power function that can work
 for float x and negative y*/
 
static float power(float x, int y)
{
    float temp;
    if( y == 0)
       return 1;
    temp = power(x, y/2);       
    if (y%2 == 0)
        return temp*temp;
    else
    {
        if(y > 0)
            return x*temp*temp;
        else
            return (temp*temp)/x;
    }
}

void CountertoTime(uint32_t counter, char* timeStr)
  {    
    uint8_t Hours,Minutes,Seconds;
    
    if(timeStr)
    {           
      Hours     = counter / 3600U;
      Minutes   = (uint8_t)((counter % 3600U) / 60U);
      Seconds   = (uint8_t)((counter % 3600U) % 60U);    
      
      //if(Hours>=24) Hours = (Hours % 24U);
      
      intToStr(Hours,&timeStr[0],2);
      timeStr[2] = ':';
      intToStr(Minutes,&timeStr[3],2);
      timeStr[5] = ':';
      intToStr(Seconds,&timeStr[6],2);      
    }
  }

//static void mem_cpy (void* dst, const void* src, uint32_t cnt) 
//{
//	uint8_t *d = (uint8_t*)dst;
//	const uint8_t *s = (const uint8_t*)src;
//
//#if _WORD_ACCESS == 1
//	while (cnt >= sizeof (int)) {
//		*(int*)d = *(int*)s;
//		d += sizeof (int); s += sizeof (int);
//		cnt -= sizeof (int);
//	}
//#endif
//	while (cnt--)
//		*d++ = *s++;
//}
//
//static int mem_cmp (const void* dst, const void* src, uint32_t cnt) 
//{
//	const uint8_t *d = (const uint8_t *)dst, *s = (const uint8_t *)src;
//	int r = 0;
//
//	while (cnt-- && (r = *d++ - *s++) == 0) ;
//	return r;
//}


} // Utils

#endif