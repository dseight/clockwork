import QtQuick 2.6
import Sailfish.Silica 1.0
import com.dseight.clockwork 1.0

Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                //% "About %1"
                text: qsTrId("settings_clockwork-about_app").arg("Clockwork")
                onClicked: pageStack.push("AboutPage.qml")
            }
        }

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: "Clockwork"
            }

            SectionHeader {
                //% "Dynamic icons"
                text: qsTrId("settings_clockwork-dynamic_icons")
            }

            Repeater {
                model: sortedDynamicIcons

                delegate: DynamicIconItem {
                    width: column.width

                    iconSource: model.iconSource
                    text: model.applicationName

                    // Displayed only if there are several dynamic icons
                    // for a single application
                    showDescription: !model.unique
                    description: model.name

                    iconSwitch.automaticCheck: false
                    iconSwitch.checked: model.enabled
                    iconSwitch.onClicked: model.enabled = !model.enabled
                }
            }
        }
    }

    DynamicIconsModel {
        id: dynamicIcons
    }

    DynamicIconsSortModel {
        id: sortedDynamicIcons
        sourceModel: dynamicIcons
        filterRole: DynamicIconsModel.AvailableRole
        filterRegExp: /true/
    }
}
