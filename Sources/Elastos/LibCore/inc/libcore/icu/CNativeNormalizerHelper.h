
#ifndef __LIBCORE_ICU_CNATIVENORMALIZERHELPER_H__
#define __LIBCORE_ICU_CNATIVENORMALIZERHELPER_H__

#include <Singleton.h>
#include "_Libcore_ICU_CNativeNormalizerHelper.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::Singleton;

namespace Libcore {
namespace ICU {

CarClass(CNativeNormalizerHelper)
    , public Singleton
    , public INativeNormalizerHelper
{
    CAR_SINGLETON_DECL()

public:
    CAR_INTERFACE_DECL()

    CARAPI IsNormalized(
        /* [in] */ ICharSequence * src,
        /* [in] */ Form form,
        /* [out] */ Boolean * value);

    CARAPI Normalize(
        /* [in] */ ICharSequence * src,
        /* [in] */ Form form,
        /* [out] */ String * str);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CNATIVENORMALIZERHELPER_H__
