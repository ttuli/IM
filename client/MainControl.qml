import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: control

    property int currentIndex: 0

    property var options: ["好友列表", "验证消息"];

    signal reactSig(int index,string account,bool result);
    signal readyTalkSig(string name,string account);
    signal unShowTipSig();
    signal deleteFriendSig(string tar,int position);

    property bool showTip:false;
    function setTip(value)
    {
        showTip=value;
    }

    Rectangle {
        anchors.fill: parent
        radius: 8
        color: "#F5F5F5"
    }

    RowLayout {
        id:rowLayout;
        width: parent.width/1.5;
        anchors.horizontalCenter: parent.horizontalCenter;
        height: 60;
        spacing: 50;

        Repeater {
            model: 2

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // 点击区域
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if(control.currentIndex !== index) {
                            control.currentIndex = index
                            friendList.visible=(options[index]==="好友列表");
                        }
                        if(options[index]==="验证消息"){
                            showTip=false;
                            unShowTipSig();
                        }
                    }
                }

                // 文字
                Text {
                    text: control.options[index]
                    anchors.centerIn: parent
                    font.pixelSize: 30;
                    font.bold: true;
                    color: control.currentIndex === index ? "#2196F3" : "#757575"

                    Image {
                        source: "/new/prefix1/res/warning.svg";
                        width: 20;
                        height: 20;
                        anchors.right: parent.right;
                        anchors.top: parent.top;
                        anchors.rightMargin: -15;
                        anchors.topMargin: -10;
                        visible: (options[index]==="验证消息"&&showTip);
                    }
                }


                // 选中指示器
                Rectangle {
                    width: parent.width
                    height: 2
                    anchors.bottom: parent.bottom
                    color: "#2196F3"
                    visible: control.currentIndex === index

                }
            }
        }
    }


    FriendList{
        id:friendList;
        anchors.top: rowLayout.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        visible: true;

        onS_readyTalkSig: function(name,account)
        {
            readyTalkSig(name,account);
        }
        onS_deleteFriendSig: function(tar,position)
        {
            deleteFriendSig(tar,position);
        }
    }

    VerifyDia{
        id:verifyDia;
        anchors.top: rowLayout.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        visible: !friendList.visible;

        onS_reactSig: function(index,account,result)
        {
            reactSig(index,account,result);
        }

    }

    function verifyDia_setFinishVisible(value)
    {
        verifyDia.setFinishVisible(value)
    }

    function verifyDia_showMsg(text,isSuccess)
    {
        verifyDia.showMsg(text,isSuccess);
    }

}
