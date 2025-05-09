#pragma once
#include "ofMain.h"

/*
	TODO:
	- add timer mode to reduce writes. ?
	- fps plot graph. ?
	- add ofxScreenSetup/ofxNative addons to bundle other window/OS features.
	- add fullscreen/window and other bool ofParams to expose to a control ui.
	- Should add more approaches if we find some problems:
	- Too many files write during window changes (requires delayed saving)
	- Destruction app exception trigs exit then we save when closing app.
 */

//----

//#define OFX_WINDOW_APP__USE_OFX_WATCHER // <- (OPTIONAL) Enable file watcher to reload JSON file when changed:
// Requires https://github.com/nariakiiwatani/ofxWatcher and to enable the above directive.

//#define OFX_WINDOW_APP__DEVELOP_DEBUG // <- (OPTIONAL) Enable for more deep testing debug info.

//----

#define OFX_WINDOW_APP__USE_STATIC
// Main directive. Uncomment/enable enables this new more secure approach using a static instance.
//TODO: Can't be disabled now. Should check and implement other approaches.

//----

//#define OFX_WINDOW_APP__ENABLE_SAVE_ON_EXIT
// To enable force autosave on app exit.
// This approach, sometimes required fixing exceptions when closing ofApp.

//#define OFX_WINDOW_APP__USE_TIMED_SAVER
// A super simple timed saver every second if changed. TODO: Not recommended now.

#define OFX_WINDOW_APP__USE_STAY_ON_TOP // <- (OPTIONAL)  Allows an extra OS mode.
#define OFX_WINDOW_APP__USE_CONSOLE_WINDOW // <- (OPTIONAL)  Allows an extra OS mode.

//--

//#define OFX_WINDOW_APP__FORCE_RESET_WINDOW_IF_ITS_OUT_OF_DESKTOP_CANVAS // TODO: Should be improved..
#define OFX_WINDOW_APP__BAR_HEIGHT 45 // TODO: Probably fits on Windows platform only.
//#define OFX_WINDOW_APP__SIZE_SECURE_GAP_INISDE_SCREEN 18 //TODO: To avoid that window border it's outside screen monitor.

//----

#include <GLFW/glfw3.h> // Required for displays debug and window callbacks.

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
	//--------------------------------------------------------------
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
	void startup(); // Will load startup file settings here
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

#ifdef OFX_WINDOW_APP__USE_OFX_WATCHER
	// Workaround trick:
	// We need to ignore that file changed doing when saving.
	// ie: when we move the window, a save will be trigged, and the file will change,
	// but we will ignore that change.. We want to listen changes doing externally.
	// So if fille changed during a manually window moving could produce unsync file settings.
	bool bDisableCallback_FileChanged = false;
	uint64_t tLastSave = 0;
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
		ofLogNotice("ofxWindowApp:setFrameRate(f)") << f;
		fpsTarget = f;
		ofSetFrameRate(fpsTarget);
		saveSettings();
	}

	//--------------------------------------------------------------
	void setVerticalSync(bool b) {
		ofLogNotice("ofxWindowApp:setVerticalSync(b)") << b;
		bvSync = b;
		ofSetVerticalSync(bvSync);
		saveSettings();
	}

	//--------------------------------------------------------------
	void setFullScreen(bool b) {
		ofLogNotice("ofxWindowApp:setFullScreen(b)") << b;
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
		ofLogNotice("ofxWindowApp:setDisableAutoSave(b)") << b;
		bDisableAutoSave = b;
		saveSettings();
	}

	//--

	//--------------------------------------------------------------
	void setShowPerformanceAlways(bool b = true) { // Will display alert drop fps info ven when debug display disabled
		ofLogNotice("ofxWindowApp:setEnableAllowQuitAppUsingEscapeKey(b)") << b;
		bShowInfoPerformanceAlways = b;
		saveSettings();
	}

	//----

public:
	//--------------------------------------------------------------
	void setEnableAllowQuitAppUsingEscapeKey(bool b) {
		ofLogNotice("ofxWindowApp:setEnableAllowQuitAppUsingEscapeKey(b)") << b;
		ofSetEscapeQuitsApp(b);
	}

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
	string path_folder; // Folder
	string path_filename; // File
	string path_settings; // Full path

	//--

	//public:
	//TODO: Can't be used yet bc static instance mode must fix setting before setup(*) is called..
	//// Custom path
	////--------------------------------------------------------------
	//void setPathFolder(string s) {
	//	path_folder = s;
	//}

	////--------------------------------------------------------------
	//void setPathFilename(string s) {
	//	path_filename = s;
	//}

	//	void setPathSettings(string s) {
	//		path_setttings = s;
	//	}

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
		saveSettings();
	}

	void setToggleEnableKeys() {
		bKeys = !bKeys;
		saveSettings();
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

	//----

	// Params
private:
	void setupParams();

	//TODO: To avoid ofxSerializer. Use one only groupParam instead.
public:
	ofParameterGroup params { "ofxWindowApp" };

private:
	ofParameterGroup paramsExtra { "Extra" };
	ofParameterGroup paramsWindow { "Window" };
	ofParameterGroup paramsSession { "Session" };

public:
	ofParameter<bool> bDisableAutoSave { "DisableAutoSave", false };
	// Ignores next window modification. Locked window settings (JSON) that will be retained on next app load.
	ofParameter<bool> bKeys { "Keys", true }; // Enable key commands by default
	ofParameter<bool> bShowInfo { "ShowInfo", true };
	ofParameter<bool> bShowInfoPerformanceAlways { "ShowInfoPerformanceAlways", true };
	ofParameter<bool> bShowDebug { "ShowDebug", false }; // Show debug text box

	ofParameter<float> fpsTarget { "FpsTarget", 60.f, 1.f, 180.f };
	ofParameter<bool> bvSync { "vSync", false };

	//--

#ifdef OFX_WINDOW_APP__USE_CONSOLE_WINDOW
public:
	ofParameter<bool> bConsoleWindow { "ConsoleWindow", false };

	//TODO: Handle console terminal window
	//--------------------------------------------------------------
	void setConsoleWindowVisible(bool b) {
		ofLogVerbose("ofxWindowApp:setConsoleWindowVisible()") << b;
		//if (bConsoleWindow != b) bConsoleWindow.setWithoutEventNotifications(b);

	#if defined(TARGET_WIN32)
		// Taken from https://github.com/kritzikratzi/ofxNative/blob/master/src/ofxNative_win.cpp
		::ShowWindow(::GetConsoleWindow(), b ? SW_SHOW : SW_HIDE);
	#endif

		if (!bDisableAutoSave)
			bFlagToSave = true;
	}

	//--------------------------------------------------------------
	bool getConsoleWindowVisible() {
	#if defined(TARGET_WIN32)
		return (::IsWindowVisible(::GetConsoleWindow()) != FALSE);
	#else
		return false; //TODO
	#endif
	}

	//--------------------------------------------------------------
	void setToggleConsoleWindowVisible() {
		bConsoleWindow = bConsoleWindow.get();
	}

private:
	//--------------------------------------------------------------
	void doApplyConsoleWindowVisible() {
		setConsoleWindowVisible(bConsoleWindow.get());
	}
#endif

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
public:
	ofParameter<bool> bStayOnTop { "StayOnTop", false };

private:
	void doApplyStayOnTop();

	//--------------------------------------------------------------
	void setToggleStayOnTop() {
		ofLogNotice("ofxWindowApp:setToggleStayOnTop()");
		setStayOnTop(!bStayOnTop.get());
	}

	//--------------------------------------------------------------
	void setStayOnTop(bool b) {
		ofLogNotice("ofxWindowApp:setStayOnTop(b)") << b;
		bStayOnTop = b;
		if (!bDisableAutoSave)
			bFlagToSave = true;
	}
#endif

	//----

private:
	void ChangedParamsExtra(ofAbstractParameter & e);

private:
	//--------------------------------------------------------------
	void doResetWindowExtraSettings() {
		ofLogVerbose("ofxWindowApp:doResetWindowExtraSettings()");

		// Default settings
		bvSync = false;
		fpsTarget = 60;
		bShowInfo = false;
		bDisableAutoSave = false;
		bShowInfoPerformanceAlways = true;
	}

public:
	//--------------------------------------------------------------
	void doResetWindowDefault() {
		ofLogNotice("ofxWindowApp:doResetWindowDefault()");
		doResetWindowSettings();
		doApplyWindowSettings();
	}

private:
	//--------------------------------------------------------------
	void doResetWindowSettings() {
		ofLogVerbose("ofxWindowApp:doResetWindowSettings()");

		// Window mode
		windowSettings.windowMode = ofWindowMode(0);
		bIsFullScreen = false;

		// Shape
		const int w = 1280; // Default width
		int h = w * (9.f / 16.f);
		windowSettings.setSize(w, h);

		windowSettings.setPosition(glm::vec2(2, OFX_WINDOW_APP__BAR_HEIGHT)); // Left top
		//windowSettings.setPosition(glm::vec2(ofGetWidth() / 2.f - w / 2.f, ofGetHeight() / 2.f - h / 2.f )); // Centered fails...
	}

	void drawDebug();

	void windowChanged(); // Group redirect all other callbacks related to window resized and window moved
	//bool bFlagWindowChanged = false; //TODO: workaround OF/GLFW window management bugs. Get/apply delayed...

	//--------------------------------------------------------------

	// Debug system monitors
private:
	void getDiplaysMonitorsDesktopCanvas(); // Scan os displays at desktop canvas.
	void drawDebugDisplayMonitors(); // Display info
	vector<ofRectangle> monitorRects;
	vector<std::string> monitorNames;
	ofRectangle monitorsCanvasRect;
	int getDisplayIndexForWindow(); // Get in what display index is the window located
	void setWindowCentered(); // Set current app window centered in the same display index
	glm::vec2 getWindowPositionAtDisplay(); // Get window position related to current display, instead of the full canvas of OS displays.

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
