#pragma once

#include "ofMain.h"
#include "ofxSerializer.h"

//TODO:
//+ mix both json in one file settings
//+ add windowResize subscribed listener to auto refresh

class ofxWindowApp : public ofBaseApp
{

public:

	ofxWindowApp();
	~ofxWindowApp();

	//this is to folder all files to avoid mixing with other addons data
	string path_folder = "ofxWindowApp/";
	string path_filename = "AppWindow.json";
	string path_filename2 = "AppWindow_Extra.json";

	//-

	//API

	void setup();
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void windowResized(int w, int h);

	//-

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

	//TODO:
	//WORKAROUND bad window positioning..
	//callback
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

	void setShowDebug(bool b = true)
	{
		ENABLE_Debug = b;
	}
	
	//-

	//extra settings
	void setSettingsFps(float f)
	{
		fps = f;
		ofSetFrameRate(fps);
	}
	void setSettingsVsync(bool b)
	{
		vSync = b;
		ofSetVerticalSync(vSync);
	}
	float getSettingsFps()
	{
		return fps.get();
	}
	bool getSettingsVsync()
	{
		return vSync.get();
	}
	void applySettings()
	{
		ofLogNotice("ofxWindowApp") << "applySettings()";
		ofLogNotice("ofxWindowApp") << "fps: " << fps;
		ofLogNotice("ofxWindowApp") << "vSync: " << vSync;

		ofSetFrameRate(fps);
		ofSetVerticalSync(vSync);
	}

	//-

private:

	void drawDEBUG();

	ofParameter<int> window_W, window_H, window_X, window_Y;

	bool autoSaveLoad = true;
	bool bChanged = false;
	bool ENABLE_Debug = true;

	ofParameterGroup params_Settings{ "EXTRA SETTINGS" };
	ofParameter<bool> vSync{ "V-SYNC", false };
	ofParameter<float> fps{ "FPS", 60.5, 1, 120 };
};


