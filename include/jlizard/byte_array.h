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

#define JLBA_DEFAULT_ALLOC_SIZE 1024

#include <vector>

namespace jlizard
{
    class ByteArray
    {
    private:
        std::vector<unsigned char> bytes_;
    public:
        ByteArray() {bytes_.reserve(JLBA_DEFAULT_ALLOC_SIZE);};
        explicit ByteArray(const size_t size) {bytes_.resize(size);}
        explicit ByteArray(const char* hex_str);
        explicit ByteArray(const unsigned char* byte_array_raw,const int len);
        ~ByteArray();
    };

}



#endif //BYTE_ARRAY_H
