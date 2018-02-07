/*
 * Copyright (C) 2016 The Android Open Source Project
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

#include "HbFontCache.h"

#include <memory>

#include <gtest/gtest.h>
#include <hb.h>

#include "minikin/MinikinFont.h"

#include "FreeTypeMinikinFontForTest.h"
#include "MinikinInternal.h"

namespace minikin {

class HbFontCacheTest : public testing::Test {
public:
    virtual void TearDown() {
        android::AutoMutex _l(gMinikinLock);
    }
};

TEST_F(HbFontCacheTest, getHbFontLockedTest) {
    auto fontA = std::make_shared<FreeTypeMinikinFontForTest>(kTestFontDir "Regular.ttf");
    auto fontB = std::make_shared<FreeTypeMinikinFontForTest>(kTestFontDir "Bold.ttf");
    auto fontC = std::make_shared<FreeTypeMinikinFontForTest>(kTestFontDir "BoldItalic.ttf");

    android::AutoMutex _l(gMinikinLock);
    // Never return NULL.
    EXPECT_NE(nullptr, getHbFontLocked(fontA.get()));
    EXPECT_NE(nullptr, getHbFontLocked(fontB.get()));
    EXPECT_NE(nullptr, getHbFontLocked(fontC.get()));

    EXPECT_NE(nullptr, getHbFontLocked(nullptr));

    // Must return same object if same font object is passed.
    EXPECT_EQ(getHbFontLocked(fontA.get()), getHbFontLocked(fontA.get()));
    EXPECT_EQ(getHbFontLocked(fontB.get()), getHbFontLocked(fontB.get()));
    EXPECT_EQ(getHbFontLocked(fontC.get()), getHbFontLocked(fontC.get()));

    // Different object must be returned if the passed minikinFont has different ID.
    EXPECT_NE(getHbFontLocked(fontA.get()), getHbFontLocked(fontB.get()));
    EXPECT_NE(getHbFontLocked(fontA.get()), getHbFontLocked(fontC.get()));
}

}  // namespace minikin
