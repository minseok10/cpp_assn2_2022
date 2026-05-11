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

	m_state = GameState{};
	clearPending();
	m_currentPlayer = 0;
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
	if (!canHumanAct() || mustCoup(0)) {
		return false;
	}

	m_players[0].coinplus(1);
	appendLog("Player gains 1 coin from Income.");
	finishHumanAction();
	return true;
}

bool GameEngine::foreignAid()
{
	if (!canHumanAct() || mustCoup(0)) {
		return false;
	}

	appendLog("Player chooses Foreign Aid.");
	int blocker = findPlayerForeignAidBlocker(0);
	if (blocker == 0) {
		m_players[0].coinplus(2);
		appendLog("No one blocks Foreign Aid. Player gains 2 coins.");
		finishHumanAction();
	}
	else {
		blockPlayerAction(ActionKind::ForeignAid, blocker, 0);
	}
	return true;
}

bool GameEngine::tax()
{
	if (!canHumanAct() || mustCoup(0)) {
		return false;
	}

	startPlayerClaim(ActionKind::Tax, -1, 0);
	return true;
}

bool GameEngine::assassinate(int target)
{
	if (!canHumanAct() || mustCoup(0) || target < 1 || target > 3 || m_players[target].dead() || m_players[0].coins() < 3) {
		return false;
	}

	startPlayerClaim(ActionKind::Assassinate, target, 1);
	return true;
}

bool GameEngine::steal(int target)
{
	if (!canHumanAct() || mustCoup(0) || target < 1 || target > 3 || m_players[target].dead()) {
		return false;
	}

	startPlayerClaim(ActionKind::Steal, target, 2);
	return true;
}

bool GameEngine::coup(int target)
{
	if (!canHumanAct() || target < 1 || target > 3 || m_players[target].dead() || m_players[0].coins() < 7) {
		return false;
	}

	bool defeated = m_players[0].coup(target, m_players);
	appendLog("Player launches a Coup against Computer " + std::to_string(target) + ".");
	if (defeated) {
		appendLog("Computer " + std::to_string(target) + " is eliminated.");
	}
	finishHumanAction();
	return true;
}

bool GameEngine::answerChallenge(bool challenge)
{
	if (m_pending.type == PendingType::ChallengeClaim) {
		PendingDecision pending = m_pending;
		if (challenge) {
			clearPending();
			bool honest = resolveChallenge(pending.actor, pending.card, 0);
			if (honest) {
				continueComputerAction(pending.action, pending.target);
			}
			else {
				finishComputerAction();
			}
		}
		else {
			continueComputerClaimAfterHumanPass();
		}
		if (m_pending.type == PendingType::None && !m_state.gameOver && m_currentPlayer != 0) {
			advanceTurn();
			runComputerTurns();
		}
		rebuildState();
		return true;
	}

	if (m_pending.type == PendingType::ChallengeBlock) {
		PendingDecision pending = m_pending;
		if (challenge) {
			bool honest = resolveChallenge(pending.blocker, pending.card, 0);
			resolveBlockedPlayerAction(honest);
		}
		else {
			clearPending();
			appendLog("Player does not challenge the block. " + actionName(pending.action) + " is blocked.");
			finishHumanAction();
		}
		rebuildState();
		return true;
	}

	return false;
}

bool GameEngine::answerCounter(bool counter)
{
	if (m_pending.type != PendingType::CounterBlock) {
		return false;
	}

	PendingDecision pending = m_pending;
	resolveHumanCounter(counter);
	if (m_pending.type == PendingType::None && !m_state.gameOver && m_currentPlayer != 0) {
		advanceTurn();
		runComputerTurns();
	}
	rebuildState();
	return pending.type == PendingType::CounterBlock;
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
			cardState.name = (i == 0 || cardState.open) ? m_players[i].cardName(card) : "????";
			player.cards.push_back(cardState);
		}
		m_state.players.push_back(player);
	}

	checkWinner();
	m_state.currentPlayer = m_currentPlayer;
	m_state.waitingForHuman = m_pending.type != PendingType::None;
	m_state.pendingPrompt = m_pending.prompt;
	switch (m_pending.type) {
	case PendingType::ChallengeClaim:
	case PendingType::ChallengeBlock:
		m_state.pendingType = "challenge";
		break;
	case PendingType::CounterBlock:
		m_state.pendingType = "counter";
		break;
	default:
		m_state.pendingType.clear();
		break;
	}
}

