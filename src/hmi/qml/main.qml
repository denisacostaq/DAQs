import QtQuick 2.11
import QtQuick.Controls 2.4

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    flags: Qt.FramelessWindowHint | Qt.Window
    visibility: Qt.WindowFullScreen
    title: qsTr("Tabs")
    
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: Qt.platform.os === "android";

        Variable{
        }

        Historic {
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        visible: !swipeView.interactive;
        TabButton {
            text: qsTr("Variable")
        }
        TabButton {
            text: qsTr("Historic")
        }
    }
}
