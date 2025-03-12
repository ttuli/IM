import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{
    id:window;
    property string color_: "#f9f7f7";

    signal talkRequestSig(string account,string name);
    signal clearSig(int position);

    function setIndex(value)
    {
        listview.current=value;
    }

    color:color_;
    ListView{
        id:listview;
        anchors.fill: parent;
        model:chatlistModel;
        currentIndex:-1;
        property string current: "";

        delegate: Rectangle{
            id:outline;
            width: listview.width;
            height: 70;
            color: "transparent";

            Rectangle{
                anchors.fill:parent;
                color: "#33000000";
                visible: listview.current===account;
            }
            Rectangle{
                id:hoverColorRec;
                anchors.fill:parent;
                color: "#20000000";
                visible: false;
            }

            Rectangle{
                width: parent.width;
                height: 1;
                anchors.bottom: parent.bottom;
                color: "#20000000"
            }

            Image {
                id: outline_ima;
                source: "/new/prefix1/res/circle.png";
                visible: false;
            }
            Image {
                id: avator_ima;
                source: "image://imageprovider/"+account;
                visible: false;
            }
            OpacityMask{
                id:avator;
                maskSource: outline_ima;
                source: avator_ima;
                height: parent.height-14;
                width: height;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;
            }
            Rectangle{
                width: 20;
                height: 20;
                radius: width/2;
                color: "red";
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.margins: 5;
                visible: unreadMsg!==0;

                Text {
                    anchors.centerIn: parent;
                    text: unreadMsg>99?"99+":unreadMsg;
                    color:"white";
                    font.pixelSize: 13;
                    font.bold: true;
                }
            }

            Text {
                id: user_name;
                text:name;
                anchors.top: avator.top;
                anchors.left: avator.right;
                anchors.leftMargin: 5;
                anchors.right: parent.right;
                anchors.rightMargin: m_timestemp.implicitWidth+5;
                elide: Text.ElideRight;
                font.bold: true;
                font.pixelSize: 18;
            }

            Text {
                id: last_msg;
                text:lastMsg;
                anchors.bottom: avator.bottom;
                anchors.left: avator.right;
                anchors.leftMargin: 5;
                width: 120;
                font.pixelSize: 18;
                textFormat: Text.RichText;
                color:"#696969";
                clip: true;
            }
            Text {
                id: elideText;
                text: qsTr("...");
                anchors.left: last_msg.right;
                anchors.leftMargin: 3;
                anchors.bottom: avator.bottom;
                visible: last_msg.implicitWidth>last_msg.width;
                font:last_msg.font;
            }

            Text {
                id: m_timestemp;
                text: timestemp;
                anchors.right: parent.right;
                anchors.rightMargin: 5;
                anchors.top:parent.top;
                anchors.topMargin: 5;
                color:"black";
                opacity: 0.5;
                font.pixelSize: 13;
            }

            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                acceptedButtons: Qt.LeftButton|Qt.RightButton;
                onClicked: function(mouse){
                    if(mouse.button === Qt.LeftButton){
                        listview.current=account;
                        talkRequestSig(account,name);
                    } else if (mouse.button === Qt.RightButton) {
                        var contextMenu = contextMenuComponent.createObject(outline, {
                            "x": mouse.x,
                            "y": mouse.y,
                            "position":index
                        });
                        contextMenu.open();
                    }
                }
                onEntered: {
                    if(listview.current!==account)
                        hoverColorRec.visible=true;
                }
                onExited: {
                    hoverColorRec.visible=false;
                }
            }
        }
    }

    Rectangle{
        height: parent.height;
        width: 1;
        color:"#33000000";
        anchors.right: parent.right;
    }

    Component{
        id:contextMenuComponent;
        CustomMenu{
            id:contextMenu;
            property int position: -1;
            MenuItem{
                text: "删除选中项";
                onTriggered: {
                    clearSig(position);
                }
            }
            MenuItem{
                text: "清空列表";
                onTriggered: {
                    clearSig(-1);
                }
            }
        }
    }
}
