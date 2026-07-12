#!/usr/bin/env dub
/+ dub.sdl:
    name "symlink"
+/
import std.file : remove, exists, mkdirRecurse;
import std.path : dirName;
import std.stdio : writeln;

version (Windows) {
    // Creating symlinks on Windows requires administrator privileges or
    // developer mode. Fall back to copying which is good enough for the
    // build system's use of this tool (collecting binaries and data
    // directories in known locations).
    void link(string src, string dst) {
        import std.file : copy, isDir, dirEntries, SpanMode, rmdirRecurse,
            PreserveAttributes;
        import std.path : buildPath, relativePath;

        if (exists(dst)) {
            if (isDir(dst))
                rmdirRecurse(dst);
            else
                remove(dst);
        }

        if (isDir(src)) {
            mkdirRecurse(dst);
            foreach (e; dirEntries(src, SpanMode.breadth)) {
                const p = buildPath(dst, relativePath(e.name, src));
                if (e.isDir)
                    mkdirRecurse(p);
                else
                    copy(e.name, p, PreserveAttributes.no);
            }
        } else {
            copy(src, dst, PreserveAttributes.no);
        }
    }
} else {
    void link(string src, string dst) {
        import std.file : symlink;

        if (exists(dst)) {
            remove(dst);
        }
        symlink(src, dst);
    }
}

int main(string[] args) {
    if (args.length != 3) {
        writeln("Wrong number of arguments: src dst");
        return 1;
    }

    auto src = args[1];
    auto dst = args[2];

    if (!exists(dst.dirName)) {
        mkdirRecurse(dst.dirName);
    }

    link(src, dst);

    return 0;
}
