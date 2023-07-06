#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
#ifdef SURFING_WINDOW_APP__USE_STATIC
	w.setup(&w);
	//w.setEnableKeys(false);
#endif
}

//--------------------------------------------------------------
void ofApp::draw()
{
	if (w.isChanged()) ofClear(ofColor::yellow);
	else ofClear(64);

	w.drawDebugInfo();
}

//--------------------------------------------------------------
void ofApp::exit()
{
#ifndef SURFING_WINDOW_APP__USE_STATIC
	w.save();
#endif
}