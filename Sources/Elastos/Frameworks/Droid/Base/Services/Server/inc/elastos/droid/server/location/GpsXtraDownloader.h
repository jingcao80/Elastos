
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GPSXTRADOWNLOADER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GPSXTRADOWNLOADER_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IProperties;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * A class for downloading GPS XTRA data.
 *
 * {@hide}
 */
class GpsXtraDownloader
{

private:
    static const String TAG;// = "GpsXtraDownloader";

public:
    static const Boolean DEBUG;// = FALSE;

    GpsXtraDownloader(
        /* [in] */ IContext* context,
        /* [in] */ IProperties* properties);
    CARAPI_(AutoPtr<ArrayOf<Byte> >) DownloadXtraData();

protected:

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) DoDownload(
        /* [in] */ const String& url,
        /* [in] */ Boolean isProxySet,
        /* [in] */ const String& proxyHost,
        /* [in] */ Int32 proxyPort);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ArrayOf<String> > mXtraServers;
    // to load balance our server requests
    Int32 mNextServerIndex;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GPSXTRADOWNLOADER_H__
