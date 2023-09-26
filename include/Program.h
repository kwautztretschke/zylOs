#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <FastLED.h>

#include "ZylOsConfig.h"
#include "ProgramManager.h"

#define ARTNET_HISTORY_SIZE 256


class Program{
protected:
	//* utility functions you could use
	int 					m_ColorIndex = 0;
						// returns the color at the current index
	CRGB					getColor();
						// returns the color relative to the current index
	CRGB					getColorRelative(int index);
public: 
	//* necessary stuff for the program manager
	const char*				m_Name;
	CRGB					m_FB[FB_SIZE] = {0};
	Program*				m_pNext = 0;
	;						Program();
	;						Program(bool add);
	//* methods to be redefined by programs -> "interface"
	virtual int 			init(){return 0;}
	virtual void          	activate(){}
	virtual void          	render(long tick){}
	virtual int				input(char* key, char* value){return 0;}
	virtual void			artnet(const uint8_t* data, const uint16_t size){};
};

class ArtnetProgram : public Program{
protected:
	int 					m_Channel = 0;
	int 					m_Threshold = 0;
	int 					m_Smooth = 0;
	char* 					m_pArtNetHistory = nullptr;
	int 					m_ArtNetHistorySize = 0;
	int						m_ArtNetHistoryIndex = 0;
	//* artnet utility functions
	uint8_t					getArtNetHistory(int index);
						// returns the color at the current index, 
						// modulated by the artnet history (with smoothing)
	CRGB					getModulatedColor();
	CRGB					getModulatedColorRelative(int index);
						// gets called after the regular input function, for your own needs
	virtual int				inputFurther(char* key, char* value){return 0;} 
						// gets called after the regular artnet function, for your own needs
	virtual void			artnetFurther(const uint8_t* data, const uint16_t size){};
public:
	using Program::Program;
						// handles channel and threshold, if that's all you need
	virtual int 			input(char* key, char* value) override;
						// puts the selected channel into the history
	virtual void			artnet(const uint8_t* data, const uint16_t size) override;
};



#endif // __PROGRAM_H__