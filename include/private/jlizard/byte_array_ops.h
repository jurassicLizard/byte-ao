/*
 * MIT License
 * 
 * Copyright (c) 2025 Salem B.
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
 
#ifndef BYTEARRAYOPS_H
#define BYTEARRAYOPS_H
#include <cstdint>
#include <vector>


namespace  jlizard {

    struct ByteArrayOps
    {
        struct Unsafe
        {
            static void complement(const unsigned char* in,unsigned char* out,const size_t length);
            // XOR operation for raw pointers (right-aligned logic)
            static void xor_op(const unsigned char* first, size_t first_size,
                       const unsigned char* second, size_t second_size,
                       unsigned char* result, size_t result_size);

            // XOR single byte with array (useful for constant-time operations)
            static void xor_op(const unsigned char* input, size_t input_size,
                       unsigned char byte,
                       unsigned char* result, size_t result_size);

            Unsafe() = delete;
        };


        static std::vector<unsigned char> complement(const std::vector<unsigned char>& in);

        static void complement(const std::vector<unsigned char>& in,
                                std::vector<unsigned char>& out);

        // XOR two vectors with right alignment
        static void xor_op(const std::vector<unsigned char>& first_operand,
                          const std::vector<unsigned char>& second_operand,
                          std::vector<unsigned char>& result_out);

        // XOR a vector with a single byte
        static void xor_op(const std::vector<unsigned char>& input,
                          unsigned char byte,
                          std::vector<unsigned char>& result_out);
        // XOR a vector with a single byte by-copy
        static std::vector<unsigned char> xor_op(const std::vector<unsigned char>& input,
                          unsigned char byte);

        // Return new vector as result of XOR operation
        static std::vector<unsigned char> xor_op(const std::vector<unsigned char>& first_operand,
                                               const std::vector<unsigned char>& second_operand);

        // convert uint64 to byte array
        static void uint64_to_bytearray(const uint64_t in,std::vector<unsigned char>& out);
        //convert byte array to uint64 or return largest uint64 integer if byte array is to large
        static uint64_t bytearray_to_uint64(const std::vector<unsigned char>& in);

        ByteArrayOps() = delete;
    };



};



#endif //BYTEARRAYOPS_H
