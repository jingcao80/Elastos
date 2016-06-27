#include "BucketHelper.h"
#include <elastos/Logger.h>

using Elastos::Utility::Logging::Logger;

// using Elastos::Droid::Provider::IMediaStoreImagesImageColumns;
// using Elastos::Droid::Provider::IMediaStoreFilesFileColumns;
// using Elastos::Droid::Provider::IMediaStoreFiles;

namespace Gallery {

const String BucketHelper::TAG("BucketHelper");
const String BucketHelper::EXTERNAL_MEDIA = String("external");

const String BucketHelper::PROJECTION_BUCKET[] = {
    String("bucket_id"), //IMediaStoreImagesImageColumns::BUCKET_ID,
    String("media_type"), //IMediaStoreFilesFileColumns::MEDIA_TYPE,
    String("bucket_display_name")};//IMediaStoreImagesImageColumns::BUCKET_DISPLAY_NAME

const String BucketHelper::BUCKET_GROUP_BY = String("1) GROUP BY 1,(2");
const String BucketHelper::BUCKET_ORDER_BY = String("MAX(datetaken) DESC");

const String BucketHelper::PROJECTION_BUCKET_IN_ONE_TABLE[] = {
    String("bucket_id"), //IMediaStoreImagesImageColumns::BUCKET_ID,
    String("MAX(datetaken)"),
    String("bucket_display_name")};//IMediaStoreImagesImageColumns::BUCKET_DISPLAY_NAME

const String BucketHelper::BUCKET_GROUP_BY_IN_ONE_TABLE = String("1) GROUP BY (1");

BucketHelper::BucketEntry::BucketEntry(
    /* [in] */ Int32 id,
    /* [in] */ const String& name)
    : bucketName(name)
    , bucketId(id)
    , dateTaken(0)
{}

Int32 BucketHelper::BucketEntry::GetHashCode()
{
    return bucketId;
}

Boolean BucketHelper::BucketEntry::Equals(
    /* [in] */ BucketEntry* other)
{
    return bucketId == other->bucketId;
}

AutoPtr< ArrayOf<BucketHelper::BucketEntry*> > BucketHelper::LoadBucketEntries(
    /*JobContext jc,*/
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32 type)
{
    Logger::D(TAG, "LoadBucketEntries--");
    // return LoadBucketEntriesFromFilesTable(resolver, type);
    return NULL;
}

String BucketHelper::GetBucketName(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32 bucketId)
{
    Logger::D(TAG, "getBucketName--");
    // String result = getBucketNameInTable(resolver, getFilesContentUri(), bucketId);
    // return result == null ? "" : result;
    return String(NULL);
}

void BucketHelper::UpdateBucketEntriesFromTable(
    /*JobContext jc,*/
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* tableUri,
    /* [in] */ HashMap<Int32, AutoPtr<BucketEntry> >* buckets)
{
    // Cursor cursor = resolver.query(tableUri, PROJECTION_BUCKET_IN_ONE_TABLE,
    //         BUCKET_GROUP_BY_IN_ONE_TABLE, null, null);
    // if (cursor == null) {
    //     Log.w(TAG, "cannot open media database: " + tableUri);
    //     return;
    // }
    // try {
    //     while (cursor.moveToNext()) {
    //         int bucketId = cursor.getInt(INDEX_BUCKET_ID);
    //         int dateTaken = cursor.getInt(INDEX_DATE_TAKEN);
    //         BucketEntry entry = buckets.get(bucketId);
    //         if (entry == null) {
    //             entry = new BucketEntry(bucketId, cursor.getString(INDEX_BUCKET_NAME));
    //             buckets.put(bucketId, entry);
    //             entry.dateTaken = dateTaken;
    //         } else {
    //             entry.dateTaken = Math.max(entry.dateTaken, dateTaken);
    //         }
    //     }
    // } finally {
    //     Utils.closeSilently(cursor);
    // }
}

AutoPtr< ArrayOf<BucketHelper::BucketEntry*> > BucketHelper::LoadBucketEntriesFromImagesAndVideoTable(
    /*JobContext jc,*/
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32 type)
{
    // HashMap<Integer, BucketEntry> buckets = new HashMap<Integer, BucketEntry>(64);
    // if ((type & MediaObject.MEDIA_TYPE_IMAGE) != 0) {
    //     updateBucketEntriesFromTable(
    //             jc, resolver, Images.Media.EXTERNAL_CONTENT_URI, buckets);
    // }
    // if ((type & MediaObject.MEDIA_TYPE_VIDEO) != 0) {
    //     updateBucketEntriesFromTable(
    //             jc, resolver, Video.Media.EXTERNAL_CONTENT_URI, buckets);
    // }
    // BucketEntry[] entries = buckets.values().toArray(new BucketEntry[buckets.size()]);
    // Arrays.sort(entries, new Comparator<BucketEntry>() {
    //     @Override
    //     public int compare(BucketEntry a, BucketEntry b) {
    //         // sorted by dateTaken in descending order
    //         return b.dateTaken - a.dateTaken;
    //     }
    // });
    // return entries;
    return NULL;
}

AutoPtr< ArrayOf<BucketHelper::BucketEntry*> > BucketHelper::LoadBucketEntriesFromFilesTable(
    /*JobContext jc,*/
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32  type)
{
    // AutoPtr<IUri> uri = GetFilesContentUri();

    // Cursor cursor = resolver.query(uri,
    //         PROJECTION_BUCKET, BUCKET_GROUP_BY,
    //         null, BUCKET_ORDER_BY);
    // if (cursor == null) {
    //     Log.w(TAG, "cannot open local database: " + uri);
    //     return new BucketEntry[0];
    // }
    // ArrayList<BucketEntry> buffer = new ArrayList<BucketEntry>();
    // int typeBits = 0;
    // if ((type & MediaObject.MEDIA_TYPE_IMAGE) != 0) {
    //     typeBits |= (1 << FileColumns.MEDIA_TYPE_IMAGE);
    // }
    // if ((type & MediaObject.MEDIA_TYPE_VIDEO) != 0) {
    //     typeBits |= (1 << FileColumns.MEDIA_TYPE_VIDEO);
    // }
    // try {
    //     while (cursor.moveToNext()) {
    //         if ((typeBits & (1 << cursor.getInt(INDEX_MEDIA_TYPE))) != 0) {
    //             BucketEntry entry = new BucketEntry(
    //                     cursor.getInt(INDEX_BUCKET_ID),
    //                     cursor.getString(INDEX_BUCKET_NAME));
    //             if (!buffer.contains(entry)) {
    //                 buffer.add(entry);
    //                 Log.d(TAG, "YP--loadBucketEntriesFromFilesTable--id:" + entry.bucketId + ",name:" + entry.bucketName);
    //             }
    //         }
    //         if (jc.isCancelled()) return null;
    //     }
    // } finally {
    //     Utils.closeSilently(cursor);
    // }
    // return buffer.toArray(new BucketEntry[buffer.size()]);
    return NULL;
}

String BucketHelper::GetBucketNameInTable(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* tableUri,
    /* [in] */ Int32  bucketId)
{
    // String selectionArgs[] = new String[] {String.valueOf(bucketId)};
    // Uri uri = tableUri.buildUpon()
    //         .appendQueryParameter("limit", "1")
    //         .build();
    // Cursor cursor = resolver.query(uri, PROJECTION_BUCKET_IN_ONE_TABLE,
    //         "bucket_id = ?", selectionArgs, null);
    // try {
    //     if (cursor != null && cursor.moveToNext()) {
    //         return cursor.getString(INDEX_BUCKET_NAME);
    //     }
    // } finally {
    //     Utils.closeSilently(cursor);
    // }
    return String(NULL);
}

AutoPtr<IUri> BucketHelper::GetFilesContentUri()
{
    // return Files.getContentUri(EXTERNAL_MEDIA);
    return NULL;
}

} // namespace Gallery
