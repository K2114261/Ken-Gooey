#pragma once

#include "SpriteButton.h"
#include "Splash.h"

class CGooeyGame : public CGame
{
protected:
	// The Images: the backgrounds and various screens
	CGraphics theBackground, theMenuBack, theMenuScreen, theCongratsScreen;

	// Sprites:
	CSprite theMarble, theCannon, theBarrel;
	CSpriteRect thePowerSlider, thePowerMarker;
	CSpriteList theBumper;
	CSpriteList wood;
	CSpriteList grass;

	CSprite theGoal;		// goal


	// Sprites collections. Splashes are displayed after the marble gets destroyed.
	CSpriteList theWalls;
	CSpriteList theGoos;
	CSplashList theSplashes;

	// various buttons used in the levels and in the menu
	vector<CSpriteButton*> theButtons, theButtonsLevel;
	CSpriteButton* m_pCancelButton;
	CSpriteButton* m_pButtonPressed;

	// aiming time
	long m_bAimTime;
	bool m_bInMotion;

	// Levels
	Sint16 m_nCurLevel;			// level currently being played
	Sint16 m_nMaxLevel;			// max number of levels available
	Sint16 m_nUnlockedLevel;	// max unlocked level

	// Level completion flag
	bool m_bLevelCompleted;

	// Sound PLayer
	CSoundPlayer m_player;

	// Button Definitions
	enum CMD { CMD_NONE, CMD_MENU, CMD_EXPLODE, CMD_CANCEL, CMD_LEVEL = 100 };

public:
	CGooeyGame(void);
	~CGooeyGame(void);

	// Helper functions
	void SpawnMarble();
	void KillMarble();
	void BeginAim();
	bool IsAiming();
	float GetShotPower();
	float Shoot();

	CSpriteButton *FindButton(Sint16 buttonId);

	// Per-Frame Callback Funtions
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnButton(Uint16 nCmdId);
	virtual void OnLButtonDown(Uint16 x, Uint16 y);
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
