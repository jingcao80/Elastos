
#ifndef __ELASTOS_DROID_INCALLUIBIND_OBJECTFACTORY_H__
#define __ELASTOS_DROID_INCALLUIBIND_OBJECTFACTORY_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace InCallUIBind {

class ObjectFactory
{
public:
    /** @return An {@link Intent} to be broadcast when the InCallUI is visible. */
    static CARAPI_(AutoPtr<IIntent>) GetUiReadyBroadcastIntent(
        /* [in] */ IContext* context);

    /**
     * @return An {@link Intent} to be broadcast when the call state button in the InCallUI is
     * touched while in a call.
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallStateButtonBroadcastIntent(
        /* [in] */ IContext* context);
};

} // namespace InCallUIBind
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUIBIND_OBJECTFACTORY_H__

