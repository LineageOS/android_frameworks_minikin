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

#include "minikin/Buffer.h"

#include <gtest/gtest.h>

namespace minikin {

TEST(BufferTest, testMeasureWriteRead) {
    class {
    public:
        void writeTo(BufferWriter* writer) const {
            writer->writeUint8(0xAB);
            writer->writeUint16(0xCDEF);
            writer->writeUint32(0x98765432);
            uint8_t uint8Array[] = {0xAB, 0xAC};
            writer->writeUint8Array(uint8Array, 2);
            uint16_t uint16Array[] = {0xCDEF, 0xCDF0};
            writer->writeUint16Array(uint16Array, 2);
            uint32_t uint32Array[] = {0x98765432, 0x98765433};
            writer->writeUint32Array(uint32Array, 2);
        }
    } testObject;
    std::vector<uint8_t> buffer(BufferWriter::measure(testObject));

    BufferWriter writer(buffer.data());
    testObject.writeTo(&writer);
    ASSERT_EQ(writer.getSize(), buffer.size());

    BufferReader reader(buffer.data());
    ASSERT_EQ(reader.readUint8(), 0xABu);
    ASSERT_EQ(reader.readUint16(), 0xCDEFu);
    ASSERT_EQ(reader.readUint32(), 0x98765432u);
    uint32_t size;
    const uint8_t* uint8Array;
    std::tie(uint8Array, size) = reader.readUint8Array();
    ASSERT_EQ(size, 2u);
    ASSERT_EQ(uint8Array[0], 0xABu);
    ASSERT_EQ(uint8Array[1], 0xACu);
    const uint16_t* uint16Array;
    std::tie(uint16Array, size) = reader.readUint16Array();
    ASSERT_EQ(size, 2u);
    ASSERT_EQ(uint16Array[0], 0xCDEFu);
    ASSERT_EQ(uint16Array[1], 0xCDF0u);
    const uint32_t* uint32Array;
    std::tie(uint32Array, size) = reader.readUint32Array();
    ASSERT_EQ(size, 2u);
    ASSERT_EQ(uint32Array[0], 0x98765432u);
    ASSERT_EQ(uint32Array[1], 0x98765433u);
}

}  // namespace minikin
