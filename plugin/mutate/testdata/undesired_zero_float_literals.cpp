double zero_float_literal_filter_cases(long double foo0) {
    // Non-zero floating-point literals: mutating to plain 0.0 is meaningful and should remain.
    if (foo0 == 1e0) {}
    if (foo0 == 1.) {}
    if (foo0 == .1) {}
    if (foo0 == 0.1) {}
    if (foo0 == 0.0001F) {}
    if (foo0 == 1e-5L) {}
    if (foo0 == 1.0f) {}
    if (foo0 == 1e0l) {}
    if (foo0 == 3.14159) {}

    // Zero-valued floating-point literals: mutating to plain 0.0 is equivalent and should be dropped.
    if (foo0 == 0e0) {}
    if (foo0 == 0E+10) {}
    if (foo0 == 0.) {}
    if (foo0 == .0) {}
    if (foo0 == 0.0) {}
    if (foo0 == 00.00) {}
    if (foo0 == 0.0e-1) {}
    if (foo0 == 0'0.0'0e1'0F) {}
    if (foo0 == 0e1L) {}
    if (foo0 == 0.0f) {}
    if (foo0 == 0e0l) {}

    return 0.0;
}
