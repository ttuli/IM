import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Rectangle{
    id:window;
    property string color_: "#f9f7f7";
    property int maxPicWidth: 380;
    property bool multiChoiceState: false;

    color:color_;

    signal pullMoreHistorySig();
    signal showHdImage(string md5);
    signal updateChoiceState(string id,int position,bool value);
    signal deleteSig();

    function setCaption(value)
    {
        caption_text.text=value;
    }
    function setMaxPicWidth(value)
    {
        maxPicWidth=value;
    }

    Rectangle{
        id:caption_rec;
        width: parent.width;
        height: 30;
        color:color_;
        Text {
            id:caption_text;
            anchors.centerIn: parent;
            font.bold: true;
            font.pixelSize: 23;
            text:"koung";
        }
    }

    SplitView{
        anchors.bottom: parent.bottom;
        anchors.top: caption_rec.bottom;
        width: parent.width;
        orientation: Qt.Vertical;

        spacing: 0;

        handle: Rectangle{
            implicitWidth: window.width;
            implicitHeight: 2;
            color:"#33000000"
        }

        Rectangle{
            id:listview_rec;
            width: parent.width;
            SplitView.minimumHeight: parent.height/2;
            SplitView.preferredHeight: parent.height/1.2;
            color:color_;

            ListView{
                id:listview;
                anchors.fill:parent;
                model: chatDialogModel;
                clip:true;
                boundsBehavior: Flickable.StopAtBounds;
                cacheBuffer: 80;

                onCountChanged: {
                    currentIndex=count-1;
                }

                onAtYBeginningChanged: {
                    if (listview.atYBeginning) {
                        pullMoreHistorySig();
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    id: vScrollBar
                    active: false
                    policy: ScrollBar.AsNeeded
                }

                delegate:Rectangle{
                    width: listview.width;
                    height: {
                        if(type===0||type===2)
                            return msg_rec.height+name_text.height+10;
                        else if(type===1)
                            return time_rec.height+15;
                    }
                    color:multiChoiceBtn.checked&&multiChoiceState?"#20000000":"transparent";

                    Rectangle{
                        id:multiChoiceBtnRec;
                        visible: multiChoiceState&&type!==1;
                        height: parent.height;
                        width: 40;
                        color:"transparent";
                        Rectangle{
                            id:multiChoiceBtn;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            anchors.top: parent.top;
                            anchors.topMargin: 5;

                            property bool checked: false;
                            onCheckedChanged: {
                                if(type!==1){
                                    updateChoiceState(msgid,index,checked);
                                }
                            }

                            width: 20;
                            height: width;
                            radius: width/2;
                            smooth: true;
                            color: checked&&multiChoiceState?"#00EE76":"transparent";
                            border.color: "gray";
                            border.width: checked?0:1;
                            Image {
                                anchors.fill: parent;
                                anchors.margins: 2;
                                source: "/new/prefix1/res/check.svg";
                                visible: parent.checked;
                            }
                        }
                        MouseArea{
                            anchors.fill: parent;
                            onClicked: {
                                if(multiChoiceState)
                                    multiChoiceBtn.checked=!multiChoiceBtn.checked;
                            }
                        }
                    }

                    Rectangle{
                        anchors.fill: parent;
                        color:"transparent";
                        anchors.leftMargin: multiChoiceState?multiChoiceBtnRec.width:0;
                        MouseArea{
                            anchors.fill: parent;
                            onClicked: {
                                if(multiChoiceState&&type!==1)
                                    multiChoiceBtn.checked=!multiChoiceBtn.checked;
                            }
                        }

                        Image {
                            id: avator_source;
                            cache: (type===0&&way===0)?false:true;
                            source: type===0||type===2?"image://imageprovider/"+account:"";

                            visible: false;
                        }
                        Image {
                            id: avator_mask;
                            source: "/new/prefix1/res/circle.png";
                            visible:false;
                        }

                        OpacityMask{
                            id: avator;
                            width: 50;
                            height: width;
                            visible: type===0||type===2;
                            anchors.margins: 5;
                            source: avator_source;
                            maskSource: avator_mask;

                            Component.onCompleted: {
                                if(way===0){
                                    anchors.right=parent.right;
                                } else {
                                    anchors.left=parent.left;
                                }
                            }
                        }

                        Text {
                            id: name_text;
                            text: name;
                            anchors.top:avator.top;
                            anchors.leftMargin: 5;
                            anchors.rightMargin: 5;
                            visible: type===0||type===2;
                            Component.onCompleted: {
                                if(way===0){
                                    anchors.right=avator.left;
                                } else {
                                    anchors.left=avator.right;
                                }
                            }
                            font.pixelSize: 10;
                            opacity: 0.7;
                        }

                        Rectangle{
                            id:time_rec;
                            anchors.bottom: parent.bottom;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            color:"#30000000";
                            width: time_text.implicitWidth+20;
                            height: time_text.implicitHeight+10;
                            Text {
                                id:time_text;
                                text: createtime;
                                anchors.centerIn: parent;
                                color: "white";
                                font.pixelSize: 15;
                            }
                            visible:(type===1);
                        }

                        Rectangle{
                            id:withdraw_rec;
                            anchors.bottom: parent.bottom;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            color:"transparent";
                            width: withdraw_text.implicitWidth+20;
                            height: withdraw_text.implicitHeight+10;
                            Text {
                                id:withdraw_text;
                                text: content;
                                anchors.centerIn: parent;
                                color: "white";
                                font.pixelSize: 13;
                            }
                            visible:(type===3);
                        }

                        Rectangle{
                            id:msg_rec;
                            radius: 8;
                            color:{
                                if(type===2)
                                    return "transparent";
                                else
                                    return way===0?"#3eede7":"white";
                            }

                            property int maxWidth: listview.width-120;

                            width: {
                                if(type!==2)
                                    return textInLine.implicitWidth+5 < maxWidth ? textInLine.implicitWidth+5 : maxWidth;
                                else
                                    return picMSg.implicitWidth>maxPicWidth?maxPicWidth:picMSg.implicitWidth;
                            }
                            height: {
                                if(type!==2)
                                    return textInLine.implicitWidth+5 < maxWidth ? textInLine.implicitHeight : textarea.implicitHeight;
                                else{
                                    var proportion=picMSg.implicitWidth/maxPicWidth;
                                    return picMSg.implicitWidth>maxPicWidth?picMSg.implicitHeight/proportion:picMSg.implicitHeight;
                                }

                            }
                            anchors.top:name_text.bottom;
                            anchors.topMargin: 5;
                            visible: type===0||type===2;

                            Component.onCompleted: {
                                if(way===0){
                                    anchors.right=name_text.right;
                                } else {
                                    anchors.left=name_text.left;
                                }
                            }

                            Image {
                                id: picMSg;
                                anchors.fill: parent;
                                source: type===2?"image://"+"generalProvider/"+content:"";
                                cache: false;
                                visible: false;
                            }
                            OpacityMask{
                                id:opacitymask;
                                source: picMSg;
                                maskSource: picMsgOutline;
                                anchors.fill: parent;
                                visible: type===2;
                                MouseArea{
                                    anchors.fill: parent;
                                    acceptedButtons: Qt.RightButton|Qt.LeftButton;
                                    onPressed: {
                                        if(!multiChoiceState)
                                            backrec.visible=true;
                                    }
                                    onReleased: {
                                        backrec.visible=false;
                                    }
                                    onClicked: function(mouse) {
                                        if (mouse.button === Qt.RightButton) {
                                            var contextMenu = imageMenuComponent.createObject(opacitymask, {
                                                "x": mouse.x,
                                                "y": mouse.y,
                                              "msgId":msgid,
                                              "position":index
                                            })
                                            contextMenu.open();
                                        } else if (mouse.button===Qt.LeftButton){
                                            if(multiChoiceState){
                                                multiChoiceBtn.checked=!multiChoiceBtn.checked;
                                            } else {
                                                showHdImage(content);
                                            }
                                        }
                                    }
                                }
                            }

                            Rectangle{
                                id:picMsgOutline;
                                anchors.fill: parent;
                                visible: false;
                                radius: msg_rec.radius;
                            }

                            Rectangle{
                                id:backrec;
                                color: "#33000000";
                                anchors.fill: parent;
                                visible: false;
                                radius: parent.radius;
                            }


                            TextArea{
                                id:textarea;
                                anchors.fill:parent;
                                visible: type===0;
                                readOnly: true;
                                textFormat: "RichText";
                                Component.onCompleted: {
                                    if(textInLine.implicitWidth+5 <= msg_rec.maxWidth){
                                        textarea.wrapMode="NoWrap";
                                    } else {
                                        textarea.wrapMode="WrapAnywhere";
                                    }
                                }
                                selectedTextColor: "white";
                                selectionColor: "blue";
                                selectByMouse: true;

                                MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.RightButton // 只接受右键点击
                                    hoverEnabled: true;
                                    cursorShape: Qt.IBeamCursor;

                                    onEntered: {
                                        backrec.visible=true;
                                    }
                                    onExited: {
                                        backrec.visible=false;
                                    }

                                    onClicked: function(mouse) {
                                        if (mouse.button === Qt.RightButton) {
                                            var contextMenu = contextMenuComponent.createObject(textarea ,{
                                                "x": mouse.x,
                                                "y": mouse.y,
                                                "textAreaRef": textarea,
                                                "msgId":msgid,
                                                "position":index
                                            })
                                            contextMenu.open();
                                        }
                                    }
                                }

                                TextArea{
                                    id:textInLine;
                                    textFormat: "RichText";
                                    text: parent.text;
                                    font.pixelSize: parent.font.pixelSize;
                                    visible: false;
                                    wrapMode: Text.NoWrap;
                                }

                                text: content;
                                font.pixelSize: fontSize;
                            }
                        }

                        Image {
                            id:loading_ima;
                            source:{
                                if(msgstate===0)
                                    return "/new/prefix1/res/loading.png";
                                if(msgstate===2)
                                    return "/new/prefix1/res/warning.svg";
                                if(msgstate===1)
                                    return "";
                            }

                            width: 20;
                            height: width;
                            anchors.bottom: msg_rec.bottom;
                            anchors.right: msg_rec.left;
                            anchors.leftMargin: 12;
                            visible:msgstate!==1&&type!==1&&way===0;
                            rotation:{
                                if(msgstate===0)
                                    return 0;
                                else if(msgstate===2)
                                    return 0;
                                else
                                    return 0;
                            }

                            RotationAnimation on rotation {
                                from: 0;
                                to: 360;
                                duration: 1000;
                                loops: Animation.Infinite;
                                running: msgstate===0;
                            }
                        }
                    }
                }
            }

            Popup{
                id:emojipopup;
                background: emoji_choice_rec;
                width: 350;
                height: 250;
                x:5;
                y:parent.height-height-10;
            }

            Rectangle{
                id:emoji_choice_rec;
                color:"white";
                radius: 8;
                width: 350;
                height: 250;

                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: emoji_choice_rec.radius;
                    samples: 16;
                    color: "#50000000";
                }

                Behavior on visible{
                    NumberAnimation{
                        duration: 80;
                    }
                }

                MouseArea{
                    anchors.fill: parent;
                    acceptedButtons: Qt.AllButtons;
                    hoverEnabled: true;
                }

                GridView{
                    id:emoji_choice_view;
                    anchors.fill: parent;
                    anchors.margins: 5;

                    cellWidth: width/8;
                    cellHeight: cellWidth;
                    model: emojiModel;
                    delegate: Rectangle{
                        width: emoji_choice_view.cellWidth;
                        height: emoji_choice_view.cellHeight;
                        color: "transparent";
                        Rectangle{
                            anchors.fill: parent;
                            anchors.margins: 5;
                            radius: 5;
                            color:"white";
                            Image {
                                anchors.fill: parent;
                                source: "image://emojiProvider/"+name;
                            }
                            MouseArea{
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onEntered: {
                                    parent.color="grey";
                                }
                                onExited: {
                                    parent.color="white";
                                }
                                onClicked: {
                                    var t="<img src=\""+"image://emojiProvider/"+name+"\" width=\""+28+"\" style='vertical-align: middle; padding-top: 35px;'>"
                                    textedit.insert(textedit.cursorPosition,t);
                                    textedit.cursorPosition++;
                                    emojipopup.close();
                                }
                            }
                        }
                    }
                }
            }


        }
        Rectangle{
            id:input_rec;
            width: parent.width;
            color: color_;
            SplitView.minimumHeight: 150;
            SplitView.preferredHeight: 200;

            Rectangle{
                id:choice_rec;
                width: parent.width;
                height: 25;
                color:"transparent"
                Image {
                    id: emoji;
                    source: "/new/prefix1/res/FaceSmile.svg"
                    height: parent.height;
                    width: height;
                    anchors.left: parent.left;
                    MouseArea{
                        anchors.fill: parent;
                        cursorShape: Qt.PointingHandCursor;
                        onClicked: {
                            if(!emojipopup.opened)
                                emojipopup.open();
                            else
                                emojipopup.close();
                        }
                    }
                }
                Image {
                    id: picture;
                    source: "/new/prefix1/res/Photo.svg";
                    height: parent.height;
                    width: height;
                    anchors.left: emoji.right;
                    anchors.leftMargin: 5;
                    MouseArea{
                        anchors.fill: parent;
                        cursorShape: Qt.PointingHandCursor;
                        onClicked: {
                            sendPicSig();
                        }
                    }
                }

            }

            TextEdit{
                id:textedit;
                anchors.bottom: parent.bottom;
                anchors.top: choice_rec.bottom;
                width: parent.width;
                font.pixelSize: fontSize;
                textFormat: TextEdit.RichText;
                topPadding: 5;
                wrapMode:implicitWidth+5>width?"WrapAnywhere":"NoWrap";

                Keys.onPressed:(event)=> {
                    if (event.key === Qt.Key_Return||event.key === Qt.Key_Enter) {
                        if(event.modifiers & Qt.ShiftModifier)
                            textedit.append("");
                        else
                            sendMsg();
                        event.accepted = true;
                    }
                }

                MouseArea{
                    anchors.fill: parent;
                    acceptedButtons: Qt.RightButton;
                    propagateComposedEvents: true;
                    cursorShape: Qt.IBeamCursor;
                    onClicked: function(mouse) {
                        if (mouse.button === Qt.RightButton) {
                            var contextMenu = inputMenu.createObject(textedit, {
                                "x": mouse.x,
                                "y": mouse.y,
                                "tarEdit":textedit
                            })
                            contextMenu.open();
                        }
                    }
                }
            }

            Rectangle{
                id:sendBtn;
                anchors.bottom: parent.bottom;
                anchors.right: parent.right;
                anchors.bottomMargin: 10;
                anchors.rightMargin:15;
                width: 60;
                height: 25;
                radius: 8;
                layer.enabled: true;
                layer.effect: DropShadow{
                    radius: 8;
                    samples: 16;
                    color:"#33000000";
                }

                Text {
                    text: "发送";
                    anchors.centerIn: parent;
                    font.bold: true;
                    font.pixelSize: 14;
                }
                MouseArea{
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onPressed: {
                        parent.color="#F5F5F5";
                    }
                    onReleased: {
                        parent.color="white";
                    }
                    onClicked: {
                        sendMsg();
                    }
                }
            }

            Rectangle{
                anchors.fill: parent;
                color:"white";
                visible: multiChoiceState;
                MouseArea{
                    anchors.fill: parent;
                }

                Row{
                    anchors.centerIn: parent;
                    spacing: 20;
                    Rectangle{
                        id:deleteBtn;
                        width: 60;
                        height: width;
                        radius: width/2;
                        smooth: true;

                        layer.enabled: true;
                        layer.effect: DropShadow{
                            radius: 8;
                            samples: 16;
                            color: "gray";
                        }
                        Image {
                            id:deleteIcon;
                            source: "/new/prefix1/res/Trash.svg"
                            width: 27;
                            height: width;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            anchors.top: parent.top;
                            anchors.topMargin: 10;
                        }
                        Text {
                            anchors.bottom: parent.bottom;
                            anchors.bottomMargin: 5;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            text:"删除";
                            font.pixelSize: 11;
                        }
                        MouseArea{
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onEntered: {
                                parent.color="red";
                            }
                            onExited: {
                                parent.color="white";
                            }
                            onClicked: {
                                deleteSig();
                                multiChoiceState=false;
                            }
                        }
                    }

                    Rectangle{
                        id:cancelBtn;
                        width: 60;
                        height: width;
                        radius: width/2;
                        smooth: true;

                        layer.enabled: true;
                        layer.effect: DropShadow{
                            radius:8;
                            samples: 16;
                            color: "gray";
                        }
                        Image {
                            id:cancelIcon;
                            source: "/new/prefix1/res/XMark.svg"
                            width: 27;
                            height: width;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            anchors.top: parent.top;
                            anchors.topMargin: 10;
                        }
                        Text {
                            anchors.bottom: parent.bottom;
                            anchors.bottomMargin: 5;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            text:"取消";
                            font.pixelSize: 11;
                        }
                        MouseArea{
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onEntered: {
                                parent.color="#F5F5F5";
                            }
                            onExited: {
                                parent.color="white";
                            }
                            onClicked: {
                                multiChoiceState=false;
                            }
                        }
                    }
                }
            }
        }   
    }

    property int fontSize: 19;

    signal sendPicSig();
    signal sendMsgSig(string content,int type)
    function sendMsg()
    {
        if(textedit.length===0)
            return;
        sendMsgSig(textedit.text,0);
        textedit.clear();
    }

    Component{
        id:inputMenu;
        CustomMenu{
            property var tarEdit: null;

            MenuItem {
                text: "全选";
                onTriggered: {
                    tarEdit.selectAll();
                }
            }
            MenuItem {
                text: "复制";
                onTriggered: {
                    tarEdit.copy();
                }
            }
            MenuItem {
                text: "粘贴";
                onTriggered: {
                    tarEdit.paste();
                }
            }
        }
    }

    Component {
        id:imageMenuComponent;
        CustomMenu{
            id:imageMenu;
            property string msgId: "";
            property int position: -1;
            MenuItem {
                text: "删除";
                onTriggered: {
                    updateChoiceState(imageMenu.msgId,imageMenu.position,true);
                    deleteSig();
                    multiChoiceState=false;
                }
            }
            MenuItem {
                text: multiChoiceState?"取消多选":"多选";
                onTriggered: {
                    multiChoiceState=!multiChoiceState;
                }
            }
        }
    }

    Component {
        id: contextMenuComponent

        CustomMenu {
            id:contextMenu;
            property var textAreaRef: null
            property var msgId: "";
            property var position: -1;
            MenuItem {
                text: "复制";
                onTriggered: {
                    if(textAreaRef.selectedText==="")
                        textAreaRef.selectAll();
                    textAreaRef.copy();
                    textAreaRef.select(0,0);
                }
            }
            MenuItem {
                text: "全选";
                onTriggered: {
                    textAreaRef.selectAll();
                }
            }
            MenuItem {
                text: "删除";
                onTriggered: {
                    updateChoiceState(contextMenu.msgId,contextMenu.position,true);
                    deleteSig();
                }
            }
            MenuItem {
                text: multiChoiceState?"取消多选":"多选";
                onTriggered: {
                    multiChoiceState=!multiChoiceState;
                }
            }

        }
    }
}
