#include "lib8tion/noise8.h"

#include "lib8tion/math8.h"
#include "lib8tion/scale8.h"
#include "lib8tion/ease8.h"

#include <string.h>

static uint8_t const p[] = {
    151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
    140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
    247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
     57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
     74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
     60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
     65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
    200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
     52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
    207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
    119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
    129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
    218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
     81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
    184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
    222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180,
    151};

#define EASE8(x)  (ease8InOutQuad(x) )
#define EASE16(x) (ease16InOutQuad(x))

#define FADE(x) scale16(x,x)
#define LERP(a,b,u) lerp15by16(a,b,u)

static int16_t grad16(uint8_t hash, int16_t x, int16_t y, int16_t z) {
    hash = hash&15;
    int16_t u = hash<8?x:y;
    int16_t v = hash<4?y:hash==12||hash==14?x:z;
    if(hash&1) { u = -u; }
    if(hash&2) { v = -v; }

    return avg15(u,v);
}

static int16_t grad16(uint8_t hash, int16_t x, int16_t y) {
    hash = hash & 7;
    int16_t u,v;
    if(hash < 4) { u = x; v = y; } else { u = y; v = x; }
    if(hash&1) { u = -u; }
    if(hash&2) { v = -v; }

    return avg15(u,v);
}

static int16_t grad16(uint8_t hash, int16_t x) {
    hash = hash & 15;
    int16_t u,v;
    if(hash > 8) { u=x;v=x; }
    else if(hash < 4) { u=x;v=1; }
    else { u=1;v=x; }
    if(hash&1) { u = -u; }
    if(hash&2) { v = -v; }

    return avg15(u,v);
}

// selectBasedOnHashBit performs this:
//   result = (hash & (1<<bitnumber)) ? a : b
// but with an AVR asm version that's smaller and quicker than C
// (and probably not worth including in lib8tion)
static int8_t selectBasedOnHashBit(uint8_t hash, uint8_t bitnumber, int8_t a, int8_t b) {
	int8_t result;
	result = (hash & (1<<bitnumber)) ? a : b;
	return result;
}

static int8_t  grad8(uint8_t hash, int8_t x, int8_t y, int8_t z) {
    hash &= 0xF;

    int8_t u, v;
    //u = (hash&8)?y:x;
    u = selectBasedOnHashBit( hash, 3, y, x);

    v = hash<4?y:hash==12||hash==14?x:z;

    if(hash&1) { u = -u; }
    if(hash&2) { v = -v; }

    return avg7(u,v);
}

static int8_t grad8(uint8_t hash, int8_t x, int8_t y)
{
    // since the tests below can be done bit-wise on the bottom
    // three bits, there's no need to mask off the higher bits
    //  hash = hash & 7;

    int8_t u,v;
    if( hash & 4) {
        u = y; v = x;
    } else {
        u = x; v = y;
    }

    if(hash&1) { u = -u; }
    if(hash&2) { v = -v; }

    return avg7(u,v);
}

static int8_t grad8(uint8_t hash, int8_t x)
{
    // since the tests below can be done bit-wise on the bottom
    // four bits, there's no need to mask off the higher bits
    //	hash = hash & 15;

    int8_t u,v;
    if(hash & 8) {
        u=x; v=x;
    } else {
		if(hash & 4) {
			u=1; v=x;
		} else {
			u=x; v=1;
		}
    }

    if(hash&1) { u = -u; }
    if(hash&2) { v = -v; }

    return avg7(u,v);
}

static int8_t lerp7by8( int8_t a, int8_t b, fract8 frac)
{
    // int8_t delta = b - a;
    // int16_t prod = (uint16_t)delta * (uint16_t)frac;
    // int8_t scaled = prod >> 8;
    // int8_t result = a + scaled;
    // return result;
    int8_t result;
    if( b > a) {
        uint8_t delta = b - a;
        uint8_t scaled = scale8( delta, frac);
        result = a + scaled;
    } else {
        uint8_t delta = a - b;
        uint8_t scaled = scale8( delta, frac);
        result = a - scaled;
    }
    return result;
}

int16_t inoise16_raw(uint32_t x, uint32_t y, uint32_t z)
{
    // Find the unit cube containing the point
    uint8_t X = (x>>16)&0xFF;
    uint8_t Y = (y>>16)&0xFF;
    uint8_t Z = (z>>16)&0xFF;

    // Hash cube corner coordinates
    uint8_t A = X+Y;
    uint8_t AA = A+Z;
    uint8_t AB = A+1+Z;
    uint8_t B = X+1+Y;
    uint8_t BA = B + Z;
    uint8_t BB = B+1+Z;

    // Get the relative position of the point in the cube
    uint16_t u = x & 0xFFFF;
    uint16_t v = y & 0xFFFF;
    uint16_t w = z & 0xFFFF;

    // Get a signed version of the above for the grad function
    int16_t xx = (u >> 1) & 0x7FFF;
    int16_t yy = (v >> 1) & 0x7FFF;
    int16_t zz = (w >> 1) & 0x7FFF;
    uint16_t N = 0x8000L;

    u = EASE16(u); v = EASE16(v); w = EASE16(w);

    // skip the log fade adjustment for the moment, otherwise here we would
    // adjust fade values for u,v,w
    int16_t X1 = LERP(grad16(AA, xx, yy, zz), grad16(BA, xx - N, yy, zz), u);
    int16_t X2 = LERP(grad16(AB, xx, yy-N, zz), grad16(BB, xx - N, yy - N, zz), u);
    int16_t X3 = LERP(grad16(AA+1, xx, yy, zz-N), grad16(BA+1, xx - N, yy, zz-N), u);
    int16_t X4 = LERP(grad16(AB+1, xx, yy-N, zz-N), grad16(BB+1, xx - N, yy - N, zz - N), u);

    int16_t Y1 = LERP(X1,X2,v);
    int16_t Y2 = LERP(X3,X4,v);

    int16_t ans = LERP(Y1,Y2,w);

    return ans;
}

uint16_t inoise16(uint32_t x, uint32_t y, uint32_t z) {
    int32_t ans = inoise16_raw(x,y,z);
    ans = ans + 19052L;
    uint32_t pan = ans;
    // pan = (ans * 220L) >> 7.  That's the same as:
    // pan = (ans * 440L) >> 8.  And this way avoids a 7X four-byte shift-loop on AVR.
    // Identical math, except for the highest bit, which we don't care about anyway,
    // since we're returning the 'middle' 16 out of a 32-bit value anyway.
    pan *= 440L;
    return (pan>>8);
}

int16_t inoise16_raw(uint32_t x, uint32_t y)
{
    // Find the unit cube containing the point
    uint8_t X = x>>16;
    uint8_t Y = y>>16;

    // Hash cube corner coordinates
    uint8_t A = X+Y;
    uint8_t AA = A;
    uint8_t AB = A+1;
    uint8_t B = X+1+Y;
    uint8_t BA = B;
    uint8_t BB = B+1;

    // Get the relative position of the point in the cube
    uint16_t u = x & 0xFFFF;
    uint16_t v = y & 0xFFFF;

    // Get a signed version of the above for the grad function
    int16_t xx = (u >> 1) & 0x7FFF;
    int16_t yy = (v >> 1) & 0x7FFF;
    uint16_t N = 0x8000L;

    u = EASE16(u); v = EASE16(v);

    int16_t X1 = LERP(grad16(AA, xx, yy), grad16(BA, xx - N, yy), u);
    int16_t X2 = LERP(grad16(AB, xx, yy-N), grad16(BB, xx - N, yy - N), u);

    int16_t ans = LERP(X1,X2,v);

    return ans;
}

uint16_t inoise16(uint32_t x, uint32_t y) {
    int32_t ans = inoise16_raw(x,y);
    ans = ans + 17308L;
    uint32_t pan = ans;
    // pan = (ans * 242L) >> 7.  That's the same as:
    // pan = (ans * 484L) >> 8.  And this way avoids a 7X four-byte shift-loop on AVR.
    // Identical math, except for the highest bit, which we don't care about anyway,
    // since we're returning the 'middle' 16 out of a 32-bit value anyway.
    pan *= 484L;
    return (pan>>8);
}

int16_t inoise16_raw(uint32_t x)
{
    // Find the unit cube containing the point
    uint8_t X = x>>16;

    // Hash cube corner coordinates
    uint8_t A = X;
    uint8_t AA = A;
    uint8_t B = X+1;
    uint8_t BA = B;

    // Get the relative position of the point in the cube
    uint16_t u = x & 0xFFFF;

    // Get a signed version of the above for the grad function
    int16_t xx = (u >> 1) & 0x7FFF;
    uint16_t N = 0x8000L;

    u = EASE16(u);

    int16_t ans = LERP(grad16(AA, xx), grad16(BA, xx - N), u);

    return ans;
}

uint16_t inoise16(uint32_t x) {
    return ((uint32_t)((int32_t)inoise16_raw(x) + 17308L)) << 1;
}

