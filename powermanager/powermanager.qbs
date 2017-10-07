import qbs 1.0

QtGuiApplication {
    name: "liri-powermanager"
    targetName: "liri-powermanager"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["core", "dbus"] }
    Depends { name: "Qt5GSettings" }
    Depends { name: "LiriCore" }
    Depends { name: "LiriNotifications" }
    Depends { name: "KF5.Solid" }

    cpp.defines: [
        'LIRIWORKSPACE_VERSION="' + project.version + '"',
        "QT_NO_CAST_FROM_ASCII",
        "QT_NO_CAST_TO_ASCII"
    ]

    files: [
        "batterywatcher.cpp",
        "batterywatcher.h",
        "idlewatcher.cpp",
        "idlewatcher.h",
        "main.cpp",
        "powermanager.cpp",
        "powermanager.h",
    ]

    Group {
        name: "Desktop File"
        files: ["*.desktop"]
        qbs.install: true
        qbs.installDir: lirideployment.etcDir + "/xdg/autostart"
        qbs.installPrefix: ""
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.binDir
        fileTagsFilter: product.type
    }
}