void GameEngine::appendLog(const std::string& message)
{
	m_state.log.push_back(message);
	if (m_state.log.size() > 60) {
		m_state.log.erase(m_state.log.begin());
	}
}

bool GameEngine::canHumanAct() const
{
	return m_state.gameStarted && !m_state.gameOver && m_pending.type == PendingType::None
		&& m_currentPlayer == 0 && !m_players[0].dead();
}

bool GameEngine::mustCoup(int actor) const
{
	return m_players[actor].coins() > 9;
}

void GameEngine::finishHumanAction()
{
	checkWinner();
	if (!m_state.gameOver) {
		advanceTurn();
		runComputerTurns();
	}
	rebuildState();
}

void GameEngine::finishComputerAction()
{
	checkWinner();
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
	while (!m_state.gameOver && m_pending.type == PendingType::None && m_currentPlayer != 0) {
		runComputerAction();
		if (m_pending.type == PendingType::None && !m_state.gameOver) {
			advanceTurn();
			checkWinner();
		}
	}
}

void GameEngine::runComputerAction()
{
	int actor = m_currentPlayer;
	appendLog(playerName(actor) + "'s turn.");
	if (mustCoup(actor)) {
		int target = findComputerActionTarget(actor);
		bool defeated = m_players[actor].coup(target, m_players);
		appendLog(playerName(actor) + " must Coup and targets " + playerName(target) + ".");
		if (defeated) {
			appendLog(playerName(target) + " is eliminated.");
		}
		return;
	}

	if (prob(2)) {
		int choice = m_players[actor].coins() > 6 ? prob(3) + 1 : prob(2) + 1;
		if (choice == 1) {
			m_players[actor].coinplus(1);
			appendLog(playerName(actor) + " gains 1 coin from Income.");
		}
		else if (choice == 2) {
			startComputerForeignAid();
		}
		else {
			int target = findComputerActionTarget(actor);
			bool defeated = m_players[actor].coup(target, m_players);
			appendLog(playerName(actor) + " launches a Coup against " + playerName(target) + ".");
			if (defeated) {
				appendLog(playerName(target) + " is eliminated.");
			}
		}
		return;
	}

	int card = m_players[actor].coins() < 3 ? prob(2) : prob(3);
	if (m_players[actor].coins() < 3 && card == 1) {
		card = 2;
	}

	if (card == 0) {
		startComputerClaim(ActionKind::Tax, -1, 0);
	}
	else if (card == 1) {
		startComputerClaim(ActionKind::Assassinate, findComputerActionTarget(actor), 1);
	}
	else {
		startComputerClaim(ActionKind::Steal, findComputerActionTarget(actor), 2);
	}
}

void GameEngine::startPlayerClaim(ActionKind action, int target, int card)
{
	appendLog("Player claims " + cardName(card) + " for " + actionName(action) + ".");
	int challenger = findRandomChallenger(0);
	if (challenger != 0) {
		bool honest = resolveChallenge(0, card, challenger);
		if (!honest) {
			finishHumanAction();
			return;
		}
	}
	else {
		appendLog("No one challenges Player.");
	}

	continuePlayerAction(action, target);
}

void GameEngine::continuePlayerAction(ActionKind action, int target)
{
	if (action == ActionKind::Tax) {
		m_players[0].coinplus(3);
		appendLog("Player gains 3 coins from Duke.");
		finishHumanAction();
		return;
	}

	if (action == ActionKind::Assassinate) {
		bool block = m_players[target].cardhave(3) || !prob(5);
		if (block) {
			blockPlayerAction(action, target, 3);
		}
		else {
			bool defeated = openRandomCard(target);
			m_players[0].coinplus(-3);
			appendLog("Player assassinates Computer " + std::to_string(target) + ".");
			if (defeated) {
				appendLog("Computer " + std::to_string(target) + " is eliminated.");
			}
			finishHumanAction();
		}
		return;
	}

	if (action == ActionKind::Steal) {
		bool block = m_players[target].cardhave(2) || !prob(5);
		if (block) {
			blockPlayerAction(action, target, 2);
		}
		else {
			int taken = m_players[target].takeCoins(2);
			m_players[0].coinplus(taken);
			appendLog("Player steals " + std::to_string(taken) + " coins from Computer " + std::to_string(target) + ".");
			finishHumanAction();
		}
		return;
	}

	if (action == ActionKind::ForeignAid) {
		m_players[0].coinplus(2);
		appendLog("Player gains 2 coins from Foreign Aid.");
		finishHumanAction();
	}
}

