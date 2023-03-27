#include "stdafx.h"
#include "GooeyGame.h"
#include <algorithm>
#include <format>

CGooeyGame::CGooeyGame(void) :
	theBackground("back.png"),
	theMenuBack(1280, 720, 32, 0xff, 0xff00, 0xff0000, 0xff000000),
	theMenuScreen("menu.png"),
	theCongratsScreen("congrats.png"),
	theMarble(20, 20, "marble.gif", 0),
	theCannon(80, 56, "cannon.png", 0),
	theBarrel(110, 70, "barrel.png", 0),
	thePowerSlider(CRectangle(12, 2, 200, 20), CColor(255,255,255,0), CColor::Black(), 0),
	thePowerMarker(CRectangle(12, 2, 200, 20), CColor::Blue(), 0)

{
	m_pButtonPressed = NULL;
	m_bAimTime = 0;
	theBarrel.SetPivotFromCenter(-40, 0);

	m_nCurLevel = 0;
	m_nMaxLevel = 0;
	m_nUnlockedLevel = 1;
	m_bLevelCompleted = true;
}

CGooeyGame::~CGooeyGame(void)
{
}

// Tuning
#define MAX_POWER	1000
#define MIN_POWER	200
#define GRAVITY		5.f
#define RESTITUTION	0.8f

/////////////////////////////////////////////////////
// Helper Functions

// Spawns a marble inside the cannon and plays the "ready" sound
void CGooeyGame::SpawnMarble()
{
	theMarble.UnDie();
	theMarble.UnDelete();
	theMarble.SetVelocity(0, 0);
	theMarble.SetOmega(0);
	theMarble.SetPosition(theBarrel.GetPosition());
	if (IsGameMode())
	{
		m_player.Play("ready.wav"); m_player.Volume(1.f);
	}
}

// Makes the marble die and creates a new splash in its place
void CGooeyGame::KillMarble()
{
	if (theMarble.IsDying()) return;
	theSplashes.push_back(new CSplash(theMarble.GetPosition(), CColor(80, 90, 110), 1.0, 80, 100, GetTime()));
	theMarble.Die(0);
	m_player.Play("marble.wav"); m_player.Volume(1.f);
}

// Starts the aiming mode
void CGooeyGame::BeginAim()
{
	m_bAimTime = GetTime();
	m_player.Play("aim.wav", -1); m_player.Volume(1.f);
}

// Returns true if in aiming mode
bool CGooeyGame::IsAiming()
{
	return m_bAimTime != 0;
}

// Returns the shot power, which depends on the time elapsed since the aiming mode was started
float CGooeyGame::GetShotPower()
{
	if (m_bAimTime == 0) return 0;
	float t = (float)(GetTime() - m_bAimTime);
	float ft = acos(1 - 2 * ((float)MIN_POWER / (float)MAX_POWER));
	float sp = (-0.5f * cos(t  * 3.1415f / 2000 + ft) + 0.5f) * MAX_POWER;
	if (sp > MIN_POWER) return sp; else return 0;
}

