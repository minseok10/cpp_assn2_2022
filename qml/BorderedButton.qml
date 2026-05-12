import QtQuick
import QtQuick.Controls

Button {
    id: control

    implicitHeight: 36
    padding: 10
    font.pixelSize: 14
    opacity: enabled ? 1.0 : 0.55

    contentItem: Text {
        text: control.text
        font: control.font
        color: control.enabled ? "#25313d" : "#7b8794"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        radius: 6
        color: control.down ? "#e4ebf3" : control.hovered ? "#f4f7fb" : "#ffffff"
        border.width: 1
        border.color: control.enabled ? "#9fb0c1" : "#d6dce3"
    }
}
