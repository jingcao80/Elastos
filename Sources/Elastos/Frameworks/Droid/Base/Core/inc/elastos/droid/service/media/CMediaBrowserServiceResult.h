
#ifndef __ELASTOS_DROID_SERVICE_MEDIA_CMEDIABROWSERSERVICERESULT_H__
#define __ELASTOS_DROID_SERVICE_MEDIA_CMEDIABROWSERSERVICERESULT_H__

#include "_Elastos_Droid_Service_Media_CMediaBrowserServiceResult.h"
#include "MediaBrowserServiceResult.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Media {

/**
 * Completion handler for asynchronous callback methods in {@link MediaBrowserService}.
 * <p>
 * Each of the methods that takes one of these to send the result must call
 * {@link #sendResult} to respond to the caller with the given results.  If those
 * functions return without calling {@link #sendResult}, they must instead call
 * {@link #detach} before returning, and then may call {@link #sendResult} when
 * they are done.  If more than one of those methods is called, an exception will
 * be thrown.
 *
 * @see MediaBrowserService#onLoadChildren
 * @see MediaBrowserService#onLoadIcon
 */
CarClass(CMediaBrowserServiceResult)
    , public MediaBrowserServiceResult
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_MEDIA_CMEDIABROWSERSERVICERESULT_H__
