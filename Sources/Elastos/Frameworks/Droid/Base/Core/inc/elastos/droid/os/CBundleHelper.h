
#ifndef __ELASTOS_DROID_OS_CBUNDLEHELPER_H__
#define __ELASTOS_DROID_OS_CBUNDLEHELPER_H__

#include "_Elastos_Droid_Os_CBundleHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CBundleHelper)
    , public Singleton
    , public IBundleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetEMPTY(
        /* [out] */ IBundle** bundle);

    CARAPI ForPair(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [out] */ IBundle** bundle);
};


} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_OS_CBUNDLEHELPER_H__
