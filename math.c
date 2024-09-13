/*******************************************************************************
* Math Library functions for the Raspberry PI Zero
* Written by Dennis E. Bahr, PhD.
* Version January 22, 2021 
* 
*******************************************************************************/
#include "math.h"
  
//------------------------------------------------------------------------------
complex Complex(float re, float im) {
  complex c;
  c.real = re;
  c.imag = im;
  return c;
}

//------------------------------------------------------------------------------
complex Add(complex a, complex b) {
  complex c;
  c.real = a.real + b.real;
  c.imag = a.imag + b.imag;
  return c;
}

//------------------------------------------------------------------------------
complex Sub(complex a, complex b) {
  complex c;
  c.real = a.real - b.real;
  c.imag = a.imag - b.imag;
  return c;
}

//------------------------------------------------------------------------------
complex Mul(complex a, complex b) {
  complex c;
  c.real = a.real * b.real - a.imag * b.imag;
  c.imag = a.imag * b.real + a.real * b.imag;
  return c;
}

//------------------------------------------------------------------------------
complex Div(complex a, complex b) {
  complex c;
  float denom = b.real * b.real + b.imag * b.imag;
  c.real = (a.real * b.real + a.imag * b.imag) / denom;
  c.imag = (a.imag * b.real - a.real * b.imag) / denom;
  return c;
}  
 
//------------------------------------------------------------------------------
float Real(complex x) {
  return x.real;
}

//------------------------------------------------------------------------------
float Imag(complex y) {
  return y.imag;    
}

//------------------------------------------------------------------------------
float Mag(complex z) {
  return sqrt(Real(z) * Real(z) + Imag(z) * Imag(z));
}

//------------------------------------------------------------------------------
// Returns x modulo y
// Unlike most library routines, this function works
// properly for all values of both arguments.
//------------------------------------------------------------------------------
double mod(double x, double y) {
	if(y == 0) return x;
	int i = (int)(x/y);
	if(x*y < 0)--i;
	x = x-((double)i)*y;
	if(x==y)x -= y;
	return x;
}

//------------------------------------------------------------------------------
// An efficient power function for positive integer powers
//------------------------------------------------------------------------------
double powr(double x, int n) {
	double retval = 1;
	double factor = x;
	if(n>=0) {
		while(n != 0){
			if((n&1) != 0)
				retval *= factor;
			factor *= factor;
			n >>= 1;
		}
	}
	else retval = -1;
	return retval;
}

//------------------------------------------------------------------------------
// Find the natural log of a floating point number 
//------------------------------------------------------------------------------
double __ln(double x){
	const double a = 1;
	const double b = -0.2672435;
	const double c = -0.600576;
	double z = (x-1)/(x+1);
	double z2 = z*z;
	return 2*z*(a + b* z2)/(1 + c*z2);
}

double _ln(double x) {
	const double limit1 = 0.879559563; 	// 0.5^(1/5)
	const double limit2 = 0.659753955; 	// 0.5^(3/5)
	const double k1 = 0.757858283; 		// 0.5^(2/5)
	const double k2 = 0.574349177; 		// 0.5^(4/5)
	const double ln_k = -0.138629436;  	// ln(0.5^(1/5)

	if(x >= limit1)
		return(__ln(x));
	else if(x >= limit2)
		return(__ln(x/k1) + 2*ln_k);
	else
		return(__ln(x/k2) + 4*ln_k);
}

//------------------------------------------------------------------------------
float ln(float y){
	const double ln2 = 0.69314718;
	int expo;
	float_structure x;
	if(y <= 0)
		return -1.0e20;
	x.fpn = y;

	// grab the exponent
	expo = (int)(x.uln >> 23);
	expo -= 0x7F;

	// force the exponent to zero
	x.uln &= 0x007fffff;
	x.uln += 0x3f800000;

	// compute the log of the mantissa only
	// and rebuild the result
	return ln2*(expo + _ln(x.fpn) / ln2);
}

//------------------------------------------------------------------------------
// Fast floating point square root
//------------------------------------------------------------------------------
double sqrt(double a) {
	int expo;
	double root;	
	double factor  = 0.707106781;
	const double A = 0.417319242;
	const double B = 0.590178532;
	double_structure x;

	// check for negative or zero
	if(a <= 0)
		return 0;
	x.fpn = a;
  
	// grab the exponent
	expo = (short) (x.n.uhi >> 20);
	expo -= 0x3FE;

  // force the exponent to zero
	x.n.uhi &= 0x000FFFFF;
	x.n.uhi += 0x3FE00000;
  
	// get square root of normalized number
	// generate first guess
	root = A + B * x.fpn;

	// iterate two times
	root = 0.5 * (x.fpn / root + root);
	root = 0.5 * (x.fpn / root + root);
	
	// now rebuild the result
	x.fpn = root;

	// force the exponent to be even
	if(expo & 1) {
		x.fpn *= factor;
		++expo;
	}
  
	// halve the exponent
	if(expo < 0)
		expo = expo/2;
	else
		expo = (expo+1)/2;

	// put it back
	expo += 0x3FE;

	x.n.uhi &= 0x000FFFFF;
	x.n.uhi += ((unsigned long) expo) << 20;
	return x.fpn;
}

