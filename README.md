# Byte-Array Operations Library

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE.md)
![GitHub Issues](https://img.shields.io/github/issues/jurassicLizard/byte-ao)


> ⚠️ **MEMORY SECURITY NOTE**: 
> While ByteArray provides a secure_erase method for purging data from memory, no guarantees can be provided as to the reliable purging of sensitive data,
> we recommend using more formally audited 
> functions from libraries like OpenSSL for example the OpenSSL_cleanse() function which has undergone extensive security review
> and provides more reliable and deterministic memory clearing.


A high-level C++ abstraction for byte array operations designed while exploring cryptographic engineering concepts from Bruce Schneier, Ferguson and Kohno's "Cryptography Engineering" book.

<!-- TOC -->
* [Byte-Array Operations Library](#byte-array-operations-library)
  * [Overview](#overview)
  * [Features](#features)
  * [Example Usage](#example-usage)
    * [Basic Construction and creation](#basic-construction-and-creation)
    * [Static Factory Methods](#static-factory-methods)
    * [Data Access and Iteration](#data-access-and-iteration)
    * [Bitwise Operations](#bitwise-operations)
    * [Concatenation Operations](#concatenation-operations)
    * [Resizing and Memory Management](#resizing-and-memory-management)
    * [Conversion and Comparison](#conversion-and-comparison)
    * [Copy/Move Semantics and Security](#copymove-semantics-and-security)
    * [Partial Copy Constructor and Advanced Features](#partial-copy-constructor-and-advanced-features)
    * [Example use with OpenSSL (Optional)](#example-use-with-openssl-optional)
    * [Other Examples](#other-examples)
    * [Available Static Factory Methods](#available-static-factory-methods)
  * [Building and Usage](#building-and-usage)
    * [Option 1: Using add_subdirectory()](#option-1-using-add_subdirectory)
    * [Option 2: Using FetchContent](#option-2-using-fetchcontent)
    * [Standalone building (for testing or development):](#standalone-building-for-testing-or-development)
  * [Testing](#testing)
  * [Requirements](#requirements)
  * [Important Implementation Notes](#important-implementation-notes)
    * [Security Considerations](#security-considerations)
    * [Endianness Considerations](#endianness-considerations)
    * [Padding Direction and Resize Behavior](#padding-direction-and-resize-behavior)
  * [Changelog](#changelog)
  * [License](#license)
  * [WIPs and TODOs](#wips-and-todos)
<!-- TOC -->


## Overview

The Byte-Array Operations Library provides a clean, safe interface for working with byte arrays in cryptographic applications. This library offers:

- Safe byte array construction from various sources (hex strings, vectors, initializer lists)
- Secure memory wiping for sensitive data
- Efficient bitwise operations with right-alignment semantics
- Conversion between byte arrays and numeric types
- Flexible resizing options with security considerations
- RAII-compliant resource management
- Protection against common memory handling errors

## Features

- **Multiple Construction Methods**: Create byte arrays from hex strings, raw bytes, numeric values, or strings
- **Partial Copy Constructor**: Create byte arrays from portions of existing arrays with padding control
- **Bitwise Operations**: XOR and complement operations with proper alignment semantics
- **Secure Memory Handling**: Methods to securely erase sensitive data from memory
- **Flexible Resizing**: Resize byte arrays with configurable padding direction, secure purging, and warning options
- **String Conversion**: Convert to/from hex strings and create from string views
- **Memory Preallocation**: Create byte arrays with reserved capacity for performance optimization
- **Modern C++ Design**: Uses move semantics, RAII principles, and C++17 features
- **Conversion Utilities**: Easily convert between byte arrays and numeric types
- **Comprehensive Test Suite**: Thoroughly tested core functionality

## Example Usage

### Basic Construction and creation

```cpp
#include "jlizard/byte_array.h"

using namespace jlizard;

void basic_construction_examples() {
    // Create a ByteArray from a hex string
    ByteArray key("deadbeef");
    
    // Create from an initializer list
    ByteArray iv({0x01, 0x02, 0x03, 0x04});
    
    // Use initializer list for defining byte sequences
    ByteArray bytes = {0xAA, 0xBB, 0xCC, 0xDD};
    
    // Create from a single byte (requires explicit cast)
    ByteArray single_byte(static_cast<unsigned char>(0x42));
    // Or create via initializer list
    ByteArray another_single_byte = {0x42};
    
    // Fixed-size constructor with fill value
    ByteArray filled_array(5, 0xAA);  // Creates a 5-byte array filled with 0xAA
}
```

### Static Factory Methods

```cpp
void factory_methods_examples() {
    // Create from uint64_t values
    ByteArray small_uint = {42};  // Using initializer list for a single value
    // Or explicitly create from uint64_t
    ByteArray large_uint = ByteArray::create_from_uint64(0x1122334455667788);
    
    // Create from string data
    ByteArray from_string = ByteArray::create_from_string("Hello, World!");
    
    // Generate random bytes using PRNG
    ByteArray random_bytes = ByteArray::create_from_prng(16);  // Create a 16-byte random array
    // Note: There's a 1MB limit on the size of random byte arrays
    
    // Create with pre-allocated capacity for performance
    ByteArray prealloc = ByteArray::create_with_prealloc(1024);  // Reserve 1024 bytes
}
```

### Data Access and Iteration

```cpp
void data_access_examples() {
    ByteArray data = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    // Accessing raw data and size 
    const unsigned char* raw_data = data.data(); 
    size_t data_length = data.size();
    
    // Iterate through the bytes 
    for (auto byte : data) {
        // Process each byte
    }
    
    // Access bytes using subscript operator with bounds checking
    try {
        unsigned char first_byte = data[0];  // Safe access with bounds checking
        // Trying to access an out-of-bounds index would throw std::out_of_range
    } catch (const std::out_of_range& e) {
        // Handle the exception
    }
    
    // Using at() method for bounds-checked access
    try {
        unsigned char third_byte = data.at(2);  // Safe access with bounds checking
    } catch (const std::out_of_range& e) {
        // Handle the exception
    }
}
```

### Bitwise Operations

```cpp
void bitwise_operations_examples() {
    ByteArray key("deadbeef");
    ByteArray iv({0x01, 0x02, 0x03, 0x04});
    
    // XOR operations
    ByteArray result = key ^ iv;  // XOR two ByteArrays
    
    ByteArray modified = result;
    modified ^= static_cast<unsigned char>(0xFF);  // XOR with single byte
    
    // One's complement operation (bitwise NOT)
    ByteArray original({0xAA, 0xBB, 0xCC});
    ByteArray complemented = ~original;  // Creates a new ByteArray with flipped bits
    // original remains {0xAA, 0xBB, 0xCC}
    // complemented is {0x55, 0x44, 0x33}

    // Note: There is no in-place complement operator
    // To perform an in-place complement, use assignment:
    ByteArray to_modify({0xFF, 0x00});
    to_modify = ~to_modify;  // Now contains {0x00, 0xFF}
}
```

### Concatenation Operations

```cpp
void concatenation_examples() {
    ByteArray first = {0x01, 0x02};
    ByteArray second = {0x03, 0x04};
    ByteArray third = {0x05, 0x06};
    
    // Method 1: In-place concatenation
    ByteArray result1 = first;
    result1.concat(second);  // Now contains {0x01, 0x02, 0x03, 0x04}
    
    // Method 2: Create a new ByteArray from concatenation
    ByteArray result2 = first.concat_copy(second);  // Contains {0x01, 0x02, 0x03, 0x04}
    
    // Method 3: Static concatenation of multiple ByteArrays
    ByteArray combined = ByteArray::concat_and_create({first, second, third});
    // combined now contains {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}
}
```

### Resizing and Memory Management

```cpp
void resize_examples() {
    // Basic resize operations
    ByteArray data = {0xAA, 0xBB};
    data.resize(4);  // Now has size 4 with value {0xAA, 0xBB, 0x00, 0x00}
    data.resize(1);  // Now has size 1 with value {0xAA}
    
    // Resize with padding direction control
    ByteArray msb_data = {0x01, 0x02, 0x03};
    msb_data.resize(5, EZeroPadDir::MSB_PAD);  // {0x00, 0x00, 0x01, 0x02, 0x03}
    
    ByteArray lsb_data = {0x01, 0x02, 0x03};
    lsb_data.resize(5, EZeroPadDir::LSB_PAD);  // {0x01, 0x02, 0x03, 0x00, 0x00}
    
    // Secure resize operations for sensitive data
    ByteArray sensitive_data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    sensitive_data.resize(4, true);  // Securely wipes remaining data
    
    // Resize with custom security options
    ByteArray secret_data = {0x01, 0x02, 0x03, 0x04, 0x05};
    secret_data.resize(2, true, false);  // Secure wipe but no warning
    
    // Clear a byte array
    ByteArray to_clear = {0x01, 0x02, 0x03};
    to_clear.clear();  // Now empty with size 0
}
```

### Conversion and Comparison

```cpp
void conversion_comparison_examples() {
    ByteArray data = {0x12, 0x34, 0x56, 0x78};
    
    // Convert to uint64_t (throws if array is larger than 8 bytes)
    uint64_t value = data.as_64bit_uint();
    
    // Convert to hex string
    std::string hex = data.as_hex_string();  // "12345678"
    
    // Equality comparison
    ByteArray a = {0x01, 0x02, 0x03};
    ByteArray b = {0x01, 0x02, 0x03};
    ByteArray c = {0x01, 0x02, 0x04};
    
    bool are_equal = (a == b);       // true
    bool are_different = (a == c);   // false
    
    // The equality operator first checks sizes then compares all elements
    ByteArray shorter = {0x01, 0x02};
    bool different_sizes = (a == shorter);  // false - different sizes
}
```

### Copy/Move Semantics and Security

```cpp
void memory_semantics_examples() {
    ByteArray key("deadbeef");
    
    // Copy and move semantics
    ByteArray copy = key;  // Copy constructor
    ByteArray moved = std::move(copy);  // Move constructor
    
    ByteArray assign_to;
    assign_to = key;  // Copy assignment
    
    ByteArray move_to;
    move_to = std::move(moved);  // Move assignment
    
    // Reassignment with initializer list
    assign_to = {0x11, 0x22, 0x33};  // Replace contents with new values
    
    // Secure memory handling
    // When done with sensitive data, wipe it securely
    key.secure_wipe();
    assign_to.secure_wipe();
}
```

### Partial Copy Constructor and Advanced Features

```cpp
void advanced_features_examples() {
    ByteArray source = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    // Partial copy with LSB padding (default)
    ByteArray partial_lsb(source, 3);  // {0x01, 0x02, 0x03}
    
    // Partial copy with MSB padding
    ByteArray partial_msb(source, 3, EZeroPadDir::MSB_PAD);  // {0x03, 0x04, 0x05}
    
    // Extend with padding
    ByteArray extended_lsb(source, 8);  // {0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00, 0x00}
    ByteArray extended_msb(source, 8, EZeroPadDir::MSB_PAD);  // {0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05}
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

### Available Static Factory Methods

The library provides several static factory methods for creating ByteArray objects:

- `ByteArray::create_from_uint64(uint64_t value)` - Creates from a 64-bit unsigned integer
- `ByteArray::create_from_string(std::string_view sv)` - Creates from string data
- `ByteArray::create_from_prng(size_t num_bytes)` - Creates with cryptographically random bytes (up to 1MB)
- `ByteArray::create_with_prealloc(size_t reserve_size)` - Creates with pre-allocated capacity
- `ByteArray::concat_and_create(std::initializer_list<ByteArray>)` - Creates by concatenating multiple arrays

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

### Security Considerations
This library includes functionality to securely erase sensitive data from memory, but offers no guarantees against sophisticated memory forensic techniques or side-channel attacks. Always follow security best practices when handling cryptographic material.

**Note on Secure Wipe Testing:** The secure memory wiping procedures, while implemented, are not yet thoroughly tested with unit tests. This functionality should be considered a work in progress from a verification standpoint.


### Endianness Considerations

In cryptographic applications, endianness matters when converting between byte arrays and multi-byte values (like `uint32_t` or `uint64_t`). Many cryptographic algorithms specify operations in big-endian format (network byte order), while most modern CPUs use little-endian format internally.

This library provides consistent endianness handling:

- When converting from a hex string to a byte array, the bytes are stored in the order they appear in the string
- The `as_64bit_uint()` method interprets bytes in big-endian order, which is the conventional format in cryptographic contexts
- For interoperability with different systems, explicit endianness conversion functions are planned in future updates

When interfacing with platform-specific APIs or libraries that assume different endianness, be careful to apply appropriate conversions.

### Padding Direction and Resize Behavior

The library supports two padding directions controlled by the `EZeroPadDir` enum:

- **LSB_PAD (Default)**: Preserves least significant bytes
  - When growing: adds zeros at the end
  - When shrinking: keeps leftmost bytes
- **MSB_PAD**: Preserves most significant bytes
  - When growing: adds zeros at the beginning
  - When shrinking: keeps rightmost bytes

The `resize()` method has two overloads:
1. `resize(size_t new_size, bool purge_before_resize, bool output_warning, EZeroPadDir zero_pad_dir)`
2. `resize(size_t new_size, EZeroPadDir zero_pad_dir, bool purge_before_resize, bool output_warning)` - Convenience overload with padding direction as second parameter

Both methods support secure memory wiping and optional security warnings when shrinking arrays.

## Changelog

For a complete list of changes and version history, see the [CHANGELOG.md](CHANGELOG.md) file.

## License
This project is licensed under the MIT License—see the [LICENSE.md](LICENSE.md) file for details.

## WIPs and TODOs
- Add additional bitwise operations (AND, OR, NAND, two's complement)
- ✅ Add one's complement operation
- Add bit manipulation operations (bit shifting, rotation, bit extraction, bit counting)
- ✅ Add comparison operations (equality, less than, greater than) (Partial: equality implemented)
- Add endianness conversion utilities (big-endian to little-endian and vice versa)
- ✅ Add array manipulation functions (concatenation, splitting, slicing) (Partial: concatenation implemented)
- ✅ Add secure resize functionality with optional purging and warnings
- Add search and pattern matching capabilities
- Add serialization/deserialization support
- Add encoding/decoding utilities (Base64, hex)
- Add SIMD acceleration for performance-critical operations
- Add parallelization (Multi-Threading)
- Add comprehensive benchmarking
- Expand platform-specific secure memory handling
- Add testing for secure wipe procedures
- ✅ Add PRNG-based byte array generation
