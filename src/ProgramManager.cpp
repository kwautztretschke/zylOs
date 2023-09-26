#include <HardwareSerial.h>

#include "ProgramManager.h"
#include "Program.h"


namespace ProgramManager{
	
static int			s_Count 				= 0;
static Program*		s_pHead 				= NULL;
static Program*		s_pActive 				= NULL;
static CRGB			s_Color[ZPM_COLORS] 	= {0};
static CRGB			s_FB[FB_SIZE] 			= {0};


void add(Program* ptr)
{	//call this in the constructor of your programs to add them to a list of all programs
	ptr->m_pNext = 	s_pHead;
	s_pHead = 		ptr;
	s_Count++;
}

int init(){
	Serial.println("Initializing Program Manager");
	s_pActive = s_pHead;
	s_Color[0] = Z_DEFAULT_COLOR;
	#ifdef Z_DEFAULT_PROGRAM
		focus(Z_DEFAULT_PROGRAM);
	#endif
	return 0;
}

int initPrograms(){
	if(!s_Count){
		Serial.printf("No programs found! Critical Error!");
		return -1;
	}

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
	#ifdef Z_WS2812B
		for (int i=0;i<FB_SIZE;i++)
			s_FB[i] = s_pActive->m_FB[i];
	#endif
	#ifdef Z_MONOCHROME
		s_FB[0] = s_pActive->m_FB[0];
	#endif
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

void artnet(const uint8_t* data, const uint16_t size){
	s_pActive->artnet(data, size);
}

void setColor(uint8_t index, CRGB c){
	s_Color[index] = c;
}

CRGB getColor(uint8_t index) {
	if (index >= ZPM_COLORS)
		index = index % ZPM_COLORS;
	return s_Color[index];
}

CRGB* getFB() {
	return s_FB;
}

} // namespace ProgramManager