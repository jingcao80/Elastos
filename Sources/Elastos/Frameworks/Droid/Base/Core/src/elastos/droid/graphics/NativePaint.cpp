
#include "elastos/droid/graphics/NativePaint.h"

using android::VARIANT_DEFAULT;

namespace Elastos {
namespace Droid {
namespace Graphics {

NativePaint::NativePaint() : SkPaint(),
        mLetterSpacing(0), mFontFeatureSettings(), mTextLocale(), mFontVariant(VARIANT_DEFAULT) {
}

NativePaint::NativePaint(const NativePaint& paint) : SkPaint(paint),
        mLetterSpacing(paint.mLetterSpacing), mFontFeatureSettings(paint.mFontFeatureSettings),
        mTextLocale(paint.mTextLocale), mFontVariant(paint.mFontVariant) {
}

NativePaint::~NativePaint() {
}

NativePaint& NativePaint::operator=(const NativePaint& other) {
    SkPaint::operator=(other);
    mLetterSpacing = other.mLetterSpacing;
    mFontFeatureSettings = other.mFontFeatureSettings;
    mTextLocale = other.mTextLocale;
    mFontVariant = other.mFontVariant;
    return *this;
}

bool operator==(const NativePaint& a, const NativePaint& b) {
    return static_cast<const SkPaint&>(a) == static_cast<const SkPaint&>(b)
            && a.mLetterSpacing == b.mLetterSpacing
            && a.mFontFeatureSettings == b.mFontFeatureSettings
            && a.mTextLocale == b.mTextLocale
            && a.mFontVariant == b.mFontVariant;
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
