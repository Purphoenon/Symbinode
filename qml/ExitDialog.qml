import QtQuick 2.12
import QtQuick.Dialogs 1.2

MessageDialog {
    property string fileName
    id: exitDialog
    title: "Symbinode"
    icon: StandardIcon.Warning
    text: "The document " + "'" + fileName + "'" + " has been modified."
    informativeText: "Save the changes before closing?"
    standardButtons: StandardButton.Save | StandardButton.Discard | StandardButton.Cancel
    Component.onCompleted: visible = true
}
