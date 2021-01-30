#pragma once
#include "Game_Runtime/Game.h"


InsightGame::InsightGame()
{
}

InsightGame::~InsightGame()
{
}


GAME_API void* CreateGameInstance()
{
	return static_cast<void*>(new InsightGame());
}

int InsightGame::TESTGetVal()
{
	return 1;
}

Insight::ModuleLoadStatus InsightGame::GetLoadStatus()
{
	return Insight::ModuleLoadStatus::Success;
}
