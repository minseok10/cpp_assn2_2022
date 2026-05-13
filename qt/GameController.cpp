#include "GameController.h"

#include <QVariantMap>

GameController::GameController(QObject* parent)
    : QObject(parent),
      m_screen("start"),
      m_statusText("새 레지스탕스 쿠 게임을 시작할 준비가 되었습니다.")
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
        return "시작 전";
    }
    if (state.gameOver) {
        return state.winner == 0 ? "승자: 플레이어" : QString("승자: 컴퓨터 %1").arg(state.winner);
    }
    if (state.currentPlayer == 0) {
        return "플레이어 차례";
    }
    return QString("컴퓨터 %1 차례").arg(state.currentPlayer);
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
        playerMap["name"] = player.human ? QString("플레이어") : QString("컴퓨터 %1").arg(player.index);
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
    refreshState("소득, 해외 원조, 쿠 중 하나를 선택하세요.");
    setScreen("game");
}

void GameController::backToStart()
{
    setStatusText("새 레지스탕스 쿠 게임을 시작할 준비가 되었습니다.");
    setScreen("start");
}

void GameController::performIncome()
{
    if (m_engine.income()) {
        refreshState("소득을 처리했습니다.");
    } else {
        refreshState("코인이 10개 이상이면 쿠를 선택해야 합니다.");
    }
}

void GameController::performForeignAid()
{
    if (m_engine.foreignAid()) {
        refreshState(m_engine.state().waitingForHuman ? "도전 여부를 선택하세요." : "해외 원조를 처리했습니다.");
    } else {
        refreshState("코인이 10개 이상이면 쿠를 선택해야 합니다.");
    }
}

void GameController::performTax()
{
    if (m_engine.tax()) {
        refreshState("공작 세금을 처리했습니다.");
    } else {
        refreshState("공작 세금을 사용할 수 없습니다.");
    }
}

void GameController::performAssassinate(int target)
{
    if (m_engine.assassinate(target)) {
        refreshState(m_engine.state().waitingForHuman ? "도전 여부를 선택하세요." : "암살을 처리했습니다.");
    } else {
        refreshState("암살하려면 코인 3개와 살아 있는 컴퓨터 대상이 필요합니다.");
    }
}

void GameController::performSteal(int target)
{
    if (m_engine.steal(target)) {
        refreshState(m_engine.state().waitingForHuman ? "도전 여부를 선택하세요." : "강탈을 처리했습니다.");
    } else {
        refreshState("강탈하려면 살아 있는 컴퓨터 대상이 필요합니다.");
    }
}

void GameController::performCoup(int target)
{
    if (m_engine.coup(target)) {
        refreshState("쿠를 처리했습니다.");
    }
    else {
        refreshState("쿠를 하려면 코인 7개와 살아 있는 컴퓨터 대상이 필요합니다.");
    }
}

void GameController::answerChallenge(bool challenge)
{
    if (m_engine.answerChallenge(challenge)) {
        refreshState("도전 결정을 처리했습니다.");
    }
}

void GameController::answerCounter(bool counter)
{
    if (m_engine.answerCounter(counter)) {
        refreshState("방해 결정을 처리했습니다.");
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
