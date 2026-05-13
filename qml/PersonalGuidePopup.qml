import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: guidePopup

    width: Math.min(parent ? parent.width - 36 : 520, 520)
    height: Math.min(parent ? parent.height - 96 : 620, guideLayout.implicitHeight + 28)
    x: parent ? parent.width - width - 18 : 0
    y: 64
    modal: false
    dim: false
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        radius: 8
        color: "#ffffff"
        border.width: 1
        border.color: "#9fb0c1"
    }

    contentItem: ColumnLayout {
        id: guideLayout
        spacing: 14

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                text: "게임 설명"
                Layout.fillWidth: true
                font.pixelSize: 22
                font.bold: true
                color: "#1f2933"
            }

            BorderedButton {
                text: "Close"
                implicitWidth: 78
                onClicked: guidePopup.close()
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: guidePopup.availableWidth
                spacing: 10

                Label {
                    text: "일반 행동"
                    Layout.fillWidth: true
                    font.pixelSize: 16
                    font.bold: true
                    color: "#25313d"
                }

                Label {
                    text: "소득: 코인 1개를 얻습니다.\n해외 원조: 코인 2개를 얻지만 공작에게 막힐 수 있습니다.\n쿠: 코인 7개를 내고 상대 카드 1장을 공개합니다. 코인이 10개 이상이면 반드시 쿠를 해야 합니다."
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    lineHeight: 1.15
                    font.pixelSize: 14
                    color: "#52606d"
                }

                Label {
                    text: "캐릭터"
                    Layout.fillWidth: true
                    font.pixelSize: 16
                    font.bold: true
                    color: "#25313d"
                }

                Repeater {
                    model: [
                        { name: "공작 Duke", detail: "세금으로 코인 3개를 얻습니다. 다른 플레이어의 해외 원조를 막을 수 있습니다." },
                        { name: "암살자 Assassin", detail: "코인 3개를 내고 상대 카드 1장을 공개하려고 시도합니다. 백작부인이 막을 수 있습니다." },
                        { name: "사령관 Captain", detail: "상대에게서 최대 코인 2개를 빼앗습니다. 사령관은 강탈을 막을 수도 있습니다." },
                        { name: "백작부인 Contessa", detail: "암살자의 공격을 막을 수 있습니다. 직접 수행하는 행동은 없습니다." }
                    ]

                    Rectangle {
                        required property var modelData

                        Layout.fillWidth: true
                        implicitHeight: roleText.implicitHeight + 22
                        radius: 8
                        color: "#f8fafc"
                        border.width: 1
                        border.color: "#d6dce3"

                        ColumnLayout {
                            id: roleText
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 4

                            Label {
                                text: modelData.name
                                Layout.fillWidth: true
                                font.pixelSize: 15
                                font.bold: true
                                color: "#25313d"
                            }

                            Label {
                                text: modelData.detail
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                font.pixelSize: 13
                                color: "#52606d"
                            }
                        }
                    }
                }

                Label {
                    text: "도전"
                    Layout.fillWidth: true
                    font.pixelSize: 16
                    font.bold: true
                    color: "#25313d"
                }

                Label {
                    text: "누군가 캐릭터 능력을 주장하면 도전할 수 있습니다. 거짓말이면 능력을 주장한 사람이 카드 1장을 공개하고, 진실이면 도전자 쪽이 카드 1장을 공개합니다."
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    lineHeight: 1.15
                    font.pixelSize: 14
                    color: "#52606d"
                }
            }
        }
    }
}
