# AGENTS.md

## Purpose
This file provides contributor guidance for coding agents working in this repository.
Use it to keep changes consistent, readable, and verifiable without blocking reasonable engineering judgment.

## How To Extend This File
Only add guidance here when it reflects recurring mistakes, workflow traps actually encountered, rules or expectations that someone could not safely guess without knowing this codebase, or expectations that differ from generic good engineering.

## Requirement levels
- **REQUIRED**: Must be satisfied before submission.
- **PREFERRED**: Follow by default; deviations are allowed with brief rationale in the final response.

## Language and style
- **PREFERRED**: Use idiomatic D and Phobos APIs over C-style patterns when practical.
- **PREFERRED**: Prefer standard library algorithms/ranges over manual loops when they improve clarity.
- **PREFERRED**: Keep helper scope tight. If a small helper is only used by one function, prefer a local function over a file-private helper.

## Runtime and process API checks
- **REQUIRED**: When changing behavior that depends on D runtime configuration, process spawning, or environment propagation, verify assumptions against primary documentation or the local Phobos/druntime sources for the toolchain in use before introducing or removing workarounds.
- **PREFERRED**: If behavior is unclear, validate it with the smallest practical repro/probe before relying on inference from larger tests.

## Terminology
- **PREFERRED**: Refer to files under `plugin/mutate/testdata` as **test data**, **sample inputs**, or **test input files**, not **fixtures**.
- **PREFERRED**: Reserve **fixture** for test-framework setup/teardown constructs to avoid ambiguity.

## Build/tooling requirements
- **REQUIRED**: Changes must compile with `ldc2` (directly or via the project’s normal build/test target) before submission.

## Plugin-specific guidance
- Plugins are written in D and use libclang to analyze C/C++ source code.

## Mutate plugin verification
- **PREFERRED**: When changing the `mutate` plugin, run during development when practical:
  - `cmake --build build-test --target mutate_unittest__run --target dextool_debug-mutate_integration__run --parallel`
- **PREFERRED**: Do not run multiple `cmake --build` commands concurrently against the same `build-test` directory. If the targets are run separately, run them sequentially.
- This helps catch regressions early in unit/integration coverage while iterating.

## Testing requirements for plugin features
- **REQUIRED**: New plugin features and behavior changes should be tested with **integration tests as the default** coverage for behavior.
- **PREFERRED**: Prefer exhaustive/variant-heavy validation in integration tests whenever practical.
- **PREFERRED**: Add unit tests only when integration tests cannot practically provide sufficient coverage (e.g., hard-to-reach internal branches or combinatorial edge cases).
- **REQUIRED**: Validation of plugin behavior must focus on analyzed **C/C++ inputs** (the tool’s target), not D source behavior alone.
- **PREFERRED**: When adding or updating mutate integration/behavior tests, compare with existing tests under `plugin/mutate/test` and test data under `plugin/mutate/testdata`.
- **PREFERRED**: Use checks from **unit_threaded**, not plain asserts. It gives better failure output.  
  (See `./vendor/unit-threaded/subpackages/assertions/source/unit_threaded/assertions.d`.)
- **PREFERRED**: For mutate integration tests, prefer assertions on user-visible behavior. When feasible, verify normal report outputs (console, JSON, HTML) rather than only internal artifacts such as the SQLite database.
