import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{
    color:"#3f72af";

    function setImage(acc)
    {
        user_ima.source="";
        user_ima.source="image://imageprovider/"+acc;
    }

    function setTip(value)
    {
        tip.visible=value;
    }

    function setCurrentIndex(index)
    {
        selectedIndex=index;
    }

    function setTotalMsgNum(value)
    {
        totalMsgNum=value;
    }

    Rectangle{
        id:caption;
        color:"transparent";
        anchors.top: parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: parent.width;
        height: 50;

        Image {
            id:caption_ima;
            source: "/new/prefix1/res/IM_ICON.svg";
            anchors.left: parent.left;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;
            height: parent.height-25;
            width: height;
        }

        Text {
            anchors.left: caption_ima.right;
            anchors.leftMargin: 3;
            anchors.verticalCenter: parent.verticalCenter;
            text: qsTr("IM");
            verticalAlignment: Text.AlignVCenter;
            font.bold: true;
            font.pixelSize: 17;
        }
    }

    property int cubeSizdeWidth:width-10;
    property int imaMargin: 7;

    property int selectedIndex:0;
    property int hoverdIndex:-1;

    property int totalMsgNum: 0;

    signal choiceSig(int index);

    Column{
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: caption.height;
        spacing: 20;


        Rectangle{
            property int index: 0;
            color:{
                if(selectedIndex===index)
                    return "#90ffffff";
                else if(hoverdIndex===index)
                    return "#50ffffff";
                else
                    return "transparent";
            }

            radius: 8;
            width: cubeSizdeWidth;
            height: cubeSizdeWidth;

            Image {
                source: "/new/prefix1/res/message.svg";
                anchors.fill: parent;
                anchors.margins: imaMargin;
            }

            Rectangle {
                id: totalMsg;
                color:"red";
                radius: width/2;
                width: 20;
                height: 20;
                anchors.right: parent.right;
                anchors.top: parent.top;
                anchors.margins: -5;
                visible: totalMsgNum!==0
                Text {
                    anchors.centerIn: parent;
                    color:"white";
                    text: totalMsg>99?"99+":totalMsgNum;
                    font.pixelSize: 16;
                    font.bold: true;
                }
            }

            MouseArea{
                anchors.fill: parent;
                cursorShape: Qt.PointingHandCursor;
                hoverEnabled: true;
                onEntered: {
                    hoverdIndex=parent.index;
                }
                onExited: {
                    hoverdIndex=-1;
                }
                onClicked: {
                    selectedIndex=parent.index;
                    choiceSig(parent.index);
                }
            }
        }

        Rectangle{
            property int index: 1;
            color:{
                if(selectedIndex===index)
                    return "#90ffffff";
                else if(hoverdIndex===index)
                    return "#50ffffff";
                else
                    return "transparent";
            }
            radius: 8;
            width: cubeSizdeWidth;
            height: cubeSizdeWidth;


            Image {
                source: "/new/prefix1/res/friend.svg";
                anchors.fill: parent;
                anchors.margins: imaMargin;
            }

            Image {
                id: tip;
                source: "/new/prefix1/res/warning.svg";
                width: 20;
                height: 20;
                anchors.right: parent.right;
                anchors.top: parent.top;
                anchors.margins: -5;
                visible: false;
            }

            MouseArea{
                anchors.fill: parent;
                cursorShape: Qt.PointingHandCursor;
                hoverEnabled: true;
                onEntered: {
                    hoverdIndex=parent.index;
                }
                onExited: {
                    hoverdIndex=-1;
                }
                onClicked: {
                    selectedIndex=parent.index;
                    choiceSig(parent.index);
                }
            }
        }
    }

    signal checkInfoSig();
    Rectangle{
        id:user_ima_rec;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        color:"transparent";

        width: cubeSizdeWidth;
        height: width;
        radius: 30;

        Image{
            id:circle_ima;
            visible: false;
            source: "/new/prefix1/res/circle.png";
        }

        Image {
            id:user_ima;
            source: "/new/prefix1/res/mountains-9372609_1280.jpg";
            cache: false;
            visible: false;
        }

        Image {
            id: online_ima;
            source: "/new/prefix1/res/online.png";
            visible: false;
        }

        OpacityMask{
            source: user_ima;
            maskSource: circle_ima;
            anchors.fill: parent;
        }


        OpacityMask{
            property int margin: -1;
            source: online_ima;
            maskSource: circle_ima;
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;
            anchors.rightMargin: margin;
            anchors.bottomMargin: margin;
            width: 20;
            height: width;
        }

        MouseArea{
            anchors.fill: parent;
            cursorShape: Qt.PointingHandCursor;
            onClicked: {
                checkInfoSig();
            }
        }
    }

    signal addFriendSig();
    Rectangle{
        id:add_friend_rec;
        color:"transparent";
        anchors.bottom: user_ima_rec.top;
        anchors.bottomMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;

        radius: 8;
        width: cubeSizdeWidth;
        height: cubeSizdeWidth;

        Image {
            source: "/new/prefix1/res/Plus.svg";
            anchors.fill: parent;
            anchors.margins: imaMargin;
        }

        MouseArea{
            anchors.fill: parent;
            cursorShape: Qt.PointingHandCursor;
            hoverEnabled: true;
            onEntered: {
                parent.color="#50ffffff";
            }
            onExited: {
                parent.color="transparent";
            }
            onClicked: {
                addFriendSig();
            }
        }
    }
}
