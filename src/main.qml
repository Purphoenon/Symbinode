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
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
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
                                    width: 14
                                    height: 14
                                    radius: 6.5
                                    anchors.centerIn: parent
                                    visible: menuResolution.checkable
                                    color: "#484C51"
                                    Rectangle {
                                        width: 6
                                        height: 6
                                        radius: 3
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
            Action {
                text: "Height"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 4)
                }
            }
            Action {
                text: "Emission"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 23)
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
            Action {
                text: "Gradient"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 25)
                }
            }
            Action {
                text: "Bricks"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 31)
                }
            }
            Action {
                text: "Hexagons"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 32)
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
            Action {
                text:  "Grayscale"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 24)
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
            Action {
                text: "Blur Directional"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 27)
                }
            }
            Action {
                text: "Slope Blur"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 28)
                }
            }
            Action {
                text: "Bevel"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 29)
                }
            }
            Action {
                text: "Flood Fill"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 33)
                }
            }
            Action {
                text: "Flood Fill to Gradient"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 34)
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
            Action {
                text: "Warp Directional "
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 26)
                }
            }
            Action {
                text: "Polar Transform"
                onTriggered: {
                    mainWindow.createNode(addNode.x, addNode.y, 30)
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
        MenuSeparator {
            contentItem: Rectangle {
                implicitWidth: 200
                implicitHeight: 1
                color: "#3B3B3B"
            }
        }
        Action {
            text: "Frame"
            onTriggered: {
                mainWindow.createFrame(addNode.x, addNode.y)
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
    onPropertiesPanelChanged: {
        if(newPanel) {
            newPanel.parent = flickable.contentItem
        }
        if(oldPanel) oldPanel.parent = null
    }

    onPreviewUpdate: {
        preview.setPreviewData(previewData)
    }

    onPreview3DChanged: {
        if(oldPreview) {
            oldPreview.visible = false
            dragPreview3D.resized.disconnect(oldPreview.sizeUpdated)
        }
        if(newPreview) {
            newPreview.visible = true
            newPreview.parent = dragPreview3D
            newPreview.y = 25
            newPreview.height = 240
            newPreview.width = Qt.binding(function(){return newPreview.parent.width})
            newPreview.height = Qt.binding(function(){return newPreview.parent.height - 27})
            dragPreview3D.resized.connect(newPreview.sizeUpdated)
            primitivesType.currentIndex = newPreview.primitivesType
            tilesType.currentIndex = newPreview.tilesSize - 1
            heightScale.propertyValue = 10*newPreview.heightScale
            emissiveStrenght.propertyValue = newPreview.emissiveStrenght
            bloomRadius.propertyValue = newPreview.bloomRadius
            bloomIntensity.propertyValue = newPreview.bloomIntensity
            bloomThreshold.propertyValue = newPreview.bloomThreshold
            bloom.checked = newPreview.bloom
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

    onTabClosing: {
        if(tab.scene.modified) {
            var exitDialogComponent = Qt.createComponent("qml/ExitDialog.qml")
            if(exitDialogComponent.status == Component.Ready) {
                var exitDialogObject = exitDialogComponent.createObject(mainWindow, {fileName: tab.title})
                function saveFunction() {
                    var saved = tab.save()
                    if(saved) {
                        mainWindow.closeTab(tab)
                        var index = tabsList.tabs.indexOf(tab)
                        tabsList.tabs.splice(index, 1)                        
                    }
                    exitDialogObject.accepted.disconnect(saveFunction)
                    exitDialogObject.destroy()
                }
                exitDialogObject.accepted.connect(saveFunction)
                exitDialogObject.discard.connect(function() {
                    mainWindow.closeTab(tab)
                    var index = tabsList.tabs.indexOf(tab)                    
                    tabsList.tabs.splice(index, 1)                    
                    exitDialogObject.destroy()

                })
                exitDialogObject.rejected.connect(function(){
                    exitDialogObject.destroy()
                })
            }
        }
        else {
            console.log(tabsList)
            mainWindow.closeTab(tab)
            var index = tabsList.tabs.indexOf(tab)
            tabsList.tabs.splice(index, 1)            
        }
    }

    function checkTabsOnExit(index) {
        if(index < mainWindow.tabsCount()) {
            var tab = mainWindow.tab(index)
            /*console.log(tabsList.tabs.length)
            console.log(tabsList.children.length)
            console.log("scene " + tab.scene + " tab " + tab)*/
            if(tab.scene.modified) {
                var exitDialogComponent = Qt.createComponent("qml/ExitDialog.qml")
                function saveTabsScene() {
                    if(tab.save()) checkTabsOnExit(index + 1)
                    exitDialogObject.accepted.disconnect(saveTabsScene)
                    exitDialogObject.destroy()
                }
                function cancel() {
                    exitDialogObject.rejected.disconnect(cancel)
                    exitDialogObject.destroy()
                    //console.log("cancel")
                }

                if(exitDialogComponent.status == Component.Ready) {
                    var exitDialogObject = exitDialogComponent.createObject(mainWindow, {fileName: tab.title})
                    exitDialogObject.accepted.connect(saveTabsScene)
                    exitDialogObject.discard.connect(function(){                        
                        checkTabsOnExit(index + 1)
                        exitDialogObject.destroy()
                        //console.log("discard")
                    })
                    exitDialogObject.rejected.connect(cancel)
                }
            }
            else checkTabsOnExit(index + 1)
        }
        else Qt.quit()
    }

    Connections {
        target: mainWindow
        onClosing: {            
            close.accepted = false
            checkTabsOnExit(0)
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
                property bool pinned: false
                id: pin
                x: parent.width - width - 5
                y: 30
                z: 1
                radius: 2
                width: 24
                height: 24
                color: "#303133"
                Image {
                    x: 2
                    y: 2
                    source: pin.pinned ? "qrc:/icons/pin-active (1).svg" : "qrc:/icons/pin (2).svg"
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if(mainWindow.activeNode || pin.pinned) {
                            pin.pinned = !pin.pinned
                            mainWindow.pin(pin.pinned)
                        }
                    }
                }                
            }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onPressed: {
                    if(mouse.buttons == Qt.RightButton && mouse.modifiers == Qt.NoModifier){
                        nodeViewParams.popup()
                    }
                    else {
                        mouse.accepted = false
                    }
                }
            }
            Menu {
                id: nodeViewParams
                Action {
                    text: "Save texture"
                    onTriggered: {
                        mainWindow.saveCurrentTexture()
                    }
                }
                background: Rectangle {
                                implicitWidth: 120
                                implicitHeight: 30
                                color: "#2C2D2F"
                            }
                delegate: MenuItem {
                    id: menuNodeView
                    width: 120
                    height: 30
                    leftPadding: 15
                    contentItem: Text {
                                leftPadding: 10
                                rightPadding: 10
                                text: menuNodeView.text
                                color: "#A2A2A2"
                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }
                    background: Rectangle {
                          implicitWidth: 120
                          implicitHeight: 30
                          color: menuNodeView.highlighted ? "#404347" : "transparent"
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
        onResized: {
            dragPreview3D.resized(resized)
        }
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
                    width: 200
                    height: contentItem.implicitHeight
                    padding: 0

                    MouseArea {
                        id: paramsClose
                        x: parent.width - width
                        z: 1
                        width: 24
                        height: 24
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            preview3DParams.close()
                        }
                    }

                    contentItem: Item {
                        width: 200
                        implicitHeight: bloom.checked ? 330 : 230
                        Text {
                            x: 15
                            y: 26
                            text: qsTr("Mesh")
                            color: "#A2A2A2"
                        }
                        ParamDropDown {
                            id: primitivesType
                            width: 90
                            x: 40
                            y: 20
                            model: ["Sphere", "Cube", "Plane"]
                            popupColor: "#353638"
                            onActivated: {
                                changePrimitive(index)
                                focus = false
                            }
                        }

                        Text {
                            x: 15
                            y: 61
                            text: qsTr("Tile")
                            color: "#A2A2A2"
                        }
                        ParamDropDown {
                            id: tilesType
                            width: 70
                            x: 40
                            y: 55
                            model: ["1x", "2x", "3x", "4x"]
                            popupColor: "#353638"
                            onActivated: {
                                changeTilePreview3D(index)
                                focus = false
                            }
                        }

                        Item {
                            width: parent.width - 40
                            height: childrenRect.height
                            x: 10
                            y: 105
                            clip: true
                            ParamSlider {
                                id: heightScale
                                //x: 5
                                //width: parent.width - 10
                                maximum: 1
                                propertyName: "Height scale"
                                onPropertyValueChanged: {
                                    changeHeightScale(propertyValue)
                                }
                            }

                            ParamSlider {
                                id: emissiveStrenght
                                //x: 5
                                y: 18
                                //width: parent.width - 10
                                maximum: 10
                                propertyName: "Emissive strength"
                                onPropertyValueChanged: {
                                    changeEmissiveStrenght(propertyValue)
                                }
                            }

                            ParamSlider {
                                id: bloomThreshold
                                visible: bloom.checked
                                //x: 5
                                y: 88
                                //width: parent.width - 10
                                maximum: 5
                                propertyName: "Bloom threshold"
                                onPropertyValueChanged: {
                                    changeBloomThreshold(propertyValue)
                                }
                            }

                            ParamSlider {
                                id: bloomRadius
                                visible: bloom.checked
                                //x: 5
                                y: 121
                                //width: parent.width - 10
                                minimum: 1
                                maximum: 10
                                propertyName: "Bloom radius"
                                onPropertyValueChanged: {
                                    changeBloomRadius(propertyValue)
                                }
                            }

                            ParamSlider {
                                id: bloomIntensity
                                visible: bloom.checked
                                //x: 5
                                y: 154
                                //width: parent.width - 10
                                maximum: 1
                                propertyName: "Bloom intensity"
                                onPropertyValueChanged: {
                                   changeBloomIntensity(propertyValue)
                                }
                            }
                        }

                        ParamCheckbox {
                            id: bloom
                            x: 5
                            y: 180
                            width: 70
                            text: "Bloom"
                            onToggled: {
                                changeBloom(checked)
                            }
                        }                        
                    }
                    background:
                        Rectangle {
                            width: 200
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
