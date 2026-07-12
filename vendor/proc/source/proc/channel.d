/**
Copyright: Copyright (c) 2020, Joakim Brännström. All rights reserved.
License: $(LINK2 http://www.boost.org/LICENSE_1_0.txt, Boost Software License 1.0)
Author: Joakim Brännström (joakim.brannstrom@gmx.com)
*/
module proc.channel;

import logger = std.experimental.logger;
import std.stdio : File;

/** A read channel over a `File` object.
 */
struct FileReadChannel {
    File file;

    private {
        enum State {
            none,
            active,
            hup,
            eof
        }

        State st;
    }

    this(File file) @trusted {
        this.file = file;
        this.st = State.active;
    }

    /// If the channel is open.
    bool isOpen() @safe {
        return st != State.eof && st != State.none;
    }

    version (Posix) {
        /** If there is data to read, non blocking.
         *
         * If this is called before read then it is guaranteed that read will not
         * block.
         */
        bool hasPendingData() @safe {
            import core.sys.posix.poll;

            if (st == State.eof) {
                return false;
            } else if (st == State.hup) {
                // will never block and transition to eof when out of data.
                return true;
            }

            pollfd[1] fds;
            fds[0].fd = file.fileno;
            fds[0].events = POLLIN;
            auto ready = () @trusted { return poll(&fds[0], 1, 0); }();

            // timeout triggered
            if (ready == 0) {
                return false;
            }

            if (ready < 0) {
                import core.stdc.errno : errno, EINTR;

                if (errno == EINTR) {
                    // poll just interrupted. try again.
                    return false;
                }

                // an errnor occured.
                st = State.eof;
                return false;
            }

            if (fds[0].revents & POLLHUP) {
                // POLLHUP mean that the other side has been closed. A read will
                // always succeed. If the read returns zero length then it means
                // that the pipe is out of data. Thus the worst thing that happens
                // is that we get nothing, an empty slice.
                st = State.hup;
                return true;
            }

            if (fds[0].revents & (POLLNVAL | POLLERR)) {
                st = State.eof;
                return false;
            }

            return (fds[0].revents & POLLIN) != 0;
        }

        /** Read at most `s` bytes from the channel.
         *
         * The data is written directly to buf.
         * The lengt of buf determines how much is read.
         *
         * buf is not resized. Use the returned value.
         */
        ubyte[] read(ref ubyte[] buf) return scope @trusted {
            static import core.sys.posix.unistd;

            if (st == State.eof || buf.length == 0) {
                return null;
            }

            const res = core.sys.posix.unistd.read(file.fileno, &buf[0], buf.length);
            if (res <= 0) {
                st = State.eof;
                return null;
            }

            return buf[0 .. res];
        }
    } else version (Windows) {
        bool hasPendingData() @trusted {
            import core.sys.windows.winbase : PeekNamedPipe;
            import core.sys.windows.windef : DWORD;

            if (st == State.eof) {
                return false;
            } else if (st == State.hup) {
                return true;
            }

            DWORD avail;
            if (PeekNamedPipe(file.windowsHandle, null, 0, null, &avail, null) == 0) {
                // the other side closed the pipe. a final read is needed to
                // transition to eof.
                st = State.hup;
                return true;
            }

            return avail != 0;
        }

        ubyte[] read(ref ubyte[] buf) return scope @trusted {
            import core.sys.windows.winbase : ReadFile;
            import core.sys.windows.windef : DWORD;

            if (st == State.eof || buf.length == 0) {
                return null;
            }

            DWORD res;
            if (ReadFile(file.windowsHandle, &buf[0], cast(DWORD) buf.length, &res, null) == 0
                    || res == 0) {
                st = State.eof;
                return null;
            }

            return buf[0 .. res];
        }
    }

    /** Read at most `s` bytes from the channel.
     *
     * Note that this is slow because the data is copied to keep the interface
     * memory safe. Prefer the one that takes a buffer
     */
    const(ubyte)[] read(const size_t size) return scope @safe {
        auto buffer = new ubyte[size];
        return cast(const(ubyte)[]) this.read(buffer);
    }

    /// Flush the input.
    void flush() @safe {
        file.flush();
    }
}

/** IO channel via `File` objects.
 *
 * Useful when e.g. communicating over pipes.
 */
struct FileWriteChannel {
    File file;

    this(File file) @safe {
        this.file = file;
    }

    const(ubyte)[] write(scope return const(char)[] data_) @trusted {
        return this.write(cast(const(ubyte)[]) data_);
    }

    /** Write data to the output channel.
     *
     * Returns: the data that was written
     */
    version (Posix) {
        const(ubyte)[] write(scope return const(ubyte)[] data) @trusted {
            static import core.sys.posix.unistd;

            const res = core.sys.posix.unistd.write(file.fileno, &data[0], data.length);
            if (res <= 0) {
                return null;
            }
            return data[0 .. res];
        }
    } else version (Windows) {
        const(ubyte)[] write(scope return const(ubyte)[] data) @trusted {
            import core.sys.windows.winbase : WriteFile;
            import core.sys.windows.windef : DWORD;

            DWORD res;
            if (WriteFile(file.windowsHandle, &data[0], cast(DWORD) data.length, &res, null) == 0
                    || res == 0) {
                return null;
            }
            return data[0 .. res];
        }
    }

    /// Flush the output.
    void flush() @safe {
        file.flush();
    }

    /// Close the write channel.
    void closeWrite() @safe {
        file.close;
    }
}

version (Windows) {
    private enum nullFile = "NUL";
} else {
    private enum nullFile = "/dev/null";
}

/// Returns: a `File` object reading from the null device.
File nullIn() @safe {
    return File(nullFile, "r");
}

/// Returns: a `File` object writing to the null device.
File nullOut() @safe {
    return File(nullFile, "w");
}
