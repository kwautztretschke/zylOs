#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <FastLED.h>

#include "ZylOsConfig.h"
#include "ProgramManager.h"


class Program{
protected:
	//* utility functions you could use
	int 				m_ColorIndex = 0;
						// returns the color at the current index
	CRGB				getColor();
						// returns the color relative to the current index
	CRGB				getColorRelative(int index);
public: 
	//* necessary stuff for the program manager
	const char*			m_Name;
	#ifdef Z_ZYLINDER
		CRGB				m_FB[X_RES][Y_RES] = {0};
	#else
		CRGB				m_FB[FB_SIZE] = {0};
	#endif
	Program*			m_pNext = 0;
	;					Program();
	;					Program(bool add);
	//* methods to be redefined by programs -> "interface"
	virtual int 		init(){return 0;}
	virtual void       	activate(){}
	virtual void       	render(long tick){}
	virtual int			input(char* key, char* value);
	virtual void		artnet(const uint8_t* data, const uint16_t size){};
};

class ArtnetHelper{
private:
	const char*			m_pSuffix = nullptr;
	int 				m_Channel = 0;
	uint8_t				m_Threshold = 0;
	int 				m_Smooth = 0;
	uint8_t* 			m_pArtNetHistory = nullptr;
	int 				m_ArtNetHistorySize = 0;
	int					m_ArtNetHistoryIndex = 0;
public:
	;					ArtnetHelper(uint8_t* pArtNetHistory, int artNetHistorySize, const char* suffix);
	;					ArtnetHelper(uint8_t* pArtNetHistory, int artNetHistorySize) : ArtnetHelper(pArtNetHistory, artNetHistorySize, nullptr){};
						// returns the raw value, <index> steps back in time
	uint8_t				getArtNetHistory(int index);
						// returns a more usable value to scale stuff with
	uint8_t				getModulator();
						// handles input if suffix matches (e.g. "channel/l", "128")
	int 				input(char* key, char* value);
						// puts the selected channel into the history
	void				artnet(const uint8_t* data, const uint16_t size);
};



#endif // __PROGRAM_H__