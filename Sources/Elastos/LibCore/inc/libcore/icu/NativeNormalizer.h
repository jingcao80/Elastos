
#ifndef __LIBCORE_ICU_NATIVENORMALIZER_H__
#define __LIBCORE_ICU_NATIVENORMALIZER_H__

#include "Elastos.CoreLibrary.h"

using Elastos::Core::ICharSequence;
using Elastos::Text::NormalizerForm;

namespace Libcore {
namespace ICU {

class NativeNormalizer
{
public:
    static CARAPI IsNormalized(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form,
        /* [out] */ Boolean* isNormalized);

    static CARAPI Normalize(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form,
        /* [out] */ String* result);

private:
    NativeNormalizer() {}

    static CARAPI_(Int32) ToUNormalizationMode(
        /* [in] */ NormalizerForm form);

    static CARAPI NormalizeImpl(
        /* [in] */ const String& src,
        /* [in] */ NormalizerForm form,
        /* [out] */ String* result);

    static CARAPI IsNormalizedImpl(
        /* [in] */ const String& src,
        /* [in] */ NormalizerForm form,
        /* [out] */ Boolean* isNormalized);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_NATIVENORMALIZER_H__
