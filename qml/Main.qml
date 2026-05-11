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
    palette.window: "#f6f7f9"
    palette.windowText: "#1f2933"
    palette.base: "#ffffff"
    palette.alternateBase: "#eef2f5"
    palette.text: "#25313d"
    palette.button: "#ffffff"
    palette.buttonText: "#25313d"
    palette.highlight: "#2f80ed"
    palette.highlightedText: "#ffffff"
    palette.placeholderText: "#7b8794"
    palette.light: "#ffffff"
    palette.mid: "#c7d0d9"
    palette.dark: "#52606d"

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
