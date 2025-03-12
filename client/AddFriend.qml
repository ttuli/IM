import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Rectangle{
    id:window;
    color: "#fafafa"

    property int btnWidth: 30;

    signal closeSig();
    Rectangle{
        id:closebtn;
        width: btnWidth;
        height: width;
        anchors.right: parent.right;
        anchors.top: parent.top;
        color:"transparent";
        z:1;

        Image {
            anchors.fill: parent;
            anchors.margins: 5;
            source: "/new/prefix1/res/XMark.svg";
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
            onPressed: {
                parent.color="#B22222";
            }
            onReleased: {
                parent.color="red";
            }
            onClicked: {
                closeSig();
            }
        }
        Behavior on color {
            ColorAnimation {
                duration: 80
            }
        }
    }//关闭

    signal minSig();
    Rectangle{
        id:minbtn;
        height: width;
        width: btnWidth;
        anchors.right: closebtn.left;
        color:"transparent";
        z:1;

        Image {
            source: "/new/prefix1/res/Minus.svg";
            anchors.fill: parent;
            anchors.margins: 5;
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

    property string searchMode: "person"

    Rectangle{
        id:btnarea;
        width: window.width;
        height: 180;

        gradient:Gradient{
            GradientStop{position: 0;color: "#56ccf2"}
            GradientStop{position: 1;color:"#2f80ed"}
        }


        Row{
            Layout.alignment: Qt.AlignHCenter;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: parent.top;
            anchors.topMargin: 50;
            spacing: 80;

            Rectangle{
                id:findpersonbtn;
                color:"transparent";
                width: findpersonbtn_text.implicitWidth;
                height: findpersonbtn_text.implicitHeight;

                Text {
                    id: findpersonbtn_text;
                    anchors.centerIn: parent;
                    text: qsTr("找人");
                    color: "white";
                    font.bold: true;
                    font.pixelSize: 35;
                }

                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        parent.scale=1;
                        findgroupbtn.scale=0.8;
                        searchMode="person";
                    }
                }
            }

            Rectangle{
                id:findgroupbtn;
                color:"transparent";
                width: findgroupbtn_text.implicitWidth;
                height: findgroupbtn_text.implicitHeight;
                scale: 0.8;

                Text {
                    id: findgroupbtn_text;
                    anchors.centerIn: parent;
                    text: qsTr("找群");
                    color: "white";
                    font.bold: true;
                    font.pixelSize: 35;
                }
                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        parent.scale=1;
                        findpersonbtn.scale=0.8;
                        searchMode="group";
                    }
                }
            }
        }

        Rectangle{
            id:input_rec;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.bottom: parent.bottom;
            anchors.bottomMargin: 10;
            color: "#00BFFF";
            radius: 20;
            width: 400;
            height: 50;
            clip: true;

            TextField{
                id:input;
                anchors.fill: parent;
                anchors.rightMargin: 50;
                background: Rectangle{
                    color:"white";
                    topLeftRadius: 20;
                    bottomLeftRadius: 20;
                }
                font.pixelSize: 20;
                font.bold: true;
            }

            Image {
                source: "/new/prefix1/res/search.svg";
                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.rightMargin: 10;
                width: 35;
                height: 35;
                property bool canClick: true

                Timer {
                    id: clickTimer
                    interval: 1000
                    onTriggered: parent.canClick=true;
                }

                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        if(parent.canClick){
                            checkContent();
                            parent.canClick=false;
                            clickTimer.start();
                        }
                    }
                }
            }

        }
    }

    signal searchSig(string mode,int pattern,string content);

    function checkContent()
    {
        var rule1=/^[0-9]+$/;
        setFinishVisible(true);

        //0代表按账号，1代表按名字
        if(rule1.test(input.text)){
            searchSig(searchMode,0,input.text);
        }else{
            searchSig(searchMode,1,input.text);
        }
    }

    property int cellwidth: width/3;
    property int cellheight: 90;

    signal addSig(string target,int id);
    Rectangle {
        id: resultsContainer
        anchors.top: btnarea.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        color: "#F5F5F5"
        border.color: "#eeeeee"
        radius: 4

        GridView{
            id:gridview;
            anchors.fill: parent;


            cellWidth:cellwidth;
            cellHeight:cellheight;

            model:searchModel;

            delegate: Rectangle{
                width: cellwidth;
                height: cellheight;

                color: "transparent";
                Rectangle{
                    anchors.fill: parent;
                    anchors.margins: 8;
                    radius: 8;
                    color:"white";

                    Image {
                        id: circle_ima;
                        source: "/new/prefix1/res/circle.png";
                        visible: false;
                    }
                    Image {
                        id: userimage;
                        source: "image://imageprovider/"+account;
                        visible: false;
                    }

                    OpacityMask{
                        id:headimage;
                        source:userimage;
                        maskSource: circle_ima;
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        width: 50;
                        height: 50;
                    }

                    Text {
                        id: username;
                        text: name;
                        font.bold: true;
                        font.pixelSize: 20;
                        anchors.left: headimage.right;
                        anchors.right: parent.right;
                        anchors.top:parent.top;
                        anchors.topMargin: 8;
                        elide: Text.ElideRight;
                        anchors.leftMargin: 10;
                    }

                    Rectangle{
                        id:addbtn;
                        color: applystate?"transparent":"#00CD66";
                        width: 50;
                        height: 30;
                        radius: 5;
                        anchors.right: parent.right;
                        anchors.bottom: parent.bottom;
                        anchors.margins: 5;

                        Text {
                            anchors.centerIn: parent;
                            text: applystate?"√已申请":"+添加";
                            color: applystate?"#00CD66":"white";
                            font.bold: true;
                            font.pixelSize: 15;
                        }
                        MouseArea{
                            anchors.fill: parent;
                            cursorShape: applystate?Qt.ArrowCursor:Qt.PointingHandCursor;
                            onClicked: {
                                if(applystate)
                                    return;
                                addSig(account,index);
                                setFinishVisible(true);
                            }
                        }
                    }
                }
            }
        }
    }

    function setFinishVisible(value)
    {
        finish_rec.visible=value;
    }

    function setMsg(text,value)
    {
        messagePopup.showMessage(text,value);
    }

    Rectangle{
        id:finish_rec;
        anchors.fill: parent;
        color: "#50000000";
        visible: false;

        MouseArea{
            anchors.fill: parent;
            acceptedButtons: Qt.NoButton;
        }

        onVisibleChanged: {
            if(visible===false)
                finish_caption_rec.scale=0.7;
            else
                finish_caption_rec.scale=1;
        }

        Rectangle{
            id:finish_caption_rec;
            anchors.centerIn: parent;
            width: 210;
            height: 50;
            color: "#F5F5F5";
            radius: 20;
            scale:0.7;

            Behavior on scale {
                NumberAnimation{
                    duration: 80;
                }
            }

            Image {
                id: finish_image;
                source: "/new/prefix1/res/loading.png";
                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;
                width: 35;
                height: 35;

                RotationAnimation on rotation {
                    from: 0;
                    to: 360;
                    duration: 1000;
                    loops: Animation.Infinite;
                    running: true;
                }
            }

            Text {
                id:finish_caption;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left:finish_image.right;
                anchors.leftMargin: 5;
                font.bold: true;
                font.pixelSize: 30;
                text: qsTr("正在执行...");
            }

        }
    }

    // 将消息弹窗组件放在底部，可全局调用显示消息
    MessagePopup {
        id: messagePopup
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

