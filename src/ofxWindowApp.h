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
		ofLogNotice("ofxWindowApp:setup(ofxWindowApp * app)") << "FrameNum: " << ofGetFrameNum();
		this->setInstance(app);
		setup();
	}

private:
	static void windowMoved(GLFWwindow * window, int xpos, int ypos);

private:
	static ofxWindowApp * instance; // Static pointer to hold the instance

public:
	static void setInstance(ofxWindowApp * app); // Static function to set the instance
#endif

	//----

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
	ofWindowSettings windowSettings; // main storage for window shape and window mode

	ofTrueTypeFont font;
	int fontSize;

	//--

private:
	// Modifiers
	bool mod_COMMAND = false;
	bool mod_CONTROL = false;
	bool mod_ALT = false;
	bool mod_SHIFT = false;

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
		saveSettings();
	}

	//--------------------------------------------------------------
	void setVerticalSync(bool b) {
		vSync = b;
		ofSetVerticalSync(vSync);
		saveSettings();
	}

	//--------------------------------------------------------------
	void setFullScreen(bool b) {
		if (b) {
			ofSetFullscreen(true);
			bIsFullScreen = true;
		} else {
			ofSetFullscreen(false);
			bIsFullScreen = false;
		}
		doSetWindowSettingsFromAppWindow();
	}

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

//public:
//	//TODO
//	// Taken from https://github.com/kritzikratzi/ofxNative/blob/master/src/ofxNative_win.cpp
//	//--------------------------------------------------------------
//	void setConsoleVisible(bool show) {
//#if defined(TARGET_WIN32)
//		::ShowWindow(::GetConsoleWindow(), show ? SW_SHOW : SW_HIDE);
//#endif
//	}
//	//--------------------------------------------------------------
//	bool getConsoleVisible() {
//		return (::IsWindowVisible(::GetConsoleWindow()) != FALSE);
//	}

	//--

private:
	// Sync/apply states methods
	void doSetWindowSettingsFromAppWindow();
	void doApplyToggleWindowMode();
	void doApplyWindowExtraSettings();
	void doApplyWindowSettings();

	void saveSettings(bool bSlient = false); // main save method
	void loadSettings(); // main save method

public:
	void save(); // alias and public API

private:
	void saveSettingsAfterRefresh(); // refresh update before save

	//--

private:
	// Path settings
	// This is to folder all files to avoid mixing with other add-ons data
	string path_folder;
	string path_filename;

	void folderCheckAndCreate(string _path); // check if required create folder or already exist

//public:
	////--------------------------------------------------------------
	//void setPathFolder(string s) {
	//	path_folder = s;
	//}

	////--------------------------------------------------------------
	//void setPathFilename(string s) {
	//	path_filename = s;
	//}

	//--