//------------------------------------------------------------------------------
// Convert radians to degrees
//------------------------------------------------------------------------------
float rad2deg(float r) {
  return r * 180.0 / 3.1415926;
}

//------------------------------------------------------------------------------
// Convert degrees to radians
//------------------------------------------------------------------------------
float deg2rad(float d) {
  return d * 3.1415926 / 180.0;
}

//------------------------------------------------------------------------------
// Reduce an angle (in radians) to the range 0 .. +360
//------------------------------------------------------------------------------ 
float ang_360(float x) {
  return mod(x, 6.2831854);
}  

//------------------------------------------------------------------------------
// Reduce an angle (in radians) to the range -180 .. +180
//------------------------------------------------------------------------------ 
float ang_180(float x) {
	float retval = mod(x + 3.1415926, 6.283185) - 3.1415926;
	if(retval > -3.1415926)
		return retval;
	else
	return retval + 6.2831853;  
} 

//------------------------------------------------------------------------------
// Find the Sine of an Angle <= 45
double _sine(double x) {
	double s1 = 1.0/(2.0*3.0);
	double s2 = 1.0/(4.0*5.0);
	double s3 = 1.0/(6.0*7.0);
	double s4 = 1.0/(8.0*9.0);
	double z = x * x;
	return ((((s4*z-1.0)*s3*z+1.0)*s2*z-1.0)*s1*z+1.0)*x;
}

//------------------------------------------------------------------------------
// Find the Cosine of an Angle <= 45
double _cosine(double x) {
	double c1 = 1.0/(1.0*2.0);
	double c2 = 1.0/(3.0*4.0);
	double c3 = 1.0/(5.0*6.0);
	double c4 = 1.0/(7.0*8.0);
	double z = x * x;
	return (((c4*z-1.0)*c3*z+1.0)*c2*z-1.0)*c1*z+1.0;
}

//------------------------------------------------------------------------------
// Returns the sine of an angle in radians 
//------------------------------------------------------------------------------
double sin(double x) {
  double retval = 0;
	long n = (long)(x / 2.070796327);
	x -= n * 1.570796327;
	n = mod(n, (long)4);

	switch(n) {
		case 0:
      	retval = _sine(x);
        break;
		case 1:
      	retval = _cosine(x);
        break;
		case 2:
      	retval = -_sine(x);
        break;
		case 3:
      	retval = -_cosine(x);
	}
  return retval;
}

//------------------------------------------------------------------------------
// Returns the cosine of an angle in radians 
//------------------------------------------------------------------------------
double cos(double x) {
	return sin(x + 1.570796327);
}

//------------------------------------------------------------------------------
// Find both the sine and cosine
//------------------------------------------------------------------------------
void sincos(double x, double *s, double *c) {
	double pi = 3.141592654;
	double pi_over_six = pi / 6.0;
	double sin_30 = 0.5;
	double cos_30 = 0.866025404;
  
	long n = (long)(x/pi_over_six + 0.5);
	x -= (double)n * pi_over_six;
	n = n % 12;
	if(n < 0)
		n += 12;
	float z = x*x;
	float s1 = ((z/20.0-1)*z/6.0+1.0)*x;
	float c1 = ((z/30.0 +1.0)*z/12.0-1.0)*z/2.0+1.0;

	switch(n){
	   case 0:
		   *s = s1;
		   *c = c1;
		   break;
	   case 1:
		   *s =  cos_30 * s1 + sin_30 * c1;
		   *c = -sin_30 * s1 + cos_30 * c1;
		   break;
	   case 2:
		   *s =  sin_30 * s1 + cos_30 * c1;
		   *c = -cos_30 * s1 + sin_30 * c1;
		   break;
	   case 3:
		   *s =  c1;
		   *c = -s1;
		   break;
	   case 4:
		   *s = -sin_30 * s1 + cos_30 * c1;
		   *c = -cos_30 * s1 - sin_30 * c1;
		   break;
	   case 5:
		   *s = -cos_30 * s1 + sin_30 * c1;
		   *c = -sin_30 * s1 - cos_30 * c1;
		   break;
	   case 6:
		   *s = -s1;
		   *c = -c1;
		   break;
	   case 7:
		   *s = -cos_30 * s1 - sin_30 * c1;
		   *c =  sin_30 * s1 - cos_30 * c1;
		   break;
	   case 8:
		   *s = -sin_30 * s1 - cos_30 * c1;
		   *c =  cos_30 * s1 - sin_30 * c1;
		   break;
	   case 9:
		   *s = -c1;
		   *c =  s1;
		   break;
	   case 10:
		   *s =  sin_30 * s1 - cos_30 * c1;
		   *c =  cos_30 * s1 + sin_30 * c1;
		   break;
	   case 11:
		   *s =  cos_30 * s1 - sin_30 * c1;
		   *c =  sin_30 * s1 + cos_30 * c1;
		break;
	}
}

