
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLLOCATIONHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLLOCATIONHELPER_H__

#include "_Elastos_Droid_Telephony_CCellLocationHelper.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellLocationHelper)
{
public:
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
