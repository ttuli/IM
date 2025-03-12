import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Rectangle{
    id:caption;
    color:"#2D2D2D";
    signal closeSig();
    signal maxSig();
    signal minSig();

    property int imageMargin: 10;

    function isInteractiveControl(x1, y1)
    {
        var item = childAt(x1, y1)
        return item!==null;
    }
    function setIsMax(value)
    {
        isMax=value;
    }
    property bool isMax: false;

    Rectangle{
        id:closebtn;
        height: parent.height;
        width: height;
        anchors.right: parent.right;
        color:"transparent";
        Image {
            anchors.fill: parent;
            anchors.margins: imageMargin;
            source: "/new/prefix1/res/x_white.svg";
        }
        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                parent.color="red";
            }
            onExited: {
            parent.color="transparent";
            }
            onClicked: {
                closeSig();
            }
        }
    }
    Rectangle{
        id:maxbtn;
        height: parent.height;
        width: height;
        color:"transparent";
        anchors.right: closebtn.left;
        Image {
            anchors.fill: parent;
            anchors.margins: imageMargin;
            source: isMax?"/new/prefix1/res/unmax_white.svg":"/new/prefix1/res/max_white.svg";
        }
        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                parent.color="#4A90E2";
            }
            onExited: {
            parent.color="transparent";
            }
            onClicked: {
                maxSig();
            }
        }

    }
    Rectangle{
        id:minbtn;
        height: parent.height;
        width: height;
        color:"transparent";
        anchors.right: maxbtn.left;
        Image {
            anchors.fill: parent;
            anchors.margins: imageMargin;
            source: "/new/prefix1/res/min_width.svg";
        }
        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                parent.color="#4A90E2";
            }
            onExited: {
            parent.color="transparent";
            }
            onClicked: {
                minSig();
            }
        }
    }
}
