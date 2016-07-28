#include "elastos/droid/providers/media/MediaProvider.h"
#include "elastos/droid/providers/media/MediaDocumentsProvider.h"
#include "elastos/droid/providers/media/MediaThumbRequest.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/Manifest.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "R.h"
#include <elastos/droid/content/IntentFilter.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/HandlerThread.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/system/Os.h>
#include <elastos/droid/system/OsConstants.h>
#include <elastos/droid/text/format/DateUtils.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Object.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IntentFilter;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::CDatabaseInsertHelper;
using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDatabaseInsertHelper;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::EIID_ICustomFunction;
using Elastos::Droid::Database::Sqlite::EIID_ISQLiteOpenHelper;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::BitmapConfig_RGB_565;
using Elastos::Droid::Graphics::BitmapCompressFormat_JPEG;
using Elastos::Droid::Manifest;
using Elastos::Droid::Media::CMediaScanner;
using Elastos::Droid::Media::CMediaScannerHelper;
using Elastos::Droid::Media::CMiniThumbFileHelper;
using Elastos::Droid::Media::CMediaScannerConnection;
using Elastos::Droid::Media::IMediaScannerHelper;
using Elastos::Droid::Media::IMediaScanner;
using Elastos::Droid::Media::IMiniThumbFileHelper;
using Elastos::Droid::Media::CMediaFile;
using Elastos::Droid::Media::IMediaFile;
using Elastos::Droid::Media::EIID_IMediaScannerConnectionClient;
using Elastos::Droid::Media::EIID_IOnScanCompletedListener;
using Elastos::Droid::Mtp::CMtpStorageHelper;
using Elastos::Droid::Mtp::IMtpConstants;
using Elastos::Droid::Mtp::IMtpStorageHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Provider::CMediaStore;
using Elastos::Droid::Provider::CMediaStoreAudio;
using Elastos::Droid::Provider::CMediaStoreAudioAlbums;
using Elastos::Droid::Provider::CMediaStoreAudioGenres;
using Elastos::Droid::Provider::CMediaStoreAudioMedia;
using Elastos::Droid::Provider::CMediaStoreAudioPlaylists;
using Elastos::Droid::Provider::CMediaStoreFiles;
using Elastos::Droid::Provider::CMediaStoreImagesMedia;
using Elastos::Droid::Provider::CMediaStoreVideoMedia;
using Elastos::Droid::Provider::CMediaStoreVideoThumbnails;
using Elastos::Droid::Provider::CMediaStoreImagesThumbnails;
using Elastos::Droid::Provider::CMediaStoreAudioPlaylistsMembers;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::IMediaStoreAudio;
using Elastos::Droid::Provider::IMediaStoreAudioAlbums;
using Elastos::Droid::Provider::IMediaStoreAudioAlbumColumns;
using Elastos::Droid::Provider::IMediaStoreAudioArtistColumns;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreAudioGenresColumns;
using Elastos::Droid::Provider::IMediaStoreAudioGenresMembers;
using Elastos::Droid::Provider::IMediaStoreAudioPlaylistsColumns;
using Elastos::Droid::Provider::IMediaStoreAudioPlaylists;
using Elastos::Droid::Provider::IMediaStoreFiles;
using Elastos::Droid::Provider::IMediaStoreFilesFileColumns;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Provider::IMediaStoreVideoMedia;
using Elastos::Droid::Provider::IMediaStoreAudioGenres;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreAudioAudioColumns;
using Elastos::Droid::Provider::IMediaStoreImagesImageColumns;
using Elastos::Droid::Provider::IMediaStoreAudioPlaylistsMembers;
using Elastos::Droid::Provider::IMediaStoreImagesMedia;
using Elastos::Droid::Provider::IMediaStoreImagesThumbnails;
using Elastos::Droid::Provider::IMediaStoreVideoThumbnails;
using Elastos::Droid::System::IStructStat;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::Os;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CPriorityQueue;
using Elastos::Utility::CStack;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::CFileHelper;
using Elastos::IO::IFileHelper;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

static AutoPtr<IUri> InitUri(const char* uriStr)
{
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    uh->Parse(String(uriStr), (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> MediaProvider::MEDIA_URI = InitUri("content://media");
const AutoPtr<IUri> MediaProvider::ALBUMART_URI = InitUri("content://media/external/audio/albumart");
const Int32 MediaProvider::ALBUM_THUMB = 1;
const Int32 MediaProvider::IMAGE_THUMB = 2;//

static AutoPtr<IHashMap> CreateHashMap()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return map;
}
AutoPtr<IHashMap> MediaProvider::sArtistAlbumsMap = CreateHashMap();
AutoPtr<IHashMap> MediaProvider::sFolderArtMap = CreateHashMap();

String MediaProvider::sExternalPath;
String MediaProvider::sCachePath;
String MediaProvider::sLegacyPath;//

const AutoPtr<MediaProvider::GetTableAndWhereOutParameter> MediaProvider::sGetTableAndWhereParam
    = new MediaProvider::GetTableAndWhereOutParameter();

String MediaProvider::GetExternalPath()
{
    if (sExternalPath.IsNull()) {
        ECode ec = NOERROR;
        AutoPtr<IFile> file = Environment::GetExternalStorageDirectory();
        String canonicalPath;
        ec = file->GetCanonicalPath(&canonicalPath);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Logger::E(TAG, "Unable to resolve canonical paths");
            return String(NULL);
        }
        AutoPtr<IFileHelper> fh;
        CFileHelper::AcquireSingleton((IFileHelper**)&fh);
        String separator;
        fh->GetSeparator(&separator);
        sExternalPath = canonicalPath + separator;
    }
    return sExternalPath;
}

String MediaProvider::GetCachePath()
{
    if (sCachePath.IsNull()) {
        ECode ec = NOERROR;
        AutoPtr<IFile> file = Environment::GetDownloadCacheDirectory();
        String canonicalPath;
        ec = file->GetCanonicalPath(&canonicalPath);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Logger::E(TAG, "Unable to resolve canonical paths");
            return String(NULL);
        }
        AutoPtr<IFileHelper> fh;
        CFileHelper::AcquireSingleton((IFileHelper**)&fh);
        String separator;
        fh->GetSeparator(&separator);
        sCachePath = canonicalPath + separator;
    }
    return sCachePath;
}

String MediaProvider::GetLegacyPath()
{
    if (sLegacyPath.IsNull()) {
        ECode ec = NOERROR;
        AutoPtr<IFile> file = Environment::GetLegacyExternalStorageDirectory();
        String canonicalPath;
        ec = file->GetCanonicalPath(&canonicalPath);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Logger::E(TAG, "Unable to resolve canonical paths");
            return String(NULL);
        }
        AutoPtr<IFileHelper> fh;
        CFileHelper::AcquireSingleton((IFileHelper**)&fh);
        String separator;
        fh->GetSeparator(&separator);
        sLegacyPath = canonicalPath + separator;
    }
    return sLegacyPath;
}

AutoPtr<ArrayOf<String> > MediaProvider::mExternalStoragePaths;//

static AutoPtr<ArrayOf<String> > InitSearchColsLegacy()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(12);
    (*array)[0] = IBaseColumns::ID;
    (*array)[1] = IMediaStoreMediaColumns::MIME_TYPE;

    StringBuilder sb("(CASE WHEN grouporder=1 THEN ");  sb += R::drawable::ic_search_category_music_artist;
    sb += " ELSE CASE WHEN grouporder=2 THEN ";         sb += R::drawable::ic_search_category_music_album;
    sb += " ELSE ";                                     sb += R::drawable::ic_search_category_music_song;
    sb += " END END) AS ";                              sb += ISearchManager::SUGGEST_COLUMN_ICON_1;
    (*array)[2] = sb.ToString();

    (*array)[3] = String("0 AS ") + ISearchManager::SUGGEST_COLUMN_ICON_2;
    (*array)[4] = String("text1 AS ") + ISearchManager::SUGGEST_COLUMN_TEXT_1,
    (*array)[5] = String("text1 AS ") + ISearchManager::SUGGEST_COLUMN_QUERY,
    (*array)[6] = String("CASE when grouporder=1 THEN data1 ELSE artist END AS data1");
    (*array)[7] = String("CASE when grouporder=1 THEN data2 ELSE "
                    "CASE WHEN grouporder=2 THEN NULL ELSE album END END AS data2");
    (*array)[8] = String("match as ar");
    (*array)[9] = ISearchManager::SUGGEST_COLUMN_INTENT_DATA;
    (*array)[10] = String("grouporder");
    (*array)[11] = String("NULL AS itemorder"); // We should be sorting by the artist/album/title keys, but that
                                               // column is not available here, and the list is already sorted.
    return array;
}

static AutoPtr<ArrayOf<String> > InitSearchColsFancy()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(7);
    (*array)[0] = IBaseColumns::ID;
    (*array)[1] = IMediaStoreMediaColumns::MIME_TYPE;
    (*array)[2] = IMediaStoreAudioArtistColumns::ARTIST;
    (*array)[3] = IMediaStoreAudioAlbumColumns::ALBUM;
    (*array)[4] = IMediaStoreMediaColumns::TITLE,
    (*array)[5] = String("data1");
    (*array)[6] = String("data2");
    return array;
}//

static AutoPtr<ArrayOf<String> > InitSearchColsBasic()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(7);
    (*array)[0] = IBaseColumns::ID;
    (*array)[1] = IMediaStoreMediaColumns::MIME_TYPE;

    StringBuilder sb("(CASE WHEN grouporder=1 THEN ");  sb += R::drawable::ic_search_category_music_artist;
    sb += " ELSE CASE WHEN grouporder=2 THEN ";         sb += R::drawable::ic_search_category_music_album;
    sb += " ELSE ";                                     sb += R::drawable::ic_search_category_music_song;
    sb += " END END) AS ";                              sb += ISearchManager::SUGGEST_COLUMN_ICON_1;
    (*array)[2] = sb.ToString();
    (*array)[3] = String("text1 AS ") + ISearchManager::SUGGEST_COLUMN_TEXT_1;
    (*array)[4] = String("text1 AS ") + ISearchManager::SUGGEST_COLUMN_QUERY;

    StringBuilder sb1("(CASE WHEN grouporder=1 THEN \'%1\'"); // %1 gets replaced with localized string.
    sb1 += " ELSE CASE WHEN grouporder=3 THEN artist || ' - ' || album";
    sb1 += " ELSE CASE WHEN text2!='";                   sb += IMediaStore::UNKNOWN_STRING;
    sb1 += "\' THEN text2";
    sb1 += " ELSE NULL END END END) AS ";                sb += ISearchManager::SUGGEST_COLUMN_TEXT_2;
    (*array)[5] = sb1.ToString();
    (*array)[6] = ISearchManager::SUGGEST_COLUMN_INTENT_DATA;
    return array;
}

static AutoPtr<ArrayOf<String> > InitMediaTableColumns()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = IBaseColumns::ID;
    (*array)[1] = IMediaStoreFilesFileColumns::MEDIA_TYPE;
    return array;
}

const AutoPtr<ArrayOf<String> > MediaProvider::sMediaTableColumns = InitMediaTableColumns();//

static AutoPtr<ArrayOf<String> > InitIdOnlyColumn()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = IBaseColumns::ID;
    return array;
}

const AutoPtr<ArrayOf<String> > MediaProvider::sIdOnlyColumn = InitIdOnlyColumn();//

static AutoPtr<ArrayOf<String> > InitDataOnlyColumn()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = IMediaStoreMediaColumns::DATA;
    return array;
}

const AutoPtr<ArrayOf<String> > MediaProvider::sDataOnlyColumn = InitDataOnlyColumn();//

static AutoPtr<ArrayOf<String> > InitMediaTypeDataId()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    (*array)[0] = IMediaStoreFilesFileColumns::MEDIA_TYPE;
    (*array)[1] = IMediaStoreMediaColumns::DATA;
    (*array)[2] = IBaseColumns::ID;
    return array;
}

const AutoPtr<ArrayOf<String> > MediaProvider::sMediaTypeDataId = InitMediaTypeDataId();//

static AutoPtr<ArrayOf<String> > InitPlaylistIdPlayOrder()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = IMediaStoreAudioPlaylistsMembers::PLAYLIST_ID;
    (*array)[1] = IMediaStoreAudioPlaylistsMembers::PLAY_ORDER;
    return array;
}

const AutoPtr<ArrayOf<String> > MediaProvider::sPlaylistIdPlayOrder = InitPlaylistIdPlayOrder();//

const String MediaProvider::CANONICAL("canonical");//

static AutoPtr<ArrayOf<String> > InitDefaultFolderNames()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(9);
    (*arr)[0] = Environment::DIRECTORY_MUSIC;
    (*arr)[1] = Environment::DIRECTORY_PODCASTS;
    (*arr)[2] = Environment::DIRECTORY_RINGTONES;
    (*arr)[3] = Environment::DIRECTORY_ALARMS;
    (*arr)[4] = Environment::DIRECTORY_NOTIFICATIONS;
    (*arr)[5] = Environment::DIRECTORY_PICTURES;
    (*arr)[6] = Environment::DIRECTORY_MOVIES;
    (*arr)[7] = Environment::DIRECTORY_DOWNLOADS;
    (*arr)[8] = Environment::DIRECTORY_DCIM;
    return arr;
}

const AutoPtr<ArrayOf<String> > MediaProvider::sDefaultFolderNames = InitDefaultFolderNames();//

const String MediaProvider::TABLE_FILES("files");
const String MediaProvider::TABLE_ALBUM_ART("album_art");
const String MediaProvider::TABLE_THUMBNAILS("thumbnails");
const String MediaProvider::TABLE_VIDEO_THUMBNAILS("videothumbnails");

const String MediaProvider::IMAGE_COLUMNS(
    "_data,_size,_display_name,mime_type,title,date_added,"
    "date_modified,description,picasa_id,isprivate,latitude,longitude,"
    "datetaken,orientation,mini_thumb_magic,bucket_id,bucket_display_name,"
    "width,height");

const String MediaProvider::IMAGE_COLUMNSv407(
    "_data,_size,_display_name,mime_type,title,date_added,"
    "date_modified,description,picasa_id,isprivate,latitude,longitude,"
    "datetaken,orientation,mini_thumb_magic,bucket_id,bucket_display_name");//

const String MediaProvider::AUDIO_COLUMNSv99(
    "_data,_display_name,_size,mime_type,date_added,"
    "date_modified,title,title_key,duration,artist_id,composer,album_id,"
    "track,year,is_ringtone,is_music,is_alarm,is_notification,is_podcast,"
    "bookmark");

const String MediaProvider::AUDIO_COLUMNSv100(
    "_data,_display_name,_size,mime_type,date_added,"
    "date_modified,title,title_key,duration,artist_id,composer,album_id,"
    "track,year,is_ringtone,is_music,is_alarm,is_notification,is_podcast,"
    "bookmark,album_artist");

const String MediaProvider::AUDIO_COLUMNSv405(
    "_data,_display_name,_size,mime_type,date_added,is_drm,"
    "date_modified,title,title_key,duration,artist_id,composer,album_id,"
    "track,year,is_ringtone,is_music,is_alarm,is_notification,is_podcast,"
    "bookmark,album_artist");

const String MediaProvider::VIDEO_COLUMNS(
    "_data,_display_name,_size,mime_type,date_added,date_modified,"
    "title,duration,artist,album,resolution,description,isprivate,tags,"
    "category,language,mini_thumb_data,latitude,longitude,datetaken,"
    "mini_thumb_magic,bucket_id,bucket_display_name,bookmark,width,"
    "height");

const String MediaProvider::VIDEO_COLUMNSv407(
    "_data,_display_name,_size,mime_type,date_added,date_modified,"
    "title,duration,artist,album,resolution,description,isprivate,tags,"
    "category,language,mini_thumb_data,latitude,longitude,datetaken,"
    "mini_thumb_magic,bucket_id,bucket_display_name, bookmark");

const String MediaProvider::PLAYLIST_COLUMNS("_data,name,date_added,date_modified");//

static AutoPtr<ArrayOf<String> > InitGENRE_LOOKUP_PROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreAudioGenresColumns::NAME;
    return arr;
}

const AutoPtr<ArrayOf<String> > MediaProvider::GENRE_LOOKUP_PROJECTION = InitGENRE_LOOKUP_PROJECTION();

static AutoPtr<ArrayOf<String> > InitopenFileColumns()
{
    AutoPtr<ArrayOf<String> > openFileColumns = ArrayOf<String>::Alloc(1);
    (*openFileColumns)[0] = IMediaStoreMediaColumns::DATA;
    return openFileColumns;
}

const AutoPtr<ArrayOf<String> > MediaProvider::openFileColumns = InitopenFileColumns();//

String MediaProvider::TAG("MediaProvider");
const Boolean MediaProvider::LOCAL_LOGV = FALSE;

const String MediaProvider::INTERNAL_DATABASE_NAME("internal.db");
const String MediaProvider::EXTERNAL_DATABASE_NAME("external.db");//

const Int32 MediaProvider::MAX_EXTERNAL_DATABASES = 3;

const Int64 MediaProvider::OBSOLETE_DATABASE_DB = 5184000000L;

const String MediaProvider::INTERNAL_VOLUME("internal");
const String MediaProvider::EXTERNAL_VOLUME("external");
const String MediaProvider::ALBUM_THUMB_FOLDER("Android/data/com.android.providers.media/albumthumbs");//

const Int32 MediaProvider::IMAGES_MEDIA = 1;
const Int32 MediaProvider::IMAGES_MEDIA_ID = 2;
const Int32 MediaProvider::IMAGES_THUMBNAILS = 3;
const Int32 MediaProvider::IMAGES_THUMBNAILS_ID = 4;

const Int32 MediaProvider::AUDIO_MEDIA = 100;
const Int32 MediaProvider::AUDIO_MEDIA_ID = 101;
const Int32 MediaProvider::AUDIO_MEDIA_ID_GENRES = 102;
const Int32 MediaProvider::AUDIO_MEDIA_ID_GENRES_ID = 103;
const Int32 MediaProvider::AUDIO_MEDIA_ID_PLAYLISTS = 104;
const Int32 MediaProvider::AUDIO_MEDIA_ID_PLAYLISTS_ID = 105;
const Int32 MediaProvider::AUDIO_GENRES = 106;
const Int32 MediaProvider::AUDIO_GENRES_ID = 107;
const Int32 MediaProvider::AUDIO_GENRES_ID_MEMBERS = 108;
const Int32 MediaProvider::AUDIO_GENRES_ALL_MEMBERS = 109;
const Int32 MediaProvider::AUDIO_PLAYLISTS = 110;
const Int32 MediaProvider::AUDIO_PLAYLISTS_ID = 111;
const Int32 MediaProvider::AUDIO_PLAYLISTS_ID_MEMBERS = 112;
const Int32 MediaProvider::AUDIO_PLAYLISTS_ID_MEMBERS_ID = 113;
const Int32 MediaProvider::AUDIO_ARTISTS = 114;
const Int32 MediaProvider::AUDIO_ARTISTS_ID = 115;
const Int32 MediaProvider::AUDIO_ALBUMS = 116;
const Int32 MediaProvider::AUDIO_ALBUMS_ID = 117;
const Int32 MediaProvider::AUDIO_ARTISTS_ID_ALBUMS = 118;
const Int32 MediaProvider::AUDIO_ALBUMART = 119;
const Int32 MediaProvider::AUDIO_ALBUMART_ID = 120;
const Int32 MediaProvider::AUDIO_ALBUMART_FILE_ID = 121;

const Int32 MediaProvider::VIDEO_MEDIA = 200;
const Int32 MediaProvider::VIDEO_MEDIA_ID = 201;
const Int32 MediaProvider::VIDEO_THUMBNAILS = 202;
const Int32 MediaProvider::VIDEO_THUMBNAILS_ID = 203;

const Int32 MediaProvider::VOLUMES = 300;
const Int32 MediaProvider::VOLUMES_ID = 301;

const Int32 MediaProvider::AUDIO_SEARCH_LEGACY = 400;
const Int32 MediaProvider::AUDIO_SEARCH_BASIC = 401;
const Int32 MediaProvider::AUDIO_SEARCH_FANCY = 402;

const Int32 MediaProvider::MEDIA_SCANNER = 500;

const Int32 MediaProvider::FS_ID = 600;
const Int32 MediaProvider::VERSION = 601;

const Int32 MediaProvider::FILES = 700;
const Int32 MediaProvider::FILES_ID = 701;

const Int32 MediaProvider::MTP_OBJECTS = 702;
const Int32 MediaProvider::MTP_OBJECTS_ID = 703;
const Int32 MediaProvider::MTP_OBJECT_REFERENCES = 704;

const Int32 MediaProvider::MTP_CONNECTED = 705;

const Int32 MediaProvider::MEDIA_BOOKMARK = 1101;
const Int32 MediaProvider::MEDIA_BOOKMARK_ID = 1102;

AutoPtr<IUriMatcher> MediaProvider::GetURI_MATCHER()
{
    if (URI_MATCHER == NULL) {
        AutoPtr<IUriMatcher> um;
        CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&um);

        um->AddURI(String("media"), String("*/images/media"), IMAGES_MEDIA);
        um->AddURI(String("media"), String("*/images/media/#"), IMAGES_MEDIA_ID);
        um->AddURI(String("media"), String("*/images/thumbnails"), IMAGES_THUMBNAILS);
        um->AddURI(String("media"), String("*/images/thumbnails/#"), IMAGES_THUMBNAILS_ID);

        um->AddURI(String("media"), String("*/audio/media"), AUDIO_MEDIA);
        um->AddURI(String("media"), String("*/audio/media/#"), AUDIO_MEDIA_ID);
        um->AddURI(String("media"), String("*/audio/media/#/genres"), AUDIO_MEDIA_ID_GENRES);
        um->AddURI(String("media"), String("*/audio/media/#/genres/#"), AUDIO_MEDIA_ID_GENRES_ID);
        um->AddURI(String("media"), String("*/audio/media/#/playlists"), AUDIO_MEDIA_ID_PLAYLISTS);
        um->AddURI(String("media"), String("*/audio/media/#/playlists/#"), AUDIO_MEDIA_ID_PLAYLISTS_ID);
        um->AddURI(String("media"), String("*/audio/genres"), AUDIO_GENRES);
        um->AddURI(String("media"), String("*/audio/genres/#"), AUDIO_GENRES_ID);
        um->AddURI(String("media"), String("*/audio/genres/#/members"), AUDIO_GENRES_ID_MEMBERS);
        um->AddURI(String("media"), String("*/audio/genres/all/members"), AUDIO_GENRES_ALL_MEMBERS);
        um->AddURI(String("media"), String("*/audio/playlists"), AUDIO_PLAYLISTS);
        um->AddURI(String("media"), String("*/audio/playlists/#"), AUDIO_PLAYLISTS_ID);
        um->AddURI(String("media"), String("*/audio/playlists/#/members"), AUDIO_PLAYLISTS_ID_MEMBERS);
        um->AddURI(String("media"), String("*/audio/playlists/#/members/#"), AUDIO_PLAYLISTS_ID_MEMBERS_ID);
        um->AddURI(String("media"), String("*/audio/artists"), AUDIO_ARTISTS);
        um->AddURI(String("media"), String("*/audio/artists/#"), AUDIO_ARTISTS_ID);
        um->AddURI(String("media"), String("*/audio/artists/#/albums"), AUDIO_ARTISTS_ID_ALBUMS);
        um->AddURI(String("media"), String("*/audio/albums"), AUDIO_ALBUMS);
        um->AddURI(String("media"), String("*/audio/albums/#"), AUDIO_ALBUMS_ID);
        um->AddURI(String("media"), String("*/audio/albumart"), AUDIO_ALBUMART);
        um->AddURI(String("media"), String("*/audio/albumart/#"), AUDIO_ALBUMART_ID);
        um->AddURI(String("media"), String("*/audio/media/#/albumart"), AUDIO_ALBUMART_FILE_ID);

        um->AddURI(String("media"), String("*/video/media"), VIDEO_MEDIA);
        um->AddURI(String("media"), String("*/video/media/#"), VIDEO_MEDIA_ID);
        um->AddURI(String("media"), String("*/video/thumbnails"), VIDEO_THUMBNAILS);
        um->AddURI(String("media"), String("*/video/thumbnails/#"), VIDEO_THUMBNAILS_ID);

        um->AddURI(String("media"), String("*/media_scanner"), MEDIA_SCANNER);

        um->AddURI(String("media"), String("*/fs_id"), FS_ID);
        um->AddURI(String("media"), String("*/version"), VERSION);

        um->AddURI(String("media"), String("*/mtp_connected"), MTP_CONNECTED);

        um->AddURI(String("media"), String("*"), VOLUMES_ID);
        um->AddURI(String("media"), String(NULL), VOLUMES);

        // Used by MTP implementation
        um->AddURI(String("media"), String("*/file"), FILES);
        um->AddURI(String("media"), String("*/file/#"), FILES_ID);
        um->AddURI(String("media"), String("*/object"), MTP_OBJECTS);
        um->AddURI(String("media"), String("*/object/#"), MTP_OBJECTS_ID);
        um->AddURI(String("media"), String("*/object/#/references"), MTP_OBJECT_REFERENCES);

        /**
        * @deprecated use the 'basic' or 'fancy' search Uris instead
        */
        um->AddURI(String("media"), String("*/audio/") + ISearchManager::SUGGEST_URI_PATH_QUERY,
               AUDIO_SEARCH_LEGACY);
        um->AddURI(String("media"), String("*/audio/") + ISearchManager::SUGGEST_URI_PATH_QUERY + String("/*"),
               AUDIO_SEARCH_LEGACY);

        // used for search suggestions
        um->AddURI(String("media"), String("*/audio/search/") + ISearchManager::SUGGEST_URI_PATH_QUERY,
               AUDIO_SEARCH_BASIC);
        um->AddURI(String("media"), String("*/audio/search/") + ISearchManager::SUGGEST_URI_PATH_QUERY +
               "/*", AUDIO_SEARCH_BASIC);

        // used by the music app's search activity
        um->AddURI(String("media"), String("*/audio/search/fancy"), AUDIO_SEARCH_FANCY);
        um->AddURI(String("media"), String("*/audio/search/fancy/*"), AUDIO_SEARCH_FANCY);

        um->AddURI(String("media"), String("*/bookmark"), MEDIA_BOOKMARK);
        um->AddURI(String("media"), String("*/bookmark/#"), MEDIA_BOOKMARK_ID);
        URI_MATCHER = um;
    }
    return URI_MATCHER;
}

AutoPtr<IUriMatcher> MediaProvider::URI_MATCHER;

static AutoPtr<ArrayOf<String> > InitID_PROJECTION()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = IBaseColumns::ID;
    return array;
}

AutoPtr<ArrayOf<String> > MediaProvider::ID_PROJECTION = InitID_PROJECTION();

static AutoPtr<ArrayOf<String> > InitPATH_PROJECTION()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = IBaseColumns::ID;
    (*array)[1] = IMediaStoreMediaColumns::DATA;
    return array;
}

AutoPtr<ArrayOf<String> > MediaProvider::PATH_PROJECTION = InitPATH_PROJECTION();

static AutoPtr<ArrayOf<String> > InitMIME_TYPE_PROJECTION()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = IBaseColumns::ID;
    (*array)[1] = IMediaStoreMediaColumns::MIME_TYPE;
    return array;
}

AutoPtr<ArrayOf<String> > MediaProvider::MIME_TYPE_PROJECTION = InitMIME_TYPE_PROJECTION();

static AutoPtr<ArrayOf<String> > InitREADY_FLAG_PROJECTION()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    (*array)[0] = IBaseColumns::ID;
    (*array)[1] = IMediaStoreMediaColumns::DATA;
    (*array)[2] = IMediaStoreImagesImageColumns::MINI_THUMB_MAGIC;
    return array;
}

AutoPtr<ArrayOf<String> > MediaProvider::READY_FLAG_PROJECTION = InitREADY_FLAG_PROJECTION();

static String InitOBJECT_REFERENCES_QUERY()
{
    StringBuilder sb("SELECT ");
    sb += IMediaStoreAudioPlaylistsMembers::AUDIO_ID;
    sb += " FROM audio_playlists_map WHERE ";
    sb += IMediaStoreAudioPlaylistsMembers::PLAYLIST_ID;
    sb += "=? ORDER BY ";
    sb += IMediaStoreAudioPlaylistsMembers::PLAY_ORDER;
    return sb.ToString();
}
const String MediaProvider::OBJECT_REFERENCES_QUERY = InitOBJECT_REFERENCES_QUERY();

//================================================================================
//                      MediaProvider::MyBroadcastReceiver
//================================================================================
MediaProvider::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ MediaProvider* host)
    : mHost(host)
{}

ECode MediaProvider::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_MEDIA_EJECT.Equals(action)) {
        AutoPtr<IParcelable> pl;
        intent->GetParcelableExtra(IStorageVolume::EXTRA_STORAGE_VOLUME, (IParcelable**)&pl);
        AutoPtr<IStorageVolume> storage = IStorageVolume::Probe(pl);
        // If primary external storage is ejected, then remove the external volume
        // notify all cursors backed by data on that volume.
        String path;
        storage->GetPath(&path);
        AutoPtr<IUri> uri;
        AutoPtr<IUriHelper> uh;
        CUriHelper::AcquireSingleton((IUriHelper**)&uh);
        if (path.Equals((*mExternalStoragePaths)[0])) {
            uh->Parse(String("content://media/external"), (IUri**)&uri);
            mHost->DetachVolume(uri);
            sFolderArtMap->Clear();
            AutoPtr<IMiniThumbFileHelper> mtFileHelper;
            CMiniThumbFileHelper::AcquireSingleton((IMiniThumbFileHelper**)&mtFileHelper);
            mtFileHelper->Reset();
        } else {
            // If secondary external storage is ejected, then we delete all database
            // entries for that storage from the files table.
            AutoPtr<IDatabaseHelper> database;

            {
                AutoLock syncLock(mHost->mDatabases);
                // This synchronized block is limited to avoid a potential deadlock
                // with bulkInsert() method.
                AutoPtr<IInterface> obj;
                mHost->mDatabases->Get(CoreUtils::Convert(EXTERNAL_VOLUME), (IInterface**)&obj);
                database = IDatabaseHelper::Probe(obj);
            }
            uri = NULL;
            storage->GetPath(&path);
            uh->Parse(String("file://") + path, (IUri**)&uri);
            AutoPtr<IIntent> intent;
            if (database != NULL) {
               // try {
                // Send media scanner started and stopped broadcasts for apps that rely
                // on these Intents for coarse grained media database notifications.
                CIntent::New(IIntent::ACTION_MEDIA_SCANNER_STARTED, uri, (IIntent**)&intent);
                context->SendBroadcast(intent);//

                // don't send objectRemoved events - MTP be sending StorageRemoved anyway
                mHost->mDisableMtpObjectCallbacks = TRUE;
                Logger::D(TAG, "deleting all entries for storage %s", TO_CSTR(storage));
                AutoPtr<ISQLiteDatabase> db;
                ((DatabaseHelper*)database.Get())->GetWritableDatabase((ISQLiteDatabase**)&db);
                // First clear the file path to disable the _DELETE_FILE database hook.
                // We do this to avoid deleting files if the volume is remounted while
                // we are still processing the unmount event.
                AutoPtr<IContentValues> values;
                CContentValues::New((IContentValues**)&values);
                values->PutNull(IMediaStoreMediaColumns::DATA);
                String where = IMediaStoreFilesFileColumns::STORAGE_ID + String("=?");
                Int32 tempId;
                storage->GetStorageId(&tempId);
                String storageId = StringUtils::ToString(tempId);
                AutoPtr<ArrayOf<String> > whereArgs = ArrayOf<String>::Alloc(1);
                (*whereArgs)[0] = storageId;
                (((DatabaseHelper*)database.Get())->mNumUpdates)++;
                Int32 outValue;
                db->Update(String("files"), values, where, whereArgs, &outValue);
                // now delete the records
                (((DatabaseHelper*)database.Get())->mNumDeletes)++;
                Int32 numpurged;
                db->Delete(String("files"), where, whereArgs, &numpurged);
                storage->GetPath(&path);
                LogToDb(db, String("removed ") + StringUtils::ToString(numpurged) +
                     String(" rows for ejected filesystem ") + path);
                // notify on media Uris as well as the files Uri
                AutoPtr<IContentResolver> resolver;
                context->GetContentResolver((IContentResolver**)&resolver);
                AutoPtr<IMediaStoreAudioMedia> audioMedia;
                CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&audioMedia);
                AutoPtr<IUri> uuri;
                audioMedia->GetContentUri(EXTERNAL_VOLUME, (IUri**)&uuri);
                resolver->NotifyChange(uuri, NULL);

                uuri = NULL;
                AutoPtr<IMediaStoreImagesMedia> imagesMedia;
                CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&imagesMedia);
                imagesMedia->GetContentUri(EXTERNAL_VOLUME, (IUri**)&uuri);
                resolver->NotifyChange(uuri, NULL);

                uuri = NULL;
                AutoPtr<IMediaStoreVideoMedia> videoMedia;
                CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&videoMedia);
                videoMedia->GetContentUri(EXTERNAL_VOLUME, (IUri**)&uuri);
                resolver->NotifyChange(uuri, NULL);

                uuri = NULL;
                AutoPtr<IMediaStoreFiles> files;
                CMediaStoreFiles::AcquireSingleton((IMediaStoreFiles**)&files);
                files->GetContentUri(EXTERNAL_VOLUME, (IUri**)&uuri);
                resolver->NotifyChange(uuri, NULL);
                // } //catch (Exception e) {
                   // Log.e(TAG, "exception deleting storage entries", e);
               // } finally {
                intent = NULL;
                CIntent::New(IIntent::ACTION_MEDIA_SCANNER_FINISHED, uri, (IIntent**)&intent);
                context->SendBroadcast(intent);
                mHost->mDisableMtpObjectCallbacks = FALSE;
                return NOERROR;
               // }
           }
       }
   }
   return NOERROR;
}

//================================================================================
//                      MediaProvider::MySqliteDbCunstomFunction
//================================================================================
CAR_INTERFACE_IMPL(MediaProvider::MySqliteDbCunstomFunction, Object, ICustomFunction)

MediaProvider::MySqliteDbCunstomFunction::MySqliteDbCunstomFunction(
    /* [in] */ MediaProvider* host)
   : mHost(host)
{}

ECode MediaProvider::MySqliteDbCunstomFunction::Callback(
    /* [in] */ ArrayOf<String>* args)
{
    // We could remove only the deleted entry from the cache, but that
    // requires the path, which we don't have here, so instead we just
    // clear the entire cache.
    // TODO: include the path in the callback and only remove the affected
    // entry from the cache
    mHost->mDirectoryCache->Clear();
    // do nothing if the operation originated from MTP
    if (mHost->mDisableMtpObjectCallbacks) return NOERROR;//

    Logger::D(TAG, "object removed %s", ((*args)[0]).string());
    AutoPtr<IIMtpService> mtpService = mHost->mMtpService;
    if (mtpService != NULL) {
       // try {
        mHost->SendObjectRemoved(StringUtils::ParseInt64((*args)[0]));
       // } catch (NumberFormatException e) {
        Logger::E(TAG, "NumberFormatException in mObjectRemovedCallback");
       // }
    }
    return NOERROR;
}

//================================================================================
//                      MediaProvider::MyMtpServiceConnection
//================================================================================
CAR_INTERFACE_IMPL(MediaProvider::MyMtpServiceConnection, Object, IServiceConnection)

MediaProvider::MyMtpServiceConnection::MyMtpServiceConnection(
    /* [in] */ MediaProvider* host)
    : mHost(host)
{}

ECode MediaProvider::MyMtpServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoLock syncLock(this);
    mHost->mMtpService = IIMtpService::Probe(service);
    return NOERROR;
}

ECode MediaProvider::MyMtpServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    AutoLock syncLock(this);
    mHost->mMtpService = NULL;
    return NOERROR;
}

//================================================================================
//                      MediaProvider::MyHandler
//================================================================================
MediaProvider::MyHandler::MyHandler(
    /* [in] */ MediaProvider* host,
    /* [in] */ ILooper* looper,
    /* [in] */ IMessage* msg)
    : Handler(looper, FALSE)
    , mHost(host)
    , mLooper(looper)
    , mMsg(msg)
{
}

