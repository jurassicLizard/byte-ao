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

    /**
     * @brief Direction for zero-padding and truncation in ByteArray operations
     *
     * This enum specifies how bytes should be padded when extending a ByteArray
     * and which bytes to preserve when truncating, following cryptographic
     * network byte order (big-endian) conventions.
     */
    enum class EZeroPadDir : std::uint8_t
    {
        /**
         * @brief Preserves most significant bytes (right side in big-endian)
         *
         * When extending: Adds zeros at the beginning of the array
         * When truncating: Keeps the rightmost/most significant bytes
         */
        MSB_PAD = 1,
        /**
         * @brief Preserves least significant bytes (left side in big-endian)
         *
         * When extending: Adds zeros at the end of the array
         * When truncating: Keeps the leftmost/least significant bytes
         */
        LSB_PAD = 2,
        /**
         * @brief Default padding direction (LSB_PAD)
         *
         * Uses LSB_PAD as the default for consistency with std::vector behavior
         */
        DEFAULT_PAD = LSB_PAD
    };


    /**
     * @class ByteArray
     * @brief A class representing a dynamic array of bytes with various utility functions
     *        for manipulation, logical operations, and conversion.
     */
    class ByteArray
    {
    private:
        std::vector<unsigned char> bytes_;
    public:
        // Maximum size for random byte generation (1 MB)
        static constexpr size_t MAX_RANDOM_BYTES = 1024 * 1024;

        // Destructor - likely trivial since std::vector handles cleanup
        ~ByteArray() = default;
        ByteArray(const ByteArray& other) = default;
        /**
         * @brief Constructs a ByteArray with a specified number of bytes from another ByteArray
         *
         * Creates a new ByteArray with a size of exactly `num_bytes` following cryptographic
         * network byte order (big-endian) conventions.
         *
         * @param other The source ByteArray to copy from
         * @param num_bytes The exact size of the new ByteArray
         * @param zero_pad_dir The direction for padding/truncation:
         *        - MSB_PAD: Preserves the most significant bytes (rightmost in big-endian):
         *          - When extending: adds zeros at the beginning
         *          - When truncating: keeps the rightmost/most significant bytes
         *        - LSB_PAD: Preserves the least significant bytes (leftmost in big-endian):
         *          - When extending: adds zeros at the end (default, consistent with std::vector)
         *          - When truncating: keeps the leftmost/least significant bytes
         *
         * @note This constructor always creates a ByteArray of exactly `num_bytes` size
         * @note While this library works with cryptographic network byte order (big-endian),
         *       the default padding is LSB_PAD to maintain consistency with the underlying std::vector
         *
         * @example
         * // Create a ByteArray with 5 bytes
         * ByteArray original = {0x01, 0x02, 0x03, 0x04, 0x05};
         *
         * // LSB_PAD examples (default):
         * // Create a ByteArray with 3 bytes (truncates by removing from the right)
         * ByteArray lsb_truncated(original, 3); // Contains {0x01, 0x02, 0x03}
         *
         * // Create a ByteArray with 8 bytes (extends with zeros at the end)
         * ByteArray lsb_extended(original, 8); // Contains {0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00, 0x00}
         *
         * // MSB_PAD examples:
         * // Create a ByteArray with 3 bytes (truncates by removing from the left)
         * ByteArray msb_truncated(original, 3, EZeroPadDir::MSB_PAD); // Contains {0x03, 0x04, 0x05}
         *
         * // Create a ByteArray with 8 bytes (extends with zeros at the beginning)
         * ByteArray msb_extended(original, 8, EZeroPadDir::MSB_PAD); // Contains {0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05}
         */
        ByteArray(const ByteArray& other,const size_t num_bytes,const EZeroPadDir zero_pad_dir = EZeroPadDir::DEFAULT_PAD);
        ByteArray& operator=(const ByteArray& other) = default;
        ByteArray(ByteArray&& other) noexcept : bytes_(std::move(other.bytes_)) {}
        ByteArray& operator=(ByteArray&& other) noexcept;
        //FIXME collect all unit test utilities together
        ByteArray() {bytes_.reserve(JLBA_DEFAULT_ALLOC_SIZE);};
        explicit ByteArray(const std::vector<unsigned char>& byte_array): bytes_(byte_array) {};
        explicit ByteArray(std::vector<unsigned char>&& byte_array) noexcept : bytes_(std::move(byte_array)) {};
        explicit ByteArray(const size_t num_bytes, const unsigned char val) noexcept : bytes_(num_bytes,val) {};
        /**
         * @brief Constructs a ByteArray with contents from an iterator range.
         *
         * Creates a ByteArray with copies of elements in the range [first, last).
         * This constructor is restricted to only work with std::vector<unsigned char> iterators.
         *
         * @tparam InputIt Iterator type, restricted to std::vector<unsigned char>::iterator or const_iterator
         * @param first Iterator to the beginning of the range
         * @param last Iterator to the end of the range
         */
        template <typename InputIt>
        requires std::same_as<InputIt, std::vector<unsigned char>::iterator> ||
                 std::same_as<InputIt, std::vector<unsigned char>::const_iterator>
        explicit ByteArray(InputIt first, InputIt last) : bytes_(first, last) {}


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
        explicit ByteArray(const std::string_view hex_str);
        /**
         * @brief Construct a ByteArray from an initializer list of bytes
         * @param bytes The initializer list of unsigned char values
         */
        ByteArray(const std::initializer_list<unsigned char> bytes) : bytes_(bytes) {};

        /**
         *
         * @return
         */
        bool secure_wipe();

        /**
         * Concatenates a byte array to the end of this one (modifies this object)
         * @param other The byte array to append to this array
         * @return Reference to this ByteArray for method chaining
         */
        ByteArray& concat(const ByteArray& other);

        /**
         * Creates a new ByteArray by concatenating this array with another
         * @param other The byte array to append to a copy of this array
         * @return New ByteArray containing this array's data followed by other's data
         */
        [[nodiscard]] ByteArray concat_copy(const ByteArray& other) const;






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

        // Comparison operator
        bool operator==(const ByteArray& other) const;

        // Logical operation end

        // accessor methods
        // FIXME warn about the fact that downgrading the data size might leave data remnance
        [[nodiscard]] unsigned char* data() noexcept { return bytes_.data(); }
        [[nodiscard]] size_t size() const noexcept { return bytes_.size(); }
        [[nodiscard]] bool empty() const noexcept {return bytes_.empty();}
        [[nodiscard]] std::vector<unsigned char>::const_iterator begin() const noexcept { return bytes_.begin(); }
        [[nodiscard]] std::vector<unsigned char>::const_iterator end() const noexcept { return bytes_.end(); }

        /**
         * @brief Resizes the ByteArray to the specified size with configurable padding direction
         *
         * This method allows resizing with control over which bytes are preserved:
         * - When growing: zeros can be added at the beginning or end based on padding direction
         * - When shrinking: bytes can be removed from the beginning or end based on padding direction
         *
         * @param new_size The new size to resize the ByteArray to
         * @param purge_before_resize If true, securely wipes the original data before
         *        resizing to prevent data remnance. This is especially important when
         *        shrinking the array.
         * @param output_warning If true, outputs a warning to std::cerr when shrinking
         *        the array as this operation could potentially lead to data remnance
         *        if not handled properly.
         * @param zero_pad_dir Determines how the resize operation preserves bytes:
         *        - MSB_PAD: Preserves the most significant bytes (rightmost in big-endian):
         *          - When growing: adds zeros at the beginning
         *          - When shrinking: keeps the rightmost/most significant bytes
         *        - LSB_PAD: Preserves the least significant bytes (leftmost in big-endian):
         *          - When growing: adds zeros at the end (default, consistent with std::vector)
         *          - When shrinking: keeps the leftmost/least significant bytes
         *
         * @note When purge_before_resize is true, this method uses a secure copy-and-wipe
         *       approach that creates a temporary ByteArray, securely wipes the original,
         *       and then assigns the temporary to this object.
         *
         * @warning Shrinking a ByteArray may expose sensitive data if not properly wiped.
         *          Always consider using purge_before_resize=true when handling sensitive data.
         *
         * @see ByteArray(const ByteArray&, size_t, EZeroPadDir) - Uses the same padding behavior
         *      for consistent handling of byte significance
         *
         * @example
         * ByteArray data = {0x01, 0x02, 0x03, 0x04, 0x05};
         *
         * // Grow with LSB padding (default): adds zeros at the end
         * data.resize(8); // Results in {0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00, 0x00}
         *
         * // Grow with MSB padding: adds zeros at the beginning
         * data.resize(8, false, false, EZeroPadDir::MSB_PAD); // Results in {0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05}
         *
         * // Shrink with LSB padding (default): removes bytes from the right
         * data.resize(3); // Results in {0x01, 0x02, 0x03}
         *
         * // Shrink with MSB padding: removes bytes from the left
         * data.resize(3, false, false, EZeroPadDir::MSB_PAD); // Results in {0x03, 0x04, 0x05}
         */
         void resize(const size_t new_size, bool purge_before_resize = true, bool output_warning = true,const EZeroPadDir zero_pad_dir = EZeroPadDir::DEFAULT_PAD);
        /**
         * @brief Convenience overload with padding direction as the second parameter
         *
         * This overload places the padding direction parameter second for better readability
         * when specifying padding behavior is the primary concern. It maintains secure defaults
         * for purge_before_resize and output_warning while allowing them to be overridden.
         *
         * @param new_size The new size to resize the ByteArray to
         * @param zero_pad_dir Determines how the resize operation preserves bytes
         *        (see the full resize method documentation for details)
         * @param purge_before_resize If true (default), securely wipes original data
         * @param output_warning If true (default), warns when shrinking the array
         *
         * @note This overload uses secure defaults that prioritize data security:
         *       - purge_before_resize = true (securely wipes original data)
         *       - output_warning = true (warns about potential data remnance when shrinking)
         *
         * @example
         * ByteArray data = {0x01, 0x02, 0x03};
         * // Using the convenience overload with MSB padding:
         * data.resize(5, EZeroPadDir::MSB_PAD); // Results in {0x00, 0x00, 0x01, 0x02, 0x03}
         *
         * @see resize(size_t, bool, bool, EZeroPadDir) for full documentation on padding behavior
         */
         void resize(const size_t new_size,const EZeroPadDir zero_pad_dir, bool purge_before_resize = true, bool output_warning = true);



        /**
         * @brief Clears all elements from the ByteArray.
         *
         * Removes all elements from the underlying vector, leaving it with a size of 0.
         * The capacity of the vector is not affected, and no reallocation happens.
         * All references, pointers, and iterators to elements are invalidated.
         * FIXME needs secure_wipe handling
         */
        constexpr void clear() noexcept { bytes_.clear(); }

        // utility methods
        //get as 64bit unsigned long , if byte array is too large we throw an invalid argument exception
        // FIXME needs documentation
        [[nodiscard]] uint64_t as_64bit_uint() const;
        /**
         * @brief Converts the ByteArray content to a hexadecimal string representation
         *
         * Each byte is converted to a two-character hexadecimal representation
         * with leading zeros preserved (e.g., 0x0A becomes "0a").
         *
         * @return std::string The hexadecimal string representation of the ByteArray
         *
         * @example
         * ByteArray data = {0x01, 0xAB, 0xCD};
         * std::string hex = data.as_hex_string(); // Returns "01abcd"
         */
        [[nodiscard]] std::string as_hex_string() const;

        /**
         * Creates a new ByteArray from a 64-bit unsigned integer
         * @param byte_array_long The 64-bit value to convert to a ByteArray
         * @return A new ByteArray containing the byte representation of the input value
         */
        static ByteArray create_from_uint64(const uint64_t byte_array_long);

        /**
         * Creates a new ByteArray by concatenating multiple ByteArrays
         * @param arrays An initializer list of ByteArray objects to concatenate
         * @return A new ByteArray containing all input arrays concatenated in sequence
         */
        static ByteArray concat_and_create(const std::initializer_list<ByteArray>& arrays);
        /**
         * @brief Creates a ByteArray from a string_view using list initialization
         *
         * Converts a string_view into a ByteArray using list initialization syntax.
         * This allows for efficient creation of ByteArrays from string literals or
         * stack-allocated strings without creating temporary std::string objects.
         *
         * @param sv The string_view to convert to a ByteArray
         * @return A newly constructed ByteArray containing the data
         *
         * @note This avoids extra copies of the string data
         * @note Example usage: jlizard::ByteArray data = to_byte_array("Hello World");
         */
        static ByteArray create_from_string(std::string_view sv);
        /**
         * @brief Creates a new ByteArray filled with pseudo-random data
         *
         * Generates a ByteArray of the specified size containing pseudo-random bytes.
         * This method uses a pseudo-random number generator to fill the array.
         *
         * @param num_bytes The number of random bytes to generate
         * @return A new ByteArray filled with random data
         * @throws std::invalid_argument If num_bytes exceeds MAX_RANDOM_BYTES (1 MB)
         *
         * @see MAX_RANDOM_BYTES For the maximum allowed size
         *
         * @note The quality of randomness depends on the underlying PRNG implementation
         * @warning This method is NOT cryptographically secure. Do not use for
         *          security-sensitive applications or cryptographic purposes.
         *
         * @example
         * // Create a 32-byte random array
         * ByteArray random_data = ByteArray::create_from_prng(32);
         */
        static ByteArray create_from_prng(const size_t num_bytes);
        /**
         * @brief Creates a ByteArray with pre-allocated memory capacity
         *
         * Creates an empty ByteArray instance with memory already reserved for future use.
         * This can improve performance when the approximate size is known in advance,
         * avoiding multiple reallocations when adding data.
         *
         * @param reserve_size Number of bytes to pre-allocate
         * @return ByteArray A new ByteArray instance with the requested capacity
         *
         * @note The ByteArray will still have a length of zero, but will have
         * internal capacity to store at least reserve_size bytes without reallocation.
         *
         * @see ByteArray::reserve(size_t reserve_size)
         */
        static ByteArray create_with_prealloc(size_t reserve_size);
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
