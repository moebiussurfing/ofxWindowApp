#pragma once

#include "ofMain.h"
#include "ofxSerializer.h"

//TODO:
//+++ add ofxScreenSetup addon to bundle all other features
//+ add windowResize subscribed listener to auto refresh

class ofxWindowApp : public ofBaseApp
{
	//TODO:
	//I used public ofBaseApp to make auto update/draw without any call to them is required to made manually...
	//but it exposes several not needed methods (?)

public:
	ofxWindowApp();
	~ofxWindowApp();

	//-

private:
	void setup();
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
    void keyPressed(ofKeyEventArgs &eventArgs);
	void windowResized(int w, int h);
    
	void drawDEBUG();
	void drawPerformance();

	//--

	//API
public:

	///setters. 
	void setSettingsFps(float f)///required when used for first time into your project and no JSON file settings created yet
	{
		targetFps = f;
		ofSetFrameRate(targetFps);
	}
	void setSettingsVsync(bool b)
	{
		vSync = b;
		ofSetVerticalSync(vSync);
	}

	void saveWindow();
	void loadWindow();

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

	//-

	//easy callback to check from ofApp if some settings have changed
	bool isChanged()
	{
		if (bChanged)
		{
			bChanged = false;
			return true;
		}

		else
			return false;
	}
	
	//-

private:
	void applySettings()//fps and vsync only
	{
		ofLogNotice("ofxWindowApp") << "applySettings()";
		ofLogNotice("ofxWindowApp") << "targetFps: " << targetFps;
        ofLogNotice("ofxWindowApp") << "vSync: " << vSync;
        ofLogNotice("ofxWindowApp") << "SHOW DEBUG: " <<SHOW_Debug.get();

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
	void setPosition(int POS)
	{
		positionLayout = POS;
	}
	int getDEBUG_Position()
	{
		return positionLayout;
	}
	void togglePosition()
	{
		setPosition((positionLayout == 1) ? 0 : 1);
	}
	void setEnableKeys(bool b)
	{
		ENABLE_Keys = b;
	}

	//-

	//getters
	float getSettingsFps()
	{
		return targetFps.get();
	}
	bool getSettingsVsync()
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

private:

	//default folders
	//this is to folder all files to avoid mixing with other addons data
	string path_folder = "ofxWindowApp/";//NOTE: maybe you need to create this folder manually into project /data
	string path_filename = "AppWindow.json";
	//string path_filename2 = "AppWindow_Extra.json";//to split settings in more than one files...

	bool autoSaveLoad = true;
	bool bChanged = false;
	bool ENABLE_Keys = true;//keys enabled by default

	ofParameter<int> window_W, window_H, window_X, window_Y;
	ofParameterGroup params_Settings{ "extra settings" };
	ofParameter<bool> vSync{ "vsync", false };
	ofParameter<float> targetFps{ "targetFps", 60.5, 1, 120 };
    ofParameter<bool> SHOW_Debug{"debug", true};
	//TODO: add full screen/window bool param

	ofParameter<bool> SHOW_PerformanceAllways{ "performance", true };

	float realFps;

	int xx = 0;
	int yy = 0;
};


