import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    signal backRequested()

    Rectangle {
        anchors.fill: parent
        color: "#eef2f5"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 18

        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            Label {
                text: gameController.currentTurnText
                Layout.fillWidth: true
                font.pixelSize: 30
                font.bold: true
                color: "#1f2933"
            }

            BorderedButton {
                text: "Back"
                implicitWidth: 96
                onClicked: root.backRequested()
            }
        }

        Label {
            text: gameController.statusText
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            font.pixelSize: 15
            color: "#52606d"
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 620
                spacing: 14

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 12
                    rowSpacing: 12

                    Repeater {
                        model: gameController.players

                        Rectangle {
                            required property var modelData

                            Layout.fillWidth: true
                            Layout.preferredHeight: 132
                            radius: 8
                            border.color: modelData.dead ? "#d6d9de" : modelData.human ? "#2f80ed" : "#c7d0d9"
                            color: modelData.dead ? "#f1f3f5" : "#ffffff"

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 14
                                spacing: 8

                                RowLayout {
                                    Layout.fillWidth: true

                                    Label {
                                        text: modelData.name
                                        Layout.fillWidth: true
                                        font.pixelSize: 18
                                        font.bold: true
                                        color: modelData.dead ? "#7b8794" : "#25313d"
                                    }

                                    Label {
                                        text: modelData.dead ? "Out" : modelData.coins + " coins"
                                        font.pixelSize: 14
                                        color: modelData.dead ? "#9aa5b1" : "#52606d"
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    Repeater {
                                        model: modelData.cards

                                        Rectangle {
                                            required property var modelData

                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 48
                                            radius: 6
                                            border.color: modelData.open ? "#d64545" : "#c7d0d9"
                                            color: modelData.open ? "#fff1f1" : "#f8fafc"

                                            Label {
                                                anchors.centerIn: parent
                                                text: modelData.name
                                                font.pixelSize: 14
                                                color: modelData.open ? "#9b1c1c" : "#25313d"
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 252
                    radius: 8
                    border.color: "#c7d0d9"
                    color: "#ffffff"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 10

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            BorderedButton {
                                text: "Income"
                                enabled: gameController.humanTurn && gameController.canTax
                                Layout.fillWidth: true
                                onClicked: gameController.performIncome()
                            }

                            BorderedButton {
                                text: "Foreign Aid"
                                enabled: gameController.humanTurn && gameController.canTax
                                Layout.fillWidth: true
                                onClicked: gameController.performForeignAid()
                            }

                            BorderedButton {
                                text: "Duke Tax"
                                enabled: gameController.canTax
                                Layout.fillWidth: true
                                onClicked: gameController.performTax()
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text: "Coup"
                                font.pixelSize: 14
                                color: "#52606d"
                            }

                            Repeater {
                                model: gameController.players

                                BorderedButton {
                                    required property var modelData

                                    visible: !modelData.human
                                    text: modelData.name
                                    enabled: gameController.canCoup && !modelData.dead
                                    Layout.fillWidth: true
                                    onClicked: gameController.performCoup(modelData.index)
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text: "Assassinate"
                                font.pixelSize: 14
                                color: "#52606d"
                            }

                            Repeater {
                                model: gameController.players

                                BorderedButton {
                                    required property var modelData

                                    visible: !modelData.human
                                    text: modelData.name
                                    enabled: gameController.canAssassinate && !modelData.dead
                                    Layout.fillWidth: true
                                    onClicked: gameController.performAssassinate(modelData.index)
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text: "Steal"
                                font.pixelSize: 14
                                color: "#52606d"
                            }

                            Repeater {
                                model: gameController.players

                                BorderedButton {
                                    required property var modelData

                                    visible: !modelData.human
                                    text: modelData.name
                                    enabled: gameController.canSteal && !modelData.dead
                                    Layout.fillWidth: true
                                    onClicked: gameController.performSteal(modelData.index)
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            BorderedButton {
                                text: "Exchange"
                                enabled: false
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 280
                radius: 8
                border.color: "#c7d0d9"
                color: "#ffffff"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    Label {
                        text: "Log"
                        Layout.fillWidth: true
                        font.pixelSize: 18
                        font.bold: true
                        color: "#25313d"
                    }

                    ListView {
                        id: logView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        spacing: 8
                        model: gameController.logLines
                        onCountChanged: positionViewAtEnd()

                        delegate: Label {
                            required property string modelData

                            width: logView.width
                            text: modelData
                            wrapMode: Text.WordWrap
                            font.pixelSize: 13
                            color: "#52606d"
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: pendingDecisionPanel
        visible: gameController.waitingForHuman
        z: 10
        width: Math.min(root.width - 56, 520)
        height: Math.max(92, decisionLayout.implicitHeight + 24)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28
        radius: 8
        border.width: 1
        border.color: "#d69e2e"
        color: "#fffbea"

        RowLayout {
            id: decisionLayout
            anchors.fill: parent
            anchors.margins: 12
            spacing: 10

            Label {
                text: gameController.pendingPrompt
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                font.pixelSize: 13
                color: "#513c06"
            }

            BorderedButton {
                text: "Yes"
                implicitWidth: 70
                onClicked: {
                    if (gameController.pendingType === "challenge")
                        gameController.answerChallenge(true)
                    else
                        gameController.answerCounter(true)
                }
            }

            BorderedButton {
                text: "No"
                implicitWidth: 70
                onClicked: {
                    if (gameController.pendingType === "challenge")
                        gameController.answerChallenge(false)
                    else
                        gameController.answerCounter(false)
                }
            }
        }
    }
}
