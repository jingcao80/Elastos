
#include "CNormalizerHelper.h"
#include "Normalizer.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CNormalizerHelper, Singleton, INormalizer)

CAR_SINGLETON_IMPL(CNormalizerHelper)

ECode CNormalizerHelper::IsNormalized(
    /* [in] */ ICharSequence* src,
    /* [in] */ NormalizerForm form,
    /* [out] */ Boolean* isNormalized)
{
    VALIDATE_NOT_NULL(isNormalized)
    *isNormalized = FALSE;
    VALIDATE_NOT_NULL(src)
    *isNormalized = Normalizer::IsNormalized(src, form);
    return NOERROR;
}

ECode CNormalizerHelper::Normalize(
    /* [in] */ ICharSequence* src,
    /* [in] */ NormalizerForm form,
    /* [out] */ String* normalizeString)
{
    VALIDATE_NOT_NULL(normalizeString)
    *normalizeString = String(NULL);
    VALIDATE_NOT_NULL(src)

    *normalizeString = Normalizer::Normalize(src, form);
    return NOERROR;
}

} // namespace Text
} // namespace Elastos