ECode MediaProvider::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    if (what == IMAGE_THUMB) {
        {
            AutoLock syncLock(mHost->mMediaThumbQueue);
            AutoPtr<IInterface> obj;
            mHost->mMediaThumbQueue->Poll((IInterface**)&obj);
            mHost->mCurrentThumbRequest = IMediaThumbRequest::Probe(obj);
        }

        if (mHost->mCurrentThumbRequest == NULL) {
            Logger::W(TAG, "Have message but no request?");
        }

        else {
            MediaThumbRequest* mtr = (MediaThumbRequest*)mHost->mCurrentThumbRequest.Get();
          // try {
            AutoPtr<IFile> origFile;
            CFile::New(mtr->mPath, (IFile**)&origFile);
            Boolean flag = FALSE;
            origFile->Exists(&flag);
            Int64 length;
            origFile->GetLength(&length);
            if (flag && length > 0) {
                mHost->mCurrentThumbRequest->Execute();
                // Check if more requests for the same image are queued.
                {
                    AutoLock syncLock(mHost->mMediaThumbQueue);
                    AutoPtr<IIterator> it;
                    mHost->mMediaThumbQueue->GetIterator((IIterator**)&it);
                    Boolean hasNext;
                    while (it->HasNext(&hasNext), hasNext) {
                        AutoPtr<IInterface> obj;
                        it->GetNext((IInterface**)&obj);
                        MediaThumbRequest* mtq = (MediaThumbRequest*)IMediaThumbRequest::Probe(obj);
                        if ((mtq->mOrigId == mtr->mOrigId) &&
                            (mtq->mIsVideo == mtr->mIsVideo) &&
                            (mtq->mMagic == 0) &&
                            (mtq->mState == WAIT)) {
                            mtq->mMagic = mtr->mMagic;
                        }
                    }
               }
            }
            else {
                // original file hasn't been stored yet
                {
                    AutoLock syncLock(mHost->mMediaThumbQueue);
                    Logger::W(TAG, "original file hasn't been stored yet: %s", mtr->mPath.string());
                }
            }
           // } catch (IOException ex) {
           //     Log.w(TAG, ex);
           // } catch (UnsupportedOperationException ex) {
           //     This could happen if we unplug the sd card during insert/update/delete
           //     See getDatabaseForUri.
           //     Log.w(TAG, ex);
           // } catch (OutOfMemoryError err) {
               /*
                * Note: Catching Errors is in most cases considered
                * bad practice. However, in this case it is
                * motivated by the fact that corrupt or very large
                * images may cause a huge allocation to be
                * requested and denied. The bitmap handling API in
                * Android offers no other way to guard against
                * these problems than by catching OutOfMemoryError.
                */
               // Log.w(TAG, err);
           // } finally {
            {
                AutoLock syncLock(mHost->mCurrentThumbRequest);
                mtr->mState = DONE;
                mtr->NotifyAll();
            }
           // }
        }
    }
    else if (what == MediaProvider::ALBUM_THUMB) {
        AutoPtr<IThumbData> d;
        AutoPtr<IInterface> obj;
        {
            AutoLock syncLock(mHost->mThumbRequestStack);
            mHost->mThumbRequestStack->Peek((IInterface**)&obj);
            d = IThumbData::Probe(obj);
        }

        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        ioUtils->CloseQuietly(ICloseable::Probe(mHost->MakeThumbInternal((ThumbData*)d.Get())));
        {
            AutoLock syncLock(mHost->mPendingThumbs);
            mHost->mPendingThumbs->Remove(CoreUtils::Convert(((ThumbData*)d.Get())->mPath));
        }
    }

    return NOERROR;
}

//================================================================================
//                      MediaProvider::DatabaseHelper
//================================================================================
MediaProvider::DatabaseHelper::DatabaseHelper()
{}

MediaProvider::DatabaseHelper::~DatabaseHelper()
{}

CAR_INTERFACE_IMPL(MediaProvider::DatabaseHelper, SQLiteOpenHelper, IDatabaseHelper)

ECode MediaProvider::DatabaseHelper::constructor(
    /* [in] */ MediaProvider* host,
    /* [in] */ IContext* context,
    /* [in] */ const String& name,
    /* [in] */ Boolean internal,
    /* [in] */ Boolean earlyUpgrade,
    /* [in] */ ICustomFunction* objectRemovedCallback)
{
    mHost = host;
    Int32 databaseVersion = mHost->GetDatabaseVersion(context);
    SQLiteOpenHelper::constructor(context, name, NULL, databaseVersion);
    mContext = context;
    mName = name;
    mInternal = internal;
    mEarlyUpgrade = earlyUpgrade;
    mObjectRemovedCallback = objectRemovedCallback;
    return SQLiteOpenHelper::SetWriteAheadLoggingEnabled(TRUE);
}

ECode MediaProvider::DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    Int32 dataBaseVersion = mHost->GetDatabaseVersion(mContext);
    mHost->UpdateDatabase(mContext, db, mInternal, 0, dataBaseVersion);
    return NOERROR;
}

ECode MediaProvider::DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldV,
    /* [in] */ Int32 newV)
{
    mUpgradeAttempted = TRUE;
    mHost->UpdateDatabase(mContext, db, mInternal, oldV, newV);
    return NOERROR;
}

ECode MediaProvider::DatabaseHelper::GetWritableDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database)
    *database = NULL;

    AutoPtr<ISQLiteDatabase> result;
    mUpgradeAttempted = FALSE;
    ECode ec = SQLiteOpenHelper::GetWritableDatabase((ISQLiteDatabase**)&result);
    if (FAILED(ec)) {
        if (!mUpgradeAttempted) {
            Logger::E(mHost->TAG, "failed to open database %s", mName.string());
            *database = NULL;
            return NOERROR;
        }
    }

    // If we failed to open the database during an upgrade, delete the file and try again.
    // This will result in the creation of a fresh database, which will be repopulated
    // when the media scanner runs.
    if (result == NULL && mUpgradeAttempted) {
        Boolean bSucceeded = FALSE;
        mContext->DeleteDatabase(mName, &bSucceeded);
        GetWritableDatabase((ISQLiteDatabase**)&result);
    }
    *database = result;
    REFCOUNT_ADD(*database);
    return NOERROR;
}

ECode MediaProvider::DatabaseHelper::OnOpen(
    /* [in] */ ISQLiteDatabase* db)
{
    if (mInternal) return NOERROR;  // The internal database is kept separately.

    if (mEarlyUpgrade) return NOERROR; // Doing early upgrade.

    if (mObjectRemovedCallback != NULL) {
        db->AddCustomFunction(String("_OBJECT_REMOVED"), 1, mObjectRemovedCallback);
    }

    // the code below is only needed on devices with removable storage
    if (!Environment::IsExternalStorageRemovable()) return NOERROR;

    // touch the database file to show it is most recently used
    String path;
    db->GetPath(&path);
    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    AutoPtr<ISystem> ssystem;
    CSystem::AcquireSingleton((ISystem**)&ssystem);
    Int64 now;
    ssystem->GetCurrentTimeMillis(&now);
    Boolean flag = FALSE;
    file->SetLastModified(now, &flag);

    // delete least recently used databases if we are over the limit
    AutoPtr<ArrayOf<String> > databases;
    mContext->GetDatabaseList((ArrayOf<String>**)&databases);
    Int32 count = databases->GetLength();
    Int32 limit =mHost->MAX_EXTERNAL_DATABASES;

    // delete external databases that have not been used in the past two months
    Int64 twoMonthsAgo = now - mHost->OBSOLETE_DATABASE_DB;
    AutoPtr<IFile> other;
    for (Int32 i = 0; i < twoMonthsAgo; i++) {
        other = NULL;
        mContext->GetDatabasePath((*databases)[i], (IFile**)&other);
        if (mHost->INTERNAL_DATABASE_NAME.Equals((*databases)[i]) || file == other) {
            (*databases)[i] = String(NULL);
            count--;
            if (file == other) {
                // reduce limit to account for the existence of the database we
                // are about to open, which we removed from the list.
                limit--;
            }
        } else {
            Int64 time;
            other->GetLastModified(&time);
            if (time < twoMonthsAgo) {
                if (mHost->LOCAL_LOGV) Logger::V(mHost->TAG, "Deleting old database %s", (*databases)[i].string());
                mContext->DeleteDatabase((*databases)[i], &flag);
                (*databases)[i] = String(NULL);
                count--;
            }
        }
    }

    // delete least recently used databases until
    // we are no longer over the limit
    while (count > limit) {
        Int32 lruIndex = -1;
        Int64 lruTime = 0;

        Int32 length = databases->GetLength();
        AutoPtr<IFile> fl;
        for (Int32 i = 0; i < length; i++) {
            if (!(*databases)[i].IsNull()) {
                fl = NULL;
                mContext->GetDatabasePath((*databases)[i], (IFile**)&fl);
                Int64 time;
                fl->GetLastModified(&time);
                if (lruTime == 0 || time < lruTime) {
                    lruIndex = i;
                    lruTime = time;
                }
            }
        }

        // delete least recently used database
        if (lruIndex != -1) {
            if (mHost->LOCAL_LOGV) Logger::V(mHost->TAG, "Deleting old database %s", (*databases)[lruIndex].string());
            mContext->DeleteDatabase((*databases)[lruIndex], &flag);
            (*databases)[lruIndex] = String(NULL);
            count--;
        }
    }
    return NOERROR;
}

//================================================================================
//                      MediaProvider::ScannerClient
//================================================================================
CAR_INTERFACE_IMPL_2(MediaProvider::ScannerClient, Object, IMediaScannerConnectionClient, IOnScanCompletedListener)//

MediaProvider::ScannerClient::ScannerClient()
{}

MediaProvider::ScannerClient::~ScannerClient()
{}

ECode MediaProvider::ScannerClient::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& path)
{
    mDb = db;
    mPath = path;
    AutoPtr<IMediaScannerConnection> connect;
    CMediaScannerConnection::New(context, this, (IMediaScannerConnection**)&connect);
    return mScannerConnection->Connect();
}

ECode MediaProvider::ScannerClient::OnMediaScannerConnected()
{
    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
    (*arr)[0] = mPath + String("/");
    (*arr)[1] = mPath + String("0");
    mDb->Query(String("files"), openFileColumns,
          String("_data >= ? AND _data < ?"),
          arr, mPath + String("0"),
          String(NULL), String(NULL), String(NULL), (ICursor**)&c);
   // try  {
      Boolean flag = FALSE;
      AutoPtr<IFile> f;
      while ((c->MoveToNext(&flag), flag)) {
          String d;
          c->GetString(0, &d);
          f = NULL;
          CFile::New(d, (IFile**)&f);
          f->IsFile(&flag);
          if (flag) {
              mScannerConnection->ScanFile(d, String(NULL));
          }
      }
      mScannerConnection->Disconnect();
   // } finally {
      AutoPtr<IIoUtils> ioUtils;
      CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
      return ioUtils->CloseQuietly(ICloseable::Probe(c));
   // }
}

ECode MediaProvider::ScannerClient::OnScanCompleted(
    /* [in] */ const String& path,
    /* [in] */ IUri* uri)
{
    return NOERROR;
}

//================================================================================
//                      MediaProvider::ThumbData
//================================================================================
CAR_INTERFACE_IMPL(MediaProvider::ThumbData, Object, IThumbData)

//================================================================================
//                      MediaProvider::MyThread
//================================================================================
MediaProvider::MyThread::MyThread(
    /* [in] */ MediaProvider* owner,
    /* [in] */ IFile* file,
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& path)
  : mOwner(owner)
  , mFile(file)
  , mHelper(helper)
  , mDb(db)
  , mPath(path)
{}//

ECode MediaProvider::MyThread::Run()
{
    SystemClock::Sleep(2000);
    Boolean flag = FALSE;
    if ((mFile->Exists(&flag), flag)) {
        mOwner->HidePath(mHelper, mDb, mPath);
    } else {
        Logger::W(TAG, "does not exist: %s", mPath.string());
    }
    return NOERROR;
}

//================================================================================
//                      MediaProvider
//================================================================================
CAR_INTERFACE_IMPL(MediaProvider, ContentProvider, IMediaProvider);

MediaProvider::MediaProvider()
    : mCaseInsensitivePaths(FALSE)
    , SEARCH_COLUMN_BASIC_TEXT2(5)
    , mDisableMtpObjectCallbacks(FALSE)
    , mVolumeId(-1)
{
}

MediaProvider::~MediaProvider()
{}

ECode MediaProvider::constructor()
{
    mDirectoryCache = new HashMap<String, Int64>();
    CHashSet::New((IHashSet**)&mPendingThumbs);
    CStack::New((IStack**)&mThumbRequestStack);
    CHashMap::New((IHashMap**)&mDatabases);

    AutoPtr<IComparator> comparator;
    MediaThumbRequest::GetComparator((IComparator**)&comparator);
    CPriorityQueue::New(MediaThumbRequest::PRIORITY_NORMAL, comparator, (IPriorityQueue**)&mMediaThumbQueue);

    mSearchColsLegacy = InitSearchColsLegacy();
    mSearchColsFancy = InitSearchColsFancy();
    mSearchColsBasic = InitSearchColsBasic();
    mAlbumArtBaseUri = InitUri("content://media/external/audio/albumart");

    AutoPtr<MyBroadcastReceiver> br = new MyBroadcastReceiver(this);
    mUnmountReceiver = IBroadcastReceiver::Probe(br);
    mObjectRemovedCallback = new MySqliteDbCunstomFunction(this);
    mMtpServiceConnection = new MyMtpServiceConnection(this);
    return ContentProvider::constructor();
}

void MediaProvider::CreateDefaultFolders(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db)
{
    // Use a SharedPreference to ensure we only do this once.
    // We don't want to annoy the user by recreating the directories
    // after she has deleted them.
    AutoPtr<IPreferenceManagerHelper> pmh;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&pmh);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ISharedPreferences> prefs;
    pmh->GetDefaultSharedPreferences(context, (ISharedPreferences**)&prefs);
    Int32 vol;
    prefs->GetInt32(String("created_default_folders"), 0, &vol);
    if (vol == 0) {
        Int32 length = sDefaultFolderNames->GetLength();
        AutoPtr<IFile> file;
        Boolean flag = FALSE;
        String path;
        for (Int32 i = 0; i < length; ++i) {
            String folderName = (*sDefaultFolderNames)[i];
            file = NULL;
            file = Environment::GetExternalStoragePublicDirectory(folderName);
            file->Exists(&flag);//

            if (!flag) {
                Boolean bSucceeded = FALSE;
                file->Mkdirs(&bSucceeded);
                file->GetAbsolutePath(&path);
                InsertDirectory(helper, db, path);
            }
        }

        AutoPtr<ISharedPreferencesEditor> e;
        prefs->Edit((ISharedPreferencesEditor**)&e);
        e->Clear();
        e->PutInt32(String("created_default_folders"), 1);
        e->Commit(&flag);
   }
}

Int32 MediaProvider::GetDatabaseVersion(
    /* [in] */ IContext* context)
{
    // try {
    ECode ec = NOERROR;
    AutoPtr<IPackageManager> pm;
    ec = context->GetPackageManager((IPackageManager**)&pm);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "couldn't get version code for %p", context);
        return E_RUNTIME_EXCEPTION;
    }
    String packageName;

    ec = context->GetPackageName(&packageName);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "couldn't get version code for %p", context);
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IPackageInfo> info;
    ec = pm->GetPackageInfo(packageName, 0, (IPackageInfo**)&info);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "couldn't get version code for %p", context);
        return E_RUNTIME_EXCEPTION;
    }
    Int32 versionCode;
    ec = info->GetVersionCode(&versionCode);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "couldn't get version code for %p", context);
        return E_RUNTIME_EXCEPTION;
    }
    return versionCode;
// } catch (NameNotFoundException e) {
   // throw new RuntimeException("couldn't get version code for " + context);
// }
}

Boolean MediaProvider::InitHashMapWithString(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [in] */ IHashMap* hp)
{
    assert(hp);
    AutoPtr<ICharSequence> cskey;
    cskey = CoreUtils::Convert(key);
    AutoPtr<ICharSequence> csValue;
    csValue = CoreUtils::Convert(value);
    hp->Put(cskey, csValue);
    return TRUE;
}

ECode MediaProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::STORAGE_SERVICE, (IInterface**)&obj);
    mStorageManager = IStorageManager::Probe(obj);

    InitHashMapWithString(IBaseColumns::ID, String("audio.album_id AS ") + IBaseColumns::ID, sArtistAlbumsMap);
    InitHashMapWithString(IMediaStoreAudioAlbumColumns::ALBUM, String("album"), sArtistAlbumsMap);
    InitHashMapWithString(IMediaStoreAudioAlbumColumns::ALBUM_KEY, String("album_key"), sArtistAlbumsMap);
    InitHashMapWithString(IMediaStoreAudioAlbumColumns::FIRST_YEAR, String("MIN(year) AS ") +
            IMediaStoreAudioAlbumColumns::FIRST_YEAR, sArtistAlbumsMap);
    InitHashMapWithString(IMediaStoreAudioAlbumColumns::LAST_YEAR, String("MAX(year) AS ") +
            IMediaStoreAudioAlbums::LAST_YEAR, sArtistAlbumsMap);
    InitHashMapWithString(IMediaStoreAudioArtistColumns::ARTIST, String("artist"), sArtistAlbumsMap);
    InitHashMapWithString(IMediaStoreAudioAudioColumns::ARTIST_ID, String("artist"), sArtistAlbumsMap);
    InitHashMapWithString(IMediaStoreAudioAudioColumns::ARTIST_KEY, String("artist_key"), sArtistAlbumsMap);
    InitHashMapWithString(IMediaStoreAudioAlbums::NUMBER_OF_SONGS, String("count(*) AS ") +
            IMediaStoreAudioAlbums::NUMBER_OF_SONGS, sArtistAlbumsMap);
    InitHashMapWithString(IMediaStoreAudioAlbums::ALBUM_ART, String("album_art._data AS ") +
            IMediaStoreAudioAlbums::ALBUM_ART, sArtistAlbumsMap);

    String label;
    context->GetString(R::string::artist_label, &label);

    String rstr;
    StringUtils::ReplaceAll((*mSearchColsBasic)[SEARCH_COLUMN_BASIC_TEXT2], String("%1"), label, &rstr);
    (*mSearchColsBasic)[SEARCH_COLUMN_BASIC_TEXT2] = rstr;
    AutoPtr<IUri> uuri = AttachVolume(INTERNAL_VOLUME);//

    AutoPtr<IIntentFilter> iFilter;
    CIntentFilter::New(IIntent::ACTION_MEDIA_EJECT, (IIntentFilter**)&iFilter);
    iFilter->AddDataScheme(String("file"));
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mUnmountReceiver, iFilter, (IIntent**)&intent);//

    obj = NULL;
    context->GetSystemService(IContext::STORAGE_SERVICE, (IInterface**)&obj);
    AutoPtr<IStorageManager> storageManager = IStorageManager::Probe(obj);//

    storageManager->GetVolumePaths((ArrayOf<String>**)&mExternalStoragePaths);//

    // open external database if external storage is mounted
    String state = Environment::GetExternalStorageState();
    if (Environment::MEDIA_MOUNTED.Equals(state) ||
           Environment::MEDIA_MOUNTED_READ_ONLY.Equals(state)) {
       AttachVolume(EXTERNAL_VOLUME);
    }

    AutoPtr<IHandlerThread> ht;
    CHandlerThread::New(String("thumbs thread"), IProcess::THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&ht);
    IThread::Probe(ht)->Start();
    AutoPtr<ILooper> lp;
    ht->GetLooper((ILooper**)&lp);
    AutoPtr<MyHandler> myHandler = new MyHandler(this, lp, NULL);
    mThumbHandler = (IHandler*)(myHandler);
    *result = TRUE;
    return NOERROR;
}

