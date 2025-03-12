import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs

Rectangle{
    id:window;
    color: "#f5f5f5";

    signal closeSig();

    property int btnWidth: 30;

    Component.onCompleted: {
        initFunc();
    }

    function initFunc()
    {
        image_adjust_rec.visible=false;
        executing.visible=false;
        flickable_ima.source="";
    }

    function setError(isSuccess,text)
    {
        errorPopup_ima.success=isSuccess;
        errorPopup_text.text=text;
        if(popupAnimation.running)
            popupAnimation.stop();
        popupAnimation.start();
        initFunc();
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

    Rectangle{
        id:closebtn;
        width: btnWidth;
        height: width;
        anchors.right: parent.right;
        anchors.top: parent.top;
        color:"transparent";

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

    function setInfomation(name,email,account,time)
    {
        username_text.text="用户名:"+name;
        email_text.text="邮箱:"+email;
        time_text.text="创建时间:"+time;
        avatar.source="";
        avatar.source="image://imageprovider/"+account;
        avatar.currentAccount=account;
    }


    Image {
        id: outline_ima;
        visible: false;
        source: "/new/prefix1/res/circle.png";
    }

    Image {
        id: avatar;
        property string currentAccount:"";
        cache: false;

        visible: false;
    }

    Row {
        anchors.top: parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.topMargin: 50;
        spacing: 20

        // 用户头像
        OpacityMask{
            width: 80;
            height: 80;
            smooth: true;
            layer.enabled: true;
            layer.effect: DropShadow{
                radius: 8
                samples: 16
                color: "#50000000"
            }
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: "#ccc"
                border.width: 2
                radius: width / 2
            }
            source: avatar;
            maskSource: outline_ima;
        }

        // 用户信息
        Column {
            spacing: 5

            Text {
                id:username_text;
                font.bold: true
                font.pixelSize: 18
                color: "#333"
            }

            Text {
                id:email_text;
                font.bold: true
                font.pixelSize: 18
                color: "#666"
            }

            Text {
                id:time_text;
                font.bold: true
                font.pixelSize: 18
                color: "#666"
            }
        }

    }

    Rectangle{
        id:openFileBtn;
        anchors.bottom: flickable.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottomMargin: 100;
        radius: 8;
        width: openFileBtn_image.width+openFileBtn_text.width+25;
        height: 50;
        color:"white";
        border.color: "#F5F5F5";
        border.width: 1;
        visible: !image_adjust_rec.visible;

        layer.effect: DropShadow{
            radius: 8;
            samples: 16
            color: "#50000000"
        }

        Image {
            id: openFileBtn_image;
            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            height: parent.height-10;
            width: height;
            source: "/new/prefix1/res/Folder.svg";
        }

        Text {
            id:openFileBtn_text;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: openFileBtn_image.right;
            anchors.leftMargin: 5;
            text: qsTr("修改头像");
            font.bold: true;
            font.pixelSize: 25;
        }

        MouseArea{
            anchors.fill: parent;
            onPressed: {
                parent.color="#F5F5F5";
            }
            onReleased: {
                parent.color="white";
            }
            onClicked: {
                fileDialog.open();
            }
        }
    }

    Rectangle{
        id:image_adjust_rec;
        anchors.bottom: flickable.top;
        anchors.bottomMargin: 20;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: 290;
        height: 80;
        visible: false;

        color: "transparent";

        Rectangle{
            id:confirm_btn;
            height: 35;
            width: 90;
            radius: 10;
            color: "#00EE00"
            layer.enabled: true;
            layer.effect: DropShadow{
                radius: 8;
                samples: 16
                color: "#90000000"
            }

            Image {
                id: confirm_btn_ima;
                anchors.verticalCenter: parent.verticalCenter;
                height: parent.height-10;
                width: height;
                smooth: true;
                source: "/new/prefix1/res/hollowCircle.svg";
            }

            Text {
                id: confirm_btn_text;
                anchors.left: confirm_btn_ima.right;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.leftMargin: 5;
                text: qsTr("确定");
                font.bold: true;
                font.pixelSize: 25;
                color:"white";
            }

            MouseArea{
                anchors.fill: parent;
                onPressed: {
                    parent.color="#008B45";
                }
                onReleased: {
                    parent.color="#00EE00";
                }
                onClicked: {
                    getImageData();
                }
            }
        }

        Rectangle{
            id:cancel_btn;
            width: 90;
            height: 35;
            radius: 10;
            color: "red";
            anchors.right: parent.right;
            layer.enabled: true;
            layer.effect: DropShadow{
                radius: 8;
                samples: 16
                color: "#90000000"
            }

            Image {
                id: cancel_btn_ima;
                anchors.verticalCenter: parent.verticalCenter;
                height: parent.height-10;
                width: height;
                smooth: true;
                source: "/new/prefix1/res/XMark.svg";
            }

            Text {
                id: cancel_btn_text;
                anchors.left: cancel_btn_ima.right;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.leftMargin: 5;
                text: qsTr("取消");
                font.bold: true;
                font.pixelSize: 25;
                color:"white";
            }

            MouseArea{
                anchors.fill: parent;
                onPressed: {
                    parent.color="#CD2626";
                }
                onReleased: {
                    parent.color="red";
                }
                onClicked: {
                    initFunc();
                }
            }
        }

        Text {
            anchors.right: control.left;
            anchors.rightMargin: 5;
            anchors.verticalCenter: control.verticalCenter;
            text: qsTr("调整大小");
            font.bold: true;
            font.pixelSize: 20;
        }

        Slider
        {
            id: control;
            property int iniwidth: 0;
            property int iniheight: 0;
            width: 200;
            height: 30;
            from: 0;
            to:10;
            stepSize:1;
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;
            value: 0;
            snapMode:Slider.SnapAlways;
            background:
            Rectangle
            {
                    x: control.leftPadding;
                    y: control.topPadding + control.availableHeight / 2 - height / 2
                    implicitWidth: 100
                    implicitHeight: 4
                    width: control.availableWidth
                    height: implicitHeight
                    radius: 2
                    color: "#bdbebf"

                    Rectangle
                    {
                        width: control.visualPosition * parent.width
                        height: parent.height
                        color: "#21be2b"
                        radius: 2
                    }
            }

            handle: Rectangle
            {
                x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
                y: control.topPadding + control.availableHeight / 2 - height / 2
                implicitWidth: 26
                implicitHeight: 26
                radius: 13
                color: control.pressed ? "#f0f0f0" : "#f6f6f6"
                border.color: "#bdbebf"
            }

            onValueChanged:
            {
                flickable_ima.width=flickable_ima.initWidth+control.value*20;
                flickable_ima.height=flickable_ima.initHeight+control.value*20*flickable_ima.initHeight/flickable_ima.initWidth;
            }
        }
    }

    FileDialog {
            id: fileDialog
            title: "选择文件";
            fileMode: FileDialog.OpenFile;
            nameFilters: ["image files (*.jpg *.png *.jpeg)"];

            onAccepted: {
                control.value=0;
                flickable_ima.source=fileDialog.selectedFile;
                image_adjust_rec.visible=true;
            }
        }

    Flickable
    {
        id:flickable;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: parent.width-50;
        height: width;
        objectName: "flickable";

        clip: true;
        contentHeight: flickable_ima.height;
        contentWidth: flickable_ima.width;

        boundsBehavior: Flickable.StopAtBounds;

        Image {
            id: flickable_ima;
            property int initWidth: 0;
            property int initHeight: 0;
            source: "";

            onStatusChanged: {
                if (status === Image.Ready) {
                    if(implicitWidth<implicitHeight){
                        width=flickable.width;
                        var scalex=flickable.width/implicitWidth;
                        height=scalex*implicitHeight;
                    } else {
                        height=flickable.height;
                        var scaley=flickable.height/implicitHeight;
                        width=scaley*implicitWidth;
                    }
                    initHeight=height;
                    initWidth=width;
                }
            }
        }

        MouseArea
        {
            anchors.fill: parent;
            cursorShape: Qt.SizeAllCursor;
        }

    }

    function getImageData()
    {
        executing.visible=true;
        flickable.grabToImage(function(result) {
            if (result) {
                InfoDialog.updateUserImage(result);
            }
        });
    }

    ExecutingModal{
        id:executing;
        anchors.fill: parent;
    }
}
