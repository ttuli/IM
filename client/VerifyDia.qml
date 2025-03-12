import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle{

    signal s_reactSig(int index,string account,bool result);
    Rectangle{
        anchors.fill:parent
        anchors.margins: 10;

        ListView{
            id:listview;
            anchors.fill:parent;
            model:applyModel;

            interactive: false;
            spacing: 10;
            clip: true;
            boundsBehavior: Flickable.StopAtBounds;

            Rectangle{
                id:null_rec;
                anchors.fill: parent;
                visible:true;

                Text {
                    text: qsTr("无验证消息");
                    font.bold: true;
                    font.pixelSize: 35;
                    anchors.centerIn: parent;
                    opacity: 0.5;
                }
            }

            delegate: Rectangle{
                width: listview.width;
                height: (msgstate!=="time"?80:35);
                border.color: "#F5F5F5";
                border.width: 1;

                Component.onCompleted: {
                    null_rec.visible=false;
                }

                Text {
                    text: msgtime;
                    font.pixelSize: 25;
                    anchors.bottom: parent.bottom;
                    font.bold: true;
                    visible: (msgstate==="time")
                }

                Image {
                    id:uima;
                    source: (msgstate==="time"?"":"image://imageprovider/"+account);
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
                    height: parent.height-5;
                    width: height;
                    maskSource: outlineima;
                    source: uima;
                }

                Text {
                    id: name_text;
                    text: name;
                    font.bold: true;
                    font.pixelSize: 20;
                    anchors.left: image_.right;
                    anchors.leftMargin: 5;
                    anchors.right: parent.right;
                    anchors.rightMargin: 5;
                    elide: Text.ElideRight;
                    anchors.top: parent.top;
                    anchors.topMargin: 5;
                }

                Text {
                    text: qsTr("好友申请");
                    opacity: 0.5;
                    font.pixelSize: 20;
                    font.bold: true;
                    anchors.left: image_.right;
                    anchors.leftMargin: 5;
                    anchors.bottom: parent.bottom;
                    anchors.bottomMargin: 5;
                    visible: (msgstate!=="time")
                }

                Rectangle{
                    id:state_rec;
                    anchors.right: parent.right;
                    anchors.rightMargin: 5;
                    anchors.verticalCenter: parent.verticalCenter;
                    width: 180;
                    height: 35;


                    Text {
                        id: request_text;
                        text: qsTr("已发送好友申请");
                        anchors.centerIn: parent;
                        font.pixelSize: 20;
                        font.bold: true;
                        opacity: 0.5;
                        visible: (msgstate==="request")
                    }

                    Text {
                        id: accept_text;
                        text: qsTr("已同意");
                        anchors.centerIn: parent;
                        font.pixelSize: 20;
                        font.bold: true;
                        opacity: 0.5;
                        visible: (msgstate==="accept");

                    }

                    Text {
                        id: reject_text;
                        text: qsTr("已拒绝");
                        anchors.centerIn: parent;
                        font.pixelSize: 20;
                        font.bold: true;
                        opacity: 0.5;
                        visible: (msgstate==="reject");
                    }

                    Rectangle{
                        id:verify_rec;
                        anchors.fill: parent;
                        property int btnWidth: 60;
                        property int btnheight: 30;
                        visible: (msgstate==="verify");

                        Rectangle{
                            id:accept_btn;
                            anchors.verticalCenter: parent.verticalCenter;
                            anchors.left:parent.left;
                            anchors.leftMargin: 12;
                            border.color: "#DCDCDC";
                            border.width: 1;
                            color: "#00CD00";
                            radius: 8;
                            width: accept_btn_text.implicitWidth+30;
                            height: accept_btn_text.implicitHeight+5;

                            layer.effect: DropShadow{
                                radius:accept_btn.radius;
                                samples: 16;
                                color:"#DCDCDC";
                            }

                            Text {
                                id:accept_btn_text;
                                anchors.centerIn: parent;
                                text: qsTr("同意");
                                font.bold: true;
                                font.pixelSize: 21;
                                color:"white"
                            }
                            MouseArea{
                                anchors.fill: parent;

                                onPressed: {
                                    parent.color="#2E8B57";
                                }
                                onReleased: {
                                    parent.color="#00CD00";
                                }
                                onClicked: {
                                    if(name_text.text==="")
                                        return;
                                    s_reactSig(index,account,true);
                                    finish_rec.visible=true;
                                }
                            }
                        }

                        Rectangle{
                            id:reject_btn;
                            anchors.right: parent.right;
                            anchors.rightMargin: 12;
                            anchors.verticalCenter: parent.verticalCenter;
                            border.color: "#DCDCDC";
                            border.width: 1;
                            color:"#FF3030";
                            radius: 8;
                            width: reject_btn_text.implicitWidth+30;
                            height: reject_btn_text.implicitHeight+5;

                            layer.effect: DropShadow{
                                radius:reject_btn.radius;
                                samples: 16;
                                color:"#DCDCDC";
                            }

                            Text {
                                id:reject_btn_text;
                                anchors.centerIn: parent;
                                text: qsTr("拒绝");
                                font.bold: true;
                                font.pixelSize: 21;
                                color:"white";
                            }
                            MouseArea{
                                anchors.fill: parent;

                                onPressed: {
                                    parent.color="#B22222";
                                }
                                onReleased: {
                                    parent.color="#FF3030";
                                }
                                onClicked: {
                                    if(name_text.text==="")
                                        return;
                                    s_reactSig(index,account,false);
                                    finish_rec.visible=true;
                                }
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

    function showMsg(text,isSuccess)
    {
        errorPopup_ima.success=isSuccess;
        errorPopup_text.text=text;
        popupAnimation.start();
    }

    Rectangle{
        id:errorPopup;
        color:"white";
        width: errorPopup_ima.width+errorPopup_text.implicitWidth+20;
        height: 40;
        radius: 10;
        y:-height;
        opacity: 0;

        anchors.horizontalCenter: parent.horizontalCenter;

        Image {
            id:errorPopup_ima;
            property bool success: false;
            source: !success?"/new/prefix1/res/wrong.svg":"/new/prefix1/res/success.svg";
            width: 25;
            height: width;

            anchors.left: parent.left;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;
        }

        Text {
            id:errorPopup_text;
            text: qsTr("登录失败!");
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: errorPopup_ima.right;
            anchors.leftMargin: 5;
        }

        SequentialAnimation {
            id: popupAnimation
            running: false  // 默认不运行

            // 从小变大
            ParallelAnimation {
                PropertyAnimation {
                    target: errorPopup
                    property: "y"
                    to: 10  // 弹出后的位置
                    duration: 500
                    easing.type: Easing.OutBack  // 缓动效果
                }
                PropertyAnimation {
                    target: errorPopup
                    property: "opacity"
                    to: 1  // 完全显示
                    duration: 500
                }
                ScaleAnimator {
                    target: errorPopup
                    from: 0.5  // 初始缩放
                    to: 1  // 最终缩放
                    duration: 500
                    easing.type: Easing.OutBack  // 缓动效果
                }
            }

            // 停留 2 秒
            PauseAnimation {
                duration: 3000
            }

            // 从大变小
            ParallelAnimation {
                PropertyAnimation {
                    target: errorPopup
                    property: "y"
                    to: -errorPopup.height  // 回到界面外
                    duration: 500
                    easing.type: Easing.InBack  // 缓动效果
                }
                PropertyAnimation {
                    target: errorPopup
                    property: "opacity"
                    to: 0  // 完全消失
                    duration: 500
                }
                ScaleAnimator {
                    target: errorPopup
                    from: 1  // 初始缩放
                    to: 0.5  // 最终缩放
                    duration: 500
                    easing.type: Easing.InBack  // 缓动效果
                }
            }
        }
    }
}
