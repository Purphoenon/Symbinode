/*
 * Copyright © 2020 Gukova Anastasiia
 * Copyright © 2020 Gukov Anton <fexcron@gmail.com>
 *
 *
 * This file is part of Symbinode.
 *
 * Symbinode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symbinode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Symbinode.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Shapes 1.12
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0
import "qml"
import backgroundobject 1.0
import preview 1.0
import preview3d 1.0
import node 1.0
import scene 1.0
import mainwindow 1.0

MainWindow {
    id: mainWindow
    visible: true
    minimumWidth: 400
    minimumHeight: 200
    title: qsTr("Symbinode")

    MenuBar {
        id: menuBar
        padding: 0
        leftPadding: 5
        spacing: 0
        z: 3

        Menu { title: qsTr("File"); background: Rectangle {
                implicitWidth: 100
                implicitHeight: 30
                color: "#2C2D2F"
            }
            delegate: MenuItem {
                id: menuFile
                width: 100
                height: 30
                leftPadding: 15
                contentItem: Text{
                  text: menuFile.text
                  color: "#A2A2A2"
                }
                background: Rectangle {
                      implicitWidth: 100
                      implicitHeight: 30
                      color: menuFile.highlighted ? "#404347" : "transparent"
                  }
            }
            Action {
                text: "New"
                shortcut: "Ctrl+N"
                onTriggered: {
                    mainWindow.newDocument()
                }
            }
            Action {
                text: "Open"
                shortcut: "Ctrl+O"
                onTriggered: {
                    mainWindow.loadScene()
                }
            }
            Action {
                text: "Save"
                shortcut: "Ctrl+S"
                onTriggered: {
                    mainWindow.saveScene()
                }
            }
            Action {
                text: "Save as ..."
                onTriggered: {
                    mainWindow.saveSceneAs()
                }
            }
            Action {
                text: "Export"
                onTriggered: {
                    mainWindow.exportTextures()
                }
            }

        }
        Menu { title: qsTr("Edit")
            background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 30
                            color: "#2C2D2F"
                        }
            delegate: MenuItem {
                id: menuEdit
                width: 100
                height: 30
                leftPadding: 15
                arrow: Canvas {
                            x: parent.width - width
                            implicitWidth: 30
                            implicitHeight: 30
                            visible: menuEdit.subMenu
                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.fillStyle = "#a2a2a2"
                                ctx.moveTo(20, 15)
                                ctx.lineTo(14, 18)
                                ctx.lineTo(14, 12)
                                ctx.closePath()
                                ctx.fill()
                            }
                        }
                contentItem: Text{
                  text: menuEdit.text
                  color: "#A2A2A2"
                }
                background: Rectangle {
                      implicitWidth: 100
                      implicitHeight: 30
                      color: menuEdit.highlighted ? "#404347" : "transparent"
                  }
            }
            Menu {
                title: qsTr("Resolution")
                background: Rectangle {
                                implicitWidth: 100
                                implicitHeight: 30
                                color: "#2C2D2F"
                            }
                delegate: MenuItem {
                    id: menuResolution
                    width: 100
                    height: 30
                    indicator: Item {
                                implicitWidth: 30
                                implicitHeight: 30
                                Rectangle {
                                    width: 12
                                    height: 12
                                    anchors.centerIn: parent
                                    visible: menuResolution.checkable
                                    color: "transparent"
                                    border.color: "#A2A2A2"
                                    Rectangle {
                                        width: 4
                                        height: 4
                                        anchors.centerIn: parent
                                        visible: menuResolution.checked
                                        color: "#A2A2A2"
                                    }
                                }
                            }
                    contentItem: Text {
                                leftPadding: menuResolution.indicator.width
                                rightPadding: menuResolution.arrow.width
                                text: menuResolution.text
                                color: "#A2A2A2"
                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }
                    background: Rectangle {
                          implicitWidth: 100
                          implicitHeight: 30
                          color: menuResolution.highlighted ? "#404347" : "transparent"
                      }
                }
                ActionGroup {
                   id: resGroup
                   exclusive: true
                }

                Action {
                    id: res512
                    text: "512x512"
                    checkable: true
                    ActionGroup.group: resGroup

                    onTriggered: {
                        if(res512 == resGroup.checkedAction) res512.checked = true
                        mainWindow.changeResolution(Qt.vector2d(512, 512))
                    }
                }
                Action {
                    id: res1024
                    text: "1024x1024"
                    checkable: true
                    checked: true
                    ActionGroup.group: resGroup
                    onTriggered: {
                        if(res1024 == resGroup.checkedAction) res1024.checked = true
                        mainWindow.changeResolution(Qt.vector2d(1024, 1024))
                    }
                }
                Action {
                    id: res2048
                    text: "2048x2048"
                    checkable: true
                    ActionGroup.group: resGroup
                    onTriggered: {
                        if(res2048 == resGroup.checkedAction) res2048.checked = true
                        mainWindow.changeResolution(Qt.vector2d(2048, 2048))
                    }
                }
                Action {
                    id: res4096
                    text: "4096x4096"
                    checkable: true
                    ActionGroup.group: resGroup
                    onTriggered: {
                        if(res4096 == resGroup.checkedAction) res4096.checked = true
                        mainWindow.changeResolution(Qt.vector2d(4096, 4096))
                    }
                }
            }
        }

        delegate: MenuBarItem {
            id: menuBarItem
            padding: 0
            spacing: 0
            width: 40
            height: 30

            contentItem: Text {
                text: menuBarItem.text
                font.pointSize: 8
                color: "#A2A2A2"
                anchors.fill: parent
                padding: 0
                renderType: Text.NativeRendering
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                implicitWidth: 40
                implicitHeight: 30
                color: menuBarItem.highlighted ? "#2C2D2F" : "transparent"
            }
        }

        background: Rectangle {
            implicitWidth: mainWindow.width
            implicitHeight: 30
            color: "#3A3C3E"

            Rectangle {
                color: "#2C2D2F"
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
            }
        }
    }

    Menu {
        id: addNode
        y: 25
        background: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 30
                        color: "#2C2D2F"
                    }
        delegate: MenuItem {
            id: menuAdd
            width: 100
            height: 30
            leftPadding: 15
            arrow: Canvas {
                        x: parent.width - width
                        implicitWidth: 30
                        implicitHeight: 30
                        visible: menuAdd.subMenu
                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.fillStyle = "#a2a2a2"
                            ctx.moveTo(20, 15)
                            ctx.lineTo(14, 18)
                            ctx.lineTo(14, 12)
                            ctx.closePath()
                            ctx.fill()
                        }
                    }
            contentItem: Text{
              text: menuAdd.text
              color: "#A2A2A2"
            }
            background: Rectangle {
                  implicitWidth: 100
                  implicitHeight: 30
                  color: menuAdd.highlighted ? "#404347" : "transparent"
              }
        }
        Menu {
            title: qsTr("Outputs")
            Action {
                text: "Albedo"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 8)
                }
            }
            Action {
                text: "Metalness"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 9)
                }
            }
            Action {
                text: "Roughness"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 10)
                }
            }
            Action {
                text: "Normal"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 12)
                }
            }
            background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 30
                            color: "#2C2D2F"
                        }
            delegate: MenuItem {
                id: menuOutputs
                width: 100
                height: 30
                leftPadding: 15
                contentItem: Text {
                            leftPadding: 10
                            rightPadding: 10
                            text: menuOutputs.text
                            color: "#A2A2A2"
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                background: Rectangle {
                      implicitWidth: 100
                      implicitHeight: 30
                      color: menuOutputs.highlighted ? "#404347" : "transparent"
                  }
            }
        }
        Menu {
            title: qsTr("Textures")
            Action {
                text: "Noise"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 6)
                }
            }
            Action {
                text: "Voronoi"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 13)
                }
            }
            Action {
                text: "Polygon"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 14)
                }
            }
            Action {
                text: "Circle"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 15)
                }
            }
            Action {
                text: "Color"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 1)
                }
            }
            background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 30
                            color: "#2C2D2F"
                        }
            delegate: MenuItem {
                id: menuTextures
                width: 100
                height: 30
                leftPadding: 15
                contentItem: Text {
                            leftPadding: 10
                            rightPadding: 10
                            text: menuTextures.text
                            color: "#A2A2A2"
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                background: Rectangle {
                      implicitWidth: 100
                      implicitHeight: 30
                      color: menuTextures.highlighted ? "#404347" : "transparent"
                  }
            }
        }
        Menu {
            title: qsTr("Color")
            Action {
                text: "Mix"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 7)
                }
            }
            Action {
                text: "Inverse"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 20)
                }
            }
            Action {
                text: "Color Ramp"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 0)
                }
            }
            Action {
                text: "Coloring"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 2)
                }
            }
            Action {
                text: "Map Range"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 3)
                }
            }
            Action {
                text: "Brightness-Contrast"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 21)
                }
            }
            Action {
                text: "Threshold"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 22)
                }
            }
            background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 30
                            color: "#2C2D2F"
                        }
            delegate: MenuItem {
                id: menuColors
                width: 100
                height: 30
                leftPadding: 15
                contentItem: Text {
                            leftPadding: 10
                            rightPadding: 10
                            text: menuColors.text
                            color: "#A2A2A2"
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                background: Rectangle {
                      implicitWidth: 100
                      implicitHeight: 30
                      color: menuColors.highlighted ? "#404347" : "transparent"
                  }
            }
        }
        Menu {
            title: qsTr("Filters")
            Action {
                text: "Normal Map"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 11)
                }
            }
            Action {
                text: "Blur"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 19)
                }
            }
            background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 30
                            color: "#2C2D2F"
                        }
            delegate: MenuItem {
                id: menuFilters
                width: 100
                height: 30
                leftPadding: 15
                contentItem: Text {
                            leftPadding: 10
                            rightPadding: 10
                            text: menuFilters.text
                            color: "#A2A2A2"
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                background: Rectangle {
                      implicitWidth: 100
                      implicitHeight: 30
                      color: menuFilters.highlighted ? "#404347" : "transparent"
                  }
            }
        }

        Menu {
            title: qsTr("Functions")            
            Action {
                text: "Transform"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 16)
                }
            }
            Action {
                text: "Tile"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 17)
                }
            }
            Action {
                text: "Warp"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 18)
                }
            }            
            Action {
                text: "Mirror"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 5)
                }
            }            
            background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 30
                            color: "#2C2D2F"
                        }
            delegate: MenuItem {
                id: menuFunctions
                width: 100
                height: 30
                leftPadding: 15
                contentItem: Text {
                            leftPadding: 10
                            rightPadding: 10
                            text: menuFunctions.text
                            color: "#A2A2A2"
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                background: Rectangle {
                      implicitWidth: 100
                      implicitHeight: 30
                      color: menuFunctions.highlighted ? "#404347" : "transparent"
                  }
            }
        }
    }

    MouseArea {
        anchors.top: tabsArea.bottom
        anchors.left: leftDock.right
        anchors.right: rightDock.left
        height: parent.height - menuBar.height - tabsArea.height
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
            if(tabsList.children.length > 0 && mouse.buttons == Qt.RightButton) {
                addNode.popup()
            }
        }
    }
    ColorSettings {
        id:colors
    }

    onAddTab: {
        tab.scene.layer.enabled = true
        tab.scene.layer.samples = 8
        tab.x = tabsList.childrenRect.width
        tab.parent = tabsList
        tab.width = Qt.binding(function(){return 200*tabsList.children.length > tabsList.width ? Math.floor(tabsList.width/tabsList.children.length) : 200})
        tabsList.tabs.push(tab)
    }
    onDeleteTab: {
        tabsList.tabs.splice(tabsList.tabs.indexOf(tab), 1)
    }
    onPropertiesPanelChanged: {
        if(newPanel) {
            newPanel.parent = flickable.contentItem
        }
        if(oldPanel) oldPanel.parent = null

    }
    onPreviewUpdate: {
        if((preview.pinned && mainWindow.activeNode == mainWindow.pinnedNode) || !preview.pinned) {
            preview.canUpdatePreview = true
        }
        else {
            preview.canUpdatePreview = false
        }

        preview.setPreviewData(previewData, useTexture)
    }

    onPreview3DChanged: {
        if(oldPreview) {
            oldPreview.visible = false
        }
        if(newPreview) {
            newPreview.visible = true
            newPreview.parent = dragPreview3D
            newPreview.y = 25
            newPreview.height = 240
            newPreview.width = Qt.binding(function(){return newPreview.parent.width})
            newPreview.height = Qt.binding(function(){return newPreview.parent.height - 27})
            primitivesType.currentIndex = newPreview.primitivesType
        }
    }

    onResolutionChanged: {
        if(res == Qt.vector2d(512, 512)) {
            resGroup.checkedAction = res512
        }
        else if(res == Qt.vector2d(1024, 1024)) {
            resGroup.checkedAction = res1024
        }
        else if(res == Qt.vector2d(2048, 2048)) {
            resGroup.checkedAction = res2048
        }
        else if(res == Qt.vector2d(4096, 4096)) {
            resGroup.checkedAction = res4096
        }
    }

    DockPanel {
        id: rightDock
        position: "right"
        y: 30
        height: parent.height - 30
        z: 1
        limit: leftDock.width + 5 + tabsList.children.length*50
        DragContainer {
            id: dragProperties
            title: "Properties"
            y: dragPreview.height
            parent: rightDock.container
            width: parent.width
            height: parent.height - dragPreview.height
            clip: true
            Flickable {
                id: flickable
                y: 25
                width: parent.width
                height: parent.height - 27
                contentHeight: contentItem.childrenRect.height
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                MouseArea {
                    y: flickable.visibleArea.yPosition * flickable.height
                    width: flickable.width
                    height: flickable.height

                    onClicked: {
                        flickable.focus = true
                        flickable.focus = false
                    }
                }

                ScrollBar.vertical: ScrollBar{
                     z: active ? 0 : -1
                }
            }
        }
        DragContainer{
            id: dragPreview
            title: "Node View"
            parent: rightDock.container
            width: parent.width
            height: 280

            PreviewObject {
                id: preview
                y: 25
                width: parent.width
                height: parent.height - 27
            }

            Rectangle {
                id: pin
                x: parent.width - width - 5
                y: 30
                radius: 2
                width: 24
                height: 24
                color: "#303133"
                Image {
                    x: 2
                    y: 2
                    source: preview.pinned ? "qrc:/icons/pin-active (1).svg" : "qrc:/icons/pin (2).svg"
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        preview.pinned = !preview.pinned
                        mainWindow.pin(preview.pinned)
                        if((preview.pinned && mainWindow.activeNode == mainWindow.pinnedNode) ||
                                !preview.pinned) {
                            preview.canUpdatePreview = true
                        }
                        else {
                            preview.canUpdatePreview = false
                        }
                    }
                }                
            }
        }        
    }
    DockPanel {
        id: leftDock
        y: 30
        height: parent.height - 30
        z: 1
        limit: rightDock.width + 5 + tabsList.children.length*50
        DragContainer {
            title: "Material View"
            id: dragPreview3D
            width: parent.width
            parent: leftDock.container
            height: 240
            Rectangle{
                id: itemParams
                x: parent.width - width - 5
                y: 30
                z: 1
                width: 24
                height: 24
                radius: 2
                visible: tabsList.children.length > 0
                color: "#303133"
                Image {
                    x: 4
                    y: 4
                    parent: preview3DParams.opened ? paramsClose : itemParams
                    source: "qrc:/icons/params (2).svg"
                }
                MouseArea {
                    z: 1
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        preview3DParams.open()
                    }
                }
                Popup {
                    id: preview3DParams
                    x: -width + parent.width
                    z: 0
                    width: 150
                    height: contentItem.implicitHeight
                    padding: 0

                    MouseArea {
                        id: paramsClose
                        x: 126
                        z: 1
                        width: 24
                        height: 24
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            preview3DParams.close()
                        }
                    }

                    contentItem: Item {
                        width: 150
                        implicitHeight: 200
                        Text {
                            x: 28
                            y: 20
                            text: qsTr("Mesh")
                            color: "#A2A2A2"
                        }
                        ParamDropDown {
                            id: primitivesType
                            width: 90
                            x: 5
                            y: 40
                            model: ["Sphere", "Cube", "Plane"]
                            popupColor: "#353638"
                            onActivated: {
                                changePrimitive(index)
                                focus = false
                            }
                        }

                        Text {
                            x: 28
                            y:80
                            text: qsTr("Tile")
                            color: "#A2A2A2"
                        }
                        ParamDropDown {
                            id: tilesType
                            width: 70
                            x: 5
                            y: 100
                            model: ["1x", "2x", "3x", "4x"]
                            popupColor: "#353638"
                            onActivated: {
                                changeTilePreview3D(index)
                                focus = false
                            }
                        }
                    }
                    background:
                        Rectangle {
                            width: 150
                            radius: 2
                            color: "#2C2D2F"
                        }
                    }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        z: -1
        color: "#555555"
    }

    Rectangle {
        anchors.fill: tabsList
        color: "gray"
    }

    DropArea {
        property var dragItem
        property real dragX
        property string key: "tabsPanel"
        id: tabsArea
        height: 25
        width: parent.width - (leftDock.container.contain ? leftDock.width + 5 : 0) -
                              (rightDock.container.contain ? rightDock.width + 5 : 0)
        enabled: tabsList.children.length > 0
        keys: [key]
        y: 30
        x: leftDock.container.contain ? leftDock.width + 5 : 0
        z: 1

        onPositionChanged: {
            var target = null
            for(var i = 0; i < tabsList.children.length; ++i) {
                var item = tabsList.children[i]
                if((item.x < drag.source.x + drag.source.width*0.5) && (item.x + item.width >
                    drag.source.x + drag.source.width*0.5)) {
                    target = item
                    break;
                }
            }
            if(target) {
                var x = target.x
                target.x = drag.source.baseParent.x
                drag.source.baseParent.x = x
            }
        }

        Rectangle {
            color: "#2C2D2E"
            anchors.fill: parent
            Rectangle {
                y: parent.height - 1
                height: 2
                width: parent.width
                color: "#2C2D2F"
            }
        }

        Item {
            property var tabs: []
            id: tabsList
            width: parent.width
            height: 25
            function updateChildrenPos() {
                var sortChildren = []
                for(var i = 0; i < tabsList.children.length; ++i) {
                    var child = tabsList.children[i]
                    var index = sortChildren.length
                    for(var j = 0; j < sortChildren.length; ++j) {
                        if(child.x < sortChildren[j].x) {
                            index = j;
                            break;
                        }
                    }
                    sortChildren.splice(index, 0, child)
                }
                var lastX = 0
                var step = tabsList.width < 200*tabsList.children.length ?
                           Math.floor(tabsList.width/tabsList.children.length) : 200
                for(var i = 0; i < sortChildren.length; ++i) {
                    sortChildren[i].x = lastX
                    lastX += step
                }
            }

            onWidthChanged: {
                updateChildrenPos()
            }
            onChildrenChanged: {
                updateChildrenPos()
            }
        }
    }
}
