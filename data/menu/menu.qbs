import qbs 1.0

Product {
    name: "Liri Menu"

    Depends { name: "lirideployment" }

    Group {
        name: "Directories"
        files: ["*.directory"]
        qbs.install: true
        qbs.installDir: lirideployment.dataDir + "/desktop-directories"
    }

    Group {
        name: "Menu"
        files: ["*.menu"]
        qbs.install: true
        qbs.installDir: lirideployment.etcDir + "/xdg/menus"
    }
}