void GameEngine::blockPlayerAction(ActionKind action, int blocker, int card)
{
	appendLog(playerName(blocker) + " blocks Player's " + actionName(action) + " with " + cardName(card) + ".");
	setPending(
		PendingType::ChallengeBlock,
		action,
		0,
		action == ActionKind::ForeignAid ? -1 : blocker,
		blocker,
		card,
		"Challenge " + playerName(blocker) + "'s " + cardName(card) + " block?");
	rebuildState();
}

void GameEngine::resolveBlockedPlayerAction(bool blockHonest)
{
	PendingDecision pending = m_pending;
	clearPending();
	if (blockHonest) {
		appendLog(playerName(pending.blocker) + "'s block succeeds. " + actionName(pending.action) + " fails.");
		finishHumanAction();
		return;
	}

	appendLog(playerName(pending.blocker) + "'s block fails.");
	continuePlayerAction(pending.action, pending.target);
}

void GameEngine::startComputerClaim(ActionKind action, int target, int card)
{
	int actor = m_currentPlayer;
	appendLog(playerName(actor) + " claims " + cardName(card) + " for " + actionName(action) + ".");
	if (!m_players[0].dead()) {
		setPending(
			PendingType::ChallengeClaim,
			action,
			actor,
			target,
			-1,
			card,
			"Challenge " + playerName(actor) + "'s " + cardName(card) + " claim?");
		rebuildState();
		return;
	}

	setPending(PendingType::ChallengeClaim, action, actor, target, -1, card, "");
	continueComputerClaimAfterHumanPass();
}

void GameEngine::continueComputerClaimAfterHumanPass()
{
	PendingDecision pending = m_pending;
	clearPending();
	int challenger = findRandomChallenger(pending.actor);
	if (challenger != pending.actor) {
		bool honest = resolveChallenge(pending.actor, pending.card, challenger);
		if (!honest) {
			finishComputerAction();
			return;
		}
	}
	else {
		appendLog("No one challenges " + playerName(pending.actor) + ".");
	}

	continueComputerAction(pending.action, pending.target);
}

void GameEngine::continueComputerAction(ActionKind action, int target)
{
	int actor = m_currentPlayer;
	if (action == ActionKind::Tax) {
		m_players[actor].coinplus(3);
		appendLog(playerName(actor) + " gains 3 coins from Duke.");
		finishComputerAction();
		return;
	}

	if (action == ActionKind::Assassinate) {
		if (target == 0 && !m_players[0].dead()) {
			setPending(
				PendingType::CounterBlock,
				action,
				actor,
				target,
				0,
				3,
				"Block " + playerName(actor) + "'s assassination with Contessa?");
			rebuildState();
			return;
		}

		bool block = m_players[target].cardhave(3) || !prob(5);
		if (block) {
			appendLog(playerName(target) + " blocks assassination with Contessa.");
			if (!prob(5)) {
				bool honest = resolveChallenge(target, 3, actor);
				if (!honest) {
					bool defeated = openRandomCard(target);
					m_players[actor].coinplus(-3);
					appendLog(playerName(actor) + " assassinates " + playerName(target) + ".");
					if (defeated) {
						appendLog(playerName(target) + " is eliminated.");
					}
				}
				else {
					appendLog("Assassination is blocked.");
				}
			}
			else {
				appendLog(playerName(actor) + " does not challenge. Assassination is blocked.");
			}
			finishComputerAction();
			return;
		}

		bool defeated = openRandomCard(target);
		m_players[actor].coinplus(-3);
		appendLog(playerName(actor) + " assassinates " + playerName(target) + ".");
		if (defeated) {
			appendLog(playerName(target) + " is eliminated.");
		}
		finishComputerAction();
		return;
	}

	if (action == ActionKind::Steal) {
		if (target == 0 && !m_players[0].dead()) {
			setPending(
				PendingType::CounterBlock,
				action,
				actor,
				target,
				0,
				2,
				"Block " + playerName(actor) + "'s steal with Captain?");
			rebuildState();
			return;
		}

		bool block = m_players[target].cardhave(2) || !prob(5);
		if (block) {
			appendLog(playerName(target) + " blocks steal with Captain.");
			if (!prob(5)) {
				bool honest = resolveChallenge(target, 2, actor);
				if (!honest) {
					int taken = m_players[target].takeCoins(2);
					m_players[actor].coinplus(taken);
					appendLog(playerName(actor) + " steals " + std::to_string(taken) + " coins from " + playerName(target) + ".");
				}
				else {
					appendLog("Steal is blocked.");
				}
			}
			else {
				appendLog(playerName(actor) + " does not challenge. Steal is blocked.");
			}
			finishComputerAction();
			return;
		}

		int taken = m_players[target].takeCoins(2);
		m_players[actor].coinplus(taken);
		appendLog(playerName(actor) + " steals " + std::to_string(taken) + " coins from " + playerName(target) + ".");
		finishComputerAction();
	}
}

