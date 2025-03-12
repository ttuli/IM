import QtQuick
import QtQuick.Window

Rectangle{
    color:"#f9f7f7";

    signal closeSig();
    signal flushSig();

    property int btnImaMargin: 5;

    function setloadingCaptionVisible(text,value)
    {
        loadingCaption_text.text=text;
        loadingCaption.visible=value;
    }
    function isInteractiveControl(x1, y1)
    {
        var item = childAt(x1, y1)
        return item!==null;
    }

    Rectangle{
        id:flushBtn;
        anchors.left: parent.left;
        height: parent.height-5;
        anchors.verticalCenter: parent.verticalCenter;
        width:flush_ima.width+flush_text.contentWidth+20;
        color: "white";
        radius: 10;

        objectName:"flushBtn";

        Image {
            id:flush_ima;
            anchors.left: parent.left;
            anchors.leftMargin: 5;
            height: parent.height;
            width: height;
            source: "/new/prefix1/res/flushs.svg";

            RotationAnimation on rotation {
                id:flush_ima_rotate;
                from: 0;
                to: 360;
                duration: 1000;
                running: false;
            }
        }

        Text {
            id:flush_text;
            text: qsTr("刷新");
            font.bold: true;
            font.pixelSize: 15;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: flush_ima.right;
            anchors.leftMargin: 5;
        }

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            cursorShape: Qt.PointingHandCursor;

            onClicked: {
                if(!flush_ima_rotate.running)
                    flush_ima_rotate.running=true;
                flushSig();
            }
        }
    }

    Rectangle{
        id:loadingCaption;
        height: parent.height;
        width: loadingCaption_text.contentWidth;
        anchors.horizontalCenter: parent.horizontalCenter;
        color:"transparent";

        Image {
            id: loading_ima;
            source: "/new/prefix1/res/loading.png";
            width: height;
            height: parent.height-5;
            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;

            RotationAnimation on rotation {
                from: 0;
                to: 360;
                duration: 1000;
                loops: Animation.Infinite;
                running: true;
            }
        }

        Text {
            id: loadingCaption_text;
            text: qsTr("text");
            anchors.left: loading_ima.right;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;
            font.bold: true;
            font.pixelSize: 17;
        }
    }

    Rectangle{
        id:closebtn;
        anchors.right: parent.right;
        height: parent.height;
        width: height
        color:"transparent";

        Image {
            anchors.fill: parent;
            source: "/new/prefix1/res/XMark.svg";
            anchors.margins: 3;
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
    }//关闭

    signal minSig();

    Rectangle{
        id:minbtn;
        height: parent.height;
        width: height;
        anchors.right: closebtn.left;
        color:"transparent";

        Image {
            source: "/new/prefix1/res/Minus.svg";
            anchors.fill: parent;
            anchors.margins: 3;
        }

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;

            onClicked: {
                minSig();
            }

            onEntered: {
                parent.color="#c9d6df";
            }
            onExited: {
                parent.color="transparent";
            }
        }

    }//最小化

    signal maxSig();
    property bool isMax: false;

    function setIsMax(value)
    {
        isMax=value;
    }

    Rectangle{
        id:maxbtn;
        height: parent.height;
        width: height;
        anchors.right: minbtn.left;
        color:"transparent";

        objectName: "maxbtn";

        Image {
            source: isMax?"/new/prefix1/res/nscreen.svg":"/new/prefix1/res/fullscreen.svg";
            anchors.fill: parent;
            anchors.margins: 3;
        }

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            propagateComposedEvents: true
            onClicked:{
                maxSig();
            }
            onEntered:{
                parent.color="#c9d6df";
            }
            onExited:{
                parent.color="transparent";
            }
        }
    }
}
