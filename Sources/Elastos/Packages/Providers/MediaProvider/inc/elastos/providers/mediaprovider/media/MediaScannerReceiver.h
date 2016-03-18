#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIASCANNERRECEIVER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIASCANNERRECEIVER_H__

#include "_Elastos.Providers.MediaProvider.Media.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

class MediaScannerReceiver
    : public BroadcastReceiver
    , public IMediaScannerReceiver
{
public:
    CAR_INTERFACE_DECL()

    //Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) Scan(
        /* [in] */ IContext* context,
        /* [in] */ const String& volume);

    CARAPI_(void) ScanFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& path);
private:
    static const String TAG;
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIASCANNERRECEIVER_H__