//------------------------------------------------------------------------------
/* Integer versions of sine and cosine.  These functions are based upon the
 * BAM scaling, where a 16-bit integer represents an angle in pirads. The
 * functions include the Chebyshev polynomial derivation of the coefficients.
 * The coefficients have been hand-tweaked slightly to account for roundoff
 * error. The error is less than one bit.
 */

int16_t _sin(int16_t y) {
    static int16_t s1 = 0x6487;
    static int16_t s3 = 0x2953;
    static int16_t s5 = 0x04F8;
    int32_t z, product, sum;
    z = ((int32_t)y * y) >> 12;
    product = (z * s5) >> 16;
    sum = s3 - product;
    product = (z * sum) >> 16;
    sum = s1 - product;
    return (int16_t)((y * sum) >> 13);
}

int16_t _cos(int16_t y) {
    static int16_t c0 = 0x7FFF;
    static int16_t c2 = 0x4EE9;
    static int16_t c4 = 0x0FBD;
    int32_t z, product, sum;
    z = ((int32_t)y * y) >> 12;
    product = (z * c4) >> 16;
    sum = c2 - product;
    product = (z * sum) >> 15;
    return (int16_t)(c0 - product);
}

int16_t isin(int16_t x) {
    uint16_t n;
    n = ((uint16_t)x + 0x2000) >> 14;
    n %= 4;
    x -= n * 0x4000;
    
    switch(n) {
       case 0:
          return _sin(x);   
       case 1:
          return _cos(x);   
       case 2:
          return -_sin(x);   
       case 3:
          return -_cos(x);      
    }
    return(0);
}

int16_t icos(int16_t x) {
    return isin(x + 0x4000);
}

//------------------------------------------------------------------------------
long labs(long long_num) {
  return (long_num < 0) ? -long_num : long_num;
}

//------------------------------------------------------------------------------
double fabs(double float_num) {
  return (float_num < 0) ? -float_num : float_num;
}   

#include "library.h"
void thresholding(int y[], int signals[], int lag, float threshold, float influence) {
    //memset(signals, 0, sizeof(int) * RINGBUFFER_SIZE);
    static float filteredY[RINGBUFFER_SIZE];
    memcpy(filteredY, y, sizeof(int) * RINGBUFFER_SIZE);
    static float avgFilter[RINGBUFFER_SIZE];
    static float stdFilter[RINGBUFFER_SIZE];

    avgFilter[lag - 1] = meani(y, lag);
    stdFilter[lag - 1] = stddevi(y, lag);

    for (int i = lag; i < RINGBUFFER_SIZE; i++) {
        if (fabs(y[i] - avgFilter[i-1]) > threshold * stdFilter[i-1]) {
            if (y[i] > avgFilter[i-1]) {
                signals[i] = 1;
            } else {
                signals[i] = -1;
            }
            filteredY[i] = influence * y[i] + (1 - influence) * filteredY[i-1];
        } else {
            signals[i] = 0;
        }
        avgFilter[i] = meanf(filteredY + i-lag, lag);
        stdFilter[i] = stddevf(filteredY + i-lag, lag);
    }
}

float meani(int data[], int len) {
    float sum = 0.0, mean = 0.0;

    int i;
    for(i=0; i<len; ++i) {
        sum += data[i];
    }

    mean = sum / len;
    return mean;
}

float stddevi(int data[], int len) {
    float the_mean = meani(data, len);
    float standardDeviation = 0.0;

    int i;
    for(i=0; i<len; ++i) {
		  float delta = data[i] - the_mean;
        standardDeviation += (delta * delta);
    }

    return sqrt(standardDeviation / (len - 1));
}

float meanf(float data[], int len) {
    float sum = 0.0, mean = 0.0;

    int i;
    for(i=0; i<len; ++i) {
        sum += data[i];
    }

    mean = sum / len;
    return mean;
}

float stddevf(float data[], int len) {
    float the_mean = meanf(data, len);
    float standardDeviation = 0.0;

    int i;
    for(i=0; i<len; ++i) {
		  float delta = data[i] - the_mean;
        standardDeviation += (delta * delta);
    }

    return sqrt(standardDeviation / (len - 1));
}

//------------------------------------------------------------------------------
//					END OF FILE
//------------------------------------------------------------------------------