public:
	// Disables auto saving. So json file will not be overwritten and reloaded as is in the next app session.
	void setDisableAutoSaveLock(bool b) {
		bDisableAutoSaveLock = b;
		saveSettings();
	}

	//--------------------------------------------------------------
	void setAutoLoad(bool b) {
		bAutoLoad = b;
	}

	//--

	//--------------------------------------------------------------
	void setShowPerformanceAlways(bool b = true) { //will display alert drop fps info ven when debug display disabled
		bShowInfoPerformanceAlways = b;
		saveSettings();
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

	void doTogglePositionDisplayInfo() {
		setPositionDebugInfo((positionLayout == 1) ? 0 : 1);
	}

	void setEnableKeys(bool b) {
		bKeys = b;
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
	bool bAutoLoad = true; // load at startup
	bool bKeys = true; // keys enabled by default

	bool bDoneSetup = false;
	bool bDoneStartup = false;

	bool bDisableCallback_windowMovedOrResized = false;

	bool bFlagToSave = false; // listen in update to do the save
	bool bFlagIsChanged = false; // feedback to check if window moved or resized
	bool bFlagShowFeedbackDoneSaved = false; // feedback to know settings has been saved

	bool bIsFullScreen = false; // only to feedback display info

	// Layout text boxes
	int previewPad = 8;
	int previewX = previewPad / 2;
	int previewY = 0;

	float fpsReal; // current fps to display debug only

public:
	bool bShowDebug = false; // show text debug box

	//----

	// Params

	void setupParams();

	//ofParameterGroup params{ "ofxWindowApp" };//TODO: avoid ofxSerializer. use one only groupParam instead.

	ofParameterGroup paramsExtra { "Extra" };
	ofParameterGroup paramsWindow { "Window" };
	ofParameterGroup paramsSession { "Session" };

	ofParameter<bool> vSync { "vSync", false };
	ofParameter<float> fpsTarget { "FpsTarget", 60.f, 1.f, 144.f };
	ofParameter<bool> bShowInfo { "ShowDebugInfo", true };
	ofParameter<bool> bShowInfoPerformanceAlways { "ShowDebugPerformance", true };
	ofParameter<bool> bDisableAutoSaveLock { "DisableAutosaveLock", false };
	// Ignores next window modification.
	// Kind of hardcoded position that will maintain on next app load.

#ifdef SURFING_USE_STAY_ON_TOP
public:
	ofParameter<bool> bWindowStayOnTop { "WindowOnTop", false };
#endif

private:

	//----

private:
	void Changed_ParamsExtra(ofAbstractParameter & e);

private:
	//--------------------------------------------------------------
	void doResetWindowExtraSettings() {
		ofLogNotice("ofxWindowApp") << "doResetWindowExtraSettings()";

		// Default settings
		vSync = false;
		fpsTarget = 60;
		bShowInfo = false;
		bDisableAutoSaveLock = false;
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

	void windowChanged();

	//--------------------------------------------------------------

	// Debug system monitors
private:
	void checkMonitors();
	void drawDebugSystemMonitors();
	vector<ofRectangle> monitorRects;
	vector<std::string> monitorNames;
	ofRectangle monitorsCanvasRect;

	//----

	// debug
	void logSettings() {
		ofLogNotice("ofxWindowApp:logSettings()") << "----------------------logSettings() <--BEGIN";
		ofLogNotice("ofxWindowApp:logSettings()") << "FrameNum: " << ofGetFrameNum();
		ofLogNotice("ofxWindowApp:logSettings()");
		ofLogNotice("ofxWindowApp:logSettings()") << "> ofGetWindow()";
		ofLogNotice("ofxWindowApp:logSettings()") << "WindowMode: " << ofToString(ofGetWindowMode());
		ofLogNotice("ofxWindowApp:logSettings()") << "Position:   "
												  << ofToString(ofGetWindowPositionX()) << ","
												  << ofToString(ofGetWindowPositionY());
		ofLogNotice("ofxWindowApp:logSettings()") << "Size:       "
												  << ofToString(ofGetWindowWidth()) << "x"
												  << ofToString(ofGetWindowHeight());
		ofLogNotice("ofxWindowApp:logSettings()");
		ofLogNotice("ofxWindowApp:logSettings()") << "> windowSettings";
		ofLogNotice("ofxWindowApp:logSettings()") << "WindowMode: " << ofToString(windowSettings.windowMode);
		ofLogNotice("ofxWindowApp:logSettings()") << "Position:   " << ofToString(windowSettings.getPosition());
		ofLogNotice("ofxWindowApp:logSettings()") << "Size:       "
												  << ofToString(windowSettings.getWidth())
												  << "x" << ofToString(windowSettings.getHeight());
		ofLogNotice("ofxWindowApp:logSettings()") << "----------------------logSettings()--> END";
	}
};


//------

// NOTES

//--

//// workaround:
//// to fit window and his bar visible into the screen
//float windowBar_h = 25;

//// workaround
//// it's window mode..
//// kick a little down to avoid hidden window title barF
//window_Y = MAX(ofGetWindowPositionY(), windowBar_h); //avoid negative out of screen. minimal h is 25
//window_X = ofGetWindowPositionX();
//ofSetWindowPosition(window_X, window_Y);

//--

//#ifdef SURFING_USE_STAY_ON_TOP
//	// On top
//	if (!bIsFullScreen) {
//		// Workaround
//		// Refresh
//	#if defined(TARGET_WIN32)
//		HWND W = GetActiveWindow();
//		SetWindowPos(W, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
//	#endif
//		// Re trig
//		bWindowStayOnTop = bWindowStayOnTop;
//	}
//#endif

//float windowBar_h = 25;
//		//window_Y = MAX(ofGetWindowPositionY(), windowBar_h);
//		//// avoid negative out of screen. minimal h is 25
//		//window_X = ofGetWindowPositionX();
//		//ofSetWindowPosition(window_X, window_Y);
