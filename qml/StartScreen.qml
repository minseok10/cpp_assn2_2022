import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property string statusText: ""
    signal startRequested()

    Rectangle {
        anchors.fill: parent
        color: "#f6f7f9"
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width - 64, 520)
        spacing: 24

        Label {
            text: "Resistance: Coup"
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 44
            font.bold: true
            color: "#1f2933"
        }

        Label {
            text: root.statusText
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font.pixelSize: 16
            color: "#52606d"
        }

        Button {
            text: "Start"
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: 180
            implicitHeight: 44
            onClicked: root.startRequested()
        }
    }
}
