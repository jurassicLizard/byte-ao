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
 
 #include "jlizard/byte_array_ops.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>


using namespace jlizard;

//FIXME document right-alignment procedure
//FIXME document RVO

void ByteArrayOps::Unsafe::complement(const unsigned char* in, unsigned char* out, const size_t length)
{
    //FIXME make this throw exception
    if (!out) return;
    if (!in) return;

    for(size_t i=0;i<length;++i) {
        out[i] = ~in[i];
    }

    //FIXME move this to tests section
    for(size_t i=0;i<length;++i) {
        assert(((out[i] & in[i]) == 0) && "working complmentation");
    }
}

void ByteArrayOps::complement(const std::vector<unsigned char>& in, std::vector<unsigned char>& out)
{
    if (in.empty())
    {
        throw std::invalid_argument("Cannot process an empty byte array");
    }

    out.resize(in.size());

    for (size_t i = 0; i < in.size(); ++i)
    {
        out[i] = ~in[i];
    }

}

std::vector<unsigned char> ByteArrayOps::complement(const std::vector<unsigned char>& in)
{
    std::vector<unsigned char> ret;
    complement(in,ret);
    return ret;
}


void ByteArrayOps::xor_op(const std::vector<unsigned char>& first_operand, const std::vector<unsigned char>& second_operand, std::vector<unsigned char>& result_out)
{
    const size_t arr_size = std::max(first_operand.size(), second_operand.size());
    result_out.resize(arr_size);

    // Fill result with zeros initially FIXME document std::ranges
    std::fill(result_out.begin(), result_out.end(), 0);

    // XOR the overlapping parts (from the right)
    const size_t first_offset = arr_size - first_operand.size();
    const size_t second_offset = arr_size - second_operand.size();

    for (size_t i = 0; i < first_operand.size(); i++) {
        result_out[first_offset + i] ^= first_operand[i];
    }

    for (size_t i = 0; i < second_operand.size(); i++) {
        result_out[second_offset + i] ^= second_operand[i];
    }
}

void ByteArrayOps::xor_op(const std::vector<unsigned char>& input, unsigned char byte, std::vector<unsigned char>& result_out)
{
    const std::vector<unsigned char> byte_buf(1,byte);
    xor_op(input,byte_buf,result_out);
}



std::vector<unsigned char> ByteArrayOps::xor_op(const std::vector<unsigned char>& first_operand, const std::vector<unsigned char>& second_operand)
{
    std::vector<unsigned char> result;
    xor_op(first_operand,second_operand,result);
    return result;
}

void ByteArrayOps::Unsafe::xor_op(const unsigned char* first, size_t first_size,
                                  const unsigned char* second, size_t second_size,
                                  unsigned char* result, size_t result_size)
{
    // Ensure result buffer is large enough
    size_t required_size = std::max(first_size, second_size);
    if (result_size < required_size) {
        return;
    }

    // Zero out the result buffer
    std::memset(result, 0, result_size);

    // Apply right-alignment logic and XOR
    size_t first_offset = required_size - first_size;
    size_t second_offset = required_size - second_size;

    // First copy the first operand (without XOR since buffer is zeroed)
    for (size_t i = 0; i < first_size; i++) {
        result[first_offset + i] = first[i];
    }

    // Then XOR the second operand in-place
    for (size_t i = 0; i < second_size; i++) {
        result[second_offset + i] ^= second[i];
    }
}

void ByteArrayOps::Unsafe::xor_op(const unsigned char* input, size_t input_size,
                                 unsigned char byte,
                                 unsigned char* result, size_t result_size)
{
    // Make sure result buffer has enough space
    if (result_size < input_size) {
        return;
    }

    // Zero out the result buffer
    std::memset(result, 0, result_size);

    // Apply right-alignment logic
    size_t input_offset = result_size - input_size;

    // Copy input data to result (starting at the proper offset)
    for (size_t i = 0; i < input_size; i++) {
        result[input_offset + i] = input[i];
    }

    // XOR the last byte with the provided byte value
    result[result_size - 1] ^= byte;
}

std::vector<unsigned char> ByteArrayOps::xor_op(const std::vector<unsigned char>& input, unsigned char byte)
{
    std::vector<unsigned char> result;
    xor_op(input,byte,result);
    return result;
}


void ByteArrayOps::uint64_to_bytearray(const uint64_t in, std::vector<unsigned char>& out)
{
    // Convert to big-endian byte order (most significant byte first)
    uint64_t value = in;
    // Count how many bytes we need
    int bytes_needed = 0;
    uint64_t temp = value;
    do {
        bytes_needed++;
        temp >>= 8;
    } while (temp != 0);

    // Resize the output vector
    out.resize(bytes_needed);

    // Fill the vector with bytes in big-endian order
    for (int i = bytes_needed - 1; i >= 0; i--) {
        out[bytes_needed - 1 - i] = static_cast<unsigned char>((value >> (i * 8)) & 0xFF);
    }
}

uint64_t ByteArrayOps::bytearray_to_uint64(const std::vector<unsigned char>& in)
{
    if (in.size() > 8) {
        throw std::invalid_argument("Byte array is larger than 64-bit and cannot be represented as such");
    }

    uint64_t result = 0;

    // Convert from big-endian byte order
    for (size_t i = 0; i < in.size(); i++) {
        result = (result << 8) | in[i];
    }

    return result;
}