ECode MediaProvider::UpdateDatabase(
    /* [in] */ IContext* context,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Boolean internal,
    /* [in] */ Int32 fromVersion,
    /* [in] */ Int32 toVersion)
{
    // sanity checks
    Int32 dbversion = GetDatabaseVersion(context);
    if (toVersion != dbversion) {
        Logger::E(TAG, "Illegal update request. Got %d toVersion expected %d", toVersion, dbversion);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (fromVersion > toVersion) {
        Logger::E(TAG, "Illegal update request: can't downgrade from %d to %d. Did you forget to wipe data?", fromVersion, toVersion);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 startTime = SystemClock::GetCurrentTimeMicro();//
    StringBuilder sb;

    // Revisions 84-86 were a failed attempt at supporting the "album artist" id3 tag.
    // We can't downgrade from those revisions, so start over.
    // (the initial change to do this was wrong, so now we actually need to start over
    // if the database version is 84-89)
    // Post-gingerbread, revisions 91-94 were broken in a way that is not easy to repair.
    // However version 91 was reused in a divergent development path for gingerbread,
    // so we need to support upgrades from 91.
    // Therefore we will only force a reset for versions 92 - 94.
    if (fromVersion < 63 || (fromVersion >= 84 && fromVersion <= 89) ||
              (fromVersion >= 92 && fromVersion <= 94)) {
        // Drop everything and start over.
        Logger::I(TAG, "Upgrading media database from version %d to %d, which will destroy all old data", fromVersion, toVersion);
        fromVersion = 63;
        db->ExecSQL(String("DROP TABLE IF EXISTS images"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS images_cleanup"));
        db->ExecSQL(String("DROP TABLE IF EXISTS thumbnails"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS thumbnails_cleanup"));
        db->ExecSQL(String("DROP TABLE IF EXISTS audio_meta"));
        db->ExecSQL(String("DROP TABLE IF EXISTS artists"));
        db->ExecSQL(String("DROP TABLE IF EXISTS albums"));
        db->ExecSQL(String("DROP TABLE IF EXISTS album_art"));
        db->ExecSQL(String("DROP VIEW IF EXISTS artist_info"));
        db->ExecSQL(String("DROP VIEW IF EXISTS album_info"));
        db->ExecSQL(String("DROP VIEW IF EXISTS artists_albums_map"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_meta_cleanup"));
        db->ExecSQL(String("DROP TABLE IF EXISTS audio_genres"));
        db->ExecSQL(String("DROP TABLE IF EXISTS audio_genres_map"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_genres_cleanup"));
        db->ExecSQL(String("DROP TABLE IF EXISTS audio_playlists"));
        db->ExecSQL(String("DROP TABLE IF EXISTS audio_playlists_map"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_playlists_cleanup"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS albumart_cleanup1"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS albumart_cleanup2"));
        db->ExecSQL(String("DROP TABLE IF EXISTS video"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS video_cleanup"));
        db->ExecSQL(String("DROP TABLE IF EXISTS objects"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS images_objects_cleanup"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_objects_cleanup"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS video_objects_cleanup"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS playlists_objects_cleanup"));//

        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS images ("
                "_id INTEGER PRIMARY KEY,"
                "_data TEXT,"
                "_size INTEGER,"
                "_display_name TEXT,"
                "mime_type TEXT,"
                "title TEXT,"
                "date_added INTEGER,"
                "date_modified INTEGER,"
                "description TEXT,"
                "picasa_id TEXT,"
                "isprivate INTEGER,"
                "latitude DOUBLE,"
                "longitude DOUBLE,"
                "datetaken INTEGER,"
                "orientation INTEGER,"
                "mini_thumb_magic INTEGER,"
                "bucket_id TEXT,"
                "bucket_display_name TEXT"
                ");"));

        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS mini_thumb_magic_index on images(mini_thumb_magic);"));

        db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS images_cleanup DELETE ON images "
                "BEGIN "
                    "DELETE FROM thumbnails WHERE image_id = old._id;"
                    "SELECT _DELETE_FILE(old._data);"
                "END"));

        // create image thumbnail table
        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS thumbnails ("
                "_id INTEGER PRIMARY KEY,"
                "_data TEXT,"
                "image_id INTEGER,"
                "kind INTEGER,"
                "width INTEGER,"
                "height INTEGER"
                ");"));

        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS image_id_index on thumbnails(image_id);"));

        db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS thumbnails_cleanup DELETE ON thumbnails "
                "BEGIN "
                    "SELECT _DELETE_FILE(old._data);"
                "END"));

        // Contains meta data about audio files
        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS audio_meta ("
                "_id INTEGER PRIMARY KEY,"
                "_data TEXT UNIQUE NOT NULL,"
                "_display_name TEXT,"
                "_size INTEGER,"
                "mime_type TEXT,"
                "date_added INTEGER,"
                "date_modified INTEGER,"
                "title TEXT NOT NULL,"
                "title_key TEXT NOT NULL,"
                "duration INTEGER,"
                "artist_id INTEGER,"
                "composer TEXT,"
                "album_id INTEGER,"
                "track INTEGER,"     // track is an integer to allow proper sorting
                "year INTEGER CHECK(year!=0),"
                "is_ringtone INTEGER,"
                "is_music INTEGER,"
                "is_alarm INTEGER,"
                "is_notification INTEGER"
                ");"));

        // Contains a sort/group "key" and the preferred display name for artists
        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS artists ("
                "artist_id INTEGER PRIMARY KEY,"
                "artist_key TEXT NOT NULL UNIQUE,"
                "artist TEXT NOT NULL"
                ");"));

        // Contains a sort/group "key" and the preferred display name for albums
        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS albums ("
                "album_id INTEGER PRIMARY KEY,"
                "album_key TEXT NOT NULL UNIQUE,"
                "album TEXT NOT NULL"
                ");"));

        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS album_art ("
                "album_id INTEGER PRIMARY KEY,"
                "_data TEXT"
               ");"));

        RecreateAudioView(db);


        // Provides some extra info about artists, like the number of tracks
        // and albums for this artist
        db->ExecSQL(String("CREATE VIEW IF NOT EXISTS artist_info AS "
                "SELECT artist_id AS _id, artist, artist_key, "
                "COUNT(DISTINCT album) AS number_of_albums, "
                "COUNT(*) AS number_of_tracks FROM audio WHERE is_music=1 "
                "GROUP BY artist_key;"));

        // Provides extra info albums, such as the number of tracks
        sb.Reset();
        sb += "CREATE VIEW IF NOT EXISTS album_info AS "
                "SELECT audio.album_id AS _id, album, album_key, "
                "MIN(year) AS minyear, "
                "MAX(year) AS maxyear, artist, artist_id, artist_key, "
                "count(*) AS ";
        sb += IMediaStoreAudioAlbumColumns::NUMBER_OF_SONGS;
        sb += ",album_art._data AS album_art"
                " FROM audio LEFT OUTER JOIN album_art ON audio.album_id=album_art.album_id"
                " WHERE is_music=1 GROUP BY audio.album_id;";
        db->ExecSQL(sb.ToString());

        // For a given artist_id, provides the album_id for albums on
        // which the artist appears.
        db->ExecSQL(String("CREATE VIEW IF NOT EXISTS artists_albums_map AS "
                "SELECT DISTINCT artist_id, album_id FROM audio_meta;"));

        /*
        * Only external media volumes can handle genres, playlists, etc.
        */
        if (!internal) {
            // Cleans up when an audio file is deleted
            db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS audio_meta_cleanup DELETE ON audio_meta "
                    "BEGIN "
                        "DELETE FROM audio_genres_map WHERE audio_id = old._id;"
                        "DELETE FROM audio_playlists_map WHERE audio_id = old._id;"
                    "END"));

            // Contains audio genre definitions
            db->ExecSQL(String("CREATE TABLE IF NOT EXISTS audio_genres ("
                    "_id INTEGER PRIMARY KEY,"
                    "name TEXT NOT NULL"
                    ");"));

            // Contains mappings between audio genres and audio files
            db->ExecSQL(String("CREATE TABLE IF NOT EXISTS audio_genres_map ("
                    "_id INTEGER PRIMARY KEY,"
                    "audio_id INTEGER NOT NULL,"
                    "genre_id INTEGER NOT NULL"
                    ");"));

            // Cleans up when an audio genre is delete
            db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS audio_genres_cleanup DELETE ON audio_genres "
                    "BEGIN "
                        "DELETE FROM audio_genres_map WHERE genre_id = old._id;"
                    "END"));

            // Contains audio playlist definitions
            db->ExecSQL(String("CREATE TABLE IF NOT EXISTS audio_playlists ("
                    "_id INTEGER PRIMARY KEY,"
                    "_data TEXT,"   // _data is path for file based playlists, or null
                    "name TEXT NOT NULL,"
                    "date_added INTEGER,"
                    "date_modified INTEGER"
                    ");"));

            // Contains mappings between audio playlists and audio files
            db->ExecSQL(String("CREATE TABLE IF NOT EXISTS audio_playlists_map ("
                   "_id INTEGER PRIMARY KEY,"
                   "audio_id INTEGER NOT NULL,"
                   "playlist_id INTEGER NOT NULL,"
                   "play_order INTEGER NOT NULL"
                   ");"));

            // Cleans up when an audio playlist is deleted
            db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS audio_playlists_cleanup DELETE ON audio_playlists "
                   "BEGIN "
                       "DELETE FROM audio_playlists_map WHERE playlist_id = old._id;"
                       "SELECT _DELETE_FILE(old._data);"
                   "END"));

            // Cleans up album_art table entry when an album is deleted
            db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS albumart_cleanup1 DELETE ON albums "
                    "BEGIN "
                        "DELETE FROM album_art WHERE album_id = old.album_id;"
                    "END"));

            // Cleans up album_art when an album is deleted
            db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS albumart_cleanup2 DELETE ON album_art "
                    "BEGIN "
                        "SELECT _DELETE_FILE(old._data);"
                    "END"));
        }

        // Contains meta data about video files
        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS video ("
                   "_id INTEGER PRIMARY KEY,"
                   "_data TEXT NOT NULL,"
                   "_display_name TEXT,"
                   "_size INTEGER,"
                   "mime_type TEXT,"
                   "date_added INTEGER,"
                   "date_modified INTEGER,"
                   "title TEXT,"
                   "duration INTEGER,"
                   "artist TEXT,"
                   "album TEXT,"
                   "resolution TEXT,"
                   "description TEXT,"
                   "isprivate INTEGER,"    // for YouTube videos
                   "tags TEXT,"            // for YouTube videos
                   "category TEXT,"        // for YouTube videos
                   "language TEXT,"        // for YouTube videos
                   "mini_thumb_data TEXT,"
                   "latitude DOUBLE,"
                   "longitude DOUBLE,"
                   "datetaken INTEGER,"
                   "mini_thumb_magic INTEGER"
                   ");"));

        db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS video_cleanup DELETE ON video "
                "BEGIN "
                    "SELECT _DELETE_FILE(old._data);"
                "END"));
    }

    // At this point the database is at least at schema version 63 (it was
    // either created at version 63 by the code above, or was already at
    // version 63 or later)

    if (fromVersion < 64) {
        // create the index that updates the database to schema version 64
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS sort_index on images(datetaken ASC, _id ASC);"));
    }

    /*
    *  Android 1.0 shipped with database version 64
    */

    if (fromVersion < 65) {
        // create the index that updates the database to schema version 65
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS titlekey_index on audio_meta(title_key);"));
    }

    // In version 66, originally we updateBucketNames(db, "images"),
    // but we need to do it in version 89 and therefore save the update here.//

    if (fromVersion < 67) {
        // create the indices that update the database to schema version 67
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS albumkey_index on albums(album_key);"));
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS artistkey_index on artists(artist_key);"));
    }

    if (fromVersion < 68) {
        // Create bucket_id and bucket_display_name columns for the video table.
        db->ExecSQL(String("ALTER TABLE video ADD COLUMN bucket_id TEXT;"));
        db->ExecSQL(String("ALTER TABLE video ADD COLUMN bucket_display_name TEXT"));//

        // In version 68, originally we updateBucketNames(db, "video"),
        // but we need to do it in version 89 and therefore save the update here.
    }

    if (fromVersion < 69) {
        UpdateDisplayName(db, String("images"));
    }

    if (fromVersion < 70) {
        // Create bookmark column for the video table.
        db->ExecSQL(String("ALTER TABLE video ADD COLUMN bookmark INTEGER;"));
    }

    if (fromVersion < 71) {
        // There is no change to the database schema, however a code change
        // fixed parsing of metadata for certain files bought from the
        // iTunes music store, so we want to rescan files that might need it.
        // We do this by clearing the modification date in the database for
        // those files, so that the media scanner will see them as updated
        // and rescan them.
        sb.Reset();
        sb += "UPDATE audio_meta SET date_modified=0 WHERE _id IN ("
                "SELECT _id FROM audio where mime_type='audio/mp4' AND ";
        sb += "artist='"; sb += IMediaStore::UNKNOWN_STRING; sb += "' AND ";
        sb += "album='"; sb += IMediaStore::UNKNOWN_STRING; sb += "');";
        db->ExecSQL(sb.ToString());
    }

    if (fromVersion < 72) {
        // Create is_podcast and bookmark columns for the audio table.
        db->ExecSQL(String("ALTER TABLE audio_meta ADD COLUMN is_podcast INTEGER;"));
        db->ExecSQL(String("UPDATE audio_meta SET is_podcast=1 WHERE _data LIKE '%/podcasts/%';"));
        db->ExecSQL(String("UPDATE audio_meta SET is_music=0 WHERE is_podcast=1"
                    " AND _data NOT LIKE '%/music/%';"));
        db->ExecSQL(String("ALTER TABLE audio_meta ADD COLUMN bookmark INTEGER;"));//

        // New columns added to tables aren't visible in views on those tables
        // without opening and closing the database (or using the 'vacuum' command,
        // which we can't do here because all this code runs inside a transaction).
        // To work around this, we drop and recreate the affected view and trigger.
        RecreateAudioView(db);
    }

    /*
    *  Android 1.5 shipped with database version 72
    */
    if (fromVersion < 73) {
        // There is no change to the database schema, but we now do case insensitive
        // matching of folder names when determining whether something is music, a
        // ringtone, podcast, etc, so we might need to reclassify some files.
        db->ExecSQL(String("UPDATE audio_meta SET is_music=1 WHERE is_music=0 AND "
                    "_data LIKE '%/music/%';"));
        db->ExecSQL(String("UPDATE audio_meta SET is_ringtone=1 WHERE is_ringtone=0 AND "
                    "_data LIKE '%/ringtones/%';"));
        db->ExecSQL(String("UPDATE audio_meta SET is_notification=1 WHERE is_notification=0 AND "
                    "_data LIKE '%/notifications/%';"));
        db->ExecSQL(String("UPDATE audio_meta SET is_alarm=1 WHERE is_alarm=0 AND "
                    "_data LIKE '%/alarms/%';"));
        db->ExecSQL(String("UPDATE audio_meta SET is_podcast=1 WHERE is_podcast=0 AND "
                    "_data LIKE '%/podcasts/%';"));
    }

    if (fromVersion < 74) {
        // This view is used instead of the audio view by the union below, to force
        // sqlite to use the title_key index. This greatly reduces memory usage
        // (no separate copy pass needed for sorting, which could cause errors on
        // large datasets) and improves speed (by about 35% on a large dataset)
        db->ExecSQL(String("CREATE VIEW IF NOT EXISTS searchhelpertitle AS SELECT * FROM audio "
                    "ORDER BY title_key;"));

        sb.Reset();
        sb += "CREATE VIEW IF NOT EXISTS search AS "
                    "SELECT _id,"
                    "'artist' AS mime_type,"
                    "artist,"
                    "NULL AS album,"
                    "NULL AS title,"
                    "artist AS text1,"
                    "NULL AS text2,"
                    "number_of_albums AS data1,"
                    "number_of_tracks AS data2,"
                    "artist_key AS match,"
                    "'content://media/external/audio/artists/'||_id AS suggest_intent_data,"
                    "1 AS grouporder "
                    "FROM artist_info WHERE (artist!='";
        sb += IMediaStore::UNKNOWN_STRING; sb += "') ";
        sb += "UNION ALL "
                    "SELECT _id,"
                    "'album' AS mime_type,"
                    "artist,"
                    "album,"
                    "NULL AS title,"
                    "album AS text1,"
                    "artist AS text2,"
                    "NULL AS data1,"
                    "NULL AS data2,"
                    "artist_key||' '||album_key AS match,"
                    "'content://media/external/audio/albums/'||_id AS suggest_intent_data,"
                    "2 AS grouporder "
                    "FROM album_info WHERE (album!='";
        sb += IMediaStore::UNKNOWN_STRING; sb += "') ";
        sb += "UNION ALL "
                    "SELECT searchhelpertitle._id AS _id,"
                    "mime_type,"
                    "artist,"
                    "album,"
                    "title,"
                    "title AS text1,"
                    "artist AS text2,"
                    "NULL AS data1,"
                    "NULL AS data2,"
                    "artist_key||' '||album_key||' '||title_key AS match,"
                    "'content://media/external/audio/media/'||searchhelpertitle._id AS "
                    "suggest_intent_data,"
                    "3 AS grouporder "
                    "FROM searchhelpertitle WHERE (title != '') ";
        db->ExecSQL(sb.ToString());
    }

    if (fromVersion < 75) {
        // Force a rescan of the audio entries so we can apply the new logic to
        // distinguish same-named albums.
        db->ExecSQL(String("UPDATE audio_meta SET date_modified=0;"));
        db->ExecSQL(String("DELETE FROM albums"));
    }

    if (fromVersion < 76) {
        // We now ignore double quotes when building the key, so we have to remove all of them
        // from existing keys.
        db->ExecSQL(String("UPDATE audio_meta SET title_key="
                    "REPLACE(title_key,x'081D08C29F081D',x'081D') "
                    "WHERE title_key LIKE '%'||x'081D08C29F081D'||'%';"));
        db->ExecSQL(String("UPDATE albums SET album_key="
                    "REPLACE(album_key,x'081D08C29F081D',x'081D') "
                    "WHERE album_key LIKE '%'||x'081D08C29F081D'||'%';"));
        db->ExecSQL(String("UPDATE artists SET artist_key="
                    "REPLACE(artist_key,x'081D08C29F081D',x'081D') "
                    "WHERE artist_key LIKE '%'||x'081D08C29F081D'||'%';"));
   }

   /*
    *  Android 1.6 shipped with database version 76
    */

    if (fromVersion < 77) {
        // create video thumbnail table
        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS videothumbnails ("
                    "_id INTEGER PRIMARY KEY,"
                    "_data TEXT,"
                    "video_id INTEGER,"
                    "kind INTEGER,"
                    "width INTEGER,"
                    "height INTEGER"
                    ");"));

        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS video_id_index on videothumbnails(video_id);"));

        db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS videothumbnails_cleanup DELETE ON videothumbnails "
                    "BEGIN "
                        "SELECT _DELETE_FILE(old._data);"
                    "END"));
    }

    /*
    *  Android 2.0 and 2.0.1 shipped with database version 77
    */

    if (fromVersion < 78) {
        // Force a rescan of the video entries so we can update
        // latest changed DATE_TAKEN units (in milliseconds).
        db->ExecSQL(String("UPDATE video SET date_modified=0;"));
    }

    /*
    *  Android 2.1 shipped with database version 78
    */

    if (fromVersion < 79) {
        // move /sdcard/albumthumbs to
        // /sdcard/Android/data/com.android.providers.media/albumthumbs,
        // and update the database accordingly//

        sb.Reset();
        sb += (*mExternalStoragePaths)[0];
        sb += "/albumthumbs";
        String oldthumbspath = sb.ToString();
        AutoPtr<IFile> thumbsfolder;
        CFile::New(oldthumbspath, (IFile**)&thumbsfolder);
        Boolean flag = FALSE;
        thumbsfolder->Exists(&flag);
        if (flag) {
            sb.Reset();
            sb += (*mExternalStoragePaths)[0];
            sb += "/";
            sb += ALBUM_THUMB_FOLDER;
            String newthumbspath = sb.ToString();
            // move folder to its new location
            AutoPtr<IFile> newthumbsfolder;
            CFile::New(newthumbspath, (IFile**)&newthumbsfolder);
            AutoPtr<IFile> parentFile;
            newthumbsfolder->GetParentFile((IFile**)&parentFile);
            parentFile->Mkdirs(&flag);
            thumbsfolder->RenameTo(newthumbsfolder, &flag);
            if (flag) {
                sb.Reset();
                sb += "UPDATE album_art SET _data=REPLACE(_data, '";
                sb += oldthumbspath; sb += "','"; sb += newthumbspath; sb += "');";
               // update the database
               db->ExecSQL(sb.ToString());
            }
        }
    }

    if (fromVersion < 80) {
        // Force rescan of image entries to update DATE_TAKEN as UTC timestamp.
        db->ExecSQL(String("UPDATE images SET date_modified=0;"));
    }

    if (fromVersion < 81 && !internal) {
        // Delete entries starting with /mnt/sdcard. This is for the benefit
        // of users running builds between 2.0.1 and 2.1 final only, since
        // users updating from 2.0 or earlier will not have such entries.//

        // First we need to update the _data fields in the affected tables, since
        // otherwise deleting the entries will also delete the underlying files
        // (via a trigger), and we want to keep them.
        db->ExecSQL(String("UPDATE audio_playlists SET _data='////' WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE images SET _data='////' WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE video SET _data='////' WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE videothumbnails SET _data='////' WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE thumbnails SET _data='////' WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE album_art SET _data='////' WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE audio_meta SET _data='////' WHERE _data LIKE '/mnt/sdcard/%';"));
        // Once the paths have been renamed, we can safely delete the entries
        db->ExecSQL(String("DELETE FROM audio_playlists WHERE _data IS '////';"));
        db->ExecSQL(String("DELETE FROM images WHERE _data IS '////';"));
        db->ExecSQL(String("DELETE FROM video WHERE _data IS '////';"));
        db->ExecSQL(String("DELETE FROM videothumbnails WHERE _data IS '////';"));
        db->ExecSQL(String("DELETE FROM thumbnails WHERE _data IS '////';"));
        db->ExecSQL(String("DELETE FROM audio_meta WHERE _data  IS '////';"));
        db->ExecSQL(String("DELETE FROM album_art WHERE _data  IS '////';"));

        // rename existing entries starting with /sdcard to /mnt/sdcard
        db->ExecSQL(String("UPDATE audio_meta SET _data='/mnt/sdcard'||SUBSTR(_data,8) WHERE _data LIKE '/sdcard/%';"));
        db->ExecSQL(String("UPDATE audio_playlists SET _data='/mnt/sdcard'||SUBSTR(_data,8) WHERE _data LIKE '/sdcard/%';"));
        db->ExecSQL(String("UPDATE images SET _data='/mnt/sdcard'||SUBSTR(_data,8) WHERE _data LIKE '/sdcard/%';"));
        db->ExecSQL(String("UPDATE video SET _data='/mnt/sdcard'||SUBSTR(_data,8) WHERE _data LIKE '/sdcard/%';"));
        db->ExecSQL(String("UPDATE videothumbnails SET _data='/mnt/sdcard'||SUBSTR(_data,8) WHERE _data LIKE '/sdcard/%';"));
        db->ExecSQL(String("UPDATE thumbnails SET _data='/mnt/sdcard'||SUBSTR(_data,8) WHERE _data LIKE '/sdcard/%';"));
        db->ExecSQL(String("UPDATE album_art SET _data='/mnt/sdcard'||SUBSTR(_data,8) WHERE _data LIKE '/sdcard/%';"));

        // Delete albums and artists, then clear the modification time on songs, which
        // will cause the media scanner to rescan everything, rebuilding the artist and
        // album tables along the way, while preserving playlists.
        // We need this rescan because ICU also changed, and now generates different
        // collation keys
        db->ExecSQL(String("DELETE from albums"));
        db->ExecSQL(String("DELETE from artists"));
        db->ExecSQL(String("UPDATE audio_meta SET date_modified=0;"));
    }

    if (fromVersion < 82) {
        // recreate this view with the correct "group by" specifier
        db->ExecSQL(String("DROP VIEW IF EXISTS artist_info"));
        db->ExecSQL(String("CREATE VIEW IF NOT EXISTS artist_info AS "
                   "SELECT artist_id AS _id, artist, artist_key, "
                   "COUNT(DISTINCT album_key) AS number_of_albums, "
                   "COUNT(*) AS number_of_tracks FROM audio WHERE is_music=1 "
                   "GROUP BY artist_key;"));
    }

    /* we skipped over version 83, and reverted versions 84, 85 and 86 *///

    if (fromVersion < 87) {
        // The fastscroll thumb needs an index on the strings being displayed,
        // otherwise the queries it does to determine the correct position
        // becomes really inefficient
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS title_idx on audio_meta(title);"));
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS artist_idx on artists(artist);"));
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS album_idx on albums(album);"));
    }

    if (fromVersion < 88) {
        // Clean up a few more things from versions 84/85/86, and recreate
        // the few things worth keeping from those changes.
        db->ExecSQL(String("DROP TRIGGER IF EXISTS albums_update1;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS albums_update2;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS albums_update3;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS albums_update4;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS artist_update1;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS artist_update2;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS artist_update3;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS artist_update4;"));
        db->ExecSQL(String("DROP VIEW IF EXISTS album_artists;"));
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS album_id_idx on audio_meta(album_id);"));
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS artist_id_idx on audio_meta(artist_id);"));
        // For a given artist_id, provides the album_id for albums on
        // which the artist appears.
        db->ExecSQL(String("CREATE VIEW IF NOT EXISTS artists_albums_map AS "
            "SELECT DISTINCT artist_id, album_id FROM audio_meta;"));
    }

    // In version 89, originally we updateBucketNames(db, "images") and
    // updateBucketNames(db, "video"), but in version 101 we now updateBucketNames
    //  for all files and therefore can save the update here.//

    if (fromVersion < 91) {
        // Never query by mini_thumb_magic_index
        db->ExecSQL(String("DROP INDEX IF EXISTS mini_thumb_magic_index"));//

        // sort the items by taken date in each bucket
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS image_bucket_index ON images(bucket_id, datetaken)"));
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS video_bucket_index ON video(bucket_id, datetaken)"));
    }


    // Gingerbread ended up going to version 100, but didn't yet have the "files"
    // table, so we need to create that if we're at 100 or lower. This means
    // we won't be able to upgrade pre-release Honeycomb.
    if (fromVersion <= 100) {
        // Remove various stages of work in progress for MTP support
        db->ExecSQL(String("DROP TABLE IF EXISTS objects"));
        db->ExecSQL(String("DROP TABLE IF EXISTS files"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS images_objects_cleanup;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_objects_cleanup;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS video_objects_cleanup;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS playlists_objects_cleanup;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS files_cleanup_images;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS files_cleanup_audio;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS files_cleanup_video;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS files_cleanup_playlists;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS media_cleanup;"));

        // Create a new table to manage all files in our storage.
        // This contains a union of all the columns from the old
        // images, audio_meta, videos and audio_playlist tables.
        db->ExecSQL(String("CREATE TABLE files ("
                    "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "_data TEXT,"     // this can be null for playlists
                    "_size INTEGER,"
                    "format INTEGER,"
                    "parent INTEGER,"
                    "date_added INTEGER,"
                    "date_modified INTEGER,"
                    "mime_type TEXT,"
                    "title TEXT,"
                    "description TEXT,"
                    "_display_name TEXT,"

                    // for images
                    "picasa_id TEXT,"
                    "orientation INTEGER,"

                    // for images and video
                    "latitude DOUBLE,"
                    "longitude DOUBLE,"
                    "datetaken INTEGER,"
                    "mini_thumb_magic INTEGER,"
                    "bucket_id TEXT,"
                    "bucket_display_name TEXT,"
                    "isprivate INTEGER,"

                    // for audio
                    "title_key TEXT,"
                    "artist_id INTEGER,"
                    "album_id INTEGER,"
                    "composer TEXT,"
                    "track INTEGER,"
                    "year INTEGER CHECK(year!=0),"
                    "is_ringtone INTEGER,"
                    "is_music INTEGER,"
                    "is_alarm INTEGER,"
                    "is_notification INTEGER,"
                    "is_podcast INTEGER,"
                    "album_artist TEXT,"

                    // for audio and video
                    "duration INTEGER,"
                    "bookmark INTEGER,"

                    // for video
                    "artist TEXT,"
                    "album TEXT,"
                    "resolution TEXT,"
                    "tags TEXT,"
                    "category TEXT,"
                    "language TEXT,"
                    "mini_thumb_data TEXT,"

                    // for playlists
                    "name TEXT,"

                    // media_type is used by the views to emulate the old
                    // images, audio_meta, videos and audio_playlist tables.
                    "media_type INTEGER,"

                    // Value of _id from the old media table.
                    // Used only for updating other tables during database upgrade.
                    "old_id INTEGER"
                    ");"));

        db->ExecSQL(String("CREATE INDEX path_index ON files(_data);"));
        db->ExecSQL(String("CREATE INDEX media_type_index ON files(media_type);"));//

        // Copy all data from our obsolete tables to the new files table//

        // Copy audio records first, preserving the _id column.
        // We do this to maintain compatibility for content Uris for ringtones.
        // Unfortunately we cannot do this for images and videos as well.
        // We choose to do this for the audio table because the fragility of Uris
        // for ringtones are the most common problem we need to avoid.
        sb.Reset();
        sb += "INSERT INTO files (_id,"; sb += AUDIO_COLUMNSv99;
        sb += ",old_id,media_type) SELECT _id,"; sb += AUDIO_COLUMNSv99;
        sb += ",_id,"; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO;
        sb += " FROM audio_meta;";
        db->ExecSQL(sb.ToString());

        sb.Reset();
        sb += "INSERT INTO files ("; sb += IMAGE_COLUMNSv407;
        sb += ",old_id,media_type) SELECT "; sb += IMAGE_COLUMNSv407;
        sb += ",_id,"; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE;
        sb += " FROM images;";
        db->ExecSQL(sb.ToString());

        sb.Reset();
        sb += "INSERT INTO files ("; sb += VIDEO_COLUMNSv407;
        sb += ",old_id,media_type) SELECT "; sb += VIDEO_COLUMNSv407;
        sb += ",_id,"; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO;
        sb += " FROM video;";
        db->ExecSQL(sb.ToString());

        if (!internal) {
            sb.Reset();
            sb += "INSERT INTO files ("; sb += PLAYLIST_COLUMNS;
            sb += ",old_id,media_type) SELECT "; sb += PLAYLIST_COLUMNS;
            sb += ",_id,"; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST;
            sb += " FROM audio_playlists;";
            db->ExecSQL(sb.ToString());
        }

        // Delete the old tables
        db->ExecSQL(String("DROP TABLE IF EXISTS images"));
        db->ExecSQL(String("DROP TABLE IF EXISTS audio_meta"));
        db->ExecSQL(String("DROP TABLE IF EXISTS video"));
        db->ExecSQL(String("DROP TABLE IF EXISTS audio_playlists"));

        // Create views to replace our old tables
        sb.Reset();
        sb += "CREATE VIEW images AS SELECT _id,"; sb += IMAGE_COLUMNSv407;
        sb += " FROM files WHERE "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE; sb += ";";
        db->ExecSQL(sb.ToString());

        sb.Reset();
        sb += "CREATE VIEW audio_meta AS SELECT _id,"; sb += AUDIO_COLUMNSv100;
        sb += " FROM files WHERE "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO; sb += ";";
        db->ExecSQL(sb.ToString());

        sb.Reset();
        sb += "CREATE VIEW video AS SELECT _id,"; sb += VIDEO_COLUMNSv407;
        sb += " FROM files WHERE "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO; sb += ";";
        db->ExecSQL(sb.ToString());

        if (!internal) {
            sb.Reset();
            sb += "CREATE VIEW audio_playlists AS SELECT _id,"; sb += PLAYLIST_COLUMNS;
            sb += " FROM files WHERE "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
            sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST; sb += ";";
            db->ExecSQL(sb.ToString());
        }

        // create temporary index to make the updates go faster
        db->ExecSQL(String("CREATE INDEX tmp ON files(old_id);"));//

        // update the image_id column in the thumbnails table.
        sb.Reset();
        sb += "UPDATE thumbnails SET image_id = (SELECT _id FROM files "
                "WHERE files.old_id = thumbnails.image_id AND files.media_type = ";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE; sb += ");";
        db->ExecSQL(sb.ToString());

        if (!internal) {
            // update audio_id in the audio_genres_map table, and
            // audio_playlists_map tables and playlist_id in the audio_playlists_map table
            sb.Reset();
            sb += "UPDATE audio_genres_map SET audio_id = (SELECT _id FROM files "
                    "WHERE files.old_id = audio_genres_map.audio_id "
                    "AND files.media_type = ";
            sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO; sb += ");";
            db->ExecSQL(sb.ToString());

            sb.Reset();
            sb += "UPDATE audio_playlists_map SET audio_id = (SELECT _id FROM files "
                    "WHERE files.old_id = audio_playlists_map.audio_id "
                    "AND files.media_type = ";
            sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO; sb += ");";
            db->ExecSQL(sb.ToString());

            sb.Reset();
            sb += "UPDATE audio_playlists_map SET playlist_id = (SELECT _id FROM files "
                    "WHERE files.old_id = audio_playlists_map.playlist_id "
                    "AND files.media_type = ";
            sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST; sb += ");";
            db->ExecSQL(sb.ToString());
        }

        // update video_id in the videothumbnails table.
        sb.Reset();
        sb += "UPDATE videothumbnails SET video_id = (SELECT _id FROM files "
                "WHERE files.old_id = videothumbnails.video_id AND files.media_type = ";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO; sb += ");";
        db->ExecSQL(sb.ToString());

        // we don't need this index anymore now
        db->ExecSQL(String("DROP INDEX tmp;"));

        // update indices to work on the files table
        db->ExecSQL(String("DROP INDEX IF EXISTS title_idx"));
        db->ExecSQL(String("DROP INDEX IF EXISTS album_id_idx"));
        db->ExecSQL(String("DROP INDEX IF EXISTS image_bucket_index"));
        db->ExecSQL(String("DROP INDEX IF EXISTS video_bucket_index"));
        db->ExecSQL(String("DROP INDEX IF EXISTS sort_index"));
        db->ExecSQL(String("DROP INDEX IF EXISTS titlekey_index"));
        db->ExecSQL(String("DROP INDEX IF EXISTS artist_id_idx"));
        db->ExecSQL(String("CREATE INDEX title_idx ON files(title);"));
        db->ExecSQL(String("CREATE INDEX album_id_idx ON files(album_id);"));
        db->ExecSQL(String("CREATE INDEX bucket_index ON files(bucket_id, datetaken);"));
        db->ExecSQL(String("CREATE INDEX sort_index ON files(datetaken ASC, _id ASC);"));
        db->ExecSQL(String("CREATE INDEX titlekey_index ON files(title_key);"));
        db->ExecSQL(String("CREATE INDEX artist_id_idx ON files(artist_id);"));

        // Recreate triggers for our obsolete tables on the new files table
        db->ExecSQL(String("DROP TRIGGER IF EXISTS images_cleanup"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_meta_cleanup"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS video_cleanup"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_playlists_cleanup"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_delete"));

        sb.Reset();
        sb += "CREATE TRIGGER IF NOT EXISTS images_cleanup DELETE ON files "
                "WHEN old.media_type = "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE;
        sb +=   " BEGIN "
                    "DELETE FROM thumbnails WHERE image_id = old._id;"
                    "SELECT _DELETE_FILE(old._data);"
                "END";
        db->ExecSQL(sb.ToString());

        sb.Reset();
        sb += "CREATE TRIGGER IF NOT EXISTS video_cleanup DELETE ON files "
                "WHEN old.media_type = "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO;
        sb += " BEGIN SELECT _DELETE_FILE(old._data);END";
        db->ExecSQL(sb.ToString());


        if (!internal) {
            sb.Reset();
            sb += "CREATE TRIGGER IF NOT EXISTS audio_meta_cleanup DELETE ON files "
                    "WHEN old.media_type = "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO;
            sb +=   " BEGIN "
                        "DELETE FROM audio_genres_map WHERE audio_id = old._id;"
                        "DELETE FROM audio_playlists_map WHERE audio_id = old._id;"
                    "END";
            db->ExecSQL(sb.ToString());

            sb.Reset();
            sb += "CREATE TRIGGER IF NOT EXISTS audio_playlists_cleanup DELETE ON files "
                    "WHEN old.media_type = "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST;
            sb +=   " BEGIN "
                        "DELETE FROM audio_playlists_map WHERE playlist_id = old._id;"
                        "SELECT _DELETE_FILE(old._data);"
                    "END";
            db->ExecSQL(sb.ToString());

            db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS audio_delete INSTEAD OF DELETE ON audio "
                "BEGIN "
                    "DELETE from files where _id=old._id;"
                    "DELETE from audio_playlists_map where audio_id=old._id;"
                    "DELETE from audio_genres_map where audio_id=old._id;"
                "END"));
        }
    }

    if (fromVersion < 301) {
        db->ExecSQL(String("DROP INDEX IF EXISTS bucket_index"));
        db->ExecSQL(String("CREATE INDEX bucket_index on files(bucket_id, media_type, datetaken, _id)"));
        db->ExecSQL(String("CREATE INDEX bucket_name on files(bucket_id, media_type, bucket_display_name)"));
    }

    if (fromVersion < 302) {
        db->ExecSQL(String("CREATE INDEX parent_index ON files(parent);"));
        db->ExecSQL(String("CREATE INDEX format_index ON files(format);"));
    }

    if (fromVersion < 303) {
        // the album disambiguator hash changed, so rescan songs and force
        // albums to be updated. Artists are unaffected.
        db->ExecSQL(String("DELETE from albums"));

        sb.Reset();
        sb += "UPDATE files SET date_modified=0 WHERE ";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO; sb += ";";
        db->ExecSQL(sb.ToString());
    }

    if (fromVersion < 304 && !internal) {
       // notifies host when files are deleted
        db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS files_cleanup DELETE ON files "
                    "BEGIN "
                        "SELECT _OBJECT_REMOVED(old._id);"
                    "END"));

    }

    if (fromVersion < 305 && internal) {
        // version 304 erroneously added this trigger to the internal database
        db->ExecSQL(String("DROP TRIGGER IF EXISTS files_cleanup"));
    }

    if (fromVersion < 306 && !internal) {
        // The genre list was expanded and genre string parsing was tweaked, so
        // rebuild the genre list
        sb.Reset();
        sb += "UPDATE files SET date_modified=0 WHERE ";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO; sb += ";";
        db->ExecSQL(sb.ToString());
        db->ExecSQL(String("DELETE FROM audio_genres_map"));
        db->ExecSQL(String("DELETE FROM audio_genres"));
    }

    if (fromVersion < 307 && !internal) {
        // Force rescan of image entries to update DATE_TAKEN by either GPSTimeStamp or
        // EXIF local time.
        sb.Reset();
        sb += "UPDATE files SET date_modified=0 WHERE ";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE; sb += ";";
        db->ExecSQL(sb.ToString());
    }

    // Honeycomb went up to version 307, ICS started at 401//

    // Database version 401 did not add storage_id to the internal database.
    // We need it there too, so add it in version 402
    if (fromVersion < 401 || (fromVersion == 401 && internal)) {
        // Add column for MTP storage ID
        db->ExecSQL(String("ALTER TABLE files ADD COLUMN storage_id INTEGER;"));
        // Anything in the database before this upgrade step will be in the primary storage
        Int32 storageId;
        AutoPtr<IMtpStorageHelper> msh;
        CMtpStorageHelper::AcquireSingleton((IMtpStorageHelper**)&msh);
        msh->GetStorageId(0, &storageId);
        sb.Reset();
        sb += "UPDATE files SET storage_id="; sb += storageId; sb += ";";
        db->ExecSQL(sb.ToString());
    }

    if (fromVersion < 403 && !internal) {
        db->ExecSQL(String("CREATE VIEW audio_genres_map_noid AS "
                    "SELECT audio_id,genre_id from audio_genres_map;"));
    }

    if (fromVersion < 404) {
        // There was a bug that could cause distinct same-named albums to be
        // combined again. Delete albums and force a rescan.
        db->ExecSQL(String("DELETE from albums"));
        sb.Reset();
        sb += "UPDATE files SET date_modified=0 WHERE ";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO; sb += ";";
        db->ExecSQL(sb.ToString());
    }

    if (fromVersion < 405) {
        // Add is_drm column.
        db->ExecSQL(String("ALTER TABLE files ADD COLUMN is_drm INTEGER;"));
        db->ExecSQL(String("DROP VIEW IF EXISTS audio_meta"));
        sb.Reset();
        sb += "CREATE VIEW audio_meta AS SELECT _id,"; sb += AUDIO_COLUMNSv405;
        sb += " FROM files WHERE "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO; sb += ";";
        db->ExecSQL(sb.ToString());
        RecreateAudioView(db);
    }

    if (fromVersion < 407) {
        // Rescan files in the media database because a new column has been added
        // in table files in version 405 and to recover from problems populating
        // the genre tables
        db->ExecSQL(String("UPDATE files SET date_modified=0;"));
    }

    if (fromVersion < 408) {
        // Add the width/height columns for images and video
        db->ExecSQL(String("ALTER TABLE files ADD COLUMN width INTEGER;"));
        db->ExecSQL(String("ALTER TABLE files ADD COLUMN height INTEGER;"));//

        // Rescan files to fill the columns
        db->ExecSQL(String("UPDATE files SET date_modified=0;"));//

        // Update images and video views to contain the width/height columns
        db->ExecSQL(String("DROP VIEW IF EXISTS images"));
        db->ExecSQL(String("DROP VIEW IF EXISTS video"));
        sb.Reset();
        sb += "CREATE VIEW images AS SELECT _id,"; sb += IMAGE_COLUMNS;
        sb += " FROM files WHERE "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE; sb += ";";
        db->ExecSQL(sb.ToString());

        sb.Reset();
        sb += "CREATE VIEW video AS SELECT _id,"; sb += VIDEO_COLUMNS;
        sb += " FROM files WHERE "; sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO; sb += ";";
        db->ExecSQL(sb.ToString());
    }

    if (fromVersion < 409 && !internal) {
        // A bug that prevented numeric genres from being parsed was fixed, so
        // rebuild the genre list
        sb.Reset();
        sb += "UPDATE files SET date_modified=0 WHERE ";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO; sb += ";";
        db->ExecSQL(sb.ToString());

        db->ExecSQL(String("DELETE FROM audio_genres_map"));
        db->ExecSQL(String("DELETE FROM audio_genres"));
    }

    // ICS went out with database version 409, JB started at 500//

    if (fromVersion < 500) {
        // we're now deleting the file in mediaprovider code, rather than via a trigger
        db->ExecSQL(String("DROP TRIGGER IF EXISTS videothumbnails_cleanup;"));
    }

    if (fromVersion < 501) {
        // we're now deleting the file in mediaprovider code, rather than via a trigger
        // the images_cleanup trigger would delete the image file and the entry
        // in the thumbnail table, which in turn would trigger thumbnails_cleanup
        // to delete the thumbnail image
        db->ExecSQL(String("DROP TRIGGER IF EXISTS images_cleanup;"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS thumbnails_cleanup;"));
    }

    if (fromVersion < 502) {
        // we're now deleting the file in mediaprovider code, rather than via a trigger
        db->ExecSQL(String("DROP TRIGGER IF EXISTS video_cleanup;"));
    }

    if (fromVersion < 503) {
        // genre and playlist cleanup now done in mediaprovider code, instead of in a trigger
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_delete"));
        db->ExecSQL(String("DROP TRIGGER IF EXISTS audio_meta_cleanup"));
    }

    if (fromVersion < 504) {
        // add an index to help with case-insensitive matching of paths
        db->ExecSQL(String("CREATE INDEX IF NOT EXISTS path_index_lower ON files(_data COLLATE NOCASE);"));
    }

    if (fromVersion < 505) {
        // Starting with schema 505 we fill in the width/height/resolution columns for videos,
        // so force a rescan of videos to fill in the blanks
        sb.Reset();
        sb += "UPDATE files SET date_modified=0 WHERE ";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE; sb += "=";
        sb += IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO; sb += ";";
        db->ExecSQL(sb.ToString());
    }

    if (fromVersion < 506) {
        // sd card storage got moved to /storage/sdcard0
        // first delete everything that already got scanned in /storage before this
        // update step was added
        db->ExecSQL(String("DROP TRIGGER IF EXISTS files_cleanup"));
        db->ExecSQL(String("DELETE FROM files WHERE _data LIKE '/storage/%';"));
        db->ExecSQL(String("DELETE FROM album_art WHERE _data LIKE '/storage/%';"));
        db->ExecSQL(String("DELETE FROM thumbnails WHERE _data LIKE '/storage/%';"));
        db->ExecSQL(String("DELETE FROM videothumbnails WHERE _data LIKE '/storage/%';"));
        // then rename everything from /mnt/sdcard/ to /storage/sdcard0,
        // and from /mnt/external1 to /storage/sdcard1
        db->ExecSQL(String("UPDATE files SET _data='/storage/sdcard0'||SUBSTR(_data,12) WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE files SET _data='/storage/sdcard1'||SUBSTR(_data,15) WHERE _data LIKE '/mnt/external1/%';"));
        db->ExecSQL(String("UPDATE album_art SET _data='/storage/sdcard0'||SUBSTR(_data,12) WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE album_art SET _data='/storage/sdcard1'||SUBSTR(_data,15) WHERE _data LIKE '/mnt/external1/%';"));
        db->ExecSQL(String("UPDATE thumbnails SET _data='/storage/sdcard0'||SUBSTR(_data,12) WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE thumbnails SET _data='/storage/sdcard1'||SUBSTR(_data,15) WHERE _data LIKE '/mnt/external1/%';"));
        db->ExecSQL(String("UPDATE videothumbnails SET _data='/storage/sdcard0'||SUBSTR(_data,12) WHERE _data LIKE '/mnt/sdcard/%';"));
        db->ExecSQL(String("UPDATE videothumbnails SET _data='/storage/sdcard1'||SUBSTR(_data,15) WHERE _data LIKE '/mnt/external1/%';"));//

        if (!internal) {
            db->ExecSQL(String("CREATE TRIGGER IF NOT EXISTS files_cleanup DELETE ON files "
                        "BEGIN "
                            "SELECT _OBJECT_REMOVED(old._id);"
                        "END"));
        }
    }

    if (fromVersion < 507) {
        // we update _data in version 506, we need to update the bucket_id as well
        UpdateBucketNames(db);
    }

    if (fromVersion < 508 && !internal) {
        // ensure we don't get duplicate entries in the genre map
        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS audio_genres_map_tmp ("
                    "_id INTEGER PRIMARY KEY,"
                    "audio_id INTEGER NOT NULL,"
                    "genre_id INTEGER NOT NULL,"
                    "UNIQUE (audio_id,genre_id) ON CONFLICT IGNORE"
                    ");"));
        db->ExecSQL(String("INSERT INTO audio_genres_map_tmp (audio_id,genre_id)"
                    " SELECT DISTINCT audio_id,genre_id FROM audio_genres_map;"));
       db->ExecSQL(String("DROP TABLE audio_genres_map;"));
       db->ExecSQL(String("ALTER TABLE audio_genres_map_tmp RENAME TO audio_genres_map;"));
    }

    if (fromVersion < 509) {
        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS log (time DATETIME PRIMARY KEY, message TEXT);"));
    }

    // Emulated external storage moved to user-specific paths
    if (fromVersion < 510 && Environment::IsExternalStorageEmulated()) {
        // File.fixSlashes() removes any trailing slashes
        AutoPtr<IFile> file = Environment::GetExternalStorageDirectory();
        String externalStorage;
        file->ToString(&externalStorage);;
        Logger::D(TAG, "Adjusting external storage paths to: %s", externalStorage.string());//

        AutoPtr<ArrayOf<String> > tables = ArrayOf<String>::Alloc(4);
        (*tables)[0] = TABLE_FILES;
        (*tables)[1] = TABLE_ALBUM_ART;
        (*tables)[2] = TABLE_THUMBNAILS;
        (*tables)[3] = TABLE_VIDEO_THUMBNAILS;
        for (Int32 i = 0; i <= 3; i++) {
            sb.Reset();
            sb += "UPDATE "; sb += (*tables)[i]; sb += " SET _data='"; sb += externalStorage;
            sb += "'||SUBSTR(_data,17) WHERE _data LIKE '/storage/sdcard0/%';";
            db->ExecSQL(sb.ToString());
        }
    }

    if (fromVersion < 511) {
        // we update _data in version 510, we need to update the bucket_id as well
        UpdateBucketNames(db);
    }

    // JB 4.2 went out with database version 511, starting next release with 600//

    if (fromVersion < 600) {
        // modify _data column to be unique and collate nocase. Because this drops the original
        // table and replaces it with a new one by the same name, we need to also recreate all
        // indices and triggers that refer to the files table.
        // Views don't need to be recreated.//
        sb.Reset();
        sb += "CREATE TABLE files2 (_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "_data TEXT UNIQUE";
        // the internal filesystem is case-sensitive
        sb += internal ? "," : " COLLATE NOCASE,";
        sb += "_size INTEGER,format INTEGER,parent INTEGER,date_added INTEGER,"
                "date_modified INTEGER,mime_type TEXT,title TEXT,description TEXT,"
                "_display_name TEXT,picasa_id TEXT,orientation INTEGER,latitude DOUBLE,"
                "longitude DOUBLE,datetaken INTEGER,mini_thumb_magic INTEGER,bucket_id TEXT,"
                "bucket_display_name TEXT,isprivate INTEGER,title_key TEXT,artist_id INTEGER,"
                "album_id INTEGER,composer TEXT,track INTEGER,year INTEGER CHECK(year!=0),"
                "is_ringtone INTEGER,is_music INTEGER,is_alarm INTEGER,"
                "is_notification INTEGER,is_podcast INTEGER,album_artist TEXT,"
                "duration INTEGER,bookmark INTEGER,artist TEXT,album TEXT,resolution TEXT,"
                "tags TEXT,category TEXT,language TEXT,mini_thumb_data TEXT,name TEXT,"
                "media_type INTEGER,old_id INTEGER,storage_id INTEGER,is_drm INTEGER,"
                "width INTEGER, height INTEGER);";
        db->ExecSQL(sb.ToString());

        // copy data from old table, squashing entries with duplicate _data
        db->ExecSQL(String("INSERT OR REPLACE INTO files2 SELECT * FROM files;"));
        db->ExecSQL(String("DROP TABLE files;"));
        db->ExecSQL(String("ALTER TABLE files2 RENAME TO files;"));//

        // recreate indices and triggers
        db->ExecSQL(String("CREATE INDEX album_id_idx ON files(album_id);"));
        db->ExecSQL(String("CREATE INDEX artist_id_idx ON files(artist_id);"));
        db->ExecSQL(String("CREATE INDEX bucket_index on files(bucket_id,media_type,"
                    "datetaken, _id);"));
        db->ExecSQL(String("CREATE INDEX bucket_name on files(bucket_id,media_type,"
                    "bucket_display_name);"));
        db->ExecSQL(String("CREATE INDEX format_index ON files(format);"));
        db->ExecSQL(String("CREATE INDEX media_type_index ON files(media_type);"));
        db->ExecSQL(String("CREATE INDEX parent_index ON files(parent);"));
        db->ExecSQL(String("CREATE INDEX path_index ON files(_data);"));
        db->ExecSQL(String("CREATE INDEX sort_index ON files(datetaken ASC, _id ASC);"));
        db->ExecSQL(String("CREATE INDEX title_idx ON files(title);"));
        db->ExecSQL(String("CREATE INDEX titlekey_index ON files(title_key);"));
        if (!internal) {
            db->ExecSQL(String("CREATE TRIGGER audio_playlists_cleanup DELETE ON files"
                        " WHEN old.media_type=4"
                        " BEGIN DELETE FROM audio_playlists_map WHERE playlist_id = old._id;"
                        "SELECT _DELETE_FILE(old._data);END;"));
           db->ExecSQL(String("CREATE TRIGGER files_cleanup DELETE ON files"
                        " BEGIN SELECT _OBJECT_REMOVED(old._id);END;"));
        }
    }

    if (fromVersion < 601) {
        // remove primary key constraint because column time is not necessarily unique
        db->ExecSQL(String("CREATE TABLE IF NOT EXISTS log_tmp (time DATETIME, message TEXT);"));
        db->ExecSQL(String("DELETE FROM log_tmp;"));
        db->ExecSQL(String("INSERT INTO log_tmp SELECT time, message FROM log order by rowid;"));
        db->ExecSQL(String("DROP TABLE log;"));
        db->ExecSQL(String("ALTER TABLE log_tmp RENAME TO log;"));
    }

    if (fromVersion < 700) {
        // fix datetaken fields that were added with an incorrect timestamp
        // datetaken needs to be in milliseconds, so should generally be a few orders of
        // magnitude larger than date_modified. If it's within the same order of magnitude, it
        // is probably wrong.
        // (this could do the wrong thing if your picture was actually taken before ~3/21/1970)
        db->ExecSQL(String("UPDATE files set datetaken=date_modified*1000"
                    " WHERE date_modified IS NOT NULL"
                    " AND datetaken IS NOT NULL"
                    " AND datetaken<date_modified*5;"));
   }

   SanityCheck(db, fromVersion);

   Int64 elapsedSeconds = (SystemClock::GetCurrentTimeMicro() - startTime) / 1000;
   sb.Reset();
   sb += "Database upgraded from version "; sb += fromVersion;
   sb += " to "; sb += toVersion; sb += " in "; sb += elapsedSeconds; sb += " ms";
   LogToDb(db, sb.ToString());
   if (LOCAL_LOGV) Logger::V(TAG, sb.ToString());
   return NOERROR;
}

