#pragma once

/// TODO:
///
/// BUG:	! Sometimes json lib from ofxSerialize collides with other addons that are using other/older json libs
///				I need to found a workaround to this problem.
///				maybe must put all json libs to the same version (?)
///				Seems that must be the same json OF version in all addons!
///				https://github.com/bakercp/ofxSerializer/issues/3#issuecomment-629564391

#include "ofMain.h"

//you can comment or undefine USE_ofxWindowApp to disable the addon and avoid to include the addon and depencies
#define USE_ofxWindowApp

#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

class ofApp : public ofBaseApp {

public:
	void setup();
	void keyPressed(int key);
	void windowResized(int w, int h);

#ifdef USE_ofxWindowApp
	ofxWindowApp windowApp;
#endif
};
