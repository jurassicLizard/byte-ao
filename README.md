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
- **Bitwise Operations**: XOR operations with proper alignment semantics
- **Secure Memory Handling**: Methods to securely erase sensitive data from memory
- **Modern C++ Design**: Uses move semantics, RAII principles, and C++17 features
- **Conversion Utilities**: Easily convert between byte arrays and numeric types
- **Comprehensive Test Suite**: Thoroughly tested core functionality

## Example Usage

### General Byte array operations

```cpp
#include "jlizard/byte_array.h"
#include "jlizard/byte_array_ops.h"
using namespace jlizard;


void example() {
  
    // Create a ByteArray from a hex string
    ByteArray key("deadbeef");
    
    // Create from an initializer list
    ByteArray iv({0x01, 0x02, 0x03, 0x04});
    
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
    // When done with sensitive data, wipe it securely
     key.secure_wipe();

}


```
### Example use with OpenSSL (Optional)

ByteArrayOps can be used alongside other cryptographic libraries like OpenSSL:

- **Easy Conversion**: Efficiently convert between ByteArray objects and OpenSSL's buffer formats
- **Consistent Memory Handling**: Apply secure memory practices when interfacing with OpenSSL functions
- **Simplified API**: Wrapper functions to reduce the complexity of common OpenSSL operations

*Note: Examples are WIP and will be added soon 

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

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## WIPs and TODOs
- Add additional bitwise operations (AND, OR, NAND, one's complement, two's complement)
- Add bit manipulation operations (bit shifting, rotation, bit extraction, bit counting)
- Add comparison operations (equality, less than, greater than)
- Add endianness conversion utilities (big-endian to little-endian and vice versa)
- Add array manipulation functions (concatenation, splitting, slicing)
- Add search and pattern matching capabilities
- Add serialization/deserialization support
- Add encoding/decoding utilities (Base64, hex)
- Add SIMD acceleration for performance-critical operations
- Add parallelization (Multi-Threading)
- Add comprehensive benchmarking
- Expand platform-specific secure memory handling
- Add testing for secure wipe procedures