void GameEngine::startComputerForeignAid()
{
	int actor = m_currentPlayer;
	appendLog(playerName(actor) + " chooses Foreign Aid.");
	if (!m_players[0].dead()) {
		setPending(
			PendingType::CounterBlock,
			ActionKind::ForeignAid,
			actor,
			-1,
			0,
			0,
			"Block " + playerName(actor) + "'s Foreign Aid with Duke?");
		rebuildState();
		return;
	}

	setPending(PendingType::CounterBlock, ActionKind::ForeignAid, actor, -1, 0, 0, "");
	continueComputerForeignAidAfterHumanPass();
}

void GameEngine::continueComputerForeignAidAfterHumanPass()
{
	PendingDecision pending = m_pending;
	clearPending();
	int blocker = findPlayerForeignAidBlocker(pending.actor);
	if (blocker == pending.actor) {
		m_players[pending.actor].coinplus(2);
		appendLog("No one blocks Foreign Aid. " + playerName(pending.actor) + " gains 2 coins.");
		finishComputerAction();
		return;
	}

	appendLog(playerName(blocker) + " blocks " + playerName(pending.actor) + "'s Foreign Aid with Duke.");
	if (!prob(5)) {
		bool honest = resolveChallenge(blocker, 0, pending.actor);
		if (!honest) {
			m_players[pending.actor].coinplus(2);
			appendLog("Foreign Aid succeeds after the block challenge.");
		}
		else {
			appendLog("Foreign Aid is blocked.");
		}
	}
	else {
		appendLog(playerName(pending.actor) + " does not challenge. Foreign Aid is blocked.");
	}
	finishComputerAction();
}

void GameEngine::resolveHumanCounter(bool counter)
{
	PendingDecision pending = m_pending;
	if (!counter) {
		appendLog("Player does not block.");
		if (pending.action == ActionKind::ForeignAid) {
			continueComputerForeignAidAfterHumanPass();
		}
		else {
			clearPending();
			if (pending.action == ActionKind::Assassinate) {
				bool defeated = openRandomCard(pending.target);
				m_players[pending.actor].coinplus(-3);
				appendLog(playerName(pending.actor) + " assassinates " + playerName(pending.target) + ".");
				if (defeated) {
					appendLog(playerName(pending.target) + " is eliminated.");
				}
			}
			else if (pending.action == ActionKind::Steal) {
				int taken = m_players[pending.target].takeCoins(2);
				m_players[pending.actor].coinplus(taken);
				appendLog(playerName(pending.actor) + " steals " + std::to_string(taken) + " coins from " + playerName(pending.target) + ".");
			}
			finishComputerAction();
		}
		return;
	}

	clearPending();
	appendLog("Player blocks " + playerName(pending.actor) + "'s " + actionName(pending.action) + " with " + cardName(pending.card) + ".");
	if (!prob(5)) {
		bool honest = resolveChallenge(0, pending.card, pending.actor);
		if (!honest) {
			if (pending.action == ActionKind::ForeignAid) {
				m_players[pending.actor].coinplus(2);
				appendLog("Foreign Aid succeeds after Player's failed block.");
			}
			else {
				if (pending.action == ActionKind::Assassinate) {
					bool defeated = openRandomCard(pending.target);
					m_players[pending.actor].coinplus(-3);
					appendLog(playerName(pending.actor) + " assassinates " + playerName(pending.target) + ".");
					if (defeated) {
						appendLog(playerName(pending.target) + " is eliminated.");
					}
				}
				else if (pending.action == ActionKind::Steal) {
					int taken = m_players[pending.target].takeCoins(2);
					m_players[pending.actor].coinplus(taken);
					appendLog(playerName(pending.actor) + " steals " + std::to_string(taken) + " coins from " + playerName(pending.target) + ".");
				}
			}
		}
		else {
			appendLog(actionName(pending.action) + " is blocked.");
		}
	}
	else {
		appendLog(playerName(pending.actor) + " does not challenge. " + actionName(pending.action) + " is blocked.");
	}
	finishComputerAction();
}

