#pragma once

#include "Insight/Runtime/Game_Framework/Game_Core.h"

class GAME_API InsightGame : public Insight::IGame
{
public:
	InsightGame();
	virtual ~InsightGame();

	virtual void OnAppUpdate() override
	{

	}

	virtual int TESTGetVal() override;


	virtual Insight::ModuleLoadStatus GetLoadStatus();
protected:

};

GAME_API void* CreateGameInstance();

