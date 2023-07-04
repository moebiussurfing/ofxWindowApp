#pragma once

/*

	TODO:

	+ should have a callback to detect when window is moved. could that be too expensive?
	+ add fullscreen bool param with listener. to allow expose on gui's
	+ broken full screen save load?
	+ add ofxScreenSetup addon to bundle all other features

 */

 //TODO: Fixing exceptions hen closing ofApp.
 //#define SURFING_IMGUI__CREATE_EXIT_LISTENER // to enable that ofApp exit will call exit and save settings.
 //#define SURFING_IMGUI__ENABLE_SAVE_ON_EXIT // to enable auto save on exit.

#include "ofMain.h"

//#include <Windows.h>
#include <GLFW/glfw3.h>

#include "ofxSerializer.h"

#define USE_ofxWindowApp
#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

#define SIZE_SECURE_GAP_INISDE_SCREEN 18
// to avoid that window border it's outside screen monitor

#define BAR_HEIGHT 25
// probably fits on Win32 only.

#define USE_CUSTOM_FONT

#define USING__OFX_WINDOW_APP

//#define USE_MINI_WINDOW
// Mini window mode can be switched to normal/big mode.
// both states will be handled in parallel


class ofxWindowApp
{
public:
	ofxWindowApp();
	~ofxWindowApp();

private:
	void setup();
	void startup();
	void update(ofEventArgs& args);
	void draw(ofEventArgs& args);
	void exit();

	void keyPressed(ofKeyEventArgs& eventArgs);
	void keyReleased(ofKeyEventArgs& eventArgs);

	//--

private:
	ofWindowSettings WindowPRE;
	ofWindowSettings BigWindow;

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

	//TODO:
	////TODO: fix
	//// auto save
	//void windowIsMoved() {
	//	if (bAutoSaveLoad)
	//	{
	//		ofLogNotice(__FUNCTION__) << "Just saved after window been resized";
	//		saveFileWindow();
	//	}
	//}
	//void windowIsMoved() {
	//	if (bAutoSaveLoad)
	//	{
	//		ofLogNotice(__FUNCTION__) << "Just saved after window been resized";
	//		saveFileWindow();
	//	}
	//}

	bool bFlagSave = 0;
	float timeWhenToSaveFlag;

	bool bFlagSaved = 0;

	//-

	void drawDebug();
	void drawPerformance();

	//----

	// API

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
			bigFullScreen = true;
		}
		else
		{
			ofSetFullscreen(false);

			float windowBar_h = 25;

			window_Y = MAX(ofGetWindowPositionY(), windowBar_h);
			// avoid negative out of screen. minimal h is 25
			window_X = ofGetWindowPositionX();

			ofSetWindowPosition(window_X, window_Y);

			bigFullScreen = false;
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
		setAlwaysOnTop(!isOnTop);
	}

	//--------------------------------------------------------------
	void setAlwaysOnTop(bool b)
	{
		ofLogNotice("ofxWindowApp::setAlwaysOnTop") << b;
		bOnTop = b;
	}

private:
	bool isOnTop = false;

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
	void refreshGetWindowSettings();
	void saveSettings(bool bSlient = false);

public:
	void save();//alias and public

private:
	void saveSettingsAfterRefresh();
	void loadSettings();

private:
	void applyMode();

public:
	//--------------------------------------------------------------
	string getPathFolder() const
	{
		return path_folder;
	}
	//--------------------------------------------------------------
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
		bDebug = b;
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
		return bDebug;
	}
	//--------------------------------------------------------------
	void toggleShowDebug()
	{
		bDebug = !bDebug;
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
		if (bChangedWindow)
		{
			bChangedWindow = false;
			return true;
		}

		else
			return false;
	}

	//--

