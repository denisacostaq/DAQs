import QtQuick 2.0
import QtQuick.Controls 2.5
import QtCharts 2.3

Page {
    width: 600
    height: 400
    
    header: Label {
        text: qsTr("Historic")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }
    
    Connections {
        target: dataLayer
        onValsChanged: {
            var vals = dataLayer.m_vals;
            lineSeries.clear();
            var max = 0;
            var min = 0;
            for (var i = 0; i < vals.length; ++i) {
                var emulated = dataLayer.getEmulatedValue(vals[i]);
                if (max < emulated) {
                    max = emulated;
                }
                if (min > emulated) {
                    min = emulated;
                }
                lineSeries.append(vals[i], emulated);
            }
            line.axisY().max = max;
            line.axisY().min = min;
        }
    }
    
    ChartView {
        id: line
        anchors.fill: parent
        animationOptions: ChartView.AllAnimations
        antialiasing: true
        
        ValueAxis {
            id: yAxis
            min: -10
            max: 10
            tickCount: 20
        }
        
        LineSeries {
            id: lineSeries
            name: "LineSeries"
            axisY: yAxis
            XYPoint {
                x: 0
                y: 2
            }
            
            XYPoint {
                x: 1
                y: 1.2
            }
            
            XYPoint {
                x: 2
                y: 3.3
            }
            
            XYPoint {
                x: 5
                y: 2.1
            }
        }
        
        MouseArea {
            id: charMouseArea
            anchors.fill: parent
            onPositionChanged: {
                console.log('onPositionChanged', mouse.x, mouse.y)
            }
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: {
                console.log('onClicked', mouse.button, mouse.x, mouse.y)
                if(mouse.button & Qt.RightButton) {
                    line.zoomIn()
                } else {
                    line.zoomOut()
                }
            }
            onPressed: {
                console.log("onPressed", mouse.button, mouse.x, mouse.y)
            }
            onReleased: {
                console.log("onReleased", mouse.button, mouse.x, mouse.y)
            }
        }
    }
}


