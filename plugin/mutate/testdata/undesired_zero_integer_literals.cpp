int zero_literal_filter_cases(unsigned long long foo0) {
    // Non-zero integer literals: mutating to plain 0 is meaningful and should remain.
    if (foo0 == 1) {}
    if (foo0 == 01) {}
    if (foo0 == 01u) {}
    if (foo0 == 0001) {}
    if (foo0 == 0x1) {}
    if (foo0 == 0x0001ULL) {}
    if (foo0 == 0X2A) {}
    if (foo0 == 0b1) {}
    if (foo0 == 0b0001) {}
    if (foo0 == 0b101010) {}
    if (foo0 == 052) {}
    if (foo0 == 0x2a) {}
    if (foo0 == 0xFFu) {}
    if (foo0 == 18'446'744'073'709'550'592llu) {}
    if (foo0 == 1844'6744'0737'0955'0592uLL) {}
    if (foo0 == 184467'440737'0'95505'92LLU) {}

    // Zero-valued integer literals: mutating to plain 0 is equivalent and should be dropped.
    if (foo0 == 0u) {}
    if (foo0 == 0U) {}
    if (foo0 == 0l) {}
    if (foo0 == 0L) {}
    if (foo0 == 0ll) {}
    if (foo0 == 0LL) {}
    if (foo0 == 0ul) {}
    if (foo0 == 0uL) {}
    if (foo0 == 0Ul) {}
    if (foo0 == 0UL) {}
    if (foo0 == 0lu) {}
    if (foo0 == 0lU) {}
    if (foo0 == 0Lu) {}
    if (foo0 == 0LU) {}
    if (foo0 == 0ull) {}
    if (foo0 == 0uLL) {}
    if (foo0 == 0Ull) {}
    if (foo0 == 0ULL) {}
    if (foo0 == 0llu) {}
    if (foo0 == 0llU) {}
    if (foo0 == 0LLu) {}
    if (foo0 == 0LLU) {}
    if (foo0 == 00) {}
    if (foo0 == 000) {}
    if (foo0 == 0x0) {}
    if (foo0 == 0x0ULL) {}
    if (foo0 == 0x00) {}
    if (foo0 == 0x00ULL) {}
    if (foo0 == 0x0'000'000) {}
    if (foo0 == 0b0) {}
    if (foo0 == 0B0) {}
    if (foo0 == 0b00) {}
    if (foo0 == 0B0000ULL) {}
    if (foo0 == 0000u) {}
    if (foo0 == 0x0000u) {}
    if (foo0 == 0X0000ULL) {}
    if (foo0 == 0'0'0'0) {}
    if (foo0 == 0'000'000'000) {}
    if (foo0 == 0'000'000'000llu) {}

    return 0;
}
