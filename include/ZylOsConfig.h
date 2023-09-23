#ifndef __ZYLOSCONFIG_META_H__
#define __ZYLOSCONFIG_META_H__
// **********************************************************
//
// Welcome, glad you made it.
//
// this file needs to include your zylOs configuration. It should be found at:
#include "../../../config/ZylOsConfig.h"
//
// to create yours, just copy everything between the lines below to a file
// <project_root>/config/ZylOsConfig.h
// and modify it to your needs.
//
//*********************************************************** FROM HERE


#ifndef __ZYLOSCONFIG_H__  //dont touch that
#define __ZYLOSCONFIG_H__

// what's the name of your device?
#define Z_DEVICE_NAME "tollerESP"

// the basics first, what kind of LED strip are you using?
//#define Z_MONOCHROME
#define Z_WS2812B

// if you're using an addressable LED strip, how many LEDs are there?
#define FB_SIZE 50
// if not, just delete this line.

// do you have a favorite color? (Green is not a creative color)
#define Z_DEFAULT_COLOR CRGB::Green

// how about a favorite program? uncomment this and it will be the first to run.
//#define Z_DEFAULT_PROGRAM "Rainbow"

#endif // __ZYLOSCONFIG_H__


// ********************************************************** TO HERE
//
// great job, you can go home now.
//
// the stuff below is just to make sure you didn't mess up. don't copy that.
// if you do, you will mess up beyond my capabilities of safeguarding your dumbass.
//
// why are you still reading this
//
// **********************************************************

#ifdef Z_MONOCHROME
	#ifdef Z_LED_TYPE
		#error "You can't use multiple LED types at once!"
	#endif
	#define Z_LED_TYPE

	#define FB_SIZE 1
#endif
#ifdef Z_WS2812B
	#ifdef Z_LED_TYPE
		#error "You can't use multiple LED types at once!"
	#endif
	#define Z_LED_TYPE

	#ifndef FB_SIZE
		#error "You need to define FB_SIZE when using Z_WS2812B!"
	#endif
#endif

#ifndef Z_LED_TYPE
	#error "You need to define either Z_MONOCHROME or Z_WS2812B!"
#endif

#ifndef Z_DEFAULT_COLOR
	#define Z_DEFAULT_COLOR CRGB::Green
#endif


#endif // __ZYLOSCONFIG_META_H__