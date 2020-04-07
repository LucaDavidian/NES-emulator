#include "RenderingSystem.h"
#include "GraphicsSystem.h"
#include "SDL.h"
#include "Error.h"

RenderingSystem &RenderingSystem::GetInstance()
{
	static RenderingSystem instance;

	return instance;
}

RenderingSystem::RenderingSystem()
{
	
}

RenderingSystem::~RenderingSystem() 
{ 
	
}

void RenderingSystem::Render()
{
	
}