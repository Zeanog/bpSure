/******************************************************************************/
// Math library header for the Raspberry PI Zero
// Written by Dennis E. Bahr, PhD
// Version January 22, 2021

/******************************************************************************/
#include <stdio.h>

#ifndef INC_MATH_H
#define INC_MATH_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#undef max
#undef min
#undef abs
#undef sign

#define max(x,y)  (((x) > (y)) ? (x) : (y))  
#define min(x,y)  (((x) < (y)) ? (x) : (y))  
#define abs(x) 	(((x) < 0 ) ? -(x): (x))  
#define sign(x,y) (((y) < 0 ) ? (-(abs(x))): (abs(x)))  

//------------------------------------------------------------------------------
// structure to handle complex numbers
//------------------------------------------------------------------------------
typedef struct {
  float real;
  float imag;
} complex;   

//------------------------------------------------------------------------------
// structure to separate exponent and mantissa
// from a single precision floating point number
//------------------------------------------------------------------------------
typedef union {
	float fpn;				// floating point number
	unsigned long uln;	// unsigned long number
} float_structure; 

//------------------------------------------------------------------------------
// structure to separate exponent and mantissa
// from a double precision floating point number
//------------------------------------------------------------------------------
typedef union {
	double fpn;
	struct {
		unsigned long ulo;
		unsigned long uhi;
	} n;
} double_structure;


//------------------------------------------------------------------------------
complex Complex(float, float);
complex Add(complex, complex); 
complex Sub(complex, complex);
complex Mul(complex, complex);
complex Div(complex, complex);
float Real(complex);
float Imag(complex);
float Mag(complex);
double mod(double, double);
double powr(double, int); 
float ln(float);
double sqrt(double);
float rad2deg(float);
float deg2rad(float);
double sin(double);
double cos(double);
void sincos(double x, double *, double *);
int16_t _sin(int16_t);
int16_t _cos(int16_t);
int16_t isin(int16_t);
int16_t icos(int16_t);
long labs(long);
double fabs(double);
float stddevf(float data[], int len);
float meanf(float data[], int len);
float stddevi(int data[], int len);
float meani(int data[], int len);
void thresholding(int y[], int signals[], int lag, float threshold, float influence);
#endif

//-----------------------------------------------------------------------------
//					END OF FILE
//-----------------------------------------------------------------------------
