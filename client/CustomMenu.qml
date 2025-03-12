import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

// 使用示例：CustomMenu { id: menu; MenuItem { text: "选项1" } ... }
Menu {
    id: customMenu

    // 默认属性设置
    property int radius: 8
    property color backgroundColor: "#ffffff"
    property color borderColor: "#e0e0e0"
    property int borderWidth: 1
    property color shadowColor: "#88000000"
    property int shadowRadius: 10
    property int shadowOffsetX: 0
    property int shadowOffsetY: 3
    property color highlightColor: "#f0f0f0"        // 默认高亮背景色
    property color textColor: "#333333"             // 默认文本颜色
    property color highlightTextColor: "#2196F3"    // 高亮文本颜色

    // 菜单间距
    spacing: 2

    // 确保菜单在最上层显示
    z: 1000

    // 定义菜单背景和效果
    background: Item {
        implicitWidth: 80
        implicitHeight: 40

        Rectangle {
            id: menuBackground
            anchors.fill: parent
            color: customMenu.backgroundColor
            radius: customMenu.radius
            border.width: customMenu.borderWidth
            border.color: customMenu.borderColor
        }

        // 阴影效果
        DropShadow {
            anchors.fill: menuBackground
            horizontalOffset: customMenu.shadowOffsetX
            verticalOffset: customMenu.shadowOffsetY
            radius: customMenu.shadowRadius
            samples: customMenu.shadowRadius * 2 + 1
            color: customMenu.shadowColor
            source: menuBackground
            // 确保阴影在背景之下
            z: -1
        }
    }

    // 自定义菜单项样式
    delegate: MenuItem {
        id: menuItem

        // 菜单项高度
        height: 36

        background: Rectangle {
            color: "transparent";
            border.color: "grey";
            border.width: 1;
        }

        // 菜单项内容
        contentItem: Text {
            text: menuItem.text
            font.pixelSize: 20
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            leftPadding: 16
            elide: Text.ElideRight
        }
    }

    // 这是Qt6中新增的，保证显示正确
    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 100 }
    }

    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 100 }
    }
}
