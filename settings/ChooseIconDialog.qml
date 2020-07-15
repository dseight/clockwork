import QtQuick 2.6
import Sailfish.Silica 1.0
import com.jolla.settings 1.0
import com.dseight.clockwork 1.0

Page {
    id: root

    property string provider
    signal accepted

    function acceptProvider(newProvider) {
        provider = newProvider
        accepted()
        pageStack.pop()
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTrId("settings_clockwork-icon_packs")
            }

            IconPackItem {
                width: column.width
                //% "Default icon"
                text: qsTrId("settings_clockwork-default_icon")
                onClicked: {
                    provider = ""
                    accepted()
                    pageStack.pop()
                }
            }

            Repeater {
                model: iconPacks

                delegate: IconPackItem {
                    width: column.width
                    text: model.displayName
                    previewIcons: model.previewIcons
                    onClicked: {
                        var dialog = pageStack.push(chooseIconPage, {
                            iconPack: model
                        })
                    }
                }
            }
        }
    }

    IconPacksModel {
        id: iconPacks
    }

    Component {
        id: chooseIconPage

        Page {
            property var iconPack

            ApplicationsGridView {
                id: gridView

                model: iconPack.icons

                header: PageHeader {
                    title: iconPack.displayName
                }

                delegate: BackgroundItem {
                    readonly property string path: iconPack.name + "/" + modelData

                    width: gridView.cellWidth
                    height: gridView.cellHeight
                    highlightedColor: "transparent"

                    onClicked: {
                        provider = "icon-pack://" + path
                        accepted()
                        pageStack.pop(pageStack.previousPage(root))
                    }

                    HighlightImage {
                        property real size: Theme.iconSizeLauncher

                        anchors.centerIn: parent

                        width: size
                        height: size
                        sourceSize.width: size
                        sourceSize.height: size

                        source: "image://clockwork/icon-pack/" + path
                        monochromeWeight: colorWeight
                        highlightColor: Theme.highlightBackgroundColor
                    }
                }
            }
        }
    }
}
