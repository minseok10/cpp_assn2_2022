#include "GameController.h"

#include <QVariantMap>

GameController::GameController(QObject* parent)
    : QObject(parent),
      m_screen("start"),
      m_statusText("Ready to start a new Resistance: Coup session.")
{
}

QString GameController::screen() const
{
    return m_screen;
}

QString GameController::statusText() const
{
    return m_statusText;
}

QString GameController::currentTurnText() const
{
    const GameState& state = m_engine.state();
    if (!state.gameStarted) {
        return "Not started";
    }
    if (state.gameOver) {
        return state.winner == 0 ? "Winner: Player" : QString("Winner: Computer %1").arg(state.winner);
    }
    if (state.currentPlayer == 0) {
        return "Player turn";
    }
    return QString("Computer %1 turn").arg(state.currentPlayer);
}

QVariantList GameController::players() const
{
    QVariantList result;
    const GameState& state = m_engine.state();
    for (const PlayerState& player : state.players) {
        QVariantList cards;
        for (const CardState& card : player.cards) {
            QVariantMap cardMap;
            cardMap["name"] = QString::fromStdString(card.name);
            cardMap["open"] = card.open;
            cards.push_back(cardMap);
        }

        QVariantMap playerMap;
        playerMap["index"] = player.index;
        playerMap["name"] = player.human ? QString("Player") : QString("Computer %1").arg(player.index);
        playerMap["coins"] = player.coins;
        playerMap["dead"] = player.dead;
        playerMap["human"] = player.human;
        playerMap["cards"] = cards;
        result.push_back(playerMap);
    }
    return result;
}

QStringList GameController::logLines() const
{
    QStringList result;
    const GameState& state = m_engine.state();
    for (const std::string& line : state.log) {
        result.push_back(QString::fromStdString(line));
    }
    return result;
}

bool GameController::humanTurn() const
{
    const GameState& state = m_engine.state();
    return state.gameStarted && !state.gameOver && !state.waitingForHuman && state.currentPlayer == 0;
}

bool GameController::waitingForHuman() const
{
    return m_engine.state().waitingForHuman;
}

QString GameController::pendingType() const
{
    return QString::fromStdString(m_engine.state().pendingType);
}

QString GameController::pendingPrompt() const
{
    return QString::fromStdString(m_engine.state().pendingPrompt);
}

bool GameController::canCoup() const
{
    const GameState& state = m_engine.state();
    if (state.players.empty()) {
        return false;
    }

    return humanTurn() && state.players[0].coins >= 7;
}

bool GameController::canTax() const
{
    const GameState& state = m_engine.state();
    if (state.players.empty()) {
        return false;
    }

    return humanTurn() && state.players[0].coins <= 9;
}

bool GameController::canAssassinate() const
{
    const GameState& state = m_engine.state();
    if (state.players.empty()) {
        return false;
    }

    return humanTurn() && state.players[0].coins >= 3 && state.players[0].coins <= 9;
}

bool GameController::canSteal() const
{
    const GameState& state = m_engine.state();
    if (state.players.empty()) {
        return false;
    }

    return humanTurn() && state.players[0].coins <= 9;
}

bool GameController::gameOver() const
{
    return m_engine.state().gameOver;
}

void GameController::startGame()
{
    m_engine.start();
    refreshState("Choose Income, Foreign Aid, or Coup.");
    setScreen("game");
}

void GameController::backToStart()
{
    setStatusText("Ready to start a new Resistance: Coup session.");
    setScreen("start");
}

void GameController::performIncome()
{
    if (m_engine.income()) {
        refreshState("Income resolved.");
    } else {
        refreshState("Choose Coup when you have 10 or more coins.");
    }
}

void GameController::performForeignAid()
{
    if (m_engine.foreignAid()) {
        refreshState(m_engine.state().waitingForHuman ? "Waiting for your challenge decision." : "Foreign Aid resolved.");
    } else {
        refreshState("Choose Coup when you have 10 or more coins.");
    }
}

void GameController::performTax()
{
    if (m_engine.tax()) {
        refreshState("Duke Tax resolved.");
    } else {
        refreshState("Duke Tax is unavailable.");
    }
}

void GameController::performAssassinate(int target)
{
    if (m_engine.assassinate(target)) {
        refreshState(m_engine.state().waitingForHuman ? "Waiting for your challenge decision." : "Assassination resolved.");
    } else {
        refreshState("Assassination needs 3 coins and a living computer target.");
    }
}

void GameController::performSteal(int target)
{
    if (m_engine.steal(target)) {
        refreshState(m_engine.state().waitingForHuman ? "Waiting for your challenge decision." : "Steal resolved.");
    } else {
        refreshState("Steal needs a living computer target.");
    }
}

void GameController::performCoup(int target)
{
    if (m_engine.coup(target)) {
        refreshState("Coup resolved.");
    }
    else {
        refreshState("Coup needs 7 coins and a living computer target.");
    }
}

void GameController::answerChallenge(bool challenge)
{
    if (m_engine.answerChallenge(challenge)) {
        refreshState("Challenge decision resolved.");
    }
}

void GameController::answerCounter(bool counter)
{
    if (m_engine.answerCounter(counter)) {
        refreshState("Counteraction decision resolved.");
    }
}

void GameController::setScreen(const QString& screen)
{
    if (m_screen == screen) {
        return;
    }

    m_screen = screen;
    emit screenChanged();
}

void GameController::setStatusText(const QString& statusText)
{
    if (m_statusText == statusText) {
        return;
    }

    m_statusText = statusText;
    emit statusTextChanged();
}

void GameController::refreshState(const QString& statusText)
{
    setStatusText(statusText);
    emit stateChanged();
}
