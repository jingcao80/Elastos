#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATLOG_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATLOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.telephony.Rlog;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class CatLog
{
public:
    static CARAPI D(
        /* [in] */ IInterface* caller,
        /* [in] */ const String& msg);

    static CARAPI D(
        /* [in] */ const String& caller,
        /* [in] */ const String& msg);

    static CARAPI E(
        /* [in] */ IInterface* caller,
        /* [in] */ const String& msg);

    static CARAPI E(
        /* [in] */ const String& caller,
        /* [in] */ const String& msg);

public:
    static const Boolean DEBUG;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATLOG_H__

