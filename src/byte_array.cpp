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
#include "jlizard/security_ops.h"
#include "jlizard/byte_array_ops.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace jlizard;

ByteArray::ByteArray(const std::string& hex_str)
{
    bytes_.resize(hex_str.length() / 2 + hex_str.length() % 2);

    size_t output_index = 0;

    for (size_t i = 0; i < hex_str.size(); i += 2) {
        char hex_byte[3] = {0};

        if (i + 1 < hex_str.size()) {
            hex_byte[0] = hex_str[i];
            hex_byte[1] = hex_str[i+1];
        } else {
            hex_byte[0] = '0'; // Pad with 0
            hex_byte[1] = hex_str[i];
        }

        char* end_ptr;
        long value = strtol(hex_byte, &end_ptr, 16);
        //FIXME document this function

        // Check for conversion errors
        if (end_ptr == hex_byte || *end_ptr != '\0') {
            throw std::runtime_error("Invalid hex character");
        }

        bytes_[output_index++] = static_cast<unsigned char>(value);
    }
}

bool ByteArray::secure_wipe()
{
    const auto options = security::unsafe::SecureErase::Options(true);
    return security::unsafe::SecureErase::secure_zero_vector(bytes_,options);
}

ByteArray& ByteArray::operator=(ByteArray&& other) noexcept
{
    if (this != &other) {
        bytes_ = std::move(other.bytes_);
    }
    return *this;
}

ByteArray ByteArray::operator^(const ByteArray& other) const
{
    return ByteArray(ByteArrayOps::xor_op(this->bytes_, other.bytes_));
}


ByteArray ByteArray::operator^(unsigned char byte) const
{
    return ByteArray(ByteArrayOps::xor_op(this->bytes_,byte));
}

ByteArray& ByteArray::operator^=(const ByteArray& other)
{
    const std::vector<unsigned char> temp_bytes = this->bytes_;
    ByteArrayOps::xor_op(temp_bytes, other.bytes_, this->bytes_);

    return *this;
}

ByteArray& ByteArray::operator^=(unsigned char byte)
{
    const std::vector<unsigned char> temp_bytes = this->bytes_;
    ByteArrayOps::xor_op(temp_bytes, byte, bytes_);
    return *this;
}


uint64_t ByteArray::as_64bit_uint() const
{
    if (bytes_.size() > 8)
    {
        throw std::invalid_argument("Byte array is larger than 64-bit and cannot be represented as such");
    }

    return ByteArrayOps::bytearray_to_uint64(bytes_);
}

ByteArray ByteArray::create_from_uint64(const uint64_t byte_array_long)
{
    ByteArray b;
    ByteArrayOps::uint64_to_bytearray(byte_array_long,b.bytes_);
    return b;
}





