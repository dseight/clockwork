import QtQuick 2.6
import Sailfish.Silica 1.0
import com.dseight.clockwork 1.0

ListItem {
    property var previewIcons
    property alias text: textLabel.text
    property alias description: descriptionLabel.text
    readonly property real iconSize: Theme.iconSizeLauncher

    contentHeight: iconSize + 2 * Theme.paddingMedium

    Row {
        id: preview
        anchors.verticalCenter: parent.verticalCenter
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        height: iconSize
        spacing: Theme.paddingMedium
        layoutDirection: Qt.RightToLeft

        Repeater {
            id: previewRepeater

            readonly property int maxIcons: preview.width / (iconSize + preview.spacing)
            model: previewIcons ? previewIcons.slice(0, maxIcons) : undefined

            delegate: HighlightImage {
                width: iconSize
                height: iconSize
                sourceSize.width: iconSize
                sourceSize.height: iconSize
                source: modelData

                monochromeWeight: colorWeight
                highlightColor: Theme.highlightBackgroundColor
            }
        }
    }

    OpacityRampEffect {
        sourceItem: preview
        direction: OpacityRamp.RightToLeft
        offset: 0.0
        slope: preview.width / (preview.width - textLabel.width - 2 * Theme.paddingLarge)
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin + 2 * iconSize
            verticalCenter: parent.verticalCenter
        }

        Label {
            id: textLabel
            width: implicitWidth < parent.width ? implicitWidth : parent.width
            truncationMode: TruncationMode.Fade
        }

        Label {
            id: descriptionLabel
            width: parent.width
            visible: text != ""
            truncationMode: TruncationMode.Fade
            color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            font.pixelSize: Theme.fontSizeExtraSmall
        }
    }
}
