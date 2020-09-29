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

    // Return a pointer to an uint8_t array and its number of element.
    std::pair<const uint8_t*, uint32_t> readUint8Array() {
        uint32_t size = readUint32();
        const uint8_t* data = mCur;
        mCur += size;
        return std::make_pair(data, size);
    }

    // Return a pointer to an uint16_t array and its number of element.
    std::pair<const uint16_t*, uint32_t> readUint16Array() {
        uint32_t size = readUint32();
        // TODO: align data to alignof(uint16_t)
        const uint16_t* data = reinterpret_cast<const uint16_t*>(mCur);
        mCur += size * sizeof(uint16_t);
        return std::make_pair(data, size);
    }

    // Return a pointer to an uint32_t array and its number of element.
    std::pair<const uint32_t*, uint32_t> readUint32Array() {
        uint32_t size = readUint32();
        // TODO: align data to alignof(uint32_t)
        const uint32_t* data = reinterpret_cast<const uint32_t*>(mCur);
        mCur += size * sizeof(uint32_t);
        return std::make_pair(data, size);
    }

private:
    const uint8_t* mCur;
};

// This is a helper class to write data to a memory buffer.
class BufferWriter {
public:
    BufferWriter(void* buffer) : BufferWriter(buffer, false) {}

    BufferWriter(BufferWriter&&) = default;
    BufferWriter& operator=(BufferWriter&&) = default;

    void writeUint8(uint8_t value) {
        if (!mDryRun) {
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

    void writeUint8Array(const uint8_t* data, uint32_t size) {
        writeUint32(size);
        if (!mDryRun) {
            memcpy(mCur, data, size);
        }
        mCur += size;
    }

    void writeUint16Array(const uint16_t* data, uint32_t size) {
        writeUint32(size);
        if (!mDryRun) {
            memcpy(mCur, data, size * sizeof(uint16_t));
        }
        mCur += size * sizeof(uint16_t);
    }

    void writeUint32Array(const uint32_t* data, uint32_t size) {
        writeUint32(size);
        if (!mDryRun) {
            memcpy(mCur, data, size * sizeof(uint32_t));
        }
        mCur += size * sizeof(uint32_t);
    }

    // Return the number of bytes written.
    size_t getSize() const { return mCur - mHead; }

    // Return the number of bytes required to write the object.
    template <class T>
    static size_t measure(const T& t) {
        BufferWriter writer(nullptr, true /* dryRun */);
        t.writeTo(&writer);
        return writer.getSize();
    }

private:
    BufferWriter(void* buffer, bool dryRun)
            : mCur(reinterpret_cast<uint8_t*>(buffer)),
              mHead(reinterpret_cast<uint8_t*>(buffer)),
              mDryRun(dryRun) {}

    uint8_t* mCur;
    uint8_t* mHead;
    bool mDryRun;

    // Forbid copy and assign.
    BufferWriter(const BufferWriter&) = delete;
    void operator=(const BufferWriter&) = delete;
};

}  // namespace minikin

#endif  // MINIKIN_BUFFER_H
