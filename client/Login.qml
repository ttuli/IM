import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Particles
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts

Rectangle
{
    id:window;
    LinearGradient{
        anchors.fill: parent
        start:Qt.point(0,0);
        end:Qt.point(parent.width,parent.height);
        gradient: Gradient{
            GradientStop{
                position: 0.0
                color:"#1cd8d2"
            }
            GradientStop{
                position: 1
                color:"#93edc7"
            }
        }
    }

    property int btnLength: 25;
    property int minAcloInterval: 5;
    property int minAcloLength: btnLength*2+minAcloInterval;


    signal closeSig();
    signal minSig();

    function setInfo(acc,pwd,remberPwd,autoLogin,doChange)
    {
        acc_input.text=acc;
        pwd_input.text=pwd;
        if(!doChange)
            return;

        remeber_pwd.checked=remberPwd;
        auto_login.checked=autoLogin;
        if(autoLogin){
            login_btn.readyToLogin();
        }
    }

    function showCaption(type,text,duration)
    {
        loading=false;
        //login_btn.readyLoading=false;

        errorPopup_ima.infotype=type;
        pauseanimation.duration=duration;
        errorPopup_text.text=text;

        if(type!==0)
            login_text.text="登录";
        if(popupAnimation.running)
            popupAnimation.stop();
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
            property int infotype: 0;
            source: {
                if(infotype===0){
                    //loading
                    return "/new/prefix1/res/loading.png";
                } else if (infotype===1) {
                    //success
                    return "/new/prefix1/res/success.svg";
                } else if(infotype===2) {
                    //fail
                    return "/new/prefix1/res/wrong.svg";
                }
            }

            width: 25;
            height: width;

            anchors.left: parent.left;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;
            rotation:(infotype!==0?0:0);

            RotationAnimation on rotation {
                from: 0;
                to: 360;
                duration: 1000;
                loops: Animation.Infinite;
                running: errorPopup_ima.infotype===0;
            }
        }

        Text {
            id:errorPopup_text;
            text: "";
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
                id:pauseanimation;
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
        id:caption;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 60;
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

    Rectangle{
        id:closebtn;
        width: btnLength;
        height: btnLength;
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
        id:minbtn;
        width: btnLength;
        height: btnLength;
        anchors.right: closebtn.left;
        anchors.rightMargin: minAcloInterval;
        anchors.top: parent.top;
        color:"transparent";

        objectName: "minbtn";

        Image {
            anchors.fill: parent;
            source: "/new/prefix1/res/Minus.svg";
        }
        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                parent.color="#F5F5F5";
            }
            onExited: {
                parent.color="transparent";
            }
            onPressed: {
                parent.color="#DCDCDC";
            }
            onReleased: {
                parent.color="#F5F5F5";
            }
            onClicked: {
                minSig();
            }
        }
        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }
    }//最小化

    ParticleSystem{
        id:parsystem
    }

    Emitter{
        id:emitter;
        anchors.fill: parent;
        system: parsystem;
        emitRate: 3;
        lifeSpan: 2000;
        lifeSpanVariation: 200;
        size: 30

        sizeVariation:80
        velocity: PointDirection{
            y:-100
            x:5
        }
    }

    ImageParticle{
        system: parsystem
        id:ima
        opacity: 0.5
        source:"/new/prefix1/res/small_cube.png"
        rotation: 720
        rotationVariation: 5
        rotationVelocity: 45
        rotationVelocityVariation: 15
    }


    property int inputBoxWidth: 250;
    property int inputBoxHeight: 50;
    property int loginBoxWidth: 220;



    Rectangle{
        id:acc_input_rec;
        color:"white";
        width: inputBoxWidth;
        height: inputBoxHeight;
        anchors.top: caption.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.topMargin: 60;
        border.color: "red";
        border.width: 0;
        radius:8;

        layer.enabled: true;
        layer.effect: DropShadow{
            radius: acc_input_rec.radius;
            samples: 16;
        }

        TextInput{
            id:acc_input;
            anchors.fill: parent;
            anchors.leftMargin: 5;
            anchors.rightMargin: 5;
            verticalAlignment: TextInput.AlignVCenter;
            font.bold: true;
            font.pixelSize: 25;
            clip: true;

            validator: RegularExpressionValidator {regularExpression: /^[0-9]*$/}

            PlaceholderText{
                id:acc_holdertext;
                text: "账号";
                anchors.left: parent.left;
                width: 80;
                height: parent.height;
                font.pixelSize: 25;
                color: "grey";
                verticalAlignment:PlaceholderText.AlignVCenter;
                visible: acc_input.length===0;
            }

            Keys.onReturnPressed: {
                pwd_input.focus=true;
            }
        }
    }

    Rectangle{
        id:pwd_input_rec;
        color:"white";
        width: inputBoxWidth;
        height: inputBoxHeight;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: acc_input_rec.bottom;
        anchors.topMargin: 10;
        border.color: "red";
        border.width: 0;
        radius: 8;

        layer.enabled: true;
        layer.effect: DropShadow{
            radius: pwd_input_rec.radius;
            samples: 16;
        }

        TextInput{
            id:pwd_input;
            anchors.fill: parent;
            anchors.leftMargin: 5;
            anchors.rightMargin: 5;
            verticalAlignment: TextInput.AlignVCenter;
            font.bold: true;
            font.pixelSize: 25;
            echoMode: TextInput.Password;
            clip: true;

            validator: RegularExpressionValidator {regularExpression:  /^[a-zA-Z0-9_]*$/}

            PlaceholderText{
                id:pwd_holdertext;
                text: "密码";
                anchors.left: parent.left;
                width: 80;
                height: parent.height;
                font.pixelSize: 25;
                color: "grey";
                verticalAlignment:PlaceholderText.AlignVCenter;
                visible: pwd_input.length===0;
            }

            Keys.onReturnPressed: {
                login_btn.readyToLogin();
            }
        }
    }


    signal loginSig(string account,string password,bool rememberPwd,bool autoLogin);
    property bool loading: false;

    function setError(isAcc,isPwd)
    {
        if(!isAcc)
            acc_input_rec.border.width=2;
        else
            acc_input_rec.border.width=0;

        if(!isPwd)
            pwd_input_rec.border.width=2;
        else
            pwd_input_rec.border.width=0;
    }

    CheckBox{
        id:remeber_pwd;
        anchors.top: pwd_input_rec.bottom;
        anchors.left: pwd_input_rec.left;
        anchors.topMargin: 10;
        text: "记住密码";
        font.pixelSize: 15;
        indicator: Rectangle {
            x: remeber_pwd.leftPadding
            anchors.verticalCenter: parent.verticalCenter
            width: 26; height: width
            antialiasing: true
            radius: 5
            border.width: 2
            border.color: "#0ACF97";

            Rectangle {
                anchors.centerIn: parent
                width: parent.width*0.7; height: width
                antialiasing: true
                radius: parent.radius * 0.7
                color: "#0ACF97"
                visible: remeber_pwd.checked
            }
        }
    }
    CheckBox{
        id:auto_login;
        anchors.top: pwd_input_rec.bottom;
        anchors.right: pwd_input_rec.right;
        anchors.topMargin: 10;
        text: "自动登录";
        font.pixelSize: 15;
        indicator: Rectangle {
            x: auto_login.leftPadding
            anchors.verticalCenter: parent.verticalCenter
            width: 26; height: width
            antialiasing: true
            radius: 5
            border.width: 2
            border.color: "#0ACF97";

            Rectangle {
                anchors.centerIn: parent
                width: parent.width*0.7; height: width
                antialiasing: true
                radius: parent.radius * 0.7
                color: "#0ACF97"
                visible: auto_login.checked
            }
        }
    }

    Rectangle{
        id:login_btn;
        anchors.bottom: parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottomMargin: 90;
        width: loginBoxWidth;
        height: inputBoxHeight;
        radius: 8;

        property bool readyLoading: false;

        color: loading?"grey":"white";

        Behavior on color {
            ColorAnimation {
                duration: 100;
            }
        }

        layer.enabled: true;
        layer.effect: DropShadow{
            radius: login_btn.radius;
            samples: 16;
        }

        Text {
            id:login_text;
            text: "登录";
            font.bold: true;
            font.pixelSize: 25;
            anchors.centerIn: parent;
        }

        Behavior on scale {
            NumberAnimation{
                duration: 80;
            }
        }

        MouseArea{
            anchors.fill:parent;
            cursorShape: loading?Qt.ForbiddenCursor:Qt.PointingHandCursor;
            onPressed: {
                if(!loading)
                    parent.scale=0.9;
            }
            onReleased: {
                if(!loading)
                    parent.scale=1;
            }
            onClicked: {
                if(parent.readyLoading){
                    loginCountDown.stop();
                    login_text.text="登录";
                    showCaption(1,"已取消",1000);
                    login_btn.readyLoading=false;
                }
                else {
                    parent.readyToLogin();
                }
            }
        }

        property var regex10Digits: /^\d{10}$/; // 10 位数字
        property var regex6To15Digits: /^[a-zA-Z0-9_]{6,15}$/;  // 6 到 18 位数字

        Timer{
            id:loginCountDown;
            interval: 1200;
            onTriggered: {
                loading=true;
                login_btn.readyLoading=false;
                parent.checkInfo();
            }
        }

        function readyToLogin()
        {
            if(loading)
                return;
            login_btn.readyLoading=true;
            login_text.text="取消";
            showCaption(0,"登录中...",10000);
            loginCountDown.start();
        }

        function checkInfo()
        {
            var acc=acc_input.text;
            var pwd=pwd_input.text;

            var acc_res=regex10Digits.test(acc);
            var pwd_res=regex6To15Digits.test(pwd);

            if(acc_res&&pwd_res){
                loginSig(acc,pwd,remeber_pwd.checked,auto_login.checked);
                acc_input_rec.border.width=0;
                pwd_input_rec.border.width=0;
                return;
            }

            loading=false;
            login_text.text="登录";
            setError(acc_res,pwd_res);
        }

    }

    signal registerSig();

    Rectangle{
        id:registerbtn;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        width: 50;
        height: 30;
        color: "transparent";

        Text {
            text: qsTr("注册账号");
            color:"blue";
            font.pixelSize: 15;
            anchors.centerIn: parent;

        }

        MouseArea{
            anchors.fill: parent;
            cursorShape: (login_btn.readyLoading||loading)?Qt.ForbiddenCursor:Qt.PointingHandCursor;

            onClicked: {
                if(!login_btn.readyLoading&&!loading)
                    registerSig();
            }
        }
    }

}
