
#include "Normalizer.h"
#include "NativeNormalizer.h"

using Libcore::ICU::NativeNormalizer;

namespace Elastos {
namespace Text {

Boolean Normalizer::IsNormalized(
    /* [in] */ ICharSequence* src,
    /* [in] */ NormalizerForm form)
{
    assert(src);

    Boolean result;
    NativeNormalizer::IsNormalized(src, form, &result);
    return result;
}

String Normalizer::Normalize(
    /* [in] */ ICharSequence* src,
    /* [in] */ NormalizerForm form)
{
    assert(src);

    String result;
    NativeNormalizer::Normalize(src, form, &result);
    return result;
}

} // namespace Text
} // namespace Elastos
