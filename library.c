/******************************************************************************/
//	library.c   February 20, 2022
/******************************************************************************/
#include "library.h"
#include "math.h"

//------------------------------------------------------------------------------
// convert a binary number to a decimal number
//------------------------------------------------------------------------------
unsigned int bcd2dec(unsigned int bcd) {
	return ((bcd & 0xf0) >> 4) * 10 + (bcd & 0x0f);
}

//------------------------------------------------------------------------------
// convert a decimal number to a binary number
//------------------------------------------------------------------------------
unsigned int dec2bcd(unsigned int d) {
	return ((d / 10) << 4) + (d % 10);
}

//------------------------------------------------------------------------------
// convert a hexidecimal string to a decimal number
//------------------------------------------------------------------------------
long hex2dec(char * a) {
	char c;
	long n = 0;
	
	while (*a) {
		c = *a++;

		if (c >= '0' && c <= '9') {
			c -= '0';
		}
		else if (c >= 'a' && c <= 'f') {
			c = (c - 'a') + 10;
		} 
		else if (c >= 'A' && c <= 'F') {
			c = (c - 'A') + 10;
		}
		else
			return -1;

		n = (n << 4) + c;
	}

	return n;
}

//------------------------------------------------------------------------------
// convert a decimal number to a hexadecimal number
//------------------------------------------------------------------------------   
void dec2hex(int quotient, char *str, int d) { 
	int i=0;
	 
	while(quotient != 0) { 
		int remainder = quotient % 16;
           
		if(remainder < 10) { 
			str[i++] = remainder + 48; 
		}
		else { 
			str[i++] = remainder + 55;  
		}
		quotient = (quotient - remainder) / 16; 
	}
	
	str[i] = '\0';
	reverse(str, 0);
} 

 //------------------------------------------------------------------------------ 
// reverses a string 'str' of length 'len'
//------------------------------------------------------------------------------  
void reverse(char* str, int len) { 
	int i = 0, j = len - 1, temp; 
	while (i < j) { 
		temp = str[i]; 
		str[i] = str[j]; 
		str[j] = temp; 
		i++; j--; 
	}
} 

//------------------------------------------------------------------------------ 
// Converts a signed integer x to an ASCII string 
//------------------------------------------------------------------------------ 
// numdigits is the number of digits required in the output.  
// If numdigits is more than the number of digits in x,  
// then 0s are added at the beginning. 
int itos(char* str, int num, int numdigits) { 
	int startIndex = 0;
	if(num < 0) {
		num = -num;
		str[startIndex++] = '-';
	} else {
		str[startIndex++] = ' ';
	}
	
	for( int ix = numdigits - 1; ix >= 0; --ix ) {
		str[ix + startIndex] = (num % 10) + '0';
		num = num / 10;
	}
	str[numdigits + startIndex] = '\0';
	return startIndex + numdigits;
} 

//------------------------------------------------------------------------------ 
// Converts a floating point number to an ASCII String
//------------------------------------------------------------------------------  
void ftos(char *str, double num, int digits) {  
	int i=0, j, sign = 1;
	double result = 1.0;
	
	if(num < 0) {
		num = -num;
		sign = -1.0;
	}
	
	int ipart = (int)num;
	double fpart = num - (double)ipart;

	i = itos(str, sign*ipart, 3);
	
	if(fpart != 0) {
		str[i] = '.';
		
		for(j=digits; j>0; j--)
			result = 10*result;
		
		fpart = fpart * result;
		itos(str + i + 1, (int)fpart, digits);
	} 
}

//------------------------------------------------------------------------------

void memset( void* m, char val, int numBytes ) {
	char* ptr = (char*)m;
	for( int ix = 0; ix < numBytes; ++ix ) {
		ptr[ix] = val;
	}
}

void memcpy( void* dest, const void* src, int numBytes ) {
	char* d = dest;
	const char* s = src;
	for( int ix = 0; ix < numBytes; ++ix ) {
		d[ix] = s[ix];
	}
}

//------------------------------------------------------------------------------

void InitPulseInfo(PulseInfo* info) {
	info->sistolic = 0;
	info->diastolic = 0;
}
 
//------------------------------------------------------------------------------

void InitRingBuffer( RingBuffer* buffer ) {
	buffer->Write_Index = 0;
	
	for( int ix = 0; ix < RINGBUFFER_SIZE; ++ix ) {
		buffer->Buffer[ix] = 0;
	}
}

int WriteToRingBuffer( RingBuffer* buffer, int val ) {
	buffer->Buffer[buffer->Write_Index] = val;
	buffer->Write_Index = (buffer->Write_Index + 1) % RINGBUFFER_SIZE;
	return RETURN_SUCCESS;	
}

int ReadFromRingBuffer( const RingBuffer* buffer, int offset ) {
	int index = -1;
	if( offset >= 0 ) {
		index = (buffer->Write_Index + offset) % RINGBUFFER_SIZE;
	} else {
		index = (buffer->Write_Index + offset);
		if( index < 0 ) {
			index = RINGBUFFER_SIZE + index;
		}
	}
	
	return buffer->Buffer[index];
}

float DetermineAverage( const RingBuffer* buffer ) {
	int sum = 0;
	for( int ix = 0; ix < RINGBUFFER_SIZE; ++ix ) {
		sum += buffer->Buffer[ix];
	}
	
	return (float)sum / RINGBUFFER_SIZE;
}

float DetermineDeviation(const RingBuffer* buffer ) {
	float average = DetermineAverage(buffer);
	
	float devSqrSum = 0;
	for( int ix = 0; ix < RINGBUFFER_SIZE; ++ix) {
		float diff = (buffer->Buffer[ix] - average);
		devSqrSum += (diff * diff);
	}
	
	return (float)sqrt(devSqrSum / (RINGBUFFER_SIZE - 1));
}
