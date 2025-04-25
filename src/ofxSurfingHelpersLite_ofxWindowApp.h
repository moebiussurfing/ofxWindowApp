#pragma once
#include "ofMain.h"

//------

namespace ofxSurfingHelpersLite {
namespace ofxWindowApp {

/*
* App Window
* 
*	App window tools to handle position, settings or name.
*/

//// Get the OF project file path name and set to the window title.
////--------------------------------------------------------------
//inline string getProjectName() {
//	of::filesystem::path path = ofToDataPath("", true);
//	string s = (path.parent_path().parent_path().filename()).string();
//	return s;
//}
//
////--------------------------------------------------------------
//inline void setWindowTitleAsProjectName() {
//	ofSetWindowTitle(getProjectName());
//}
////--------------------------------------------------------------
//inline void setWindowTitleAsProjectNameWithFPS() {
//	string s = "";
//	s = getProjectName();
//	string sfps = ofToString(ofGetFrameRate(), 1);
//	s = s + " - " + sfps + " FPS";
//	ofSetWindowTitle(s);
//}
//
////--------------------------------------------------------------
//inline void setWindowMaxFrameRate() {
//	ofSetFrameRate(0);
//	ofSetVerticalSync(false);
//}

//--------------------------------------------------------------
inline string setWindowShapeSquared(int sz = 800) {
	ofSetWindowShape(sz, sz);

	string s = "Squared " + ofToString(sz) + "x" + ofToString(sz) + " (1:1)";
	return s;
}

////--------------------------------------------------------------
//inline string setWindowCentered() {
//	int ww = ofGetWindowWidth();
//	int hw = ofGetWindowHeight();
//	int ws = ofGetScreenWidth();
//	int hs = ofGetScreenHeight();
//	glm::vec2 p = glm::vec2(ws/2-ww/2,hs/2-hw/2);
//	ofSetWindowShape(p.x,p.y);
//	ofSetWindowPosition(ww,hw);
//
//	string s = "Centered: " + ofToString(p.x) + "," + ofToString(p.y) + " " + ofToString(ww) + "x" + ofToString(hw) + " px";
//	return s;
//}

//--------------------------------------------------------------
inline string setWindowShapeForInstagram(size_t i = 0, bool bForcePos = false) {
	/*
	* Instagram Social media 
	* Image type sizes
	* Dimensions in pixels
	*/

	vector<string> names = {
		"1080x1920 9:16\nIG Story",
		"1080x1080 1:1\nIG Square",
		"1080x1350 4:5\nIG Portrait",
		"1080x566 1.91:1\nIG Landscape",
		"420x654 1:1.55\nIGTV Cover",
	};

	i = ofClamp(i, 0, names.size() - 1);

	//if (bForcePos) ofSetWindowPosition(0, 0);

	if (i == 0) ofSetWindowShape(1080, 1920); // IG Story
	else if (i == 1) ofSetWindowShape(1080, 1080); // IG Square
	else if (i == 2) ofSetWindowShape(1080, 1350); // IG Portrait
	else if (i == 3) ofSetWindowShape(1080, 566); // IG Landscape
	else if (i == 4) ofSetWindowShape(420, 654); // IGTV Cover

	if (bForcePos) ofSetWindowPosition(0, 0); //fix force

	return names[i];

	//// TODO:
	//// allow resize on runtime
	//// set instagram size
	//case OF_KEY_F6: {
	//	int w, h;
	//	w = 864;
	//	h = 1080 + 19;
	//	ofSetWindowShape(w, h);
	//	windowResized(w, h);
	//	cap_w = w;
	//	cap_h = h;
	//	buildAllocateFbo();
	//} break;
}

//--------------------------------------------------------------
inline string keyPressedToSetWindowShape(int key) {
	//s += "\n";
	//s += "PRESETS\n";
	//s += "q : Squared 800 x 800 px\n";
	//s += "w : Squared Width x Width px\n";
	//s += "c : Center\n";
	//s += "1 : IGTV Cover Photo\n";
	//s += "2 : IG Landscape Photo\n";
	//s += "3 : IG Portrait\n";
	//s += "4 : IG Story\n";
	//s += "5 : IG Square\n";

	string sWindowDimensions = "";
	if (key == 'q')
		sWindowDimensions = setWindowShapeSquared(); // Squared 800 x 800
	else if (key == 'w')
		sWindowDimensions = setWindowShapeSquared(ofGetWindowWidth()); // Squared w x w
	//else if (key == 'c')
	//	sWindowDimensions = setWindowCentered(); // Centered
	else if (key == '1')
		sWindowDimensions = setWindowShapeForInstagram(4); // IGTV Cover Photo
	else if (key == '2')
		sWindowDimensions = setWindowShapeForInstagram(3); // IG Landscape Photo
	else if (key == '3')
		sWindowDimensions = setWindowShapeForInstagram(2); // IG Portrait
	else if (key == '4')
		sWindowDimensions = setWindowShapeForInstagram(0); // IG Story
	else if (key == '5')
		sWindowDimensions = setWindowShapeForInstagram(1); // IG Square
	//else sWindowDimensions = ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight()) + " px";
	
	if(sWindowDimensions != ""){
		ofLogNotice("ofxSurfingHelpersLite::ofxWindowApp::keyPressedToSetWindowShape") << "IG presets";
	}
	return sWindowDimensions;
}

//------

/*
*  ofDrawBitmapStringBox
* 
	A text box widget with 12 layout positions management.
*/

// CONSTANTS
#define SURFING__PAD_TO_WINDOW_BORDERS 8
#define SURFING__STRING_BOX__DEFAULT_XPAD 20
#define SURFING__STRING_BOX__DEFAULT_YPAD 20
#define SURFING__STRING_BOX__DEFAULT_ROUND 4.f // 0.f to not rounded rectangle

enum SURFING_LAYOUT {
	SURFING_LAYOUT_TOP_LEFT = 0,
//	SURFING_LAYOUT_TOP_CENTER,
//	SURFING_LAYOUT_TOP_RIGHT,
//
//	SURFING_LAYOUT_CENTER_LEFT,
//	SURFING_LAYOUT_CENTER,
//	SURFING_LAYOUT_CENTER_RIGHT,
//
//	SURFING_LAYOUT_BOTTOM_LEFT,
//	SURFING_LAYOUT_BOTTOM_CENTER,
//	SURFING_LAYOUT_BOTTOM_RIGHT,

