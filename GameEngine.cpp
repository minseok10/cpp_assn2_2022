#include "GameEngine.h"
#include "randoms.h"

#include <cstdlib>
#include <ctime>

GameEngine::GameEngine()
	: m_players{ Player(0), Player(1), Player(2), Player(3) },
	  m_currentPlayer(0)
{
	rebuildState();
}

void GameEngine::start()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	Player::deadcnt = 0;
	for (int i = 0; i < 4; i++) {
		m_players[i].reset(i);
	}

	m_currentPlayer = 0;
	m_state = GameState{};
	m_state.gameStarted = true;
	dealCards();
	appendLog("New game started.");
	rebuildState();
}

const GameState& GameEngine::state() const
{
	return m_state;
}

bool GameEngine::income()
{
	if (!m_state.gameStarted || m_state.gameOver || m_currentPlayer != 0 || m_players[0].dead()) {
		return false;
	}

	m_players[0].coinplus(1);
	appendLog("Player gains 1 coin from Income.");
	advanceTurn();
	runComputerTurns();
	rebuildState();
	return true;
}

bool GameEngine::foreignAid()
{
	if (!m_state.gameStarted || m_state.gameOver || m_currentPlayer != 0 || m_players[0].dead()) {
		return false;
	}

	m_players[0].coinplus(2);
	appendLog("Player gains 2 coins from Foreign Aid. Blocks are TODO.");
	advanceTurn();
	runComputerTurns();
	rebuildState();
	return true;
}

bool GameEngine::coup(int target)
{
	if (!m_state.gameStarted || m_state.gameOver || m_currentPlayer != 0 || m_players[0].dead()) {
		return false;
	}
	if (target < 1 || target > 3 || m_players[target].dead() || m_players[0].coins() < 7) {
		return false;
	}

	bool defeated = m_players[0].coup(target, m_players);
	appendLog("Player launches a Coup against Computer " + std::to_string(target) + ".");
	if (defeated) {
		appendLog("Computer " + std::to_string(target) + " is eliminated.");
	}
	advanceTurn();
	runComputerTurns();
	rebuildState();
	return true;
}

void GameEngine::dealCards()
{
	int cards[12] = { 0,0,0,1,1,1,2,2,2,3,3,3 };
	shuffleCards(cards);
	m_players[0].setCard(cards[0], cards[1]);
	m_players[1].setCard(cards[2], cards[3]);
	m_players[2].setCard(cards[4], cards[5]);
	m_players[3].setCard(cards[6], cards[7]);
	m_deck.setCard(cards[8], cards[9], cards[10], cards[11]);
}

void GameEngine::shuffleCards(int(&cards)[12])
{
	for (int i = 0; i < 100; i++) {
		int a = std::rand() % 12;
		int b = std::rand() % 12;
		int temp = cards[a];
		cards[a] = cards[b];
		cards[b] = temp;
	}
}

void GameEngine::rebuildState()
{
	m_state.players.clear();
	for (int i = 0; i < 4; i++) {
		PlayerState player;
		player.index = i;
		player.coins = m_players[i].coins();
		player.dead = m_players[i].dead();
		player.human = i == 0;
		for (int card = 0; card < 2; card++) {
			CardState cardState;
			cardState.open = m_players[i].cardOpen(card) != 0;
			if (i == 0 || cardState.open) {
				cardState.name = m_players[i].cardName(card);
			}
			else {
				cardState.name = "????";
			}
			player.cards.push_back(cardState);
		}
		m_state.players.push_back(player);
	}

	m_state.currentPlayer = m_currentPlayer;
	checkWinner();
}

void GameEngine::appendLog(const std::string& message)
{
	m_state.log.push_back(message);
	if (m_state.log.size() > 40) {
		m_state.log.erase(m_state.log.begin());
	}
}

void GameEngine::advanceTurn()
{
	if (aliveCount() <= 1) {
		checkWinner();
		return;
	}

	do {
		m_currentPlayer = (m_currentPlayer + 1) % 4;
	} while (m_players[m_currentPlayer].dead());
}

void GameEngine::runComputerTurns()
{
	while (!m_state.gameOver && m_currentPlayer != 0) {
		runComputerAction();
		advanceTurn();
		checkWinner();
	}
}

void GameEngine::runComputerAction()
{
	if (m_players[m_currentPlayer].coins() >= 7 && aliveCount() > 1) {
		int target = randomOpponent(m_currentPlayer);
		bool defeated = m_players[m_currentPlayer].coup(target, m_players);
		appendLog(playerName(m_currentPlayer) + " launches a Coup against " + playerName(target) + ".");
		if (defeated) {
			appendLog(playerName(target) + " is eliminated.");
		}
		return;
	}

	if (prob(2) == 0) {
		m_players[m_currentPlayer].coinplus(1);
		appendLog(playerName(m_currentPlayer) + " gains 1 coin from Income.");
	}
	else {
		m_players[m_currentPlayer].coinplus(2);
		appendLog(playerName(m_currentPlayer) + " gains 2 coins from Foreign Aid. Blocks are TODO.");
	}
}

void GameEngine::checkWinner()
{
	if (aliveCount() > 1) {
		m_state.gameOver = false;
		m_state.winner = -1;
		return;
	}

	for (int i = 0; i < 4; i++) {
		if (!m_players[i].dead()) {
			bool alreadyOver = m_state.gameOver && m_state.winner == i;
			m_state.gameOver = true;
			m_state.winner = i;
			if (!alreadyOver) {
				appendLog(playerName(i) + " wins the game.");
			}
			return;
		}
	}
}

int GameEngine::aliveCount() const
{
	int count = 0;
	for (int i = 0; i < 4; i++) {
		if (!m_players[i].dead()) {
			count++;
		}
	}
	return count;
}

int GameEngine::randomOpponent(int self) const
{
	int target;
	do {
		target = prob(4);
	} while (target == self || m_players[target].dead());

	return target;
}

std::string GameEngine::playerName(int index) const
{
	if (index == 0) {
		return "Player";
	}

	return "Computer " + std::to_string(index);
}
