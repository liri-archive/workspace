import qbs 1.0

Project {
    name: "Workspace"

    readonly property string version: "0.9.0"

    condition: qbs.targetOS.contains("linux")

    minimumQbsVersion: "1.6"

    qbsSearchPaths: "qbs/shared"

    references: [
        "data/menu/menu.qbs",
        "data/settings/settings.qbs",
        "powermanager/powermanager.qbs",
        "screencast/screencast.qbs",
        "screenshot/screenshot.qbs",
    ]
}
