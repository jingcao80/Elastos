
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_HBPCDUTIL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_HBPCDUTIL_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class HbpcdUtils
    : public Object
    , public IHbpcdUtils
{
public:
    CAR_INTERFACE_DECL()

    HbpcdUtils(
        /* [in] */ IContext* context);

    /**
     *  Resolves the unknown MCC with SID and Timezone information.
    */
    CARAPI GetMcc(
        /* [in] */ Int32 sid,
        /* [in] */ Int32 tz,
        /* [in] */ Int32 DSTflag,
        /* [in] */ Boolean isNitzTimeZone,
        /* [out] */ Int32* result);

    /**
     *  Gets country information with given MCC.
    */
    CARAPI GetIddByMcc(
        /* [in] */ Int32 mcc,
        /* [out] */ String* result);

private:
    static const String TAG;
    static const Boolean DBG;
    AutoPtr<IContentResolver> mResolver;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_HBPCDUTIL_H__
