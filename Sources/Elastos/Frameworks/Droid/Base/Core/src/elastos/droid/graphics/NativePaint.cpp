//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/graphics/NativePaint.h"

using android::VARIANT_DEFAULT;

namespace Elastos {
namespace Droid {
namespace Graphics {

NativePaint::NativePaint()
    : SkPaint()
    , mLetterSpacing(0)
    , mFontFeatureSettings()
    , mTextLocale()
    , mFontVariant(VARIANT_DEFAULT)
{
}

NativePaint::NativePaint(const NativePaint& paint)
    : SkPaint(paint)
    , mLetterSpacing(paint.mLetterSpacing)
    , mFontFeatureSettings(paint.mFontFeatureSettings)
    , mTextLocale(paint.mTextLocale)
    , mFontVariant(paint.mFontVariant)
{
}

NativePaint::~NativePaint() {
}

NativePaint& NativePaint::operator=(const NativePaint& other)
{
    SkPaint::operator=(other);
    mLetterSpacing = other.mLetterSpacing;
    mFontFeatureSettings = other.mFontFeatureSettings;
    mTextLocale = other.mTextLocale;
    mFontVariant = other.mFontVariant;
    return *this;
}

bool operator==(const NativePaint& a, const NativePaint& b)
{
    return static_cast<const SkPaint&>(a) == static_cast<const SkPaint&>(b)
            && a.mLetterSpacing == b.mLetterSpacing
            && a.mFontFeatureSettings == b.mFontFeatureSettings
            && a.mTextLocale == b.mTextLocale
            && a.mFontVariant == b.mFontVariant;
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
