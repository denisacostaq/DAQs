import QtQuick 2.4
import QtQuick.Dialogs 1.2

ColorSelectorForm {
    id: colorSelector
    roundButton.onClicked: {
        colorDialog.visible = true;
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
