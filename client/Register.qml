import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects

Rectangle {
    id:window;

    gradient: Gradient{
        GradientStop{
            position: 0;
            color:"#ece9e6";
        }
        GradientStop{
            position: 1;
            color:"#ffffff";
        }
    }


    signal closeSig();


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
            source: "/new/prefix1/res/wrong.svg";
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
        width: 25;
        height: 25;
        anchors.right: parent.right;
        anchors.top: parent.top;
        color:"transparent";

        Image {
            anchors.fill: parent;
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


    Rectangle{
        id:caption;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 70;
        width: 100;
        height: 40;
        color:"transparent";

        Image {
            id:caption_image;
            source: "/new/prefix1/res/IM_ICON.svg";
            anchors.left: parent.left;
            height: parent.height;
            width: height;
        }

        Text {
            text: qsTr("IM");
            font.bold: true;
            font.pixelSize: 30;
            anchors.left: caption_image.right;
            anchors.leftMargin: 10;
            verticalAlignment: Text.AlignVCenter;
        }
    }

    property int textWidth: 40;
    property int inputWidth: 100;
    property int fontSize: 20;
    property int recWidth: 350;
    property int recHeight: 40;
    property int textLeftMargin: 10;
    property int minterval: 5;

    Column{
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 150;
        spacing: 10;


        Rectangle{
            id:username_rec_out;
            width: recWidth;
            height: recHeight;
            color:"transparent";
            border.color: "red";
            border.width: 0;

            Text {
                id: username_text;
                text: qsTr("用户名:");
                anchors.left: parent.left
                anchors.leftMargin: textLeftMargin;
                anchors.verticalCenter: parent.verticalCenter;
                font.bold: true;
                font.pixelSize: fontSize;

            }

            Rectangle{
                id:username_field_rec;
                anchors.right: parent.right;
                height: parent.height;
                anchors.left: username_text.right;
                anchors.leftMargin: minterval;
                radius: 8;

                border.color: "red";
                border.width: 0;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color: "grey";
                }

                TextInput{
                    id:username_field;
                    anchors.fill: parent;
                    anchors.margins:3;

                    verticalAlignment: TextInput.AlignVCenter;

                    PlaceholderText{
                        font: parent.font;
                        anchors.fill: parent;
                        color:"#33000000";
                        text: "1-12位不包含-";
                        verticalAlignment: "AlignVCenter";
                        visible: parent.length===0;
                    }

                    Keys.onReturnPressed: {
                        pwd_field.focus=true;
                    }
                    Keys.onEnterPressed: {
                        pwd_field.focus=true;
                    }

                    font.bold: true;
                    font.pixelSize: fontSize;
                    clip: true;
                }
            }
        }

        Rectangle{
            id:pwd_rec_out;
            width: recWidth;
            height: recHeight;
            color:"transparent";

            Text {
                id: pwd_text;
                text: qsTr("密码:");
                anchors.left: parent.left
                anchors.leftMargin: textLeftMargin;
                anchors.verticalCenter: parent.verticalCenter;
                font.bold: true;
                font.pixelSize: fontSize;
            }

            Rectangle{
                id:pwd_field_rec;
                anchors.right: parent.right;
                height: parent.height;
                anchors.left: pwd_text.right;
                anchors.leftMargin: minterval;
                radius: 8;

                border.color: "red";
                border.width: 0;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color: "grey";
                }

                TextInput{
                    id:pwd_field;
                    anchors.fill: parent;
                    anchors.margins:3;

                    verticalAlignment: TextInput.AlignVCenter;
                    PlaceholderText{
                        font: parent.font;
                        anchors.fill: parent;
                        color:"#33000000";
                        text: "6-15位数字,大小写字母或_";
                        verticalAlignment: "AlignVCenter";
                        visible: parent.length===0;
                    }

                    Keys.onReturnPressed: {
                        cpwd_field.focus=true;
                    }
                    Keys.onEnterPressed: {
                        cpwd_field.focus=true;
                    }

                    echoMode: "Password";

                    font.bold: true;
                    font.pixelSize: fontSize;
                    clip: true;
                }
            }
        }

        Rectangle{
            id:cpwd_rec_out;
            width: recWidth;
            height: recHeight;
            color:"transparent";


            Text {
                id: cpwd_text;
                text: qsTr("确认密码:");
                anchors.left: parent.left
                anchors.leftMargin: textLeftMargin;
                anchors.verticalCenter: parent.verticalCenter;
                font.bold: true;
                font.pixelSize: fontSize;
            }

            Rectangle{
                id:cpwd_field_rec;
                anchors.right: parent.right;
                height: parent.height;
                anchors.left: cpwd_text.right;
                anchors.leftMargin: minterval;
                radius: 8;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color: "grey";
                }

                border.color: "red";
                border.width: 0;

                TextInput{
                    id:cpwd_field;
                    anchors.fill: parent;
                    anchors.margins:3;

                    verticalAlignment: TextInput.AlignVCenter;

                    echoMode: "Password";

                    Keys.onReturnPressed: {
                        email_field.focus=true;
                    }
                    Keys.onEnterPressed: {
                        email_field.focus=true;
                    }

                    font.bold: true;
                    font.pixelSize: fontSize;
                    clip: true;
                }
            }

        }

        Rectangle{
            id:email_rec_out;
            width: recWidth;
            height: recHeight;
            color:"transparent";

            Text {
                id: email_text;
                text: qsTr("邮箱:");
                anchors.left: parent.left
                anchors.leftMargin: textLeftMargin;
                anchors.verticalCenter: parent.verticalCenter;
                font.bold: true;
                font.pixelSize: fontSize;
            }

            Rectangle{
                id:email_field_rec;
                anchors.right: parent.right;
                height: parent.height;
                anchors.left: email_text.right;
                anchors.leftMargin: minterval;
                radius: 8;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color: "grey";
                }

                border.color: "red";
                border.width: 0;

                TextInput{
                    id:email_field;
                    anchors.fill: parent;
                    anchors.margins:3;

                    PlaceholderText{
                        font: parent.font;
                        anchors.fill: parent;
                        color:"#33000000";
                        text: "可选";
                        verticalAlignment: "AlignVCenter";
                        visible: parent.length===0;
                    }

                    Keys.onReturnPressed: {
                        doRegister();
                    }
                    Keys.onEnterPressed: {
                        doRegister();
                    }

                    verticalAlignment: TextInput.AlignVCenter;

                    font.bold: true;
                    font.pixelSize: fontSize;
                    clip: true;
                }

            }
        }
    }

    Rectangle{
        id:btn;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 50;

        layer.enabled: true;
        layer.effect: DropShadow{
            radius: 8;
            samples: 16;
            color: "grey";
        }

        color:"white";
        radius: 5;

        width: 150;
        height: loading_ima.height;

        Rectangle{
            id:loading_rec;
            color: "transparent";
            height: loading_ima.height;
            width: 70;
            visible: false;

            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.leftMargin: 5;

            Image {
                id: loading_ima;
                source: "/new/prefix1/res/loading.png";
                width: 35;
                height: 35;
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
                text: qsTr("注册中...");
                anchors.left: loading_ima.right;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.leftMargin: 5;
                font.bold: true;
                font.pixelSize: fontSize;
                verticalAlignment: Text.AlignVCenter;
            }
        }

        Text {
            id: btn_text;
            text: qsTr("立即注册");
            font.bold: true;
            font.pixelSize: fontSize;
            anchors.centerIn: parent;
        }

        MouseArea{
            anchors.fill: parent;
            onClicked: {
                doRegister();
            }
            onPressed: {
                parent.scale=0.9;
            }
            onReleased: {
                parent.scale=1;
            }
        }

        Behavior on scale {
            NumberAnimation{
                duration: 80;
            }
        }
    }

    function doRegister()
    {
        if(isfinish)
            return;
        isfinish=true;
        var res1=checkname();
        var res2=checkpwd();
        var res3=checkemail();

        if(res1&&res2&&res3){
            btn_text.visible=false;
            loading_rec.visible=true;
            registerSig(username_field.text,pwd_field.text,email_field.text);
        }
    }


    Rectangle{
        id:finish_rec;
        anchors.fill: parent;
        color: "#50000000";
        visible: false;

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
        }

        Rectangle{
            id:finish_caption_rec;
            anchors.centerIn: parent;
            width: parent.width-120;
            height: 240;
            color: "#F5F5F5";
            radius: 20;
            scale:0.7;

            Behavior on scale {
                NumberAnimation{
                    duration: 80;
                }
            }

            Text {
                id:finish_caption;
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.top: parent.top;
                anchors.topMargin: 20;
                font.bold: true;
                font.pixelSize: 30;
                text: qsTr("注册成功!");
            }
            TextArea{
                id:finish_text;
                readOnly: true;
                text: "账号:1000000001";
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.top: finish_caption.bottom;
                anchors.topMargin: 20;
                font.bold: true;
                font.pixelSize: 30;
            }

            Rectangle{
                id:finish_close;
                anchors.bottom: parent.bottom;
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.bottomMargin: 30;
                width: 120;
                height: 45;
                border.color: "#50000000";
                border.width: 1;
                radius: 8;


                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color: "#90000000";
                }

                Text {
                    text: qsTr("关闭");
                    anchors.centerIn: parent;
                    font.bold: true;
                    font.pixelSize: 30;
                }

                MouseArea{
                    anchors.fill: parent;
                    onPressed: {
                        parent.scale=0.9;
                    }
                    onReleased: {
                        parent.scale=1;
                    }
                    onClicked: {
                        closeSig();
                    }
                }

                Behavior on scale {
                    NumberAnimation{
                        duration: 80;
                    }
                }
            }
        }
    }

    function init()
    {
        finish_rec.visible=false;
        pwd_field.text="";
        username_field.text="";
        cpwd_field.text="";
        email_field.text="";
    }

    function showFinish(acc)
    {
        finish_text.text="账号: "+acc;
        finish_rec.visible=true;
    }

    signal registerSig(string uname,string pwd,string email);

    function checkname()
    {
        if(username_field.length>12||username_field.text.indexOf('-')!==-1||username_field.length===0){
            username_field_rec.border.width=2;
            return false;
        }
        return true;
    }


    function checkpwd()
    {
        var regex = /^[a-zA-Z0-9_]{6,15}$/;
        if(!regex.test(pwd_field.text)||pwd_field.text!==cpwd_field.text){
            pwd_field_rec.border.width=2;
            cpwd_field_rec.border.width=2;
            return false;
        }
        return true;
    }

    function checkemail()
    {
        if(email_field.length!==0){
            var emailRegex = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
            if(!emailRegex.test(email_field.text)){
                email_field_rec.border.width=2;
                return false;
            }
        }
        return true;
    }

    function showError(text,isSuccess)
    {
        isfinish=false;
        btn_text.visible=true;
        loading_rec.visible=false;

        if(popupAnimation.running)
            popupAnimation.stop();
        errorPopup_ima.source=isSuccess?"/new/prefix1/res/success.svg":"/new/prefix1/res/wrong.svg";
        errorPopup_text.text=text;
        popupAnimation.start();
    }

    property bool isfinish: false;
}
