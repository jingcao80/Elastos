#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREINTERNALTHUMBNAILS_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREINTERNALTHUMBNAILS_H__

#include "_Elastos_Droid_Provider_CMediaStoreInternalThumbnails.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreInternalThumbnails)
    , public Singleton
    , public IMediaStoreInternalThumbnails
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * This method cancels the thumbnail request so clients waiting for getThumbnail will be
     * interrupted and return immediately. Only the original process which made the getThumbnail
     * requests can cancel their own requests.
     *
     * @param cr ContentResolver
     * @param origId original image or video id. use -1 to cancel all requests.
     * @param groupId the same groupId used in getThumbnail
     * @param baseUri the base URI of requested thumbnails
     */
    CARAPI CancelThumbnailRequest(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 origId,
        /* [in] */ IUri* baseUri,
        /* [in] */ Int64 groupId);

    /**
     * This method ensure thumbnails associated with origId are generated and decode the byte
     * stream from database (MICRO_KIND) or file (MINI_KIND).
     *
     * Special optimization has been done to avoid further IPC communication for MICRO_KIND
     * thumbnails.
     *
     * @param cr ContentResolver
     * @param origId original image or video id
     * @param kind could be MINI_KIND or MICRO_KIND
     * @param options this is only used for MINI_KIND when decoding the Bitmap
     * @param baseUri the base URI of requested thumbnails
     * @param groupId the id of group to which this request belongs
     * @return Bitmap bitmap of specified thumbnail kind
     */
    CARAPI GetThumbnail(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 origId,
        /* [in] */ Int64 groupId,
        /* [in] */ Int32 kind,
        /* [in] */ IBitmapFactoryOptions* options,
        /* [in] */ IUri* baseUri,
        /* [in] */ Boolean isVideo,
        /* [out] */ IBitmap** bitmap);

private:
    CARAPI_(AutoPtr<IBitmap>) GetMiniThumbFromFile(
        /* [in] */ ICursor* c,
        /* [in] */ IUri* baseUri,
        /* [in] */ IContentResolver* cr,
        /* [in] */ IBitmapFactoryOptions* options);

private:
    static const Int32 MINI_KIND;
    static const Int32 FULL_SCREEN_KIND;
    static const Int32 MICRO_KIND;
    static const AutoPtr<ArrayOf<String> > PROJECTION;
    static Object sThumbBufLock;
    static AutoPtr<ArrayOf<Byte> > sThumbBuf;
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREINTERNALTHUMBNAILS_H__
