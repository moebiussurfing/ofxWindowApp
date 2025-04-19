#pragma once

/*

	TODO:

	+ should have a callback to detect when window is moved. could that be too expensive?
	+ add full screen bool param with listener. to allow expose on gui's
	+ broken full screen save load?
	+ add ofxScreenSetup addon to bundle all other features

 */

 //----

#define SURFING_WINDOW_APP__USE_STATIC 
// -> Main directive. Uncomment/enable to allow WIN32 platform for windowMove callback.

//--

 //TODO: Fixing exceptions hen closing ofApp.
 //#define SURFING_WINDOW_APP__CREATE_EXIT_LISTENER // to enable that ofApp exit will call exit and save settings.
 //#define SURFING_WINDOW_APP__ENABLE_SAVE_ON_EXIT // to enable auto save on exit.

#define SURFING_WINDOW_APP__USE_TIMED_SAVER

//#define SURFING_WINDOW_APP__USE_FULLHD_COMMAND

//----

#include "ofMain.h"

#if defined(TARGET_WIN32) && defined(SURFING_WINDOW_APP__USE_STATIC)
#include <GLFW/glfw3.h>
#if defined(SURFING_WINDOW_APP__USE_TIMED_SAVER)
#undef SURFING_WINDOW_APP__USE_TIMED_SAVER
#endif
#else
#ifndef SURFING_WINDOW_APP__USE_TIMED_SAVER
#define SURFING_WINDOW_APP__USE_TIMED_SAVER
#endif
#endif

#include "ofxSerializer.h"

#define USING_ofxWindowApp
#ifdef USING_ofxWindowApp
#include "ofxWindowApp.h"
#endif

#define SIZE_SECURE_GAP_INISDE_SCREEN 18
// to avoid that window border it's outside screen monitor

#define OFX_WINDOW_APP_BAR_HEIGHT 25
// probably fits on Win32 only.

#define USE_CUSTOM_FONT

#define USING__OFX_WINDOW_APP

//#define USE_MINI_WINDOW
// Mini window mode can be switched to normal/big mode.
// both states will be handled in parallel

//----

class ofxWindowApp
{
public:
	ofxWindowApp();
	~ofxWindowApp();

public:
	void setup();

#ifdef SURFING_WINDOW_APP__USE_STATIC
public:
	void setup(ofxWindowApp* app) {
		this->setInstance(app);
		setup();
	}
public:
	static void windowMoved(GLFWwindow* window, int xpos, int ypos);
private:
	static ofxWindowApp* instance; // Static pointer to hold the instance
public:
	static void setInstance(ofxWindowApp* app); // Static function to set the instance
#endif

private:
	bool bDoneSetup = false;

private:
	void startup();
	void update(ofEventArgs& args);
	void draw(ofEventArgs& args);

//public://TODO
	void exit();

private:
	void keyPressed(ofKeyEventArgs& eventArgs);
	void keyReleased(ofKeyEventArgs& eventArgs);

	//--

private:
	//ofWindowSettings windowSettingsPRE;
	ofWindowSettings windowSettings;

#ifdef USE_MINI_WINDOW
	ofWindowSettings MiniWindow;
#endif

#ifdef USE_CUSTOM_FONT
	ofTrueTypeFont font;
	int fontSize;
#endif

	//--

private:
	// Modifiers
	bool mod_COMMAND = false;
	bool mod_CONTROL = false;
	bool mod_ALT = false;
	bool mod_SHIFT = false;

	void windowResized(int w, int h);
	void windowResized(ofResizeEventArgs& e);

	bool bFlagToSave = 0;
	bool bFlagDoneSaved = 0;

#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
	float timeWhenToSaveFlag;
#endif

	//--

	void drawHelpInfo();
	void drawPerformanceWidget();

	//----

	// API METHODS TO CONFIGURE

public:
	// Setters
	// required when used for first time into your project and no JSON file settings created yet.
	// Default will be 60 fps if not defined!
	//--------------------------------------------------------------
	void setFrameRate(float f)
	{
		fpsTarget = f;
		ofSetFrameRate(fpsTarget);
	}

	//--------------------------------------------------------------
	void setVerticalSync(bool b)
	{
		vSync = b;
		ofSetVerticalSync(vSync);
	}

	//--------------------------------------------------------------
	void setFullScreen(bool b)
	{
		if (b)
		{
			ofSetFullscreen(true);
			bIsFullScreen = true;
		}
		else
		{
			ofSetFullscreen(false);

			float windowBar_h = 25;

			window_Y = MAX(ofGetWindowPositionY(), windowBar_h);
			// avoid negative out of screen. minimal h is 25
			window_X = ofGetWindowPositionX();

			ofSetWindowPosition(window_X, window_Y);

			bIsFullScreen = false;
		}

		// Update
		windowResized(ofGetWidth(), ofGetHeight());
	}

	//--

	//--------------------------------------------------------------
	void setEnableAllowQuitAppUsingEscapeKey(bool b)
	{
		ofLogNotice("ofxWindowApp::setEnableAllowQuitAppUsingEscapeKey");
		ofSetEscapeQuitsApp(b);
	}

	//--------------------------------------------------------------
	void setToggleAlwaysOnTop()
	{
		ofLogNotice("ofxWindowApp::setToggleAlwaysOnTop");
		setAlwaysOnTop(!isWindowOnTop);
	}

	//--------------------------------------------------------------
	void setAlwaysOnTop(bool b)
	{
		ofLogNotice("ofxWindowApp::setAlwaysOnTop") << b;
		bWindowOnTop = b;
	}

private:
	bool isWindowOnTop = false;

	//--

public:
	// Taken from https://github.com/kritzikratzi/ofxNative/blob/master/src/ofxNative_win.cpp
	//--------------------------------------------------------------
	void setConsoleVisible(bool show)
	{
#if defined(TARGET_WIN32)
		::ShowWindow(::GetConsoleWindow(), show ? SW_SHOW : SW_HIDE);
#endif
	}
	//--------------------------------------------------------------
	bool getConsoleVisible()
	{
		return (::IsWindowVisible(::GetConsoleWindow()) != FALSE);
	}

	//--

private:
	void doRefreshGetWindowSettings();
	void saveSettings(bool bSlient = false);

public:
	void save();//alias and public

private:
	void saveSettingsAfterRefresh();
	void loadSettings();

private:
	void doApplyWindowMode();

public:
	string getPathFolder() const
	{
		return path_folder;
	}
	string getPathSettings() const
	{
		string p = path_folder + "/" + path_filename;
		return p;
	}

	//--------------------------------------------------------------
	void setPathFolder(string s)
	{
		path_folder = s;
	}

	//--------------------------------------------------------------
	void setPathFilename(string s)
	{
		path_filename = s;
	}

	//--------------------------------------------------------------
	void setAutoSaveLoad(bool b)
	{
		bAutoSaveLoad = b;
	}

	//--------------------------------------------------------------
	void setShowDebug(bool b = true)
	{
		bShowWindowInfo = b;
		//bShowPerformanceAlways = b;
	}
	//--------------------------------------------------------------
	void setShowDebugPerformance(bool b = true)
	{
		bShowPerformanceAlways = b;
	}
	//--------------------------------------------------------------
	bool getShowDebug()
	{
		return bShowWindowInfo;
	}
	//--------------------------------------------------------------
	void toggleShowDebug()
	{
		bShowWindowInfo = !bShowWindowInfo;
	}

	//--------------------------------------------------------------
	void setShowPerformanceAllways(bool b = true)
	{
		bShowPerformanceAlways = b;
	}

	void folderCheckAndCreate(string _path);

	//--

	// Easy callback only to check from parent scope/ofApp if window shape has changed.
	//--------------------------------------------------------------
	bool isChanged()
	{
		if (bDoneSavedEasyCallback)
		{
			bDoneSavedEasyCallback = false;
			return true;
		}

		else return false;
	}

	//--

private:
	void doApplyExtraSettings(); // fps and vsync only
	
public:
	// Layout modes
	enum DEBUG_Position
	{
		DEBUG_POSITION_TOP = 0,
		DEBUG_POSITION_BOTTOM
	};

private:
	int positionLayout = DEBUG_POSITION_BOTTOM;

public:
	void setPositionDebugInfo(int POS)
	{
		positionLayout = POS;
	}

	void setPositionDebugInfo(DEBUG_Position POS = DEBUG_POSITION_BOTTOM)
	{
		positionLayout = POS;
	}

	int getDEBUG_Position()
	{
		return positionLayout;
	}

	void togglePositionDebugInfo()
	{
		setPositionDebugInfo((positionLayout == 1) ? 0 : 1);
	}

	void setEnableKeys(bool b)
	{
		bKeys = b;
	}

	//--

public:
	float getFrameRate()
	{
		return fpsTarget.get();
	}

	bool getVerticalSync()
	{
		return vSync.get();
	}

	float getWindowWidth()
	{
		return window_W;
	}

	float getWindowHeight()
	{
		return window_H;
	}

	float getWindowPositionX()
	{
		return window_X;
	}

	float getWindowPositionY()
	{
		return window_Y;
	}

	//--

#ifdef USE_MINI_WINDOW
	// Mini/Big mode
public:
	//TODO:
	//void setShapeMiniMode(glm::vec2 pos, glm::vec2 size)
	//{
	//	position_Mini = pos;
	//	size_Mini = size;
	//}
	//void setMiniMode()
	//{
	//}

	bool isModeMini()
	{
		return bModeMini.get();
	}
	bool isModeBig()
	{
		return (!bModeMini.get());
	}
	void toggleModeWindowBigMini()
	{
		if (bAutoSaveLoad) {
			doRefreshGetWindowSettings();
		}

		bModeMini = !bModeMini;

		// big preset
		if (!bModeMini)
		{
			ofSetFullscreen(bIsFullScreen);

			if (bIsFullScreen) {
				ofSetWindowPosition(0, 0);
				ofSetWindowShape(ofGetWidth(), ofGetHeight());
			}

			//if (windowBigMode == "OF_WINDOW") ofSetFullscreen(false);
			//else if (windowBigMode == "OF_FULLSCREEN") ofSetFullscreen(true);
		}
		//mini preset
		else
		{
			//if (ofGetWindowMode() == OF_FULLSCREEN)

			ofSetFullscreen(false);
		}

		doApplyWindowMode();
	}
	void enableMiniMode(bool b)
	{
		if (bAutoSaveLoad) {
			doRefreshGetWindowSettings();
		}

		bModeMini = b;

		doApplyWindowMode();
	}
#endif

	//--

	// Disables saving. So json file will not be overwritten.
	void setLock(bool b)
	{
		bDisableAutoSave = b;
	}

	//--

#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
public:
	void setEnableTimerSaver(bool b = true)
	{
		bAutoSaverTimed = b;
	}

	void setTimerSaver(int t)
	{
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
	//TODO: (disabled->) and saves on exit

	bool bDoneSavedEasyCallback = false;
	bool bKeys = true; // keys enabled by default

	ofParameter<int> window_W, window_H, window_X, window_Y;

	ofParameterGroup paramsExtra{ "Extra" };
	ofParameterGroup paramsWindow{ "Window" };
	ofParameterGroup paramsSession{ "Session" };
	ofParameter<bool> vSync{"vSync", false};
	ofParameter<float> fpsTarget{"Fps", 60.f, 1, 120};
	ofParameter<bool> bShowPerformanceAlways{"DebugPerformance", true};

	ofParameter<bool> bDisableAutoSave{"DisableAutosave", false};
	// Ignores next window modification. 
	// Kind of hardcoded position that will maintain on next app load.

public:
	ofParameter<bool> bWindowOnTop{ "WindowOnTop", false };
	ofParameter<bool> bShowWindowInfo{"ShowWindowInfo", true};

private:
	bool bDoneStartup = false;

#ifdef USE_MINI_WINDOW
	ofParameter<bool> bModeMini{ "miniPreset", false };
#endif

private:
	void Changed_ParamsExtra(ofAbstractParameter& e);

	float fpsReal;

#ifndef USE_CUSTOM_FONT
	int xx = 10;
	int yy = 0;
#endif
	int pad = 8;
#ifdef USE_CUSTOM_FONT
	int xx = pad / 2;
	int yy = 0;
#endif

	//TODO: Add full screen/window bool param
	bool bIsFullScreen = false;
	bool bIsFullScreenInSettings = false;

	glm::vec2 posSettings = glm::vec2(0);
	glm::vec2 sizeSettings = glm::vec2(0);

	void doRefreshToggleWindowMode();

public:
	//--------------------------------------------------------------
	void doReset()
	{
		bIsFullScreen = false;
		vSync = false;
		fpsTarget = 60;

#ifdef USE_MINI_WINDOW
		bModeMini = false;
#endif

		windowSettings.setPosition(glm::vec2(0, OFX_WINDOW_APP_BAR_HEIGHT));
		windowSettings.setSize(1920, 1080 - OFX_WINDOW_APP_BAR_HEIGHT);

#ifdef USE_MINI_WINDOW
		MiniWindow.setPosition(glm::vec2(20, 20));
		MiniWindow.setSize(200, 200);
#endif

		doApplyWindowMode();
	}

	//--------------------------------------------------------------
	void drawDebugInfo()
	{
		string s;
		s += "ofxWindowApp";
		if (bFlagDoneSaved) s += "  SAVE";
		s += "\n\n";
		s += "KEYS\n\n";
		s += "Alt +\n\n";
		s += "W : SHOW INFO\n";
		s += "V : V_SYNC = " + ofToString(vSync ? "ON " : "OFF") + "\n";
		bool bMode = (ofGetWindowMode() == OF_FULLSCREEN);
		s += "F : SCREEN = " + ofToString(bMode ? "FULLSCREEN_MODE" : "WINDOW_MODE") + "\n";
#ifdef USE_MINI_WINDOW
		s += "Alt + M: PRESET ";
		if (bModeMini) s += "*MINI  BIG";
		else s += " MINI *BIG";
#endif
		s += "\n";
#ifdef SURFING_WINDOW_APP__USE_FULLHD_COMMAND
		s += "Alt + R : RESET TO FULLHD \n";
#endif
		s += "T : ON_TOP = " + ofToString(bWindowOnTop ? "TRUE" : "FALSE") + "\n";
		//TODO: WIP: Lock mode
		s += "L : NO_SAVE = " + ofToString(bDisableAutoSave ? "TRUE" : "FALSE");
		s += "\n\n";
#ifndef SURFING_WINDOW_APP__USE_TIMED_SAVER
		s += "SAVES IF WINDOW IS\n";
		s += "RESIZED OR MOVED\n";
#else
		s += "SAVES IF WINDOW IS\n";
		s += "RESIZED\n";
		if (bAutoSaverTimed)
		{
			s += "MOVED AND TIMER FINISHED\n";
			s += "\n";
			s += "Autosaver Timed: " + ofToString((bAutoSaverTimed ? "ON " : "OFF"));
			s += "\n";
			s += ofToString(timePeriodToCheckIfSave / 10.f, 1) + "secs\n";
			auto t = ofGetElapsedTimeMillis() - timeLastAutoSaveCheck;
			int pct = ofMap(t, 0, timePeriodToCheckIfSave, 0, 100);
			s += ofToString(pct) + "%";
		}
#endif

		// Flash when saving
		ofColor c1 = bFlagDoneSaved ? 255 : 0;
		ofColor c2 = bFlagDoneSaved ? 0 : 255;
		if (bFlagDoneSaved) bFlagDoneSaved = 0;

		int x = 7;
		int y = 18;
		ofDrawBitmapStringHighlight(s, x, y, c1, c2);
	}
};
