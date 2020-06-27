import QtQuick 2.6
import Sailfish.Silica 1.0
import Sailfish.Lipstick 1.0
import com.jolla.settings 1.0
import com.dseight.clockwork 1.0

Dialog {
    property var iconPack

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            DialogHeader {
                //: %1 is the icon pack name
                //% "Use %1?"
                title: qsTrId("settings_clockwork-use_some_icon_pack").arg(iconPack.displayName)
            }

            Item {
                width: parent.width
                height: gridView.height

                ApplicationsGridView {
                    id: gridView

                    height: cellHeight * Math.ceil(count/columns)
                    interactive: false

                    delegate: LauncherGridItem {
                        property bool available: iconProvider.iconAvailable(model.filePath)

                        width: gridView.cellWidth
                        height: gridView.cellHeight
                        icon: available
                              ? iconProvider.iconSource(model.filePath)
                              : model.iconId
                        text: model.name
                        enabled: available
                    }
                }
            }
        }
    }

    AppGridIconProvider {
        id: iconProvider
        iconPackName: iconPack.name
    }
}
