import QtQuick 2.6
import Sailfish.Silica 1.0
import com.dseight.clockwork 1.0

Page {
    SilicaFlickable {
        anchors.fill: parent

        Column {
            width: parent.width

            PageHeader {
                // Looks like it's better to leave page header empty,
                // otherwise page becomes cluttered with application name
            }

            Image {
                property real size: Theme.iconSizeExtraLarge

                anchors.horizontalCenter: parent.horizontalCenter
                width: size
                height: size
                sourceSize.width: size
                sourceSize.height: size
                source: "icon-clockwork.svg"
            }

            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                topPadding: Theme.paddingMedium
                bottomPadding: Theme.paddingMedium
                color: Theme.highlightColor
                text: "Clockwork " + clockworkPluginVersion
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                topPadding: Theme.paddingMedium
                bottomPadding: 2 * Theme.paddingLarge

                //: %1 is application name, text surrounded by %2 and %3 is a link
                //% "%1 is distributed under terms of %2MIT License%3."
                text: qsTrId("settings_clockwork-licensing")
                      .arg("Clockwork")
                      .arg("<a href='https://github.com/dseight/clockwork/blob/master/LICENSE'>")
                      .arg("</a>")

                color: Theme.highlightColor
                linkColor: Theme.primaryColor
                textFormat: Text.StyledText
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall

                onLinkActivated: Qt.openUrlExternally(link)
            }

            ButtonLayout {
                width: parent.width

                Button {
                    //% "Report issue"
                    text: qsTrId("settings_clockwork-report_issue")
                    onClicked: Qt.openUrlExternally("https://github.com/dseight/clockwork/issues/new")
                }
            }
        }
    }
}
