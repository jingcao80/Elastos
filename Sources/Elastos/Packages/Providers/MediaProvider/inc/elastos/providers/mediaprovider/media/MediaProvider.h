#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_MEDIAPROVIDER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_MEDIAPROVIDER_H__

#include "_Elastos.Providers.MediaProvider.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/providers/mediaprovider/media/MediaThumbRequest.h"
#include "Elastos.Droid.Media.h"
#include <elastos/droid/content/ContentProvider.h>
#include <elastos/droid/database/sqlite/SQLiteOpenHelper.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Object.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/Stack.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentProviderResult;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ICustomFunction;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Media::IMediaScannerConnection;
using Elastos::Droid::Media::IMediaScannerConnectionClient;
using Elastos::Droid::Media::IOnScanCompletedListener;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::Stack;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IPriorityQueue;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IStack;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Core::Object;

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

class MediaProvider
    : public ContentProvider
    , public IMediaProvider
{
    friend class DatabaseHelper;
    friend class MediaScannerReceiver;
    friend class MediaScannerService;
    friend class MtpService;

public:
    class DatabaseHelper
        : public SQLiteOpenHelper
        , public IDatabaseHelper
    {
    public:
        DatabaseHelper();

        ~DatabaseHelper();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ MediaProvider* host,
            /* [in] */ IContext* context,
            /* [in] */ const String& name,
            /* [in] */ Boolean internal,
            /* [in] */ Boolean earlyUpgrade,
            /* [in] */ ICustomFunction* objectRemovedCallback);

        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldV,
            /* [in] */ Int32 newV);

        CARAPI GetWritableDatabase(
            /* [out] */ ISQLiteDatabase** database);

        CARAPI OnOpen(
            /* [in] */ ISQLiteDatabase* db);

    public:
        AutoPtr<IContext> mContext;
        String mName;
        Boolean mInternal; // True if this is the internal database
        Boolean mEarlyUpgrade;
        AutoPtr<ICustomFunction> mObjectRemovedCallback;
        Boolean mUpgradeAttempted; // Used for upgrade error handling
        Int32 mNumQueries;
        Int32 mNumUpdates;
        Int32 mNumInserts;
        Int32 mNumDeletes;
        Int64 mScanStartTime;
        Int64 mScanStopTime;

        // In memory caches of artist and album data.
        HashMap<String, Int64> mArtistCache;
        HashMap<String, Int64> mAlbumCache;

    private:
        MediaProvider* mHost;
   };

private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ MediaProvider* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        MediaProvider* mHost;
    };

    class MySqliteDbCunstomFunction
        : public Object
        , public ICustomFunction
    {
    public:
        CAR_INTERFACE_DECL()

        MySqliteDbCunstomFunction(
            /* [in] */ MediaProvider* host);

        CARAPI Callback(
            /* [in] */ ArrayOf<String>* args);

    private:
        MediaProvider* mHost;
    };

    class MyMtpServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);
    };

    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ MediaProvider* host,
            /* [in] */ ILooper* looper,
            /* [in] */ IMessage* msg);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        MediaProvider* mHost;
        AutoPtr<ILooper> mLooper;
        AutoPtr<IMessage> mMsg;
   };

    class ScannerClient
        : public Object
        , public IOnScanCompletedListener
        , public IMediaScannerConnectionClient
    {
    public:
        CAR_INTERFACE_DECL()

        ScannerClient(
            /* [in] */ IContext* context,
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ const String& path);

        ~ScannerClient();

        CARAPI OnMediaScannerConnected();

        CARAPI OnScanCompleted(
            /* [in] */ const String& path,
            /* [in] */ IUri* uri);

    public:
        String mPath;
        AutoPtr<IMediaScannerConnection> mScannerConnection;
        AutoPtr<ISQLiteDatabase> mDb;
    };

    class ThumbData
        : public Object
        , public IThumbData
    {
    public:
        CAR_INTERFACE_DECL()

        AutoPtr<DatabaseHelper> mHelper;
        AutoPtr<ISQLiteDatabase> mDb;
        String mPath;
        Int64 mAlbum_id;
        AutoPtr<IUri> mAlbumart_uri;
    };

    class GetTableAndWhereOutParameter
        : public Object
   {
    public:
        String mTable;
        String mWhere;
    };

    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ MediaProvider* owner,
            /* [in] */ IFile* file,
            /* [in] */ DatabaseHelper* helper,
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ const String& path);

        CARAPI Run();

    private:
        MediaProvider* mOwner;
        AutoPtr<IFile> mFile;
        AutoPtr<DatabaseHelper> mHelper;
        AutoPtr<ISQLiteDatabase> mDb;
        String mPath;
    };

