import qbs 1.0

QtGuiApplication {
    name: "liri-screenshot"
    targetName: "liri-screenshot"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["core", "core-private", "gui", "widgets", "quickcontrols2"]; versionAtLeast: "5.8" }
    Depends { name: "LiriWaylandClient" }

    cpp.defines: [
        'LIRIWORKSPACE_VERSION="' + project.version + '"',
        "QT_NO_CAST_FROM_ASCII",
        "QT_NO_CAST_TO_ASCII"
    ]

    files: ["*.cpp", "*.h", "*.qrc"]

    Group {
        name: "QML Files"
        files: ["*.qml"]
        prefix: "qml/"
        fileTags: ["qml"]
    }

    Group {
        name: "Translations"
        files: ["*_*.ts"]
        prefix: "translations/"
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.binDir
        fileTagsFilter: product.type
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.dataDir + "/liri-workspace/translations"
        fileTagsFilter: "qm"
    }
}
