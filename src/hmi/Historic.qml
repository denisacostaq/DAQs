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
            if (vals.length === 0) {
                return;
            }
            var min = 0;
            var max = 0;
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
            var d = dataLayer.getEmulatedDateTime(vals.length - 1);
            if (xTime.min.getTime() === d.getTime()) {
                d.setSeconds(d.getSeconds() + 1);
            }
            xTime.max = d;
            yAxis.min = min;
            yAxis.max = min === max ? max + 1 : max;
            headerLabel.text = qsTr("Historic data from ") + xTime.min + qsTr(" to ") + xTime.max;
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
                var startPoint = Qt.point(slected.x, slected.y);
                var endPoint = Qt.point(
                            slected.x + slected.width, slected.height);
                // FIXME(denisacostaq@gmail.com): Improve resolution of
                // mapToValue. xTime.min.getTime(), xTime.max.getTime()
                var startVal = line.mapToValue(startPoint, lineSeries);
                var endVal = line.mapToValue(endPoint, lineSeries);
                var startDate = Math.floor(startVal.x);
                var endDate = Math.floor(endVal.x);
                dataLayer.getValues("temp", startDate, endDate);
            }
        }
    }
}


