#include "stdafx.h"
#include "GooeyGame.h"

CGooeyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	//app.OpenConsole();

	app.OpenWindow(1280, 720, "Gooey Cannon");
	//app.OpenFullScreen(1280, 720, 24);

	app.SetClearColor(CColor::White());
	app.Run(&game);
	return(0);
}
