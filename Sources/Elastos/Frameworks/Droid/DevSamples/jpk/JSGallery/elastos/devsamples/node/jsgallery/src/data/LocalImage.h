
#ifndef __LOCALIMAGE_H__
#define __LOCALIMAGE_H__

#include "app/Activity.h"
#include "Path.h"
#include "LocalMediaItem.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;

namespace Gallery {

class LocalImage
    : public LocalMediaItem
{
public:
    class UpdateHelper
        : public ElRefBase
    {
    public:
        UpdateHelper();

        CARAPI_(Int32) Update(
            /* [in] */ Int32 original,
            /* [in] */ Int32 update);

        CARAPI_(Int64) Update(
            /* [in] */ Int64 original,
            /* [in] */ Int64 update);

        CARAPI_(Double) Update(
            /* [in] */ Double original,
            /* [in] */ Double update);

        CARAPI_(String) Update(
            /* [in] */ const String& original,
            /* [in] */ const String& update);

        CARAPI_(Boolean) IsUpdated();

    private:
        Boolean mUpdated;

    };

public:
    LocalImage(
       /* [in] */ Path* path,
       /* [in] */ IContext* application,
       /* [in] */ ICursor* cursor);

    LocalImage(
        /* [in] */ Path* path,
        /* [in] */ IContext* application,
        /* [in] */ Int32 id);

    virtual CARAPI_(PInterface) Probe(
       /* [in] */ REIID riid);

    // Job<Bitmap> requestImage(int type);
    // Job<BitmapRegionDecoder> requestLargeImage();

    virtual CARAPI_(Int32) GetSupportedOperations();

    virtual CARAPI Delete();

    virtual CARAPI Rotate(
        /* [in] */ Int32 degrees);

    virtual CARAPI_(AutoPtr<IUri>) GetContentUri();

    virtual CARAPI_(Int32) GetMediaType();

    virtual CARAPI_(AutoPtr<MediaDetails>) GetDetails();

    virtual CARAPI_(Int32) GetRotation();

    virtual CARAPI_(Int32) GetWidth();

    virtual CARAPI_(Int32) GetHeight();

    virtual CARAPI_(String) GetFilePath();

protected:
    virtual CARAPI_(Boolean) UpdateFromCursor(
        /* [in] */ ICursor* cursor);

private:
    static CARAPI_(Boolean) UpdateWidthAndHeightProjection();

    static CARAPI_(String) GetExifOrientation(
        /* [in] */ Int32 orientation);

private:
    CARAPI_(void) LoadFromCursor(
        /* [in] */ ICursor* cursor);

public:
    static AutoPtr< ArrayOf<String> > PROJECTION;

    Int32 rotation;

private:
    static const String TAG;
    // Must preserve order between these indices and the order of the terms in
    // the following PROJECTION array.
    static const Int32 INDEX_ID;
    static const Int32 INDEX_CAPTION;
    static const Int32 INDEX_MIME_TYPE;
    static const Int32 INDEX_LATITUDE;
    static const Int32 INDEX_LONGITUDE;
    static const Int32 INDEX_DATE_TAKEN;
    static const Int32 INDEX_DATE_ADDED;
    static const Int32 INDEX_DATE_MODIFIED;
    static const Int32 INDEX_DATA;
    static const Int32 INDEX_ORIENTATION;
    static const Int32 INDEX_BUCKET_ID;
    static const Int32 INDEX_SIZE;
    static const Int32 INDEX_WIDTH;
    static const Int32 INDEX_HEIGHT;

    static Boolean sArrayInit;

    AutoPtr<IContext> mApplication;
};

} // namespace Gallery

#endif // __LOCALIMAGE_H__
