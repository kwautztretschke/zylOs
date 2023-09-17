# zylOs
provides the program manager and API to render different LED patterns

# The Program Manager
When you have a LED strip, and you want different nice patterns to show up, you can imagine these patterns to be programs, and the OS can only focus one program at a time.
This is the job of the program manager, if you tell it focus("name"), it will look into its linked list of program instances and retreive the one with that name.
Then, if you call render() each loop, you can display your patterns Framebuffer via getFB()

It also provides some extra features like a default color(s), which every program is encouraged to use, so you configure a nice set of colors, and then all programs will have that color scheme.

# The Programs
Each program is an implementation of the Program base class.
To make a new program, make a new file in the programs directory, where you copy this structure:
```cpp
#include "Program.h"

static class : public Program{
public:
  using Program::Program;
  int init(){
    m_Name = "sample";
    // initialize your program once
    return 0;
  }
  void activate(){
    // this gets called everytime your program is pulled into focus
    // if you had an animation ongoing, you could reset all timings to 0 here
  }
  int input(char* key, char* value){
    // handle the two strings your program has received
    return 0;
  }
	void render(long ms){
    // do things to the framebuffer each tick
	}
} myProgram;
```

# Configuration
You need to configure some parts depending on your setup.
For this, you simply need to create a header file "ZylOsConfig.h" in a /config/ directory in your project root folder.
(Note: for this to work, a certain folder structure is required. The library folder should be located at /*path/to/your/project*/lib/zylOs)

An example configuration file can be found in zylOs/include/ZylOsConfig.h
Just follow the instructions in the comments there.

# Dependencies
This library was built using platform.io, and as of now uses FastLED for the CRGB type. Expect that to change.