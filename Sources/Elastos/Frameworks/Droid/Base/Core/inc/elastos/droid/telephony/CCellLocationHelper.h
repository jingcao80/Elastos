
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLLOCATIONHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLLOCATIONHELPER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telephony_CCellLocationHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellLocationHelper)
    , public Singleton
    , public ICellLocationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI RequestLocationUpdate();

    CARAPI NewFromBundle(
        /* [in] */ IBundle* bundle,
        /* [out] */ ICellLocation** cl);

    CARAPI GetEmpty(
        /* [out] */ ICellLocation** cl);
};

} //namespace Elastos
} //namespace Droid
} //namespace Telephony

#endif // __ELASTOS_DROID_TELEPHONY_CCELLLOCATIONHELPER_H__
