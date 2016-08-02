#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_MTPRECEIVER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_MTPRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Providers.Media.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

class MtpReceiver
    : public BroadcastReceiver
    , public IMtpReceiver
{
public:
    TO_STRING_IMPL("MtpReceiver")

    CAR_INTERFACE_DECL()

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI HandleUsbState(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static const String TAG;
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_MTPRECEIVER_H__