#pragma once

#include "ofMain.h"
#include "ofxSerializer.h"

#define USE_ofxWindowApp
#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

#define SIZE_SECURE_GAP_INISDE_SCREEN 15 // to avoid that window border it's outside screen monitor

//TODO:
// +++	add ofxScreenSetup addon to bundle all other features
// +	add windowResize subscribed listener to auto refresh

class ofxWindowApp : public ofBaseApp
{
	//TODO:
	//I used public ofBaseApp to make auto update/draw without any call to them is required to made manually...
	//but it exposes several not needed methods (?)

public:
	ofxWindowApp();
	~ofxWindowApp();

	ofWindowSettings WindowPRE;
	ofWindowSettings BigWindow;
	ofWindowSettings MiniWindow;

	void exit();//TODO: test to avoid crashes on exit

	//-

//private:
	void setup();
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void keyPressed(ofKeyEventArgs &eventArgs);
	void windowResized(int w, int h);

	//-

	void drawDEBUG();
	void drawPerformance();

	//----

	//API

public:
	//setters
	void setFrameRate(float f)///required when used for first time into your project and no JSON file settings created yet
	{
		targetFps = f;
		ofSetFrameRate(targetFps);
	}
	void setVerticalSync(bool b)
	{
		vSync = b;
		ofSetVerticalSync(vSync);
	}

	void refreshGetWindowSettings();
	void saveFileWindow();
	void loadFileSettings();

	void setPathFolder(string s)
	{
		path_folder = s;
	}

	void setPathFilename(string s)
	{
		path_filename = s;
	}

	void setAutoSaveLoad(bool b)
	{
		autoSaveLoad = b;
	}

	void setShowDebug(bool b = true)
	{
		SHOW_Debug = b;
	}
	bool getShowDebug()
	{
		return SHOW_Debug;
	}
	void toggleShowDebug()
	{
		SHOW_Debug = !SHOW_Debug;
	}

	void setShowPerformanceAllways(bool b = true)
	{
		SHOW_PerformanceAllways = b;
	}

	void folderCheckAndCreate(string _path);

	//-

	//easy callback to check from ofApp if some settings have changed
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

	//-

private:
	void applyExtra()//fps and vsync only
	{
		ofLogVerbose(__FUNCTION__);
		ofLogVerbose(__FUNCTION__) << "targetFps : " << targetFps;
		ofLogVerbose(__FUNCTION__) << "vSync		: " << vSync;
		ofLogVerbose(__FUNCTION__) << "SHOW DEBUG: " << SHOW_Debug.get();
		ofLogVerbose(__FUNCTION__) << "bModeMini : " << bModeMini.get();
		ofLogVerbose(__FUNCTION__) << "bLock : " << bLock.get();

		ofSetFrameRate(targetFps);
		ofSetVerticalSync(vSync);
	}

private:
	//layout modes
	enum DEBUG_Position
	{
		DEBUG_POSITION_TOP = 0,
		DEBUG_POSITION_BOTTOM
	};
	int positionLayout = DEBUG_POSITION_BOTTOM;

public:
	void setPositionDebugInfo(int POS)
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
		ENABLE_Keys = b;
	}

	//-

public:
	//getters
	float getFrameRate()
	{
		return targetFps.get();
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

	//-

	//mini/big mode
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
		if (autoSaveLoad) {
			refreshGetWindowSettings();
		}

		bModeMini = !bModeMini;

		//big
		if (!bModeMini)
		{
			//if (windowBigMode == "OF_WINDOW") ofSetFullscreen(false);
			//else if (windowBigMode == "OF_FULLSCREEN") ofSetFullscreen(true);
		}
		//mini
		else
		{
			if (ofGetWindowMode() == OF_FULLSCREEN)
				ofSetFullscreen(false);
		}

		applyMode();
	}
	void enableMiniMode(bool b)
	{
		if (autoSaveLoad) {
			refreshGetWindowSettings();
		}

		bModeMini = b;

		applyMode();
	}
	void setLock(bool b) {
		bLock = b;
	}

private:

	void applyMode()
	{
		ofLogVerbose(__FUNCTION__);

		//mini
		if (bModeMini) {
			ofSetWindowPosition(MiniWindow.getPosition().x, MiniWindow.getPosition().y);
			ofSetWindowShape(MiniWindow.getWidth(), MiniWindow.getHeight());
		}
		//big
		else {
			ofSetWindowPosition(BigWindow.getPosition().x, BigWindow.getPosition().y);
			ofSetWindowShape(BigWindow.getWidth(), BigWindow.getHeight());

			////TODO:
			//if (BigWindow.windowMode == ofWindowMode(OF_FULLSCREEN)) ofSetFullscreen(true);
			//if (windowBigMode == ("OF_FULLSCREEN")) ofSetFullscreen(true);
		}

		//apply extra
		ofSetVerticalSync(vSync);
		ofSetFrameRate(targetFps);
	}

	//-

private:

	//this is to folder all files to avoid mixing with other addons data
	string path_folder;
	string path_filename;

	bool autoSaveLoad = true;
	bool bChangedWindow = false;
	bool ENABLE_Keys = true;//keys enabled by default

	ofParameter<int> window_W, window_H, window_X, window_Y;
	ofParameterGroup params_Extra{ "extra settings" };
	ofParameter<bool> vSync{ "vsync", false };
	ofParameter<float> targetFps{ "fps", 60.5, 1, 120 };
	ofParameter<bool> SHOW_Debug{ "showInfo", true };
	ofParameter<bool> bModeMini{ "miniMode", false };
	ofParameter<bool> SHOW_PerformanceAllways{ "debugPerformance", true };
	ofParameter<bool> bLock{ "lock", false };

	string windowBigMode;//fulscreen or window mode
	float realFps;
	int xx = 0;
	int yy = 0;

	//TODO: add full screen/window bool param
};


