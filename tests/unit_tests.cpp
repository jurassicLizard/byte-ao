
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

#include "jlizard/byte_array.h"
#include "jlizard/byte_array_ops.h"
#include <cassert>
#include <iostream>

using namespace jlizard;

// Test hex string constructor
void test_hex_string_constructor() {
    const ByteArray b1("fe81eabd5");
    assert(b1.size() == 5);
    assert(b1.data()[0] == 0xfe);
    assert(b1.data()[1] == 0x81);
    assert(b1.data()[2] == 0xea);
    assert(b1.data()[3] == 0xbd);
    assert(b1.data()[4] == 0x05);

    const ByteArray b2("abcdef0123");
    assert(b2.size() == 5);
    assert(b2.data()[0] == 0xab);
    assert(b2.data()[1] == 0xcd);
    assert(b2.data()[2] == 0xef);
    assert(b2.data()[3] == 0x01);
    assert(b2.data()[4] == 0x23);
}

// Test vector constructor
void test_vector_constructor() {
    std::vector<unsigned char> vec{0xB8, 0xAB, 0xAF, 0xFF};
    ByteArray b1(vec);
    assert(b1.size() == 4);
    assert(b1.data()[0] == 0xB8);
    assert(b1.data()[1] == 0xAB);
    assert(b1.data()[2] == 0xAF);
    assert(b1.data()[3] == 0xFF);

    std::vector<unsigned char> empty_vec;
    ByteArray b2(empty_vec);
    assert(b2.size() == 0);

    std::vector<unsigned char> move_vec{0x01, 0x02, 0x03};
    ByteArray b3(std::move(move_vec));
    assert(b3.size() == 3);
    assert(b3.data()[0] == 0x01);
    assert(b3.data()[1] == 0x02);
    assert(b3.data()[2] == 0x03);
    assert(move_vec.empty());
}

// Test initializer list constructor
void test_initializer_list_constructor() {
    ByteArray b1({0xAF, 0xFF, 0xBA});
    assert(b1.size() == 3);
    assert(b1.data()[0] == 0xAF);
    assert(b1.data()[1] == 0xFF);
    assert(b1.data()[2] == 0xBA);

    ByteArray b2({});
    assert(b2.size() == 0);

    ByteArray b3({0x42});
    assert(b3.size() == 1);
    assert(b3.data()[0] == 0x42);
}

// Test single byte constructor
void test_single_byte_constructor() {
    const ByteArray b1(static_cast<unsigned char>(0xAA));
    assert(b1.size() == 1);
    assert(b1.data()[0] == 0xAA);

    const ByteArray b2(static_cast<unsigned char>(0x00));
    assert(b2.size() == 1);
    assert(b2.data()[0] == 0x00);

    const ByteArray b3(static_cast<unsigned char>(0xFF));
    assert(b3.size() == 1);
    assert(b3.data()[0] == 0xFF);
}

// Test XOR operators
void test_xor_operators() {
    // Test XOR with another ByteArray
    const ByteArray b1({0xAA, 0xBB, 0xCC});
    const ByteArray b2({0x55, 0x44, 0x33});
    const ByteArray b3 = b1 ^ b2;

    assert(b3.size() == 3);
    assert(b3.data()[0] == (0xAA ^ 0x55));
    assert(b3.data()[1] == (0xBB ^ 0x44));
    assert(b3.data()[2] == (0xCC ^ 0x33));

    // Test XOR with unequal lengths (right-aligned)
    const ByteArray b4({0xAA, 0xBB});
    const ByteArray b5({0x11, 0x22, 0x33});
    const ByteArray b6 = b4 ^ b5;

    assert(b6.size() == 3);
    // With right alignment, b4 is aligned at the end of b6
    assert(b6.data()[0] == 0x11);  // Only b5's first byte
    assert(b6.data()[1] == (0xAA ^ 0x22));  // First byte of b4 XORed with second byte of b5
    assert(b6.data()[2] == (0xBB ^ 0x33));  // Second byte of b4 XORed with third byte of b5

    // Test XOR with single byte (right-aligned)
    const ByteArray b7({0x12, 0x34, 0x56});
    const ByteArray b8 = b7 ^ 0xFF;

    assert(b8.size() == 3);
    assert(b8.data()[0] == 0x12);  // Unchanged
    assert(b8.data()[1] == 0x34);  // Unchanged
    assert(b8.data()[2] == (0x56 ^ 0xFF));  // Only last byte gets XORed

    // Test XOR-assignment
    ByteArray b9({0xA0, 0xB0, 0xC0});
    const ByteArray b10({0x0A, 0x0B, 0x0C});
    b9 ^= b10;

    assert(b9.size() == 3);
    assert(b9.data()[0] == (0xA0 ^ 0x0A));
    assert(b9.data()[1] == (0xB0 ^ 0x0B));
    assert(b9.data()[2] == (0xC0 ^ 0x0C));

    // Test XOR-assignment with single byte (right-aligned)
    ByteArray b11({0x11, 0x22, 0x33});
    b11 ^= 0x01;

    assert(b11.size() == 3);
    assert(b11.data()[0] == 0x11);  // Unchanged
    assert(b11.data()[1] == 0x22);  // Unchanged
    assert(b11.data()[2] == (0x33 ^ 0x01));  // Only last byte gets XORed
}

// Test copy and move semantics
void test_copy_move_semantics() {
    // Test copy constructor
    const ByteArray original({0x01, 0x02, 0x03});
    const ByteArray copy = original;

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
    for (auto it = ba.begin(); it != ba.end(); ++it) {
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
    ByteArray new_data({0x44, 0x55, 0x66});
    reusable = new_data;

    assert(reusable.size() == 3);
    assert(reusable.data()[0] == 0x44);
    assert(reusable.data()[1] == 0x55);
    assert(reusable.data()[2] == 0x66);
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

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}