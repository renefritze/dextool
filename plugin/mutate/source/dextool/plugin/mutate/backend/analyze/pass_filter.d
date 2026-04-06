/**
Copyright: Copyright (c) 2020, Joakim Brännström. All rights reserved.
License: MPL-2
Author: Joakim Brännström (joakim.brannstrom@gmx.com)

This Source Code Form is subject to the terms of the Mozilla Public License,
v.2.0. If a copy of the MPL was not distributed with this file, You can obtain
one at http://mozilla.org/MPL/2.0/.

Filter mutants based on simple textual pattern matching. These are the obvious
equivalent or unproductive mutants.
*/
module dextool.plugin.mutate.backend.analyze.pass_filter;

import logger = std.experimental.logger;
import std.algorithm : among, map, filter, cache, all;
import std.algorithm.mutation : stripRight;
import std.array : appender, empty;
import std.typecons : Tuple;

import blob_model : Blob;

static import colorlog;

import dextool.plugin.mutate.backend.interface_ : FilesysIO;
import dextool.plugin.mutate.backend.type : Language, Offset, Mutation;
import dextool.plugin.mutate.backend.analyze.pass_mutant : MutantsResult;
import dextool.plugin.mutate.backend.generate_mutant : makeMutationText, MakeMutationTextResult;

alias log = colorlog.log!"analyze.pass_filter";

shared static this() {
    colorlog.make!(colorlog.SimpleLogger)(logger.LogLevel.info, "analyze.pass_filter");
}

@safe:

MutantsResult filterMutants(FilesysIO fio, MutantsResult mutants) {
    foreach (f; mutants.files.map!(a => a.path)) {
        log.trace(f);
        auto file = fio.makeInput(f);
        foreach (r; mutants.getMutationPoints(f)
                .map!(a => analyzeForUnproductiveMutant(file, a, mutants.lang))
                .cache
                .filter!(a => !a.kind.empty)) {
            foreach (k; r.kind) {
                mutants.drop(f, r.point, k);
            }
        }
    }

    return mutants;
}

private:

alias Mutants = Tuple!(Mutation.Kind[], "kind", MutantsResult.MutationPoint, "point");

/// Returns: mutants to drop from the mutation point.
Mutants analyzeForUnproductiveMutant(Blob file, Mutants mutants, const Language lang) {
    auto app = appender!(Mutation.Kind[])();

    foreach (k; mutants.kind) {
        if (isEmpty(file, mutants.point.offset)) {
            log.tracef("Dropping unproductive mutant. Mutant is empty (%s %s %s)",
                    file.uri, mutants.point, k);
            app.put(k);
            continue;
        }

        auto mutant = makeMutationText(file, mutants.point.offset, k, lang);
        if (isTextuallyEqual(file, mutants.point.offset, mutant.rawMutation)) {
            log.tracef("Dropping equivalent mutant. Original and mutant is textually equivalent (%s %s %s)",
                    file.uri, mutants.point, k);
            app.put(k);
        } else if (lang.among(Language.assumeCpp, Language.cpp)
                && isUnproductiveCppPattern(file, mutants.point.offset, mutant.rawMutation)) {
            log.tracef("Dropping unproductive mutant. The mutant is an unproductive C++ mutant pattern (%s %s %s)",
                    file.uri, mutants.point, k);
            app.put(k);
        } else if (isOnlyWhitespace(file, mutants.point.offset, mutant.rawMutation)) {
            log.tracef("Dropping equivalent mutant. Both the original and the mutant is only whitespaces (%s %s %s)",
                    file.uri, mutants.point, k);
            app.put(k);
        }
    }

    return Mutants(app.data, mutants.point);
}

bool isEmpty(Blob file, Offset o) {
    // well an empty region can just be removed
    return o.isZero || o.end > file.content.length;
}

bool isTextuallyEqual(Blob file, Offset o, const(ubyte)[] mutant) {
    return file.content[o.begin .. o.end] == mutant;
}

// if both the original and mutation is only whitespace
bool isOnlyWhitespace(Blob file, Offset o, const(ubyte)[] mutant) {
    import std.algorithm : canFind;

    static immutable ubyte[6] whitespace = [
        cast(ubyte) ' ', cast(ubyte) '\t', cast(ubyte) '\v', cast(ubyte) '\r',
        cast(ubyte) '\n', cast(ubyte) '\f'
    ];

    bool rval = true;
    foreach (a; file.content[o.begin .. o.end]) {
        rval = rval && whitespace[].canFind(a);
    }

    foreach (a; mutant) {
        rval = rval && whitespace[].canFind(a);
    }

    return rval;
}

bool isUnproductiveCppPattern(Blob file, Offset o, const(ubyte)[] mutant) {
    static immutable ubyte[2] ctorParenthesis = ['(', ')'];
    static immutable ubyte[2] ctorCurly = ['{', '}'];
    static immutable ubyte zero = '0';
    static immutable ubyte one = '1';
    static immutable ubyte[5] false_ = ['f', 'a', 'l', 's', 'e'];
    static immutable ubyte[4] true_ = ['t', 'r', 'u', 'e'];

    // e.g. delete of the constructor {} is unproductive. It is almost always an
    // equivalent mutant.
    if (o.end - o.begin == 2 && file.content[o.begin .. o.end].among(ctorParenthesis[],
            ctorCurly[])) {
        return true;
    }

    // replacing '0' with 'false' and '1' with 'true' is equivalent
    if (file.content[o.begin] == zero && false_ == mutant
            || file.content[o.begin] == one && true_ == mutant) {
        return true;
    }

    // replacing zero-valued literals with plain zero is equivalent
    const original = file.content[o.begin .. o.end];
    if (isEquivalentZeroIntMutant(original, mutant)
            || isEquivalentZeroFloatMutant(original, mutant)) {
        return true;
    }

    return false;
}

