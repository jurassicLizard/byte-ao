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

#ifndef SECURITY_OPS_H
#define SECURITY_OPS_H

// FIXME: Enhance erasure security with a more complex multi-pattern approach
// Consider implementing a 4-pass pattern with: 0x00, 0xFF, alternating 0xAA/0x55, and final 0x00
// This provides better protection against sophisticated memory forensics techniques
// Example implementation in secure_zero_raw_() function

// FIXME: Add memory allocation tracking system for comprehensive sensitive data protection
// Implement a thread_local registry to track sensitive allocations with automatic cleanup
// This would ensure sensitive data is erased even in exceptional situations
// Consider adding: register_sensitive_allocation() and cleanup_all_sensitive_allocations() methods

#include <vector>
#include <stdexcept>

/**
 * @file security_ops.h
 * @brief Secure memory operations for sensitive data
 *
 * @section platform Platform Support
 * This module implements platform-specific memory erasure techniques:
 * - Windows: Uses SecureZeroMemory (untested)
 * - macOS/iOS: Uses memset_s (untested)
 * - Linux with glibc ≥2.25: Uses explicit_bzero
 * - Other platforms: Uses a multi-pass volatile memory overwrite with memory barriers
 *
 * @note The code for Windows and macOS is provided but has not been tested by the
 * developers. Community testing and feedback for these platforms is welcomed.
 *
 * @section security Security Considerations
 * While this library uses best practices for secure erasure, it cannot guarantee
 * complete protection against all types of memory analysis. Some factors beyond
 * the library's control include:
 *
 * - Hardware-level memory remanence (cold boot attacks)
 * - Memory compression by the operating system
 * - Memory swapped to disk before erasure
 * - Compiler optimizations that eliminate "unnecessary" memory operations
 *
 * The verification feature helps ensure the immediate effectiveness of the erasure
 * but cannot detect all possible issues.
 */

namespace jlizard::security::unsafe
{
    /**
     * @brief Exception thrown when secure erasure verification fails
     */
    class ErasureVerificationError : public std::runtime_error {
    public:
        explicit ErasureVerificationError(const std::string& what_arg)
            : std::runtime_error(what_arg) {}
    };

    /**
     * @brief Provides secure memory erasure functionality
     *
     * This class offers methods to securely erase sensitive data from memory,
     * using techniques that resist compiler optimizations and help protect
     * against memory forensic analysis.
     *
     * @warning The platform-specific implementations for Windows and macOS have not been
     * extensively tested. The Linux implementation is tested on major distributions with
     * modern glibc. The fallback implementation should work on all platforms but may
     * provide varying levels of security based on compiler behavior.
     */
    class SecureErase
    {
    private:
        /**
         * @brief Platform-specific implementation of secure memory zeroing
         *
         * Uses the most secure available method for the current platform:
         * - Windows: SecureZeroMemory
         * - macOS/iOS: memset_s
         * - Linux/Unix with modern glibc: explicit_bzero
         * - Other platforms: Fallback to a multi-pass overwrite with memory barriers
         *
         * @param ptr Pointer to memory to be erased
         * @param len Length of memory in bytes
         */
        static void secure_zero_raw_(void* ptr, size_t len);

        /**
         * @brief Verifies if memory has been zeroed out in constant time
         *
         * This function checks each byte of memory without early return to
         * prevent timing side-channel attacks.
         *
         * @param ptr Pointer to memory to verify
         * @param len Length of memory in bytes
         * @return true If memory is completely zeroed
         * @return false If any non-zero byte is found
         */
        static bool verify_zeroed_(const void* ptr, size_t len);

    public:
        /**
         * @brief Configuration options for secure erasure
         */
        struct Options {
            bool verify_after_erase = false;  ///< Whether to verify memory is zeroed after erasure
            bool throw_on_verification_failure = true;  ///< Whether to throw on verification failure

            /**
             * @brief Construct options with default settings
             */
            Options() = default;

            /**
             * @brief Construct options with specified settings
             *
             * @param verify Enable verification after erasure
             * @param throw_on_failure Throw exception on verification failure
             */
            explicit Options(bool verify, bool throw_on_failure = true)
                : verify_after_erase(verify),
                  throw_on_verification_failure(throw_on_failure) {}
        };

        /**
         * @brief Securely zeros a trivially copyable object
         *
         * @tparam T Type of object to erase (must be trivially copyable)
         * @param obj Object to securely erase
         * @param options Configuration options for this operation
         * @return bool True if verification succeeded or wasn't requested, false if verification failed
         * @throws ErasureVerificationError If verification fails and throwing is enabled
         *
         * @note For highest security, consider using verification. However, verification might
         * still succeed while the memory could be recovered through other means (like cold boot
         * attacks or memory that was previously swapped to disk).
         *
         * @code
         * // Basic usage with defaults (no verification)
         * int secret_key = 12345;
         * unsafe::SecureErase::secure_zero(secret_key);
         * @endcode
         */
        template<typename T>
        static bool secure_zero(T& obj, const Options& options = Options());

        /**
         * @brief Securely zeros a vector of trivially copyable objects and deallocates its memory
         *
         * This specialized method for vectors zeroes the vector's data memory and then forces
         * deallocation by swapping with an empty vector. This method should be used for vectors
         * instead of the generic secure_zero method.
         *
         * @tparam T Element type (must be trivially copyable)
         * @param vec Vector to securely erase
         * @param options Configuration options for this operation
         * @return bool True if verification succeeded or wasn't requested, false if verification failed
         * @throws ErasureVerificationError If verification fails and throwing is enabled
         *
         * @code
         * // Secure erase a vector with verification
         * std::vector<unsigned char> password{1,2,3,4,5};
         * unsafe::SecureErase::secure_zero_vector(password,
         *     unsafe::SecureErase::Options(true));
         * @endcode
         */
        template<typename T>
        static bool secure_zero_vector(std::vector<T>& vec, const Options& options = Options());
        
        /**
         * @brief Prevents instantiation of this utility class
         */
        SecureErase() = delete;
    };
}

#endif //SECURITY_OPS_H