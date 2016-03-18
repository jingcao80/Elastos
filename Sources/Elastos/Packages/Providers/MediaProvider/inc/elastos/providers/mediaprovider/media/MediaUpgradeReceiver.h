#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIAUPGRADERECEIVER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIAUPGRADERECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Providers.MediaProvider.Media.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Providers {
namespace MediaProvider {
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
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIAUPGRADERECEIVER_H__
