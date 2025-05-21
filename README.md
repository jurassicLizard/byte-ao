# Byte-Array Operations Library

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE.md)
![GitHub Issues](https://img.shields.io/github/issues/jurassicLizard/byte-ao)

A high-level C++ abstraction for byte array operations designed while exploring cryptographic engineering concepts from Bruce Schneier, Ferguson and Kohno's "Cryptography Engineering" book.

<!-- TOC -->
* [Byte-Array Operations Library](#byte-array-operations-library)
  * [Overview](#overview)
  * [Features](#features)
  * [Example Usage](#example-usage)
    * [General Byte array operations](#general-byte-array-operations)
    * [Example use with OpenSSL (Optional)](#example-use-with-openssl-optional)
  * [Building and Usage](#building-and-usage)
  * [Testing](#testing)
  * [Requirements](#requirements)
  * [Security Considerations](#security-considerations)
  * [License](#license)
  * [WIPs and TODOs](#wips-and-todos)
<!-- TOC -->


## Overview

The Byte-Array Operations Library provides a clean, safe interface for working with byte arrays in cryptographic applications. This library offers:

- Safe byte array construction from various sources (hex strings, vectors, initializer lists)
- Secure memory wiping for sensitive data
- Efficient bitwise operations with right-alignment semantics
- Conversion between byte arrays and numeric types
- RAII-compliant resource management
- Protection against common memory handling errors

## Features

- **Multiple Construction Methods**: Create byte arrays from hex strings, raw bytes, or numeric values
- **Bitwise Operations**: XOR and complement operations with proper alignment semantics
- **Secure Memory Handling**: Methods to securely erase sensitive data from memory
- **Modern C++ Design**: Uses move semantics, RAII principles, and C++17 features
- **Conversion Utilities**: Easily convert between byte arrays and numeric types
- **Comprehensive Test Suite**: Thoroughly tested core functionality

## Example Usage

### General Byte array operations

```cpp
#include "jlizard/byte_array.h"

using namespace jlizard;

void example() {
    // Create a ByteArray from a hex string
    ByteArray key("deadbeef");
    
    // Create from an initializer list
    ByteArray iv({0x01, 0x02, 0x03, 0x04});
    
    // Use initializer list for defining byte sequences
    ByteArray bytes = {0xAA, 0xBB, 0xCC, 0xDD};  // Uses initializer_list constructor
    
    // Create from a single byte (requires explicit cast)
    ByteArray single_byte(static_cast<unsigned char>(0x42));
    // Or create via initializer list
    ByteArray another_single_byte = {0x42};
    
    // Create from uint64_t values
    // INCORRECT: ByteArray small_uint = 42;  // No longer allowed - prevents accidental truncation
    // CORRECT:
    ByteArray small_uint = {42};  // Using initializer list for a single value
    // Or explicitly create from uint64_t
    ByteArray large_uint = ByteArray::create_from_uint64(0x1122334455667788);  // Full 8-byte representation
    
    // Fixed-size constructor with fill value
    ByteArray filled_array(5, 0xAA);  // Creates a 5-byte array filled with 0xAA
    
    // XOR operation
    ByteArray result = key ^ iv;
    
    // Convert to uint64_t
    uint64_t value = result.as_64bit_uint();
    
    // Accessing raw data and size 
    const unsigned char* raw_data = result.data(); 
    size_t data_length = result.size();
    
    // Iterate through the bytes 
    for (auto byte : result) 
    { 
        // Process each byte
    }
    
    // Access bytes using subscript operator with bounds checking
    try {
        unsigned char first_byte = result[0];  // Safe access with bounds checking
        // Trying to access an out-of-bounds index would throw std::out_of_range
        // unsigned char invalid = result[1000];  // This would throw if result.size() < 1001
    } catch (const std::out_of_range& e) {
        // Handle the exception
    }
    
    // Using at() method for bounds-checked access
    try {
        unsigned char third_byte = result.at(2);  // Safe access with bounds checking
    } catch (const std::out_of_range& e) {
        // Handle the exception
    }
    
    // Single-byte operations (requires explicit cast)
    ByteArray modified = result;
    // INCORRECT: modified ^= 0xFF;  // No longer allowed
    // CORRECT:
    modified ^= static_cast<unsigned char>(0xFF);  // XOR the last byte with 0xFF
    
    // Reassignment with initializer list (still works)
    modified = {0x11, 0x22, 0x33};  // Replace contents with new values
    
    // One's complement operation (bitwise NOT)
    ByteArray original({0xAA, 0xBB, 0xCC});
    ByteArray complemented = ~original;  // Creates a new ByteArray with flipped bits
    // original remains {0xAA, 0xBB, 0xCC}
    // complemented is {0x55, 0x44, 0x33}

    // Note: There is no in-place complement operator - the ~ operator always returns a new ByteArray
    // To perform an in-place complement, use assignment:
    ByteArray to_modify({0xFF, 0x00});
    to_modify = ~to_modify;  // Now contains {0x00, 0xFF}
    
    // Copy and move semantics
    ByteArray copy = key;  // Copy constructor
    ByteArray moved = std::move(copy);  // Move constructor
    
    ByteArray assign_to;
    assign_to = key;  // Copy assignment
    
    ByteArray move_to;
    move_to = std::move(moved);  // Move assignment
    
    // When done with sensitive data, wipe it securely
    key.secure_wipe();
    
        // NEW v0.0.4 features
    
    // Clear a byte array
    ByteArray to_clear = {0x01, 0x02, 0x03};
    to_clear.clear();  // Now empty with size 0
    
    // Resize a byte array
    ByteArray to_resize = {0xAA, 0xBB};
    to_resize.resize(4);  // Now has size 4 with value {0xAA, 0xBB, 0x00, 0x00}
    to_resize.resize(1);  // Now has size 1 with value {0xAA}
    
    // Concatenate byte arrays (multiple methods)
    ByteArray first = {0x01, 0x02};
    ByteArray second = {0x03, 0x04};
    
    // Method 1: In-place concatenation with another ByteArray
    ByteArray result = first;
    result.concat(second);  // Now contains {0x01, 0x02, 0x03, 0x04}
    
    // Method 2: Create a new ByteArray from concatenation
    ByteArray concat_result = first.concat_copy(second);  // Contains {0x01, 0x02, 0x03, 0x04}
    
    // Method 3: Static concatenation of multiple ByteArrays
    ByteArray third = {0x05, 0x06};
    ByteArray combined = ByteArray::concat_and_create({first, second, third});
    // combined now contains {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}
    
    // Generate random bytes using PRNG
    ByteArray random_bytes = ByteArray::create_from_prng(16);  // Create a 16-byte random array
    // There's a 1MB limit on the size of random byte arrays
    try {
        // This would throw an exception
        // ByteArray too_large = ByteArray::create_from_prng(2 * 1024 * 1024);
    } catch (const std::invalid_argument& e) {
        // Handle exception
    }
}
```

### Example use with OpenSSL (Optional)

ByteArrayOps can be used alongside other cryptographic libraries like OpenSSL:

- **Easy Conversion**: Efficiently convert between ByteArray objects and OpenSSL's buffer formats
- **Consistent Memory Handling**: Apply secure memory practices when interfacing with OpenSSL functions
- **Simplified API**: Wrapper functions to reduce the complexity of common OpenSSL operations

```cpp
#include "jlizard/byte_array.h"
#include "jlizard/crypto_handler.h"

using namespace jlizard;

void openssl_decryption_example() {
    // Create a 256-bit key (32 bytes)
    ByteArray key({0x80, 0x00, 0x00, 0x00, 0x00, 
                   0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01});

    // A 128-bit IV (16 bytes)
    ByteArray iv({0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00});

    // Encrypted message to be decrypted
    ByteArray ciphertext = {0x53, 0x9B, 0x33, 0x3b, 0x39,
                            0x70, 0x6d, 0x14, 0x90, 0x28,
                            0xcf, 0xe1, 0xd9, 0xd4,
                            0xa4, 0x07};

    // Initialize output buffer for plaintext
    ByteArray plaintext(16, 0x00);  // 16 bytes initialized to zero
    
    // Create a crypto handler for AES-256-CBC
    CryptoHandler ch(EVP_aes_256_cbc());
    
    // Decrypt the ciphertext using OpenSSL's EVP interface (inside decrypt function)
    int decrypted_len = ch.decrypt(
        ciphertext.data(), ciphertext.size(),
        key.data(), iv.data(),
        plaintext.data()
    );
    
    // plaintext now contains the decrypted data
    // Use BIO_dump_fp from the openssl library to print the result
    // BIO_dump_fp(stdout, (const char *)plaintext.data(), decrypted_len);
    
    // Don't forget to wipe sensitive data when done
    key.secure_wipe();
    iv.secure_wipe();
    plaintext.secure_wipe();
}
```

### Other Examples

Check [Unit Tests](tests/unit_tests.cpp) that contain ample examples on the usage of this library

## Building and Usage

This library is designed to be included in other CMake projects. You can integrate it using one of these methods:

### Option 1: Using add_subdirectory()

```cmake
# In your project's CMakeLists.txt
add_subdirectory(path/to/byte_array_ops)
# Link your target with the library
target_link_libraries(your_target PRIVATE jlizard::byte-ao)
``` 

### Option 2: Using FetchContent

```cmake
include(FetchContent)

# Declare the byte-ao project - note that the name must be a valid variable name
# so we use byte_ao as the content name but it will fetch from byte-ao repository
FetchContent_Declare(
        byte_ao
        GIT_REPOSITORY https://github.com/jurassicLizard/byte-ao.git
        GIT_TAG master
        # uncomment the following to always force a synchronization with upstream without having to remove the build directory
        # UPDATE_COMMAND ${GIT_EXECUTABLE} pull

)

# Make it available
FetchContent_MakeAvailable(byte_ao)

# Your existing project configuration
# ...

# When linking, use the full namespaced target
target_link_libraries(your_target PRIVATE jlizard::byte-ao)

``` 

### Standalone building (for testing or development):
```bash
mkdir build && cd build 
cmake .. 
make
``` 

## Testing

The library comes with a comprehensive test suite. To run the tests:

```bash 
mkdir build && cd build
cmake ..
make 
ctest --output-on-failure
``` 

Tests validate core functionality including byte array construction, bitwise operations, memory handling, and conversion utilities.

**Note on Secure Wipe Testing:** The secure memory wiping procedures, while implemented, are not yet thoroughly tested with unit tests. This functionality should be considered a work in progress from a verification standpoint.


## Requirements
- C++20 compatible compiler
- CMake 3.31 or higher

## Important Implementation Notes

### Endianness Considerations

In cryptographic applications, endianness matters when converting between byte arrays and multi-byte values (like `uint32_t` or `uint64_t`). Many cryptographic algorithms specify operations in big-endian format (network byte order), while most modern CPUs use little-endian format internally.

This library provides consistent endianness handling:

- When converting from a hex string to a byte array, the bytes are stored in the order they appear in the string
- The `as_64bit_uint()` method interprets bytes in big-endian order, which is the conventional format in cryptographic contexts
- For interoperability with different systems, explicit endianness conversion functions are planned in future updates

When interfacing with platform-specific APIs or libraries that assume different endianness, be careful to apply appropriate conversions.

## Security Considerations
This library includes functionality to securely erase sensitive data from memory, but offers no guarantees against sophisticated memory forensic techniques or side-channel attacks. Always follow security best practices when handling cryptographic material.

**Note on Secure Wipe Testing:** The secure memory wiping procedures, while implemented, are not yet thoroughly tested with unit tests. This functionality should be considered a work in progress from a verification standpoint.

## Changelog

For a complete list of changes and version history, see the [CHANGELOG.md](CHANGELOG.md) file.

## License
This project is licensed under the MIT License—see the [LICENSE.md](LICENSE.md) file for details.

## WIPs and TODOs
- Add additional bitwise operations (AND, OR, NAND, two's complement)
- ✅ Add one's complement operation
- Add bit manipulation operations (bit shifting, rotation, bit extraction, bit counting)
- Add comparison operations (equality, less than, greater than)
- Add endianness conversion utilities (big-endian to little-endian and vice versa)
- ✅ Add array manipulation functions (concatenation, splitting, slicing) (Partial: concatenation implemented)
- Add search and pattern matching capabilities
- Add serialization/deserialization support
- Add encoding/decoding utilities (Base64, hex)
- Add SIMD acceleration for performance-critical operations
- Add parallelization (Multi-Threading)
- Add comprehensive benchmarking
- Expand platform-specific secure memory handling
- Add testing for secure wipe procedures
- ✅ Add PRNG-based byte array generation