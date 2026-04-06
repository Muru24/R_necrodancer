#pragma once
#include <iostream>
#include <vector>
#include "Struct.h"

std::vector<Vector2> Slime_Default_Patten =
{
	{-1,0},
	{0,1},
	{1,0},
	{0,-1} 
};

std::vector<Vector2> Slime_X_Patten =
{
	{1,-1},
	{-1,1},
	{-1,-1},
	{1,1},
	{-1,1},
	{1,-1},
	{1,1},
	{-1,-1}
};

std::vector<std::vector<Vector2>> Slime_Move_Pattens =
{
	Slime_Default_Patten, Slime_X_Patten
};
