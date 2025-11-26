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
#include "jlizard/security_ops.h"
#include <type_traits>
#include <memory>
#include <atomic> //needed for one of the #elif branches
#include <string> //needed for one of the #elif branches
#include <sstream>

using namespace jlizard::security;

// Platform-specific secure memory zeroing implementations
#if defined(_WIN32)
#include <windows.h>
void unsafe::SecureErase::secure_zero_raw_(void* ptr, size_t len) {
    SecureZeroMemory(ptr, len);
}
#elif defined(__APPLE__)
#include <string.h>
void unsafe::SecureErase::secure_zero_raw_(void* ptr, size_t len) {
    memset_s(ptr, len, 0, len);
}
#elif defined(__linux__) || defined(__unix__)
#if defined(__GLIBC__) && (__GLIBC__ >= 2) && (__GLIBC_MINOR__ >= 25)
#include <cstring>
void unsafe::SecureErase::secure_zero_raw_(void* ptr, size_t len) {
    explicit_bzero(ptr, len);
}
#else
// Fallback implementation with memory barrier for other platforms
void unsafe::SecureErase::secure_zero_raw_(void* ptr, size_t len) {
    volatile unsigned char* p = static_cast<volatile unsigned char*>(ptr);

    // Multi-pass overwrite with different patterns to counteract memory remanence
    // First pass: set all bits to 0
    for (size_t i = 0; i < len; ++i) {
        p[i] = 0x00;
    }
    std::atomic_thread_fence(std::memory_order_seq_cst);

    // Second pass: set all bits to 1
    for (size_t i = 0; i < len; ++i) {
        p[i] = 0xFF;
    }
    std::atomic_thread_fence(std::memory_order_seq_cst);

    // Final pass: zero out
    for (size_t i = 0; i < len; ++i) {
        p[i] = 0x00;
    }
    std::atomic_thread_fence(std::memory_order_seq_cst);
}
#endif
#else
// Default fallback implementation
void unsafe::SecureErase::secure_zero_raw_(void* ptr, size_t len) {
    volatile unsigned char* p = static_cast<volatile unsigned char*>(ptr);

    // Multi-pass overwrite with different patterns
    for (size_t i = 0; i < len; ++i) {
        p[i] = 0x00;
    }
    std::atomic_thread_fence(std::memory_order_seq_cst);

    for (size_t i = 0; i < len; ++i) {
        p[i] = 0xFF;
    }
    std::atomic_thread_fence(std::memory_order_seq_cst);

    for (size_t i = 0; i < len; ++i) {
        p[i] = 0x00;
    }
    std::atomic_thread_fence(std::memory_order_seq_cst);
}
#endif

bool unsafe::SecureErase::verify_zeroed_(const void* ptr, size_t len) {
    const unsigned char* p = static_cast<const unsigned char*>(ptr);
    unsigned char result = 0;

    // Constant-time verification - always examine all bytes
    for (size_t i = 0; i < len; ++i) {
        result |= p[i];
    }

    return result == 0;
}

template <typename T>
bool unsafe::SecureErase::secure_zero(T& obj, const Options& options) {
    static_assert(std::is_trivially_copyable_v<T>,
              "secure_zero can only be used on trivially copyable types");

    // Perform the secure erasure
    secure_zero_raw_(static_cast<void*>(&obj), sizeof(T));

    // Verify if required
    if (options.verify_after_erase) {
        bool verified = verify_zeroed_(static_cast<const void*>(&obj), sizeof(T));

        if (!verified && options.throw_on_verification_failure) {
            std::stringstream ss;
            ss << "Secure erasure verification failed for object at address "
               << &obj << " of size " << sizeof(T) << " bytes";
            throw ErasureVerificationError(ss.str());
        }

        return verified;
    }

    return true;
}

// Special implementation for vectors
template <typename T>
bool unsafe::SecureErase::secure_zero_vector(std::vector<T>& vec, const Options& options) {
    static_assert(std::is_trivially_copyable_v<T>,
              "secure_zero_vector only supports vectors of trivially copyable types");

    if (vec.empty()) {
        return true;
    }

    // Store relevant info before erasure for potential error messages
    void* data_ptr = static_cast<void*>(vec.data());
    size_t total_size = vec.size() * sizeof(T);

    // Perform the secure erasure
    secure_zero_raw_(data_ptr, total_size);

    // Verify if required (must be done before swapping because swap will deallocate)
    bool verified = true;
    if (options.verify_after_erase) {
        verified = verify_zeroed_(static_cast<const void*>(vec.data()), total_size);

        if (!verified && options.throw_on_verification_failure) {
            std::stringstream ss;
            ss << "Secure erasure verification failed for vector at address "
               << &vec << " with data pointer " << data_ptr
               << " of total size " << total_size << " bytes";
            throw ErasureVerificationError(ss.str());
        }
    }

    // Force a deallocation of the memory by swapping with an empty vector
    std::vector<T>().swap(vec);

    return verified;
}

// Explicit template instantiations for common types to avoid linker errors
template bool unsafe::SecureErase::secure_zero<unsigned char>(unsigned char&, const Options&);
template bool unsafe::SecureErase::secure_zero<char>(char&, const Options&);
template bool unsafe::SecureErase::secure_zero<int>(int&, const Options&);
template bool unsafe::SecureErase::secure_zero<unsigned int>(unsigned int&, const Options&);

// Vector specializations (using the separate function for vectors)
template bool unsafe::SecureErase::secure_zero_vector<unsigned char>(std::vector<unsigned char>&, const Options&);
template bool unsafe::SecureErase::secure_zero_vector<char>(std::vector<char>&, const Options&);