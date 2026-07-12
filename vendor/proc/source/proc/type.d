/**
Copyright: Copyright (c) 2020, Joakim Brännström. All rights reserved.
License: $(LINK2 http://www.boost.org/LICENSE_1_0.txt, Boost Software License 1.0)
Author: Joakim Brännström (joakim.brannstrom@gmx.com)
*/
module proc.type;

version (Posix) {
    public import core.sys.posix.unistd : pid_t;
} else {
    alias pid_t = int;
}

struct Pid {
    pid_t value;
    alias value this;
}

struct PidGroup {
    pid_t value;
    alias value this;
}
