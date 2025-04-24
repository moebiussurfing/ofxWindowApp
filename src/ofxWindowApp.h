#pragma once
#include "ofMain.h"

/*
	TODO:
	- 
	- add timer mode to reduce writes
	- check listen when file changes. allowing edit json externally and auto update the app window. https://github.com/nariakiiwatani/ofxWatchFile
	- fix force setFrameRate ofSetVerticalSync calls in setup workflow. currently requires app restart. set->save->apply
	- add mode to unlock full fps / fps=0 toggle.
	- fps plot graph.
	- add ofxScreenSetup / ofxNative addons to bundle other window/OS features.
	- add fullscreen/window and other bool ofParams to expose to a control ui.
 */

//----

#define OFX_WINDOW_APP__USE_STATIC
// Main directive. Uncomment/enable enables this new more secure approach using a static instance.
//TODO: Can't be disabled now. Should check and implement other approaches.

//----

//#define OFX_WINDOW_APP__ENABLE_SAVE_ON_EXIT // To enable auto save on exit.
// This approach, sometimes required fixing exceptions when closing ofApp.

//#define OFX_WINDOW_APP__USE_TIMED_SAVER // A super simple timed saver every second if changed. TODO: Not recommended now.

#define OFX_WINDOW_APP__BAR_HEIGHT 45 // Probably fits on Windows platform only.

//#define OFX_WINDOW_APP__SIZE_SECURE_GAP_INISDE_SCREEN 18 //TODO: To avoid that window border it's outside screen monitor.

#define OFX_WINDOW_APP__USE_STAY_ON_TOP // Allow an extra OS mode.

//#define OFX_WINDOW_APP__DEVELOP_DEBUG // Enable some more deep testing displaying info.

//----

#include <GLFW/glfw3.h> // Required for displays debug and window callbacks.

//TODO: Should add more approaches if we find some problems:
// - Too many files write during window changes (requires delayed saving)
// - Destruction app exception qhen we save on exit.

//#if defined(OFX_WINDOW_APP__USE_STATIC)
//	#if defined(OFX_WINDOW_APP__USE_TIMED_SAVER)
//		#undef OFX_WINDOW_APP__USE_TIMED_SAVER // Force disable
//	#endif
//#else
//	#ifndef OFX_WINDOW_APP__USE_TIMED_SAVER
//		#define OFX_WINDOW_APP__USE_TIMED_SAVER // Force enable
//	#endif
//#endif

//--

#include "ofxSerializer_ofxWindowApp.h" // For windows size/pos serialization
#include "ofxSurfingHelpersLite_ofxWindowApp.h" // For some windows sizes keycommands presets

//--------

#define USING__OFX_WINDOW_APP

class ofxWindowApp {
public:
	ofxWindowApp();
	~ofxWindowApp();

public:
	void setup();

	//----

#ifdef OFX_WINDOW_APP__USE_STATIC
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
	ofWindowSettings windowSettings; // Main storage for window shape (Position/Size) and window mode (window/fullscreen)

	ofTrueTypeFont font;
	int fontSize;

	//--

private:
	// Modifiers
	bool mod_COMMAND = false;
	bool mod_CONTROL = false;
	bool mod_ALT = false;
	bool mod_SHIFT = false;

#ifdef OFX_WINDOW_APP__USE_TIMED_SAVER
	float timeWhenToSaveFlag;
#endif

	//--

	void drawInfo();
	void drawInfoPerformanceWidget();

	//----

	// API METHODS TO FORCE CONFIG

public:
	// Setters
	// Required when used for first time into your project and no JSON file settings created yet.
	// Default will be 60 fps if not defined!
	//--------------------------------------------------------------
	void setFrameRate(float f) {
		fpsTarget = f;
		ofSetFrameRate(fpsTarget);
//		saveSettings();//TODO: Warning! will trig undesired save before loading the settings!
	}

	//--------------------------------------------------------------
	void setVerticalSync(bool b) {
		bvSync = b;
		ofSetVerticalSync(bvSync);
//		saveSettings();//TODO: Warning! will trig undesired save before loading the settings!
	}
	
private://TODO: Warning! will trig undesired save before loading the settings!
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
		saveSettings();
	}
	
	// Disables auto saving. So json file will not be overwritten and reloaded as is in the next app session.
	//--------------------------------------------------------------
	void setDisableAutoSave(bool b) {
		bDisableAutoSave = b;
		saveSettings();
	}

	//--

	//--------------------------------------------------------------
	void setShowPerformanceAlways(bool b = true) { // Will display alert drop fps info ven when debug display disabled
		bShowInfoPerformanceAlways = b;
		saveSettings();
	}

	//--
	
public:
	//--------------------------------------------------------------
	void setEnableAllowQuitAppUsingEscapeKey(bool b) {
		ofLogNotice("ofxWindowApp:setEnableAllowQuitAppUsingEscapeKey");
		ofSetEscapeQuitsApp(b);
	}

	//--

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	//--------------------------------------------------------------
	void setToggleStayOnTop() {
		ofLogNotice("ofxWindowApp:setToggleStayOnTop");
		setStayOnTop(!bWindowStayOnTop);
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
//	//TODO: handle console terminal window
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

	void saveSettings(bool bSlient = false); // Main save method
	void loadSettings(); // main save method

public:
	void save(); // Alias method and public for API external usage

private:
	void saveSettingsAfterRefresh(); // Refresh update before save

	//--

private:
	// Path settings
	// This is to folder all files to avoid mixing with other add-ons data
	string path_folder;
	string path_filename;

	void folderCheckAndCreate(string _path); // Check if required create folder or already exist

//public:
	//// Custom path
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
	
	//--
	
	void setEnableKeys(bool b) {
		bKeys = b;
	}
	
	void setToggleEnableKeys() {
		bKeys = !bKeys;
	}

	//--

#ifdef OFX_WINDOW_APP__USE_TIMED_SAVER
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

	int timePeriodToCheckIfSave = 10000; //every 1000 ms
	uint32_t timeLastAutoSaveCheck = 0;
#endif

	//--

private:
	bool bKeys = true; // Enable keys by default

	bool bDoneSetup = false; // Flags that setup() is done
	bool bDoneStartup = false; // Flags that startup() is done

	bool bDisableCallback_windowMovedOrResized = false;

	bool bFlagToSave = false; // Flags and listen in update to do the save
	bool bFlagIsChanged = false; // Flags for feedback to check if window moved or resized
	bool bFlagShowFeedbackDoneSaved = false; // Flags for feedback to know that settings has been saved

	bool bIsFullScreen = false; // Only to feedback display info

	// Layout text boxes
	int previewPad = 8;
	int previewX = previewPad / 2;
	int previewY = 0;

	float fpsReal; // Current readed fps to display debug only

public:
	bool bShowDebug = false; // Show debug text box

	//----

	// Params

	void setupParams();

	//ofParameterGroup params{ "ofxWindowApp" };
	//TODO: To avoid ofxSerializer. Use one only groupParam instead.

	ofParameterGroup paramsExtra { "Extra" };
	ofParameterGroup paramsWindow { "Window" };
	ofParameterGroup paramsSession { "Session" };

	ofParameter<bool> bvSync { "vSync", false };
	ofParameter<float> fpsTarget { "FpsTarget", 60.f, 1.f, 144.f };
	ofParameter<bool> bShowInfo { "ShowInfo", true };
	ofParameter<bool> bShowInfoPerformanceAlways { "ShowInfoPerformanceAlways", true };
	ofParameter<bool> bDisableAutoSave { "DisableAutoSave", false };
	// Ignores next window modification.
	// Kind of hardcoded position that will maintain on next app load.

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
//#ifdef TARGET_WIN32
public:
	ofParameter<bool> bWindowStayOnTop { "WindowOnTop", false };
//#endif
#endif
	
private:

	//----

private:
	void ChangedParamsExtra(ofAbstractParameter & e);

private:
	//--------------------------------------------------------------
	void doResetWindowExtraSettings() {
		ofLogNotice("ofxWindowApp:doResetWindowExtraSettings()");

		// Default settings
		bvSync = false;
		fpsTarget = 60;
		bShowInfo = false;
		bDisableAutoSave = false;
		bShowInfoPerformanceAlways = true;
	}

private:
	//--------------------------------------------------------------
	void doResetWindowSettings() {
		ofLogNotice("ofxWindowApp:doResetWindowSettings()");

		// Window mode
		windowSettings.windowMode = ofWindowMode(0);
		bIsFullScreen = false;

		// Shape
		int w = 1280;
		int h = w * (9.f / 16.f);
		windowSettings.setSize(w, h);

		windowSettings.setPosition(glm::vec2(2, OFX_WINDOW_APP__BAR_HEIGHT)); // Left top
		//windowSettings.setPosition(glm::vec2(ofGetWidth() / 2.f - w / 2.f, ofGetHeight() / 2.f - h / 2.f )); // Centered fails...
	}

	void drawDebug();

	void windowChanged(); // Group redirect all other callbacks related to window resized and window moved
	//bool bFlagWindowChanged = false; // Flags workaround trying to call apply-settings to app window methods but delayed

	//--------------------------------------------------------------

	// Debug system monitors
private:
	void checkMonitors(); // Scan
	void drawDebugSystemMonitors(); // Display info
	vector<ofRectangle> monitorRects;
	vector<std::string> monitorNames;
	ofRectangle monitorsCanvasRect;

	//----

	// Debug
	void logSettings() {
		ofLogVerbose("ofxWindowApp:logSettings()") << "----------------------logSettings() <--BEGIN";
		ofLogVerbose("ofxWindowApp:logSettings()") << "FrameNum: " << ofGetFrameNum();
		ofLogVerbose("ofxWindowApp:logSettings()");
		ofLogVerbose("ofxWindowApp:logSettings()") << "> ofGetWindow()"; // Window itself
		ofLogVerbose("ofxWindowApp:logSettings()") << "WindowMode: " << ofToString(ofGetWindowMode());
		ofLogVerbose("ofxWindowApp:logSettings()") << "Position:   "
												  << ofToString(ofGetWindowPositionX()) << ","
												  << ofToString(ofGetWindowPositionY());
		ofLogVerbose("ofxWindowApp:logSettings()") << "Size:       "
												  << ofToString(ofGetWindowWidth()) << "x"
												  << ofToString(ofGetWindowHeight());
		ofLogVerbose("ofxWindowApp:logSettings()");
		ofLogVerbose("ofxWindowApp:logSettings()") << "> windowSettings"; // Stored settings
		ofLogVerbose("ofxWindowApp:logSettings()") << "WindowMode: " << ofToString(windowSettings.windowMode);
		ofLogVerbose("ofxWindowApp:logSettings()") << "Position:   " << ofToString(windowSettings.getPosition());
		ofLogVerbose("ofxWindowApp:logSettings()") << "Size:       "
												  << ofToString(windowSettings.getWidth())
												  << "x" << ofToString(windowSettings.getHeight());
		ofLogVerbose("ofxWindowApp:logSettings()") << "----------------------logSettings()--> END";
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

//#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
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
