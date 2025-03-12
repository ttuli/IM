import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{
    signal s_readyTalkSig(string name,string account);
    signal s_deleteFriendSig(string tar,int position);
    property int currentPosition: 0;

    Rectangle{
        id:select_rec;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        height: 35;


        Rectangle{
            id:friend_choice;
            height: parent.height;
            width: parent.width/2;
            property int position: 0;

            Text {
                anchors.centerIn: parent;
                text: qsTr("好友")
                font.bold: true;
                font.pixelSize: 20;
            }

            Rectangle{
                id:friend_underline;
                width:{
                    return currentPosition===parent.position?parent.width:0;
                }

                anchors.bottom: parent.bottom;
                height: 2;
                color: "#2196F3";

                Behavior on width {
                    NumberAnimation{
                        duration: 80;
                    }
                }
            }

            MouseArea{
                anchors.fill: parent;
                onClicked: {
                    currentPosition=parent.position;
                }
            }
        }

        Rectangle{
            id:group_choice;
            height: parent.height;
            width: parent.width/2;
            anchors.left: friend_choice.right;
            property int position: 1;

            Text {
                anchors.centerIn: parent;
                text: qsTr("群聊")
                font.bold: true;
                font.pixelSize: 20;
            }

            Rectangle{
                id:group_underline;
                width:{
                    return currentPosition===parent.position?parent.width:0;
                }

                anchors.bottom: parent.bottom;
                height: 2;
                color: "#2196F3";

                Behavior on width {
                    NumberAnimation{
                        duration: 80;
                    }
                }
            }

            MouseArea{
                anchors.fill: parent;
                onClicked: {
                    currentPosition=parent.position;
                }
            }
        }
    }

    GridView{
        id:friend_view;
        width: parent.width;
        anchors.top: select_rec.bottom;
        anchors.bottom: parent.bottom;
        clip: true;

        property int position: 0;
        visible: currentPosition===position;

        cellWidth: width/3;
        cellHeight: 110;

        model:friendListModel;

        delegate: Rectangle{
            width: friend_view.cellWidth;
            height: friend_view.cellHeight;

            color: "transparent";
            Rectangle{
                anchors.fill: parent;
                anchors.margins: 5;
                radius: 10;

                smooth: true;
                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8
                    samples: 16
                    color: "#50000000"
                }

                Image {
                    anchors.top: parent.top;
                    anchors.right: parent.right;
                    anchors.margins: 10;
                    width: 20;
                    height: width;
                    source: "/new/prefix1/res/delete.svg";

                    ToolTip{
                        id:delete_msg;
                        visible: false;
                    }

                    MouseArea{
                        anchors.fill: parent;
                        hoverEnabled: true;
                        cursorShape: Qt.PointingHandCursor;
                        onEntered: {
                            delete_msg.show("删除好友",3000);
                        }
                        onExited: {
                            delete_msg.hide();
                        }
                        onClicked: {
                            s_deleteFriendSig(account,index);
                        }
                    }
                }

                Image {
                    id:uima;
                    source: "image://imageprovider/"+account
                    visible: false;
                }
                Image {
                    id: outlineima;
                    source: "/new/prefix1/res/circle.png";
                    visible: false;
                }

                OpacityMask{
                    id: image_;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: 5;
                    height: parent.height-28;
                    width: height;
                    maskSource: outlineima;
                    source: uima;

                    smooth: true;
                    layer.enabled: true;
                    layer.effect: DropShadow{
                        radius: 8
                        samples: 16
                        color: "#50000000"
                    }
                }

                Text {
                    anchors.left: image_.right;
                    anchors.top: parent.top;
                    anchors.topMargin: 5;
                    anchors.leftMargin: 5;
                    text: name;
                    font.bold: true;
                    font.pixelSize: 23;
                }

                Rectangle{
                    id:btn;
                    anchors.right:parent.right;
                    anchors.bottom: parent.bottom;
                    anchors.margins: 5;
                    width: btn_text.implicitWidth+20;
                    height: btn_text.implicitHeight+10;
                    color:"#00CD00";
                    radius: 8;

                    layer.enabled: true;
                    layer.effect: DropShadow{
                        radius: 8;
                        samples: 16;
                        color: "grey";
                    }

                    Text {
                        id: btn_text;
                        anchors.centerIn: parent;
                        text: "去聊天";
                        color: "white";
                        font.bold: true;
                        font.pixelSize: 20;
                    }

                    MouseArea{
                        anchors.fill: parent;
                        cursorShape: Qt.PointingHandCursor;
                        onClicked: {
                            if(name==="")
                                return;
                            s_readyTalkSig(name,account);
                        }
                    }
                }
            }
        }
    }


    GridView{
        id:group_view;
        width: parent.width;
        anchors.top: select_rec.bottom;
        anchors.bottom: parent.bottom;
        clip: true;

        property int position: 1;
        visible: currentPosition===position;


        cellWidth: width/4;
        cellHeight: cellWidth;

        delegate: Rectangle{
            width: group_view.cellWidth;
            height: group_view.cellHeight;

            color: "transparent";
            Rectangle{
                anchors.fill: parent;
                anchors.margins: 5;
            }
        }
    }


}
