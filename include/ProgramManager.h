#ifndef __PROGRAMMANAGER_H__
#define __PROGRAMMANAGER_H__

#include <stdint.h>
#include <FastLED.h>

#ifndef FB_SIZE
	#define FB_SIZE 115 //TODO bad
#endif

class Program;

namespace ProgramManager{

void		add(Program *ptr);
int			initPrograms();
int			init();

void		render(long tick);
int			focus(char* program);
int			input(char* key, char* value);
void		artnet(const uint8_t* data, const uint16_t size);

void		setColor(CRGB c); //TODO consistency
CRGB		getColor();
CRGB*		getFB();

} // namespace ProgramManager

#endif // __PROGRAMMANAGER_H__