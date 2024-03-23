import QtQuick.Controls
import QtQuick

ToolTip {
   id: tip

   property alias message: message_label.text
   property color background_color: "red"

   signal messageClicked()

   width: parent.width
   timeout: 5000
   height: 50
   y: parent.height - height

   ParallelAnimation {
        id: move_anim

        NumberAnimation {
            target: tip.background
            properties: "x"
            from: tip.x
            to: tip.width
            duration: 300
            easing.type: Easing.Bezier
        }

        PropertyAnimation {
             target: tip
             property: "opacity"
             duration: 300
             from : 1
             to : 0
             easing.type: Easing.Bezier
        }
        onFinished : tip.reset()
   }

   contentItem: Label {
       id: message_label
       font.bold: true
       font.pixelSize: 25
    }

   TapHandler {
       onSingleTapped: {
            move_anim.start()
            messageClicked()
       }
   }

   background:  Rectangle {
       id: background_bg
       color: background_color
   }

   function reset() {
       tip.visible = false;
       tip.contentItem.visible = false;
       tip.background.visible = false;
       tip.background.opacity = 1;
       tip.background.x = 0
       tip.background.y = tip.height - tip.background.height
   }

   function open() {
       tip.visible = true
       tip.background.visible= tip.visible
       tip.contentItem.visible =tip.visible
   }
}
