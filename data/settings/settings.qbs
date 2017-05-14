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
                DATA_DIR: FileInfo.joinPaths(product.moduleProperty("qbs", "installRoot"),
                                             product.moduleProperty("lirideployment", "dataDir")),
            };
            cmd.sourceCode = function() {
                File.copy(input.filePath, output.filePath);

                var inFile = new TextFile(output.filePath, TextFile.ReadWrite);
                var contents = inFile.readAll();
                for (var i in vars)
                    contents = contents.replace(new RegExp('@' + i + '@(?!\w)', 'g'), vars[i]);
                inFile.truncate();
                inFile.write(contents);
                inFile.close();
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
