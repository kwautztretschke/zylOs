#ifndef __PIXELBUFFER_H__
#define __PIXELBUFFER_H__

#include <stdint.h>

// TODO have these in some config.h
#define X_RES 1
#define Y_RES 1

class Pixel {
public:
	uint8_t     r;
	uint8_t     g;
	uint8_t 	b;
	uint8_t     a;
	//****** constructors  *********************************************************
	Pixel() = default;
	Pixel(const Pixel& rhs) = default;
	Pixel(uint8_t ir, uint8_t ig, uint8_t ib) : a(255), r(ir), g(ig), b(ib) {}
	Pixel(uint8_t ia, uint8_t ir, uint8_t ig, uint8_t ib) : a(ia), r(ir), g(ig), b(ib) {}

	//***** operators *************************************************************
	Pixel& operator=  (const Pixel& rhs) = default;
	Pixel& operator+= (const Pixel& rhs);

	//***** methods (mostly stolen from FastLED) **********************************
	Pixel&	 		setARGB(uint8_t na, uint8_t nr, uint8_t ng, uint8_t nb); 
	Pixel&	 		setAlpha(uint8_t na);
	Pixel&	 		setHue(uint8_t h);
	Pixel&	 		setHSV(uint8_t h, uint8_t s, uint8_t v);
	Pixel&	 		addToRGB(uint8_t d);
	Pixel&	 		subtractFromRGB(uint8_t d);
	Pixel&	 		nscale8(uint8_t scaledown);
	static Pixel 	blend(const Pixel from, const Pixel to, const uint8_t amount);
};


class Framebuffer {
private:
	Pixel				m_FB[X_RES][Y_RES];
public:
	//****** constructors  *********************************************************
	;			Framebuffer(Pixel c);
	;			Framebuffer();

	//***** operators *************************************************************
	Pixel&		operator()(int x, int y);
	void		operator=(Pixel c);

	//***** methods ***************************************************************
	Pixel&		xy(int x, int y);
	void		setAll(Pixel c);
	void		setAlpha(uint8_t na);
	void		blur(uint8_t amount);
	void		blurRows(uint8_t amount);
	void		blurColumns(uint8_t amount);
};


#endif // __PIXELBUFFER_H__