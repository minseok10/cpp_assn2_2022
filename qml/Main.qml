import QtQuick
import QtQuick.Controls
import ResistanceCoup

ApplicationWindow {
    id: window
    width: 960
    height: 640
    minimumWidth: 720
    minimumHeight: 520
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