public:
    MediaProvider();

    virtual ~MediaProvider();

    CAR_INTERFACE_DECL()

    static CARAPI_(Int32) GetDatabaseVersion(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    static CARAPI_(void) LogToDb(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& message);

    //@Override
    CARAPI Canonicalize(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    //@Override
    CARAPI Uncanonicalize(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    // @SuppressWarnings("fallthrough")
    // @Override
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projectionIn,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sort,
        /* [out] */ ICursor** ret);

    //@Override
    CARAPI GetType(
        /* [in] */ IUri* url,
        /* [out] */ String* result);

    //@Override
    CARAPI BulkInsert(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues*>* values,
        /* [out] */ Int32* result);

    //@Override
    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** result);

    //@Override
    CARAPI ApplyBatch(
        /* [in] */ IArrayList* operations,
        /* [out] */ ArrayOf<IContentProviderResult*>** result);

    //@Override
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& userWhere,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

    //@Override
    CARAPI Call(
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** result);

    //@Override
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [in] */ const String& userWhere,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

    // Return a URI to write the album art to and update the database as necessary.
    AutoPtr<IUri> GetAlbumArtOutputUri(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int64 album_id,
        /* [in] */ IUri* albumart_uri);

    CARAPI_(String) MakeBestName(
        /* [in] */ const String& one,
        /* [in] */ const String& two);

    static CARAPI_(Boolean) IsMediaDatabaseName(
        /* [in] */ const String& name);

    static CARAPI_(Boolean) IsInternalMediaDatabaseName(
        /* [in] */ const String& name);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

private:
    // creates default folders (Music, Downloads, etc)
    CARAPI_(void) CreateDefaultFolders(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db);//

    static CARAPI_(void) UpdateDatabase(
        /* [in] */ IContext* context,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Boolean internal,
        /* [in] */ Int32 fromVersion,
        /* [in] */ Int32 toVersion);

    static CARAPI_(void) SanityCheck(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 fromVersion);

    static CARAPI_(void) RecreateAudioView(
        /* [in] */ ISQLiteDatabase* db);

    static CARAPI_(void) UpdateBucketNames(
        /* [in] */ ISQLiteDatabase* db);

    static CARAPI_(void) UpdateDisplayName(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& tableName);

    static CARAPI_(void) ComputeBucketValues(
        /* [in] */ const String& data,
        /* [in] */ IContentValues* values);

    static CARAPI_(void) ComputeDisplayName(
        /* [in] */ const String& data,
        /* [in] */ IContentValues* values);

    static CARAPI_(void) ComputeTakenTime(
        /* [in] */ IContentValues* values);

    CARAPI_(Boolean) WaitForThumbnailReady(
        /* [in] */ IUri* origUri);

    CARAPI_(Boolean) MatchThumbRequest(
        /* [in] */ MediaThumbRequest* req,
        /* [in] */ Int32 pid,
        /* [in] */ Int64 id,
        /* [in] */ Int64 gid,
        /* [in] */ Boolean isVideo);

    CARAPI_(Boolean) QueryThumbnail(
        /* [in] */ ISQLiteQueryBuilder* qb,
        /* [in] */ IUri* uri,
        /* [in] */ const String& table,
        /* [in] */ const String& column,
        /* [in] */ Boolean hasThumbnailId);

    CARAPI_(AutoPtr<IUri>) SafeUncanonicalize(
        /* [in] */ IUri* uri);

    CARAPI_(AutoPtr<ArrayOf<String> >) Combine(
        /* [in] */ List<String>& prepend,
        /* [in] */ ArrayOf<String>* userArgs);

    CARAPI_(AutoPtr<ICursor>) DoAudioSearch(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ ISQLiteQueryBuilder* qb,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projectionIn,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sort,
        /* [in] */ Int32 mode,
        /* [in] */ const String& limit);

    CARAPI EnsureFile(
        /* [in] */ Boolean internal,
        /* [in] */ IContentValues* initialValues,
        /* [in] */ const String& preferredExtension,
        /* [in] */ const String& directoryName,
        /* [out] */ IContentValues** result);

    CARAPI_(void) SendObjectAdded(
        /* [in] */ Int64 objectHandle);

    CARAPI_(void) SendObjectRemoved(
        /* [in] */ Int64 objectHandle);

    CARAPI_(void) NotifyMtp(
        /* [in] */ IArrayList* rowIds);

    CARAPI_(Int32) PlaylistBulkInsert(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues*>* values);

    CARAPI_(Int64) InsertDirectory(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& path);

    CARAPI_(Int64) GetParent(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& path);

    CARAPI_(Int32) GetStorageId(
        /* [in] */ const String& path);

    CARAPI InsertFile(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [in] */ Int32 mediaType,
        /* [in] */ Boolean notify,
        /* [in] */ IArrayList* notifyRowIds,
        /* [out] */ Int64* result);

    CARAPI_(AutoPtr<ICursor>) GetObjectReferences(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 handle);

    CARAPI_(Int32) SetObjectReferences(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 handle,
        /* [in] */ ArrayOf<IContentValues*>* values);

    CARAPI_(void) UpdateGenre(
        /* [in] */ Int64 rowId,
        /* [in] */ const String& genre);

    CARAPI InsertInternal(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 match,
        /* [in] */ IContentValues* initialValues,
        /* [in] */ IArrayList* notifyRowIds,
        /* [out] */ IUri** result);

    CARAPI_(void) ProcessNewNoMediaPath(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& path);

    CARAPI_(void) HidePath(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& path);

    CARAPI_(void) ProcessRemovedNoMediaPath(
        /* [in] */ const String& path);

    CARAPI_(AutoPtr<MediaThumbRequest>) RequestMediaThumbnail(
        /* [in] */ const String& path,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 priority,
        /* [in] */ Int64 magic);

    CARAPI_(String) GenerateFileName(
        /* [in] */ Boolean internal,
        /* [in] */ const String& preferredExtension,
        /* [in] */ const String& directoryName);

    CARAPI_(Boolean) EnsureFileExists(
        /* [in] */ IUri* uri,
        /* [in] */ const String& path);

    CARAPI_(void) GetTableAndWhere(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 match,
        /* [in] */ const String& userWhere,
        /* [in] */ GetTableAndWhereOutParameter* out);

    CARAPI_(Int32) MovePlaylistEntry(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int64 playlist,
        /* [in] */ Int32 from,
        /* [in] */ Int32 to);

    CARAPI OpenFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** result);

    CARAPI_(AutoPtr<IFile>) QueryForDataFile(
        /* [in] */ IUri* uri);

    CARAPI OpenFileAndEnforcePathPermissionsHelper(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** result);

    CARAPI_(void) DeleteIfAllowed(
        /* [in] */ IUri* uri,
        /* [in] */ const String& path);

    CARAPI_(void) CheckAccess(
        /* [in] */ IUri* uri,
        /* [in] */ IFile* file,
        /* [in] */ Int32 modeBits);

    CARAPI_(Boolean) IsSecondaryExternalPath(
        /* [in] */ const String& path);

    CARAPI_(void) CheckWorldReadAccess(
        /* [in] */ const String& path);

    CARAPI_(void) CheckLeadingPathComponentsWorldExecutable(
        /* [in] */ const String& filePath);//

    CARAPI_(void) MakeThumbAsync(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& path,
        /* [in] */ Int64 album_id);

    //Return true if the artPath is the dir as it in mExternalStoragePaths
    //for multi storage support
    static CARAPI_(Boolean) IsRootStorageDir(
        /* [in] */ const String& artPath);

    // Extract compressed image data from the audio file itself or, if that fails,
    // look for a file "AlbumArt.jpg" in the containing directory.
    static AutoPtr<ArrayOf<Byte> > GetCompressedAlbumArt(
        /* [in] */ IContext* context,
        /* [in] */ const String& path);

    // Write out the album art to the output URI, recompresses the given Bitmap
    // if necessary, otherwise writes the compressed data.
    CARAPI_(void) WriteAlbumArt(
        /* [in] */ Boolean need_to_recompress,
        /* [in] */ IUri* out,
        /* [in] */ ArrayOf<Byte>* compressed,
        /* [in] */ IBitmap* bm);

    CARAPI_(AutoPtr<IParcelFileDescriptor>) GetThumb(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& path,
        /* [in] */ Int64 album_id,
        /* [in] */ IUri* albumart_uri);

    CARAPI_(AutoPtr<IParcelFileDescriptor>) MakeThumbInternal(
        /* [in] */ ThumbData* d);

    Int64 GetKeyIdForName(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ const String& keyField,
        /* [in] */ const String& nameField,
        /* [in] */ const String& rawName,
        /* [in] */ const String& cacheName,
        /* [in] */ const String& path,
        /* [in] */ Int32 albumHash,
        /* [in] */ const String& artist,
        /* [in] */ HashMap<String, Int64>& cache,
        /* [in] */ IUri* srcuri);

    CARAPI_(AutoPtr<IUri>) AttachVolume(
        /* [in] */ const String& volume);

    CARAPI GetDatabaseForUri(
        /* [in] */ IUri* uri,
        /* [out] */ IDatabaseHelper** result);

    CARAPI_(void) DetachVolume(
        /* [in]*/ IUri* uri);

    CARAPI_(String) GetVolumeName(
        /* [in] */ IUri* uri);

    CARAPI_(String) Dump(
        /* [in] */ IDatabaseHelper* dbh,
        /* [in] */ Boolean dumpDbLog);

    CARAPI initStaticBlock();

    CARAPI initStaticBlock2();

    CARAPI_(Boolean) initHashMapWithString(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [in] */ IHashMap* hp);