void MediaProvider::SanityCheck(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 fromVersion)
{
    AutoPtr<ICursor> c1, c2;
    // try {
    String nullStr;
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = "count(*)";
    db->Query(String("audio_meta"), array,
           nullStr, NULL, nullStr, nullStr, nullStr, (ICursor**)&c1);
    (*array)[0] = "count(distinct _data)";
    db->Query(String("audio_meta"), array,
            nullStr, NULL, nullStr, nullStr, nullStr, (ICursor**)&c2);
    Boolean flag = FALSE;
    c1->MoveToFirst(&flag);
    c2->MoveToFirst(&flag);
    Int32 num1, num2;
    c1->GetInt32(0, &num1);
    c2->GetInt32(0, &num2);
    if (num1 != num2) {
       Logger::E(TAG, "audio_meta._data column is not unique while upgrading from schema %d : %d/%d", fromVersion, num1, num2);
       // Delete all audio_meta rows so they will be rebuilt by the media scanner
       db->ExecSQL(String("DELETE FROM audio_meta;"));
    }
    // } finally {
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(c1));
    ioUtils->CloseQuietly(ICloseable::Probe(c2));
    // }
}

void MediaProvider::RecreateAudioView(
    /* [in] */ ISQLiteDatabase* db)
{
    // Provides a unified audio/artist/album info view.
    db->ExecSQL(String("DROP VIEW IF EXISTS audio"));
    db->ExecSQL(String("CREATE VIEW IF NOT EXISTS audio as SELECT * FROM audio_meta "
                "LEFT OUTER JOIN artists ON audio_meta.artist_id=artists.artist_id "
                "LEFT OUTER JOIN albums ON audio_meta.album_id=albums.album_id;"));
}

void MediaProvider::UpdateBucketNames(
    /* [in] */ ISQLiteDatabase* db)
{
    // Rebuild the bucket_display_name column using the natural case rather than lower case.
    db->BeginTransaction();
    // try {
    AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(2);
    (*columns)[0] = IBaseColumns::ID;
    (*columns)[1] = IMediaStoreMediaColumns::DATA;
    // update only images and videos
    AutoPtr<ICursor> cursor;
    String nullStr;
    db->Query(String("files"), columns, String("media_type=1 OR media_type=3"),
           NULL, nullStr, nullStr, nullStr, (ICursor**)&cursor);
    // try {
    Int32 idColumnIndex, dataColumnIndex;
    cursor->GetColumnIndex(IBaseColumns::ID, &idColumnIndex);
    cursor->GetColumnIndex(IMediaStoreMediaColumns::DATA, &dataColumnIndex);
    AutoPtr<ArrayOf<String> > rowId = ArrayOf<String>::Alloc(1);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    Boolean bval = FALSE;
    Int32 vol;
    while ((cursor->MoveToNext(&bval), bval)) {
        String data, tmp;
        cursor->GetString(dataColumnIndex, &data);
        cursor->GetString(idColumnIndex, &tmp);
        (*rowId)[0] = tmp;//

        if (!data.IsNull()) {
            values->Clear();
            ComputeBucketValues(data, values);
            db->Update(String("files"), values, String("_id=?"), rowId, &vol);
        } else {
            Logger::W(TAG, "null data at id %s", tmp.string());
        }
    }
       // } finally {
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(cursor));
       // }
    db->SetTransactionSuccessful();
   // } finally {
    db->EndTransaction();
   // }
}

void MediaProvider::UpdateDisplayName(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& tableName)
{
    // Fill in default values for null displayName values
    db->BeginTransaction();
    // try {
    String nullStr;
    AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(3);
    (*columns)[0] = IBaseColumns::ID;
    (*columns)[1] = IMediaStoreMediaColumns::DATA;
    (*columns)[2] = IMediaStoreMediaColumns::DISPLAY_NAME;
    AutoPtr<ICursor> cursor;
    db->Query(tableName, columns, nullStr, NULL, nullStr, nullStr, nullStr, (ICursor**)&cursor);
    // try {
    Int32 idColumnIndex, dataColumnIndex, displayNameIndex;
    cursor->GetColumnIndex(IBaseColumns::ID, &idColumnIndex);
    cursor->GetColumnIndex(IMediaStoreMediaColumns::DATA, &dataColumnIndex);
    cursor->GetColumnIndex(IMediaStoreMediaColumns::DISPLAY_NAME, &displayNameIndex);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    Boolean flag = FALSE;
    while ((cursor->MoveToNext(&flag), flag)) {
        String displayName;
        cursor->GetString(displayNameIndex, &displayName);
        if (displayName.IsNull()) {
            String data;
            cursor->GetString(dataColumnIndex, &data);
            values->Clear();
            ComputeDisplayName(data, values);
            Int32 rowId, vol;
            cursor->GetInt32(idColumnIndex, &rowId);
            StringBuilder sb("_id=");
            sb += rowId;
            db->Update(tableName, values, sb.ToString(), NULL, &vol);
        }
    }
       // } finally {
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(cursor));
       // }
    db->SetTransactionSuccessful();
   // } finally {
    db->EndTransaction();
   // }
}

void MediaProvider::ComputeDisplayName(
    /* [in] */ const String& data,
    /* [in] */ IContentValues* values)
{
    String s = (data.IsNull() ? String("") : data);
    Int32 idx = s.LastIndexOf('/');
    if (idx >= 0) {
        s = s.Substring(idx + 1);
    }
    values->Put(String("_display_name"), s);
}

void MediaProvider::ComputeTakenTime(
    /* [in] */ IContentValues* values)
{
    Boolean flag = FALSE;
    values->ContainsKey(IMediaStoreImagesImageColumns::DATE_TAKEN, &flag);
    if (!flag) {
        // This only happens when MediaScanner finds an image file that doesn't have any useful
        // reference to get this value. (e.g. GPSTimeStamp)
        AutoPtr<IInteger64> lastModified;
        values->GetAsInteger64(IMediaStoreMediaColumns::DATE_MODIFIED, (IInteger64**)&lastModified);
        if (lastModified != NULL) {
            Int64 value;
            lastModified->GetValue(&value);
            values->Put(IMediaStoreImagesImageColumns::DATE_TAKEN, value * 1000);
        }
    }
}

Boolean MediaProvider::MatchThumbRequest(
    /* [in] */ MediaThumbRequest* req,
    /* [in] */ Int32 pid,
    /* [in] */ Int64 id,
    /* [in] */ Int64 gid,
    /* [in] */ Boolean isVideo)
{
    Boolean cancelAllOrigId = (id == -1);
    Boolean cancelAllGroupId = (gid == -1);
    return (req->mCallingPid == pid) &&
           (cancelAllGroupId || req->mGroupId == gid) &&
           (cancelAllOrigId || req->mOrigId == id) &&
           (req->mIsVideo == isVideo);
}

Boolean MediaProvider::QueryThumbnail(
    /* [in] */ ISQLiteQueryBuilder* qb,
    /* [in] */ IUri* uri,
    /* [in] */ const String& table,
    /* [in] */ const String& column,
    /* [in] */ Boolean hasThumbnailId)
{
    AutoPtr<IList> list;
    uri->GetPathSegments((IList**)&list);
    AutoPtr<IInterface> obj;
    AutoPtr<ICharSequence> cs;
    StringBuilder sb;

    qb->SetTables(table);
    if (hasThumbnailId) {
        // For uri dispatched to this method, the 4th path segment is always
        // the thumbnail id.
        list->Get(3, (IInterface**)&obj);
        sb += "_id = "; sb += TO_STR(obj);
        qb->AppendWhere(CoreUtils::Convert(sb.ToString()));
        // client already knows which thumbnail it wants, bypass it.
        return TRUE;
   }
    String origId;
    uri->GetQueryParameter(String("orig_id"), &origId);
    // We can't query ready_flag unless we know original id
    if (origId.IsNull()) {
        // this could be thumbnail query for other purpose, bypass it.
        return TRUE;
    }

    String block, cancel;
    uri->GetQueryParameter(String("blocking"), &block);
    uri->GetQueryParameter(String("cancel"), &cancel);
    Boolean needBlocking = block.Equals("1");
    Boolean cancelRequest = cancel.Equals("1");
    AutoPtr<IUri> origUri;
    AutoPtr<IUriBuilder> ub;
    uri->BuildUpon((IUriBuilder**)&ub);
    String path, temp;
    uri->GetPath(&temp);
    StringUtils::ReplaceFirst(temp, String("thumbnails"), String("media"), &path);
    ub->EncodedPath(path);
    ub->AppendPath(origId);
    ub->Build((IUri**)&origUri);

    if (needBlocking && !WaitForThumbnailReady(origUri)) {
        Logger::W(TAG, "original media doesn't exist or it's canceled.");
        return FALSE;
    } else if (cancelRequest) {
        String groupId;
        uri->GetQueryParameter(String("group_id"), &groupId);
        obj = NULL;
        list->Get(1, (IInterface**)&obj);
        Boolean isVideo = Object::ToString(obj).Equals("video");
        Int32 pid = Binder::GetCallingPid();
        Int64 id = -1;
        Int64 gid = -1;

        // try {
        id = StringUtils::ParseInt64(origId);
        gid = StringUtils::ParseInt64(groupId);
       // } catch (NumberFormatException ex) {
           // invalid cancel request
           // return false;
       // }

        {
            AutoLock syncLock(mMediaThumbQueue);
            if (mCurrentThumbRequest != NULL &&
                MatchThumbRequest((MediaThumbRequest*)(mCurrentThumbRequest.Get()), pid, id, gid, isVideo)) {
                {
                    MediaThumbRequest* mtqObj = (MediaThumbRequest*)(mCurrentThumbRequest.Get());
                    AutoLock syncLock(mtqObj);
                    mtqObj->mState = CANCEL;
                    mtqObj->NotifyAll();
                }
            }

            AutoPtr<IIterator> it;
            mMediaThumbQueue->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IMediaThumbRequest> mtq = IMediaThumbRequest::Probe(obj);
                MediaThumbRequest* mtqObj = (MediaThumbRequest*)mtq.Get();
                if (MatchThumbRequest(mtqObj, pid, id, gid, isVideo)) {
                    {
                        AutoLock lock(mtqObj);
                        mtqObj->mState = CANCEL;
                        mtqObj->NotifyAll();
                    }

                    Boolean flag = FALSE;
                    mMediaThumbQueue->Remove(mtq, &flag);
                }
            }
        }
    }

    if (!origId.IsNull()) {
        StringBuilder sb(column); sb += " = "; sb += origId;
        cs = CoreUtils::Convert(sb.ToString());
        qb->AppendWhere(cs);
    }
    return TRUE;
}

void MediaProvider::ComputeBucketValues(
    /* [in] */ const String& data,
    /* [in] */ IContentValues* values)
{
    AutoPtr<IFile> file;
    CFile::New(data, (IFile**)&file);
    AutoPtr<IFile> parentFile;
    file->GetParentFile((IFile**)&parentFile);
    if (parentFile == NULL) {
        CFile::New(String("/"), (IFile**)&parentFile);
    }

    // Lowercase the path for hashing. This avoids duplicate buckets if the
    // filepath case is changed externally.
    // Keep the original case for display.//

    String str;
    parentFile->ToString(&str);
    String path = str.ToLowerCase();
    String name;
    parentFile->GetName(&name);//

    // Note: the BUCKET_ID and BUCKET_DISPLAY_NAME attributes are spelled the
    // same for both images and video. However, for backwards-compatibility reasons
    // there is no common base class. We use the ImageColumns version here
    Int32 hashCode = path.GetHashCode();
    values->Put(IMediaStoreImagesImageColumns::BUCKET_ID, hashCode);
    values->Put(IMediaStoreImagesImageColumns::BUCKET_DISPLAY_NAME, name);
}

AutoPtr<ArrayOf<String> > MediaProvider::Combine(
    /* [in] */ List<String>& prepend,
    /* [in] */ ArrayOf<String>* userArgs)
{
    Int32 presize = prepend.GetSize();
    if (presize == 0) {
       return userArgs;
    }//

    Int32 usersize = (userArgs != NULL) ? userArgs->GetLength() : 0;
    AutoPtr<ArrayOf<String> > combined = ArrayOf<String>::Alloc(presize + usersize);

    List<String>::Iterator it = prepend.Begin();
    for (Int32 i = 0; it != prepend.End(); ++it) {
       (*combined)[i] = *it;
       i++;
    }

    for (Int32 i = 0; i < usersize; i++) {
       (*combined)[presize + i] = (*userArgs)[i];
    }
    return combined;
}

AutoPtr<ICursor> MediaProvider::DoAudioSearch(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ ISQLiteQueryBuilder* qb,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sort,
    /* [in] */ Int32 mode,
    /* [in] */ const String& limit)
{
    String path;
    uri->GetPath(&path);
    Boolean flag = path.EndWith("/");
    String lpSegment;
    uri->GetLastPathSegment(&lpSegment);
    String mSearchString = flag ? String("") : lpSegment;
    String replace;
    StringUtils::ReplaceAll(mSearchString, "  ", " ", &replace);
    mSearchString = replace.Trim().ToLowerCase();

    Int32 slength = mSearchString.GetLength();

    AutoPtr<ArrayOf<String> > stemp;
    StringUtils::Split(mSearchString, String(" "), (ArrayOf<String>**)&stemp);

    AutoPtr<ArrayOf<String> > stemp2 = ArrayOf<String>::Alloc(0);
    AutoPtr<ArrayOf<String> > searchWords = slength > 0 ? stemp : stemp2;
    Int32 swLength = searchWords->GetLength();
    AutoPtr<ArrayOf<String> > wildcardWords = ArrayOf<String>::Alloc(swLength);
    Int32 len = searchWords->GetLength();
    AutoPtr<IMediaStoreAudio> msa;
    CMediaStoreAudio::AcquireSingleton((IMediaStoreAudio**)&msa);
    String key;
    String tmp;
    for (Int32 i = 0; i < len; i++) {
       // Because we match on individual words here, we need to remove words
       // like 'a' and 'the' that aren't part of the keys.
       msa->KeyFor((*searchWords)[i], &key);
       StringUtils::Replace(key, "\\", "\\\\", &tmp);
       key = tmp;
       StringUtils::Replace(key, "%", "\\%", &tmp);
       key = tmp;
       StringUtils::Replace(key, "_", "\\_", &tmp);
       key = tmp;
       (*wildcardWords)[i] =
           ((*searchWords)[i].Equals("a") || (*searchWords)[i].Equals("an") ||
                   (*searchWords)[i].Equals("the")) ? String("%") : String("%") + key + "%";
    }

    StringBuilder where("");
    len = searchWords->GetLength();
    for (Int32 i = 0; i < len; i++) {
       if (i == 0) {
           where += "match LIKE ? ESCAPE '\\'";
       } else {
           where += " AND match LIKE ? ESCAPE '\\'";
       }
    }

    qb->SetTables(String("search"));
    AutoPtr<ArrayOf<String> > cols;
    if (mode == AUDIO_SEARCH_FANCY) {
       cols = mSearchColsFancy;
    } else if (mode == AUDIO_SEARCH_BASIC) {
       cols = mSearchColsBasic;
    } else {
       cols = mSearchColsLegacy;
    }
    String nullStr;
    AutoPtr<ICursor> c;
    qb->Query(db, cols, where.ToString(), wildcardWords, nullStr, nullStr, limit, (ICursor**)&c);
    return c;
}

ECode MediaProvider::EnsureFile(
    /* [in] */ Boolean internal,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ const String& preferredExtension,
    /* [in] */ const String& directoryName,
    /* [out] */ IContentValues** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IContentValues> values;
    String file;
    initialValues->GetAsString(IMediaStoreMediaColumns::DATA, &file);
    AutoPtr<ITextUtils> tu;
    CTextUtils::AcquireSingleton((ITextUtils**)&tu);
    Boolean flag = FALSE;
    tu->IsEmpty(file, &flag);
    if (flag) {
       file = GenerateFileName(internal, preferredExtension, directoryName);
       CContentValues::New(initialValues, (IContentValues**)&values);
       values->Put(IMediaStoreMediaColumns::DATA, file);
    }
    else {
       values = initialValues;
    }

    // we used to create the file here, but now defer this until openFile() is called
    *result = values;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void MediaProvider::SendObjectAdded(
    /* [in] */ Int64 objectHandle)
{
    AutoLock syncLock(mMtpServiceConnection);
    if (mMtpService != NULL) {
        // try {
        mMtpService->SendObjectAdded((Int32)objectHandle);
        // } catch (RemoteException e) {
        Logger::E(TAG, "RemoteException in sendObjectAdded");
        mMtpService = NULL;
        // }
    }
}

void MediaProvider::SendObjectRemoved(
    /* [in] */ Int64 objectHandle)
{
    AutoLock syncLock(mMtpServiceConnection);
    if (mMtpService != NULL) {
        // try {
        ECode ec = mMtpService->SendObjectRemoved((Int32)objectHandle);
        // } catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::E(TAG, "RemoteException in sendObjectRemoved");
            mMtpService = NULL;
        }
    // }
    }
}

void MediaProvider::NotifyMtp(
    /* [in] */ IArrayList* rowIds)
{
    Int32 size;
    rowIds->GetSize(&size);
    Int64 vol;
    AutoPtr<IInterface> obj;
    for (Int32 i = 0; i < size; ++i) {
        obj = NULL;
        rowIds->Get(i, (IInterface**)&obj);
        IInteger64::Probe(obj)->GetValue(&vol);
        SendObjectAdded(vol);
    }
}

Int32 MediaProvider::PlaylistBulkInsert(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues*>* values)
{
    AutoPtr<IDatabaseInsertHelper> helper;
    CDatabaseInsertHelper::New(db, String("audio_playlists_map"), (IDatabaseInsertHelper**)&helper);
    Int32 audioidcolidx, playlistididx, playorderidx;
    helper->GetColumnIndex(IMediaStoreAudioPlaylistsMembers::AUDIO_ID, &audioidcolidx);
    helper->GetColumnIndex(IMediaStoreAudioPlaylistsMembers::PLAYLIST_ID, &playlistididx);
    helper->GetColumnIndex(IMediaStoreAudioPlaylistsMembers::PLAY_ORDER, &playorderidx);
    AutoPtr<IList> list;
    uri->GetPathSegments((IList**)&list);
    AutoPtr<IInterface> obj;
    list->Get(3, (IInterface**)&obj);
    Int64 playlistId = StringUtils::ParseInt64(TO_STR(obj));//

    db->BeginTransaction();
    Int32 numInserted = 0;
    // try {
        Int32 len = values->GetLength();
        Int64 audioid;
        Int32 playorder;
        Int64 ret;
        for (Int32 i = 0; i < len; i++) {
            helper->PrepareForInsert();
            // getting the raw Object and converting it long ourselves saves
            // an allocation (the alternative is ContentValues.getAsLong, which
            // returns a Long object)
            obj = NULL;
            ((*values)[i])->Get(IMediaStoreAudioPlaylistsMembers::AUDIO_ID, (IInterface**)&obj);
            IInteger64::Probe(obj)->GetValue(&audioid);
            helper->BindInt64(audioidcolidx, audioid);
            helper->BindInt64(playlistididx, playlistId);

            // convert to int ourselves to save an allocation.
            obj = NULL;
            (*values)[i]->Get(IMediaStoreAudioPlaylistsMembers::PLAY_ORDER, (IInterface**)&obj);
            IInteger32::Probe(obj)->GetValue(&playorder);
            helper->BindInt32(playorderidx, playorder);
            helper->Execute(&ret);
       }
       numInserted = len;
       db->SetTransactionSuccessful();
    // } finally {
       db->EndTransaction();
       helper->Close();
    // }
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->NotifyChange(uri, NULL);
    return numInserted;
}

ECode MediaProvider::BulkInsert(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues*>* values,
    /* [out] */ Int32* result)
{
    Logger::I(TAG, "====================BulkInsert: %s, %d",
        TO_CSTR(uri), values ? values->GetLength() : 0);
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 match;
    GetURI_MATCHER()->Match(uri, &match);
    if (match == VOLUMES) {
        return ContentProvider::BulkInsert(uri, values, result);
    }
    AutoPtr<IDatabaseHelper> dbh;
    GetDatabaseForUri(uri, (IDatabaseHelper**)&dbh);
    if (dbh == NULL) {
        Logger::E(TAG, "Unknown URI: s", TO_CSTR(uri));
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    DatabaseHelper* helper = (DatabaseHelper*)dbh.Get();
    AutoPtr<ISQLiteDatabase> db;
    helper->GetWritableDatabase((ISQLiteDatabase**)&db);
    if (db == NULL) {
        Logger::E(TAG, "Couldn't open database for %s", TO_CSTR(uri));
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (match == AUDIO_PLAYLISTS_ID || match == AUDIO_PLAYLISTS_ID_MEMBERS) {
        *result = PlaylistBulkInsert(db, uri, values);
        return NOERROR;
    }
    else if (match == MTP_OBJECT_REFERENCES) {
        AutoPtr<IList> list;
        uri->GetPathSegments((IList**)&list);
        AutoPtr<IInterface> obj;
        list->Get(2, (IInterface**)&obj);
        Int32 handle = StringUtils::ParseInt32(TO_STR(obj));
        *result = SetObjectReferences(helper, db, handle, values);
        return NOERROR;
    }

    db->BeginTransaction();
    AutoPtr<IArrayList> notifyRowIds;
    CArrayList::New((IArrayList**)&notifyRowIds);
    Int32 numInserted = 0;

    Int32 len = values->GetLength();
    AutoPtr<IUri> tmpUri;
    for (Int32 i = 0; i < len; i++) {
        if ((*values)[i] != NULL) {
            tmpUri = NULL;
            InsertInternal(uri, match, (*values)[i], notifyRowIds, (IUri**)&tmpUri);
        }
    }
    numInserted = len;
    db->SetTransactionSuccessful();
    db->EndTransaction();

    // Notify MTP (outside of successful transaction)
    NotifyMtp(notifyRowIds);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->NotifyChange(uri, NULL);
    return numInserted;
}

ECode MediaProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** result)
{
    if (LOCAL_LOGV) Logger::V(TAG, " >> Insert: %s, values: %s", TO_CSTR(uri), TO_CSTR(initialValues));

    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 match;
    GetURI_MATCHER()->Match(uri, &match);

    AutoPtr<IArrayList> notifyRowIds;
    CArrayList::New((IArrayList**)&notifyRowIds);

    AutoPtr<IUri> newUri;
    InsertInternal(uri, match, initialValues, notifyRowIds, (IUri**)&newUri);
    NotifyMtp(notifyRowIds);

    // do not signal notification for MTP objects.
    // we will signal instead after file transfer is successful.
    if (newUri != NULL && match != MTP_OBJECTS) {
       AutoPtr<IContext> context;
       GetContext((IContext**)&context);
       AutoPtr<IContentResolver> resolver;
       context->GetContentResolver((IContentResolver**)&resolver);
       resolver->NotifyChange(uri, NULL);
    }
    *result = newUri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaProvider::ApplyBatch(
    /* [in] */ IArrayList* operations,
    /* [out] */ ArrayOf<IContentProviderResult*>** result)
{
    VALIDATE_NOT_NULL(result)
    // The operations array provides no overall information about the URI(s) being operated
    // on, so begin a transaction for ALL of the databases.
    AutoPtr<IDatabaseHelper> ihelper;
    AutoPtr<IDatabaseHelper> ehelper;
    AutoPtr<IMediaStoreAudioMedia> msa;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msa);
    AutoPtr<IUri> uri;
    msa->GetINTERNAL_CONTENT_URI((IUri**)&uri);
    GetDatabaseForUri(uri, (IDatabaseHelper**)&ihelper);
    uri = NULL;
    msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    GetDatabaseForUri(uri, (IDatabaseHelper**)&ehelper);
    AutoPtr<ISQLiteDatabase> idb;
    ((DatabaseHelper*)(ihelper.Get()))->GetWritableDatabase((ISQLiteDatabase**)&idb);
    idb->BeginTransaction();
    AutoPtr<ISQLiteDatabase> edb;
    if (ehelper != NULL) {
        ((DatabaseHelper*)(ehelper.Get()))->GetWritableDatabase((ISQLiteDatabase**)&edb);
        edb->BeginTransaction();
    }
    // try {
    ContentProvider::ApplyBatch(operations, result);
    idb->SetTransactionSuccessful();
    if (edb != NULL) {
        edb->SetTransactionSuccessful();
    }
    // Rather than sending targeted change notifications for every Uri
    // affected by the batch operation, just invalidate the entire internal
    // and external name space.
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> res;
    context->GetContentResolver((IContentResolver**)&res);
    uri = NULL;
    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    uh->Parse(String("content://media/"), (IUri**)&uri);
    res->NotifyChange(uri, NULL);

    // } finally {
    idb->EndTransaction();
    if (edb != NULL) {
        edb->EndTransaction();
    }
    return NOERROR;
    // }
}

Int64 MediaProvider::InsertDirectory(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& path)
{
    if (LOCAL_LOGV) Logger::V(TAG, "inserting directory %s", path.string());
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IMediaStoreFilesFileColumns::FORMAT, IMtpConstants::FORMAT_ASSOCIATION);
    values->Put(IMediaStoreMediaColumns::DATA, path);
    values->Put(IMediaStoreFilesFileColumns::PARENT, GetParent(helper, db, path));
    values->Put(IMediaStoreFilesFileColumns::STORAGE_ID, GetStorageId(path));
    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    Boolean flag = FALSE;
    file->Exists(&flag);
    Int64 lastModified;
    file->GetLastModified(&lastModified);
    if (flag) {
        values->Put(IMediaStoreMediaColumns::DATE_MODIFIED, lastModified / 1000);
    }
    helper->mNumInserts++;
    Int64 rowId;
    db->Insert(String("files"), IMediaStoreMediaColumns::DATE_MODIFIED, values, &rowId);
    SendObjectAdded(rowId);
    return rowId;
}

Int64 MediaProvider::GetParent(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& path)
{
    Int32 lastSlash = path.LastIndexOf('/');
    if (lastSlash > 0) {
        String parentPath = path.Substring(0, lastSlash);
        Int32 length = mExternalStoragePaths->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if (parentPath.Equals((*mExternalStoragePaths)[i])) {
               return 0;
            }
        }
        HashMap<String, Int64>::Iterator it = mDirectoryCache->Find(parentPath);
        if (it != mDirectoryCache->End()) {
            Int64 cid = it->mSecond;
            if (LOCAL_LOGV) Logger::V(TAG, "Returning cached entry for %s", parentPath.string());
            return cid;
        }

        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        String selection = IMediaStoreMediaColumns::DATA + "=?";
        AutoPtr<ArrayOf<String> > selargs = ArrayOf<String>::Alloc(1);
        (*selargs)[0] = parentPath;
        helper->mNumQueries++;
        AutoPtr<ICursor> c;
        db->Query(String("files"), sIdOnlyColumn, selection, selargs, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
        Int64 id;
        Int32 count;
        if (c == NULL || (c->GetCount(&count), count == 0)) {
           // parent isn't in the database - so add it
           id = InsertDirectory(helper, db, parentPath);
           if (LOCAL_LOGV) Logger::V(TAG, "Inserted %s", parentPath.string());
        }
        else {
            if (count > 1) {
               Logger::E(TAG, "more than one match for %s", parentPath.string());
            }
            Boolean flag = FALSE;
            c->MoveToFirst(&flag);
            c->GetInt64(0, &id);
            if (LOCAL_LOGV) Logger::V(TAG, "Queried %s", parentPath.string());
        }
        if (c) ioUtils->CloseQuietly(ICloseable::Probe(c));

        (*mDirectoryCache)[parentPath] = id;
        return id;
    }
    return 0;
}

Int32 MediaProvider::GetStorageId(
    /* [in] */ const String& path)
{
    Int32 length = mExternalStoragePaths->GetLength();
    AutoPtr<IMtpStorageHelper> msh;
    CMtpStorageHelper::AcquireSingleton((IMtpStorageHelper**)&msh);
    Int32 vol;
    for (Int32 i = 0; i < length; i++) {
        String test = (*mExternalStoragePaths)[i];
        if (path.StartWith(test)) {
            Int32 length = test.GetLength();
            Int32 pathLength = path.GetLength();
            if (pathLength == length || path.GetChar(length) == '/') {
               msh->GetStorageId(i, &vol);
               return vol;
            }
        }
    }
    // default to primary storage
    msh->GetStorageId(0, &vol);
    return vol;
}

