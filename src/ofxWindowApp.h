#pragma once

/*
	TODO:
	- add vars/ofRectangle for aux window pos/sz to avoid startup moves...
	- fps plot graph.
	- add ofxScreenSetup / ofxNative addons to bundle other window/OS features.
	- add unlock full fps / fps=0 toggle.
	- add fullscreen/window and other bool ofParams to expose to a control ui.
	- fix force setFrameRate ofSetVerticalSync calls in setup workflow. currently requires app restart.
 */

//----

#define SURFING_WINDOW_APP__USE_STATIC
// <- Main directive
// Uncomment (enable) to allow WIN32 platform for probably better windowMove callback.

//----

//TODO: Fixing exceptions hen closing ofApp.
//#define SURFING_WINDOW_APP__CREATE_EXIT_LISTENER // to enable that ofApp exit will call exit and save settings.
//#define SURFING_WINDOW_APP__ENABLE_SAVE_ON_EXIT // to enable auto save on exit.

#define SURFING_WINDOW_APP__USE_TIMED_SAVER // would be force disabled on Windows platform in static mode.

//----

#include "ofMain.h"

#if defined(TARGET_WIN32) && defined(SURFING_WINDOW_APP__USE_STATIC)
	#include <GLFW/glfw3.h>
	#if defined(SURFING_WINDOW_APP__USE_TIMED_SAVER)
		#undef SURFING_WINDOW_APP__USE_TIMED_SAVER //force disable
	#endif
#else
	#ifndef SURFING_WINDOW_APP__USE_TIMED_SAVER
		#define SURFING_WINDOW_APP__USE_TIMED_SAVER
	#endif
#endif

#define SIZE_SECURE_GAP_INISDE_SCREEN 18 // to avoid that window border it's outside screen monitor
#define OFX_WINDOW_APP_BAR_HEIGHT 45 // probably fits on Win32 only.

//#define SURFING_USE_STAY_ON_TOP

//--

#include "ofxSerializer_ofxWindowApp.h" //for windows size/pos serialization
#include "ofxSurfingHelpersLite_ofxWindowApp.h" //for some windows sizes keycommands presets

//--------

#define USING__OFX_WINDOW_APP

class ofxWindowApp {
public:
	ofxWindowApp();
	~ofxWindowApp();

public:
	void setup();

	//----

#ifdef SURFING_WINDOW_APP__USE_STATIC
public:
	void setup(ofxWindowApp * app) {
		ofLogNotice("ofxWindowApp:setup(ofxWindowApp * app)") << "at frameNum: " << ofGetFrameNum();
		this->setInstance(app);
		setup();
	}

private:
	static void windowMoved(GLFWwindow * window, int xpos, int ypos);
	bool bDisableCallback_windowMovedOrResized = false;

private:
	static ofxWindowApp * instance; // Static pointer to hold the instance

public:
	static void setInstance(ofxWindowApp * app); // Static function to set the instance
#endif

	//----

private:
	bool bDoneSetup = false;

private:
	void startup();
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void exit();

private:
	void keyPressed(ofKeyEventArgs & eventArgs);
	void keyReleased(ofKeyEventArgs & eventArgs);

	void windowResized(int w, int h);
	void windowResized(ofResizeEventArgs & e);

	//--

private:
	ofWindowSettings windowSettings;

	ofTrueTypeFont font;
	int fontSize;

	//--

private:
	// Modifiers
	bool mod_COMMAND = false;
	bool mod_CONTROL = false;
	bool mod_ALT = false;
	bool mod_SHIFT = false;

	bool bFlagToSave = false;

	bool bFlagShowFeedbackDoneSaved = false;

#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
	float timeWhenToSaveFlag;
#endif

	//--

	void drawInfo();
	void drawInfoPerformanceWidget();

	//----

	// API METHODS TO FORCE CONFIG

public:
	// Setters
	// required when used for first time into your project and no JSON file settings created yet.
	// Default will be 60 fps if not defined!
	//--------------------------------------------------------------
	void setFrameRate(float f) {
		fpsTarget = f;
		ofSetFrameRate(fpsTarget);
	}

	//--------------------------------------------------------------
	void setVerticalSync(bool b) {
		vSync = b;
		ofSetVerticalSync(vSync);
	}

	////--------------------------------------------------------------
	//void setFullScreen(bool b) {
	//	if (b) {
	//		ofSetFullscreen(true);
	//		bIsFullScreen = true;
	//	} else {
	//		ofSetFullscreen(false);
	//		bIsFullScreen = false;

	//		//float windowBar_h = 25;
	//		//window_Y = MAX(ofGetWindowPositionY(), windowBar_h);
	//		//// avoid negative out of screen. minimal h is 25
	//		//window_X = ofGetWindowPositionX();
	//		//ofSetWindowPosition(window_X, window_Y);
	//	}

