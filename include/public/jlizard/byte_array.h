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
 
#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#define JLBA_DEFAULT_ALLOC_SIZE 32

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <string>

//FIXME create overloads for xor and complement
//FIXME iterator for transparently pushing bytes
//FIXME create Unsafe block for raw pointer op
//FIXME use SIMD
//FIXME add uint64_t constructor
//FIXME add boolean flag for automatic secure wipe operation
namespace jlizard
{
//FIXME consider aliasing unsigned char to Byte via using Byte = unsigned char;
    class ByteArray
    {
    private:
        std::vector<unsigned char> bytes_;

    public:
        // Destructor - likely trivial since std::vector handles cleanup
        ~ByteArray() = default;
        ByteArray(const ByteArray& other) = default;
        ByteArray& operator=(const ByteArray& other) = default;
        ByteArray(ByteArray&& other) noexcept : bytes_(std::move(other.bytes_)) {}
        ByteArray& operator=(ByteArray&& other) noexcept;

        ByteArray() {bytes_.reserve(JLBA_DEFAULT_ALLOC_SIZE);};
        explicit ByteArray(const std::string& hex_str);
        explicit ByteArray(const std::vector<unsigned char>& byte_array): bytes_(byte_array) {};
        explicit ByteArray(std::vector<unsigned char>&& byte_array) noexcept : bytes_(std::move(byte_array)) {};
        explicit ByteArray(const size_t num_bytes, const unsigned char val) noexcept : bytes_(num_bytes,val) {};
        /**
         * Creates a ByteArray containing a single byte.
         * @param byte The single byte value to store
         */
        template <typename T> requires std::is_same_v<T, unsigned char>
        explicit ByteArray(T byte): bytes_(1,byte) {};
        /**
         * @brief Takes a string hex input and converts it to a byte array
         * @param hex_str
         * @param do_secure_wipe
         */
        explicit ByteArray(const char* hex_str) : ByteArray(std::string(hex_str)) {};
        /**
         * @brief Construct a ByteArray from an initializer list of bytes
         * @param bytes The initializer list of unsigned char values
         */
        ByteArray(std::initializer_list<unsigned char> bytes) : bytes_(bytes) {};

        bool secure_wipe();

        // Logical Operations Begin
        // XOR-assignment operator
        ByteArray& operator^=(const ByteArray& other);

        // XOR with another ByteArray, returning a new ByteArray
        ByteArray operator^(const ByteArray& other) const;

        // XOR with a single byte, returning a new ByteArray
        ByteArray operator^(unsigned char byte) const;

        // XOR-assignment with a single byte
        ByteArray& operator^=(unsigned char byte);

        // 1's complement operator (unary ~)
        ByteArray operator~() const;

        // Logical operation end

        // accessor methods
        [[nodiscard]] unsigned char* data() noexcept { return bytes_.data(); }
        [[nodiscard]] size_t size() const noexcept { return bytes_.size(); }
        [[nodiscard]] auto begin() const noexcept { return bytes_.begin(); }
        [[nodiscard]] auto end() const noexcept { return bytes_.end(); }

        // utility methods
        //get as 64bit unsigned long , if byte array is too large we throw an invalid argument exception
        [[nodiscard]] uint64_t as_64bit_uint() const;
        static ByteArray create_from_uint64(const uint64_t byte_array_long);

        /**
         * @brief Access the byte at the specified index with bounds checking
         *
         * Unlike standard library containers, this operator performs bounds checking
         * to prevent undefined behavior. If the index is out of bounds, a std::out_of_range
         * exception will be thrown.
         *
         * @param index The zero-based index of the byte to access
         * @return A const reference to the byte at the specified index
         * @throws std::out_of_range If index is out of bounds
         * @see at() For similar bounds-checked access
         */
        const unsigned char& operator[](const size_t index) const {
             return bytes_.at(index);
        }

        // Bounds-checking accessor - allows modification
        // Not noexcept because it can throw std::out_of_range
        unsigned char& at(const size_t index) { return bytes_.at(index); }

        // Const bounds-checking accessor - prevents modification
        // Not noexcept because it can throw std::out_of_range
        [[nodiscard]] const unsigned char& at(const size_t index) const { return bytes_.at(index); }


    };

}



#endif //BYTE_ARRAY_H