bool GameEngine::resolveChallenge(int claimant, int card, int challenger)
{
	appendLog(playerName(challenger) + " challenges " + playerName(claimant) + "'s " + cardName(card) + ".");
	if (m_players[claimant].cardhave(card)) {
		appendLog("Challenge fails. " + playerName(challenger) + " reveals a card.");
		openRandomCard(challenger);
		exchangeClaimedCard(claimant, card);
		return true;
	}

	appendLog("Challenge succeeds. " + playerName(claimant) + " reveals a card.");
	openRandomCard(claimant);
	return false;
}

bool GameEngine::openRandomCard(int player)
{
	bool defeated = m_players[player].openrand();
	if (defeated) {
		appendLog(playerName(player) + " is eliminated.");
	}
	return defeated;
}

void GameEngine::exchangeClaimedCard(int player, int card)
{
	if (m_players[player].exchgeCard(card, m_deck.shuffle(card))) {
		appendLog(playerName(player) + " returns " + cardName(card) + " to the deck and draws a replacement.");
	}
}

int GameEngine::findPlayerForeignAidBlocker(int actor)
{
	for (int candidate = 1; candidate <= 3; candidate++) {
		if (candidate != actor && !m_players[candidate].dead() && m_players[candidate].botherwonjo()) {
			return candidate;
		}
	}
	return actor;
}

int GameEngine::findRandomChallenger(int actor)
{
	for (int candidate = 1; candidate <= 3; candidate++) {
		if (candidate != actor && !m_players[candidate].dead() && !prob(5)) {
			return candidate;
		}
	}
	return actor;
}

int GameEngine::findComputerActionTarget(int actor) const
{
	int target;
	do {
		target = prob(4);
	} while (target == actor || m_players[target].dead());
	return target;
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

void GameEngine::checkWinner()
{
	if (!m_state.gameStarted || aliveCount() > 1) {
		m_state.gameOver = false;
		m_state.winner = -1;
		return;
	}

	for (int i = 0; i < 4; i++) {
		if (!m_players[i].dead()) {
			bool alreadyOver = m_state.gameOver && m_state.winner == i;
			m_state.gameOver = true;
			m_state.winner = i;
			clearPending();
			if (!alreadyOver) {
				appendLog(playerName(i) + " wins the game.");
			}
			return;
		}
	}
}

void GameEngine::clearPending()
{
	m_pending = PendingDecision{};
}

void GameEngine::setPending(PendingType type, ActionKind action, int actor, int target, int blocker, int card, const std::string& prompt)
{
	m_pending.type = type;
	m_pending.action = action;
	m_pending.actor = actor;
	m_pending.target = target;
	m_pending.blocker = blocker;
	m_pending.card = card;
	m_pending.prompt = prompt;
}

std::string GameEngine::playerName(int index) const
{
	return index == 0 ? "Player" : "Computer " + std::to_string(index);
}

std::string GameEngine::cardName(int card) const
{
	switch (card) {
	case 0:
		return "Duke";
	case 1:
		return "Assassin";
	case 2:
		return "Captain";
	case 3:
		return "Contessa";
	default:
		return "Unknown";
	}
}

std::string GameEngine::actionName(ActionKind action) const
{
	switch (action) {
	case ActionKind::ForeignAid:
		return "Foreign Aid";
	case ActionKind::Tax:
		return "Duke Tax";
	case ActionKind::Assassinate:
		return "Assassination";
	case ActionKind::Steal:
		return "Steal";
	default:
		return "Action";
	}
}
