#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIAUPGRADERECEIVER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIAUPGRADERECEIVER_H__

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

class MediaUpgradeReceiver
    : public BroadcastReceiver
    , public IMediaUpgradeReceiver
{
public:
    CAR_INTERFACE_DECL()

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

public:
    static const String TAG;
    static const String PREF_DB_VERSION;
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIAUPGRADERECEIVER_H__
