import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3

Item {
    property color selectedColor: "gray"
    id: colorSelector

    RoundButton {
        id: roundButton
        anchors.fill: parent
        Material.accent: selectedColor
        highlighted: true
        onClicked: {
            colorDialog.visible = true;
        }
    }
    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        onAccepted: {
            colorSelector.selectedColor = colorDialog.color
        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
