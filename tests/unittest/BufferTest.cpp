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
            writer->write<uint8_t>(0xAB);
            writer->write<uint16_t>(0xCDEF);
            writer->write<uint8_t>(0x01);
            uint32_t uint32Array[] = {0x98765432, 0x98765433};
            writer->writeArray<uint32_t>(uint32Array, 2);
        }
    } testObject;
    BufferWriter fakeWriter(nullptr);
    testObject.writeTo(&fakeWriter);
    // uint8_t (1), padding (1), uint16_t (2), uint8_t(1), padding (3),
    // array size (4), uint32_t (4) * 2
    ASSERT_EQ(fakeWriter.size(), 20u);
    std::vector<uint8_t> buffer(fakeWriter.size());

    BufferWriter writer(buffer.data());
    testObject.writeTo(&writer);
    ASSERT_EQ(writer.size(), buffer.size());

    BufferReader reader(buffer.data());
    ASSERT_EQ(reader.read<uint8_t>(), 0xABu);
    ASSERT_EQ(reader.read<uint16_t>(), 0xCDEFu);
    ASSERT_EQ(reader.read<uint8_t>(), 0x01u);
    auto [uint32Array, size] = reader.readArray<uint32_t>();
    ASSERT_EQ(size, 2u);
    ASSERT_EQ(uint32Array[0], 0x98765432u);
    ASSERT_EQ(uint32Array[1], 0x98765433u);
}

}  // namespace minikin
