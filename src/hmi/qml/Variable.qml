import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import com.github.denisacostaq.daqs 1.0

Page {
    width: 600
    height: 400
    
    header: Label {
        id: hdr
        text: qsTr("Page 1")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }
    
    Column {
        anchors.fill: parent
        spacing: 10
        //        color: "steelblue"
        Button {
            id: btn
//            z: 3
            icon.name: "list-add"
            onClicked: {
                variableDialog.visible = true
            }
        }
        ScrollView {
            width: parent.width
            height: parent.height - btn.height - btn.padding
            ListView {
                id: listView
                header: Label {
                    text: "cabecera"
//                    z: 20
                }
                clip: true
                model: varsModel.vars
                delegate: Rectangle {
                    y: hdr.y + hdr.height + hdr.bottomPadding
                    height: 50
                    width: listView.width
//                    z: 1
                    RowLayout {
                        id: entry
                        anchors.fill: parent
                        Column {
                            Label {
                                id: modelName
                                Layout.alignment: Qt.AlignLeft
                                text: modelData.name
                            }
                            Rectangle {
                                z: -1
    //                                anchors.fill: parent
                                width: entry.width/2
                                height: modelName.height/10
                                LinearGradient  {
                                    anchors.fill: parent
                                        gradient: Gradient {
                                            orientation: Gradient.Horizontal
                                            GradientStop { position: 0; color: modelData.color }
                                            GradientStop { position: 1; color: "transparent" }
                                        }
                                    }
                            }
                        }
                        Row {
                            Layout.alignment: Qt.AlignRight
                            spacing: 5
                            Button {
                                icon.name: "edit-entry"
                            }
                            Button {
                                icon.name: "delete"
                            }
                            Button {
                                icon.name: "dialog-ok"
                                visible: false
                            }
                        }
                    }
                }
            }
        }
    }
    
    Dialog {
        id: variableDialog
        visible: false
        title: qsTr("Add variable")
        width: 360
        height: 240
        modality: Qt.WindowModal
        VariableDialog {
            id: varDialog
            anchors.fill: parent
        }
        VarModel {
            id: model
        }
        onAccepted: {
            model.name = varDialog.name;
            model.color = varDialog.color;
            varsModel.vars.push(model);
        }
        onVisibilityChanged: {
            if (variableDialog.visible) {
                varDialog.name = qsTr('var name')
            }
        }
    }
    Component.onCompleted: {
        console.log(varsModel.vars, varsModel.vars.length);
        
    }
    
}
