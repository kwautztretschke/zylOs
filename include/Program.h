#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "ProgramManager.h"

#define NUM_LEDS 115 //TODO bad

class Program{
public: //TODO make some of these Private?
	const char*				m_Name;
	CRGB					m_FB[NUM_LEDS] = {0};
	Program*				m_pNext = 0;
	//* methods applicable for each Program
	;						Program();
	;						Program(bool add);
	//* methods to be redefined by programs -> "interface"
	virtual int 			init(){return 0;}
	virtual void          	activate(){}
	virtual void          	render(long tick){}
	virtual int				input(char* key, char* value){return 0;}
	virtual void			artnet(const uint8_t* data, const uint16_t size){};
};


#endif // __PROGRAM_H__