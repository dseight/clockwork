import QtQuick 2.6
import Sailfish.Silica 1.0

ListItem {
    property alias iconSource: icon.source
    property alias text: titleLabel.text
    property alias description: descriptionLabel.text
    property alias showDescription: descriptionLabel.visible
    property alias iconSwitch: _iconSwitch

    contentHeight: icon.height + 2 * Theme.paddingMedium

    HighlightImage {
        id: icon
        property real size: Theme.iconSizeLauncher

        anchors {
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }

        width: size
        height: size
        sourceSize.width: size
        sourceSize.height: size

        opacity: _iconSwitch.checked ? 1.0 : Theme.opacityLow
        cache: false

        monochromeWeight: colorWeight
        highlightColor: Theme.highlightBackgroundColor
    }

    Column {
        anchors {
            left: icon.right
            right: _iconSwitch.left
            leftMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }

        opacity: _iconSwitch.checked ? 1.0 : Theme.opacityLow

        Label {
            id: titleLabel
            width: parent.width
            truncationMode: TruncationMode.Fade
        }

        Label {
            id: descriptionLabel

            visible: true
            width: parent.width
            truncationMode: TruncationMode.Fade
            color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            font.pixelSize: Theme.fontSizeExtraSmall
        }
    }

    Switch {
        id: _iconSwitch
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
    }
}
