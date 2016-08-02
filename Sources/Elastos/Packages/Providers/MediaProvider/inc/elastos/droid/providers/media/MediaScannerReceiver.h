#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIASCANNERRECEIVER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIASCANNERRECEIVER_H__

#include "_Elastos.Droid.Providers.Media.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

class MediaScannerReceiver
    : public BroadcastReceiver
    , public IMediaScannerReceiver
{
public:
    TO_STRING_IMPL("MediaScannerReceiver")

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
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIASCANNERRECEIVER_H__