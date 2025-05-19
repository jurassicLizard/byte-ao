
/*
 * MIT License
 *
 * Copyright (c) 2025 Salem Bouraoui
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <array>

#include "jlizard/byte_array.h"
#include <cassert>
#include <iostream>

using namespace jlizard;

// Test hex string constructor
void test_hex_string_constructor() {
    ByteArray b1("fe81eabd5");
    assert(b1.size() == 5);
    assert(b1[0] == 0xfe);
    assert(b1[1] == 0x81);
    assert(b1[2] == 0xea);
    assert(b1[3] == 0xbd);
    assert(b1[4] == 0x05);

    ByteArray b2("abcdef0123");
    assert(b2.size() == 5);
    assert(b2[0] == 0xab);
    assert(b2[1] == 0xcd);
    assert(b2[2] == 0xef);
    assert(b2[3] == 0x01);
    assert(b2[4] == 0x23);
}

// Test vector constructor
void test_vector_constructor() {
    std::vector<unsigned char> vec{0xB8, 0xAB, 0xAF, 0xFF};
    const ByteArray b1(vec);
    assert(b1.size() == 4);
    assert(b1[0] == 0xB8);
    assert(b1[1] == 0xAB);
    assert(b1[2] == 0xAF);
    assert(b1[3] == 0xFF);

    std::vector<unsigned char> empty_vec;
    const ByteArray b2(empty_vec);
    assert(b2.size() == 0);

    std::vector<unsigned char> move_vec{0x01, 0x02, 0x03};
    ByteArray b3(std::move(move_vec));
    assert(b3.size() == 3);
    assert(b3[0] == 0x01);
    assert(b3[1] == 0x02);
    assert(b3[2] == 0x03);
    assert(move_vec.empty());
}

// Test initializer list constructor
void test_initializer_list_constructor() {
    const ByteArray b1({0xAF, 0xFF, 0xBA});
    assert(b1.size() == 3);
    assert(b1[0] == 0xAF);
    assert(b1[1] == 0xFF);
    assert(b1[2] == 0xBA);

    const ByteArray b2({});
    assert(b2.size() == 0);

    const ByteArray b3({0x42});
    assert(b3.size() == 1);
    assert(b3[0] == 0x42);
}


// Test single byte constructor
void test_single_byte_constructor() {
    // Test with explicit construction
    const ByteArray b1(static_cast<unsigned char>(0xAA));
    assert(b1.size() == 1);
    assert(b1[0] == 0xAA);

    const ByteArray b2(static_cast<unsigned char>(0x00));
    assert(b2.size() == 1);
    assert(b2[0] == 0x00);

    const ByteArray b3(static_cast<unsigned char>(0xFF));
    assert(b3.size() == 1);
    assert(b3[0] == 0xFF);

    // Test single byte init list conversion
    const ByteArray b4 = {0xBB};
    assert(b4.size() == 1);
    assert(b4[0] == 0xBB);

    // Test copy assignment with implicit conversion
    ByteArray b5; // NOLINT
    b5 = {0xCC};  // Should work with init-list constructor
    assert(b5.size() == 1);
    assert(b5[0] == 0xCC);
}

// Test XOR operators
void test_xor_operators() {
    // Test XOR with another ByteArray
    const ByteArray b1({0xAA, 0xBB, 0xCC});
    const ByteArray b2({0x55, 0x44, 0x33});
    ByteArray b3 = b1 ^ b2;

    assert(b3.size() == 3);
    assert(b3[0] == (0xAA ^ 0x55));
    assert(b3[1] == (0xBB ^ 0x44));
    assert(b3[2] == (0xCC ^ 0x33));

    // Test XOR with unequal lengths (right-aligned)
    const ByteArray b4({0xAA, 0xBB});
    const ByteArray b5({0x11, 0x22, 0x33});
    ByteArray b6 = b4 ^ b5;

    assert(b6.size() == 3);
    // With right alignment, b4 is aligned at the end of b6
    assert(b6[0] == 0x11);  // Only b5's first byte
    assert(b6[1] == (0xAA ^ 0x22));  // First byte of b4 XORed with second byte of b5
    assert(b6[2] == (0xBB ^ 0x33));  // Second byte of b4 XORed with third byte of b5

    // Test XOR with single byte (right-aligned)
    const ByteArray b7({0x12, 0x34, 0x56});
    ByteArray b8 = b7 ^ 0xFF;

    assert(b8.size() == 3);
    assert(b8[0] == 0x12);  // Unchanged
    assert(b8[1] == 0x34);  // Unchanged
    assert(b8[2] == (0x56 ^ 0xFF));  // Only last byte gets XORed

    // Test XOR-assignment
    ByteArray b9({0xA0, 0xB0, 0xC0});
    const ByteArray b10({0x0A, 0x0B, 0x0C});
    b9 ^= b10;

    assert(b9.size() == 3);
    assert(b9[0] == (0xA0 ^ 0x0A));
    assert(b9[1] == (0xB0 ^ 0x0B));
    assert(b9[2] == (0xC0 ^ 0x0C));

    // Test XOR-assignment with single byte (right-aligned)
    ByteArray b11({0x11, 0x22, 0x33});
    b11 ^= 0x01;

    assert(b11.size() == 3);
    assert(b11[0] == 0x11);  // Unchanged
    assert(b11[1] == 0x22);  // Unchanged
    assert(b11[2] == (0x33 ^ 0x01));  // Only last byte gets XORed
}
// Test copy and move semantics
void test_copy_move_semantics() {
    // Test copy constructor
    const ByteArray original({0x01, 0x02, 0x03});
    ByteArray copy = original; // NOLINT

    assert(copy.size() == original.size());
    assert(copy.data()[0] == 0x01);
    assert(copy.data()[1] == 0x02);
    assert(copy.data()[2] == 0x03);

    // Test copy assignment
    ByteArray destination({0xFF, 0xFF});
    destination = original;

    assert(destination.size() == original.size());
    assert(destination.data()[0] == 0x01);
    assert(destination.data()[1] == 0x02);
    assert(destination.data()[2] == 0x03);

    // Test move constructor
    ByteArray to_move({0xAA, 0xBB, 0xCC});
    ByteArray moved(std::move(to_move));

    assert(moved.size() == 3);
    assert(moved.data()[0] == 0xAA);
    assert(moved.data()[1] == 0xBB);
    assert(moved.data()[2] == 0xCC);

    // Test move assignment
    ByteArray to_move_assign({0xDD, 0xEE, 0xFF});
    ByteArray move_assigned;
    move_assigned = std::move(to_move_assign);

    assert(move_assigned.size() == 3);
    assert(move_assigned.data()[0] == 0xDD);
    assert(move_assigned.data()[1] == 0xEE);
    assert(move_assigned.data()[2] == 0xFF);
}

// Test iterators
void test_iterators() {
    ByteArray ba({0x01, 0x02, 0x03, 0x04});

    // Test begin/end iterators
    unsigned char sum = 0;
    for (auto it = ba.begin(); it != ba.end(); ++it) { // NOLINT
        sum += *it;
    }
    assert(sum == 0x0A);  // 0x01 + 0x02 + 0x03 + 0x04 = 0x0A

    // Test range-based for loop
    unsigned char prod = 1;
    for (auto byte : ba) {
        prod *= byte;
    }
    assert(prod == 0x18);  // 0x01 * 0x02 * 0x03 * 0x04 = 0x18
}

// Test secure erase functionality
void test_secure_erase() {
    // Test with a non-empty ByteArray
    ByteArray sensitive_data({0xAA, 0xBB, 0xCC, 0xDD, 0xEE});

    // Confirm data is as expected before wiping
    assert(sensitive_data.size() == 5);
    assert(sensitive_data.data()[0] == 0xAA);
    assert(sensitive_data.data()[1] == 0xBB);
    assert(sensitive_data.data()[2] == 0xCC);
    assert(sensitive_data.data()[3] == 0xDD);
    assert(sensitive_data.data()[4] == 0xEE);

    // Perform secure wipe
    bool wipe_result = sensitive_data.secure_wipe();

    // Verify wipe operation was successful
    assert(wipe_result);

    // Verify size is now 0 (should be emptied by the secure_zero_vector operation)
    assert(sensitive_data.size() == 0);

    // Test with an empty ByteArray
    ByteArray empty_data({});
    assert(empty_data.size() == 0);

    // Wiping an empty array should succeed without issues
    wipe_result = empty_data.secure_wipe();
    assert(wipe_result);
    assert(empty_data.size() == 0);

    // Test with single byte value
    ByteArray single_byte(static_cast<unsigned char>(0x42));
    assert(single_byte.size() == 1);
    assert(single_byte.data()[0] == 0x42);

    wipe_result = single_byte.secure_wipe();
    assert(wipe_result);
    assert(single_byte.size() == 0);

    // Test with hex string constructed ByteArray
    ByteArray hex_constructed("deadbeef");
    assert(hex_constructed.size() == 4);

    wipe_result = hex_constructed.secure_wipe();
    assert(wipe_result);
    assert(hex_constructed.size() == 0);

    // Test post-wipe operations to ensure object remains usable
    // Create a new ByteArray, wipe it, then add new data
    ByteArray reusable({0x11, 0x22, 0x33});
    assert(reusable.size() == 3);

    wipe_result = reusable.secure_wipe();
    assert(wipe_result);
    assert(reusable.size() == 0);

    // After wiping, the object should still be usable
    // We can't directly assign new data since there's no method for that,
    // but we can test that the object is in a valid state by assigning from another ByteArray
    const ByteArray new_data({0x44, 0x55, 0x66});
    reusable = new_data;

    assert(reusable.size() == 3);
    assert(reusable.data()[0] == 0x44);
    assert(reusable.data()[1] == 0x55);
    assert(reusable.data()[2] == 0x66);
}

// Test uint64_t constructor and as_64bit_uint conversion
void test_uint64_constructor_and_conversion() {
    // Test constructor with small value
    constexpr uint64_t small_value = 42;
    ByteArray ba1 = {small_value};

    // Check correct construction (should use big-endian byte order)
    assert(ba1.size() == 1);
    assert(ba1.data()[0] == 42);

    // Test round-trip conversion
    assert(ba1.as_64bit_uint() == small_value);

    // Test constructor with multi-byte value
    constexpr uint64_t multi_byte = 0x1122334455667788;
    ByteArray ba2 = ByteArray::create_from_uint64(multi_byte);

    // Check correct byte count and values (big-endian byte order)
    assert(ba2.size() == 8);
    assert(ba2.data()[0] == 0x11);
    assert(ba2.data()[1] == 0x22);
    assert(ba2.data()[2] == 0x33);
    assert(ba2.data()[3] == 0x44);
    assert(ba2.data()[4] == 0x55);
    assert(ba2.data()[5] == 0x66);
    assert(ba2.data()[6] == 0x77);
    assert(ba2.data()[7] == 0x88);

    // Test round-trip conversion
    assert(ba2.as_64bit_uint() == multi_byte);

    // Test with value that doesn't need all 8 bytes
    constexpr uint64_t medium_value = 0x112233;
    ByteArray ba3= ByteArray::create_from_uint64(medium_value);

    // Should only use 3 bytes
    assert(ba3.size() == 3);
    assert(ba3.data()[0] == 0x11);
    assert(ba3.data()[1] == 0x22);
    assert(ba3.data()[2] == 0x33);

    // Test round-trip conversion
    assert(ba3.as_64bit_uint() == medium_value);

    // Test with 0
    constexpr uint64_t zero_value = 0;
    ByteArray ba4 = ByteArray::create_from_uint64(zero_value);

    // Should produce a 1-byte array with value 0
    assert(ba4.size() == 1);
    assert(ba4.data()[0] == 0);
    assert(ba4.as_64bit_uint() == zero_value);
}

// Test as_64bit_uint exception handling
void test_as_64bit_uint_exceptions() {
    // Create a ByteArray with more than 8 bytes
    ByteArray large_array({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09});
    assert(large_array.size() == 9);  // More than 8 bytes

    // Attempting to convert should throw an exception
    bool exception_thrown = false;
    try {
        uint64_t value = large_array.as_64bit_uint();
        (void)value;  // Avoid unused variable warning
    } catch (const std::invalid_argument& e) {
        exception_thrown = true;
        assert(std::string(e.what()) == "Byte array is larger than 64-bit and cannot be represented as such");
    }
    assert(exception_thrown);

    // Test boundary case with exactly 8 bytes
    const ByteArray boundary_array({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08});
    assert(boundary_array.size() == 8);

    // This should not throw
    bool boundary_exception = false;
    try {
        uint64_t value = boundary_array.as_64bit_uint();
        (void)value;  // Avoid unused variable warning
    } catch (...) {
        boundary_exception = true;
    }
    assert(!boundary_exception);

    // Test conversion result with 8 bytes
    uint64_t expected = 0x0102030405060708;
    assert(boundary_array.as_64bit_uint() == expected);
}

// Test size and value constructor
void test_size_and_value_constructor() {
    // Test with non-zero value
    ByteArray b1(5, 0xAA);
    assert(b1.size() == 5);
    assert(b1.data()[0] == 0xAA);
    assert(b1.data()[1] == 0xAA);
    assert(b1.data()[2] == 0xAA);
    assert(b1.data()[3] == 0xAA);
    assert(b1.data()[4] == 0xAA);

    // Test with zero value
    ByteArray b2(3, 0x00);
    assert(b2.size() == 3);
    assert(b2.data()[0] == 0x00);
    assert(b2.data()[1] == 0x00);
    assert(b2.data()[2] == 0x00);

    // Test with zero size
    ByteArray b3(0, 0xFF);
    assert(b3.size() == 0);

    // Test with large size
    constexpr size_t large_size = 1000;
    ByteArray b4(large_size, 0xFF);
    assert(b4.size() == large_size);
    // Check some sample elements
    assert(b4.data()[0] == 0xFF);
    assert(b4.data()[large_size / 2] == 0xFF);
    assert(b4.data()[large_size - 1] == 0xFF);
}

// Test subscript operator bounds checking
void test_subscript_operator_bounds_checking() {
    // Create a test ByteArray with known values
    ByteArray ba({0x11, 0x22, 0x33, 0x44, 0x55});

    // Test valid access - should not throw
    bool exception_thrown = false;
    try {
        // Access each valid index
        assert(ba[0] == 0x11);
        assert(ba[1] == 0x22);
        assert(ba[2] == 0x33);
        assert(ba[3] == 0x44);
        assert(ba[4] == 0x55);
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    assert(!exception_thrown);

    // Test out-of-bounds access - should throw std::out_of_range
    exception_thrown = false;
    try {
        const unsigned char& value = ba[5]; // One past the end
        (void)value; // Prevent unused variable warning
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    assert(exception_thrown && "Out of range exception should be thrown");

    // Test access on empty ByteArray
    ByteArray empty_ba; // NOLINT
    exception_thrown = false;
    try {
        const unsigned char& value = empty_ba[0];
        (void)value; // Prevent unused variable warning
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    assert(exception_thrown && "Out of range exception should be thrown");

    // Test with large index
    exception_thrown = false;
    try {
        const unsigned char& value = ba[SIZE_MAX]; //NOLINT
        (void)value; // Prevent unused variable warning
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    assert(exception_thrown && "Out of range exception should be thrown");

    // Verify const correctness - operator[] should not allow modification
    // This is a compile-time check, not a runtime check
    const ByteArray const_ba({0xAA, 0xBB, 0xCC});
    assert(const_ba[0] == 0xAA);
    assert(const_ba[1] == 0xBB);
    assert(const_ba[2] == 0xCC);

    // Test that const version throws on out-of-bounds
    exception_thrown = false;
    try {
        const unsigned char& value = const_ba[3];
        (void)value; // Prevent unused variable warning
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    assert(exception_thrown && "Out of range exception should be thrown");
}

// Test complement operators
void test_complement_operators() {
    // Test unary complement operator (~) - returns a new ByteArray
    ByteArray b1({0xAA, 0xBB, 0xCC});
    ByteArray b2 = ~b1;

    assert(b2.size() == 3);
    assert(b2[0] == static_cast<unsigned char>(~0xAA)); // 0x55
    assert(b2[1] == static_cast<unsigned char>(~0xBB)); // 0x44
    assert(b2[2] == static_cast<unsigned char>(~0xCC)); // 0x33

    // Test in-place complement operator
    ByteArray b3({0x00, 0xFF, 0x55});
    b3 = ~b3;

    assert(b3.size() == 3);
    assert(b3[0] == 0xFF); // ~0x00 = 0xFF
    assert(b3[1] == 0x00); // ~0xFF = 0x00
    assert(b3[2] == 0xAA); // ~0x55 = 0xAA

    // Test complement of empty ByteArray
    ByteArray empty;
    bool exception_thrown = false;
    try {
        ByteArray result = ~empty;
        (void)result; // Prevent unused variable warning
    } catch (const std::invalid_argument&) {
        exception_thrown = true;
    }
    assert(exception_thrown && "Complement of empty ByteArray should throw");

    // Verify original ByteArray is unchanged by ~ operator
    assert(b1.size() == 3);
    assert(b1[0] == 0xAA);
    assert(b1[1] == 0xBB);
    assert(b1[2] == 0xCC);

    // Test in-place complementation
     b1 = ~b1;
    assert(b1.size() == 3);
    assert(b1[0] == static_cast<unsigned char>(~0xAA)); // 0x55
    assert(b1[1] == static_cast<unsigned char>(~0xBB)); // 0x44
    assert(b1[2] == static_cast<unsigned char>(~0xCC)); // 0x33
}

// Test iterator range constructor
void test_iterator_range_constructor() {
    // Test with std::vector
    std::vector<unsigned char> vec{0x11, 0x22, 0x33, 0x44, 0x55};
    ByteArray from_vector(vec.begin(), vec.end());

    assert(from_vector.size() == 5);
    assert(from_vector[0] == 0x11);
    assert(from_vector[1] == 0x22);
    assert(from_vector[2] == 0x33);
    assert(from_vector[3] == 0x44);
    assert(from_vector[4] == 0x55);

    // Test with partial range from a vector
    ByteArray partial_range(vec.begin() + 1, vec.begin() + 4);

    assert(partial_range.size() == 3);
    assert(partial_range[0] == 0x22);
    assert(partial_range[1] == 0x33);
    assert(partial_range[2] == 0x44);

    // Test with std::array
    std::array<unsigned char, 3> arr{0xAA, 0xBB, 0xCC};
    ByteArray from_array(arr.begin(), arr.end());

    assert(from_array.size() == 3);
    assert(from_array[0] == 0xAA);
    assert(from_array[1] == 0xBB);
    assert(from_array[2] == 0xCC);

    // Test with another ByteArray's iterators
    ByteArray original({0x01, 0x02, 0x03, 0x04, 0x05});
    ByteArray from_bytearray(original.begin() + 1, original.end() - 1);

    assert(from_bytearray.size() == 3);
    assert(from_bytearray[0] == 0x02);
    assert(from_bytearray[1] == 0x03);
    assert(from_bytearray[2] == 0x04);

    // Test with empty range
    ByteArray empty_range(vec.begin(), vec.begin());
    assert(empty_range.size() == 0);

    // Test with C-style array
    unsigned char c_array[] = {0xDD, 0xEE, 0xFF};
    ByteArray from_c_array(std::begin(c_array), std::end(c_array));

    assert(from_c_array.size() == 3);
    assert(from_c_array[0] == 0xDD);
    assert(from_c_array[1] == 0xEE);
    assert(from_c_array[2] == 0xFF);

    // Test with string (as a sequence of bytes)
    std::string str = "ABC";
    ByteArray from_string(str.begin(), str.end());

    assert(from_string.size() == 3);
    assert(from_string[0] == 'A');
    assert(from_string[1] == 'B');
    assert(from_string[2] == 'C');
}

// Main test function
int main() {
    test_hex_string_constructor();
    test_vector_constructor();
    test_initializer_list_constructor();
    test_single_byte_constructor();
    test_xor_operators();
    test_copy_move_semantics();
    test_iterators();
    test_secure_erase();
    test_uint64_constructor_and_conversion();
    test_as_64bit_uint_exceptions();
    test_size_and_value_constructor();
    test_subscript_operator_bounds_checking();
    test_complement_operators();
    test_iterator_range_constructor();

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}