// Finishes the aiming mode
float CGooeyGame::Shoot()
{
	float f = GetShotPower();
	m_bAimTime = 0;
	if (f > 0) m_player.Play("cannon.wav"); else m_player.Stop(); m_player.Volume(0.4f);
	return f;
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CGooeyGame::OnUpdate()
{
	if (!IsGameMode()) return;

	Uint32 t = GetTime();
	Uint32 dt = GetDeltaTime();	// time since the last frame (in milliseconds)

	if (!theMarble.IsDead() && theMarble.GetSpeed() > 0)
	{
		// Apply accelerations
		if (theMarble.IsDying())
			// rapid linear deceleration if trapped inside a Goo
			theMarble.SetSpeed(theMarble.GetSpeed() * 0.9f);
		else
			// gravity!
			theMarble.Accelerate(0, -GRAVITY);
		for (CSprite* pBumper : theBumper)
			pBumper->Accelerate(0, -GRAVITY);


		//// TO DO: Test collisions with the walls
		// Hint: When collision detected, apply reflection. Note that you have the RESTITUTION defined as 0.8 (see line 36)
		// Also, play sound:  m_player.Play("hit.wav");
		for (CSprite* pBumper : theBumper)
		{
			CVector v1 = theMarble.GetVelocity();
			CVector v2 = pBumper->GetVelocity();
			CVector n = CVector(0, 1);
			CVector rp = pBumper->GetPos() - theMarble.GetPos();
			CVector no;

			if (theMarble.HitTest(&*pBumper))
			{
				CVector no = Normalise(rp);

				CVector u = Dot(v1 - v2, no) * no;
				v1 -= u;
				v2 += u;
			}
			for (CSprite* pWall : theWalls)
			{
				if (pBumper->HitTest(&*pWall) && Dot(v2, n) < 0)
				{
					v2 = 0.6 * Reflect(v2, n);
				}
			}

			pBumper->SetVelocity(v2);

		}

		for (CSprite* pWall : theWalls)
		{
			const float X = pWall->GetWidth() / 2;
			const float Y = pWall->GetHeight() / 2;
			float alpha = DEG2RAD(pWall->GetRotation());
			const float R = 12;			// radius of the ball
			const float damp = 0.6f;

			CVector v = theMarble.GetVelocity() * t;
			CVector tp = pWall->GetPos() - theMarble.GetPos();
			CVector n = CVector(sin(alpha), cos(alpha));

			if (theMarble.HitTest(&*pWall))
			{

				if (Dot(v, n) < 0)
				{
					// Perpendicular component (oncoming)
					float vy = Dot(v, n);		// velocity component
					CVector d = tp + (Y + R) * -n;	// distance vector between edges
					float dy = Dot(d, n);		// perpendicular space between
					float f1 = dy / vy;

					// Parallel component (breadth control)
					float vx = Cross(v, n);		// velocity component
					float tx = Cross(tp, n);		// distance between centres
					float f2 = (tx - vx * f1) / (X + R);

					if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n));
				}

				if (Dot(v, n) > 0)
				{
					// Perpendicular component (oncoming)
					float vy = Dot(v, n);		// velocity component
					CVector d = tp + (Y + R) * n;	// distance vector between edges
					float dy = Dot(d, n);		// perpendicular space between
					float f1 = dy / vy;

					// Parallel component (breadth control)
					float vx = Cross(v, n);		// velocity component
					float tx = Cross(tp, n);		// distance between centres
					float f2 = (tx - vx * f1) / (X + R);

					if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n));
				}

				if (Cross(v, n) > 0)
				{
					CVector n = CVector(cos(alpha), -sin(alpha));

					// Perpendicular component (oncoming)
					float vy = Dot(v, n);		// velocity component
					CVector d = tp + (X + R) * n;	// distance vector between edges
					float dy = Dot(d, n);		// perpendicular space between
					float f1 = dy / vy;

					// Parallel component (breadth control)
					float vx = Cross(v, n);		// velocity component
					float tx = Cross(tp, n);		// distance between centres
					float f2 = (tx - vx * f1) / (Y + R);

					if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n));
				}

				if (Cross(v, n) < 0)
				{
					CVector n = CVector(-cos(alpha), sin(alpha));

					// Perpendicular component (oncoming)
					float vy = Dot(v, n);		// velocity component
					CVector d = tp + (X + R) * n;	// distance vector between edges
					float dy = Dot(d, n);		// perpendicular space between
					float f1 = dy / vy;

					// Parallel component (breadth control)
					float vx = Cross(v, n);		// velocity component
					float tx = Cross(tp, n);		// distance between centres
					float f2 = (tx - vx * f1) / (Y + R);

					if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
						theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n));
				}

			}
		}
	}

	// Marble Update Call
	theMarble.Update(t);
	for (CSprite* pBumper : theBumper)
		pBumper->Update(t);

	// Kill very slow moving marbles
	if (!theMarble.IsDying() && theMarble.GetSpeed() > 0 && theMarble.GetSpeed() < 2)
		KillMarble();	// kill very slow moving marble

	// Kill the marble if lost of sight
	if (theMarble.GetRight() < 0 || theMarble.GetLeft() > GetWidth() || theMarble.GetTop() < 0)
		KillMarble();

	// Test for hitting Goos
	CSprite *pGooHit = NULL;
	for (CSprite *pGoo : theGoos)
		if (theMarble.HitTest(pGoo))
			pGooHit = pGoo;

	// When just hit a Goo - slow down and die in 2 seconds
	if (pGooHit && !theMarble.IsDying())
	{
		theMarble.SetSpeed(min(200, theMarble.GetSpeed()));
		theMarble.Die(2000);	// just hit
	}
	// else, if dying but not touching a Goo any more - un-die, and the goo will also survive
	else if (theMarble.IsDying() && theMarble.GetTimeToDie() > 100 && !pGooHit)
		theMarble.UnDie();
	// else, if stuck inside the goo, delete the goo and kill the marble
	else if (pGooHit && theMarble.IsDying() && theMarble.GetSpeed() < 2)
	{
		// Killing a Goo!
		pGooHit->Delete();
		theMarble.Die(0);
		theSplashes.push_back(new CSplash(theMarble.GetPosition(), CColor(98, 222, 0), 2.0, 60, 90, GetTime()));
		m_player.Play("goo.wav"); m_player.Volume(1.f);
	}
	theGoos.remove_if(deleted);
	
	// Update the Splashes (if needed)
	for (CSplash *pSplash : theSplashes)
		pSplash->Update(t);
	theSplashes.remove_if(deleted<CSplash*>);

	// Respawn the Marble - when all splashes are gone
	if (theMarble.IsDead() && theSplashes.size() == 0)
		SpawnMarble();

	// Success Test - if no more goos and no more splashes, then the level is complete!
	if (theGoos.size() == 0 && theSplashes.size() == 0)
	{
		m_bLevelCompleted = true;
		NewGame();
	}
}