private:
	void applyExtra() // fps and vsync only
	{
		ofLogVerbose("ofxWindowApp::applyExtra");
		ofLogVerbose("ofxWindowApp") << "FpsTarget  : " << fpsTarget;
		ofLogVerbose("ofxWindowApp") << "vSync	  : " << vSync;
		ofLogVerbose("ofxWindowApp") << "Show Debug : " << bDebug.get();
		ofLogVerbose("ofxWindowApp") << "bLock      : " << bLock.get();
#ifdef USE_MINI_WINDOW
		ofLogVerbose("ofxWindowApp") << "bModeMini  : " << bModeMini.get();
#endif

		ofSetFrameRate(fpsTarget);
		ofSetVerticalSync(vSync);
	}

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
			refreshGetWindowSettings();
		}

		bModeMini = !bModeMini;

		// big preset
		if (!bModeMini)
		{
			ofSetFullscreen(bigFullScreen);

			if (bigFullScreen) {
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

		applyMode();
	}
	void enableMiniMode(bool b)
	{
		if (bAutoSaveLoad) {
			refreshGetWindowSettings();
		}

		bModeMini = b;

		applyMode();
	}
#endif

	//--

	// Disables saving. So json file will not be overwritten.
	void setLock(bool b)
	{
		bLock = b;
	}

	//--

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

private:
	// this is to folder all files to avoid mixing with other add-ons data
	string path_folder;
	string path_filename;

	bool bAutoSaveLoad = true;//load at startup 
	//TODO: (disabled->) and saves on exit

	bool bChangedWindow = false;
	bool bKeys = true; // keys enabled by default

	ofParameter<int> window_W, window_H, window_X, window_Y;

	ofParameterGroup params{ "Extra" };
	ofParameter<bool> vSync{"vSync", false};
	ofParameter<float> fpsTarget{"Fps", 60.f, 1, 120};
	ofParameter<bool> bShowPerformanceAlways{"DebugPerformance", true};

	ofParameter<bool> bLock{"LockMode", false};
	//Ignores next window modification. 
	// Kind of hardcoded position that will maintain on next app load.


public:
	ofParameter<bool> bOnTop{ "OnTop", false };
	ofParameter<bool> bDebug{"WindowInfo", true};

private:
	bool bDoneStartup = false;

#ifdef USE_MINI_WINDOW
	ofParameter<bool> bModeMini{ "miniPreset", false };
#endif

	//string windowBigMode; // full screen or window mode

	void Changed_Params(ofAbstractParameter& e);

	float realFps;

#ifndef USE_CUSTOM_FONT
	int xx = 10;
	int yy = 0;
#endif
	int pad = 8;
#ifdef USE_CUSTOM_FONT
	int xx = pad / 2;
	int yy = 0;
#endif

	//TODO: add full screen/window bool param
	bool bigFullScreen = false;

	void refreshToggleWindowMode();

public:
	//--------------------------------------------------------------
	void doReset()
	{
		bigFullScreen = false;
		vSync = false;
		fpsTarget = 60;

#ifdef USE_MINI_WINDOW
		bModeMini = false;
#endif

		BigWindow.setPosition(glm::vec2(0, BAR_HEIGHT));
		BigWindow.setSize(1920, 1080 - BAR_HEIGHT);

#ifdef USE_MINI_WINDOW
		MiniWindow.setPosition(glm::vec2(20, 20));
		MiniWindow.setSize(200, 200);
#endif

		applyMode();
	}

	//--------------------------------------------------------------
	void drawInfo()
	{
		string s;

		s += "ofxWindowApp";
		if (bFlagSaved) s += " | SAVED!";
		s += "\n\n";
		s += "KEY STROKES\n\n";
		s += "Alt +\n";
		s += "W : SHOW INFO\n";
		s += "V : VERTICAL SYNC = " + ofToString(vSync ? "ON " : "OFF") + "\n";
		bool bMode = (ofGetWindowMode() == OF_FULLSCREEN);
		s += "F : SCREEN " + ofToString(bMode ? "FULL-SCREEN_MODE" : "WINDOW_MODE") + "\n";

#ifdef USE_MINI_WINDOW
		s += "Alt + M: PRESET ";
		if (bModeMini) s += "*MINI  BIG";
		else s += " MINI *BIG";
#endif

		s += "\n";
		s += "Alt + R : RESET TO FULLHD \n";
		s += "Alt + T : ON TOP = " + ofToString(bOnTop ? "TRUE" : "FALSE") + "\n";

		//TODO: WIP lock mode
		s += "Alt + L : LOCK = " + ofToString(bLock ? "TRUE" : "FALSE");

		s += "\n\n";

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

		ofColor c1 = bFlagSaved ? 255 : 0;
		ofColor c2 = bFlagSaved ? 0 : 255;
		if (bFlagSaved) bFlagSaved = 0;

		ofDrawBitmapStringHighlight(s, 50, 50, c1, c2);
	}

	//--



	// GPT callback
	//ofAddListener(ofEvents().windowMoved, this, &ofxWindowApp::windowIsMoved);//TODO:
#if 0
	void setupMover() {
		//ofGetWindowPtr()->getTitle();
		// Register the callback for window movement events
		HWND hWnd = FindWindowA(NULL, ofGetWindowTitle().c_str());
		if (hWnd != NULL) {
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(windowProc));
		}
	}

	static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		ofApp* app = reinterpret_cast<ofApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		switch (message) {
		case WM_MOVE:
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			// Call your custom function here to handle the window movement event
			app->onWindowMoved(x, y);
			break;
			// Handle other messages if necessary
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	void onWindowMoved(int x, int y) {
		// Handle the window movement event
		// ...
	}
#endif

};
