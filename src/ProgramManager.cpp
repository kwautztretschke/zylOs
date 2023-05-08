#include <HardwareSerial.h>

#include "ProgramManager.h"
#include "Program.h"


namespace ProgramManager{
	
static int			s_Count = 0;
static Program*		s_pHead = NULL;
static Program*		s_pActive = NULL;
static Pixel		s_Color(255, 0, 0);
static Framebuffer	s_FB;

void add(Program* ptr)
{	//call this in the constructor of your programs to add them to a list of all programs
	ptr->m_pNext = 	s_pHead;
	s_pHead = 		ptr;
	s_Count++;
}

int init(){
	Serial.println("Initializing Program Manager");
	s_pActive = s_pHead;
	return 0;
}

int initPrograms(){
	Serial.printf("initializing %d programs\n", s_Count);
	int error=0;
	Program* ptr = s_pHead;
	for(int i=0; i<s_Count; i++){
		error += ptr->init();
		Serial.printf("Program \"%s\" at %p\n", ptr->m_Name, ptr);
		ptr = ptr->m_pNext;
	}
	return error;
}

void render(long tick){
	s_pActive->render(tick);
	s_FB = s_pActive->m_FB;
}

int focus(char* name){
	Serial.printf("Program Manager searching \"%s\"... ", name);
	Program* ptr = s_pHead;
	while(ptr){
		if(!strcmp(ptr->m_Name, name)){
			Serial.println("Found!");
			s_pActive = ptr;
			s_pActive->activate();
			return 0;
		}
		ptr = ptr->m_pNext;
	}
	Serial.println("Error!");
	return 1;
}

int input(char* key, char* value){
	Serial.printf("Program Manager received input key=\"%s\", value=\"%s\"\n", key, value);
	return s_pActive->input(key, value);
}

void setColor(Pixel c){
	s_Color = c;
}

Pixel getColor() {
	return s_Color;
}

Framebuffer getFB() {
	return s_FB;
}

} // namespace ProgramManager