/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

import QtQuick 1.0
import Monitor 1.0
import "MainView.js" as Plotter

Rectangle {
    id: root

    property bool dataAvailable: true;
    property int eventCount: 0;
    property real progress: 0;

    property bool mouseOverSelection : true

    // move the cursor in the editor
    signal updateCursorPosition
    property string fileName: ""
    property int lineNumber: -1
    function gotoSourceLocation(file,line) {
        root.fileName = file;
        root.lineNumber = line;
        root.updateCursorPosition();
    }

    function clearData() {
        Plotter.reset();
        view.clearData();
        root.dataAvailable = false;
        root.eventCount = 0;
        rangeMover.x = 2
        rangeMover.opacity = 0
        hideRangeDetails();
    }

    function clearAll() {
        clearData();
        selectedEventIndex = -1;
        canvas.requestPaint();
        view.visible = false;
        root.elapsedTime = 0;
        root.updateTimer();
    }

    property int selectedEventIndex : -1;
    property bool mouseTracking: false;

    onSelectedEventIndexChanged: {
        if ((!mouseTracking) && eventCount > 0
                && selectedEventIndex > -1 && selectedEventIndex < eventCount) {
            // re-center flickable if necessary
            var xs = Plotter.xScale(canvas);
            var startTime = qmlEventList.firstTimeMark();
            var eventStartTime = qmlEventList.getStartTime(selectedEventIndex);
            var eventDuration = qmlEventList.getDuration(selectedEventIndex);
            if (rangeMover.value + startTime > eventStartTime) {
                rangeMover.x = Math.max(0,
                    Math.floor((eventStartTime - startTime) / xs - canvas.canvasWindow.x - rangeMover.zoomWidth/2) );
            } else if (rangeMover.value + startTime + rangeMover.zoomWidth * xs < eventStartTime + eventDuration) {
                rangeMover.x = Math.floor((eventStartTime + eventDuration - startTime) / xs - canvas.canvasWindow.x - rangeMover.zoomWidth/2);
            }
        }

        if (selectedEventIndex == -1) {
            selectionHighlight.visible = false;
        }
    }

    function nextEvent() {
        if (eventCount > 0) {
            ++selectedEventIndex;
            if (selectedEventIndex >= eventCount)
                selectedEventIndex = 0;
        }
    }

    function prevEvent() {
        if (eventCount > 0) {
            --selectedEventIndex;
            if (selectedEventIndex < 0)
                selectedEventIndex = eventCount - 1;
        }
    }

    function zoomIn() {
        // 10%
        var newZoom = rangeMover.zoomWidth / 1.1;

        // 0.1 ms minimum zoom
        if (newZoom * Plotter.xScale(canvas) > 100000) {
            rangeMover.zoomWidth =  newZoom
            rangeMover.updateZoomControls();
        }
    }

    function zoomOut() {
        // 10%
        var newZoom = rangeMover.zoomWidth * 1.1;
        if (newZoom > canvas.width)
            newZoom = canvas.width;
        if (newZoom + rangeMover.x > canvas.width)
            rangeMover.x = canvas.width - newZoom;
        rangeMover.zoomWidth = newZoom;
        rangeMover.updateZoomControls();
    }

    Connections {
        target: qmlEventList
        onCountChanged: {
            eventCount = qmlEventList.count();
            if (eventCount == 0)
                root.clearAll();
            if (eventCount > 1) {
                root.progress = Math.min(1.0,
                    (qmlEventList.lastTimeMark() - qmlEventList.firstTimeMark()) / root.elapsedTime * 1e-9 ) * 0.5;
            } else
            root.progress = 0;
        }

        onParsingStatusChanged: {
            root.dataAvailable = false;
        }

        onDataReady: {
            if (eventCount > 0) {
                view.clearData();
                view.rebuildCache();
            }
        }
    }

    // Elapsed
    property real elapsedTime;
    signal updateTimer;
    Timer {
        id: elapsedTimer
        property date startDate
        property bool reset:  true
        running: connection.recording
        repeat: true
        onRunningChanged: if (running) reset = true
        interval:  100
        triggeredOnStart: true
        onTriggered: {
            if (reset) {
                startDate = new Date()
                reset = false
            }
            var time = (new Date() - startDate)/1000
            root.elapsedTime = time.toFixed(1);
            root.updateTimer();
        }
    }

    // time markers
    TimeDisplay {
        height: flick.height + labels.y
        anchors.left: flick.left
        anchors.right: flick.right
        startTime: rangeMover.x * Plotter.xScale(canvas) + qmlEventList.firstTimeMark();
        endTime: (rangeMover.x + rangeMover.zoomWidth) * Plotter.xScale(canvas) + qmlEventList.firstTimeMark();
    }

    function hideRangeDetails() {
        rangeDetails.visible = false
        rangeDetails.duration = ""
        rangeDetails.label = ""
        rangeDetails.type = ""
        rangeDetails.file = ""
        rangeDetails.line = -1

        root.mouseOverSelection = true;
        selectionHighlight.visible = false;
    }

    //our main interaction view
    Flickable {
        id: flick
        anchors.top: parent.top
        anchors.topMargin: labels.y
        anchors.right: parent.right
        anchors.left: labels.right
        anchors.bottom: canvas.top
        contentWidth: view.totalWidth
        contentHeight: height
        flickableDirection: Flickable.HorizontalFlick

        clip:true

        TimelineView {
            id: view

            eventList: qmlEventList;
            onEventListChanged: Plotter.qmlEventList = qmlEventList;

            width: flick.width;
            height: flick.contentHeight;

            startX: flick.contentX
            onStartXChanged: {
                var newX = startTime / Plotter.xScale(canvas) - canvas.canvasWindow.x;
                if (Math.abs(rangeMover.x - newX) > .01)
                    rangeMover.x = newX
                if (Math.abs(startX - flick.contentX) > .01)
                    flick.contentX = startX
            }
            startTime: rangeMover.value
            endTime: startTime + (rangeMover.zoomWidth*Plotter.xScale(canvas))
            onEndTimeChanged: {
                updateTimeline();
            }

            property real timeSpan: endTime - startTime
            onTimeSpanChanged: {
                if (selectedEventIndex != -1 && selectionHighlight.visible) {
                    var spacing = flick.width / timeSpan;
                    selectionHighlight.x = (qmlEventList.getStartTime(selectedEventIndex) - qmlEventList.firstTimeMark()) * spacing;
                    selectionHighlight.width = qmlEventList.getDuration(selectedEventIndex) * spacing;
               }
            }

            onCachedProgressChanged: root.progress = 0.5 + cachedProgress * 0.5;
            onCacheReady: {
                root.progress = 1.0;
                root.dataAvailable = true;
                if (root.eventCount > 0) {
                    view.visible = true;
                    view.updateTimeline();
                    canvas.requestPaint();
                    rangeMover.x = 1    //### hack to get view to display things immediately
                    rangeMover.x = 0
                    rangeMover.opacity = 1
                }
            }

            delegate: Rectangle {
                id: obj
                property color myColor: Plotter.colors[type]

                function conditionalHide() {
                    if (!mouseArea.containsMouse)
                        mouseArea.exited()
                }

                property int baseY: type * view.height / labels.rowCount;
                property int baseHeight: view.height / labels.rowCount
                y: baseY + (nestingLevel-1)*(baseHeight / nestingDepth);
                height: baseHeight / nestingDepth;
                gradient: Gradient {
                    GradientStop { position: 0.0; color: myColor }
                    GradientStop { position: 0.5; color: Qt.darker(myColor, 1.1) }
                    GradientStop { position: 1.0; color: myColor }
                }
                smooth: true

                property bool componentIsCompleted: false
                Component.onCompleted: {
                    componentIsCompleted = true;
                    updateDetails();
                }

                property bool isSelected: root.selectedEventIndex == index;
                onIsSelectedChanged: updateDetails();
                function updateDetails() {
                    if (!root.mouseTracking && componentIsCompleted) {
                        if (isSelected) {
                            enableSelected(0, 0);
                        }
                    }
                }

                function enableSelected(x,y) {
                    rangeDetails.duration = qmlEventList.getDuration(index)/1000.0;
                    rangeDetails.label = qmlEventList.getDetails(index);
                    rangeDetails.file = qmlEventList.getFilename(index);
                    rangeDetails.line = qmlEventList.getLine(index);
                    rangeDetails.type = Plotter.names[type]

                    rangeDetails.visible = true

                    selectionHighlight.x = obj.x;
                    selectionHighlight.y = obj.y;
                    selectionHighlight.width = width;
                    selectionHighlight.height = height;
                    selectionHighlight.visible = true;
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        if (root.mouseOverSelection) {
                            root.mouseTracking = true;
                            root.selectedEventIndex = index;
                            enableSelected(mouseX, y);
                            root.mouseTracking = false;
                        }
                    }

                    onPressed: {
                        root.mouseTracking = true;
                        root.selectedEventIndex = index;
                        enableSelected(mouseX, y);
                        root.mouseTracking = false;

                        root.mouseOverSelection = false;
                        root.gotoSourceLocation(rangeDetails.file, rangeDetails.line);
                    }
                }
            }

            Rectangle {
                id: selectionHighlight
                color:"transparent"
                border.width: 2
                border.color: "blue"
                radius: 2
                visible: false
                z:1
            }

            MouseArea {
                width: parent.width
                height: parent.height
                x: flick.contentX
                onClicked: root.hideRangeDetails();
            }
        }
    }
    //popup showing the details for the hovered range
    RangeDetails {
        id: rangeDetails
    }

    Rectangle {
        id: labels
        width: 150
        color: "#dcdcdc"
        y: 24
        height: flick.height

        property int rowCount: 5

        Column {
            id: col
            //### change to use Repeater + Plotter.names?
            Label { text: qsTr("Painting"); height: labels.height/labels.rowCount}
            Label { text: qsTr("Compiling"); height: labels.height/labels.rowCount }
            Label { text: qsTr("Creating"); height: labels.height/labels.rowCount }
            Label { text: qsTr("Binding"); height: labels.height/labels.rowCount }
            Label { text: qsTr("Signal Handler"); height: labels.height/labels.rowCount }
        }

        //right border divider
        Rectangle {
            width: 1
            height: parent.height
            anchors.right: parent.right
            color: "#cccccc"
        }
    }

    //bottom border divider
    Rectangle {
        height: 1
        width: parent.width
        anchors.bottom: canvas.top
        color: "#cccccc"
    }

    //"overview" graph at the bottom
    TiledCanvas {
        id: canvas

        anchors.bottom: parent.bottom
        width: parent.width; height: 50

        property int canvasWidth: width

        canvasSize {
            width: canvasWidth
            height: canvas.height
        }

        tileSize.width: width
        tileSize.height: height

        canvasWindow.width: width
        canvasWindow.height: height

        onDrawRegion: {
             if (root.dataAvailable)
                 Plotter.plot(canvas, ctxt, region);
             else
                 Plotter.drawGraph(canvas, ctxt, region)    //just draw the background
        }
    }

    //moves the range mover to the position of a click
    MouseArea {
        anchors.fill: canvas
        //### ideally we could press to position then immediately drag
        onPressed: rangeMover.x = mouse.x - rangeMover.zoomWidth/2
    }

    RangeMover {
        id: rangeMover
        opacity: 0
        anchors.top: canvas.top
    }

    // the next elements are here because I want them rendered on top of the other
    Rectangle {
        id: timeDisplayLabel
        color: "lightsteelblue"
        border.color: Qt.darker(color)
        border.width: 1
        radius: 2
        height: Math.max(labels.y-2, timeDisplayText.height);
        y: timeDisplayEnd.visible ? flick.height - 1 : 1
        width: timeDisplayText.width + 10 + ( timeDisplayEnd.visible ? timeDisplayCloseControl.width + 10 : 0 )
        visible: false

        function hideAll() {
            timeDisplayBegin.visible = false;
            timeDisplayEnd.visible = false;
            timeDisplayLabel.visible = false;
        }
        Text {
            id: timeDisplayText
            x: 5
            y: parent.height/2 - height/2 + 1
            font.pointSize: 8
        }

        Text {
            id: timeDisplayCloseControl
            text:"X"
            anchors.right: parent.right
            anchors.rightMargin: 3
            y: parent.height/2 - height/2 + 1
            visible: timeDisplayEnd.visible
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    timeDisplayLabel.hideAll();
                }
            }
        }
    }

    Rectangle {
        id: timeDisplayBegin
        width: 1
        color: Qt.rgba(0,0,64,0.7);
        height: flick.height + labels.y
        visible: false
    }

    Rectangle {
        id: timeDisplayEnd
        width: 1
        color: Qt.rgba(0,0,64,0.7);
        height: flick.height + labels.y
        visible: false
    }

    StatusDisplay {
        anchors.centerIn: flick
    }
}