private:
    static const AutoPtr<IUri> MEDIA_URI;
    static const AutoPtr<IUri> ALBUMART_URI;
    static const Int32 ALBUM_THUMB;
    static const Int32 IMAGE_THUMB;

    static AutoPtr<IHashMap> sArtistAlbumsMap;
    static AutoPtr<IHashMap> sFolderArtMap;

    /** Resolved canonical path to external storage. */
    static String sExternalPath;
    /** Resolved canonical path to cache storage. */
    static String sCachePath;
    /** Resolved canonical path to legacy storage. */
    static String sLegacyPath;//

    AutoPtr<IStorageManager> mStorageManager;//

    // In memory cache of path<->id mappings, to speed up inserts during media scan
    AutoPtr<HashMap<String, Int64> > mDirectoryCache;//

    // A HashSet of paths that are pending creation of album art thumbnails.
    static AutoPtr<IHashSet> mPendingThumbs;

    // A Stack of outstanding thumbnail requests.
    static AutoPtr<IStack> mThumbRequestStack;

    // The lock of mMediaThumbQueue protects both mMediaThumbQueue and mCurrentThumbRequest.
    static AutoPtr<IMediaThumbRequest> mCurrentThumbRequest;
    static AutoPtr<IPriorityQueue> mMediaThumbQueue;

    Boolean mCaseInsensitivePaths;
    static AutoPtr<ArrayOf<String> > mExternalStoragePaths;

    // For compatibility with the approximately 0 apps that used mediaprovider search in
    // releases 1.0, 1.1 or 1.5
    AutoPtr<ArrayOf<String> > mSearchColsLegacy;
    AutoPtr<ArrayOf<String> > mSearchColsFancy;
    // If this array gets changed, please update the constant below to point to the correct item.
    AutoPtr<ArrayOf<String> > mSearchColsBasic;
    // Position of the TEXT_2 item in the above array.
    const Int32 SEARCH_COLUMN_BASIC_TEXT2;

    static const AutoPtr<ArrayOf<String> > sMediaTableColumns;

    static const AutoPtr<ArrayOf<String> > sIdOnlyColumn;

    static const AutoPtr<ArrayOf<String> > sDataOnlyColumn;

    static const AutoPtr<ArrayOf<String> > sMediaTypeDataId;

    static const AutoPtr<ArrayOf<String> > sPlaylistIdPlayOrder;

    AutoPtr<IUri> mAlbumArtBaseUri;

    static const String CANONICAL;

    AutoPtr<IBroadcastReceiver> mUnmountReceiver;

    // set to disable sending events when the operation originates from MTP
    static Boolean mDisableMtpObjectCallbacks;

    AutoPtr<ICustomFunction> mObjectRemovedCallback;

    // synchronize on mMtpServiceConnection when accessing mMtpService
    static AutoPtr<IIMtpService> mMtpService;

    AutoPtr<IServiceConnection> mMtpServiceConnection;

    static const AutoPtr<ArrayOf<String> > sDefaultFolderNames;

    static const String TABLE_FILES;
    static const String TABLE_ALBUM_ART;
    static const String TABLE_THUMBNAILS;
    static const String TABLE_VIDEO_THUMBNAILS;

    static const String IMAGE_COLUMNS;

    static const String IMAGE_COLUMNSv407;

    static const String AUDIO_COLUMNSv99;

    static const String AUDIO_COLUMNSv100;

    static const String AUDIO_COLUMNSv405;

    static const String VIDEO_COLUMNS;

    static const String VIDEO_COLUMNSv407;

    static const String PLAYLIST_COLUMNS ;

    static const AutoPtr<ArrayOf<String> > GENRE_LOOKUP_PROJECTION;

    /*static*/ const AutoPtr<GetTableAndWhereOutParameter> sGetTableAndWhereParam;

    static const AutoPtr<ArrayOf<String> > openFileColumns;

    static String TAG;
    static const Boolean LOCAL_LOGV;

    static const String INTERNAL_DATABASE_NAME;
    static const String EXTERNAL_DATABASE_NAME;

    // maximum number of cached external databases to keep
    static const Int32 MAX_EXTERNAL_DATABASES;

    static const Int64 OBSOLETE_DATABASE_DB;

    static AutoPtr<IHashMap> mDatabases; // HashMap<String, AutoPtr<DatabaseHelper> >

    AutoPtr<IHandler> mThumbHandler;

    // name of the volume currently being scanned by the media scanner (or null)
    String mMediaScannerVolume;

    // current FAT volume ID
    Int32 mVolumeId;

    static const String INTERNAL_VOLUME;
    static const String EXTERNAL_VOLUME;
    static const String ALBUM_THUMB_FOLDER;

    // path for writing contents of in memory temp database
    String mTempDatabasePath;

    // WARNING: the values of IMAGES_MEDIA, AUDIO_MEDIA, and VIDEO_MEDIA and AUDIO_PLAYLISTS
    // are stored in the "files" table, so do not renumber them unless you also add
    // a corresponding database upgrade step for it.
    static const Int32 IMAGES_MEDIA;
    static const Int32 IMAGES_MEDIA_ID;
    static const Int32 IMAGES_THUMBNAILS;
    static const Int32 IMAGES_THUMBNAILS_ID;

    static const Int32 AUDIO_MEDIA;
    static const Int32 AUDIO_MEDIA_ID;
    static const Int32 AUDIO_MEDIA_ID_GENRES;
    static const Int32 AUDIO_MEDIA_ID_GENRES_ID;
    static const Int32 AUDIO_MEDIA_ID_PLAYLISTS;
    static const Int32 AUDIO_MEDIA_ID_PLAYLISTS_ID;
    static const Int32 AUDIO_GENRES;
    static const Int32 AUDIO_GENRES_ID;
    static const Int32 AUDIO_GENRES_ID_MEMBERS;
    static const Int32 AUDIO_GENRES_ALL_MEMBERS;
    static const Int32 AUDIO_PLAYLISTS;
    static const Int32 AUDIO_PLAYLISTS_ID;
    static const Int32 AUDIO_PLAYLISTS_ID_MEMBERS;
    static const Int32 AUDIO_PLAYLISTS_ID_MEMBERS_ID;
    static const Int32 AUDIO_ARTISTS;
    static const Int32 AUDIO_ARTISTS_ID;
    static const Int32 AUDIO_ALBUMS;
    static const Int32 AUDIO_ALBUMS_ID;
    static const Int32 AUDIO_ARTISTS_ID_ALBUMS;
    static const Int32 AUDIO_ALBUMART;
    static const Int32 AUDIO_ALBUMART_ID;
    static const Int32 AUDIO_ALBUMART_FILE_ID;

    static const Int32 VIDEO_MEDIA;
    static const Int32 VIDEO_MEDIA_ID;
    static const Int32 VIDEO_THUMBNAILS;
    static const Int32 VIDEO_THUMBNAILS_ID;

    static const Int32 VOLUMES;
    static const Int32 VOLUMES_ID;

    static const Int32 AUDIO_SEARCH_LEGACY;
    static const Int32 AUDIO_SEARCH_BASIC;
    static const Int32 AUDIO_SEARCH_FANCY;

    static const Int32 MEDIA_SCANNER;

    static const Int32 FS_ID;
    static const Int32 VERSION;

    static const Int32 FILES;
    static const Int32 FILES_ID;

    // Used only by the MTP implementation
    static const Int32 MTP_OBJECTS;
    static const Int32 MTP_OBJECTS_ID;
    static const Int32 MTP_OBJECT_REFERENCES;
    // UsbReceiver calls insert() and delete() with this URI to tell us
    // when MTP is connected and disconnected
    static const Int32 MTP_CONNECTED;

    static AutoPtr<IUriMatcher> URI_MATCHER;

    static AutoPtr<ArrayOf<String> > ID_PROJECTION;

    static AutoPtr<ArrayOf<String> > PATH_PROJECTION;

    static AutoPtr<ArrayOf<String> > MIME_TYPE_PROJECTION;

    static AutoPtr<ArrayOf<String> > READY_FLAG_PROJECTION;

    static const String OBJECT_REFERENCES_QUERY;

    static ECode sStaticBlock2;
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_MEDIAPROVIDER_H__