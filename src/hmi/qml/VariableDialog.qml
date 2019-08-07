import QtQuick 2.12
import QtQuick.Controls 2.5

VariableDialogForm {
    id: varDialogForm
    anchors.fill: parent
    property alias color: varDialogForm.selectedColor
    property alias name: varDialogForm.name
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
