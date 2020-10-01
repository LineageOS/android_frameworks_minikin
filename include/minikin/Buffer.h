/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINIKIN_BUFFER_H
#define MINIKIN_BUFFER_H

#include <cstring>
#include <string_view>
#include <utility>

namespace minikin {

// This is a helper class to read data from a memory buffer.
// This class does not copy memory, and may return pointers to parts of the memory buffer.
// Thus the memory buffer should outlive objects created using this class.
class BufferReader {
public:
    BufferReader(const void* buffer) : mCur(reinterpret_cast<const uint8_t*>(buffer)) {}

    uint8_t readUint8() { return *(mCur++); }

    uint16_t readUint16() {
        uint8_t upper = readUint8();
        return (upper << 8) | readUint8();
    }

    uint32_t readUint32() {
        uint16_t upper = readUint16();
        return (upper << 16) | readUint16();
    }

    // Return a pointer to an array and its number of elements.
    template <typename T>
    std::pair<const T*, uint32_t> readArray() {
        static_assert(std::is_pod<T>::value, "T must be a POD");
        uint32_t size = readUint32();
        // TODO: align data to alignof(T)
        const T* data = reinterpret_cast<const T*>(mCur);
        mCur += size * sizeof(T);
        return std::make_pair(data, size);
    }

    std::string_view readString() {
        auto [data, size] = readArray<char>();
        return std::string_view(data, size);
    }

private:
    const uint8_t* mCur;
};

// This is a helper class to write data to a memory buffer.
class BufferWriter {
public:
    // Create a buffer writer. Passing nullptr creates a fake writer,
    // which can be used to measure the buffer size needed.
    BufferWriter(void* buffer)
            : mCur(reinterpret_cast<uint8_t*>(buffer)), mHead(reinterpret_cast<uint8_t*>(buffer)) {}

    BufferWriter(BufferWriter&&) = default;
    BufferWriter& operator=(BufferWriter&&) = default;

    void writeUint8(uint8_t value) {
        if (mHead != nullptr) {
            *mCur = value;
        }
        mCur++;
    }

    void writeUint16(uint16_t value) {
        writeUint8((value >> 8) & 0xFF);
        writeUint8(value & 0xFF);
    }

    void writeUint32(uint32_t value) {
        writeUint16((value >> 16) & 0xFFFF);
        writeUint16(value & 0xFFFF);
    }

    template <typename T>
    void writeArray(const T* data, uint32_t size) {
        static_assert(std::is_pod<T>::value, "T must be a POD");
        writeUint32(size);
        if (mHead != nullptr) {
            memcpy(mCur, data, size * sizeof(T));
        }
        mCur += size * sizeof(T);
    }

    void writeString(std::string_view string) { writeArray<char>(string.data(), string.size()); }

    // Return the number of bytes written.
    size_t size() const { return mCur - mHead; }

private:
    uint8_t* mCur;
    uint8_t* mHead;

    // Forbid copy and assign.
    BufferWriter(const BufferWriter&) = delete;
    void operator=(const BufferWriter&) = delete;
};

}  // namespace minikin

#endif  // MINIKIN_BUFFER_H
