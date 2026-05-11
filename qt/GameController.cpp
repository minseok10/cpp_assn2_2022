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
    return state.gameStarted && !state.gameOver && state.currentPlayer == 0;
}

bool GameController::canCoup() const
{
    const QVariantList playerList = players();
    if (playerList.isEmpty()) {
        return false;
    }

    const QVariantMap player = playerList.first().toMap();
    return humanTurn() && player.value("coins").toInt() >= 7;
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
    }
}

void GameController::performForeignAid()
{
    if (m_engine.foreignAid()) {
        refreshState("Foreign Aid resolved. Blocking is TODO.");
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
