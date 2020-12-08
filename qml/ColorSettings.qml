import QtQuick 2.9
import Qt.labs.settings 1.1

Settings {
    property color node_selected: "#EE8738"
    property color node_hovered: "#FFC599"
    property color node_title_background: "#4A4A4A"
    property color node_content_background: "#E3212121"
    property var node_socket_background: ["#FFFF7700", "#FF52E220", "#FF0056A6", "#FFA86DB1", "#FFB54747",
                                          "#FFDBE220"]
    property color edge: "#001000"
    property color edge_selected: "#00ff00"
}
