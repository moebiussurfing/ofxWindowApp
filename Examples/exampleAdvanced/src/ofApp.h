#pragma once

/*

	TODO:

	add move callback
	flash when saving
	remove timed stuff by a directive

*/

#include "ofMain.h"

#include "ofxWindowApp.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void exit();
	void windowResized(int w, int h);

	ofxWindowApp w;
};
