#include "Pixelbuffer.h"

#include "lib8tion/lib8tion.h"
#include "lib8tion/math8.h"
#include "lib8tion/scale8.h"
#include "lib8tion/hsv2rgb.h"

//*************************** Pixel ***********************

Pixel& Pixel::operator+= (const Pixel& rhs )
{
	r = qadd8( r, rhs.r);
	g = qadd8( g, rhs.g);
	b = qadd8( b, rhs.b);
	return *this;
}

Pixel& Pixel::setARGB (uint8_t na, uint8_t nr, uint8_t ng, uint8_t nb){
	a = na; r = nr; g = ng; b = nb; 
	return *this;
}

Pixel& Pixel::setAlpha (uint8_t na){
	a = na; 
	return *this;
}

Pixel& Pixel::setHue(uint8_t h){
	hsv2rgb_rainbow(this, h, 255, 255);
	return *this;
}

Pixel& Pixel::setHSV(uint8_t h, uint8_t s, uint8_t v){
	hsv2rgb_rainbow(this, h, s, v);
	return *this;
}

Pixel& Pixel::addToRGB(uint8_t d){
	r = qadd8(r, d);
	g = qadd8(g, d);
	b = qadd8(b, d);
	return *this;
}

Pixel& Pixel::subtractFromRGB(uint8_t d){
	r = qsub8(r, d);
	g = qsub8(g, d);
	b = qsub8(b, d);
	return *this;
}

Pixel& Pixel::nscale8(uint8_t scaledown){
	nscale8x3(r, g, b, scaledown);
	return *this;
}

Pixel Pixel::blend(const Pixel from, const Pixel to, uint8_t amount){
	if(amount == 0)
		return from;
	if(amount == 255)
		return to;
	return Pixel(
		scale8(from.r, 255-amount) + scale8(to.r, amount),
		scale8(from.g, 255-amount) + scale8(to.g, amount),
		scale8(from.b, 255-amount) + scale8(to.b, amount)
	);
}

//*************************** Framebuffer ***********************

Framebuffer::Framebuffer(Pixel c){
	setAll(c);
}

Framebuffer::Framebuffer(){
	setAll(Pixel(255, 0, 0, 0));
}

Pixel&	Framebuffer::operator()(int x, int y)	{
	return xy(x,y);
}

void Framebuffer::operator=(Pixel c)	{
	setAll(c);
}

Pixel&	Framebuffer::xy(int x, int y){
	return m_FB[x][y];
}
void Framebuffer::setAll(Pixel c){
	for(int x=0;x<X_RES;x++)
		for(int y=0;y<Y_RES;y++)
			m_FB[x][y]=c;
}
void Framebuffer::setAlpha(uint8_t na){
	for(int x=0;x<X_RES;x++)
		for(int y=0;y<Y_RES;y++)
			m_FB[x][y].setAlpha(na);
}

void Framebuffer::blur(uint8_t amount){
	blurRows(amount);
	blurColumns(amount);
}

void Framebuffer::blurRows(uint8_t amount){
    // blur rows same as columns, for irregular matrix
    uint8_t keep = 255 - amount;
    uint8_t seep = amount >> 1;
    for(int row = 0; row < Y_RES; row++) {
        Pixel carryover = Pixel(0, 0, 0);
        for(int i = 0; i < X_RES; i++) {
            Pixel cur = xy(i,row);
            Pixel part = cur;
            part.nscale8(seep);
            cur.nscale8(keep);
            cur += carryover;
            if(i) 
				xy(i-1,row) += part;
            xy(i,row) = cur;
            carryover = part;
        }
    }
}

void Framebuffer::blurColumns(uint8_t amount){
    // blur columns
    uint8_t keep = 255 - amount;
    uint8_t seep = amount >> 1;
    for(uint8_t col = 0; col < X_RES; ++col) {
        Pixel carryover = Pixel(0, 0, 0);
        for(int i = 0; i < Y_RES; ++i) {
            Pixel cur = xy(col,i);
            Pixel part = cur;
            part.nscale8(seep);
            cur.nscale8(keep);
            cur += carryover;
            if(i) 
				xy(col,i-1) += part;
            xy(col,i) = cur;
            carryover = part;
        }
    }
}