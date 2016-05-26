
#ifndef __ELASTOS_TEXT_CNORMALIZERHELPER_H__
#define __ELASTOS_TEXT_CNORMALIZERHELPER_H__

#include "_Elastos_Text_CNormalizerHelper.h"
#include "Singleton.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CNormalizerHelper)
    , public Singleton
    , public INormalizer
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsNormalized(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form,
        /* [out] */ Boolean* isNormalized);

    CARAPI Normalize(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form,
        /* [out] */ String* normalizeString);

};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CNORMALIZERHELPER_H__
