import QtQuick
import QtQuick.Controls
import ResistanceCoup

ApplicationWindow {
    id: window
    width: 960
    height: 760
    minimumWidth: 720
    minimumHeight: 680
    visible: true
    title: "Resistance: Coup"
    color: "#f6f7f9"

    Loader {
        id: screenLoader
        anchors.fill: parent
        sourceComponent: gameController.screen === "game" ? gameScreen : startScreen
    }

    Component {
        id: startScreen

        StartScreen {
            statusText: gameController.statusText
            onStartRequested: gameController.startGame()
        }
    }

    Component {
        id: gameScreen

        GameScreen {
            onBackRequested: gameController.backToStart()
        }
    }
}
