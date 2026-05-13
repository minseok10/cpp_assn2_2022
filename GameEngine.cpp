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
	appendLog("새 게임을 시작했습니다.");
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
	appendLog("플레이어가 소득으로 코인 1개를 얻었습니다.");
	finishHumanAction();
	return true;
}

bool GameEngine::foreignAid()
{
	if (!canHumanAct() || mustCoup(0)) {
		return false;
	}

	appendLog("플레이어가 해외 원조를 선택했습니다.");
	int blocker = findPlayerForeignAidBlocker(0);
	if (blocker == 0) {
		m_players[0].coinplus(2);
		appendLog("아무도 해외 원조를 막지 않았습니다. 플레이어가 코인 2개를 얻었습니다.");
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
	appendLog("플레이어가 컴퓨터 " + std::to_string(target) + "에게 쿠를 시도했습니다.");
	if (defeated) {
		appendLog("컴퓨터 " + std::to_string(target) + "이 탈락했습니다.");
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
			appendLog("플레이어가 방해에 도전하지 않았습니다. " + actionName(pending.action) + "이 막혔습니다.");
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
	appendLog(playerName(actor) + " 차례입니다.");
	if (mustCoup(actor)) {
		int target = findComputerActionTarget(actor);
		bool defeated = m_players[actor].coup(target, m_players);
		appendLog(playerName(actor) + "은 코인이 10개 이상이라 " + playerName(target) + "에게 반드시 쿠를 해야 합니다.");
		if (defeated) {
			appendLog(playerName(target) + "이 탈락했습니다.");
		}
		return;
	}

	if (prob(2)) {
		int choice = m_players[actor].coins() > 6 ? prob(3) + 1 : prob(2) + 1;
		if (choice == 1) {
			m_players[actor].coinplus(1);
			appendLog(playerName(actor) + "이 소득으로 코인 1개를 얻었습니다.");
		}
		else if (choice == 2) {
			startComputerForeignAid();
		}
		else {
			int target = findComputerActionTarget(actor);
			bool defeated = m_players[actor].coup(target, m_players);
			appendLog(playerName(actor) + "이 " + playerName(target) + "에게 쿠를 시도했습니다.");
			if (defeated) {
				appendLog(playerName(target) + "이 탈락했습니다.");
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
	appendLog("플레이어가 " + actionName(action) + "을 위해 " + cardName(card) + "을 주장했습니다.");
	int challenger = findRandomChallenger(0);
	if (challenger != 0) {
		bool honest = resolveChallenge(0, card, challenger);
		if (!honest) {
			finishHumanAction();
			return;
		}
	}
	else {
		appendLog("아무도 플레이어에게 도전하지 않았습니다.");
	}

	continuePlayerAction(action, target);
}

void GameEngine::continuePlayerAction(ActionKind action, int target)
{
	if (action == ActionKind::Tax) {
		m_players[0].coinplus(3);
		appendLog("플레이어가 공작으로 코인 3개를 얻었습니다.");
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
			appendLog("플레이어가 컴퓨터 " + std::to_string(target) + "을 암살했습니다.");
			if (defeated) {
				appendLog("컴퓨터 " + std::to_string(target) + "이 탈락했습니다.");
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
			appendLog("플레이어가 컴퓨터 " + std::to_string(target) + "에게서 코인 " + std::to_string(taken) + "개를 강탈했습니다.");
			finishHumanAction();
		}
		return;
	}

	if (action == ActionKind::ForeignAid) {
		m_players[0].coinplus(2);
		appendLog("플레이어가 해외 원조로 코인 2개를 얻었습니다.");
		finishHumanAction();
	}
}

void GameEngine::blockPlayerAction(ActionKind action, int blocker, int card)
{
	appendLog(playerName(blocker) + "이 " + cardName(card) + "으로 플레이어의 " + actionName(action) + "을 막았습니다.");
	setPending(
		PendingType::ChallengeBlock,
		action,
		0,
		action == ActionKind::ForeignAid ? -1 : blocker,
		blocker,
		card,
		playerName(blocker) + "의 " + cardName(card) + " 방해에 도전하시겠습니까?");
	rebuildState();
}

void GameEngine::resolveBlockedPlayerAction(bool blockHonest)
{
	PendingDecision pending = m_pending;
	clearPending();
	if (blockHonest) {
		appendLog(playerName(pending.blocker) + "의 방해가 성공했습니다. " + actionName(pending.action) + "은 실패했습니다.");
		finishHumanAction();
		return;
	}

	appendLog(playerName(pending.blocker) + "의 방해가 실패했습니다.");
	continuePlayerAction(pending.action, pending.target);
}

void GameEngine::startComputerClaim(ActionKind action, int target, int card)
{
	int actor = m_currentPlayer;
	appendLog(playerName(actor) + "이 " + actionName(action) + "을 위해 " + cardName(card) + "을 주장했습니다.");
	if (!m_players[0].dead()) {
		setPending(
			PendingType::ChallengeClaim,
			action,
			actor,
			target,
			-1,
			card,
			playerName(actor) + "의 " + cardName(card) + " 주장에 도전하시겠습니까?");
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
		appendLog("아무도 " + playerName(pending.actor) + "에게 도전하지 않았습니다.");
	}

	continueComputerAction(pending.action, pending.target);
}

void GameEngine::continueComputerAction(ActionKind action, int target)
{
	int actor = m_currentPlayer;
	if (action == ActionKind::Tax) {
		m_players[actor].coinplus(3);
		appendLog(playerName(actor) + "이 공작으로 코인 3개를 얻었습니다.");
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
				playerName(actor) + "의 암살을 백작부인으로 막으시겠습니까?");
			rebuildState();
			return;
		}

		bool block = m_players[target].cardhave(3) || !prob(5);
		if (block) {
			appendLog(playerName(target) + "이 백작부인으로 암살을 막았습니다.");
			if (!prob(5)) {
				bool honest = resolveChallenge(target, 3, actor);
				if (!honest) {
					bool defeated = openRandomCard(target);
					m_players[actor].coinplus(-3);
					appendLog(playerName(actor) + "이 " + playerName(target) + "을 암살했습니다.");
					if (defeated) {
						appendLog(playerName(target) + "이 탈락했습니다.");
					}
				}
				else {
					appendLog("암살이 막혔습니다.");
				}
			}
			else {
				appendLog(playerName(actor) + "이 도전하지 않았습니다. 암살이 막혔습니다.");
			}
			finishComputerAction();
			return;
		}

		bool defeated = openRandomCard(target);
		m_players[actor].coinplus(-3);
		appendLog(playerName(actor) + "이 " + playerName(target) + "을 암살했습니다.");
		if (defeated) {
			appendLog(playerName(target) + "이 탈락했습니다.");
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
				playerName(actor) + "의 강탈을 사령관으로 막으시겠습니까?");
			rebuildState();
			return;
		}

		bool block = m_players[target].cardhave(2) || !prob(5);
		if (block) {
			appendLog(playerName(target) + "이 사령관으로 강탈을 막았습니다.");
			if (!prob(5)) {
				bool honest = resolveChallenge(target, 2, actor);
				if (!honest) {
					int taken = m_players[target].takeCoins(2);
					m_players[actor].coinplus(taken);
					appendLog(playerName(actor) + "이 " + playerName(target) + "에게서 코인 " + std::to_string(taken) + "개를 강탈했습니다.");
				}
				else {
					appendLog("강탈이 막혔습니다.");
				}
			}
			else {
				appendLog(playerName(actor) + "이 도전하지 않았습니다. 강탈이 막혔습니다.");
			}
			finishComputerAction();
			return;
		}

		int taken = m_players[target].takeCoins(2);
		m_players[actor].coinplus(taken);
		appendLog(playerName(actor) + "이 " + playerName(target) + "에게서 코인 " + std::to_string(taken) + "개를 강탈했습니다.");
		finishComputerAction();
	}
}

void GameEngine::startComputerForeignAid()
{
	int actor = m_currentPlayer;
	appendLog(playerName(actor) + "이 해외 원조를 선택했습니다.");
	if (!m_players[0].dead()) {
		setPending(
			PendingType::CounterBlock,
			ActionKind::ForeignAid,
			actor,
			-1,
			0,
			0,
			playerName(actor) + "의 해외 원조를 공작으로 막으시겠습니까?");
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
		appendLog("아무도 해외 원조를 막지 않았습니다. " + playerName(pending.actor) + "이 코인 2개를 얻었습니다.");
		finishComputerAction();
		return;
	}

	appendLog(playerName(blocker) + "이 공작으로 " + playerName(pending.actor) + "의 해외 원조를 막았습니다.");
	if (!prob(5)) {
		bool honest = resolveChallenge(blocker, 0, pending.actor);
		if (!honest) {
			m_players[pending.actor].coinplus(2);
			appendLog("방해 도전 이후 해외 원조가 성공했습니다.");
		}
		else {
			appendLog("해외 원조가 막혔습니다.");
		}
	}
	else {
		appendLog(playerName(pending.actor) + "이 도전하지 않았습니다. 해외 원조가 막혔습니다.");
	}
	finishComputerAction();
}

void GameEngine::resolveHumanCounter(bool counter)
{
	PendingDecision pending = m_pending;
	if (!counter) {
		appendLog("플레이어가 막지 않았습니다.");
		if (pending.action == ActionKind::ForeignAid) {
			continueComputerForeignAidAfterHumanPass();
		}
		else {
			clearPending();
			if (pending.action == ActionKind::Assassinate) {
				bool defeated = openRandomCard(pending.target);
				m_players[pending.actor].coinplus(-3);
				appendLog(playerName(pending.actor) + "이 " + playerName(pending.target) + "을 암살했습니다.");
				if (defeated) {
					appendLog(playerName(pending.target) + "이 탈락했습니다.");
				}
			}
			else if (pending.action == ActionKind::Steal) {
				int taken = m_players[pending.target].takeCoins(2);
				m_players[pending.actor].coinplus(taken);
				appendLog(playerName(pending.actor) + "이 " + playerName(pending.target) + "에게서 코인 " + std::to_string(taken) + "개를 강탈했습니다.");
			}
			finishComputerAction();
		}
		return;
	}

	clearPending();
	appendLog("플레이어가 " + cardName(pending.card) + "으로 " + playerName(pending.actor) + "의 " + actionName(pending.action) + "을 막았습니다.");
	if (!prob(5)) {
		bool honest = resolveChallenge(0, pending.card, pending.actor);
		if (!honest) {
			if (pending.action == ActionKind::ForeignAid) {
				m_players[pending.actor].coinplus(2);
				appendLog("플레이어의 방해가 실패해 해외 원조가 성공했습니다.");
			}
			else {
				if (pending.action == ActionKind::Assassinate) {
					bool defeated = openRandomCard(pending.target);
					m_players[pending.actor].coinplus(-3);
					appendLog(playerName(pending.actor) + "이 " + playerName(pending.target) + "을 암살했습니다.");
					if (defeated) {
						appendLog(playerName(pending.target) + "이 탈락했습니다.");
					}
				}
				else if (pending.action == ActionKind::Steal) {
					int taken = m_players[pending.target].takeCoins(2);
					m_players[pending.actor].coinplus(taken);
					appendLog(playerName(pending.actor) + "이 " + playerName(pending.target) + "에게서 코인 " + std::to_string(taken) + "개를 강탈했습니다.");
				}
			}
		}
		else {
			appendLog(actionName(pending.action) + "이 막혔습니다.");
		}
	}
	else {
		appendLog(playerName(pending.actor) + "이 도전하지 않았습니다. " + actionName(pending.action) + "이 막혔습니다.");
	}
	finishComputerAction();
}

bool GameEngine::resolveChallenge(int claimant, int card, int challenger)
{
	appendLog(playerName(challenger) + "이 " + playerName(claimant) + "의 " + cardName(card) + " 주장에 도전했습니다.");
	if (m_players[claimant].cardhave(card)) {
		appendLog("도전이 실패했습니다. " + playerName(challenger) + "이 카드 1장을 공개합니다.");
		openRandomCard(challenger);
		exchangeClaimedCard(claimant, card);
		return true;
	}

	appendLog("도전이 성공했습니다. " + playerName(claimant) + "이 카드 1장을 공개합니다.");
	openRandomCard(claimant);
	return false;
}

bool GameEngine::openRandomCard(int player)
{
	bool defeated = m_players[player].openrand();
	if (defeated) {
		appendLog(playerName(player) + "이 탈락했습니다.");
	}
	return defeated;
}

void GameEngine::exchangeClaimedCard(int player, int card)
{
	if (m_players[player].exchgeCard(card, m_deck.shuffle(card))) {
		appendLog(playerName(player) + "이 " + cardName(card) + "을 덱에 돌려놓고 새 카드를 뽑았습니다.");
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
				appendLog(playerName(i) + "이 게임에서 승리했습니다.");
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
	return index == 0 ? "플레이어" : "컴퓨터 " + std::to_string(index);
}

std::string GameEngine::cardName(int card) const
{
	switch (card) {
	case 0:
		return "공작";
	case 1:
		return "암살자";
	case 2:
		return "사령관";
	case 3:
		return "백작부인";
	default:
		return "알 수 없음";
	}
}

std::string GameEngine::actionName(ActionKind action) const
{
	switch (action) {
	case ActionKind::ForeignAid:
		return "해외 원조";
	case ActionKind::Tax:
		return "공작 세금";
	case ActionKind::Assassinate:
		return "암살";
	case ActionKind::Steal:
		return "강탈";
	default:
		return "행동";
	}
}
