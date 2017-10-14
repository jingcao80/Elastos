//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_MEDIA_CMEDIASCANNER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIASCANNER_H__

#include "_Elastos_Droid_Media_CMediaScanner.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Sax.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <media/mediascanner.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Sax::IStartElementListener;
using Elastos::Droid::Sax::IEndElementListener;
using Elastos::Droid::Sax::IElementListener;
using Org::Xml::Sax::IContentHandler;
using Org::Xml::Sax::IAttributes;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaScanner)
    , public Object
    , public IMediaScanner
{
public:
    class ElementListener
        : public Object
        , public IElementListener
        , public IStartElementListener
        , public IEndElementListener
    {
    public:
        CAR_INTERFACE_DECL()

        ElementListener(
            /* [in] */ CMediaScanner* host,
            /* [in] */ const String& playListDirectory);

        CARAPI Start(
           /* [in] */ IAttributes* attributes);

        //@Override
        CARAPI End();

    private:
        CMediaScanner* mHost;
        String mPlayListDirectory;
    };

    class WplHandler
        : public Object
    {
    public:
        WplHandler(
            /* [in] */ CMediaScanner* host,
            /* [in] */ const String& playListDirectory,
            /* [in] */ IUri* uri,
            /* [in] */ ICursor* fileList);

        CARAPI_(AutoPtr<IContentHandler>) GetContentHandler();

    private:
        CMediaScanner* mHost;
        AutoPtr<IContentHandler> mHandler;
    };

    /*static*/ class FileEntry : public Object
    {
    public:
        FileEntry(
            /* [in] */ Int64 rowId,
            /* [in] */ const String& path,
            /* [in] */ Int64 lastModified,
            /* [in] */ Int32 format);

        using Object::ToString;

        CARAPI_(String) ToString();

    public:
        Int64 mRowId;
        String mPath;
        Int64 mLastModified;
        Int32 mFormat;
        Boolean mLastModifiedChanged;
    };

    class MyMediaScannerClient
        : public Object
        , public IMediaScannerClient
    {
    public:
        CAR_INTERFACE_DECL()

        MyMediaScannerClient(
            /* [in] */ CMediaScanner* owner);

        CARAPI_(AutoPtr<FileEntry>) BeginFile(
            /* [in] */ const String& path,
            /* [in] */ const String& mimeType,
            /* [in] */ Int64 lastModified,
            /* [in] */ Int64 fileSize,
            /* [in] */ Boolean isDirectory,
            /* [in] */ Boolean noMedia);

        CARAPI ScanFile(
            /* [in] */ const String& path,
            /* [in] */ Int64 lastModified,
            /* [in] */ Int64 fileSize,
            /* [in] */ Boolean isDirectory,
            /* [in] */ Boolean noMedia);

        CARAPI_(AutoPtr<IUri>) DoScanFile(
            /* [in] */ const String& path,
            /* [in] */ const String& mimeType,
            /* [in] */ Int64 lastModified,
            /* [in] */ Int64 fileSize,
            /* [in] */ Boolean isDirectory,
            /* [in] */ Boolean scanAlways,
            /* [in] */ Boolean noMedia) ;

        CARAPI HandleStringTag(
            /* [in] */ const String& name,
            /* [in] */ const String& value);

        CARAPI GetGenreName(
            /* [in] */ const String& genreTagValue,
            /* [out] */ String* result);

        CARAPI SetMimeType(
            /* [in] */ const String& mimeType);

    private:
        CARAPI_(Int32) ParseSubstring(
            /* [in] */ const String& s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 defaultValue);

        CARAPI_(Boolean) ConvertGenreCode(
            /* [in] */ const String& input,
            /* [in] */ const String& expected);

        CARAPI_(void) TestGenreNameConverter();

        CARAPI_(void) ProcessImageFile(
            /* [in] */ const String& path);

        /**
         * Formats the data into a values array suitable for use with the Media
         * Content Provider.
         *
         * @return a map of values
         */
        CARAPI_(AutoPtr<IContentValues>) ToValues();

        CARAPI_(AutoPtr<IUri>) EndFile(
            /* [in] */ FileEntry* entry,
            /* [in] */ Boolean ringtones,
            /* [in] */ Boolean notifications,
            /* [in] */ Boolean alarms,
            /* [in] */ Boolean music,
            /* [in] */ Boolean podcasts);

        CARAPI_(Boolean) RingtoneDefaultsSet();

        CARAPI_(Boolean) DoesPathHaveFilename(
            /* [in] */ const String& path,
            /* [in] */ const String& filename);

        CARAPI_(void) SetSettingIfNotSet(
            /* [in] */ const String& settingName,
            /* [in] */ IUri* uri,
            /* [in] */ Int64 rowId);

        CARAPI_(Int32) GetFileTypeFromDrm(
            /* [in] */ const String& path);

    private:
        String mArtist;
        String mAlbumArtist;    // use this if mArtist is missing
        String mAlbum;
        String mTitle;
        String mComposer;
        String mGenre;
        String mMimeType;
        Int32 mFileType;

        Int32 mTrack;
        Int32 mYear;
        Int32 mDuration;

        String mPath;
        Int64 mLastModified;
        Int64 mFileSize;
        String mWriter;
        Int32 mCompilation;

        Boolean mIsDrm;
        Boolean mNoMedia;   // flag to suppress file from appearing in media tables

        Int32 mWidth;
        Int32 mHeight;

        CMediaScanner* mHost;
    }; // end of anonymous MediaScannerClient instance

private:

    /*static*/ class PlaylistEntry : public Object
    {
    public:
        PlaylistEntry()
            : mBestmatchid(0)
            , mBestmatchlevel(0)
        {}

        String mPath;
        Int64 mBestmatchid;
        Int32 mBestmatchlevel;
    };
    class MediaBulkDeleter : public Object
    {
    public:
        MediaBulkDeleter(
            /* [in] */ IIContentProvider* provider,
            /* [in] */ const String& packageName,
            /* [in] */ IUri* baseUri);

        CARAPI Delete( // throws RemoteException
            /* [in] */ Int64 id);

        CARAPI Flush(); // throws RemoteException

    private:
        StringBuilder mWhereClause;
        List<String> mWhereArgs; // = new ArrayList<String>(100);
        AutoPtr<IIContentProvider> mProvider;
        String mPackageName;
        AutoPtr<IUri> mBaseUri;
    };

public:
    CMediaScanner();

    ~CMediaScanner();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* c);

    CARAPI ScanDirectories(
        /* [in] */ ArrayOf<String>* directories,
        /* [in] */ const String& volumeName);

    CARAPI ScanSingleFile(
        /* [in] */ const String& path,
        /* [in] */ const String& volumeName,
        /* [in] */ const String& mimeType,
        /* [out] */ IUri** result);

    CARAPI ScanMtpFile(
        /* [in] */ const String& path,
        /* [in] */ const String& volumeName,
        /* [in] */ Int32 objectHandle,
        /* [in] */ Int32 format);

    CARAPI SetLocale(
        /* [in] */ const String& locale);

    CARAPI ExtractAlbumArt(
        /* [in] */ IFileDescriptor* fd,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Releases resources associated with this MediaScanner object.
     * It is considered good practice to call this method when
     * one is done using the MediaScanner object. After this method
     * is called, the MediaScanner object can no longer be used.
     */
    CARAPI ReleaseReSources();

    static CARAPI IsNoMediaPath(
        /* [in] */ const String& path,
        /* [out] */ Boolean* result);

    CARAPI StopScan();

    static CARAPI ClearMediaPathCache(
        /* [in] */ Boolean clearMediaPaths,
        /* [in] */ Boolean clearNoMediaPaths);

protected:
    CARAPI_(void) Finalize();

private:
    CARAPI_(void) SetDefaultRingtoneFileNames();

    CARAPI_(Boolean) IsDrmEnabled();

    CARAPI Prescan(  //throws RemoteException
        /* [in] */ const String& filePath,
        /* [in] */ Boolean prescanFiles);

    CARAPI_(Boolean) InScanDirectory(
        /* [in] */ const String& path,
        /* [in] */ ArrayOf<String>* directories);

    CARAPI_(void) PruneDeadThumbnailFiles();

    CARAPI Postscan( // throws RemoteException
        /* [in] */ ArrayOf<String>* directories);

    CARAPI Initialize(
        /* [in] */ const String& volumeName);

    CARAPI_(void) StartScan();

    CARAPI_(void) ExitScan();

    static CARAPI_(Boolean) IsNoMediaFile(
        /* [in] */ const String& path);

    CARAPI_(AutoPtr<FileEntry>) MakeEntryFor(
        /* [in] */ const String& path);

    // returns the number of matching file/directory names, starting from the right
    CARAPI_(Int32) MatchPaths(
        /* [in] */ const String& path1,
        /* [in] */ const String& path2);

    CARAPI_(Boolean) MatchEntries(
        /* [in] */ Int64 rowId,
        /* [in] */ const String& data);

    CARAPI_(void) CachePlaylistEntry(
        /* [in] */ const String& line,
        /* [in] */ const String& playListDirectory);

    CARAPI ProcessCachedPlaylist(
        /* [in] */ ICursor* fileList,
        /* [in] */ IContentValues* values,
        /* [in] */ IUri* playlistUri);

    CARAPI_(void) ProcessM3uPlayList(
        /* [in] */ const String& path,
        /* [in] */ const String& playListDirectory,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ ICursor* fileList);

    CARAPI_(void) ProcessPlsPlayList(
        /* [in] */ const String& path,
        /* [in] */ const String& playListDirectory,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ ICursor* fileList);

    CARAPI_(void) ProcessWplPlayList(
        /* [in] */ const String& path,
        /* [in] */ const String& playListDirectory,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ ICursor* fileList);

    CARAPI ProcessPlayList( //throws RemoteException
        /* [in] */ FileEntry* entry,
        /* [in] */ ICursor* fileList);

    CARAPI ProcessPlayLists(); //throws RemoteException

    CARAPI ProcessDirectory(
        /* [in] */ const String& path,
        /* [in] */ IMediaScannerClient* client);

    CARAPI ProcessFile(
        /* [in] */ const String& path,
        /* [in] */ const String& mimeType,
        /* [in] */ IMediaScannerClient* client);

    CARAPI NativeSetup();

    CARAPI_(void) NativeFinalize();

    static CARAPI_(Boolean) InitStatic();

private:
    const static String TAG; // = "MediaScanner";

    static AutoPtr< ArrayOf<String> > FILES_PRESCAN_PROJECTION;
    static AutoPtr< ArrayOf<String> > ID_PROJECTION;

    static const Int32 FILES_PRESCAN_ID_COLUMN_INDEX;
    static const Int32 FILES_PRESCAN_PATH_COLUMN_INDEX;
    static const Int32 FILES_PRESCAN_FORMAT_COLUMN_INDEX;
    static const Int32 FILES_PRESCAN_DATE_MODIFIED_COLUMN_INDEX;

    static AutoPtr< ArrayOf<String> > PLAYLIST_MEMBERS_PROJECTION;

    static const Int32 ID_PLAYLISTS_COLUMN_INDEX = 0;
    static const Int32 PATH_PLAYLISTS_COLUMN_INDEX = 1;
    static const Int32 DATE_MODIFIED_PLAYLISTS_COLUMN_INDEX = 2;

    static const String RINGTONES_DIR; // = "/ringtones/";
    static const String NOTIFICATIONS_DIR; // = "/notifications/";
    static const String ALARMS_DIR; // = "/alarms/";
    static const String MUSIC_DIR; // = "/music/";
    static const String PODCAST_DIR; // = "/podcasts/";

    static AutoPtr< ArrayOf<String> > ID3_GENRES;

    android::MediaScanner* mNativeContext;

    AutoPtr<IContext> mContext;
    String mPackageName;
    AutoPtr<IIContentProvider> mMediaProvider;

    AutoPtr<IUri> mAudioUri;
    AutoPtr<IUri> mVideoUri;
    AutoPtr<IUri> mImagesUri;
    AutoPtr<IUri> mThumbsUri;
    AutoPtr<IUri> mPlaylistsUri;
    AutoPtr<IUri> mFilesUri;
    AutoPtr<IUri> mFilesUriNoNotify;
    Boolean mProcessPlaylists;
    Boolean mProcessGenres;
    Handle32 mMtpObjectHandle;

    String mExternalStoragePath;
    Boolean mExternalIsEmulated;

    /** whether to use bulk inserts or individual inserts for each item */
    static const Boolean ENABLE_BULK_INSERTS; // = TRUE;

    // used when scanning the image database so we know whether we have to prune
    // old thumbnail files
    Int32 mOriginalCount;
    /** Whether the database had any entries in it before the scan started */
    Boolean mWasEmptyPriorToScan; // = FALSE;
    /** Whether the scanner has set a default sound for the ringer ringtone. */
    AutoPtr< ArrayOf<Boolean> > mDefaultRingtonesSet;
    /** Whether the scanner has set a default sound for the notification ringtone. */
    Boolean mDefaultNotificationSet;
    /** Whether the scanner has set a default sound for the alarm ringtone. */
    Boolean mDefaultAlarmSet;
    /** The filenames for the default sound for the ringer ringtone. */
    AutoPtr< ArrayOf<String> > mDefaultRingtoneFilenames;
    /** The filename for the default sound for the notification ringtone. */
    String mDefaultNotificationFilename;
    /** The filename for the default sound for the alarm ringtone. */
    String mDefaultAlarmAlertFilename;

    /**
     * The prefix for system properties that define the default sound for
     * ringtones. Concatenate the name of the setting from Settings
     * to get the full system property.
     */
    static const String DEFAULT_RINGTONE_PROPERTY_PREFIX; // "ro.config.";

    static const Int32 DEFAULT_SIM_INDEX = 0;

    // set to true if file path comparisons should be case insensitive.
    // this should be set when scanning files on a case insensitive file system.
    Boolean mCaseInsensitivePaths;

    AutoPtr<IBitmapFactoryOptions> mBitmapOptions; // = new BitmapFactory.Options();

    List< AutoPtr<PlaylistEntry> > mPlaylistEntries; // = new ArrayList<PlaylistEntry>();

    AutoPtr<IMediaInserter> mMediaInserter;

    List< AutoPtr<FileEntry> > mPlayLists;

    // AutoPtr<IDrmManagerClient> mDrmManagerClient; // = NULL;

    AutoPtr<MyMediaScannerClient> mClient; // = new MyMediaScannerClient();

    Boolean mStopScan; // = FALSE; //jni will read it to check if need stop scan

    static Object  mLock;

    Int32 mScanCount; // = 0;
    static HashMap<String, String> mNoMediaPaths;
    static HashMap<String, String> mMediaPaths;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIASCANNER_H__
