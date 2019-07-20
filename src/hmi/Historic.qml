import QtQuick 2.0
import QtQuick.Controls 2.5
import QtCharts 2.3

Page {
    width: 600
    height: 400

    header: Label {
        id: headerLabel
        text: qsTr("Historic")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
        visible: Qt.platform.os !== "android";
    }
    
    Connections {
        target: dataLayer
        onValsChanged: {
            var vals = dataLayer.m_vals;
            lineSeries.clear();
            var max = 0;
            var min = 0;
            xTime.min = dataLayer.getEmulatedDateTime(0);
            for (var i = 0; i < vals.length; ++i) {
                var emulated = dataLayer.getEmulatedValue(vals[i]);
                if (max < emulated) {
                    max = emulated;
                }
                if (min > emulated) {
                    min = emulated;
                }
                lineSeries.append(dataLayer.getEmulatedDateTime(i), emulated);
            }
            xTime.max = dataLayer.getEmulatedDateTime(vals.length - 1);
            yAxis.max = max;
            yAxis.min = min;
            headerLabel.text = qsTr("Historic data from ") + xTime.min + qsTr(" to ") + xTime.max
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

        DateTimeAxis {
            id: xTime
            format: "hh:mm:ss:MM"
            tickCount: 10
        }
        
        LineSeries {
            id: lineSeries
            name: "temperature"
            axisY: yAxis
            axisX: xTime
            useOpenGL: true
        }
        RubberBand {
            anchors.fill: parent
            onSlectedChanged: {
                line.zoomIn(slected);
            }
        }
    }
}