	SURFING_LAYOUT_AMOUNT
};

//--------------------------------------------------------------
inline void ofDrawBitmapStringBox(string s, int x, int y, float round = SURFING__STRING_BOX__DEFAULT_ROUND) {
	bool bdebug = 0;

#ifdef SURFING__STRING_BOX___USE_TTF_INSTEAD_OF_BITMAP_FONT___WIP
	static bool bDone = 0;
	static ofTrueTypeFont font;
	if (!bDone) {
		bDone = 1;
		string path = SURFING__OFXGUI__FONT_DEFAULT_PATH;
		int sz = SURFING__OFXGUI__FONT_DEFAULT_SIZE;
		font.loadFont(path, sz);
	}
#endif

	int a1 = 255;
	int a2 = 200;
	ofColor c1 = ofColor(ofColor::white, a1);
	ofColor c2 = ofColor(ofColor::black, a2);

	int xpad = SURFING__STRING_BOX__DEFAULT_XPAD;
	int ypad = SURFING__STRING_BOX__DEFAULT_YPAD;

	x += xpad;
	y += ypad;

	ofBitmapFont bf;
	auto bb = bf.getBoundingBox(s, 0, 0);

	//fix
	int xoffset = 0;
	int yoffset = 0;

	//xoffset = 0; //TODO
	yoffset = 11; //TODO

	ofRectangle bb1;
	bb1 = { (float)x, (float)y, bb.width, bb.height };

	ofRectangle bb2;
	bb2 = { (float)x - xpad, (float)y - ypad, bb1.width + 2 * xpad, bb1.height + 2 * ypad };

	{
		ofPushStyle();
		ofFill();
		ofSetColor(c2);

		if (round == 0)
			ofDrawRectangle(bb2);
		else
			ofDrawRectRounded(bb2, round);

		ofPopStyle();
	}

#if (bdebug)
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(2);
	ofSetColor(ofColor::yellow);

	ofDrawRectangle(bb1);

	ofSetColor(ofColor::red);
	ofDrawRectangle(bb2);

	ofPopStyle();
#endif

	ofPushStyle();
	ofSetColor(c1);

#ifdef SURFING__STRING_BOX___USE_TTF_INSTEAD_OF_BITMAP_FONT___WIP
	if (font.isLoaded())
		font.drawString(s, x + xoffset, y + yoffset);
	else
		ofDrawBitmapString(s, x + xoffset, y + yoffset);
#else
	ofDrawBitmapString(s, x + xoffset, y + yoffset);
#endif

	ofPopStyle();
}

//--------------------------------------------------------------
inline ofRectangle getBBBitmapStringBox(string s, int x = 0, int y = 0) {
	int xpad = SURFING__STRING_BOX__DEFAULT_XPAD;
	int ypad = SURFING__STRING_BOX__DEFAULT_YPAD;

	x += xpad;
	y += ypad;

	ofRectangle bb;

#ifdef SURFING__STRING_BOX___USE_TTF_INSTEAD_OF_BITMAP_FONT___WIP
	static bool bDone = 0;
	static ofTrueTypeFont font;
	if (!bDone) {
		bDone = 1;
		string path = SURFING__OFXGUI__FONT_DEFAULT_PATH;
		int sz = SURFING__OFXGUI__FONT_DEFAULT_SIZE;
		font.loadFont(path, sz);
	}
	if (font.isLoaded())
		bb = font.getStringBoundingBox(s, 0, 0);
	else {
		ofBitmapFont bf;
		bb = bf.getBoundingBox(s, 0, 0);
	}
#else
	ofBitmapFont bf;
	bb = bf.getBoundingBox(s, 0, 0);
#endif

	ofRectangle bb1;
	bb1 = { (float)x, (float)y, bb.width, bb.height };

	ofRectangle bb2;
	bb2 = { (float)x - xpad, (float)y - ypad, bb1.width + 2 * xpad, bb1.height + 2 * ypad };

	return bb2;
}

////--------------------------------------------------------------
//inline glm::vec2 getBitmapStringBoxPosToCenterLeft(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS) {
//	auto bb = getBBBitmapStringBox(s, 0, 0);
//	int x = xpad;
//	int y = ofGetHeight() / 2 - bb.getHeight() / 2;
//	glm::vec2 p { x, y };
//	return p;
//}
//
////--------------------------------------------------------------
//inline glm::vec2 getBitmapStringBoxPosToCenter(string s) {
//	auto bb = getBBBitmapStringBox(s, 0, 0);
//	int x = ofGetWidth() / 2 - bb.getWidth() / 2;
//	int y = ofGetHeight() / 2 - bb.getHeight() / 2;
//	glm::vec2 p { x, y };
//	return p;
//}
//
////--------------------------------------------------------------
//inline glm::vec2 getBitmapStringBoxPosToCenterRight(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS) {
//	auto bb = getBBBitmapStringBox(s, 0, 0);
//	int x = ofGetWidth() - bb.getWidth() - xpad;
//	int y = ofGetHeight() / 2 - bb.getHeight() / 2;
//	glm::vec2 p { x, y };
//	return p;
//}
//
////--------------------------------------------------------------
//inline glm::vec2 getBitmapStringBoxPosToTopCenter(string s, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
//	auto bb = getBBBitmapStringBox(s, 0, 0);
//	int x = ofGetWidth() / 2 - bb.getWidth() / 2;
//	int y = ypad;
//	glm::vec2 p { x, y };
//	return p;
//}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToTopLeft(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = xpad;
	int y = ypad;
	glm::vec2 p { x, y };
	return p;
}

////--------------------------------------------------------------
//inline glm::vec2 getBitmapStringBoxPosToTopRight(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
//	auto bb = getBBBitmapStringBox(s, 0, 0);
//	int x = ofGetWidth() - bb.getWidth() - xpad;
//	int y = ypad;
//	glm::vec2 p { x, y };
//	return p;
//}
//
////--------------------------------------------------------------
//inline glm::vec2 getBitmapStringBoxPosToBottomLeft(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
//	auto bb = getBBBitmapStringBox(s, 0, 0);
//	int x = xpad;
//	int y = ofGetHeight() - bb.getHeight() - ypad;
//	glm::vec2 p { x, y };
//	return p;
//}
//
////--------------------------------------------------------------
//inline glm::vec2 getBitmapStringBoxPosToBottomCenter(string s, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
//	auto bb = getBBBitmapStringBox(s, 0, 0);
//	int x = ofGetWidth() / 2 - bb.getWidth() / 2;
//	int y = ofGetHeight() - bb.getHeight() - ypad;
//	glm::vec2 p { x, y };
//	return p;
//}
//
////--------------------------------------------------------------
//inline glm::vec2 getBitmapStringBoxPosToBottomRight(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
//	auto bb = getBBBitmapStringBox(s, 0, 0);
//	int x = ofGetWidth() - bb.getWidth() - xpad;
//	int y = ofGetHeight() - bb.getHeight() - ypad;
//	glm::vec2 p { x, y };
//	return p;
//}

//--------------------------------------------------------------
inline string getLayoutName(SURFING_LAYOUT layout) {
	string s;
	switch (layout) {

	case SURFING_LAYOUT_TOP_LEFT:
		s = "TOP LEFT";
		break;
//	case SURFING_LAYOUT_TOP_CENTER:
//		s = "TOP CENTER";
//		break;
//	case SURFING_LAYOUT_TOP_RIGHT:
//		s = "TOP RIGHT";
//		break;
//
//	case SURFING_LAYOUT_CENTER_LEFT:
//		s = "CENTER LEFT";
//		break;
//	case SURFING_LAYOUT_CENTER:
//		s = "CENTER";
//		break;
//	case SURFING_LAYOUT_CENTER_RIGHT:
//		s = "CENTER RIGHT";
//		break;
//
//	case SURFING_LAYOUT_BOTTOM_LEFT:
//		s = "BOTTOM LEFT";
//		break;
//	case SURFING_LAYOUT_BOTTOM_CENTER:
//		s = "BOTTOM CENTER";
//		break;
//	case SURFING_LAYOUT_BOTTOM_RIGHT:
//		s = "BOTTOM RIGHT";
//		break;
//
//		//--
//
//#ifdef SURFING__STRING_BOX__INCLUDE_EXTRA_LAYOUTS
//	case SURFING_LAYOUT_MOUSE_POS:
//		s = "MOUSE POS";
//		break;
//	case SURFING_LAYOUT_MOUSE_POS_CENTER:
//		s = "MOUSE POS C";
//		break;
//#endif

	default:
		s = "NONE";
		break;
	}
	return s;
}
//--------------------------------------------------------------
inline string getLayoutName(int layout) {
	return getLayoutName((SURFING_LAYOUT)layout);
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToLayout(string s, SURFING_LAYOUT layout) {
	glm::vec2 p { 0, 0 };

	if (layout == SURFING_LAYOUT_TOP_LEFT) {
		// top-left
		p = getBitmapStringBoxPosToTopLeft(s);
	}
//	else if (layout == SURFING_LAYOUT_TOP_CENTER) {
//		// top-center
//		p = getBitmapStringBoxPosToTopCenter(s);
//	} else if (layout == SURFING_LAYOUT_TOP_RIGHT) {
//		// top-right
//		p = getBitmapStringBoxPosToTopRight(s);
//	}
//
//	else if (layout == SURFING_LAYOUT_CENTER_LEFT) {
//		// center-left
//		p = getBitmapStringBoxPosToCenterLeft(s);
//	} else if (layout == SURFING_LAYOUT_CENTER) {
//		// center-center
//		p = getBitmapStringBoxPosToCenter(s);
//	} else if (layout == SURFING_LAYOUT_CENTER_RIGHT) {
//		// center-right
//		p = getBitmapStringBoxPosToCenterRight(s);
//	}
//
//	else if (layout == SURFING_LAYOUT_BOTTOM_LEFT) {
//		// bottom-left
//		p = getBitmapStringBoxPosToBottomLeft(s);
//	} else if (layout == SURFING_LAYOUT_BOTTOM_CENTER) {
//		// bottom-center
//		p = getBitmapStringBoxPosToBottomCenter(s);
//	} else if (layout == SURFING_LAYOUT_BOTTOM_RIGHT) {
//		// bottom-right
//		p = getBitmapStringBoxPosToBottomRight(s);
//	}

	//--

//#ifdef SURFING__STRING_BOX__INCLUDE_EXTRA_LAYOUTS
//	else if (layout == SURFING_LAYOUT_MOUSE_POS) {
//		// mouse pos clamped inside the window
//		auto bb = getBBBitmapStringBox(s);
//		int xm = ofGetMouseX();
//		int ym = ofGetMouseY();
//		int x = ofClamp(xm, 0, ofGetWidth() - bb.width);
//		int y = ofClamp(ym, 0, ofGetHeight() - bb.height);
//		p = { x, y };
//	} else if (layout == SURFING_LAYOUT_MOUSE_POS_CENTER) {
//		// mouse pos centered clamped inside the window
//		auto bb = getBBBitmapStringBox(s);
//		int xm = ofGetMouseX() - bb.width / 2;
//		int ym = ofGetMouseY() - bb.height / 2;
//		int x = ofClamp(xm, 0, ofGetWidth() - bb.width);
//		int y = ofClamp(ym, 0, ofGetHeight() - bb.height);
//		p = { x, y };
//	}
//#endif

	return p;
}

//--------------------------------------------------------------
inline ofRectangle getBBBitmapStringBoxToLayout(string s, SURFING_LAYOUT layout) {
	glm::vec2 p = getBitmapStringBoxPosToLayout(s, layout);
	return getBBBitmapStringBox(s, p.x, p.y);
}
//--------------------------------------------------------------
inline ofRectangle getBBBitmapStringBoxToLayout(string s, int layout) {
	return getBBBitmapStringBoxToLayout(s, (SURFING_LAYOUT)layout);
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToLayout(string s, int layout) {
	return getBitmapStringBoxPosToLayout(s, (SURFING_LAYOUT)layout);
}

//--------------------------------------------------------------
inline void ofDrawBitmapStringBox(string s, SURFING_LAYOUT layout = SURFING_LAYOUT_TOP_LEFT) {
	glm::vec2 p = getBitmapStringBoxPosToLayout(s, layout);
	ofDrawBitmapStringBox(s, p.x, p.y);
}
//--------------------------------------------------------------
inline void ofDrawBitmapStringBox(string s, int layout /* = 0*/) {
	ofDrawBitmapStringBox(s, (SURFING_LAYOUT)layout);
}

/*
	EXAMPLES

	// simple
	{
		ofxSurfing::ofDrawBitmapStringBox(s, layout);//centered
	}
	
	// debug fps
	{
		float fps = ofGetFrameRate();
		string s = ofToString(fps, 1);
		s += " FPS";
		ofxSurfing::ofDrawBitmapStringBox(s, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);
	}

	// browse layouts
	{
		keyPressed(int key) 
		if (key == 'l') {
			helpLayout++;
			helpLayout = helpLayout % ((int)ofxSurfing::SURFING_LAYOUT_AMOUNT + 1);
			buildHelp();
		}
	}
*/

//------

};
};
