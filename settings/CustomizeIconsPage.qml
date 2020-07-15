import QtQuick 2.6
import Sailfish.Silica 1.0
import Sailfish.Lipstick 1.0
import com.jolla.settings 1.0
import Nemo.Configuration 1.0

Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTrId("settings_clockwork-customize_icons")
            }

            Item {
                width: parent.width
                height: gridView.height

                ApplicationsGridView {
                    id: gridView

                    height: cellHeight * Math.ceil(count/columns)
                    interactive: false

                    delegate: LauncherGridItem {
                        id: appItem

                        readonly property string packageName: model.filePath.split('/').pop().split('.').shift()
                        readonly property bool customized: provider.value != ""

                        width: gridView.cellWidth
                        height: gridView.cellHeight
                        icon: model.iconId
                        text: model.name

                        onClicked: {
                            var dialog = pageStack.push("ChooseIconDialog.qml")
                            dialog.accepted.connect(function() {
                                provider.value = dialog.provider
                            })
                        }

                        Image {
                            anchors {
                                top: parent.top
                                right: parent.right
                            }
                            source: "image://theme/icon-s-asterisk"
                            visible: customized
                        }

                        ConfigurationValue {
                            id: provider
                            key: "/com/dseight/clockwork/applications/" + packageName + "/provider"
                            defaultValue: ""
                        }
                    }
                }
            }
        }
    }
}
