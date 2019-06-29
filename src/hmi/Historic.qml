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
    
    ChartView {
        id: line
        anchors.fill: parent
        
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
    }
}


