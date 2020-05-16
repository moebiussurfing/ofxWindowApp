#pragma once

#include "ofMain.h"

//TODO:
//you can comment or undefine USE_ofxWindowApp to disable the addon and avoid to include the addon and depencies
//this happens sometimes when several json libs are used and they collide...
//I need to found a workaround to this problem.
//maybe must put all json libs to the same version (?)

//#define USE_ofxWindowApp
#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

#ifdef USE_ofxWindowApp
	ofxWindowApp windowApp;
#endif
};
