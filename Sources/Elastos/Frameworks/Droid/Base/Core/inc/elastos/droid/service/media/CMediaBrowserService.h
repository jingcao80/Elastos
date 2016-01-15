
#ifndef __ELASTOS_DROID_SERVICE_MEDIA_CMEDIABROWSERSERVICE_H__
#define __ELASTOS_DROID_SERVICE_MEDIA_CMEDIABROWSERSERVICE_H__

#include "_Elastos_Droid_Service_Media_CMediaBrowserService.h"
#include "MediaBrowserService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Media {

/**
 * Base class for media browse services.
 * <p>
 * Media browse services enable applications to browse media content provided by an application
 * and ask the application to start playing it.  They may also be used to control content that
 * is already playing by way of a {@link MediaSession}.
 * </p>
 *
 * To extend this class, you must declare the service in your manifest file with
 * an intent filter with the {@link #SERVICE_INTERFACE} action.
 *
 * For example:
 * </p><pre>
 * &lt;service android:name=".MyMediaBrowserService"
 *          android:label="&#64;string/service_name" >
 *     &lt;intent-filter>
 *         &lt;action android:name="android.media.browse.MediaBrowserService" />
 *     &lt;/intent-filter>
 * &lt;/service>
 * </pre>
 *
 */
 CarClass(CMediaBrowserService)
    , public MediaBrowserService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_MEDIA_CMEDIABROWSERSERVICE_H__
