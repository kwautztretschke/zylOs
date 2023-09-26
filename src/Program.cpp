#include "Program.h"

Program::Program()
{	// automatically add new Program to linked list
	ProgramManager::add(this);
}

Program::Program(bool add)
{	// automatically add new Program to linked list
	//! to use this, derived Program needs "using Program::Program"
	if(add)
		ProgramManager::add(this);
}

int Program::input(char* key, char* value)
{	// handles color, if that's all you need
	if(!strcmp(key, "colorindex"))
		m_ColorIndex = strtol(value, NULL, 10);
	return 0;
}

CRGB Program::getColor()
{	// returns the color at the current index
	return ProgramManager::getColor(m_ColorIndex);
}

CRGB Program::getColorRelative(int index)
{	
	if (index == 0)
		return ProgramManager::getColor(m_ColorIndex);
	if (index < 0) {
		// add enough multiples of ZPM_COLORS to make the index positive
		index += (-index / ZPM_COLORS + 1) * ZPM_COLORS;
	}
	return ProgramManager::getColor((index+m_ColorIndex)%ZPM_COLORS);
}

int ArtnetProgram::input(char* key, char* value)
{	// handles channel and threshold, if that's all you need
	Program::input(key, value);
	if(!strcmp(key, "channel"))
		m_Channel = strtol(value, NULL, 10);
	else if(!strcmp(key, "threshold"))	
		m_Threshold = strtol(value, NULL, 10);
	else if(!strcmp(key, "smooth"))	
		m_Smooth = strtol(value, NULL, 10);
	return 0;
}

void ArtnetProgram::artnet(const uint8_t* data, const uint16_t size)
{	// puts the selected channel into the history
	if(m_Channel >= size)
		return;
	if (m_ArtNetHistoryIndex-- < 0)
		m_ArtNetHistoryIndex = m_ArtNetHistorySize - 1;
	m_pArtNetHistory[m_ArtNetHistoryIndex] = data[m_Channel];
}

uint8_t ArtnetProgram::getArtNetHistory(int index)
{	
	if (index < 0)
	return 0;
	
	index = (m_ArtNetHistoryIndex + index) % m_ArtNetHistorySize;
	return m_pArtNetHistory[index];
}

CRGB ArtnetProgram::getModulatedColor()
{	// returns the color at the current index, 
	// modulated by the artnet history (with smoothing)
	return getModulatedColorRelative(0);
}

CRGB ArtnetProgram::getModulatedColorRelative(int index)
{	// returns the color relative to the current index, 
	// modulated by the artnet history (with smoothing)
	CRGB c = getColorRelative(index);
	if (m_Smooth == 0)
		return c.nscale8(getArtNetHistory(0));

	int sum = 0;
	for (int i = 0; i < m_Smooth; i++)
		sum += getArtNetHistory(i);
	sum /= m_Smooth;
	return c.nscale8(sum);
}