#pragma once

#include "Deck.h"
#include "GameState.h"
#include "Player.h"

#include <string>

class GameEngine
{
public:
	GameEngine();

	void start();
	const GameState& state() const;

	bool income();
	bool foreignAid();
	bool tax();
	bool assassinate(int target);
	bool steal(int target);
	bool coup(int target);
	bool answerChallenge(bool challenge);
	bool answerCounter(bool counter);

private:
	enum class ActionKind {
		None,
		ForeignAid,
		Tax,
		Assassinate,
		Steal
	};

	enum class PendingType {
		None,
		ChallengeClaim,
		ChallengeBlock,
		CounterBlock
	};

	struct PendingDecision {
		PendingType type = PendingType::None;
		ActionKind action = ActionKind::None;
		int actor = -1;
		int target = -1;
		int blocker = -1;
		int card = -1;
		std::string prompt;
	};

	Player m_players[4];
	Deck m_deck;
	GameState m_state;
	PendingDecision m_pending;
	int m_currentPlayer;

	void dealCards();
	void shuffleCards(int(&cards)[12]);
	void rebuildState();
	void appendLog(const std::string& message);

	bool canHumanAct() const;
	bool mustCoup(int actor) const;
	void finishHumanAction();
	void finishComputerAction();
	void advanceTurn();
	void runComputerTurns();
	void runComputerAction();

	void startPlayerClaim(ActionKind action, int target, int card);
	void continuePlayerAction(ActionKind action, int target);
	void blockPlayerAction(ActionKind action, int blocker, int card);
	void resolveBlockedPlayerAction(bool blockHonest);

	void startComputerClaim(ActionKind action, int target, int card);
	void continueComputerClaimAfterHumanPass();
	void continueComputerAction(ActionKind action, int target);
	void startComputerForeignAid();
	void continueComputerForeignAidAfterHumanPass();
	void resolveHumanCounter(bool counter);

	bool resolveChallenge(int claimant, int card, int challenger);
	bool openRandomCard(int player);
	void exchangeClaimedCard(int player, int card);
	int findPlayerForeignAidBlocker(int actor);
	int findRandomChallenger(int actor);
	int findComputerActionTarget(int actor) const;
	int aliveCount() const;
	void checkWinner();
	void clearPending();
	void setPending(PendingType type, ActionKind action, int actor, int target, int blocker, int card, const std::string& prompt);
	std::string playerName(int index) const;
	std::string cardName(int card) const;
	std::string actionName(ActionKind action) const;
};
