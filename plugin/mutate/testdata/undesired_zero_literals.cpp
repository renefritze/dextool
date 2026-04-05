int zero_literal_filter_cases(unsigned int foo0) {
    // Non-zero integer literals: mutating to plain 0 is meaningful and should remain.
    if (foo0 == 01u) {}
    if (foo0 == 0001) {}
    if (foo0 == 0x1) {}
    if (foo0 == 0x0001ULL) {}
    if (foo0 == 052) {}
    if (foo0 == 0b1) {}
    if (foo0 == 0b0001) {}
    if (foo0 == 0xFFu) {}

    // Zero-valued integer literals: mutating to plain 0 is equivalent and should be dropped.
    if (foo0 == 0u) {}
    if (foo0 == 0U) {}
    if (foo0 == 0LL) {}
    if (foo0 == 00) {}
    if (foo0 == 0x0ULL) {}
    if (foo0 == 0x00) {}
    if (foo0 == 0x0'000'000) {}
    if (foo0 == 0b00) {}
    if (foo0 == 0B0000ULL) {}
    if (foo0 == 0'0'0'0) {}
    if (foo0 == 0'000'000'000llu) {}

    return 0;
}