	//	// Update
	//	windowResized(ofGetWidth(), ofGetHeight());
	//}

	//--

	//--------------------------------------------------------------
	void setEnableAllowQuitAppUsingEscapeKey(bool b) {
		ofLogNotice("ofxWindowApp:setEnableAllowQuitAppUsingEscapeKey");
		ofSetEscapeQuitsApp(b);
	}

	//--

#ifdef SURFING_USE_STAY_ON_TOP
	//--------------------------------------------------------------
	void setToggleStayOnTop() {
		ofLogNotice("ofxWindowApp:setToggleStayOnTop");
		setStayOnTop(!bIsWindowStayOnTop);
	}

	//--------------------------------------------------------------
	void setStayOnTop(bool b) {
		ofLogNotice("ofxWindowApp:setStayOnTop") << b;
		bWindowStayOnTop = b;
	}

private:
	bool bIsWindowStayOnTop = false;
#endif

	//--

public:
	//TODO
	// Taken from https://github.com/kritzikratzi/ofxNative/blob/master/src/ofxNative_win.cpp
	//--------------------------------------------------------------
	void setConsoleVisible(bool show) {
#if defined(TARGET_WIN32)
		::ShowWindow(::GetConsoleWindow(), show ? SW_SHOW : SW_HIDE);
#endif
	}
	//--------------------------------------------------------------
	bool getConsoleVisible() {
		return (::IsWindowVisible(::GetConsoleWindow()) != FALSE);
	}

	//--

private:
	void doGetWindowSettings();
	void saveSettings(bool bSlient = false);

public:
	void save(); // alias and public API

private:
	void saveSettingsAfterRefresh();
	void loadSettings();

private:
	void doApplyWindowSettings();

	//--

public:
	//string getPathFolder() const {
	//	return path_folder;
	//}
	//string getPathSettings() const {
	//	string p = path_folder + "/" + path_filename;
	//	return p;
	//}

	////--------------------------------------------------------------
	//void setPathFolder(string s) {
	//	path_folder = s;
	//}

	void folderCheckAndCreate(string _path);

	////--------------------------------------------------------------
	//void setPathFilename(string s) {
	//	path_filename = s;
	//}

	//--

	////--------------------------------------------------------------
	//void setAutoSaveLoad(bool b) {
	//	bAutoSaveLoad = b;
	//}

	////--------------------------------------------------------------
	//void setShowDebug(bool b = true) {
	//	bShowInfo = b;
	//	//bShowInfoPerformanceAlways = b;
	//}
	////--------------------------------------------------------------
	//void setShowDebugPerformance(bool b = true) {
	//	bShowInfoPerformanceAlways = b;
	//}
	////--------------------------------------------------------------
	//bool getShowDebug() {
	//	return bShowInfo;
	//}
	////--------------------------------------------------------------
	//void toggleShowDebug() {
	//	bShowInfo = !bShowInfo;
	//}

	//--------------------------------------------------------------
	void setShowPerformanceAlways(bool b = true) { //will display alert drop fps info ven when debug display disabled
		bShowInfoPerformanceAlways = b;
	}

	//--

	// Easy callback only to check from parent scope/ofApp if window shape has changed.
	//--------------------------------------------------------------
	bool isChanged() {
		if (bFlagIsChanged) {
			bFlagIsChanged = false;
			return true;
		}

		else
			return false;
	}

	//--

private:
	void doApplyWindowExtraSettings(); // fps and vsync only

public:
	// Layout modes
	enum DEBUG_Position {
		DEBUG_POSITION_TOP = 0,
		DEBUG_POSITION_BOTTOM
	};

private:
	int positionLayout = DEBUG_POSITION_BOTTOM;

public:
	void setPositionDebugInfo(int POS) {
		positionLayout = POS;
	}

	void setPositionDebugInfo(DEBUG_Position POS = DEBUG_POSITION_BOTTOM) {
		positionLayout = POS;
	}

	int getDEBUG_Position() {
		return positionLayout;
	}

	void togglePositionDebugInfo() {
		setPositionDebugInfo((positionLayout == 1) ? 0 : 1);
	}

	void setEnableKeys(bool b) {
		bKeys = b;
	}

	//--

	//public:
	//	float getFrameRate() {
	//		return fpsTarget.get();
	//	}
	//
	//	bool getVerticalSync() {
	//		return vSync.get();
	//	}

	//--

	// Disables saving. So json file will not be overwritten.
public:
	void setLock(bool b) {
		bDisableAutoSave = b;
	}

	//--

#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
public:
	void setEnableTimerSaver(bool b = true) {
		bAutoSaverTimed = b;
	}

	void setTimerSaver(int t) {
		timePeriodToCheckIfSave = t;
	}

private:
	//TODO:
	bool bAutoSaverTimed = true;
	//bool bAutoSaverTimed = false;

	int timePeriodToCheckIfSave = 10000; //every x/1000 seconds
	uint32_t timeLastAutoSaveCheck = 0;
#endif