int8_t inoise8_raw(uint16_t x, uint16_t y, uint16_t z)
{
    // Find the unit cube containing the point
    uint8_t X = x>>8;
    uint8_t Y = y>>8;
    uint8_t Z = z>>8;

    // Hash cube corner coordinates
    uint8_t A = X+Y;
    uint8_t AA = A+Z;
    uint8_t AB = A+1+Z;
    uint8_t B = X+1+Y;
    uint8_t BA = B + Z;
    uint8_t BB = B+1+Z;

    // Get the relative position of the point in the cube
    uint8_t u = x;
    uint8_t v = y;
    uint8_t w = z;

    // Get a signed version of the above for the grad function
    int8_t xx = ((uint8_t)(x)>>1) & 0x7F;
    int8_t yy = ((uint8_t)(y)>>1) & 0x7F;
    int8_t zz = ((uint8_t)(z)>>1) & 0x7F;
    uint8_t N = 0x80;

    u = EASE8(u); v = EASE8(v); w = EASE8(w);

    int8_t X1 = lerp7by8(grad8(AA, xx, yy, zz), grad8(BA, xx - N, yy, zz), u);
    int8_t X2 = lerp7by8(grad8(AB, xx, yy-N, zz), grad8(BB, xx - N, yy - N, zz), u);
    int8_t X3 = lerp7by8(grad8(AA+1, xx, yy, zz-N), grad8(BA+1, xx - N, yy, zz-N), u);
    int8_t X4 = lerp7by8(grad8(AB+1, xx, yy-N, zz-N), grad8(BB+1, xx - N, yy - N, zz - N), u);

    int8_t Y1 = lerp7by8(X1,X2,v);
    int8_t Y2 = lerp7by8(X3,X4,v);

    int8_t ans = lerp7by8(Y1,Y2,w);

    return ans;
}

uint8_t inoise8(uint16_t x, uint16_t y, uint16_t z) {
    //return scale8(76+(inoise8_raw(x,y,z)),215)<<1;
    int8_t n = inoise8_raw( x, y, z);  // -64..+64
    n+= 64;                            //   0..128
    uint8_t ans = qadd8( n, n);        //   0..255
    return ans;
}

int8_t inoise8_raw(uint16_t x, uint16_t y)
{
    // Find the unit cube containing the point
    uint8_t X = x>>8;
    uint8_t Y = y>>8;

    // Hash cube corner coordinates
    uint8_t A = X+Y;
    uint8_t AA = A;
    uint8_t AB = A+1;
    uint8_t B = X+1+Y;
    uint8_t BA = B;
    uint8_t BB = B+1;

    // Get the relative position of the point in the cube
    uint8_t u = x;
    uint8_t v = y;

    // Get a signed version of the above for the grad function
    int8_t xx = ((uint8_t)(x)>>1) & 0x7F;
    int8_t yy = ((uint8_t)(y)>>1) & 0x7F;
    uint8_t N = 0x80;

    u = EASE8(u); v = EASE8(v);

    int8_t X1 = lerp7by8(grad8(AA, xx, yy), grad8(BA, xx - N, yy), u);
    int8_t X2 = lerp7by8(grad8(AB, xx, yy-N), grad8(BB, xx - N, yy - N), u);

    int8_t ans = lerp7by8(X1,X2,v);

    return ans;
}



uint8_t inoise8(uint16_t x, uint16_t y) {
  //return scale8(69+inoise8_raw(x,y),237)<<1;
    int8_t n = inoise8_raw( x, y);  // -64..+64
    n+= 64;                         //   0..128
    uint8_t ans = qadd8( n, n);     //   0..255
    return ans;
}

// output range = -64 .. +64
int8_t inoise8_raw(uint16_t x)
{
  // Find the unit cube containing the point
  uint8_t X = x>>8;

  // Hash cube corner coordinates
  uint8_t A = X;
  uint8_t AA = A;
  uint8_t B = X+1;
  uint8_t BA = B;

  // Get the relative position of the point in the cube
  uint8_t u = x;

  // Get a signed version of the above for the grad function
  int8_t xx = ((uint8_t)(x)>>1) & 0x7F;
  uint8_t N = 0x80;

  u = EASE8( u);
  
  int8_t ans = lerp7by8(grad8(AA, xx), grad8(BA, xx - N), u);

  return ans;
}

uint8_t inoise8(uint16_t x) {
    int8_t n = inoise8_raw(x);    //-64..+64
    n += 64;                      // 0..128
    uint8_t ans = qadd8(n,n);     // 0..255
    return ans;
}

