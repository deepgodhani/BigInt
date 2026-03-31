# BigInt — Arbitrary-Precision Integer Arithmetic in C++

A header-style C++ class that breaks the 64-bit integer ceiling, letting you add, subtract, and multiply numbers of any size with correct sign handling.

---

## Demo

```
Input:
  a = 987654321098765432109876543210
  b = 120

Output (a - b):
  987654321098765432109876543090
```

```
ASCII data-flow for multiplication (Karatsuba):

  num1 ──┬── high1 ──┐           ┌── z2 = karatsuba(high1, high2)
         │            ├─ recurse ─┤
         └── low1  ──┘           ├── z1 = karatsuba(low1+high1, low2+high2)
                                  │
  num2 ──┬── high2 ──┐           └── z0 = karatsuba(low1, low2)
         │            │
         └── low2  ──┘    result = z2·B² + (z1−z2−z0)·B + z0
```

---

## Why I Built This

Standard C++ integer types cap out at 64-bit values (~9.2 × 10¹⁸). Many competitive-programming and cryptographic problems—factorials of large numbers, RSA key generation, precise scientific computation—need integers that are hundreds or thousands of digits long. This project implements that capability from scratch, focusing on algorithmic efficiency rather than just correctness.

---

## Key Technical Highlights

- **Karatsuba multiplication** — reduces digit-by-digit work from O(n²) to O(n^1.585) by recursively splitting each operand into two halves and reusing intermediate products.
- **Little-endian digit storage** — digits are stored least-significant-first in a `std::vector<int>`, making carry propagation and index arithmetic straightforward.
- **Full sign arithmetic** — a single `isNeg` boolean combined with absolute-value helpers (`addAbsValues`, `subAbsValues`) handles all four sign combinations for `+` and `−` without code duplication.
- **Automatic zero trimming** — `trimLeadingZeros()` is called after every operation, keeping the internal representation canonical and comparisons correct.
- **Multiple construction paths** — accepts a `std::string`, a `long long`, or a raw `std::vector<int>`, making it easy to integrate into existing code.

---

## Tech Stack

| Layer | Technology |
|---|---|
| Language | C++ 17 |
| Standard library | `<bits/stdc++.h>` (GCC) |
| Digit storage | `std::vector<int>` |
| Core algorithm | Karatsuba divide-and-conquer |
| Build tool | Any C++ compiler (g++, clang++) |

---

## How to Run Locally

### Prerequisites

- GCC 9+ **or** Clang 10+ with C++17 support

### Build & Run

```bash
# Clone the repository
git clone https://github.com/deepgodhani/BigInt.git
cd BigInt

# Compile
g++ -std=c++17 -O2 -o bigint BIGINT.cpp

# Run
./bigint
```

The `main()` function at the bottom of `BIGINT.cpp` serves as a quick smoke test. Modify it to exercise whichever operations you need:

```cpp
bigInt a("123456789012345678901234567890");
bigInt b("987654321098765432109876543210");
cout << a + b << "\n";   // addition
cout << b - a << "\n";   // subtraction
cout << a * b << "\n";   // Karatsuba multiplication
```

---

## Architecture Overview

```
bigInt
├── Storage
│   ├── vector<int> num   — digits, least-significant first (base 10)
│   └── bool isNeg        — sign flag
│
├── Constructors
│   ├── bigInt(string)    — parse decimal string, handle leading '-'
│   ├── bigInt(long long) — extract digits via repeated modulo
│   └── bigInt(vector)    — direct digit-array construction
│
├── Arithmetic operators  (+, -, *)
│   ├── operator+  — delegates to addAbsValues / subAbsValues based on sign
│   ├── operator-  — mirrors operator+ with flipped sign logic
│   └── operator*  — pads operands, calls karatsuba()
│
├── Core helpers
│   ├── karatsuba()          — recursive divide-and-conquer multiply
│   ├── longMultiplication() — O(n²) base case for small inputs
│   ├── addAbsValues()       — grade-school addition with carry
│   ├── subAbsValues()       — grade-school subtraction with borrow
│   ├── split() / shift()    — digit-array manipulation for Karatsuba
│   └── trimLeadingZeros()   — normalise representation after operations
│
├── Comparison operators  (>, <)
│   ├── isGreater() / isLesser() — magnitude comparison
│   └── sign-aware wrappers
│
└── I/O
    └── operator<<  — stream digits most-significant-first with optional '-'
```

---

## Known Limitations & Future Improvements

| Limitation | Potential Improvement |
|---|---|
| Base 10 digit storage is memory-heavy | Switch to base 10⁹ to store ~9 decimal digits per `int`, cutting vector size by ~9× |
| No division or modulo operator | Implement `operator/` and `operator%` (long division or Newton–Raphson) |
| No `==` / `!=` / `<=` / `>=` operators | Add the missing comparison operators for completeness |
| `multMat()` function is incomplete | Finish or remove the matrix-multiplication stub |
| No unit-test suite | Add a test harness (e.g., GoogleTest) with edge cases: zero, negative, single-digit, overflow boundary |
| Single-file implementation | Separate into `bigInt.h` / `bigInt.cpp` for reuse as a library |
| No exception handling | Validate input strings and throw on malformed input instead of silently misparsing |