void CGooeyGame::OnDraw(CGraphics* g)
{
	// Draw Sprites
	g->Blit(CVector(0, 0), theBackground);
	for (CSprite *pWall : theWalls)
		pWall->Draw(g);
	for (CSprite *pGoo : theGoos)
		pGoo->Draw(g);
	if (IsGameMode())
	{
		theButtons[1]->Enable(!theMarble.IsDying() && theMarble.GetSpeed() != 0);
		for (CSpriteButton* pButton : theButtons)
			if (pButton->IsVisible())
				pButton->Draw(g);
	}
	for (CSplash *pSplash : theSplashes)
		pSplash->Draw(g);

	for (CSprite* pBumper : theBumper)
		pBumper->Draw(g);

	theMarble.Draw(g);
	theBarrel.Draw(g);
	theCannon.Draw(g);

	// Draw Power Meter
	float x = (GetShotPower() - MIN_POWER) * thePowerSlider.GetWidth() / (MAX_POWER - MIN_POWER);
	if (x < 0) x = 0;
	if (theMarble.GetSpeed() == 0)
	{
		thePowerMarker.SetSize(x, thePowerSlider.GetHeight());
		thePowerMarker.SetPosition(thePowerSlider.GetPosition() + CVector((x - thePowerSlider.GetWidth()) / 2, 0));
		thePowerMarker.Invalidate();
		thePowerMarker.Draw(g);
		thePowerSlider.Draw(g);
	}

	if (IsGameMode())
		*g << bottom << right << "LEVEL " << m_nCurLevel;
	
	// Draw Menu Items
	if (IsMenuMode())
	{
		g->Blit(CVector(0, 0), theMenuBack);
		g->Blit(CVector((float)GetWidth() - theMenuScreen.GetWidth(), (float)GetHeight() - theMenuScreen.GetHeight()) / 2, theMenuScreen);
		for (CSpriteButton *pButton : theButtonsLevel)
			if (pButton->IsVisible())
				pButton->Draw(g);
		if (m_pCancelButton->IsVisible())
			m_pCancelButton->Draw(g);
	}

	// Draw the Congratulations screen
	if (IsGameOver())
	{
		g->Blit(CVector(0, 0), theMenuBack);
		g->Blit(CVector((float)GetWidth() - theCongratsScreen.GetWidth(), (float)GetHeight() - theCongratsScreen.GetHeight()) / 2, theCongratsScreen);
		m_pCancelButton->Draw(g);
	}
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CGooeyGame::OnInitialize()
{
	m_nMaxLevel = 6;
		
	// Prepare menu background: dark grey, semi-transparent
	Uint32 col = SDL_MapRGBA(theMenuBack.GetSurface()->format, 64, 64, 64, 192);
	SDL_FillRect(theMenuBack.GetSurface(), NULL, col);

	// setup buttons
	theButtons.push_back(new CSpriteButton(50, 695, 80, 30, CMD_MENU, CColor::Black(), CColor(192, 192, 192), "menu", "arial.ttf", 16, GetTime()));
	theButtons.push_back(new CSpriteButton(140, 695, 80, 30, CMD_EXPLODE, CColor::Black(), CColor::LightGray(), "explode", "arial.ttf", 16, GetTime()));

	// setup level buttons
	float x = (GetWidth() - (m_nMaxLevel - 1) * 50) / 2;
	for (int i = 1; i <= m_nMaxLevel; i++)
	{
		CSpriteButton *p = new CSpriteButton(x, 320, 40, 40, CMD_LEVEL + i, CColor::Black(), CColor(192, 192, 192), to_string(i), "arial.ttf", 14, GetTime());
		p->SetSelectedBackColor(CColor(99, 234, 1));
		p->SetSelectedTextColor(CColor(1, 128, 1));
		p->SetSelectedFrameColor(CColor(1, 128, 1));
		theButtonsLevel.push_back(p);
		x += 50;
	}

	// setup cancel button
	m_pCancelButton = new CSpriteButton(1072, 569, 16, 16, CMD_CANCEL, CColor::Black(), CColor(192, 192, 192), "", "arial.ttf", 14, GetTime());
	m_pCancelButton->LoadImages("xcross.png", "xsel.png", "xsel.png");
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CGooeyGame::OnDisplayMenu()
{
	// Spawn the marble and prepare the level buttons for display
	SpawnMarble();

	// Progress the level
	if (m_bLevelCompleted)
	{
		m_nCurLevel++;
		if (m_nCurLevel > m_nMaxLevel)
		{
			GameOver();	// Game Over means the game won!
			m_player.Play("success2.wav"); m_player.Volume(1.f);
			m_pCancelButton->Show(true);
			return;
		}
		else
		{
			m_nUnlockedLevel = max(m_nUnlockedLevel, m_nCurLevel);
			m_player.Play("success1.wav"); m_player.Volume(1.f);
		}
	}

	// Update button states
	for (int nLevel = 1; nLevel <= m_nMaxLevel; nLevel++)
	{
		CSpriteButton* pButton = theButtonsLevel[nLevel-1];
		pButton->Select(nLevel == max(m_nCurLevel, 1));
		pButton->Enable(nLevel <= m_nUnlockedLevel);
		pButton->LoadImages("", "", "", "padlock.png");
	}
	m_pCancelButton->Show(!m_bLevelCompleted);
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CGooeyGame::OnStartGame()
{
}

void CGooeyGame::OnStartLevel(Sint16 nLevel)
{
	if (nLevel == 0) return;	// menu mode...

	// if level not completed, it should be continued (this is when cancel pressed in level selection menu)
	if (!m_bLevelCompleted)
		return;

	// spawn the marble
	SpawnMarble();

	// destroy the old playfield
	for (CSprite *pWall : theWalls) delete pWall;
	theWalls.clear();
	for (CSprite *pGoo : theGoos) delete pGoo;
	theGoos.clear();
	for (CSplash *pSplash : theSplashes) delete pSplash;
	theSplashes.clear();
	for (CSprite* pBumper : theBumper) delete pBumper;
	theBumper.clear();

	// create the new playfield, depending on the current level
	switch (m_nCurLevel)
	{
	// Level 1
	case 1:
		// The walls
		theWalls.push_back(new CSprite(CRectangle(300, 200, 600, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(300, 300, 600, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));

		// The goos
		theGoos.push_back(new CSprite(CRectangle(300, 220, 40, 40), "goo.png", GetTime()));
		theGoos.push_back(new CSprite(CRectangle(520, 320, 40, 40), "goo.png", GetTime()));
		theGoos.push_back(new CSprite(CRectangle(720, 220, 40, 40), "goo.png", GetTime()));
		theGoos.push_back(new CSprite(CRectangle(860, 320, 40, 40), "goo.png", GetTime()));
		
		// The Wild Warpers
		theBumper.push_back(new CSprite(CRectangle(325, 325, 40, 40), "wild.png", GetTime()));
		theBumper.push_back(new CSprite(CRectangle(725, 325, 40, 40), "wild.png", GetTime()));


		break;

	// Level 2
	case 2:
		// The walls
		theWalls.push_back(new CSprite(CRectangle(500, 220, 240, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(500, 220, 20, 220), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(840, 0, 20, 720), "wallvert.bmp", CColor::Blue(), GetTime()));
		// The goos
		theGoos.push_back(new CSprite(CRectangle(560, 240, 40, 40), "goo.png", GetTime()));
		// The Wild Warpers
		theBumper.push_back(new CSprite(CRectangle(660, 240, 40, 40), "wild.png", GetTime()));

		break;

	// Level 3
	case 3:
		// The walls
		theWalls.push_back(new CSprite(CRectangle(745, 267, 597, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-40);
		theWalls.push_back(new CSprite(CRectangle(290, 170, 160, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-90);
		theWalls.push_back(new CSprite(CRectangle(360, 80, 460, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(1139, 580, 260, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-90);
		theWalls.push_back(new CSprite(CRectangle(695, 334, 172, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-126);
		theWalls.push_back(new CSprite(CRectangle(360, 260, 480, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(345, 329, 405, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(-20);

		// The goos
		theGoos.push_back(new CSprite(CRectangle(460, 100, 40, 40), "goo.png", GetTime()));

		// The Wild Warpers
		theBumper.push_back(new CSprite(CRectangle(560, 240, 40, 40), "wild.png", GetTime()));
		theBumper.push_back(new CSprite(CRectangle(560, 340, 40, 40), "wild.png", GetTime()));

		break;

	// Level 4
	case 4:
		// The walls
		theWalls.push_back(new CSprite(CRectangle(899, 40, 540, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(90);
		theWalls.push_back(new CSprite(CRectangle(310, 150, 280, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.back()->Rotate(90);
		theWalls.push_back(new CSprite(CRectangle(440, 300, 480, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(460, 140, 420, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(440, 0, 720, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));

		// The goos
		theGoos.push_back(new CSprite(CRectangle(560, 160, 40, 40), "goo.png", GetTime()));
		theGoos.push_back(new CSprite(CRectangle(780, 160, 40, 40), "goo.png", GetTime()));
		theGoos.push_back(new CSprite(CRectangle(780, 20, 40, 40), "goo.png", GetTime()));
		theGoos.push_back(new CSprite(CRectangle(560, 20, 40, 40), "goo.png", GetTime()));

		theBumper.push_back(new CSprite(CRectangle(560, 40, 40, 40), "wild.png", GetTime()));
		theBumper.push_back(new CSprite(CRectangle(560, 240, 60, 40), "wild.png", GetTime()));
		break;

	// Level 5:
	case 5:
		// The walls
		theWalls.push_back(new CSprite(CRectangle(580, 0, 300, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(580, 20, 20, 440), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(720, 20, 20, 520), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(860, 20, 20, 440), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(560, 460, 40, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(860, 460, 40, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		// The goos
		theGoos.push_back(new CSprite(CRectangle(560, 480, 40, 40), "goo.png", GetTime()));
		theGoos.push_back(new CSprite(CRectangle(860, 480, 40, 40), "goo.png", GetTime()));
		theGoos.push_back(new CSprite(CRectangle(640, 20, 40, 40), "goo.png", GetTime()));
		theGoos.push_back(new CSprite(CRectangle(780, 20, 40, 40), "goo.png", GetTime()));

		theBumper.push_back(new CSprite(CRectangle(560, 240, 40, 40), "wild.png", GetTime()));
		theBumper.push_back(new CSprite(CRectangle(660, 240, 40, 40), "wild.png", GetTime()));

		break;

	// Level 6
	case 6:
		// The walls
		theWalls.push_back(new CSprite(CRectangle(180, 20, 1100, 20), "wallhorz.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(640, 40, 20, 400), "wallvert.bmp", CColor::Blue(), GetTime()));
		theWalls.push_back(new CSprite(CRectangle(640, 520, 20, 200), "wallvert.bmp", CColor::Blue(), GetTime()));
		// The goos
		theGoos.push_back(new CSprite(CRectangle(1160, 40, 40, 40), "goo.png", GetTime()));

		theBumper.push_back(new CSprite(CRectangle(725, 325, 40, 40), "wild.png", GetTime()));
		theBumper.push_back(new CSprite(CRectangle(660, 240, 40, 40), "wild.png", GetTime()));
		theBumper.push_back(new CSprite(CRectangle(560, 240, 40, 40), "wild.png", GetTime()));


		break;
	}

	m_bLevelCompleted = false;
}

// called when the game is over
void CGooeyGame::OnGameOver()
{
}

// one time termination code
void CGooeyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CGooeyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
}

void CGooeyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// On-Screen Buttons Handler

void CGooeyGame::OnButton(Uint16 nCmdId)
{
	if (IsGameOver())
	{
		// Cancel button clicked in the final screen
		if (nCmdId == CMD_CANCEL)
		{
			m_nCurLevel = 0;
			NewGame();
		}
	}
	else if (nCmdId >= CMD_LEVEL)
	{
		// New level selected from the menu
		m_bLevelCompleted = true;	// pretend previous level completed even if not
		m_nCurLevel = nCmdId - CMD_LEVEL;
		StartGame();
	}
	else
		switch (nCmdId)
		{
		case CMD_CANCEL: StartGame(); break; // cancel button in level selection - will go back to the game
		case CMD_MENU: NewGame(); break;	// proceed to menu without completing the level
		case CMD_EXPLODE: KillMarble(); break;
		}
}

/////////////////////////////////////////////////////
// Mouse Events Handlers

void CGooeyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
	// Find out if any button is pressed and which one
	m_pButtonPressed = NULL;
	if (IsGameMode())
	{
		for (CSpriteButton* pButton : theButtons)
			if (pButton->IsVisible() && pButton->IsEnabled() && pButton->HitTest(x, y))
				m_pButtonPressed = pButton;
	}
	else
	{
		for (CSpriteButton* pButton : theButtonsLevel)
			if (pButton->IsVisible() && pButton->IsEnabled() && pButton->HitTest(x, y))
				m_pButtonPressed = pButton;
		if (m_pCancelButton->IsVisible() && m_pCancelButton->IsEnabled() && m_pCancelButton->HitTest(x, y))
			m_pButtonPressed = m_pCancelButton;
	}

	// In game mode, if the marble isn't moving yet, the mouse click will start aiming mode
	if (IsGameMode() && !m_pButtonPressed)
	{
		theBarrel.SetRotation(theBarrel.GetY() - y, x - theBarrel.GetX());
		if (theMarble.GetSpeed() == 0)
			BeginAim();
	}
}

void CGooeyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	// Control hovering over on-screen buttons
	if (IsGameMode())
	{
		for (CSpriteButton* pButton : theButtons)
			pButton->Hover(pButton->IsVisible() && pButton->IsEnabled() && pButton->HitTest(x, y));
	}
	else
	{
		for (CSpriteButton* pButton : theButtonsLevel)
			pButton->Hover(pButton->IsVisible() && pButton->IsEnabled() && pButton->HitTest(x, y));
		m_pCancelButton->Hover(m_pCancelButton->IsVisible() && m_pCancelButton->IsEnabled() && m_pCancelButton->HitTest(x, y));
	}

	// In game mode, rotate the cannon's barrel
	if (IsGameMode() && !m_pButtonPressed)
		theBarrel.SetRotation(theBarrel.GetY() - y, x - theBarrel.GetX());
}

void CGooeyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	// If on-screen button was pressed, handle this button (OnButton function above)
	if (m_pButtonPressed)
	{
		if (m_pButtonPressed->IsEnabled() && m_pButtonPressed->HitTest(x, y))
			OnButton(m_pButtonPressed->GetCmd());
		m_pButtonPressed = NULL;
	}
	// In game mode, rotate the cannon and, if additionally in aiming mode, shoot!
	else if (IsGameMode())
	{
		theBarrel.SetRotation(theBarrel.GetY() - y, x - theBarrel.GetX());

		if (IsAiming())
		{
			float P = Shoot();	// read the shooting power
			if (P > 0)
			{
				// create the nozzle-rotated vector and shoot the marble!
				CVector nozzle(95, 0);
				theBarrel.LtoG(nozzle, true);
				theMarble.SetPosition(nozzle);
				theMarble.Accelerate(P * Normalize(CVector(x, y) - theBarrel.GetPosition()));
			}
		}
	}
}

void CGooeyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CGooeyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CGooeyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CGooeyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
