import qbs 1.0

QtGuiApplication {
    name: "liri-powermanager"
    targetName: "liri-powermanager"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["core", "dbus"] }

    cpp.defines: [
        'LIRIWORKSPACE_VERSION="' + project.version + '"',
        "QT_NO_CAST_FROM_ASCII",
        "QT_NO_CAST_TO_ASCII"
    ]

    files: ["*.cpp", "*.h"]

    Group {
        name: "Desktop File"
        files: ["*.desktop"]
        qbs.install: true
        qbs.installDir: lirideployment.etcDir + "/xdg/autostart"
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.binDir
        fileTagsFilter: product.type
    }
}
