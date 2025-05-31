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
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>

using namespace jlizard;

ByteArray::ByteArray(const std::string_view hex_str)
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

ByteArray::ByteArray(const ByteArray& other, const size_t num_bytes) : bytes_(num_bytes) {
    size_t copy_size = std::min(other.size(), num_bytes);
    std::copy(other.begin(), other.begin() + copy_size, bytes_.begin());
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

ByteArray ByteArray::operator~() const {
    // Create a new ByteArray by using the static complement function
    ByteArray result;
    result.bytes_ = ByteArrayOps::complement(this->bytes_);
    return result;
}

bool ByteArray::operator==(const ByteArray& other) const
{
    if (size() != other.size()) return false;

    return std::ranges::equal(*this,other);
}


uint64_t ByteArray::as_64bit_uint() const
{
    if (bytes_.size() > 8)
    {
        throw std::invalid_argument("Byte array is larger than 64-bit and cannot be represented as such");
    }

    return ByteArrayOps::bytearray_to_uint64(bytes_);
}

std::string ByteArray::as_hex_string() const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (const auto byte : *this) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

ByteArray ByteArray::create_with_prealloc(size_t reserve_size)
{
    std::vector<unsigned char> buf;
    buf.reserve(reserve_size);
    return ByteArray(std::move(buf));
}

ByteArray ByteArray::create_from_uint64(const uint64_t byte_array_long)
{
    ByteArray b;
    ByteArrayOps::uint64_to_bytearray(byte_array_long,b.bytes_);
    return b;
}

ByteArray ByteArray::create_from_string(std::string_view sv)
{
    // Pre-allocate the vector with the exact size needed
    ByteArray result(sv.size(),0x00);

    // Copy all bytes at once (single memcpy operation)
    if (!sv.empty()) {
        std::memcpy(result.data(), sv.data(), sv.size());
    }


    return result;
}


ByteArray ByteArray::concat_and_create(const std::initializer_list<ByteArray>& arrays)
{
    ByteArray result;
    for (const auto& array : arrays) {
        result.bytes_.insert(result.bytes_.end(), array.bytes_.begin(), array.bytes_.end());
    }
    return result;

}

ByteArray& ByteArray::concat(const ByteArray& other)
{
    bytes_.insert(bytes_.end(), other.bytes_.begin(), other.bytes_.end());
    return *this;
}

ByteArray ByteArray::concat_copy(const ByteArray& other) const
{
    ByteArray result(*this);
    result.concat(other);

    return result;
}

ByteArray ByteArray::create_from_prng(const size_t num_bytes)
{

    // Check if requested size exceeds the limit
    if (num_bytes > MAX_RANDOM_BYTES) {
        throw std::invalid_argument("Requested random bytes exceed maximum allowed size (1 MB)");
    }

    std::random_device rd;  // True random source
    std::uniform_int_distribution<unsigned int> dist(0, 255);  // Range for a byte

    std::vector<unsigned char> random_bytes(num_bytes);
    for (size_t i = 0; i < num_bytes; ++i) {
        random_bytes[i] = static_cast<unsigned char>(dist(rd));
    }

    return ByteArray(std::move(random_bytes));
}


void ByteArray::resize(const size_t new_size,const bool purge_before_resize,const bool output_warning)
{

    // if we are shrinking then we need to secure wipe the old byte array to avoid data remnance
    if (purge_before_resize && output_warning && (new_size < size())) std::cerr << "SECURITY WARNING : attempting to shrink a byte array buffer this could lead to data remnance" << std::endl;

    if (purge_before_resize)
    {
        ByteArray temp(*this,new_size);
        secure_wipe();
        *this = temp;
    }else
    {
        bytes_.resize(new_size);
    }

}







