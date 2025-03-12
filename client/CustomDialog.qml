import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Rectangle{
    color:"#F5F5F5";

    function init(text,showAccept,showReject)
    {
        information.text=text;
        confirm.visible=showAccept;
        reject.visible=showReject;
    }

    signal rejectSig();
    signal acceptSig();

    Rectangle{
        width: parent.width;
        height: closebtn.height;
        color: "#DCDCDC";
    }

    Rectangle{
        id:closebtn;
        width: 25;
        height: width;
        anchors.right: parent.right;
        anchors.top:parent.top;
        color:"transparent";
        Image {
            anchors.fill:parent;
            source: "/new/prefix1/res/XMark.svg"
        }
        MouseArea{
            anchors.fill:parent;
            hoverEnabled: true;
            onEntered: {
                parent.color="red";
            }
            onExited: {
                parent.color="transparent";
            }
            onClicked: {
                rejectSig();
            }
        }
    }

    Text {
        id: information;
        text: "";
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: closebtn.bottom;
        anchors.topMargin: 15;
        font.pixelSize: 18;
    }

    Row{
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 20;
        spacing: 60;
        Rectangle{
            id:confirm;
            width:60;
            height: 25;
            radius: 5;
            color:"#00CD00";

            layer.enabled: true;
            layer.effect: DropShadow{
                radius: 8;
                samples: 16;
                color: "#50000000";
            }

            Text {
                anchors.centerIn: parent;
                text:"确定";
                font.pixelSize: 15;
                color:"white";
            }
            MouseArea{
                anchors.fill:parent;
                onPressed: {
                    parent.color="#008B00";
                }
                onReleased: {
                    parent.color="#00CD00";
                }
                onClicked: {
                    acceptSig();
                }
            }
        }
        Rectangle{
            id:reject;
            width:60;
            height: 25;
            radius: 5;
            color:"white";

            layer.enabled: true;
            layer.effect: DropShadow{
                radius: 8;
                samples: 16;
                color: "#50000000";
            }

            Text {
                anchors.centerIn: parent;
                text:"取消";
                font.pixelSize: 15;
            }
            MouseArea{
                anchors.fill:parent;
                onPressed: {
                    parent.color="#F5F5F5";
                }
                onReleased: {
                    parent.color="white";
                }
                onClicked: {
                    rejectSig();
                }
            }
        }
    }
}
