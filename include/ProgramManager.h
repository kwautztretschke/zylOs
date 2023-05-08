#ifndef __PROGRAMMANAGER_H__
#define __PROGRAMMANAGER_H__

#include <stdint.h>

class Program;
class Framebuffer;
class Pixel;

namespace ProgramManager{

void			add(Program *ptr);
int				initPrograms();
int				init();

void			render(long tick);
int				focus(char* program);
int				input(char* key, char* value);

void			setColor(Pixel c);
Pixel			getColor();
Framebuffer		getFB();

} // namespace ProgramManager

#endif // __PROGRAMMANAGER_H__