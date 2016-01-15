
#ifndef __CNOTIFICATIONHANDLER_H__
#define __CNOTIFICATIONHANDLER_H__

//#include "BroadcastReceiver.h"
#include "_CNotificationHandler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

CarClass(CNotificationHandler) //: public BroadcastReceiver
{
public:
    CARAPI OnReciever(
        /* [in] */ IContext* pContext,
        /* [in] */ IIntent *pIntent);

protected:
    ECode HandleIntent(
        /* [in] */ IContext* pContext,
        /* [in] */ IIntent* pNotificationIntent);

private:
};

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CNOTIFICATIONHANDLER_H__