bool isEquivalentZeroIntMutant(const(ubyte)[] original, const(ubyte)[] mutant) {
    import std.algorithm.searching : canFind;

    static immutable ubyte[11] ignoredChars = ['u', 'U', 'l', 'L', 'z', 'Z', 'x', 'X', 'b',
        'B', '\''];

    return mutant == ['0']
        && original.all!(c => c == '0' || ignoredChars[].canFind(c));
}

bool isEquivalentZeroFloatMutant(const(ubyte)[] original, const(ubyte)[] mutant) {
    if (mutant != ['0','.','0']) {
        return false;
    }

    const literal = stripFloatingLiteralSuffix(original);
    if (literal.length >= 2 && literal[0] == '0' && literal[1].among('x', 'X')) {
        return isZeroHexFloatingLiteral(literal);
    }

    return isZeroDecimalFloatingLiteral(literal);
}

const(ubyte)[] stripFloatingLiteralSuffix(const(ubyte)[] literal) {
    import std.algorithm.searching : endsWith;

    static immutable suffixes = [
        "bf16", "BF16", "f128", "F128", "f64", "F64", "f32", "F32", "f16",
        "F16", "f", "F", "l", "L"
    ];

    foreach (suffix; suffixes) {
        const suffixBytes = cast(const(ubyte)[]) suffix;
        if (literal.length > suffixBytes.length && literal.endsWith(suffixBytes)) {
            return literal[0 .. $ - suffixBytes.length];
        }
    }

    return literal;
}

bool isZeroDecimalFloatingLiteral(const(ubyte)[] literal) {
    import std.algorithm.searching : countUntil;

    const exponentPos = literal.countUntil!(c => c == 'e' || c == 'E');
    const significand = exponentPos >= 0 ? literal[0 .. exponentPos] : literal;
    return isZeroFloatingSignificand(significand);
}

bool isZeroHexFloatingLiteral(const(ubyte)[] literal) {
    import std.algorithm.searching : countUntil;

    const significand = literal[2 .. $];
    const exponentPos = significand.countUntil!(c => c == 'p' || c == 'P');
    assert(exponentPos >= 0);
    return isZeroFloatingSignificand(significand[0 .. exponentPos]);
}

bool isZeroFloatingSignificand(const(ubyte)[] literal) {
    return literal.all!(c => c.among('0', '.', '\''));
}

// TODO: move these newer C++ literal cases to the analyzer integration tests
// once the oldest supported LLVM/libclang can parse them there:
// - C++23 z/Z integer suffixes
// - C++23 fixed-width floating suffixes
// - C++17 hex floating literals
@("shall treat zero-valued C++23 integer literals as equivalent to plain 0")
@safe unittest {
    import unit_threaded : shouldBeTrue;

    foreach (literal; [
        "0z", "0Z", "00z", "0uz", "0uZ", "0Uz", "0UZ", "0zu", "0zU", "0Zu",
        "0ZU", "0x0z", "0x0UZ", "0b0z", "0'0z"
    ]) {
        isEquivalentZeroIntMutant(cast(const(ubyte)[]) literal, ['0']).shouldBeTrue;
    }
}

@("shall not treat non-zero C++23 integer literals as equivalent to plain 0")
@safe unittest {
    import unit_threaded : shouldBeFalse;

    foreach (literal; [
        "1z", "1Z", "01z", "01uZ", "0x1z", "0x1UZ", "0b1z", "0b1UZ",
        "1'000z"
    ]) {
        isEquivalentZeroIntMutant(cast(const(ubyte)[]) literal, ['0']).shouldBeFalse;
    }
}

@("shall treat zero-valued C++17 and newer floating literals as equivalent to plain 0.0")
@safe unittest {
    import unit_threaded : shouldBeTrue;

    foreach (literal; [
        "0.0f16", "0.0F16", "0.0bf16", "0.0BF16", "0.0f32", "0.0F32", "0.0f64",
        "0.0F64", "0.0f128", "0.0F128", "0x0p0", "0X0P-1", "0x0.p0", "0x.0p+0",
        "0x0.0p1", "0x0'0.0'0p1'0L", "0x0p0F16"
    ]) {
        isEquivalentZeroFloatMutant(cast(const(ubyte)[]) literal, ['0', '.', '0'])
            .shouldBeTrue;
    }
}

@("shall not treat non-zero C++17 and newer floating literals as equivalent to plain 0.0")
@safe unittest {
    import unit_threaded : shouldBeFalse;

    foreach (literal; [
        "1.0F16", "42.0f16", "1.0bf16", "1.0BF16", "1.0f32", "1.0F32", "1.0f64",
        "1.0F64", "1.0f128", "1.0F128", "0x1p0", "0x0.1p0", "0x1.0p-1",
        "0x1'0.0p-4F"
    ]) {
        isEquivalentZeroFloatMutant(cast(const(ubyte)[]) literal, ['0', '.', '0'])
            .shouldBeFalse;
    }
}
