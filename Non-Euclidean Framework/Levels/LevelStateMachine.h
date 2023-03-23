#pragma once
#ifndef LEVELSTATEMACHINE_H
#define LEVELSTATEMACHINE_H

#include "LevelContainer.h"

class LevelStateMachine
{
public:
	LevelStateMachine();
	~LevelStateMachine();

	void BeginRender();
	void Render();
	void Update( const float dt );
	void SpawnWindows();

	std::shared_ptr<LevelContainer> GetCurrentLevel() const noexcept { return currentLevel; }
	std::string Add (std::shared_ptr<LevelContainer> level);
	void Remove( std::string levelName );
	void SwitchTo( std::string levelName );

private:
	uint32_t insertedLevelID;
	std::shared_ptr<LevelContainer> currentLevel;
	std::map<std::string, std::shared_ptr<LevelContainer>> levels;
};

#endif