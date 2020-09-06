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

            SectionHeader {
                //% "Icon packs"
                text: qsTrId("settings_clockwork-icon_packs")
            }

            Repeater {
                id: iconPacksRepeater
                model: iconPacks

                delegate: IconPackItem {
                    width: column.width
                    text: model.displayName
                    description: model.enabled
                        //% "In use"
                        ? qsTrId("settings_clockwork-icon_pack_enabled")
                        : ""
                    previewIcons: model.previewIcons
                    onClicked: {
                        if (model.enabled) {
                            openMenu()
                        } else {
                            var dialog = pageStack.push("UseIconPackDialog.qml", {
                                iconPack: model
                            })
                            dialog.accepted.connect(function() {
                                model.enabled = true
                            })
                        }
                    }

                    menu: ContextMenu {
                        MenuItem {
                            text: model.enabled
                                //% "Not use"
                                ? qsTrId("settings_clockwork-not_use")
                                //% "Use"
                                : qsTrId("settings_clockwork-use")
                            onClicked: model.enabled = !model.enabled
                        }
                    }
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                //% "No icon packs available"
                text: qsTrId("settings_clockwork-no_icon_packs_avail")
                visible: iconPacksRepeater.count === 0
                color: Theme.secondaryHighlightColor
                topPadding: Theme.paddingMedium
                bottomPadding: Theme.paddingMedium
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                visible: iconPacksRepeater.count !== 0
                color: Theme.secondaryHighlightColor
                topPadding: Theme.paddingLarge
                bottomPadding: Theme.paddingLarge
                font.pixelSize: Theme.fontSizeExtraSmall
                //% "You can choose icon for application from any installed pack."
                text: qsTrId("settings_clockwork-customize_icons_hint")
            }

            ButtonLayout {
                visible: iconPacksRepeater.count !== 0

                Button {
                    //% "Customize icons"
                    text: qsTrId("settings_clockwork-customize_icons")
                    onClicked: pageStack.push("CustomizeIconsPage.qml")
                }
            }

            Item { width: 1; height: Theme.paddingLarge }
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

    IconPacksModel {
        id: iconPacks
    }
}
