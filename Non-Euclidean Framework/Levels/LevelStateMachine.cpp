#include "stdafx.h"
#include "LevelStateMachine.h"

LevelStateMachine::LevelStateMachine() : levels(), currentLevel( 0 ) { }

LevelStateMachine::~LevelStateMachine() { }

void LevelStateMachine::Update( const float dt )
{
	if ( currentLevel )
		currentLevel->Update( dt );
}

void LevelStateMachine::BeginRender()
{
	if ( currentLevel )
		currentLevel->BeginFrame();
}

void LevelStateMachine::Render()
{
	if ( currentLevel )
		currentLevel->RenderFrame();
}

void LevelStateMachine::SpawnWindows()
{
	if ( currentLevel )
		currentLevel->SpawnWindows();
}

std::string LevelStateMachine::Add( std::shared_ptr<LevelContainer> level )
{
	std::string levelName = level->GetLevelName();
	levels.insert( std::make_pair(levelName, level ) );
	return levelName;
}

void LevelStateMachine::Remove( std::string levelName )
{
	auto it = levels.find( levelName );
	if ( it != levels.end() )
		if ( currentLevel == it->second )
			currentLevel = nullptr;
	levels.erase( it );
}

void LevelStateMachine::SwitchTo( std::string levelName )
{
	auto it = levels.find( levelName );
	if ( it != levels.end() )
	{
		if ( currentLevel )
			currentLevel->CleanUp();

		currentLevel = it->second;
		if ( !currentLevel->GetIsCreated() )
		{
			currentLevel->OnCreate();
			currentLevel->SetIsCreated( true );
		}
		currentLevel->OnSwitch();
	}
}