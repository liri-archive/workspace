import qbs 1.0

Project {
    name: "Workspace"

    readonly property string version: "0.9.0"

    property bool useStaticAnalyzer: false

    condition: qbs.targetOS.contains("linux") && !qbs.targetOS.contains("android")

    minimumQbsVersion: "1.8.0"

    references: [
        "data/menu/menu.qbs",
        "data/settings/settings.qbs",
        "powermanager/powermanager.qbs",
        "screencast/screencast.qbs",
        "screenshot/screenshot.qbs",
    ]
}
