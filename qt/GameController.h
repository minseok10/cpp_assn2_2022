#pragma once

#include "GameEngine.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

class GameController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString screen READ screen NOTIFY screenChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString currentTurnText READ currentTurnText NOTIFY stateChanged)
    Q_PROPERTY(QVariantList players READ players NOTIFY stateChanged)
    Q_PROPERTY(QStringList logLines READ logLines NOTIFY stateChanged)
    Q_PROPERTY(bool humanTurn READ humanTurn NOTIFY stateChanged)
    Q_PROPERTY(bool waitingForHuman READ waitingForHuman NOTIFY stateChanged)
    Q_PROPERTY(QString pendingType READ pendingType NOTIFY stateChanged)
    Q_PROPERTY(QString pendingPrompt READ pendingPrompt NOTIFY stateChanged)
    Q_PROPERTY(bool canCoup READ canCoup NOTIFY stateChanged)
    Q_PROPERTY(bool canTax READ canTax NOTIFY stateChanged)
    Q_PROPERTY(bool canAssassinate READ canAssassinate NOTIFY stateChanged)
    Q_PROPERTY(bool canSteal READ canSteal NOTIFY stateChanged)
    Q_PROPERTY(bool gameOver READ gameOver NOTIFY stateChanged)

public:
    explicit GameController(QObject* parent = nullptr);

    QString screen() const;
    QString statusText() const;
    QString currentTurnText() const;
    QVariantList players() const;
    QStringList logLines() const;
    bool humanTurn() const;
    bool waitingForHuman() const;
    QString pendingType() const;
    QString pendingPrompt() const;
    bool canCoup() const;
    bool canTax() const;
    bool canAssassinate() const;
    bool canSteal() const;
    bool gameOver() const;

    Q_INVOKABLE void startGame();
    Q_INVOKABLE void backToStart();
    Q_INVOKABLE void performIncome();
    Q_INVOKABLE void performForeignAid();
    Q_INVOKABLE void performTax();
    Q_INVOKABLE void performAssassinate(int target);
    Q_INVOKABLE void performSteal(int target);
    Q_INVOKABLE void performCoup(int target);
    Q_INVOKABLE void answerChallenge(bool challenge);
    Q_INVOKABLE void answerCounter(bool counter);

signals:
    void screenChanged();
    void statusTextChanged();
    void stateChanged();

private:
    void setScreen(const QString& screen);
    void setStatusText(const QString& statusText);
    void refreshState(const QString& statusText);

    GameEngine m_engine;
    QString m_screen;
    QString m_statusText;
};
