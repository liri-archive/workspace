import qbs 1.0
import qbs.File
import qbs.FileInfo
import qbs.ModUtils
import qbs.TextFile

Product {
    name: "GSettings Schemas"
    type: "gschemaOut"

    Depends { name: "lirideployment" }

    Group {
        name: "Schemas"
        files: ["*.gschema.xml"]
        qbs.install: true
        qbs.installDir: lirideployment.dataDir + "/glib-2.0/schemas"
    }

    Group {
        name: "Schemas Templates"
        files: ["*.gschema.xml.in"]
        fileTags: ["gschemaIn"]
    }

    Rule {
        inputs: ["gschemaIn"]

        Artifact {
            filePath: input.fileName.replace(".in", "")
            fileTags: ["gschemaOut"]
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "creating " + output.fileName;
            cmd.highlight = "filegen";
            cmd.vars = {
                DATADIR: FileInfo.joinPaths(product.moduleProperty("qbs", "installRoot"),
                                            product.moduleProperty("lirideployment", "dataDir")),
            };
            cmd.sourceCode = function() {
                File.copy(input.filePath, output.filePath);

                var file = new TextFile(output.filePath, TextFile.ReadWrite);
                var contents = file.readAll();
                for (var i in vars)
                    contents = contents.replace(new RegExp('@' + i + '@(?!\w)', 'g'), vars[i]);
                file.truncate();
                file.write(contents);
                file.close();
            };
            return [cmd];
        }
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.dataDir + "/glib-2.0/schemas"
        fileTagsFilter: "gschemaOut"
    }
}
