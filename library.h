/******************************************************************************/
//	library.h   February 20, 2022
/******************************************************************************/
#ifndef LIBRARY_H
#define LIBRARY_H

unsigned int bcd2dec(unsigned int);
unsigned int dec2bcd(unsigned int);
void reverse(char *, int);
void dec2hex(int, char *, int);
long hex2dec(char *);
int itos(char *, int, int);
void ftos(char *, double, int);
 
#define COM_BUFFER_FULL  0x1F00
#define COM_BUFFER_EMPTY 0x1E00

#define RETURN_SUCCESS           0
#define RETURN_FAILURE          -1

#define bool	int
#define true	1
#define false  0

void memset( void* m, char val, int numBytes );
void memcpy( void* dest, const void* src, int numBytes );

#define PULSE_VAL_THRESHOLD 100
typedef struct _PulseInfo {
	int		sistolic;
	int		diastolic;
} PulseInfo;

void  InitPulseInfo(PulseInfo* info);

#define RINGBUFFER_SIZE  32
typedef struct _RingBuffer {
	int Buffer[RINGBUFFER_SIZE];
	int Write_Index;
} RingBuffer;

void InitRingBuffer( RingBuffer* buffer );
int WriteToRingBuffer( RingBuffer* buffer, int val );
int ReadFromRingBuffer( const RingBuffer* buffer, int offset );
void	PrintRingBuffer(const RingBuffer* buffer);

float DetermineAverage( const RingBuffer* buffer );
float DetermineDeviation(const RingBuffer* buffer );
#endif /* LIBRARY_H */
