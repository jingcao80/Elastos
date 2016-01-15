
#ifndef __BUCKETHELPER_H__
#define __BUCKETHELPER_H__

#include "elastos/droid/app/Activity.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::Etl::HashMap;

namespace Gallery {

class BucketHelper
    : public ElRefBase
{
public:
    class BucketEntry
        : public ElRefBase
    {
    public:
        BucketEntry(
            /* [in] */ Int32 id,
            /* [in] */ const String& name);

        Int32 GetHashCode();

        Boolean Equals(BucketEntry* other);

    public:
        String bucketName;
        Int32 bucketId;
        Int32 dateTaken;
    };

public:
    static AutoPtr< ArrayOf<BucketEntry*> > LoadBucketEntries(
        /*JobContext jc,*/
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int32 type);

    static String GetBucketName(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int32 bucketId);

private:
    static void UpdateBucketEntriesFromTable(
        /*JobContext jc,*/
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* tableUri,
        /* [in] */ HashMap<Int32, AutoPtr<BucketEntry> >* buckets);

    static AutoPtr< ArrayOf<BucketEntry*> > LoadBucketEntriesFromImagesAndVideoTable(
        /*JobContext jc,*/
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int32 type);

    static AutoPtr< ArrayOf<BucketEntry*> > LoadBucketEntriesFromFilesTable(
        /*JobContext jc,*/
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int32  type);

    static String GetBucketNameInTable(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* tableUri,
        /* [in] */ Int32  bucketId);

    static AutoPtr<IUri> GetFilesContentUri();

private:
    static const String TAG;
    static const String EXTERNAL_MEDIA;

    // BUCKET_DISPLAY_NAME is a string like "Camera" which is the directory
    // name of where an image or video is in. BUCKET_ID is a hash of the path
    // name of that directory (see computeBucketValues() in MediaProvider for
    // details). MEDIA_TYPE is video, image, audio, etc.
    //
    // The "albums" are not explicitly recorded in the database, but each image
    // or video has the two columns (BUCKET_ID, MEDIA_TYPE). We define an
    // "album" to be the collection of images/videos which have the same value
    // for the two columns.
    //
    // The goal of the query (used in loadSubMediaSetsFromFilesTable()) is to
    // find all albums, that is, all unique values for (BUCKET_ID, MEDIA_TYPE).
    // In the meantime sort them by the timestamp of the latest image/video in
    // each of the album.
    //
    // The order of columns below is important: it must match to the index in
    // MediaStore.
    static const String PROJECTION_BUCKET[];

    // The indices should match the above projections.
    static const Int32 INDEX_BUCKET_ID = 0;
    static const Int32 INDEX_MEDIA_TYPE = 1;
    static const Int32 INDEX_BUCKET_NAME = 2;

    // We want to order the albums by reverse chronological order. We abuse the
    // "WHERE" parameter to insert a "GROUP BY" clause into the SQL statement.
    // The template for "WHERE" parameter is like:
    //    SELECT ... FROM ... WHERE (%s)
    // and we make it look like:
    //    SELECT ... FROM ... WHERE (1) GROUP BY 1,(2)
    // The "(1)" means true. The "1,(2)" means the first two columns specified
    // after SELECT. Note that because there is a ")" in the template, we use
    // "(2" to match it.
    static const String BUCKET_GROUP_BY;

    static const String BUCKET_ORDER_BY;

    // Before HoneyComb there is no Files table. Thus, we need to query the
    // bucket info from the Images and Video tables and then merge them
    // together.
    //
    // A bucket can exist in both tables. In this case, we need to find the
    // latest timestamp from the two tables and sort ourselves. So we add the
    // MAX(date_taken) to the projection and remove the media_type since we
    // already know the media type from the table we query from.
    static const String PROJECTION_BUCKET_IN_ONE_TABLE[];

    // We keep the INDEX_BUCKET_ID and INDEX_BUCKET_NAME the same as
    // PROJECTION_BUCKET so we can reuse the values defined before.
    static const Int32 INDEX_DATE_TAKEN = 1;

    // When query from the Images or Video tables, we only need to group by BUCKET_ID.
    static const String BUCKET_GROUP_BY_IN_ONE_TABLE;
};

} // namespace Gallery

#endif // __BUCKETHELPER_H__
