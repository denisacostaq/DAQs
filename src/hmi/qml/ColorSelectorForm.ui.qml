import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

Item {
    width: 60
    height: 60
    property alias roundButton: roundButton
    property color selectedColor: "gray"

    RoundButton {
        id: roundButton
        anchors.fill: parent
        Material.accent: selectedColor
        highlighted: true
    }
}
