import QtQuick 2.0

Item {
    id: componentRoot
    anchors.fill: parent
    property int xScaleZoom: 0
    property int yScaleZoom: 0
    property rect slected: Qt.rect(0, 0, 0, 0)

    Rectangle {
        id: recZoom
        border.color: "steelblue"
        border.width: 1
        color: "steelblue"
        opacity: 0.3
        visible: false
        transform: Scale {
            origin.x: 0
            origin.y: 0
            xScale: componentRoot.xScaleZoom
            yScale: componentRoot.yScaleZoom
        }
    }
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            recZoom.x = mouseX;
            recZoom.y = mouseY;
            recZoom.visible = true;
        }
        onMouseXChanged: {
            if (recZoom.visible) {
                componentRoot.xScaleZoom = (mouseX - recZoom.x >= 0) ? 1 : -1;
                recZoom.width = Math.abs(recZoom.x - mouseX);
            }
        }
        onMouseYChanged: {
            if (recZoom.visible) {
                componentRoot.yScaleZoom = (mouseY - recZoom.y >= 0) ? 1 : -1;
                recZoom.height = Math.abs(recZoom.y - mouseY);
            }
        }
        onReleased: {
            var x = (mouseX >= recZoom.x) ? recZoom.x : mouseX;
            var y = (mouseY >= recZoom.y) ? recZoom.y : mouseY;
            componentRoot.slected = Qt.rect(x, y, recZoom.width, recZoom.height);
            recZoom.visible = false;
        }
    }
}
