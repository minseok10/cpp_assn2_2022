#pragma once

#include <string>
#include <vector>

struct CardState
{
	std::string name;
	bool open = false;
};

struct PlayerState
{
	int index = 0;
	int coins = 0;
	bool dead = false;
	bool human = false;
	std::vector<CardState> cards;
};

struct GameState
{
	std::vector<PlayerState> players;
	std::vector<std::string> log;
	std::string pendingType;
	std::string pendingPrompt;
	int currentPlayer = 0;
	int winner = -1;
	bool gameStarted = false;
	bool gameOver = false;
	bool waitingForHuman = false;
};
