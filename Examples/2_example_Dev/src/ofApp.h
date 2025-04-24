#pragma once
#include "ofMain.h"

#include "ofxWindowApp.h"

// NOTE: Press "k" to toggle enable key commands!

// NOTE: In ofxWindowApp.h enable directive below to full debug.
// #define OFX_WINDOW_APP__DEVELOP_DEBUG

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void exit();
	void keyPressed(int key);
			
	ofxWindowApp w;
	
	uint64_t tLast=0;
};