	//--

private:
	// This is to folder all files to avoid mixing with other add-ons data
	string path_folder;
	string path_filename;

	bool bAutoSaveLoad = true; // load at startup
	//TODO: is disabled but still saving on exit?

	bool bFlagIsChanged = false; // to check if window moved or resized
	bool bKeys = true; // keys enabled by default

	//----

	// Params

	void setupParams();
	//ofParameterGroup params{ "ofxWindowApp" };//TODO: avoid ofxSerializer. use one only groupParam instead.

	ofParameterGroup paramsExtra { "Extra" };
	ofParameterGroup paramsWindow { "Window" };
	ofParameterGroup paramsSession { "Session" };

	ofParameter<bool> vSync { "vSync", false };
	ofParameter<float> fpsTarget { "FpsTarget", 60.f, 1.f, 144.f };
	ofParameter<bool> bShowInfoPerformanceAlways { "ShowDebugPerformance", true };

	ofParameter<bool> bDisableAutoSave { "DisableAutosave", false };
	// Ignores next window modification.
	// Kind of hardcoded position that will maintain on next app load.

#ifdef SURFING_USE_STAY_ON_TOP
public:
	ofParameter<bool> bWindowStayOnTop { "WindowOnTop", false };
#endif

private:
	ofParameter<bool> bShowInfo { "ShowDebugInfo", true };

	//----

public:
	bool bShowDebug = false;

private:
	bool bDoneStartup = false;

private:
	void Changed_ParamsExtra(ofAbstractParameter & e);

	float fpsReal;

	int previewPad = 8;
	int previewX = previewPad / 2;
	int previewY = 0;

	bool bIsFullScreen = false;

	glm::vec2 posSettings = glm::vec2(0);
	glm::vec2 sizeSettings = glm::vec2(0);

	void doApplyToggleWindowMode();

private:
	//--------------------------------------------------------------
	void doResetWindowExtraSettings() {
		ofLogNotice("ofxWindowApp") << "doResetWindowExtraSettings()";
		// Default settings
		vSync = false;
		fpsTarget = 60;
		bShowInfo = false;
		bDisableAutoSave = false;
		bShowInfoPerformanceAlways = true;
	}

private:
	//--------------------------------------------------------------
	void doResetWindowSettings() {
		ofLogNotice("ofxWindowApp") << "doResetWindowSettings()";

		// window mode
		windowSettings.windowMode = ofWindowMode(0);

		// shape
		int w = 1280;
		int h = w * (9.f / 16.f);
		windowSettings.setSize(w, h);

		windowSettings.setPosition(glm::vec2(2, OFX_WINDOW_APP_BAR_HEIGHT)); // left top
		//windowSettings.setPosition(glm::vec2(ofGetWidth() / 2.f - w / 2.f, ofGetHeight() / 2.f - h / 2.f )); // centered fails
	}

	void drawDebug();

	//--------------------------------------------------------------

	// debug system monitors
private:
	void checkMonitors();
	void drawDebugSystemMonitors();
	vector<ofRectangle> monitorRects;
	vector<std::string> monitorNames;
	ofRectangle monitorsCanvasRect;

	//----

	// debug
	void logSettings() {
		ofLogNotice("ofxWindowApp") << "----------------------logSettings()";
		ofLogNotice("ofxWindowApp") << "> ofGetWindow";
		ofLogNotice("ofxWindowApp") << "Position: "
									<< ofToString(ofGetWindowPositionX()) << ","
									<< ofToString(ofGetWindowPositionY());
		ofLogNotice("ofxWindowApp") << "Size:     "
									<< ofToString(ofGetWindowWidth()) << "x"
									<< ofToString(ofGetWindowHeight());
		ofLogNotice("ofxWindowApp") << "> windowSettings";
		ofLogNotice("ofxWindowApp") << "OF_WINDOW/OF_FULLSCREEN/OF_GAME_MODE";
		ofLogNotice("ofxWindowApp") << "WindowMode:" << ofToString(windowSettings.windowMode);
		ofLogNotice("ofxWindowApp") << "Position:  " << ofToString(windowSettings.getPosition());
		ofLogNotice("ofxWindowApp") << "Size:      "
									<< ofToString(windowSettings.getWidth())
									<< "x" << ofToString(windowSettings.getHeight());
		ofLogNotice("ofxWindowApp") << "----------------------";
		ofLogNotice("ofxWindowApp");
	}
};




//// workaround:
//// to fit window and his bar visible into the screen
//float windowBar_h = 25;

//// workaround
//// it's window mode..
//// kick a little down to avoid hidden window title barF
//window_Y = MAX(ofGetWindowPositionY(), windowBar_h); //avoid negative out of screen. minimal h is 25
//window_X = ofGetWindowPositionX();
//ofSetWindowPosition(window_X, window_Y);
