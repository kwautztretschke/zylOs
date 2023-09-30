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
	if(!strcmp(key, "colorindex")){
		m_ColorIndex = strtol(value, NULL, 10);
		return 0;
	}
	return 1;
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


ArtnetHelper::ArtnetHelper(uint8_t *pArtNetHistory, int artNetHistorySize, const char* suffix)
{	// automatically add new Program to linked list
	m_pArtNetHistory = pArtNetHistory;
	m_ArtNetHistorySize = artNetHistorySize;
	m_ArtNetHistoryIndex = m_ArtNetHistorySize - 1;
	m_pSuffix = suffix;
}

int ArtnetHelper::input(char* key, char* value)
{	
	// Check if the key ends with the suffix (if we have one)
	if (m_pSuffix != nullptr){
		if((strlen(key) > strlen(m_pSuffix) &&					// is the input long enough to contain a suffix?
				key[strlen(key) - strlen(m_pSuffix) - 1] == '/' &&	// does the input contain a '/' before the suffix?
				strcmp(key + strlen(key) - strlen(m_pSuffix), m_pSuffix) == 0)) { // does the suffix match?
			// remove suffix from key
			key[strlen(key) - strlen(m_pSuffix) - 1] = '\0';
		} else {
			return 2; // no suffix match
		}
	}
	if(!strcmp(key, "channel")){
		int channel = strtol(value, NULL, 10);
		if (0 <= channel && channel < 512)
			m_Channel = strtol(value, NULL, 10);
		return 0;
	}
	if(!strcmp(key, "threshold")){
		int threshold = strtol(value, NULL, 10);
		if (0 <= threshold && threshold <= 255)
			m_Threshold = strtol(value, NULL, 10);
		return 0;
	}
	if(!strcmp(key, "smooth")){
		int smooth = strtol(value, NULL, 10);
		if (0 <= smooth && smooth <= m_ArtNetHistorySize)
			m_Smooth = strtol(value, NULL, 10);
		return 0;
	}
	return 1; //not found
}

void ArtnetHelper::artnet(const uint8_t* data, const uint16_t size)
{	
	if(m_Channel >= size)
		return;
	if (--m_ArtNetHistoryIndex < 0)
		m_ArtNetHistoryIndex = m_ArtNetHistorySize - 1;
	m_pArtNetHistory[m_ArtNetHistoryIndex] = data[m_Channel];
}

uint8_t ArtnetHelper::getArtNetHistory(int index)
{	
	if (index < 0)
		return 0;
	index = (m_ArtNetHistoryIndex + index) % m_ArtNetHistorySize;
	return m_pArtNetHistory[index];
}

uint8_t ArtnetHelper::getModulator()
{	
	if (m_Smooth == 0){
		uint8_t scalar = getArtNetHistory(0);
		if (scalar < m_Threshold)
			scalar = m_Threshold;
		return scalar;
	}
	int sum = 0;
	for (int i = 0; i < m_Smooth; i++)
		sum += getArtNetHistory(i);
	sum /= m_Smooth;
	if (sum < m_Threshold)
		sum = m_Threshold;
	return (uint8_t)sum;
}