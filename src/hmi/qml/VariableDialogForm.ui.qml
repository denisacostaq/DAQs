import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.3

Item {
    anchors.fill: parent
    property alias colorSelector: colorSelector
    width: 320
    height: 240
    
    id: dialog
    
    ColumnLayout {
        x: 9
        y: 60
        spacing: 20
        
        RowLayout {
            spacing: 20
            
            Label {
                id: label
                text: qsTr("Name")
            }
            
            TextInput {
                id: textInput
                text: qsTr("Variable name")
                horizontalAlignment: Text.AlignLeft
                Layout.preferredHeight: 20
                Layout.preferredWidth: 80
                font.pixelSize: 12
            }
        }
        
        RowLayout {
            spacing: 20
            
            Label {
                id: label1
                text: qsTr("Color")
            }
            
            ColorSelector {
                id: colorSelector
                Layout.preferredHeight: 35
                Layout.preferredWidth: 35
            }
        }
    }
    
}
