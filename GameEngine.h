#pragma once

#include "Deck.h"
#include "GameState.h"
#include "Player.h"

#include <string>
#include <vector>

class GameEngine
{
public:
	GameEngine();

	void start();
	const GameState& state() const;

	bool income();
	bool foreignAid();
	bool coup(int target);

private:
	Player m_players[4];
	Deck m_deck;
	GameState m_state;
	int m_currentPlayer;

	void dealCards();
	void shuffleCards(int(&cards)[12]);
	void rebuildState();
	void appendLog(const std::string& message);
	void advanceTurn();
	void runComputerTurns();
	void runComputerAction();
	void checkWinner();
	int aliveCount() const;
	int randomOpponent(int self) const;
	std::string playerName(int index) const;
};
