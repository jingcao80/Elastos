
#include "CNativeNormalizerHelper.h"
#include "NativeNormalizer.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CNativeNormalizerHelper)

CAR_INTERFACE_IMPL(CNativeNormalizerHelper, Singleton, INativeNormalizerHelper)

ECode CNativeNormalizerHelper::IsNormalized(
    /* [in] */ ICharSequence * src,
    /* [in] */ Form form,
    /* [out] */ Boolean * value)
{
    return NativeNormalizer::IsNormalized(src,form,value);
}

ECode CNativeNormalizerHelper::Normalize(
    /* [in] */ ICharSequence * src,
    /* [in] */ Form form,
    /* [out] */ String * str)
{
    return NativeNormalizer::Normalize(src,form,str);
}

} // namespace ICU
} // namespace Libcore
