import QtQuick 2.0

Item {
    id: componentRoot
    anchors.fill: parent
    property rect slected: Qt.rect(0, 0, 0, 0)

    Rectangle {
        id: recZoom
        border.color: "steelblue"
        border.width: 1
        color: "steelblue"
        opacity: 0.3
        visible: false
    }
    PinchArea {
        pinch.maximumRotation: 0
        anchors.fill: parent
        onPinchStarted: function(pinch) {
            recZoom.x = pinch.point1.x;
            recZoom.y = pinch.point1.y;
            recZoom.visible = true;
        }
        onPinchUpdated: function(pinch) {
            if (pinch.point1 !== pinch.point2) {
                recZoom.x = pinch.point1.x < pinch.point2.x 
                        ? pinch.point1.x : pinch.point2.x;
                recZoom.y = pinch.point1.y < pinch.point2.y 
                        ? pinch.point1.y : pinch.point2.y;
                recZoom.width = Math.abs(pinch.point1.x - pinch.point2.x);
                recZoom.height = Math.abs(pinch.point1.y - pinch.point2.y);
            }
        }
        onPinchFinished: function(pinch) {
            componentRoot.slected = Qt.rect(
                        recZoom.x, recZoom.y, recZoom.width, recZoom.height);
            recZoom.visible = false;
        }
    }
}