ECode MediaProvider::InsertFile(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ Int32 mediaType,
    /* [in] */ Boolean notify,
    /* [in] */ IArrayList* notifyRowIds,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<ISQLiteDatabase> db;
    helper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IContentValues> values;

    AutoPtr<ISystem> isystem;
    CSystem::AcquireSingleton((ISystem**)&isystem);
    Int64 timeMillis;

    Boolean flag = FALSE;

    AutoPtr<IMediaFile> mf;
    CMediaFile::AcquireSingleton((IMediaFile**)&mf);

    switch (mediaType) {
        case IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE: {
            ECode ec = EnsureFile(helper->mInternal, initialValues,
                String(".jpg"), String("Pictures"), (IContentValues**)&values);
            FAIL_RETURN(ec)

            isystem->GetCurrentTimeMillis(&timeMillis);
            values->Put(IMediaStoreMediaColumns::DATE_ADDED, (timeMillis) / 1000);
            String data;
            values->GetAsString(IMediaStoreMediaColumns::DATA, &data);
            values->ContainsKey(IMediaStoreMediaColumns::DISPLAY_NAME, &flag);
            if (!flag) {
                ComputeDisplayName(data, values);
            }
            ComputeTakenTime(values);
            break;
        }//

        case IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO: {
            // SQLite Views are read-only, so we need to deconstruct this
            // insert and do inserts into the underlying tables.
            // If doing this here turns out to be a performance bottleneck,
            // consider moving this to native code and using triggers on
            // the view.
            CContentValues::New(initialValues, (IContentValues**)&values);

            String albumartist, compilation;
            values->GetAsString(IMediaStoreAudioAudioColumns::ALBUM_ARTIST, &albumartist);
            values->GetAsString(IMediaStoreAudioAudioColumns::COMPILATION, &compilation);
            values->Remove(IMediaStoreAudioAudioColumns::COMPILATION);

            const String artistStr("artist");
            // Insert the artist into the artist table and remove it from
            // the input values
            AutoPtr<IInterface> so;
            values->Get(artistStr, (IInterface**)&so);
            String s = (so == NULL ? String("") : TO_STR(so));
            values->Remove(artistStr);
            Int64 artistRowId;

            String path;
            values->GetAsString(IMediaStoreMediaColumns::DATA, &path);
            {
                AutoLock syncLock(helper->mArtistCacheLock);
                HashMap<String, Int64> artistCache = helper->mArtistCache;
                HashMap<String, Int64>::Iterator it = artistCache.Find(s);
                if (it == artistCache.End()) {
                    artistRowId = GetKeyIdForName(helper, db,
                        String("artists"), String("artist_key"), artistStr,
                        s, s, path, 0, String(NULL), artistCache, uri);
                }
                else {
                   artistRowId = it->mSecond;
                }
            }
            String artist = s;

            // Do the same for the album field
            so = NULL;
            values->Get(String("album"), (IInterface**)&so);
            s = (so == NULL ? String("") : TO_STR(so));
            values->Remove(String("album"));
            Int64 albumRowId;
            {
                AutoLock syncLock(helper->mAlbumCacheLock);
                HashMap<String, Int64> albumCache = helper->mAlbumCache;
                Int32 albumhash = 0;
                if (!albumartist.IsNull()) {
                    albumhash = albumartist.GetHashCode();
                }
                else if (!compilation.IsNull() && compilation.Equals("1")) {
                   // nothing to do, hash already set
                }
                else {
                    Int32 index = path.LastIndexOf('/');
                    albumhash = path.Substring(0, index).GetHashCode();
                }
                StringBuilder sb(s);
                sb += albumhash;
                String cacheName = sb.ToString();
                HashMap<String, Int64>::Iterator it = albumCache.Find(cacheName);
                if (it == albumCache.End()) {
                    albumRowId = GetKeyIdForName(helper, db,
                        String("albums"), String("album_key"), String("album"),
                        s, cacheName, path, albumhash, artist, albumCache, uri);
                }
                else {
                    albumRowId = it->mSecond;
                }
            }

            values->Put(String("artist_id"), StringUtils::ToString((Int32)artistRowId));
            values->Put(String("album_id"), StringUtils::ToString((Int32)albumRowId));
            values->GetAsString(String("title"), &s);

            AutoPtr<IMediaStoreAudio> msa;
            CMediaStoreAudio::AcquireSingleton((IMediaStoreAudio**)&msa);
            String key;
            msa->KeyFor(s, &key);
            values->Put(String("title_key"), key);
            // do a final trim of the title, in case it started with the special
            // "sort first" character (ascii \001)
            values->Remove(String("title"));
            values->Put(String("title"), s.Trim());

            values->GetAsString(IMediaStoreMediaColumns::DATA, &s);
            ComputeDisplayName(s, values);
            break;
        }

        case IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO: {
            ECode ec = EnsureFile(helper->mInternal, initialValues,
                String(".3gp"), String("video"), (IContentValues**)&values);
            FAIL_RETURN(ec)
            String data;
            values->GetAsString(IMediaStoreMediaColumns::DATA, &data);
            ComputeDisplayName(data, values);
            ComputeTakenTime(values);
            break;
        }
    }

    if (values == NULL) {
        CContentValues::New(initialValues, (IContentValues**)&values);
    }
    // compute bucket_id and bucket_display_name for all files
    String path;
    values->GetAsString(IMediaStoreMediaColumns::DATA, &path);
    if (!path.IsNull()) {
       ComputeBucketValues(path, values);
    }
    isystem->GetCurrentTimeMillis(&timeMillis);
    values->Put(IMediaStoreMediaColumns::DATE_ADDED, timeMillis / 1000);

    Int64 rowId = 0;
    AutoPtr<IInteger32> ivalObj;
    values->GetAsInteger32(IMediaStoreMediaColumns::MEDIA_SCANNER_NEW_OBJECT_ID, (IInteger32**)&ivalObj);
    if (ivalObj != 0) {
        Int32 ival;
        ivalObj->GetValue(&ival);
        rowId = ival;
        CContentValues::New(values, (IContentValues**)&values);
        values->Remove(IMediaStoreMediaColumns::MEDIA_SCANNER_NEW_OBJECT_ID);
    }

    String title;
    values->GetAsString(IMediaStoreMediaColumns::TITLE, &title);
    if (title.IsNull() && !path.IsNull()) {
       mf->GetFileTitle(path, &title);
    }
    values->Put(IMediaStoreFilesFileColumns::TITLE, title);

    String mimeType;
    values->GetAsString(IMediaStoreMediaColumns::MIME_TYPE, &mimeType);
    AutoPtr<IInteger32> formatObject;
    values->GetAsInteger32(IMediaStoreFilesFileColumns::FORMAT, (IInteger32**)&formatObject);
    Int32 format = 0;
    if (formatObject != NULL) formatObject->GetValue(&format);
    if (format == 0) {
       if (TextUtils::IsEmpty(path)) {
            // special case device created playlists
            if (mediaType == IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST) {
                values->Put(IMediaStoreFilesFileColumns::FORMAT, IMtpConstants::FORMAT_ABSTRACT_AV_PLAYLIST);
                // create a file path for the benefit of MTP
                String vStr;
                values->GetAsString(IMediaStoreAudioPlaylistsColumns::NAME, &vStr);
                StringBuilder sb((*mExternalStoragePaths)[0]);
                sb += "/Playlists/"; sb += vStr;
                path = sb.ToString();
                values->Put(IMediaStoreMediaColumns::DATA, path);
                values->Put(IMediaStoreFilesFileColumns::PARENT, GetParent(helper, db, path));
            }
            else {
                Logger::E(TAG, "path is empty in insertFile()");
            }
       }
       else {
            mf->GetFormatCode(path, mimeType, &format);
       }
    }
    if (format != 0) {
        values->Put(IMediaStoreFilesFileColumns::FORMAT, format);
        if (mimeType.IsNull()) {
            mf->GetMimeTypeForFormatCode(format, &mimeType);
        }
    }

    if (mimeType.IsNull() && !path.IsNull()) {
        mf->GetMimeTypeForFile(path, &mimeType);
    }
    if (!mimeType.IsNull()) {
        values->Put(IMediaStoreMediaColumns::MIME_TYPE, mimeType);

        AutoPtr<IMediaScannerHelper> msh;
        CMediaScannerHelper::AcquireSingleton((IMediaScannerHelper**)&msh);
        if (mediaType == IMediaStoreFilesFileColumns::MEDIA_TYPE_NONE
            && (msh->IsNoMediaPath(path, &flag), !flag)) {
            Int32 fileType;
            mf->GetFileTypeForMimeType(mimeType, &fileType);
            if ((mf->IsAudioFileType(fileType, &flag), flag)) {
                mediaType = IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO;
            }
            else if ((mf->IsVideoFileType(fileType, &flag), flag)) {
                mediaType = IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO;
            }
            else if ((mf->IsImageFileType(fileType, &flag), flag)) {
                mediaType = IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE;
            }
            else if ((mf->IsPlayListFileType(fileType, &flag), flag)) {
                mediaType = IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST;
            }
        }
    }
    values->Put(IMediaStoreFilesFileColumns::MEDIA_TYPE, mediaType);

    if (rowId == 0) {
       if (mediaType == IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST) {
            String name;
            values->GetAsString(IMediaStoreAudioPlaylistsColumns::NAME, &name);
            if (name.IsNull() && path.IsNull()) {
                // MediaScanner will compute the name from the path if we have one
                Logger::E(TAG, "no name was provided when inserting abstract playlist");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else {
            if (path.IsNull()) {
                // path might be null for playlists created on the device
                // or transfered via MTP
                Logger::E(TAG, "no path was provided when inserting new file");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        // make sure modification date and size are set
        if (!path.IsNull()) {
            AutoPtr<IFile> file;
            CFile::New(path, (IFile**)&file);
            if ((file->Exists(&flag), flag)) {
                Int64 flf;
                file->GetLastModified(&flf);
                values->Put(IMediaStoreMediaColumns::DATE_MODIFIED, flf / 1000);
                if (!(values->ContainsKey(IMediaStoreMediaColumns::SIZE, &flag), flag)) {
                   Int64 flen;
                   file->GetLength(&flen);
                   values->Put(IMediaStoreMediaColumns::SIZE, flen);
                }
                // make sure date taken time is set
                if (mediaType == IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE
                       || mediaType == IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO) {
                   ComputeTakenTime(values);
                }
            }
        }

        AutoPtr<IInteger64> parent;
        values->GetAsInteger64(IMediaStoreFilesFileColumns::PARENT, (IInteger64**)&parent);
        if (parent != NULL) {
            if (!path.IsNull()) {
                Int64 parentId = GetParent(helper, db, path);
                values->Put(IMediaStoreFilesFileColumns::PARENT, parentId);
            }
        }

        AutoPtr<IInteger32> storage;
        values->GetAsInteger32(IMediaStoreFilesFileColumns::STORAGE_ID, (IInteger32**)&storage);
        if (storage == NULL) {
           Int32 storageId = GetStorageId(path);
           values->Put(IMediaStoreFilesFileColumns::STORAGE_ID, storageId);
        }

        helper->mNumInserts++;
        db->Insert(String("files"), IMediaStoreMediaColumns::DATE_MODIFIED, values, &rowId);
        if (LOCAL_LOGV) Logger::V(TAG, "insertFile: values=%s returned: %lld", TO_CSTR(values), rowId);

        if (rowId != -1 && notify) {
           AutoPtr<IInteger64> irowId = CoreUtils::Convert(rowId);
           notifyRowIds->Add(irowId);
        }
    }
    else {
        helper->mNumUpdates++;
        AutoPtr<ArrayOf<String> > sarr = ArrayOf<String>::Alloc(1);
        (*sarr)[0] = StringUtils::ToString(rowId);
        StringBuilder sb(IBaseColumns::ID); sb += "=?";
        Int32 vol;
        db->Update(String("files"), values, sb.ToString(), sarr, &vol);
    }
    if (format == IMtpConstants::FORMAT_ASSOCIATION) {
        (*mDirectoryCache)[path] = rowId;
    }

    *result = rowId;
    return NOERROR;
}

AutoPtr<ICursor> MediaProvider::GetObjectReferences(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 handle)
{
    helper->mNumQueries++;
    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = StringUtils::ToString(handle);
    db->Query(String("files"), sMediaTableColumns, String("_id=?"),
           arr, String(NULL), String(NULL), String(NULL), (ICursor**)&c);

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);//

    // try {
       Boolean flag = FALSE;
       c->MoveToFirst(&flag);
       if (c != NULL && flag) {
            Int64 playlistId;
            c->GetInt64(0, &playlistId);
            Int32 mediaType;
            c->GetInt32(1, &mediaType);
            if (mediaType != IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST) {
               // we only support object references for playlist objects
               ioUtils->CloseQuietly(ICloseable::Probe(c));
               return NULL;
            }
            helper->mNumQueries++;
            AutoPtr<ArrayOf<String> > strArray = ArrayOf<String>::Alloc(1);
            (*strArray)[0] = StringUtils::ToString(playlistId);
            db->RawQuery(OBJECT_REFERENCES_QUERY, strArray, (ICursor**)&c);
            ioUtils->CloseQuietly(ICloseable::Probe(c));
            return c;
        }
    // } finally {
       // AutoPtr<IIoUtils> ioUtils;
       // CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
       // ioUtils->CloseQuietly(ICloseable::Probe(c));
    // }
    ioUtils->CloseQuietly(ICloseable::Probe(c));
    return NULL;
}

Int32 MediaProvider::SetObjectReferences(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 handle,
    /* [in] */ ArrayOf<IContentValues*>* values)
{
    // first look up the media table and media ID for the object
    Int64 playlistId = 0;
    helper->mNumQueries++;
    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = StringUtils::ToString(handle);

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    db->Query(String("files"), sMediaTableColumns, String("_id=?"),
           arr, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
    // try {
        Boolean flag = FALSE;
        if (c != NULL && (c->MoveToNext(&flag), flag)) {
            Int32 mediaType;
            c->GetInt32(1, &mediaType);
            if (mediaType != IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST) {
                // we only support object references for playlist objects
                ioUtils->CloseQuietly(ICloseable::Probe(c));
                return 0;
            }
            c->GetInt64(0, &playlistId);
        }
    // } finally {
       // ioUtils->CloseQuietly(ICloseable::Probe(c));
    // }
    if (playlistId == 0) {
        ioUtils->CloseQuietly(ICloseable::Probe(c));
        return 0;
    }

    // next delete any existing entries
    helper->mNumDeletes++;
    AutoPtr<ArrayOf<String> > as = ArrayOf<String>::Alloc(1);
    (*as)[0] = StringUtils::ToString(playlistId);
    Int32 r1;
    db->Delete(String("audio_playlists_map"), String("playlist_id=?"), as, &r1);//

    // finally add the new entries
    Int32 count = values->GetLength();
    Int32 added = 0;
    AutoPtr<ArrayOf<IContentValues*> > valuesList = ArrayOf<IContentValues*>::Alloc(count);
    String nullStr;

    for (Int32 i = 0; i < count; i++) {
        // convert object ID to audio ID
        Int64 audioId = 0, objectId = 0;
        AutoPtr<IInteger64> valObj;
        (*values)[i]->GetAsInteger64(IBaseColumns::ID, (IInteger64**)&valObj);
        valObj->GetValue(&objectId);
        helper->mNumQueries++;
        c = NULL;
        AutoPtr<ArrayOf<String> > r2 = ArrayOf<String>::Alloc(1);
        (*r2)[0] = StringUtils::ToString(objectId);
        db->Query(String("files"), sMediaTableColumns, String("_id=?"),
            r2, nullStr, nullStr, nullStr, (ICursor**)&c);
       // try {
        if (c != NULL && (c->MoveToNext(&flag), flag)) {
            Int32 mediaType;
            c->GetInt32(1, &mediaType);
            if (mediaType != IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO) {
                // we only allow audio files in playlists, so skip
                continue;
            }
            c->GetInt64(0, &audioId);
        }
       // } finally {

           // ioUtils->CloseQuietly(ICloseable:Probe(c));
       // }
       if (audioId != 0) {
           AutoPtr<IContentValues> v;
           CContentValues::New((IContentValues**)&v);
           v->Put(IMediaStoreAudioPlaylistsMembers::PLAYLIST_ID, playlistId);
           v->Put(IMediaStoreAudioPlaylistsMembers::AUDIO_ID, audioId);
           v->Put(IMediaStoreAudioPlaylistsMembers::PLAY_ORDER, added);
           (*valuesList)[added++] = v;
       }
    }
    if (added < count) {
       // we weren't able to find everything on the list, so lets resize the array
       // and pass what we have.
       AutoPtr<ArrayOf<IContentValues*> > newValues = ArrayOf<IContentValues*>::Alloc(added);

       for (Int32 i = 0; i < added; ++i) {
           newValues->Set(i, (*valuesList)[i]);
       }
       valuesList = newValues;
    }
    AutoPtr<IMediaStoreAudioPlaylistsMembers> mspl;
    CMediaStoreAudioPlaylistsMembers::AcquireSingleton((IMediaStoreAudioPlaylistsMembers**)&mspl);
    AutoPtr<IUri> uri;
    mspl->GetContentUri(EXTERNAL_VOLUME, playlistId, (IUri**)&uri);
    ioUtils->CloseQuietly(ICloseable::Probe(c));
    return PlaylistBulkInsert(db, uri, valuesList);
}

void MediaProvider::UpdateGenre(
    /* [in] */ Int64 rowId,
    /* [in] */ const String& genre)
{
    AutoPtr<IUri> uri;
    AutoPtr<ICursor> cursor;
    AutoPtr<IUri> genresUri;
    AutoPtr<IMediaStoreAudioGenres> msag;
    CMediaStoreAudioGenres::AcquireSingleton((IMediaStoreAudioGenres**)&msag);
    msag->GetContentUri(String("external"), (IUri**)&genresUri);
    // try {
        // see if the genre already exists
        AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
        (*arr)[0] = genre;
        Query(genresUri, GENRE_LOOKUP_PROJECTION, IMediaStoreAudioGenresColumns::NAME + "=?",
                       arr, String(NULL), (ICursor**)&cursor);
        Int32 count;
        cursor->GetCount(&count);
        if (cursor == NULL || count == 0) {
           // genre does not exist, so create the genre in the genre table
           AutoPtr<IContentValues> values;
           CContentValues::New((IContentValues**)&values);
           values->Put(IMediaStoreAudioGenresColumns::NAME, genre);
           Insert(genresUri, values, (IUri**)&uri);
        }
        else {
            // genre already exists, so compute its Uri
            Boolean flag = FALSE;
            cursor->MoveToNext(&flag);
            AutoPtr<IContentUris> cu;
            CContentUris::AcquireSingleton((IContentUris**)&cu);
            Int64 cvol;
            cursor->GetInt64(0, &cvol);
            cu->WithAppendedId(genresUri, cvol, (IUri**)&uri);
        }
        if (uri != NULL) {
            AutoPtr<IUriHelper> uh;
            CUriHelper::AcquireSingleton((IUriHelper**)&uh);
            uh->WithAppendedPath(uri, IMediaStoreAudioGenresMembers::CONTENT_DIRECTORY, (IUri**)&uri);
        }
    // } finally {
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        ioUtils->CloseQuietly(ICloseable::Probe(cursor));
    // }//

    if (uri != NULL) {
        // add entry to audio_genre_map
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        values->Put(IMediaStoreAudioGenresMembers::AUDIO_ID, StringUtils::ToString(rowId));
        AutoPtr<IUri> uuri;
        Insert(uri, values, (IUri**)&uuri);
    }
}

ECode MediaProvider::InsertInternal(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 match,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ IArrayList* notifyRowIds,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    String volumeName = GetVolumeName(uri);
    Int64 rowId;

    if (LOCAL_LOGV) Logger::V(TAG, "volumeName: %s, insertInternal: %s, initValues=%s",
        volumeName.string(), TO_CSTR(uri), TO_CSTR(initialValues));

    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);

    // handle MEDIA_SCANNER before calling getDatabaseForUri()
    if (match == MEDIA_SCANNER) {
        initialValues->GetAsString(IMediaStore::MEDIA_SCANNER_VOLUME, &mMediaScannerVolume);
        StringBuilder sb("content://media/"); sb += mMediaScannerVolume; sb += "/audio";
        AutoPtr<IUri> iuri;
        uh->Parse(sb.ToString(), (IUri**)&iuri);
        AutoPtr<IDatabaseHelper> database;
        GetDatabaseForUri(iuri, (IDatabaseHelper**)&database);
        if (database == NULL) {
            Logger::W(TAG, "no database for scanned volume %s", mMediaScannerVolume.string());
        }
        else {
            ((DatabaseHelper*)database.Get())->mScanStartTime = SystemClock::GetCurrentTimeMicro();
        }
        AutoPtr<IMediaStore> ms;
        CMediaStore::AcquireSingleton((IMediaStore**)&ms);
        iuri = NULL;
        ms->GetMediaScannerUri((IUri**)&iuri);
        *result = iuri;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    String genre;
    String path;
    if (initialValues != NULL) {
        initialValues->GetAsString(IMediaStoreAudioAudioColumns::GENRE, &genre);
        initialValues->Remove(IMediaStoreAudioAudioColumns::GENRE);
        initialValues->GetAsString(IMediaStoreMediaColumns::DATA, &path);
    }

    AutoPtr<IUri> newUri;
    AutoPtr<IDatabaseHelper> helper;
    GetDatabaseForUri(uri, (IDatabaseHelper**)&helper);
    if (helper == NULL && match != VOLUMES && match != MTP_CONNECTED) {
        Logger::E(TAG, "Unknown URI: %s", TO_CSTR(uri));
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    DatabaseHelper* dbh = (DatabaseHelper*)helper.Get();
    AutoPtr<ISQLiteDatabase> isd;
    AutoPtr<ISQLiteDatabase> db = ((match == VOLUMES || match == MTP_CONNECTED)
            ? NULL : (dbh->GetWritableDatabase((ISQLiteDatabase**)&isd), isd));

    AutoPtr<IContentUris> icu;
    CContentUris::AcquireSingleton((IContentUris**)&icu);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<IList> list;
    uri->GetPathSegments((IList**)&list);
    AutoPtr<IInterface> obj;
    AutoPtr<ICharSequence> cs;
    String tmp;

    Boolean flag = FALSE;
    AutoPtr<IContentValues> values;
    switch (match) {
        case IMAGES_MEDIA: {
            InsertFile(dbh, uri, initialValues,
                    IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE, TRUE, notifyRowIds, &rowId);
            if (rowId > 0) {
                MediaDocumentsProvider::OnMediaStoreInsert(
                        context, volumeName, IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE, rowId);
                AutoPtr<IMediaStoreImagesMedia> mim;
                CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&mim);
                AutoPtr<IUri> iiuri;
                mim->GetContentUri(volumeName, (IUri**)&iiuri);
                icu->WithAppendedId(iiuri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert images %s, initValues: %s", TO_CSTR(uri), TO_CSTR(initialValues));
                }
            }
            break;
        }

         // This will be triggered by requestMediaThumbnail (see getThumbnailUri)
        case IMAGES_THUMBNAILS: {
            EnsureFile(dbh->mInternal, initialValues, String(".jpg"),
                    String("DCIM/.thumbnails"), (IContentValues**)&values);
            dbh->mNumInserts++;
            db->Insert(String("thumbnails"), String("name"), values, &rowId);
            if (rowId > 0) {
                AutoPtr<IUri> tUri;
                AutoPtr<IMediaStoreImagesThumbnails> mit;
                CMediaStoreImagesThumbnails::AcquireSingleton((IMediaStoreImagesThumbnails**)&mit);
                mit->GetContentUri(volumeName, (IUri**)&tUri);
                icu->WithAppendedId(tUri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert thumbnails, initValues: %s", TO_CSTR(values));
                }
            }
            break;
        }

        // This is currently only used by MICRO_KIND video thumbnail (see getThumbnailUri)
        case VIDEO_THUMBNAILS: {
            EnsureFile(dbh->mInternal, initialValues, String(".jpg"),
                    String("DCIM/.thumbnails"), (IContentValues**)&values);
            dbh->mNumInserts++;
            db->Insert(String("videothumbnails"), String("name"), values, &rowId);
            if (rowId > 0) {
                AutoPtr<IMediaStoreVideoThumbnails> mvt;
                CMediaStoreVideoThumbnails::AcquireSingleton((IMediaStoreVideoThumbnails**)&mvt);
                AutoPtr<IUri> iiuri;
                mvt->GetContentUri(volumeName, (IUri**)&iiuri);
                icu->WithAppendedId(iiuri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert videothumbnails, initValues: %s", TO_CSTR(values));
                }
            }
            break;
        }

        case AUDIO_MEDIA: {
            InsertFile(dbh, uri, initialValues,
                    IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO, TRUE, notifyRowIds, &rowId);
            if (rowId > 0) {
                MediaDocumentsProvider::OnMediaStoreInsert(
                        context, volumeName, IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO, rowId);
                AutoPtr<IMediaStoreAudioMedia> msam;
                CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msam);
                AutoPtr<IUri> inUri;
                msam->GetContentUri(volumeName, (IUri**)&inUri);
                icu->WithAppendedId(inUri, rowId, (IUri**)&newUri);
                if (!genre.IsNull()) {
                    UpdateGenre(rowId, genre);
                }
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert audio %s, initValues: %s", TO_CSTR(uri), TO_CSTR(initialValues));
                }
            }
            break;
        }

        case AUDIO_MEDIA_ID_GENRES: {
            list->Get(2, (IInterface**)&obj);
            Int64 audioId = StringUtils::ParseInt64(TO_STR(obj));
            AutoPtr<IContentValues> values;
            CContentValues::New(initialValues, (IContentValues**)&values);
            values->Put(IMediaStoreAudioGenresMembers::AUDIO_ID, audioId);
            dbh->mNumInserts++;
            db->Insert(String("audio_genres_map"), String("genre_id"), values, &rowId);
            if (rowId > 0) {
                icu->WithAppendedId(uri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert audio_genres_map, initValues: %s", TO_CSTR(values));
                }
            }
            break;
        }

        case AUDIO_MEDIA_ID_PLAYLISTS: {
            list->Get(2, (IInterface**)&obj);
            Int64 audioId = StringUtils::ParseInt64(TO_STR(obj));
            AutoPtr<IContentValues> values;
            CContentValues::New(initialValues, (IContentValues**)&values);
            values->Put(IMediaStoreAudioPlaylistsMembers::AUDIO_ID, audioId);
            dbh->mNumInserts++;
            db->Insert(String("audio_playlists_map"), String("playlist_id"),
                    values, &rowId);
            if (rowId > 0) {
                icu->WithAppendedId(uri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert audio_playlists_map, initValues: %s", TO_CSTR(values));
                }
            }
            break;
        }

        case AUDIO_GENRES: {
            dbh->mNumInserts++;
            db->Insert(String("audio_genres"), String("audio_id"), initialValues, &rowId);
            if (rowId > 0) {
                AutoPtr<IMediaStoreAudioGenres> msag;
                CMediaStoreAudioGenres::AcquireSingleton((IMediaStoreAudioGenres**)&msag);
                AutoPtr<IUri> iiuri;
                msag->GetContentUri(volumeName, (IUri**)&iiuri);
                icu->WithAppendedId(iiuri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert audio_genres, initValues: %s", TO_CSTR(initialValues));
                }
            }
            break;
        }

        case AUDIO_GENRES_ID_MEMBERS: {
            list->Get(3, (IInterface**)&obj);
            Int64 genreId = StringUtils::ParseInt64(TO_STR(obj));
            AutoPtr<IContentValues> values;
            CContentValues::New(initialValues, (IContentValues**)&values);
            values->Put(IMediaStoreAudioGenresMembers::GENRE_ID, genreId);
            dbh->mNumInserts++;
            db->Insert(String("audio_genres_map"), String("genre_id"), values, &rowId);
            if (rowId > 0) {
                icu->WithAppendedId(uri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert audio_genres_map, initValues: %s", TO_CSTR(values));
                }
            }
            break;
        }

        case AUDIO_PLAYLISTS: {
            AutoPtr<IContentValues> values;
            CContentValues::New(initialValues, (IContentValues**)&values);
            AutoPtr<ISystem> isystem;
            CSystem::AcquireSingleton((ISystem**)&isystem);
            Int64 timeMillis;
            values->Put(IMediaStoreMediaColumns::DATE_ADDED,  (isystem->GetCurrentTimeMillis(&timeMillis), timeMillis) / 1000);
            InsertFile(dbh, uri, values,
                    IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST, TRUE, notifyRowIds, &rowId);
            if (rowId > 0) {
                AutoPtr<IMediaStoreAudioPlaylists> msap;
                CMediaStoreAudioPlaylists::AcquireSingleton((IMediaStoreAudioPlaylists**)&msap);
                AutoPtr<IUri> iiuri;
                msap->GetContentUri(volumeName, (IUri**)&iiuri);
                icu->WithAppendedId(iiuri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert audio playlist %s, initValues: %s", TO_CSTR(uri), TO_CSTR(values));
                }
            }
            break;
        }

        case AUDIO_PLAYLISTS_ID:
        case AUDIO_PLAYLISTS_ID_MEMBERS: {
            list->Get(3, (IInterface**)&obj);
            Int64 playlistId = StringUtils::ParseInt64(TO_STR(obj));
            AutoPtr<IContentValues> values;
            CContentValues::New(initialValues, (IContentValues**)&values);
            values->Put(IMediaStoreAudioPlaylistsMembers::PLAYLIST_ID, playlistId);
            dbh->mNumInserts++;
            db->Insert(String("audio_playlists_map"), String("playlist_id"), values, &rowId);
            if (rowId > 0) {
                icu->WithAppendedId(uri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert %s, initValues: %s", "audio_playlists_map", TO_CSTR(values));
                }
            }
            break;
        }

        case VIDEO_MEDIA: {
            InsertFile(dbh, uri, initialValues,
                IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO, TRUE, notifyRowIds, &rowId);
            if (rowId > 0) {
                MediaDocumentsProvider::OnMediaStoreInsert(
                        context, volumeName, IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO, rowId);
                AutoPtr<IUri> iiuri;
                AutoPtr<IMediaStoreVideoMedia> msvm;
                CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msvm);
                msvm->GetContentUri(volumeName, (IUri**)&iiuri);
                icu->WithAppendedId(iiuri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert video %s, initValues: %s", TO_CSTR(uri), TO_CSTR(initialValues));
                }
            }
            break;
        }

        case AUDIO_ALBUMART: {
            if (dbh->mInternal) {
                Logger::E(TAG, "no internal album art allowed");
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
               // throw new UnsupportedOperationException("no internal album art allowed");
            }
            AutoPtr<IContentValues> values;
            ECode ec = EnsureFile(FALSE, initialValues, String(""), ALBUM_THUMB_FOLDER, (IContentValues**)&values);
            if(ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
                // probably no more room to store albumthumbs
                values = initialValues;
            }
            dbh->mNumInserts++;
            db->Insert(String("album_art"), IMediaStoreMediaColumns::DATA, values, &rowId);
            if (rowId > 0) {
               icu->WithAppendedId(uri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert %s, initValues: %s", "album_art", TO_CSTR(values));
                }
            }
            break;
        }

        case VOLUMES:
        {
           String name;
           initialValues->GetAsString(String("name"), &name);
           AutoPtr<IUri> attachedVolume = AttachVolume(name);
           if (!mMediaScannerVolume.IsNull() && mMediaScannerVolume.Equals(name)) {
               AutoPtr<IDatabaseHelper> dbhelper;
               GetDatabaseForUri(attachedVolume, (IDatabaseHelper**)&dbhelper);
               if (dbhelper == NULL) {
                   Logger::E(TAG, "no database for attached volume %s", TO_CSTR(attachedVolume));
               }
               else {
                   ((DatabaseHelper*)dbhelper.Get())->mScanStartTime = SystemClock::GetCurrentTimeMicro();
               }
           }
           *result = attachedVolume;
           REFCOUNT_ADD(*result)
           return NOERROR;
        }

        case MTP_CONNECTED:
            {
                AutoLock syncLock(mMtpServiceConnection);
                if (mMtpService == NULL) {
                    AutoPtr<IContext> context;
                    GetContext((IContext**)&context);
                    // MTP is connected, so grab a connection to MtpService
                    AutoPtr<IIntent> obj;
                    CIntent::New(context, ECLSID_CMtpService, (IIntent**)&obj);
                    context->BindService(obj, mMtpServiceConnection, IContext::BIND_AUTO_CREATE, &flag);
                }
            }
            break;

        case FILES:
            InsertFile(dbh, uri, initialValues,
                   IMediaStoreFilesFileColumns::MEDIA_TYPE_NONE, TRUE, notifyRowIds, &rowId);
            if (rowId > 0) {
                AutoPtr<IMediaStoreFiles> msf;
                CMediaStoreFiles::AcquireSingleton((IMediaStoreFiles**)&msf);
                msf->GetContentUri(volumeName, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert files %s, initValues: %s", TO_CSTR(uri), TO_CSTR(initialValues));
                }
            }
            break;

        case MTP_OBJECTS:
            // We don't send a notification if the insert originated from MTP
            InsertFile(dbh, uri, initialValues,
                   IMediaStoreFilesFileColumns::MEDIA_TYPE_NONE, FALSE, notifyRowIds, &rowId);
            if (rowId > 0) {
                AutoPtr<IMediaStoreFiles> msf;
                CMediaStoreFiles::AcquireSingleton((IMediaStoreFiles**)&msf);
                msf->GetMtpObjectsUri(volumeName, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert objects %s, initValues: %s", TO_CSTR(uri), TO_CSTR(initialValues));
                }
            }
            break;

        case MEDIA_BOOKMARK: {
            db->Insert(String("bookmarks"), String("mime_type"), initialValues, &rowId);
            if (rowId > 0) {
               icu->WithAppendedId(uri, rowId, (IUri**)&newUri);
            }
            else {
                if (LOCAL_LOGV) {
                    Logger::W(TAG, "Failed to insert bookmarks %s, initValues: %s", "bookmarks", TO_CSTR(initialValues));
                }
            }
            break;
        }

        default:
           Logger::E(TAG, "Invalid URI %s", TO_CSTR(uri));
           return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    if (!path.IsNull() && path.ToLowerCase(/*Locale.US*/).EndWith("/.nomedia")) {
       // need to set the media_type of all the files below this folder to 0
       ProcessNewNoMediaPath(dbh, db, path);
    }
    *result = newUri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void MediaProvider::ProcessNewNoMediaPath(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& path)
{
    AutoPtr<IFile> nomedia;
    CFile::New(path, (IFile**)&nomedia);
    Boolean flag = FALSE;
    if (nomedia->Exists(&flag), flag) {
       HidePath(helper, db, path);
    }
    else {
       // File doesn't exist. Try again in a little while.
       // XXX there's probably a better way of doing this
       AutoPtr<MyThread> myThread = new MyThread(this, nomedia, helper, db, path);
       myThread->Start();
    }
}

void MediaProvider::HidePath(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& path)
{
    // a new nomedia path was added, so clear the media paths
    AutoPtr<IMediaScannerHelper> msh;
    CMediaScannerHelper::AcquireSingleton((IMediaScannerHelper**)&msh);
    msh->ClearMediaPathCache(TRUE /* media */, FALSE /* nomedia */);
    AutoPtr<IFile> nomedia;
    CFile::New(path, (IFile**)&nomedia);
    Boolean flag = FALSE;
    nomedia->IsDirectory(&flag);
    String parent;
    nomedia->GetParent(&parent);
    String hiddenroot = flag ? path : parent;
    AutoPtr<IContentValues> mediatype;
    CContentValues::New((IContentValues**)&mediatype);
    mediatype->Put(String("media_type"), 0);
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = hiddenroot  + "/", hiddenroot + "0";
    Int32 numrows;
    db->Update(String("files"), mediatype,
           String("_data >= ? AND _data < ?"),
           arr, &numrows);
    helper->mNumUpdates += numrows;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> res;
    context->GetContentResolver((IContentResolver**)&res);
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    uh->Parse(String("content://media/"), (IUri**)&uri);
    res->NotifyChange(uri, NULL);
}

void MediaProvider::ProcessRemovedNoMediaPath(
    /* [in] */ const String& path)
{
    AutoPtr<IMediaScannerHelper> msh;
    CMediaScannerHelper::AcquireSingleton((IMediaScannerHelper**)&msh);
    msh->ClearMediaPathCache(FALSE /* media */, TRUE /* nomedia */);
    // a nomedia path was removed, so clear the nomedia paths
    AutoPtr<IDatabaseHelper> helper;
    AutoPtr<IMediaStoreAudioMedia> msa;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msa);
    AutoPtr<IUri> uri;
    if (path.StartWith((*mExternalStoragePaths)[0])) {
      msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
      GetDatabaseForUri(uri, (IDatabaseHelper**)&helper);
    } else {
       msa->GetINTERNAL_CONTENT_URI((IUri**)&uri);
      GetDatabaseForUri(uri, (IDatabaseHelper**)&helper);
    }
    AutoPtr<ISQLiteDatabase> db;
    ((DatabaseHelper*)helper.Get())->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ScannerClient> sc = new ScannerClient();
    sc->constructor(context, db, path);
}

String MediaProvider::GenerateFileName(
    /* [in] */ Boolean internal,
    /* [in] */ const String& preferredExtension,
    /* [in] */ const String& directoryName)
{
    // create a random file
    AutoPtr<ISystem> isystem;
    CSystem::AcquireSingleton((ISystem**)&isystem);
    Int64 timeMillis;
    isystem->GetCurrentTimeMillis(&timeMillis);
    String name = StringUtils::ToString(timeMillis);//

    if (internal) {
       Logger::E(TAG, "Writing to internal storage is not supported.");
       // return E_UNSUPPORTED_OPERATION_EXCEPTION;
       return String(NULL);
       // throw new UnsupportedOperationException("Writing to internal storage is not supported.");
          // return Environment.getDataDirectory()
          //     + "/" + directoryName + "/" + name + preferredExtension;
       } else {
           return (*mExternalStoragePaths)[0] + "/" + directoryName + "/" + name + preferredExtension;
       }
}

Boolean MediaProvider::EnsureFileExists(
    /* [in] */ IUri* uri,
    /* [in] */ const String& path)
{
    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    Boolean flag = FALSE;
    file->Exists(&flag);
    if (flag) {
       return TRUE;
    }

    ECode ec = CheckAccess(uri, file,
        IParcelFileDescriptor::MODE_READ_WRITE | IParcelFileDescriptor::MODE_CREATE);
    if (FAILED(ec)) {
       return FALSE;
    }

    // we will not attempt to create the first directory in the path
    // (for example, do not create /sdcard if the SD card is not mounted)
    Int32 secondSlash = path.IndexOf('/', 1);
    if (secondSlash < 1) return FALSE;
    String directoryPath = path.Substring(0, secondSlash);
    AutoPtr<IFile> directory;
    CFile::New(directoryPath, (IFile**)&directory);
    directory->Exists(&flag);
    if (!flag) return FALSE;
    AutoPtr<IFile> parentFile;
    file->GetParentFile((IFile**)&parentFile);
    parentFile->Mkdirs(&flag);
    ec = file->CreateNewFile(&flag);
    if (FAILED(ec)) {
        Logger::E(TAG, "File creation failed. %s", path.string());
        return FALSE;
    }
    return flag;
}

void MediaProvider::GetTableAndWhere(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 match,
    /* [in] */ const String& userWhere,
    /* [in] */ GetTableAndWhereOutParameter* out)
{
    StringBuilder where;
    AutoPtr<IList> list;
    uri->GetPathSegments((IList**)&list);

    switch (match) {
        case IMAGES_MEDIA:
            out->mTable = "files";
            where += IMediaStoreFilesFileColumns::MEDIA_TYPE ;
            where += "=";
            where += IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE;
            break;

        case IMAGES_MEDIA_ID: {
            out->mTable = "files";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "_id=";
            where += TO_CSTR(obj);
            break;
        }

       case IMAGES_THUMBNAILS_ID:{
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "_id=";
            where += TO_CSTR(obj);
            break;
        }

       case IMAGES_THUMBNAILS:
           out->mTable = "thumbnails";
           break;

       case AUDIO_MEDIA:
           out->mTable = "files";
           where += IMediaStoreFilesFileColumns::MEDIA_TYPE ;
           where += "=";
           where += IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO;
           break;

       case AUDIO_MEDIA_ID: {
            out->mTable = "files";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "_id=";
            where += TO_CSTR(obj);
            break;
        }

       case AUDIO_MEDIA_ID_GENRES: {
            out->mTable = "audio_genres";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "audio_id=";
            where += TO_CSTR(obj);
            break;
        }

       case AUDIO_MEDIA_ID_GENRES_ID: {
            out->mTable = "audio_genres";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "audio_id=";
            where += TO_CSTR(obj);

            obj = NULL;
            list->Get(5, (IInterface**)&obj);
            where += " AND genre_id=";
            where += TO_CSTR(obj);
            break;
        }

       case AUDIO_MEDIA_ID_PLAYLISTS: {
            out->mTable = "audio_playlists";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "audio_id=";
            where += TO_CSTR(obj);
            break;
        }

       case AUDIO_MEDIA_ID_PLAYLISTS_ID: {
            out->mTable = "audio_playlists";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "audio_id=";
            where += TO_CSTR(obj);

            obj = NULL;
            list->Get(5, (IInterface**)&obj);
            where += " AND playlists_id=";
            where += TO_CSTR(obj);
            break;
        }

       case AUDIO_GENRES:
           out->mTable = String("audio_genres");
           break;

       case AUDIO_GENRES_ID: {
            out->mTable = "audio_genres";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "_id=";
            where += TO_CSTR(obj);
            break;
        }

       case AUDIO_GENRES_ID_MEMBERS: {
            out->mTable = "audio_genres";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "genre_id=";
            where += TO_CSTR(obj);
            break;
        }

       case AUDIO_PLAYLISTS:
           out->mTable = "files";
           where += IMediaStoreFilesFileColumns::MEDIA_TYPE ;
           where += "=";
           where += IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST;
           break;

       case AUDIO_PLAYLISTS_ID: {
            out->mTable = "files";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "_id=";
            where += TO_CSTR(obj);
            break;
        }

       case AUDIO_PLAYLISTS_ID_MEMBERS: {
            out->mTable = "audio_playlists_map";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "playlist_id=";
            where += TO_CSTR(obj);
            break;
        }

       case AUDIO_PLAYLISTS_ID_MEMBERS_ID: {
            out->mTable = "audio_playlists_map";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "playlist_id=";
            where += TO_CSTR(obj);

            obj = NULL;
            list->Get(5, (IInterface**)&obj);
            where += " AND _id=";
            where += TO_CSTR(obj);

            break;
        }

       case AUDIO_ALBUMART_ID: {
            out->mTable = "album_art";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "album_id=";
            where += TO_CSTR(obj);
            break;
        }

       case VIDEO_MEDIA:
           out->mTable = "files";
           where += IMediaStoreFilesFileColumns::MEDIA_TYPE ;
           where += "=";
           where += IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO;
           break;

       case VIDEO_MEDIA_ID: {
            out->mTable = "files";
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "_id=";
            where += TO_CSTR(obj);
            break;
        }

       case VIDEO_THUMBNAILS_ID: {
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            where += "_id=";
            where += TO_CSTR(obj);

            // fall through
        }
       case VIDEO_THUMBNAILS:
           out->mTable = "videothumbnails";
           break;

       case FILES_ID:
       case MTP_OBJECTS_ID: {
            AutoPtr<IInterface> obj;
            list->Get(2, (IInterface**)&obj);
            where += "_id=";
            where += TO_CSTR(obj);

            // fall through
        }
       case FILES:
       case MTP_OBJECTS:
           out->mTable = "files";
           break;

        case MEDIA_BOOKMARK_ID: {
            AutoPtr<IInterface> obj;
            list->Get(2, (IInterface**)&obj);
            where += "_id=";
            where += TO_CSTR(obj);

            // fall through
        }
        case MEDIA_BOOKMARK:
            out->mTable = "bookmarks";
            break;

       default:
           Logger::E(TAG, "Unknown or unsupported URL: %s", TO_CSTR(uri));
           break;
    }

    // Add in the user requested WHERE clause, if needed//

    if (!TextUtils::IsEmpty(userWhere)) {
       if (!TextUtils::IsEmpty(where.ToString())) {
            StringBuilder sb(where.ToString());
            sb += " AND (";
            sb += userWhere;
            sb += ")";
            out->mWhere = sb.ToString();
        } else {
            out->mWhere = userWhere;
        }
    }
    else {
        out->mWhere = where.ToString();
    }
}

ECode MediaProvider::Call(
    /* [in] */ const String& method,
    /* [in] */ const String& arg,
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    if (IMediaStore::UNHIDE_CALL.Equals(method)) {
       ProcessRemovedNoMediaPath(arg);
       *result = NULL;
       return NOERROR;
    }
    Logger::E(TAG, "Unsupported call: %s", method.string());
    return E_UNSUPPORTED_OPERATION_EXCEPTION;

}

ECode MediaProvider::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ const String& userWhere,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    AutoPtr<IUri> oUri;
    oUri = SafeUncanonicalize(uri);
    uri = oUri;
    Int32 count;
    Logger::V(TAG, "update for uri=%p, initValues=", uri, initialValues);
    Int32 match;
    GetURI_MATCHER()->Match(uri, &match);
    AutoPtr<IDatabaseHelper> helper;
    GetDatabaseForUri(uri, (IDatabaseHelper**)&helper);
    if (helper == NULL) {
       Logger::E(TAG, "Unknown URI: %p", uri);
       return E_UNSUPPORTED_OPERATION_EXCEPTION;
       // throw new UnsupportedOperationException(
       //         "Unknown URI: " + uri);
    }
    DatabaseHelper* dbh = ((DatabaseHelper*)helper.Get());
    dbh->mNumUpdates++;

    AutoPtr<ISQLiteDatabase> db;
    dbh->GetWritableDatabase((ISQLiteDatabase**)&db);

    String genre;
    if (initialValues != NULL) {
       initialValues->GetAsString(IMediaStoreAudioAudioColumns::GENRE, &genre);
       initialValues->Remove(IMediaStoreAudioAudioColumns::GENRE);
    }

    Boolean flag = FALSE;
    {
        AutoLock syncLock(sGetTableAndWhereParam);
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        GetTableAndWhere(uri, match, userWhere, sGetTableAndWhereParam);//

        // special case renaming directories via MTP.
        // in this case we must update all paths in the database with
        // the directory name as a prefix
        Int32 size;
        initialValues->GetSize(&size);
        if ((match == MTP_OBJECTS || match == MTP_OBJECTS_ID)
               && initialValues != NULL && size == 1) {
           String oldPath;
           String newPath;
           initialValues->GetAsString(IMediaStoreMediaColumns::DATA, &newPath);
           mDirectoryCache->Erase(newPath);
           // MtpDatabase will rename the directory first, so we test the new file name
           AutoPtr<IFile> f;
           CFile::New(newPath, (IFile**)&f);
           f->IsDirectory(&flag);
           if (!newPath.IsNull() && flag) {
               dbh->mNumQueries++;
               AutoPtr<ICursor> cursor;
               db->Query(sGetTableAndWhereParam->mTable, PATH_PROJECTION,
                   userWhere, whereArgs, String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
               if (cursor != NULL && (cursor->MoveToNext(&flag), flag)) {
                   cursor->GetString(1, &oldPath);
               }
                if (cursor) ioUtils->CloseQuietly(ICloseable::Probe(cursor));

               if (!oldPath.IsNull()) {
                   mDirectoryCache->Erase(oldPath);
                   // first rename the row for the directory
                   dbh->mNumUpdates++;
                   db->Update(sGetTableAndWhereParam->mTable, initialValues,
                           sGetTableAndWhereParam->mWhere, whereArgs, &count);
                   String name;
                   if (count > 0) {
                       // update the paths of any files and folders contained in the directory
                       AutoPtr<ArrayOf<IInterface*> > bindArgs = ArrayOf<IInterface*>::Alloc(6);
                       AutoPtr<ArrayOf<ICharSequence*> > _bind = ArrayOf<ICharSequence*>::Alloc(6);
                       (*_bind)[0] = CoreUtils::Convert(newPath);
                       Int32 oldLen = oldPath.GetLength();
                       (*_bind)[1] = CoreUtils::Convert(StringUtils::ToString(oldLen + 1));
                       (*_bind)[2] = CoreUtils::Convert(oldPath + "/");
                       (*_bind)[3] = CoreUtils::Convert(oldPath + "0");
                       f->GetName(&name);
                       (*_bind)[4] = CoreUtils::Convert(name);
                       f->ToString(&name);
                       (*_bind)[5] = CoreUtils::Convert(StringUtils::ToString(name.ToLowerCase().GetHashCode()));

                       dbh->mNumUpdates++;
                       Int32 bLen = _bind->GetLength();
                       for(Int32 i = 0; i < bLen; i++) {
                           bindArgs->Set(i, TO_IINTERFACE((*_bind)[i]));
                       }
                       db->ExecSQL(String("UPDATE files SET _data=?1||SUBSTR(_data, ?2)") +
                               // also update bucket_display_name
                               ",bucket_display_name=?5" +
                               ",bucket_id=?6" +
                               " WHERE _data >= ?3 AND _data < ?4;",
                               bindArgs);
                   }//

                   if (count > 0 && !(db->InTransaction(&flag), flag)) {
                       AutoPtr<IContext> context;
                       GetContext((IContext**)&context);
                       AutoPtr<IContentResolver> resolver;
                       context->GetContentResolver((IContentResolver**)&resolver);
                       resolver->NotifyChange(uri, NULL);
                   }
                   f->GetName(&name);
                   if (name.StartWith(".")) {
                       // the new directory name is hidden
                       ProcessNewNoMediaPath(dbh, db, newPath);
                   }
                   *result = count;
                   return NOERROR;
               }
           } else if (newPath.ToLowerCase(/*Locale.US*/).EndWith("/.nomedia")) {
               ProcessNewNoMediaPath(dbh, db, newPath);
           }
       }//

       switch (match) {
           case AUDIO_MEDIA:
           case AUDIO_MEDIA_ID:
               {
                   AutoPtr<IContentValues> values;
                   CContentValues::New(initialValues, (IContentValues**)&values);
                   String albumartist;
                   values->GetAsString(IMediaStoreAudioAudioColumns::ALBUM_ARTIST, &albumartist);
                   String compilation;
                   values->GetAsString(IMediaStoreAudioAudioColumns::COMPILATION, &compilation);
                   values->Remove(IMediaStoreAudioAudioColumns::COMPILATION);//

                   // Insert the artist into the artist table and remove it from
                   // the input values
                   String artist;
                   values->GetAsString(artist, &artist);
                   values->Remove(artist);
                   if (!artist.IsNull()) {
                        Int64 artistRowId;
                        {
                            AutoLock syncLock(dbh->mArtistCacheLock);
                            HashMap<String, Int64> artistCache = dbh->mArtistCache;
                            Int64 temp = artistCache[artist];
                            AutoPtr<IInteger64> it = CoreUtils::Convert(temp);
                            if (it == NULL) {
                                artistRowId = GetKeyIdForName(dbh, db,
                                       String("artists"), String("artist_key"), artist,
                                       artist, artist, String(NULL), 0, String(NULL), artistCache, uri);
                            } else {
                                artistRowId = temp;
                            }
                        }
                        values->Put(String("artist_id"), StringUtils::ToString(artistRowId));
                   }//

                   // Do the same for the album field.
                   String so;
                   values->GetAsString(String("album"), &so);
                   values->Remove(String("album"));
                   if (!so.IsNull()) {
                       String path;
                       values->GetAsString(IMediaStoreMediaColumns::DATA, &path);
                       Int32 albumHash = 0;
                       if (!albumartist.IsNull()) {
                           albumHash = albumartist.GetHashCode();
                       } else if (!compilation.IsNull() && compilation.Equals("1")) {
                           // nothing to do, hash already set
                       } else {
                           if (path.IsNull()) {
                               if (match == AUDIO_MEDIA) {
                                   Logger::W(TAG, "Possible multi row album name update without path could give wrong album key");
                               } else {
                                   //Log.w(TAG, "Specify path to avoid extra query");
                                   AutoPtr<ICursor> c;
                                   AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
                                   (*arr)[0] = IMediaStoreMediaColumns::DATA;
                                   Query(uri, arr,
                                           String(NULL), NULL, String(NULL), (ICursor**)&c);
                                   if (c != NULL) {
                                       // try {
                                           Int32 numrows;
                                           c->GetCount(&numrows);
                                           if (numrows == 1) {
                                               c->MoveToFirst(&flag);
                                               c->GetString(0, &path);
                                           } else {
                                               Logger::E(TAG, "%d rows for ", numrows, uri);
                                           }
                                       // } finally {
                                           ioUtils->CloseQuietly(ICloseable::Probe(c));
                                       // }
                                   }
                               }
                           }
                           if (!path.IsNull()) {//

                               albumHash = path.Substring(0, path.LastIndexOf('/')).GetHashCode();
                           }
                       }//

                        String s = so;
                        Int64 albumRowId;
                        {
                            AutoLock syncLock(dbh->mAlbumCacheLock);
                            HashMap<String, Int64> albumCache = dbh->mAlbumCache;
                            StringBuilder sb(s); sb += albumHash;
                            String cacheName = sb.ToString();
                            Int64 temp = albumCache[cacheName];
                            AutoPtr<IInteger64> ig = CoreUtils::Convert(temp);
                            if (ig == NULL) {
                                albumRowId = GetKeyIdForName(dbh, db,
                                       String("albums"), String("album_key"), String("album"),
                                       s, cacheName, path, albumHash, artist, albumCache, uri);
                            } else {
                                albumRowId = temp;
                            }
                        }
                       values->Put(String("album_id"), StringUtils::ToString(albumRowId));
                   }//

                   // don't allow the title_key field to be updated directly
                   values->Remove(String("title_key"));
                   // If the title field is modified, update the title_key
                   values->GetAsString(String("title"), &so);
                   if (!so.IsNull()) {
                       String s = so;
                       AutoPtr<IMediaStoreAudio> msa;
                       CMediaStoreAudio::AcquireSingleton((IMediaStoreAudio**)&msa);
                       String str;
                       msa->KeyFor(s, &str);
                       values->Put(String("title_key"), str);
                       // do a final trim of the title, in case it started with the special
                       // "sort first" character (ascii \001)
                       values->Remove(String("title"));
                       values->Put(String("title"), s.Trim());
                   }//

                   dbh->mNumUpdates++;
                   db->Update(sGetTableAndWhereParam->mTable, values,
                           sGetTableAndWhereParam->mWhere, whereArgs, &count);
                   if (!genre.IsNull()) {
                       if (count == 1 && match == AUDIO_MEDIA_ID) {
                           AutoPtr<IList> list;
                           uri->GetPathSegments((IList**)&list);
                           AutoPtr<IInterface> obj;
                           list->Get(3, (IInterface**)&obj);
                           Int64 rowId = StringUtils::ParseInt64(TO_STR(obj));
                           UpdateGenre(rowId, genre);
                       } else {
                           // can't handle genres for bulk update or for non-audio files
                           Logger::W(TAG, "ignoring genre in update: count = %d match = ",
                               count, match);
                       }
                   }
               }
               break;
           case IMAGES_MEDIA:
           case IMAGES_MEDIA_ID:
           case VIDEO_MEDIA:
           case VIDEO_MEDIA_ID:
               {
                   AutoPtr<IContentValues> values;
                   CContentValues::New(initialValues, (IContentValues**)&values);
                   // Don't allow bucket id or display name to be updated directly.
                   // The same names are used for both images and table columns, so
                   // we use the ImageColumns constants here.
                   values->Remove(IMediaStoreImagesImageColumns::BUCKET_ID);
                   values->Remove(IMediaStoreImagesImageColumns::BUCKET_DISPLAY_NAME);
                   // If the data is being modified update the bucket values
                   String data;
                   values->GetAsString(IMediaStoreMediaColumns::DATA, &data);
                   if (!data.IsNull()) {
                       ComputeBucketValues(data, values);
                   }
                   ComputeTakenTime(values);
                   dbh->mNumUpdates++;
                   db->Update(sGetTableAndWhereParam->mTable, values,
                           sGetTableAndWhereParam->mWhere, whereArgs, &count);
                   // if this is a request from MediaScanner, DATA should contains file path
                   // we only process update request from media scanner, otherwise the requests
                   // could be duplicate.
                   String as;
                   values->GetAsString(IMediaStoreMediaColumns::DATA, &as);
                   if (count > 0 && !as.IsNull()) {
                       dbh->mNumQueries++;
                       AutoPtr<ICursor> c;
                       db->Query(sGetTableAndWhereParam->mTable,
                               READY_FLAG_PROJECTION, sGetTableAndWhereParam->mWhere,
                               whereArgs, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
                       if (c != NULL) {
                           // try {
                               while ((c->MoveToNext(&flag), flag)) {
                                   Int64 magic;
                                   c->GetInt64(2, &magic);
                                   if (magic == 0) {
                                       String gs;
                                       c->GetString(1, &gs);
                                       RequestMediaThumbnail(gs, uri,
                                               MediaThumbRequest::PRIORITY_NORMAL, 0);
                                   }
                               }
                           // } finally {
                               ioUtils->CloseQuietly(ICloseable::Probe(c));
                           // }
                       }
                   }
               }
               break;

           case AUDIO_PLAYLISTS_ID_MEMBERS_ID:
              {
                  String moveit;
                  uri->GetQueryParameter(String("move"), &moveit);
                  if (!moveit.IsNull()) {
                      String key = IMediaStoreAudioPlaylistsMembers::PLAY_ORDER;
                      if ((initialValues->ContainsKey(key, &flag), flag)) {
                          Int32 newpos;
                          AutoPtr<IInteger32> newposObj;
                          initialValues->GetAsInteger32(key, (IInteger32**)&newposObj);
                          newposObj->GetValue(&newpos);
                          AutoPtr<IList> segments;
                          uri->GetPathSegments((IList**)&segments);
                          AutoPtr<IInterface> obj;
                          segments->Get(3, (IInterface**)&obj);
                          Int64 playlist = StringUtils::ParseInt64(TO_STR(obj));
                          obj = NULL;
                          segments->Get(5, (IInterface**)&obj);
                          Int32 oldpos = StringUtils::ParseInt64(TO_STR(obj));
                          *result = MovePlaylistEntry(dbh, db, playlist, oldpos, newpos);
                          return NOERROR;
                    }
                   Logger::E(TAG, "Need to specify %s when using 'move' parameter", key.string());
                   return E_ILLEGAL_ARGUMENT_EXCEPTION;
                   // throw new IllegalArgumentException("Need to specify " + key +
                   //         " when using 'move' parameter");
               }
               // fall through
              }

           default:
              {
                  dbh->mNumUpdates++;
                  db->Update(sGetTableAndWhereParam->mTable, initialValues,
                     sGetTableAndWhereParam->mWhere, whereArgs, &count);
                  break;
              }

        }
    }//synchronized

    // in a transaction, the code that began the transaction should be taking
    // care of notifications once it ends the transaction successfully
    if (count > 0 && !(db->InTransaction(&flag), flag)) {
       AutoPtr<IContext> context;
       GetContext((IContext**)&context);
       AutoPtr<IContentResolver> resolver;
       context->GetContentResolver((IContentResolver**)&resolver);
       resolver->NotifyChange(uri, NULL);
    }
    *result = count;
    return NOERROR;
}

Int32 MediaProvider::MovePlaylistEntry(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 playlist,
    /* [in] */ Int32 from,
    /* [in] */ Int32 to)
{
    if (from == to) {
       return 0;
    }
    String nullStr;
    db->BeginTransaction();
    Int32 numlines = 0;
    AutoPtr<ICursor> c;
    // try {
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    helper->mNumUpdates += 3;
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = String("play_order");//

    StringBuilder sb(",1"); sb += from;
    AutoPtr<ArrayOf<String> > plArray = ArrayOf<String>::Alloc(1);
    (*plArray)[0] = StringUtils::ToString(playlist);
    db->Query(String("audio_playlists_map"), arr,
           String("playlist_id=?"), plArray, nullStr, nullStr, String("play_order"),
           sb.ToString(), (ICursor**)&c);
    Boolean flag = FALSE;
    c->MoveToFirst(&flag);
    Int32 from_play_order;
    c->GetInt32(0, &from_play_order);
    ioUtils->CloseQuietly(ICloseable::Probe(c));

    sb.Reset(); sb += ",1"; sb += to;
    db->Query(String("audio_playlists_map"), arr,
           String("playlist_id=?"), plArray, nullStr, nullStr, String("play_order"),
           sb.ToString(), (ICursor**)&c);
    c->MoveToFirst(&flag);
    Int32 to_play_order;
    c->GetInt32(0, &to_play_order);

    sb.Reset();
    sb += "UPDATE audio_playlists_map SET play_order=-1"
        " WHERE play_order=";
    sb += from_play_order; sb += "AND playlist_id="; sb += playlist;

    db->ExecSQL(sb.ToString());

    // We could just run both of the next two statements, but only one of
    // of them will actually do anything, so might as well skip the compile
    // and execute steps.
    if (from  < to) {
        sb.Reset();
        sb += "UPDATE audio_playlists_map SET play_order=play_order-1"
                " WHERE play_order<=";
        sb += to_play_order; sb += "AND play_order>"; sb += from_play_order;
        sb += "AND playlist_id="; sb += playlist;
        db->ExecSQL(sb.ToString());
        numlines = to - from + 1;
    }
    else {
        sb.Reset();
        sb += "UPDATE audio_playlists_map SET play_order=play_order+1"
                " WHERE play_order<=";
        sb += to_play_order; sb += "AND play_order>"; sb += from_play_order;
        sb += "AND playlist_id="; sb += playlist;
        db->ExecSQL(sb.ToString());
        numlines = from - to + 1;
    }

    sb.Reset();
    sb += "UPDATE audio_playlists_map SET play_order="; sb += to_play_order;
    sb += "WHEREplay_order=-1 AND playlist_id="; sb += playlist;
    db->ExecSQL(sb.ToString());
    db->SetTransactionSuccessful();
    // } finally {
    db->EndTransaction();
    ioUtils->CloseQuietly(ICloseable::Probe(c));
    // }//

    AutoPtr<IMediaStoreAudioPlaylists> msal;
    CMediaStoreAudioPlaylists::AcquireSingleton((IMediaStoreAudioPlaylists**)&msal);
    AutoPtr<IUri> uuri;
    msal->GetEXTERNAL_CONTENT_URI((IUri**)&uuri);
    AutoPtr<IUriBuilder> ub;
    uuri->BuildUpon((IUriBuilder**)&ub);
    ub->AppendEncodedPath(StringUtils::ToString(playlist));
    AutoPtr<IUri> uri;
    ub->Build((IUri**)&uri);
    // notifyChange() must be called after the database transaction is ended
    // or the listeners will read the old data in the callback
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->NotifyChange(uri, NULL);
    return numlines;
}

ECode MediaProvider::OpenFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    uri = SafeUncanonicalize(uri);
    AutoPtr<IParcelFileDescriptor> pfd;
    ECode ec = NOERROR;
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);//

    Int32 match;
    Boolean flag = FALSE;
    String nullStr;
    if ((GetURI_MATCHER()->Match(uri, &match), match == AUDIO_ALBUMART_FILE_ID)) {
        // get album art for the specified media file
        AutoPtr<IDatabaseHelper> database;
        GetDatabaseForUri(uri, (IDatabaseHelper**)&database);
        if (database == NULL) {
            Logger::E(TAG, "Couldn't open database for %s", TO_CSTR(uri));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        AutoPtr<ISQLiteDatabase> db;
        ((DatabaseHelper*)(database.Get()))->GetReadableDatabase((ISQLiteDatabase**)&db);//

        if (db == NULL) {
            Logger::E(TAG, "Couldn't open database for %s", TO_CSTR(uri));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        AutoPtr<ISQLiteQueryBuilder> qb;
        CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
        AutoPtr<IList> list;
        uri->GetPathSegments((IList**)&list);
        AutoPtr<IInterface> obj;
        list->Get(3, (IInterface**)&obj);
        Int32 songid = StringUtils::ParseInt32(TO_STR(obj));
        qb->SetTables(String("audio_meta"));
        StringBuilder sb("_id="); sb += songid;
        qb->AppendWhere(CoreUtils::Convert(sb.ToString()));
        AutoPtr<ICursor> c;
        AutoPtr<ArrayOf<String> > arr1 = ArrayOf<String>::Alloc(2);
        (*arr1)[0] = IMediaStoreMediaColumns::DATA;
        (*arr1)[1] = IMediaStoreAudioAlbumColumns::ALBUM_ID;
        qb->Query(db, arr1, nullStr, NULL, nullStr, nullStr, nullStr, (ICursor**)&c);
        // try {
        if ((c->MoveToFirst(&flag), flag)) {
            String audiopath;
            Int32 albumid;
            c->GetString(0, &audiopath);
            c->GetInt32(1, &albumid);
            // Try to get existing album art for this album first, which
            // could possibly have been obtained from a different file.
            // If that fails, try to get it from this specific file.
            AutoPtr<IUri> newUri;
            AutoPtr<IContentUris> cus;
            CContentUris::AcquireSingleton((IContentUris**)&cus);
            cus->WithAppendedId(ALBUMART_URI, albumid, (IUri**)&newUri);
            // try {
            ec = OpenFileAndEnforcePathPermissionsHelper(newUri, mode, (IParcelFileDescriptor**)&pfd);
            if (FAILED(ec)) {
               // That didn't work, now try to get it from the specific file
               pfd = GetThumb(((DatabaseHelper*)(database.Get())), db, audiopath, (Int64)albumid, NULL);
            }
        }

        *result = pfd;
        REFCOUNT_ADD(*result);
        ioUtils->CloseQuietly(ICloseable::Probe(c));
        return NOERROR;
    }

    ec = OpenFileAndEnforcePathPermissionsHelper(uri, mode, (IParcelFileDescriptor**)&pfd);
    if (FAILED(ec)) {
        if (mode.Contains("w")) {
            // if the file couldn't be created, we shouldn't extract album art
            return E_FILE_NOT_FOUND_EXCEPTION;
        }

        Int32 match;
        GetURI_MATCHER()->Match(uri, &match);
        if (match == AUDIO_ALBUMART_ID) {
            // Tried to open an album art file which does not exist. Regenerate.
            AutoPtr<IDatabaseHelper> database;
            GetDatabaseForUri(uri, (IDatabaseHelper**)&database);
            if (database == NULL) {
               return E_FILE_NOT_FOUND_EXCEPTION;
            }
            AutoPtr<ISQLiteDatabase> db;
            ((DatabaseHelper*)(database.Get()))->GetReadableDatabase((ISQLiteDatabase**)&db);
            if (db == NULL) {
                Logger::E(TAG, "Couldn't open database for %s", TO_CSTR(uri));
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            AutoPtr<ISQLiteQueryBuilder> qb;
            CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
            AutoPtr<IList> list;
            uri->GetPathSegments((IList**)&list);
            AutoPtr<IInterface> obj;
            list->Get(3, (IInterface**)&obj);
            Int32 albumid = StringUtils::ParseInt32(TO_STR(obj));
            qb->SetTables(String("audio_meta"));
            StringBuilder sb("album_id=");
            sb += albumid;
            qb->AppendWhere(CoreUtils::Convert(sb.ToString()));

            AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
            (*array)[0] = IMediaStoreMediaColumns::DATA;//
            AutoPtr<ICursor> c;
            qb->Query(db, array, String(NULL), NULL, String(NULL),
                String(NULL), IMediaStoreAudioAudioColumns::TRACK, (ICursor**)&c);
            if ((c->MoveToFirst(&flag), flag)) {
                String audiopath;
                c->GetString(0, &audiopath);
                pfd = GetThumb(((DatabaseHelper*)(database.Get())), db, audiopath, albumid, uri);
            }

            ioUtils->CloseQuietly(ICloseable::Probe(c));
        }
        if (pfd == NULL) {
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
    }

    *result = pfd;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IFile> MediaProvider::QueryForDataFile(
    /* [in] */ IUri* uri)
{
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = IMediaStoreMediaColumns::DATA;
    Query(uri, arr, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (cursor == NULL) {
       Logger::E(TAG, "Missing cursor for %s", TO_CSTR(uri));
       return NULL;
    }

    // try {
    Int32 count;
    cursor->GetCount(&count);
    AutoPtr<IFile> file;//

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    switch (count) {
       case 0: {
            Logger::E(TAG, "No entry for %s", TO_CSTR(uri));
            break;
          }

       case 1: {
              Boolean flag = FALSE;
              cursor->MoveToFirst(&flag);
              if (flag) {
                 String str;
                 cursor->GetString(0, &str);
                 CFile::New(str, (IFile**)&file);
                 break;
              }
              else {
                 Logger::E(TAG, "Unable to read entry for %s", TO_CSTR(uri));
                 break;
              }
          }

       default: {
              Logger::E(TAG, "Multiple items at %s", TO_CSTR(uri));
          }
    }

    ioUtils->CloseQuietly(ICloseable::Probe(cursor));
    return file;
}

ECode MediaProvider::OpenFileAndEnforcePathPermissionsHelper(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IParcelFileDescriptorHelper> pfh;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfh);
    Int32 modeBits;
    pfh->ParseMode(mode, &modeBits);//

    AutoPtr<IFile> file = QueryForDataFile(uri);//

    FAIL_RETURN(CheckAccess(uri, file, modeBits))

    // Bypass emulation layer when file is opened for reading, but only
    // when opening read-only and we have an exact match.
    if (modeBits == IParcelFileDescriptor::MODE_READ_ONLY) {
        file = Environment::MaybeTranslateEmulatedPathToInternal(file);
    }

    return pfh->Open(file, modeBits, result);
}

void MediaProvider::DeleteIfAllowed(
    /* [in] */ IUri* uri,
    /* [in] */ const String& path)
{
    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    ECode ec = CheckAccess(uri, file, IParcelFileDescriptor::MODE_WRITE_ONLY);
    if (FAILED(ec)) {
        Logger::E(TAG, "Couldn't delete %s", path.string());
        return;
   }

   file->Delete();
}

ECode MediaProvider::CheckAccess(
    /* [in] */ IUri* uri,
    /* [in] */ IFile* file,
    /* [in] */ Int32 modeBits)
{
    if (file == NULL) {
        Logger::E(TAG, "MediaProvider::CheckAccess file should not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isWrite = (modeBits & IParcelFileDescriptor::MODE_WRITE_ONLY) != 0;
    String path;
    ECode ec = file->GetCanonicalPath(&path);
    if (FAILED(ec)) {
        Logger::E(TAG, "Unable to resolve canonical path for %s", TO_CSTR(file));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContext> c;
    GetContext((IContext**)&c);
    Boolean readGranted = FALSE;
    Boolean writeGranted = FALSE;
    Int32 vol;
    if (isWrite) {
        FAIL_RETURN(c->CheckCallingOrSelfUriPermission(uri, IIntent::FLAG_GRANT_WRITE_URI_PERMISSION, &vol))
        writeGranted = vol == IPackageManager::PERMISSION_GRANTED;
    }
    else {
        FAIL_RETURN(c->CheckCallingOrSelfUriPermission(uri, IIntent::FLAG_GRANT_READ_URI_PERMISSION, &vol))
        readGranted = vol == IPackageManager::PERMISSION_GRANTED;
    }

    if (path.StartWith(GetExternalPath()) || path.StartWith(GetLegacyPath())) {
        if (isWrite) {
           if (!writeGranted) {
               FAIL_RETURN(c->EnforceCallingOrSelfPermission(
                   Manifest::permission::WRITE_EXTERNAL_STORAGE, String("External path: ") + path))
           }
        }
        else if (!readGranted) {
            FAIL_RETURN(c->EnforceCallingOrSelfPermission(
                Manifest::permission::READ_EXTERNAL_STORAGE, String("External path: ") + path))
        }
    }
    else if (path.StartWith(GetCachePath())) {
        if ((isWrite && !writeGranted) || !readGranted) {
            FAIL_RETURN(c->EnforceCallingOrSelfPermission(
                Manifest::permission::ACCESS_CACHE_FILESYSTEM, String("Cache path: ") + path))
        }
    }
    else if (IsSecondaryExternalPath(path)) {
       // read access is OK with the appropriate permission
        if (!readGranted) {
            c->CheckCallingOrSelfPermission(Manifest::permission::WRITE_MEDIA_STORAGE, &vol);
            if (vol == IPackageManager::PERMISSION_DENIED) {
                FAIL_RETURN(c->EnforceCallingOrSelfPermission(
                    Manifest::permission::READ_EXTERNAL_STORAGE, String("External path: ") + path))
           }
        }
        if (isWrite) {
            Int32 vol;
            c->CheckCallingOrSelfUriPermission(uri, IIntent::FLAG_GRANT_WRITE_URI_PERMISSION, &vol);
            if (vol != IPackageManager::PERMISSION_GRANTED) {
                FAIL_RETURN(c->EnforceCallingOrSelfPermission(
                    Manifest::permission::WRITE_MEDIA_STORAGE, String("External path: ") + path))
            }
        }
    }
    else if (isWrite) {
        // don't write to non-cache, non-sdcard files.
        Logger::E(TAG, "Can't access %s", TO_CSTR(file));
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    else {
        return CheckWorldReadAccess(path);
    }
    return NOERROR;
}

Boolean MediaProvider::IsSecondaryExternalPath(
    /* [in] */ const String& path)
{
    Int32 length = mExternalStoragePaths->GetLength();
    for (Int32 i = length - 1; i >= 0; --i) {
        if (path.StartWith((*mExternalStoragePaths)[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode MediaProvider::CheckWorldReadAccess(
    /* [in] */ const String& path)
{
    AutoPtr<IStructStat> stat;
    Elastos::Droid::System::Os::Stat(path, (IStructStat**)&stat);
    if (stat != NULL) {
        Int32 accessBits = OsConstants::_S_IROTH;
        Int32 mode;
        stat->GetMode(&mode);
        if (OsConstants::S_IsREG(mode) && ((mode & accessBits) == accessBits)) {
           return CheckLeadingPathComponentsWorldExecutable(path);
        }
    }

    // couldn't stat the file, either it doesn't exist or isn't
    // accessible to us
    Logger::E(TAG, "Can't access %s", path.string());
    return E_FILE_NOT_FOUND_EXCEPTION;
}

ECode MediaProvider::CheckLeadingPathComponentsWorldExecutable(
    /* [in] */ const String& filePath)
{
    AutoPtr<IFile> file;
    CFile::New(filePath, (IFile**)&file);
    AutoPtr<IFile> parent;
    FAIL_RETURN(file->GetParentFile((IFile**)&parent))

    Int32 accessBits = OsConstants::_S_IXOTH;

    Boolean flag = FALSE;
    AutoPtr<IFile> pf;
    while (parent != NULL) {
        FAIL_RETURN(parent->Exists(&flag))
        if (!flag) {
            Logger::E(TAG, "access denied %s", filePath.string());
            // parent dir doesn't exist, give up
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
       // try {
        AutoPtr<IStructStat> stat;
        String path;
        ECode ec = parent->GetPath(&path);
        if (FAILED(ec)) {
           Logger::E(TAG, "Can't access %s", filePath.string());
           return E_FILE_NOT_FOUND_EXCEPTION;
        }

        Elastos::Droid::System::Os::Stat(path, (IStructStat**)&stat);
        Int32 mode;
        ec = stat->GetMode(&mode);
        if (FAILED(ec)) {
           Logger::E(TAG, "Can't access %s", filePath.string());
           return E_FILE_NOT_FOUND_EXCEPTION;
        }
        if ((mode & accessBits) != accessBits) {
           Logger::E(TAG, "Can't access %s", filePath.string());
           return E_FILE_NOT_FOUND_EXCEPTION;
        }

        pf = NULL;
        parent->GetParentFile((IFile**)&pf);
        parent = pf;
    }
    return NOERROR;
}

void MediaProvider::MakeThumbAsync(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& path,
    /* [in] */ Int64 album_id)
{
    {
        AutoLock syncLock(mPendingThumbs);
        Boolean flag = FALSE;
        mPendingThumbs->Contains(CoreUtils::Convert(path), &flag);

        if(flag) {
            // There's already a request to make an album art thumbnail
            // for this audio file in the queue.
            return;
        }

        mPendingThumbs->Add(CoreUtils::Convert(path));
    }

    AutoPtr<ThumbData> d = new ThumbData();
    d->mHelper = helper;
    d->mDb = db;
    d->mPath = path;
    d->mAlbum_id = album_id;
    AutoPtr<IContentUris> icu;
    CContentUris::AcquireSingleton((IContentUris**)&icu);
    AutoPtr<IUri> uri;
    icu->WithAppendedId(mAlbumArtBaseUri, album_id, (IUri**)&uri);
    d->mAlbumart_uri = uri;//

    // Instead of processing thumbnail requests in the order they were
    // received we instead process them stack-based, i.e. LIFO.
    // The idea behind this is that the most recently requested thumbnails
    // are most likely the ones still in the user's view, whereas those
    // requested earlier may have already scrolled off.
    {
        AutoLock syncLock(mThumbRequestStack);
        mThumbRequestStack->Push((IThumbData*)(d));
    }

    // Trigger the handler.
    AutoPtr<IMessage> msg;
    mThumbHandler->ObtainMessage(ALBUM_THUMB, (IMessage**)&msg);
    msg->SendToTarget();
}

Boolean MediaProvider::IsRootStorageDir(
    /* [in] */ const String& artPath)
{
    Int32 length = mExternalStoragePaths->GetLength();
    for (Int32 i = 0; i < length; i++) {
        if (!(*mExternalStoragePaths)[i].IsNull() &&
               (artPath.EqualsIgnoreCase((*mExternalStoragePaths)[i])))
           return TRUE;
    }
    return FALSE;
}

void MediaProvider::LogToDb(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& message)
{
    AutoPtr<ICharSequence> cs = CoreUtils::Convert(message);
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
    arr->Set(0, cs.Get());
    db->ExecSQL(String("INSERT OR REPLACE"
        " INTO log (time,message) VALUES (strftime('%Y-%m-%d %H:%M:%f','now'),?);"),  arr);
    // delete all but the last 500 rows
    db->ExecSQL(String("DELETE FROM log WHERE rowid IN"
        " (SELECT rowid FROM log ORDER BY rowid DESC LIMIT 500,-1);"));
}

Boolean MediaProvider::WaitForThumbnailReady(
    /* [in] */ IUri* origUri)
{
    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreMediaColumns::DATA;
    (*arr)[2] = IMediaStoreImagesImageColumns::MINI_THUMB_MAGIC;
    Query(origUri, arr, String(NULL), NULL, String(NULL), (ICursor**)&c);
    Boolean result = FALSE;
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);//

    if (c != NULL && (c->MoveToFirst(&result), result)) {
        Int64 id, magic;
        String path;
        c->GetInt64(0, &id);
        c->GetString(1, &path);
        c->GetInt64(2, &magic);//

        AutoPtr<MediaThumbRequest> req = RequestMediaThumbnail(path, origUri,
               MediaThumbRequest::PRIORITY_HIGH, magic);
        if (req != NULL) {
            AutoLock syncLock(req);
            while (req->mState == WAIT) {
               req->Wait();
            }

            if (req->mState == DONE) {
                result = TRUE;
            }
        }
    }
    if (c) ioUtils->CloseQuietly(ICloseable::Probe(c));

    return result;
}

ECode MediaProvider::Canonicalize(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 match;
    GetURI_MATCHER()->Match(uri, &match);

    // only support canonicalizing specific audio Uris
    if (match != AUDIO_MEDIA_ID) {
       return NOERROR;
    }

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    AutoPtr<ICursor> c;
    Query(uri, NULL, String(NULL), NULL,  String(NULL), (ICursor**)&c);
    String title;
    Boolean flag = FALSE;
    Int32 count = 0;
    if (c != NULL) {
        c->GetCount(&count);
        c->MoveToNext(&flag);
    }
    if (c == NULL || count != 1 || !flag) {
        if (c) ioUtils->CloseQuietly(ICloseable::Probe(c));
       return NOERROR;
    }

    // Construct a canonical Uri by tacking on some query parameters
    AutoPtr<IUriBuilder> builder;
    uri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendQueryParameter(CANONICAL, String("1"));
    Int32 tmp;
    c->GetColumnIndex(IMediaStoreMediaColumns::TITLE, &tmp);
    c->GetString(tmp, &title);
    ioUtils->CloseQuietly(ICloseable::Probe(c));

    if (TextUtils::IsEmpty(title)) {
       *result = NULL;
       return NOERROR;
    }
    builder->AppendQueryParameter(IMediaStoreMediaColumns::TITLE, title);
    AutoPtr<IUri> newUri;
    builder->Build((IUri**)&newUri);
    *result = newUri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaProvider::Uncanonicalize(
    /* [in] */ IUri* inUri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IUri> uri = inUri;
    String qpr;
    uri->GetQueryParameter(CANONICAL,&qpr);

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    if (uri != NULL && qpr.Equals("1")) {
        Int32 match;
        GetURI_MATCHER()->Match(uri, &match);
        if (match != AUDIO_MEDIA_ID) {
           // this type of canonical Uri is not supported
           *result = NULL;
           return NOERROR;
        }
        String titleFromUri;
        uri->GetQueryParameter(IMediaStoreMediaColumns::TITLE, &titleFromUri);
        if (titleFromUri.IsNull()) {
           // the required parameter is missing
           *result = NULL;
           return NOERROR;
        }
       // clear the query parameters, we don't need them anymore
        AutoPtr<IUriBuilder> ub;
        uri->BuildUpon((IUriBuilder**)&ub);
        ub->ClearQuery();
        uri = NULL;
        ub->Build((IUri**)&uri);

        AutoPtr<ICursor> c;
        Query(uri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&c);
       // try {
        Int32 titleIdx;
        c->GetColumnIndex(IMediaStoreMediaColumns::TITLE, &titleIdx);
        Int32 count = 0;
        Boolean flag = FALSE;
        String title;
        if (c != NULL) {
            c->GetCount(&count);
            c->MoveToNext(&flag);
            c->GetString(titleIdx, &title);
        }
        if (c != NULL && count == 1 && flag && titleFromUri.Equals(title)) {
            // the result matched perfectly
            *result = uri;
            REFCOUNT_ADD(*result);
            ioUtils->CloseQuietly(ICloseable::Probe(c));
            return NOERROR;
        }
        if (c != NULL) {
            ioUtils->CloseQuietly(ICloseable::Probe(c));
            c = NULL;
        }

        // do a lookup by title
        AutoPtr<IMediaStoreAudioMedia> msa;
        CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msa);
        AutoPtr<IList> segments;
        uri->GetPathSegments((IList**)&segments);
        AutoPtr<IInterface> obj;
        segments->Get(0, (IInterface**)&obj);
        AutoPtr<IUri> newUri;
        msa->GetContentUri(TO_STR(obj), (IUri**)&newUri);//

        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
        (*array)[0] = titleFromUri;
        (*array)[1] = String(NULL);
        Query(newUri, NULL, IMediaStoreMediaColumns::TITLE + "=?",
               array, String(NULL), (ICursor**)&c);
        if (c == NULL) {
           return NOERROR;
        }

        c->MoveToNext(&flag);
        if (!flag) {
           *result = NULL;
           ioUtils->CloseQuietly(ICloseable::Probe(c));
           return NOERROR;
        }
        // get the first matching entry and return a Uri for it
        Int64 id;
        Int32 columeId;
        c->GetColumnIndex(IBaseColumns::ID, &columeId);
        c->GetInt64(columeId, &id);
        AutoPtr<IContentUris> cur;
        CContentUris::AcquireSingleton((IContentUris**)&cur);
        ioUtils->CloseQuietly(ICloseable::Probe(c));
        return cur->WithAppendedId(newUri, id, result);
    }

    *result = uri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IUri> MediaProvider::SafeUncanonicalize(
    /* [in] */ IUri* uri)
{
    AutoPtr<IUri> newUri;
    Uncanonicalize(uri, (IUri**)&newUri);
    if (newUri != NULL) {
       return newUri;
    }
    return uri;
}

ECode MediaProvider::Query(
    /* [in] */ IUri* inUri,
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sort,
    /* [out] */ ICursor** ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = NULL;

    AutoPtr<IUri> uri = SafeUncanonicalize(inUri);

    Int32 table;
    GetURI_MATCHER()->Match(uri, &table);
    List<String> prependArgs;

    if (LOCAL_LOGV) {
        Logger::V(TAG, "Query: table=%d, uri=[%s], projectionIn=%s, selection=[%s], selectionArgs=%s",
            table, TO_CSTR(uri), Arrays::ToString(projectionIn).string(),
            selection.string(), Arrays::ToString(selectionArgs).string());
    }

    // handle MEDIA_SCANNER before calling getDatabaseForUri()
    AutoPtr<IMatrixCursor> c;
    if (table == MEDIA_SCANNER) {
       if (mMediaScannerVolume.IsNull()) {
           return NOERROR;
       }
       else {
           // create a cursor to return volume currently being scanned by the media scanner
           AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
           (*array)[0] = IMediaStore::MEDIA_SCANNER_VOLUME;
           CMatrixCursor::New(array, (IMatrixCursor**)&c);
           AutoPtr<ArrayOf<IInterface*> > inArr = ArrayOf<IInterface*>::Alloc(1);
           inArr->Set(0, CoreUtils::Convert(mMediaScannerVolume));
           c->AddRow(*inArr);
           *ret = ICursor::Probe(c);
           REFCOUNT_ADD(*ret);
           return NOERROR;
       }
    }

    // Used temporarily (until we have unique media IDs) to get an identifier
    // for the current sd card, so that the music app doesn't have to use the
    // non-public getFatVolumeId method
    if (table == FS_ID) {
       c = NULL;
       AutoPtr<ArrayOf<String> > fsd = ArrayOf<String>::Alloc(1);
       (*fsd)[0] = "fsid";
       CMatrixCursor::New(fsd, (IMatrixCursor**)&c);

       AutoPtr<ArrayOf<IInterface*> > inArr = ArrayOf<IInterface*>::Alloc(1);
       inArr->Set(0, CoreUtils::Convert(mVolumeId));
       c->AddRow(*inArr);
       *ret = ICursor::Probe(c);
       REFCOUNT_ADD(*ret);
       return NOERROR;
    }

    if (table == VERSION) {
       c = NULL;
       AutoPtr<ArrayOf<String> > vs = ArrayOf<String>::Alloc(1);
       (*vs)[0] = "version";
       CMatrixCursor::New(vs, (IMatrixCursor**)&c);
       AutoPtr<IContext> context;
       GetContext((IContext**)&context);
       Int32 vol = GetDatabaseVersion(context);
       AutoPtr<ArrayOf<IInterface*> > inArr = ArrayOf<IInterface*>::Alloc(1);
       inArr->Set(0, CoreUtils::Convert(vol));
       c->AddRow(*inArr);
       *ret = ICursor::Probe(c);
       REFCOUNT_ADD(*ret);
       return NOERROR;
    }

    String groupBy;
    AutoPtr<IDatabaseHelper> helper;
    GetDatabaseForUri(uri, (IDatabaseHelper**)&helper);
    if (helper == NULL) {
       return NOERROR;
    }
    DatabaseHelper* dbh = (DatabaseHelper*)helper.Get();
    dbh->mNumQueries++;
    AutoPtr<ISQLiteDatabase> db;
    dbh->GetReadableDatabase((ISQLiteDatabase**)&db);
    if (db == NULL) {
       return NOERROR;
    }

    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
    String limit, filter;
    uri->GetQueryParameter(String("limit"), &limit);
    uri->GetQueryParameter(String("filter"), &filter);
    AutoPtr<ArrayOf<String> > keywords;
    if (!filter.IsNull()) {
       String fs;
       AutoPtr<IUriHelper> uh;
       CUriHelper::AcquireSingleton((IUriHelper**)&uh);
       uh->Decode(filter, &fs);
       filter = fs.Trim();
       if (!TextUtils::IsEmpty(filter)) {
           AutoPtr<ArrayOf<String> > searchWords;
           StringUtils::Split(filter, String(" "), (ArrayOf<String>**)&searchWords);
           Int32 swLength = searchWords->GetLength();
           keywords = ArrayOf<String>::Alloc(swLength);
           AutoPtr<IMediaStoreAudio> msa;
           CMediaStoreAudio::AcquireSingleton((IMediaStoreAudio**)&msa);
           String key, ret;
           for (Int32 i = 0; i < swLength; i++) {
               msa->KeyFor((*searchWords)[i], &key);
               StringUtils::Replace(key, "\\", "\\\\", &ret);
               key = ret;
               StringUtils::Replace(key, "%", "\\%", &ret);
               key = ret;
               StringUtils::Replace(key, "_", "\\_", &ret);
               key = ret;
               (*keywords)[i] = key;
           }
       }
    }

    if (LOCAL_LOGV) {
        Logger::V(TAG, "keywords: %s", Arrays::ToString(keywords).string());
    }

    String str;
    uri->GetQueryParameter(String("distinct"), &str);
    if (!str.IsNull()) {
       qb->SetDistinct(TRUE);
    }

    Boolean hasThumbnailId = FALSE;
    AutoPtr<ICharSequence> acs;
    AutoPtr<IList> segments;
    uri->GetPathSegments((IList**)&segments);
    AutoPtr<IInterface> obj;
    Int32 plength = 0, klength = 0;
    if (projectionIn != NULL) plength = projectionIn->GetLength();
    if (keywords != NULL) klength = keywords->GetLength();
    switch (table) {
        case IMAGES_MEDIA:
        {
            qb->SetTables(String("images"));
            String tmp;
            uri->GetQueryParameter(String("distinct"), &tmp);
            if (!tmp.IsNull())
                qb->SetDistinct(TRUE);

            // set the project map so that data dir is prepended to _data.
            //qb.setProjectionMap(mImagesProjectionMap, true);
            break;
        }

        case IMAGES_MEDIA_ID:
        {
            qb->SetTables(String("images"));
            String tmp;
            uri->GetQueryParameter(String("distinct"), &tmp);
            if (!tmp.IsNull())
                qb->SetDistinct(TRUE);

            // set the project map so that data dir is prepended to _data.
            //qb.setProjectionMap(mImagesProjectionMap, true);
            AutoPtr<ICharSequence> cs = CoreUtils::Convert("_id=?");
            qb->AppendWhere(cs);
            obj = NULL;
            segments->Get(3, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }

        case IMAGES_THUMBNAILS_ID:
        {
            hasThumbnailId = TRUE;
            // fall through
        }
        case IMAGES_THUMBNAILS:
        {
            if (!QueryThumbnail(qb, uri, String("thumbnails"), String("image_id"), hasThumbnailId)) {
               return NOERROR;
            }
            break;
        }
        case AUDIO_MEDIA:
        {
           if (projectionIn != NULL && plength == 1 &&  selectionArgs == NULL
                   && (selection.IsNull() || selection.EqualsIgnoreCase("is_music=1")
                     || selection.EqualsIgnoreCase("is_podcast=1") )
                   && (*projectionIn)[0].EqualsIgnoreCase("count(*)")
                   && keywords != NULL) {
                //Log.i("@@@@", "taking fast path for counting songs");
                qb->SetTables(String("audio_meta"));
           }
           else {
                qb->SetTables(String("audio"));
                AutoPtr<ICharSequence> andCs = CoreUtils::Convert(" AND ");
                StringBuilder sb(IMediaStoreAudioAudioColumns::ARTIST_KEY);
                sb += "||"; sb += IMediaStoreAudioAudioColumns::ALBUM_KEY;
                sb += "||"; sb += IMediaStoreAudioAudioColumns::TITLE_KEY;
                sb += " LIKE ? ESCAPE '\\'";
                acs = CoreUtils::Convert(sb.ToString());
                for (Int32 i = 0; i < klength; i++) {
                    if (i > 0) {
                        qb->AppendWhere(andCs);
                    }
                    qb->AppendWhere(acs);
                    StringBuilder sb("%"); sb += (*keywords)[i]; sb += "%";
                    prependArgs.PushBack(sb.ToString());
                }
           }
           break;
        }

        case AUDIO_MEDIA_ID:
        {
            qb->SetTables(String("audio"));
            qb->AppendWhere(CoreUtils::Convert("_id=?"));
            obj = NULL;
            segments->Get(3, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }

        case AUDIO_MEDIA_ID_GENRES:
        {
            qb->SetTables(String("audio_genres"));
            acs = NULL;
            acs = CoreUtils::Convert("_id IN (SELECT genre_id FROM "
                   "audio_genres_map WHERE audio_id=?)");
            qb->AppendWhere(acs);
            obj = NULL;
            segments->Get(3, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }

        case AUDIO_MEDIA_ID_GENRES_ID:
        {
            qb->SetTables(String("audio_genres"));
            qb->AppendWhere(CoreUtils::Convert("_id=?"));
            obj = NULL;
            segments->Get(5, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }

        case AUDIO_MEDIA_ID_PLAYLISTS:
        {
            qb->SetTables(String("audio_playlists"));
            acs = NULL;
            acs = CoreUtils::Convert("_id IN (SELECT playlist_id FROM "
                   "audio_playlists_map WHERE audio_id=?)");
            qb->AppendWhere(acs);
            obj = NULL;
            segments->Get(3, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }

        case AUDIO_MEDIA_ID_PLAYLISTS_ID:
        {
            qb->SetTables(String("audio_playlists"));
            qb->AppendWhere(CoreUtils::Convert("_id=?"));
            obj = NULL;
            segments->Get(5, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }
        case AUDIO_GENRES:
        {
            qb->SetTables(String("audio_genres"));
            break;
        }

        case AUDIO_GENRES_ID:
        {
            qb->SetTables(String("audio_genres"));
            qb->AppendWhere(CoreUtils::Convert("_id=?"));
            obj = NULL;
            segments->Get(3, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }

        case AUDIO_GENRES_ALL_MEMBERS:
        case AUDIO_GENRES_ID_MEMBERS:
        {
            // if simpleQuery is true, we can do a simpler query on just audio_genres_map
            // we can do this if we have no keywords and our projection includes just columns
            // from audio_genres_map
            Boolean simpleQuery = (keywords == NULL && projectionIn != NULL
                   && (selection.IsNull() || selection.EqualsIgnoreCase("genre_id=?")));
            if (projectionIn != NULL) {
                for (Int32 i = 0; i < plength; i++) {
                    String p = (*projectionIn)[i];
                    if (p.Equals("_id")) {
                        // note, this is different from playlist below, because
                        // "_id" used to (wrongly) be the audio id in this query, not
                        // the row id of the entry in the map, and we preserve this
                        // behavior for backwards compatibility
                        simpleQuery = FALSE;
                    }
                    if (simpleQuery && !(p.Equals("audio_id") ||
                        p.Equals("genre_id"))) {
                        simpleQuery = FALSE;
                    }
                }
            }
            if (simpleQuery) {
                qb->SetTables(String("audio_genres_map_noid"));
                if (table == AUDIO_GENRES_ID_MEMBERS) {
                    qb->AppendWhere(CoreUtils::Convert("genre_id=?"));
                    obj = NULL;
                    segments->Get(3, (IInterface**)&obj);
                    prependArgs.PushBack(TO_STR(obj));
                }
            }
            else {
                qb->SetTables(String("audio_genres_map_noid, audio"));
                qb->AppendWhere(CoreUtils::Convert("audio._id = audio_id"));
                if (table == AUDIO_GENRES_ID_MEMBERS) {
                    qb->AppendWhere(CoreUtils::Convert(" AND genre_id=?"));
                    obj = NULL;
                    segments->Get(3, (IInterface**)&obj);
                    prependArgs.PushBack(TO_STR(obj));
                }
                AutoPtr<ICharSequence> andCs = CoreUtils::Convert(" AND ");
                StringBuilder sb(IMediaStoreAudioAudioColumns::ARTIST_KEY);
                sb += "||"; sb += IMediaStoreAudioAudioColumns::ALBUM_KEY;
                sb += "||"; sb += IMediaStoreAudioAudioColumns::TITLE_KEY;
                sb += " LIKE ? ESCAPE '\\'";
                acs = CoreUtils::Convert(sb.ToString());
                for (Int32 i = 0; i < klength; i++) {
                    qb->AppendWhere(andCs);
                    qb->AppendWhere(acs);
                    StringBuilder sb("%"); sb += (*keywords)[i]; sb += "%";
                    prependArgs.PushBack(sb.ToString());
                }
            }
            break;
        }

        case AUDIO_PLAYLISTS:
        {
            qb->SetTables(String("audio_playlists"));
            break;
        }

        case AUDIO_PLAYLISTS_ID:
        {
            qb->SetTables(String("audio_playlists"));
            qb->AppendWhere(CoreUtils::Convert("_id=?"));
            obj = NULL;
            segments->Get(3, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }

        case AUDIO_PLAYLISTS_ID_MEMBERS_ID:
        case AUDIO_PLAYLISTS_ID_MEMBERS:
        {
            // if simpleQuery is true, we can do a simpler query on just audio_playlists_map
            // we can do this if we have no keywords and our projection includes just columns
            // from audio_playlists_map
            Boolean simpleQuery = (keywords == NULL && projectionIn != NULL
                   && (selection.IsNull() || selection.EqualsIgnoreCase("playlist_id=?")));
            if (projectionIn != NULL) {
               for (Int32 i = 0; i < plength; i++) {
                   String p = (*projectionIn)[i];
                   if (simpleQuery && !(p.Equals("audio_id") ||
                           p.Equals("playlist_id") || p.Equals("play_order"))) {
                       simpleQuery = FALSE;
                   }
                   if (p.Equals("_id")) {
                       (*projectionIn)[i] = "audio_playlists_map._id AS _id";
                   }
               }
            }
            if (simpleQuery) {
               qb->SetTables(String("audio_playlists_map"));
               qb->AppendWhere(CoreUtils::Convert("playlist_id=?"));
               obj = NULL;
               segments->Get(3, (IInterface**)&obj);
               prependArgs.PushBack(TO_STR(obj));
            }
            else {
               qb->SetTables(String("audio_playlists_map, audio"));
               qb->AppendWhere(CoreUtils::Convert("audio._id = audio_id AND playlist_id=?"));
               obj = NULL;
               segments->Get(3, (IInterface**)&obj);
               prependArgs.PushBack(TO_STR(obj));

               AutoPtr<ICharSequence> andCs = CoreUtils::Convert(" AND ");
               StringBuilder sb(IMediaStoreAudioAudioColumns::ARTIST_KEY);
               sb += "||"; sb += IMediaStoreAudioAudioColumns::ALBUM_KEY;
               sb += "||"; sb += IMediaStoreAudioAudioColumns::TITLE_KEY;
               sb += " LIKE ? ESCAPE '\\'";
               acs = CoreUtils::Convert(sb.ToString());
               for (Int32 i = 0; i < klength; i++) {
                   qb->AppendWhere(andCs);
                   qb->AppendWhere(acs);
                   StringBuilder sb("%"); sb += (*keywords)[i]; sb += "%";
                   prependArgs.PushBack(sb.ToString());
               }
            }
            if (table == AUDIO_PLAYLISTS_ID_MEMBERS_ID) {
               qb->AppendWhere(CoreUtils::Convert(" AND audio_playlists_map._id=?"));
               obj = NULL;
               segments->Get(5, (IInterface**)&obj);
               prependArgs.PushBack(TO_STR(obj));
            }
            break;
        }

        case VIDEO_MEDIA:
        {
            qb->SetTables(String("video"));
            break;
        }
        case VIDEO_MEDIA_ID:
        {
            qb->SetTables(String("video"));
            qb->AppendWhere(CoreUtils::Convert("_id=?"));
            obj = NULL;
            segments->Get(3, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }

        case VIDEO_THUMBNAILS_ID:
        {
            hasThumbnailId = TRUE;
            //fall through
        }
        case VIDEO_THUMBNAILS:
        {
           if (!QueryThumbnail(qb, uri, String("videothumbnails"), String("video_id"), hasThumbnailId)) {
               return NOERROR;
           }
           break;
        }

        case AUDIO_ARTISTS:
        {
            if (projectionIn != NULL && plength == 1 &&  selectionArgs == NULL
               && (selection.IsNull() || selection.GetLength() == 0)
               && (*projectionIn)[0].EqualsIgnoreCase("count(*)")
               && keywords != NULL) {
                //Log.i("@@@@", "taking fast path for counting artists");
                qb->SetTables(String("audio_meta"));
                (*projectionIn)[0] = "count(distinct artist_id)";
                qb->AppendWhere(CoreUtils::Convert("is_music=1"));
            }
            else {
                qb->SetTables(String("artist_info"));
                AutoPtr<ICharSequence> andCs = CoreUtils::Convert(" AND ");
                StringBuilder sb(IMediaStoreAudioAudioColumns::ARTIST_KEY);
                sb += " LIKE ? ESCAPE '\\'";
                acs = CoreUtils::Convert(sb.ToString());
                for (Int32 i = 0; i < klength; i++) {
                   if (i > 0) {
                       qb->AppendWhere(andCs);
                   }
                   qb->AppendWhere(acs);
                   StringBuilder sb("%"); sb += (*keywords)[i]; sb += "%";
                   prependArgs.PushBack(sb.ToString());
                }
            }
            break;
        }
        case AUDIO_ARTISTS_ID:
        {
           qb->SetTables(String("artist_info"));
           qb->AppendWhere(CoreUtils::Convert("_id=?"));
           obj = NULL;
           segments->Get(3, (IInterface**)&obj);
           prependArgs.PushBack(TO_STR(obj));
           break;
        }//

        case AUDIO_ARTISTS_ID_ALBUMS:
        {
            qb->SetTables(String("audio LEFT OUTER JOIN album_art ON"
                   " audio.album_id=album_art.album_id"));
            acs = CoreUtils::Convert("is_music=1 AND audio.album_id IN (SELECT album_id FROM "
                   "artists_albums_map WHERE artist_id=?)");
            qb->AppendWhere(acs);
            obj = NULL;
            segments->Get(3, (IInterface**)&obj);
            String aid = TO_STR(obj);
            prependArgs.PushBack(aid);
            AutoPtr<ICharSequence> andCs = CoreUtils::Convert(" AND ");
            StringBuilder sb(IMediaStoreAudioAudioColumns::ARTIST_KEY);
            sb += "||"; sb += IMediaStoreAudioAudioColumns::ALBUM_KEY;
            sb += " LIKE ? ESCAPE '\\'";
            acs = CoreUtils::Convert(sb.ToString());
            for (Int32 i = 0; i < klength; i++) {
                qb->AppendWhere(andCs);
                qb->AppendWhere(acs);
                StringBuilder sb("%"); sb += (*keywords)[i]; sb += "%";
                prependArgs.PushBack(sb.ToString());
            }
            groupBy = "audio.album_id";
            sb.Reset(); sb += "count(CASE WHEN artist_id==";
            sb += aid;  sb += " THEN 'foo' ELSE NULL END) AS ";
            sb += IMediaStoreAudioAlbumColumns::NUMBER_OF_SONGS_FOR_ARTIST;
            InitHashMapWithString(IMediaStoreAudioAlbumColumns::NUMBER_OF_SONGS_FOR_ARTIST,
                sb.ToString(), sArtistAlbumsMap);
            qb->SetProjectionMap(IMap::Probe(sArtistAlbumsMap));
            break;
        }
        case AUDIO_ALBUMS:
        {
            if (projectionIn != NULL && plength == 1 &&  selectionArgs == NULL
                && (selection == NULL || selection.GetLength() == 0)
                && (*projectionIn)[0].EqualsIgnoreCase("count(*)")
                && keywords != NULL) {
                //Log.i("@@@@", "taking fast path for counting albums");
                qb->SetTables(String("audio_meta"));
                (*projectionIn)[0] = "count(distinct album_id)";
                qb->AppendWhere(CoreUtils::Convert("is_music=1"));
            }
            else {
                qb->SetTables(String("album_info"));
                AutoPtr<ICharSequence> andCs = CoreUtils::Convert(" AND ");
                StringBuilder sb(IMediaStoreAudioAudioColumns::ARTIST_KEY);
                sb += "||"; sb += IMediaStoreAudioAudioColumns::ALBUM_KEY;
                sb += " LIKE ? ESCAPE '\\'";
                acs = CoreUtils::Convert(sb.ToString());
                for (Int32 i = 0; i < klength; i++) {
                    if (i > 0) {
                        qb->AppendWhere(andCs);
                    }
                    qb->AppendWhere(acs);
                    StringBuilder sb("%"); sb += (*keywords)[i]; sb += "%";
                    prependArgs.PushBack(sb.ToString());
                }
            }
            break;
        }

        case AUDIO_ALBUMS_ID:
        {
            qb->SetTables(String("album_info"));
            qb->AppendWhere(CoreUtils::Convert("_id=?"));
            obj = NULL;
            segments->Get(3, (IInterface**)&obj);
            prependArgs.PushBack(TO_STR(obj));
            break;
        }

        case AUDIO_ALBUMART_ID:
        {
           qb->SetTables(String("album_art"));
           qb->AppendWhere(CoreUtils::Convert("album_id=?"));
           obj = NULL;
           segments->Get(3, (IInterface**)&obj);
           prependArgs.PushBack(TO_STR(obj));
           break;
        }

        case AUDIO_SEARCH_LEGACY:
        {
           Logger::W(TAG, "Legacy media search Uri used. Please update your code.");
           // fall through
        }
        case AUDIO_SEARCH_FANCY:
        case AUDIO_SEARCH_BASIC:
        {
           AutoPtr<ICursor> cc;
           cc = DoAudioSearch(db, qb, uri, projectionIn, selection,
                   Combine(prependArgs, selectionArgs), sort, table, limit);
           *ret = cc;
           REFCOUNT_ADD(*ret);
           return NOERROR;
        }

        case FILES_ID:
        case MTP_OBJECTS_ID:
        {
           qb->AppendWhere(CoreUtils::Convert("_id=?"));
           obj = NULL;
           segments->Get(2, (IInterface**)&obj);
           prependArgs.PushBack(TO_STR(obj));
            // fall through
        }
        case FILES:
        case MTP_OBJECTS:
        {
           qb->SetTables(String("files"));
           break;
        }

        case MTP_OBJECT_REFERENCES:
        {
           obj = NULL;
           segments->Get(2, (IInterface**)&obj);
           Int32 handle = StringUtils::ParseInt32(TO_STR(obj));
           AutoPtr<ICursor> cc = GetObjectReferences(dbh, db, handle);
           *ret = cc;
           REFCOUNT_ADD(*ret);
           return NOERROR;
        }

        case MEDIA_BOOKMARK:
        {
           qb->SetTables(String("bookmarks"));
           break;
        }
        case MEDIA_BOOKMARK_ID:
        {
           qb->SetTables(String("bookmarks"));
           obj = NULL;
           segments->Get(2, (IInterface**)&obj);
           StringBuilder sb("_id = ");
           sb += TO_CSTR(obj);
           acs = CoreUtils::Convert(sb.ToString());
           qb->AppendWhere(acs);
           break;
        }
        default:
        {
            Logger::E(TAG, "Unknown URL: %s", TO_CSTR(uri));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    String nullStr;
    AutoPtr<ArrayOf<String> > args = Combine(prependArgs, selectionArgs);
    String querySql;
    qb->BuildQuery(projectionIn, selection, args, groupBy, nullStr, sort, limit, &querySql);
    if (LOCAL_LOGV) Logger::V(TAG, "query = [%s]", querySql.string());
    AutoPtr<ICursor> ic;
    qb->Query(db, projectionIn, selection, args, groupBy, nullStr, sort, limit, (ICursor**)&ic);

    if (ic != NULL) {
        String nonotify;
        uri->GetQueryParameter(String("nonotify"), &nonotify);
        if (nonotify.IsNull() || !nonotify.Equals("1")) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IContentResolver> resolver;
            context->GetContentResolver((IContentResolver**)&resolver);
            ic->SetNotificationUri(resolver, uri);
        }
    }

    *ret = ic;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode MediaProvider::GetType(
    /* [in] */ IUri* url,
    /* [out] */ String* result)
{
    Int32 vol;
    GetURI_MATCHER()->Match(url, &vol);
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    switch (vol) {
       case IMAGES_MEDIA_ID:
       case AUDIO_MEDIA_ID:
       case AUDIO_PLAYLISTS_ID_MEMBERS_ID:
       case VIDEO_MEDIA_ID:
       case FILES_ID:
       {
           AutoPtr<ICursor> c;
           // try {
               Query(url, MIME_TYPE_PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&c);
               Int32 count;
               c->GetCount(&count);
               if (c != NULL && count == 1) {
                   Boolean flag = FALSE;
                   c->MoveToFirst(&flag);
                   String mimeType;
                   c->GetString(1, &mimeType);
                   c->Deactivate();
                   *result = mimeType;
                   ioUtils->CloseQuietly(ICloseable::Probe(c));
                   return NOERROR;
               }
           // } finally {
               // ioUtils->CloseQuietly(ICloseable::Probe(c));
           // }
           break;
       }
       case IMAGES_MEDIA:
       case IMAGES_THUMBNAILS:
           {
               *result = IMediaStoreAudioMedia::CONTENT_TYPE;
               return NOERROR;
           }
       case AUDIO_ALBUMART_ID:
       case IMAGES_THUMBNAILS_ID:
           {
               *result = String("image/jpeg");
               return NOERROR;
           }
       case AUDIO_MEDIA:
       case AUDIO_GENRES_ID_MEMBERS:
       case AUDIO_PLAYLISTS_ID_MEMBERS:
           {
               *result = IMediaStoreAudioMedia::CONTENT_TYPE;
               return NOERROR;
           }
       case AUDIO_GENRES:
       case AUDIO_MEDIA_ID_GENRES:
           {
               *result = IMediaStoreAudioGenres::CONTENT_TYPE;
               return NOERROR;
           }
       case AUDIO_GENRES_ID:
       case AUDIO_MEDIA_ID_GENRES_ID:
           {
               *result = IMediaStoreAudioGenres::ENTRY_CONTENT_TYPE;
               return NOERROR;
           }
       case AUDIO_PLAYLISTS:
       case AUDIO_MEDIA_ID_PLAYLISTS:
           {
               *result = IMediaStoreAudioPlaylists::CONTENT_TYPE;
               return NOERROR;
           }

       case AUDIO_PLAYLISTS_ID:
       case AUDIO_MEDIA_ID_PLAYLISTS_ID:
           {
               *result = IMediaStoreAudioPlaylists::ENTRY_CONTENT_TYPE;
               return NOERROR;
           }
       case VIDEO_MEDIA:
           {
               *result = IMediaStoreVideoMedia::CONTENT_TYPE;
               return NOERROR;
           }
    }
    Logger::E(TAG, "Unknown URL : %s", TO_CSTR(url));
    return  E_ILLEGAL_STATE_EXCEPTION;
}

AutoPtr<MediaThumbRequest> MediaProvider::RequestMediaThumbnail(
    /* [in] */ const String& path,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 priority,
    /* [in] */ Int64 magic)
{
    AutoLock syncLock(mMediaThumbQueue);
    AutoPtr<MediaThumbRequest> req;
    // try {
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    req = new MediaThumbRequest();
    req->constructor(resolver, path, uri, priority, magic);
    mMediaThumbQueue->Add(IMediaThumbRequest::Probe(req));
    // Trigger the handler.
    AutoPtr<IMessage> msg;
    mThumbHandler->ObtainMessage(IMAGE_THUMB, (IMessage**)&msg);
    msg->SendToTarget();
    // } catch (Throwable t) {
       // Log.w(TAG, t);
    // }
    return req;
}

ECode MediaProvider::Delete(
    /* [in] */ IUri* inUri,
    /* [in] */ const String& userWhere,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (LOCAL_LOGV) {
        Logger::V(TAG, " Delete:[%s], where:[%s], args:%s", TO_CSTR(inUri), userWhere.string(),
            Arrays::ToString(whereArgs).string());
    }

    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    AutoPtr<IUri> uri = SafeUncanonicalize(inUri);
    Int32 count, match;
    GetURI_MATCHER()->Match(uri, &match);

    // handle MEDIA_SCANNER before calling getDatabaseForUri()
    if (match == MEDIA_SCANNER) {
        if (mMediaScannerVolume.IsNull()) {
           return NOERROR;
        }

        StringBuilder sb("content://media/"); sb += mMediaScannerVolume; sb += "/audio";
        AutoPtr<IUri> oUri;
        uh->Parse(sb.ToString(), (IUri**)&oUri);
        AutoPtr<IDatabaseHelper> database;
        GetDatabaseForUri(oUri, (IDatabaseHelper**)&database);
        if (database == NULL) {
            Logger::W(TAG, "no database for scanned volume %s", mMediaScannerVolume.string());
        }
        else {
            DatabaseHelper* dbh = ((DatabaseHelper*)database.Get());
            dbh->mScanStopTime = SystemClock::GetCurrentTimeMicro();
            String msg = Dump(database, FALSE);
            AutoPtr<ISQLiteDatabase> sd;
            dbh->GetWritableDatabase((ISQLiteDatabase**)&sd);
            LogToDb(sd, msg);
        }
        mMediaScannerVolume = String(NULL);
        *result = 1;
        return NOERROR;
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Boolean flag = FALSE;

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    if (match == VOLUMES_ID) {
       DetachVolume(uri);
       count = 1;
    }
    else if (match == MTP_CONNECTED) {
        AutoLock syncLock(mMtpServiceConnection);
        if (mMtpService != NULL) {
           // MTP has disconnected, so release our connection to MtpService//
           context->UnbindService(mMtpServiceConnection);
           count = 1;
           // mMtpServiceConnection.onServiceDisconnected might not get called,
           // so set mMtpService = null here
           mMtpService = NULL;
       }
       else {
           count = 0;
       }
    }
    else {
        AutoPtr<IDatabaseHelper> database;
        GetDatabaseForUri(uri, (IDatabaseHelper**)&database);
        if (database == NULL) {
            Logger::E(TAG, "Unknown URI: %s match: %d", TO_CSTR(uri), match);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
        DatabaseHelper* dbh = ((DatabaseHelper*)database.Get());
        dbh->mNumDeletes++;
        AutoPtr<ISQLiteDatabase> db;
        dbh->GetWritableDatabase((ISQLiteDatabase**)&db);
        String volumeName = GetVolumeName(uri);

        {
            AutoLock syncLock(sGetTableAndWhereParam);
            String nullStr;
            AutoPtr<ICursor> c;
            GetTableAndWhere(uri, match, userWhere, sGetTableAndWhereParam);

            if (LOCAL_LOGV) {
                Logger::V(TAG, "Delete table:[%s], where:[%s], args:%s",
                    sGetTableAndWhereParam->mTable.string(), sGetTableAndWhereParam->mWhere.string(),
                    Arrays::ToString(whereArgs).string());
            }

            if (sGetTableAndWhereParam->mTable.Equals("files")) {
                String deleteparam;
                uri->GetQueryParameter(IMediaStore::PARAM_DELETE_DATA, &deleteparam);
                if (deleteparam.IsNull() || ! deleteparam.Equals("false")) {
                    dbh->mNumQueries++;
                    db->Query(sGetTableAndWhereParam->mTable,
                       sMediaTypeDataId,
                       sGetTableAndWhereParam->mWhere, whereArgs, nullStr, nullStr, nullStr, (ICursor**)&c);
                   AutoPtr<ArrayOf<String> > idvalue = ArrayOf<String>::Alloc(1);
                   (*idvalue)[0] = String("");

                   AutoPtr<ArrayOf<String> > playlistvalues = ArrayOf<String>::Alloc(2);
                   (*playlistvalues)[0] = String("");
                   (*playlistvalues)[1] = String("");

                    while ((c->MoveToNext(&flag), flag)) {
                        Int32 mediaType;
                        String data;
                        Int64 id;
                        c->GetInt32(0, &mediaType);
                        c->GetString(1, &data);
                        c->GetInt64(2, &id);

                        if (mediaType == IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE) {
                            DeleteIfAllowed(uri, data);
                            MediaDocumentsProvider::OnMediaStoreDelete(context,
                                volumeName, IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE, id);//

                            (*idvalue)[0] = StringUtils::ToString(id);
                            dbh->mNumQueries++;
                            AutoPtr<ICursor> cc;
                            db->Query(String("thumbnails"), sDataOnlyColumn,
                                String("image_id=?"), idvalue, nullStr, nullStr, nullStr, (ICursor**)&cc);
                            String cs;
                            while ((cc->MoveToNext(&flag), flag)) {
                                cc->GetString(0, &cs);
                                DeleteIfAllowed(uri, cs);
                            }
                            dbh->mNumDeletes++;

                            Int32 vol;
                            db->Delete(String("thumbnails"), String("image_id=?"), idvalue, &vol);
                            ioUtils->CloseQuietly(ICloseable::Probe(cc));
                        }
                        else if (mediaType == IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO) {
                            DeleteIfAllowed(uri, data);
                            MediaDocumentsProvider::OnMediaStoreDelete(context,
                                volumeName, IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO, id);

                        }
                        else if (mediaType == IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO) {
                            if (!dbh->mInternal) {
                                MediaDocumentsProvider::OnMediaStoreDelete(context,
                                    volumeName, IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO, id);

                                (*idvalue)[0] = StringUtils::ToString(id);
                                dbh->mNumDeletes += 2; // also count the one below
                                Int32 dvol;
                                db->Delete(String("audio_genres_map"), String("audio_id=?"), idvalue, &dvol);
                                // for each playlist that the item appears in, move
                                // all the items behind it forward by one
                                AutoPtr<ICursor> cc;
                                db->Query(String("audio_playlists_map"), sPlaylistIdPlayOrder,
                                    String("audio_id=?"), idvalue, nullStr, nullStr, nullStr, (ICursor**)&cc);
                                // try {
                                Int64 i;
                                Int32 j;
                                AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(2);
                                while ((cc->MoveToNext(&flag), flag)) {
                                    cc->GetInt64(0, &i);
                                    cc->GetInt32(1, &j);
                                    (*playlistvalues)[0] = StringUtils::ToString(i);
                                    (*playlistvalues)[1] = StringUtils::ToString(j);
                                    dbh->mNumUpdates++;
                                    arr->Set(0, CoreUtils::Convert((*playlistvalues)[0]));
                                    arr->Set(1, CoreUtils::Convert((*playlistvalues)[1]));
                                    db->ExecSQL(
                                        String("UPDATE audio_playlists_map SET play_order=play_order-1"
                                            " WHERE playlist_id=? AND play_order>?"), arr);
                                }
                                Int32 vol;
                                db->Delete(String("audio_playlists_map"), String("audio_id=?"), idvalue, &vol);
                                // } finally {
                                ioUtils->CloseQuietly(ICloseable::Probe(cc));
                                // }
                            }
                        }
                        else if (mediaType == IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST) {
                           // TODO, maybe: remove the audio_playlists_cleanup trigger and
                           // implement functionality here (clean up the playlist map)
                        }
                    }
                    ioUtils->CloseQuietly(ICloseable::Probe(c));
                }
            }

            switch (match) {
                case MTP_OBJECTS:
                case MTP_OBJECTS_ID:
                {
                    // don't send objectRemoved event since this originated from MTP
                    mDisableMtpObjectCallbacks = TRUE;
                    dbh->mNumDeletes++;
                    db->Delete(String("files"), sGetTableAndWhereParam->mWhere, whereArgs, &count);
                    mDisableMtpObjectCallbacks = FALSE;
                    break;
                }

                case AUDIO_GENRES_ID_MEMBERS:
                {
                    dbh->mNumDeletes++;
                    db->Delete(String("audio_genres_map"),
                       sGetTableAndWhereParam->mWhere, whereArgs, &count);
                    break;
                }

                case IMAGES_THUMBNAILS_ID:
                case IMAGES_THUMBNAILS:
                case VIDEO_THUMBNAILS_ID:
                case VIDEO_THUMBNAILS:
                {
                    // Delete the referenced files first.
                    AutoPtr<ICursor> c;
                    db->Query(sGetTableAndWhereParam->mTable,
                        sDataOnlyColumn,
                        sGetTableAndWhereParam->mWhere, whereArgs, nullStr, nullStr, nullStr, (ICursor**)&c);
                    if (c != NULL) {
                        while ((c->MoveToNext(&flag), flag)) {
                            String str;
                            c->GetString(0, &str);
                            DeleteIfAllowed(uri, str);
                        }
                        ioUtils->CloseQuietly(ICloseable::Probe(c));
                    }
                    dbh->mNumDeletes++;
                    db->Delete(sGetTableAndWhereParam->mTable,
                       sGetTableAndWhereParam->mWhere, whereArgs, &count);
                    break;
                }

                default:
                {
                    dbh->mNumDeletes++;
                    db->Delete(sGetTableAndWhereParam->mTable,
                        sGetTableAndWhereParam->mWhere, whereArgs, &count);
                    break;
                }
            }

            // Since there are multiple Uris that can refer to the same files
            // and deletes can affect other objects in storage (like subdirectories
            // or playlists) we will notify a change on the entire volume to make
            // sure no listeners miss the notification.
            StringBuilder sb("content://"); sb += IMediaStore::AUTHORITY;
            sb += "/"; sb += volumeName;
            AutoPtr<IUri> notifyUri;
            uh->Parse(sb.ToString(), (IUri**)&notifyUri);
            AutoPtr<IContentResolver> resolver;
            context->GetContentResolver((IContentResolver**)&resolver);
            resolver->NotifyChange(notifyUri, NULL);
        }
    }

    *result = count;
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > MediaProvider::GetCompressedAlbumArt(
    /* [in] */ IContext* context,
    /* [in] */ const String& path)
{
    AutoPtr<ArrayOf<Byte> > compressed;

    // try {
    AutoPtr<IFile> f;
    CFile::New(path, (IFile**)&f);
    AutoPtr<IParcelFileDescriptorHelper> pfdhelper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfdhelper);
    AutoPtr<IParcelFileDescriptor> pfd;
    pfdhelper->Open(f, IParcelFileDescriptor::MODE_READ_ONLY, (IParcelFileDescriptor**)&pfd);//

    AutoPtr<IMediaScanner> scanner;
    CMediaScanner::New(context, (IMediaScanner**)&scanner);
    AutoPtr<IFileDescriptor> fd;
    pfd->GetFileDescriptor((IFileDescriptor**)&fd);
    scanner->ExtractAlbumArt(fd, (ArrayOf<Byte>**)&compressed);
    pfd->Close();//

    // If no embedded art exists, look for a suitable image file in the
    // same directory as the media file, except if that directory is
    // is the root directory of the sd card or the download directory.
    // We look for, in order of preference:
    // 0 AlbumArt.jpg
    // 1 AlbumArt*Large.jpg
    // 2 Any other jpg image with 'albumart' anywhere in the name
    // 3 Any other jpg image
    // 4 any other png image
    if (compressed == NULL && !path.IsNull()) {
       Int32 lastSlash = path.LastIndexOf('/');
       if (lastSlash > 0) {
           String artPath = path.Substring(0, lastSlash);
           f = Environment::GetExternalStoragePublicDirectory(Environment::DIRECTORY_DOWNLOADS);
           String dwndir;
           f->GetAbsolutePath(&dwndir);//

           String bestmatch;
           Boolean flag = FALSE;
           AutoPtr<ICharSequence> key;
           AutoPtr<ICharSequence> value;
           AutoPtr<IInterface> obj;
           {
                AutoLock syncLock(sFolderArtMap);
                key = CoreUtils::Convert(artPath);
                sFolderArtMap->ContainsKey(key, &flag);
                if (flag) {
                    sFolderArtMap->Get(key, (IInterface**)&obj);
                    bestmatch = TO_STR(obj);
                }
                else if (!(IsRootStorageDir(artPath)) &&
                       !(artPath.EqualsIgnoreCase(dwndir))) {
                   AutoPtr<IFile> dir;
                   CFile::New(artPath, (IFile**)&dir);
                   AutoPtr<ArrayOf<String> > entrynames;
                   dir->List((ArrayOf<String>**)&entrynames);
                   if (entrynames == NULL) {
                       return NULL;
                   }
                   bestmatch = String(NULL);
                   Int32 matchlevel = 1000;
                   Int32 length = entrynames->GetLength();
                   for (Int32 i = length - 1; i >=0; i--) {
                       String entry = (*entrynames)[i].ToLowerCase();
                       if (entry.Equals("albumart.jpg")) {
                           bestmatch = (*entrynames)[i];
                           break;
                       } else if (entry.StartWith("albumart")
                               && entry.EndWith("large.jpg")
                               && matchlevel > 1) {
                           bestmatch = (*entrynames)[i];
                           matchlevel = 1;
                       } else if (entry.Contains("albumart")
                               && entry.EndWith(".jpg")
                               && matchlevel > 2) {
                           bestmatch = (*entrynames)[i];
                           matchlevel = 2;
                       } else if (entry.EndWith(".jpg") && matchlevel > 3) {
                           bestmatch = (*entrynames)[i];
                           matchlevel = 3;
                       } else if (entry.EndWith(".png") && matchlevel > 4) {
                           bestmatch = (*entrynames)[i];
                           matchlevel = 4;
                       }
                   }
                   // note that this may insert null if no album art was found
                   value = CoreUtils::Convert(bestmatch);
                   sFolderArtMap->Put(key, value);
               }
           }//

           if (!bestmatch.IsNull()) {
               AutoPtr<IFile> file;
               CFile::New(artPath, bestmatch, (IFile**)&file);
               Boolean flag = FALSE;
               file->Exists(&flag);
               if (flag) {
                    AutoPtr<IFileInputStream> stream;
                    Int64 fileLength;
                    file->GetLength(&fileLength);
                    compressed = ArrayOf<Byte>::Alloc(fileLength);
                    CFileInputStream::New(file, (IFileInputStream**)&stream);
                    // stream = new FileInputStream(file);
                    Int32 number;
                    ECode ec = IInputStream::Probe(stream)->Read(compressed, &number);
                    if (FAILED(ec)) {
                        compressed = NULL;
                    }

                    if (stream != NULL) {
                       ICloseable::Probe(stream)->Close();
                    }
                }
            }
        }
    }
    return compressed;
}

AutoPtr<IUri> MediaProvider::GetAlbumArtOutputUri(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 album_id,
    /* [in] */ IUri* albumart_uri)
    {
    AutoPtr<IUri> out;
    // TODO: this could be done more efficiently with a call to db.replace(), which
    // replaces or inserts as needed, making it unnecessary to query() first.//

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);//


    if (albumart_uri != NULL) {
       AutoPtr<ArrayOf<String> > arr1 = ArrayOf<String>::Alloc(1);
       (*arr1)[0] = IMediaStoreMediaColumns::DATA;
       AutoPtr<ICursor> c;
       Query(albumart_uri, arr1, String(NULL), NULL, String(NULL), (ICursor**)&c);
       Boolean flag = FALSE;
       if (c != NULL && (c->MoveToFirst(&flag), flag)) {
           String albumart_path;
           c->GetString(0, &albumart_path);
           if (EnsureFileExists(albumart_uri, albumart_path)) {
               out = albumart_uri;
           }
       }
       else {
           albumart_uri = NULL;
       }

       if (c) ioUtils->CloseQuietly(ICloseable::Probe(c));
    }

    if (albumart_uri == NULL){
        AutoPtr<IContentValues> initialValues;
        CContentValues::New((IContentValues**)&initialValues);
        initialValues->Put(String("album_id"), album_id);
       // try {
        AutoPtr<IContentValues> values;
        EnsureFile(FALSE, initialValues, String(""), ALBUM_THUMB_FOLDER, (IContentValues**)&values);
        helper->mNumInserts++;
        Int64 rowId;
        db->Insert(String("album_art"), IMediaStoreMediaColumns::DATA, values, &rowId);
        if (rowId > 0) {
            AutoPtr<IContentUris> cu;
            CContentUris::AcquireSingleton((IContentUris**)&cu);
            cu->WithAppendedId(ALBUMART_URI, rowId, (IUri**)&out);
            // ensure the parent directory exists
            String albumart_path;
            values->GetAsString(IMediaStoreMediaColumns::DATA, &albumart_path);
            EnsureFileExists(out, albumart_path);
        }
       // } catch (IllegalStateException ex) {
            // Logger::E(TAG, "error creating album thumb file");
       // }
    }

    return out;
}

Int64 MediaProvider::GetKeyIdForName(
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
    /* [in] */ IUri* srcuri)
{
    Int64 rowId;
    String rawName_ = rawName;
    if (rawName_.IsNull() || rawName_.GetLength() == 0) {
       rawName_ = IMediaStore::UNKNOWN_STRING;
    }
    AutoPtr<IMediaStoreAudio> msa;
    CMediaStoreAudio::AcquireSingleton((IMediaStoreAudio**)&msa);
    String k;
    msa->KeyFor(rawName_, &k);

    if (k.IsNull()) {
       // shouldn't happen, since we only get null keys for null inputs
       Logger::E(TAG, "null key");
       return -1;
    }

    Boolean isAlbum = table.Equals("albums");
    Boolean isUnknown = IMediaStore::UNKNOWN_STRING.Equals(rawName_);//

    // To distinguish same-named albums, we append a hash. The hash is based
    // on the "album artist" tag if present, otherwise on the "compilation" tag
    // if present, otherwise on the path.
    // Ideally we would also take things like CDDB ID in to account, so
    // we can group files from the same album that aren't in the same
    // folder, but this is a quick and easy start that works immediately
    // without requiring support from the mp3, mp4 and Ogg meta data
    // readers, as long as the albums are in different folders.
    StringBuilder sb(k);
    if (isAlbum) {
        sb += albumHash;
        if (isUnknown) {
            sb += artist;
        }
    }
    k = sb.ToString();

    AutoPtr<ArrayOf<String> > selargs = ArrayOf<String>::Alloc(1);
    (*selargs)[0] = k;
    helper->mNumQueries++;
    String nullStr;
    AutoPtr<ICursor> c;
    db->Query(table, NULL, keyField + "=?", selargs, nullStr, nullStr, nullStr, (ICursor**)&c);//

    // try {
    Int32 count;
    c->GetCount(&count);
    Boolean flag = FALSE;
        switch (count) {
        case 0: {
            // insert new entry into table
            AutoPtr<IContentValues> otherValues;
            CContentValues::New((IContentValues**)&otherValues);
            otherValues->Put(keyField, k);
            otherValues->Put(nameField, rawName_);
            helper->mNumInserts++;
            db->Insert(table, String("duration"), otherValues, &rowId);
            if (path.IsNull() && isAlbum && ! isUnknown) {
            // We just inserted a new album. Now create an album art thumbnail for it.
            MakeThumbAsync(helper, db, path, rowId);
            }
            if (rowId > 0) {
            String volume;
            String str = Object::ToString(srcuri);
            str.Substring(16, 24); // extract internal/external
            AutoPtr<IUri> uri;
            AutoPtr<IUriHelper> uh;
            CUriHelper::AcquireSingleton((IUriHelper**)&uh);
            sb.Reset(); sb += "content://media/"; sb += volume; sb += "/audio/"; sb += table; sb += "/"; sb += rowId;
            uh->Parse(sb.ToString(), (IUri**)&uri);
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IContentResolver> resolver;
            context->GetContentResolver((IContentResolver**)&resolver);
            resolver->NotifyChange(uri, NULL);
            }
            break;
        }

        case 1: {
            // Use the existing entry
            c->MoveToFirst(&flag);
            c->GetInt64(0, &rowId);

            // Determine whether the current rawName is better than what's
            // currently stored in the table, and update the table if it is.
            String currentFancyName;
            c->GetString(2, &currentFancyName);
            String bestName = MakeBestName(rawName_, currentFancyName);
            if (!bestName.Equals(currentFancyName)) {
                // update the table with the new name
                AutoPtr<IContentValues> newValues;
                CContentValues::New((IContentValues**)&newValues);
                newValues->Put(nameField, bestName);
                helper->mNumUpdates++;
                Int32 vol;
                sb.Reset(); sb += "rowid="; sb += rowId;
                db->Update(table, newValues, sb.ToString(), NULL, &vol);
                String volume;
                String tmp = Object::ToString(srcuri);
                tmp.Substring(16, 24); // extract internal/external
                AutoPtr<IUri> uri;
                AutoPtr<IUriHelper> uh;
                CUriHelper::AcquireSingleton((IUriHelper**)&uh);
                sb.Reset(); sb += "content://media/"; sb += volume; sb += "/audio/";
                sb += table; sb += "/"; sb += rowId;
                uh->Parse(sb.ToString(), (IUri**)&uri);
                AutoPtr<IContext> context;
                GetContext((IContext**)&context);
                AutoPtr<IContentResolver> resolver;
                context->GetContentResolver((IContentResolver**)&resolver);
                resolver->NotifyChange(uri, NULL);
            }
            break;
        }

        default:
            // corrupt database
            Logger::E(TAG, "Multiple entries in table %s for key %s", table.string(), k.string());
            rowId = -1;
            break;
    }
    // } finally {
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(c));
    // }

    if (cache.GetSize() == 0 && ! isUnknown) {
        cache[cacheName] = rowId;
    }
    return rowId;
}

String MediaProvider::MakeBestName(
    /* [in] */ const String& one,
    /* [in] */ const String& two)
{
    String name;

    // Longer names are usually better.
    if (one.GetLength() > two.GetLength()) {
       name = one;
    }
    else {
       // Names with accents are usually better, and conveniently sort later
       if (one.ToLowerCase().Compare(two.ToLowerCase()) > 0) {
           name = one;
       } else {
           name = two;
       }
    }

    // Prefixes are better than postfixes.
    if (name.EndWith(", the") || name.EndWith(",the") ||
        name.EndWith(", an") || name.EndWith(",an") ||
        name.EndWith(", a") || name.EndWith(",a")) {
        String fix = name.Substring(1 + name.LastIndexOf(','));
        StringBuilder sb(fix.Trim()); sb += " ";
        sb += name.Substring(0, name.LastIndexOf(','));
        name = sb.ToString();
    }

    // TODO: word-capitalize the resulting name
    return name;
}

void MediaProvider::WriteAlbumArt(
    /* [in] */ Boolean need_to_recompress,
    /* [in] */ IUri* out,
    /* [in] */ ArrayOf<Byte>* compressed,
    /* [in] */ IBitmap* bm)
{
    AutoPtr<IOutputStream> outstream;

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

   AutoPtr<IContext> context;
   GetContext((IContext**)&context);
   AutoPtr<IContentResolver> resolver;
   context->GetContentResolver((IContentResolver**)&resolver);
   resolver->OpenOutputStream(out, (IOutputStream**)&outstream);

   if (!need_to_recompress) {
       // No need to recompress here, just write out the original
       // compressed data here.
       outstream->Write(compressed);
   }
   else {
       Boolean flag = FALSE;
       bm->Compress(BitmapCompressFormat_JPEG, 85, outstream, &flag);
       if (!flag) {
           Logger::E(TAG, "failed to compress bitmap");
           ioUtils->CloseQuietly(ICloseable::Probe(outstream));
           // return E_IO_EXCEPTION;
           return;
       }
   }

   ioUtils->CloseQuietly(ICloseable::Probe(outstream));
}

AutoPtr<IParcelFileDescriptor> MediaProvider::GetThumb(
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& path,
    /* [in] */ Int64 album_id,
    /* [in] */ IUri* albumart_uri)
{
    AutoPtr<ThumbData> d = new ThumbData();
    d->mHelper = helper;
    d->mDb = db;
    d->mPath = path;
    d->mAlbum_id = album_id;
    d->mAlbumart_uri = albumart_uri;
    return MakeThumbInternal(d);
}

AutoPtr<IParcelFileDescriptor> MediaProvider::MakeThumbInternal(
    /* [in] */ ThumbData* d)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ArrayOf<Byte> > compressed = GetCompressedAlbumArt(context, d->mPath);

    if (compressed == NULL) {
       return NULL;
    }

    AutoPtr<IBitmap> bm;
    Boolean need_to_recompress = TRUE;

    // get the size of the bitmap
    AutoPtr<IBitmapFactoryOptions> opts;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opts);

    opts->SetInJustDecodeBounds(TRUE);
    opts->SetInSampleSize(1);
    AutoPtr<IBitmapFactory> bitMapFactory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bitMapFactory);
    Int32 length = compressed->GetLength();
    AutoPtr<IBitmap> bitmap;
    bitMapFactory->DecodeByteArray(compressed, 0, length, opts, (IBitmap**)&bitmap);

    // request a reasonably sized output image
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    Int32 maximumThumbSize;
    r->GetDimensionPixelSize(R::dimen::maximum_thumb_size, &maximumThumbSize);
    Int32 oHeight, oWidth, iSampleSize;
    while((opts->GetOutHeight(&oHeight), oHeight > maximumThumbSize)
        || (opts->GetOutWidth(&oWidth), oWidth > maximumThumbSize)) {
        opts->SetOutHeight(oHeight/= 2);
        opts->SetOutWidth(oWidth /= 2);
        opts->GetInSampleSize(&iSampleSize);
        opts->SetInSampleSize(iSampleSize * 2);
    }

    opts->GetInSampleSize(&iSampleSize);
    if (iSampleSize == 1) {
       // The original album art was of proper size, we won't have to
       // recompress the bitmap later.
       need_to_recompress = FALSE;
    }
    else {
        // get the image for real now
        opts->SetInJustDecodeBounds(FALSE);
        opts->SetInPreferredConfig(BitmapConfig_RGB_565);
        bitMapFactory->DecodeByteArray(compressed, 0, length,
            IBitmapFactoryOptions::Probe(opts), (IBitmap**)&bm);//

        Int32 bc;
        bm->GetConfig(&bc);//

        if (bm != NULL && bc == 0) {
            AutoPtr<IBitmap> nbm;
            bm->Copy(BitmapConfig_RGB_565, FALSE, (IBitmap**)&nbm);
            if (nbm != NULL && nbm != bm) {
                bm->Recycle();
                bm = nbm;
            }
        }
    }

    if (need_to_recompress && bm == NULL) {
        return NULL;
    }

    if (d->mAlbumart_uri == NULL) {
       // this one doesn't need to be saved (probably a song with an unknown album),
       // so stick it in a memory file and return that
        AutoPtr<IParcelFileDescriptorHelper> pfds;
        CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfds);
        AutoPtr<IParcelFileDescriptor> opfd;
        pfds->FromData(compressed, String("albumthumb"), (IParcelFileDescriptor**)&opfd);
        return opfd;
    }
    else {
        // This one needs to actually be saved on the sd card.
        // This is wrapped in a transaction because there are various things
        // that could go wrong while generating the thumbnail, and we only want
        // to update the database when all steps succeeded.
        d->mDb->BeginTransaction();
        AutoPtr<IUri> out;
        AutoPtr<IParcelFileDescriptor> pfd;
        // try {
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        out = GetAlbumArtOutputUri(d->mHelper, d->mDb, d->mAlbum_id, d->mAlbumart_uri);//

        if (out != NULL) {
           WriteAlbumArt(need_to_recompress, out, compressed, bm);
           resolver->NotifyChange(MEDIA_URI, NULL);
           OpenFileHelper(out, String("r"), (IParcelFileDescriptor**)&pfd);
           d->mDb->SetTransactionSuccessful();
           return pfd;
        }
       // } catch (IOException ex) {
           // do nothing, just return null below
       // } catch (UnsupportedOperationException ex) {
           // do nothing, just return null below
       // } finally {
        d->mDb->EndTransaction();
        if (bm != NULL) {
            bm->Recycle();
        }
        if (pfd == NULL && out != NULL) {
            // Thumbnail was not written successfully, delete the entry that refers to it.
            // Note that this only does something if getAlbumArtOutputUri() reused an
            // existing entry from the database. If a new entry was created, it will
            // have been rolled back as part of backing out the transaction.
            Int32 vol;
            resolver->Delete(out, String(NULL), NULL, &vol);
        }
       // }
    }
    return NULL;
}

AutoPtr<IUri> MediaProvider::AttachVolume(
    /* [in] */ const String& volume)
{
    if (LOCAL_LOGV) Logger::V(TAG, "AttachVolume %s", volume.string());
    Int32 callingPid = Binder::GetCallingPid();
    Int32 myPid = Process::MyPid();
    if (callingPid != myPid) {
       Logger::E(TAG, "Opening and closing databases not allowed.");
       return NULL;
    }

    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    {
        AutoLock syncLock(mDatabases);
        Boolean flag = FALSE;
        AutoPtr<IUri> uri;
        AutoPtr<IInterface> obj;
        mDatabases->Get(CoreUtils::Convert(volume), (IInterface**)&obj);
        AutoPtr<IDatabaseHelper> dbh = IDatabaseHelper::Probe(obj);
        if (dbh != NULL) {  // Already attached
            uh->Parse(String("content://media/") + volume, (IUri**)&uri);
            return uri;
        }

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<DatabaseHelper> helper;
        if (INTERNAL_VOLUME.Equals(volume)) {
            helper = new DatabaseHelper();
            helper->constructor(this, context, INTERNAL_DATABASE_NAME, TRUE,
                   FALSE, mObjectRemovedCallback);

        }
        else if (EXTERNAL_VOLUME.Equals(volume)) {
            String dbName;
            if (Environment::IsExternalStorageRemovable()) {
                AutoPtr<IStorageVolume> actualVolume;
                mStorageManager->GetPrimaryVolume((IStorageVolume**)&actualVolume);
                Int32 volumeId;
                actualVolume->GetFatVolumeId(&volumeId);//

                // Must check for failure!
                // If the volume is not (yet) mounted, this will create a new
                // external-ffffffff.db database instead of the one we expect.  Then, if
                // android.process.media is later killed and respawned, the real external
                // database will be attached, containing stale records, or worse, be empty.
                if (volumeId == -1) {
                    String state = Environment::GetExternalStorageState();
                    if (Environment::MEDIA_MOUNTED.Equals(state) ||
                            Environment::MEDIA_MOUNTED_READ_ONLY.Equals(state)) {
                        // This may happen if external storage was _just_ mounted.  It may also
                        // happen if the volume ID is _actually_ 0xffffffff, in which case it
                        // must be changed since FileUtils::getFatVolumeId doesn't allow for
                        // that.  It may also indicate that FileUtils::getFatVolumeId is broken
                        // (missing ioctl), which is also impossible to disambiguate.
                        Logger::E(TAG, "Can't obtain external volume ID even though it's mounted.");
                    } else {
                        Logger::I(TAG, "External volume is not (yet) mounted, cannot attach.");
                    }

                    Logger::E(TAG, "Can't obtain external volume ID for %s volume.", volume.string());
                }

                // generate database name based on volume ID
                StringBuilder sb("external-");
                sb += StringUtils::ToHexString(volumeId);
                sb += ".db";
                dbName = sb.ToString();
                helper = new DatabaseHelper();
                helper->constructor(this, context, dbName, FALSE, FALSE, mObjectRemovedCallback);
                mVolumeId = volumeId;
           }
           else {
               // external database name should be EXTERNAL_DATABASE_NAME
               // however earlier releases used the external-XXXXXXXX.db naming
               // for devices without removable storage, and in that case we need to convert
               // to this new convention
               AutoPtr<IFile> dbFile;
               context->GetDatabasePath(EXTERNAL_DATABASE_NAME, (IFile**)&dbFile);
               dbFile->Exists(&flag);
               if (!flag) {
                    // find the most recent external database and rename it to
                    // EXTERNAL_DATABASE_NAME, and delete any other older
                    // external database files
                    AutoPtr<IFile> recentDbFile;
                    AutoPtr<IFile> file;
                    AutoPtr<ArrayOf<String> > dblist;
                    context->GetDatabaseList((ArrayOf<String>**)&dblist);
                    if (dblist != NULL) {
                        Int32 dblength = dblist->GetLength();
                        String database;
                        for(Int32 i = 0; i < dblength; i++) {
                            database = (*dblist)[i];
                            if (database.StartWith("external-") && database.EndWith(".db")) {
                                file = NULL;
                                context->GetDatabasePath(database, (IFile**)&file);
                                Int64 flf, lmf;
                                file->GetLastModified(&flf);
                                recentDbFile->GetLastModified(&lmf);
                                if (recentDbFile == NULL) {
                                    recentDbFile = file;
                                }
                                else if (flf > lmf) {
                                    recentDbFile->GetName(&dbName);
                                    context->DeleteDatabase(dbName, &flag);
                                    recentDbFile = file;
                                }
                                else {
                                    file->GetName(&dbName);
                                    context->DeleteDatabase(dbName, &flag);
                                }
                            }
                        }
                    }

                   if (recentDbFile != NULL) {
                        recentDbFile->RenameTo(dbFile, &flag);
                        if (flag) {
                           recentDbFile->GetName(&dbName);
                           Logger::D(TAG, "renamed database %s to %s", dbName.string(), EXTERNAL_DATABASE_NAME.string());
                        }
                        else {
                            recentDbFile->GetName(&dbName);
                            Logger::E(TAG, "Failed to rename database %s to %s", dbName.string(), EXTERNAL_DATABASE_NAME.string());
                            // This shouldn't happen, but if it does, continue using
                            // the file under its old name
                            dbFile = recentDbFile;
                        }
                    }
                   // else DatabaseHelper will create one named EXTERNAL_DATABASE_NAME
               }

                dbFile->GetName(&dbName);
                helper = new DatabaseHelper();
                helper->constructor(this, context, dbName, FALSE, FALSE, mObjectRemovedCallback);
            }
        } else {
            Logger::E(TAG, "There is no volume named %s", volume.string());
            return NULL;
        }

        mDatabases->Put(CoreUtils::Convert(volume), (IDatabaseHelper*)helper.Get());

        if (!helper->mInternal) {
            // create default directories (only happens on first boot)
            AutoPtr<ISQLiteDatabase> sdb;
            helper->GetWritableDatabase((ISQLiteDatabase**)&sdb);
            CreateDefaultFolders(helper, sdb);//

            // clean up stray album art files: delete every file not in the database
            AutoPtr<IFile> f;
            CFile::New((*mExternalStoragePaths)[0], ALBUM_THUMB_FOLDER, (IFile**)&f);
            AutoPtr<ArrayOf<IFile*> > files;
            f->ListFiles((ArrayOf<IFile*>**)&files);//

            HashSet<String> fileSet;
            if (files != NULL) {
                Int32 flength = files->GetLength();
                String hsPath;
                for (Int32 i = 0; files != NULL && i < flength; i++) {
                    (*files)[i]->GetPath(&hsPath);
                    fileSet.Insert(hsPath);
                }
            }

            String nullStr;
            AutoPtr<ICursor> cursor;
            AutoPtr<ArrayOf<String> > qarr = ArrayOf<String>::Alloc(1);
            (*qarr)[0] = IMediaStoreAudioAlbumColumns::ALBUM_ART;//

            AutoPtr<IMediaStoreAudioAlbums> msaa;
            CMediaStoreAudioAlbums::AcquireSingleton((IMediaStoreAudioAlbums**)&msaa);
            AutoPtr<IUri> quri;
            msaa->GetEXTERNAL_CONTENT_URI((IUri**)&quri);
            Query(quri, qarr, nullStr, NULL, nullStr, (ICursor**)&cursor);
           // try {
            if (cursor != NULL) {
                String arrStr;
                while ((cursor->MoveToNext(&flag), flag)) {
                    cursor->GetString(0, &arrStr);
                    fileSet.Erase(arrStr);
                }
            }
            AutoPtr<IIoUtils> ioUtils;
            CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));

            HashSet<String>::Iterator it = fileSet.Begin();
            String fileName;
            for (; it != fileSet.End(); ++it) {
                fileName = *it;
                if (LOCAL_LOGV) Logger::V(TAG, "deleting obsolete album art %s", fileName.string());
                AutoPtr<IFile> tFile;
                CFile::New(fileName, (IFile**)&tFile);
                tFile->Delete();
            }
        }
    }

    if (LOCAL_LOGV) Logger::V(TAG, "Attached volume: %s", volume.string());
    AutoPtr<IUri> iiuri;
    uh->Parse(String("content://media/") + volume, (IUri**)&iiuri);
    return iiuri;
}

ECode MediaProvider::GetDatabaseForUri(
    /* [in] */ IUri* uri,
    /* [out] */ IDatabaseHelper** result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock syncLock(mDatabases);
    AutoPtr<IList> segments;
    uri->GetPathSegments((IList**)&segments);
    if (segments != NULL) {
        Int32 size;
        segments->GetSize(&size);
        if (size >= 1) {
            AutoPtr<IInterface> obj, value;
            segments->Get(0, (IInterface**)&obj);
            mDatabases->Get(obj, (IInterface**)&value);
            *result = IDatabaseHelper::Probe(value);
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    *result = NULL;
    return NOERROR;
}

Boolean MediaProvider::IsMediaDatabaseName(
    /* [in] */ const String& name)
{
    if (INTERNAL_DATABASE_NAME.Equals(name)) {
       return TRUE;
    }
    if (EXTERNAL_DATABASE_NAME.Equals(name)) {
       return TRUE;
    }
    if (name.StartWith("external-") && name.EndWith(".db")) {
       return TRUE;
    }
    return FALSE;
}

Boolean MediaProvider::IsInternalMediaDatabaseName(
    /* [in] */ const String& name)
{
    if (INTERNAL_DATABASE_NAME.Equals(name)) {
       return TRUE;
    }
    return FALSE;
}

void MediaProvider::DetachVolume(
    /* [in]*/ IUri* uri)
{
    if (Binder::GetCallingPid() != Process::MyPid()) {
       Logger::E(TAG, "Opening and closing databases not allowed.");
       return;
    }

    AutoPtr<IList> list;
    uri->GetPathSegments((IList**)&list);
    AutoPtr<IInterface> obj;
    list->Get(0, (IInterface**)&obj);
    String volume = TO_STR(obj);
    if (INTERNAL_VOLUME.Equals(volume)) {
       Logger::E(TAG, "Deleting the internal volume is not allowed");
       return;
    }
    else if (!EXTERNAL_VOLUME.Equals(volume)) {
       Logger::E(TAG, "There is no volume named %s", volume.string());
       return;
    }

    {
        AutoLock syncLock(mDatabases);
        AutoPtr<IInterface> obj;
        mDatabases->Get(CoreUtils::Convert(volume), (IInterface**)&obj);
        AutoPtr<IDatabaseHelper> database = IDatabaseHelper::Probe(obj);
        if (database == NULL) return;

        // touch the database file to show it is most recently used
        AutoPtr<ISQLiteDatabase> db;
        ((DatabaseHelper*)database.Get())->GetReadableDatabase((ISQLiteDatabase**)&db);
        String path;
        db->GetPath(&path);
        AutoPtr<IFile> file;
        CFile::New(path, (IFile**)&file);
        AutoPtr<ISystem> isystem;
        CSystem::AcquireSingleton((ISystem**)&isystem);
        Int64 timeMillis;
        isystem->GetCurrentTimeMillis(&timeMillis);
        Boolean flag = FALSE;
        file->SetLastModified(timeMillis, &flag);

        mDatabases->Remove(CoreUtils::Convert(volume));
        ISQLiteOpenHelper::Probe(database)->Close();
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->NotifyChange(uri, NULL);
    if (LOCAL_LOGV) Logger::V(TAG, "Detached volume: %s", volume.string());
}

String MediaProvider::GetVolumeName(
    /* [in] */ IUri* uri)
{
    AutoPtr<IList> segments;
    uri->GetPathSegments((IList**)&segments);
    Int32 size;
    if (segments != NULL && (segments->GetSize(&size), size > 0)) {
        AutoPtr<IInterface> obj;
        segments->Get(0, (IInterface**)&obj);
        return TO_STR(obj);
    }
    else {
       return String(NULL);
    }
}

ECode MediaProvider::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<ICollection> foo;
    mDatabases->GetValues((ICollection**)&foo);

    AutoPtr<IIterator> it;
    foo->GetIterator((IIterator**)&it);

    Boolean flag = FALSE;
    AutoPtr<IInterface> next;
    AutoPtr<IDatabaseHelper> dbh;
    while(it->HasNext(&flag), flag) {
        next = NULL;
        it->GetNext((IInterface**)&next);
        dbh = IDatabaseHelper::Probe(next);
        writer->Println(Dump(dbh, TRUE));
    }

    return IFlushable::Probe(writer)->Flush();
}

String MediaProvider::Dump(
    /* [in] */ IDatabaseHelper* dbh,
    /* [in] */ Boolean dumpDbLog)
{
    DatabaseHelper* helper = (DatabaseHelper*)dbh;
    StringBuilder sb;
    sb.Append(helper->mName);
    sb.Append(": ");
    AutoPtr<ISQLiteDatabase> db;
    helper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> c;
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    if (db == NULL) {
       sb.Append("NULL");
    }
    else {
        Int32 version;
        db->GetVersion(&version);
        sb += "version";
        sb += version;
        sb += ", ";
        AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
        (*arr)[0] = String("count(*)");
        db->Query(String("files"), arr, String(NULL), NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
        // try {
        Boolean flag = FALSE;
        if (c != NULL && (c->MoveToFirst(&flag), flag)) {
           Int32 num;
           c->GetInt32(0, &num);
           sb += num;
           sb += " rows, ";
        } else {
           sb.Append("couldn't get row count, ");
        }
        // } finally {
        // }

        sb += helper->mNumInserts; sb += " inserts, ";
        sb += helper->mNumUpdates; sb += " updates, ";
        sb += helper->mNumDeletes; sb += " deletes, ";
        sb += helper->mNumQueries; sb += " queries, ";
        if (helper->mScanStartTime != 0) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            String tmp = DateUtils::FormatDateTime(context,
                helper->mScanStartTime / 1000,
                IDateUtils::FORMAT_SHOW_DATE
                    | IDateUtils::FORMAT_SHOW_TIME
                    | IDateUtils::FORMAT_ABBREV_ALL);

            sb += "scan started ";
            sb += tmp;
            Int64 now = helper->mScanStopTime;
            if (now < helper->mScanStartTime) {
                now = SystemClock::GetCurrentTimeMicro();
            }//

            tmp = DateUtils::FormatElapsedTime((now - helper->mScanStartTime) / 1000000);
            sb += " (";
            sb += tmp;
            sb += ")";
            if (helper->mScanStopTime < helper->mScanStartTime) {
                if (!mMediaScannerVolume.IsNull() &&
                       helper->mName.StartWith(mMediaScannerVolume)) {
                    sb.Append(" (ongoing)");
                }
                else {
                    sb += " (scanning ";
                    sb += mMediaScannerVolume;
                    sb += ")";
                }
            }
        }
        if (dumpDbLog) {
            AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
            (*array)[0] = String("time");
            (*array)[1] = String("message");
            AutoPtr<ICursor> cur;
            db->Query(String("log"), array,
                   String(NULL), NULL, String(NULL), String(NULL), String("rowid"), (ICursor**)&cur);
            if (cur != NULL) {
                Boolean bMoved = FALSE;
                while ((cur->MoveToNext(&bMoved), bMoved)) {
                    String when;
                    cur->GetString(0, &when);
                    String msg;
                    cur->GetString(1, &msg);
                    sb += "\n";
                    sb += when;
                    sb += " : ";
                    sb += msg;
                }
                ioUtils->CloseQuietly(ICloseable::Probe(cur));
            }
        }
    }
    if (c != NULL) {
        ioUtils->CloseQuietly(ICloseable::Probe(c));
    }
    return sb.ToString();
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos
