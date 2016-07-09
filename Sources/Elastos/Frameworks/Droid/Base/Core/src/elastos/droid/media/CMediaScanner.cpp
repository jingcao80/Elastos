
#include "elastos/droid/media/CMediaScanner.h"
#include "elastos/droid/media/CMediaFile.h"
#include "elastos/droid/media/CExifInterface.h"
#include "elastos/droid/media/CMediaInserter.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/mtp/CMtpConstants.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/provider/CMediaStoreVideoMedia.h"
#include "elastos/droid/provider/CMediaStoreAudioMedia.h"
#include "elastos/droid/provider/CMediaStoreImagesMedia.h"
#include "elastos/droid/provider/CMediaStoreFiles.h"
#include "elastos/droid/provider/CMediaStoreImagesThumbnails.h"
#include "elastos/droid/provider/CMediaStoreAudioPlaylists.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/sax/CRootElement.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include <elastos/droid/utility/Xml.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/Character.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Logger.h>
#include <media/stagefright/StagefrightMediaScanner.h>
#include <elastos/droid/system/OsConstants.h>

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Mtp::IMtpConstants;
using Elastos::Droid::Mtp::CMtpConstants;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::CMediaStoreVideoMedia;
using Elastos::Droid::Provider::CMediaStoreAudioMedia;
using Elastos::Droid::Provider::CMediaStoreImagesMedia;
using Elastos::Droid::Provider::CMediaStoreFiles;
using Elastos::Droid::Provider::CMediaStoreImagesThumbnails;
using Elastos::Droid::Provider::CMediaStoreAudioPlaylists;
using Elastos::Droid::Provider::IMediaStoreAudioPlaylists;
using Elastos::Droid::Provider::IMediaStoreAudioAudioColumns;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreAudioPlaylistsMembers;
using Elastos::Droid::Provider::IMediaStoreFiles;
using Elastos::Droid::Provider::IMediaStoreFilesFileColumns;
using Elastos::Droid::Provider::IMediaStoreImagesMedia;
using Elastos::Droid::Provider::IMediaStoreImagesThumbnails;
using Elastos::Droid::Provider::IMediaStoreImagesImageColumns;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Provider::IMediaStoreVideoMedia;
using Elastos::Droid::Provider::IMediaStoreVideoVideoColumns;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Utility::IXml;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Sax::CRootElement;
using Elastos::Droid::Sax::EIID_IEndElementListener;
using Elastos::Droid::Sax::EIID_IStartElementListener;
using Elastos::Droid::Sax::IElement;
using Elastos::Droid::Sax::IElementListener;
using Elastos::Droid::Sax::IEndElementListener;
using Elastos::Droid::Sax::IRootElement;
using Elastos::Droid::Sax::IStartElementListener;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CFile;
using Elastos::IO::CFileHelper;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileHelper;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::IReader;
using Elastos::Core::AutoLock;
using Elastos::Core::CBoolean;
using Elastos::Core::CFloat;
using Elastos::Core::Character;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CSystem;
using Elastos::Core::CString;
using Elastos::Core::ISystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Droid::System::OsConstants;
using Libcore::IO::CLibcore;
using Libcore::IO::ILibcore;
using Libcore::IO::IOs;

namespace Elastos {
namespace Droid {
namespace Media {

static AutoPtr< ArrayOf<String> > InitFILES_PRESCAN_PROJECTION()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    array->Set(0, IMediaStoreAudioPlaylistsMembers::ID);
    array->Set(1, IMediaStoreMediaColumns::DATA);
    array->Set(2, IMediaStoreFilesFileColumns::FORMAT);
    array->Set(3, IMediaStoreMediaColumns::DATE_MODIFIED);
    return array;
}

static AutoPtr< ArrayOf<String> > InitID_PROJECTION()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, IMediaStoreAudioPlaylistsMembers::ID);
    return array;
}

static AutoPtr< ArrayOf<String> > InitPLAYLIST_MEMBERS_PROJECTION()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, IMediaStoreAudioPlaylistsMembers::PLAYLIST_ID);
    return array;
}

static AutoPtr< ArrayOf<String> > InitID3_GENRES()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(148);

    //ID3v1 Genres
    array->Set(0, String("Blues"));
    array->Set(1, String("Classic Rock"));
    array->Set(2, String("Country"));
    array->Set(3, String("Dance"));
    array->Set(4, String("Disco"));
    array->Set(5, String("Funk"));
    array->Set(6, String("Grunge"));
    array->Set(7, String("Hip-Hop"));
    array->Set(8, String("Jazz"));
    array->Set(9, String("Metal"));
    array->Set(10, String("New Age"));
    array->Set(11, String("Oldies"));
    array->Set(12, String("Other"));
    array->Set(13, String("Pop"));
    array->Set(14, String("R&B"));
    array->Set(15, String("Rap"));
    array->Set(16, String("Reggae"));
    array->Set(17, String("Rock"));
    array->Set(18, String("Techno"));
    array->Set(19, String("Industrial"));
    array->Set(20, String("Alternative"));
    array->Set(21, String("Ska"));
    array->Set(22, String("Death Metal"));
    array->Set(23, String("Pranks"));
    array->Set(24, String("Soundtrack"));
    array->Set(25, String("Euro-Techno"));
    array->Set(26, String("Ambient"));
    array->Set(27, String("Trip-Hop"));
    array->Set(28, String("Vocal"));
    array->Set(29, String("Jazz+Funk"));
    array->Set(30, String("Fusion"));
    array->Set(31, String("Trance"));
    array->Set(32, String("Classical"));
    array->Set(33, String("Instrumental"));
    array->Set(34, String("Acid"));
    array->Set(35, String("House"));
    array->Set(36, String("Game"));
    array->Set(37, String("Sound Clip"));
    array->Set(38, String("Gospel"));
    array->Set(39, String("Noise"));
    array->Set(40, String("AlternRock"));
    array->Set(41, String("Bass"));
    array->Set(42, String("Soul"));
    array->Set(43, String("Punk"));
    array->Set(44, String("Space"));
    array->Set(45, String("Meditative"));
    array->Set(46, String("Instrumental Pop"));
    array->Set(47, String("Instrumental Rock"));
    array->Set(48, String("Ethnic"));
    array->Set(49, String("Gothic"));
    array->Set(50, String("Darkwave"));
    array->Set(51, String("Techno-Industrial"));
    array->Set(52, String("Electronic"));
    array->Set(53, String("Pop-Folk"));
    array->Set(54, String("Eurodance"));
    array->Set(55, String("Dream"));
    array->Set(56, String("Southern Rock"));
    array->Set(57, String("Comedy"));
    array->Set(58, String("Cult"));
    array->Set(59, String("Gangsta"));
    array->Set(60, String("Top 40"));
    array->Set(61, String("Christian Rap"));
    array->Set(62, String("Pop/Funk"));
    array->Set(63, String("Jungle"));
    array->Set(64, String("Native American"));
    array->Set(65, String("Cabaret"));
    array->Set(66, String("New Wave"));
    array->Set(67, String("Psychadelic"));
    array->Set(68, String("Rave"));
    array->Set(69, String("Showtunes"));
    array->Set(70, String("Trailer"));
    array->Set(71, String("Lo-Fi"));
    array->Set(72, String("Tribal"));
    array->Set(73, String("Acid Punk"));
    array->Set(74, String("Acid Jazz"));
    array->Set(75, String("Polka"));
    array->Set(76, String("Retro"));
    array->Set(77, String("Musical"));
    array->Set(78, String("Rock & Roll"));
    array->Set(79, String("Hard Rock"));
    // The following genres are Winamp extensions
    array->Set(80, String("Folk"));
    array->Set(81, String("Folk-Rock"));
    array->Set(82, String("National Folk"));
    array->Set(83, String("Swing"));
    array->Set(84, String("Fast Fusion"));
    array->Set(85, String("Bebob"));
    array->Set(86, String("Latin"));
    array->Set(87, String("Revival"));
    array->Set(88, String("Celtic"));
    array->Set(89, String("Bluegrass"));
    array->Set(90, String("Avantgarde"));
    array->Set(91, String("Gothic Rock"));
    array->Set(92, String("Progressive Rock"));
    array->Set(93, String("Psychedelic Rock"));
    array->Set(94, String("Symphonic Rock"));
    array->Set(95, String("Slow Rock"));
    array->Set(96, String("Big Band"));
    array->Set(97, String("Chorus"));
    array->Set(98, String("Easy Listening"));
    array->Set(99, String("Acoustic"));
    array->Set(100, String("Humour"));
    array->Set(101, String("Speech"));
    array->Set(102, String("Chanson"));
    array->Set(103, String("Opera"));
    array->Set(104, String("Chamber Music"));
    array->Set(105, String("Sonata"));
    array->Set(106, String("Symphony"));
    array->Set(107, String("Booty Bass"));
    array->Set(108, String("Primus"));
    array->Set(109, String("Porn Groove"));
    array->Set(110, String("Satire"));
    array->Set(111, String("Slow Jam"));
    array->Set(112, String("Club"));
    array->Set(113, String("Tango"));
    array->Set(114, String("Samba"));
    array->Set(115, String("Folklore"));
    array->Set(116, String("Ballad"));
    array->Set(117, String("Power Ballad"));
    array->Set(118, String("Rhythmic Soul"));
    array->Set(119, String("Freestyle"));
    array->Set(120, String("Duet"));
    array->Set(121, String("Punk Rock"));
    array->Set(122, String("Drum Solo"));
    array->Set(123, String("A capella"));
    array->Set(124, String("Euro-House"));
    array->Set(125, String("Dance Hall"));
    // The following ones seem to be fairly widely supported as well
    array->Set(126, String("Goa"));
    array->Set(127, String("Drum & Bass"));
    array->Set(128, String("Club-House"));
    array->Set(129, String("Hardcore"));
    array->Set(130, String("Terror"));
    array->Set(131, String("Indie"));
    array->Set(132, String("Britpop"));
    array->Set(133, String(NULL));
    array->Set(134, String("Polsk Punk"));
    array->Set(135, String("Beat"));
    array->Set(136, String("Christian Gangsta"));
    array->Set(137, String("Heavy Metal"));
    array->Set(138, String("Black Metal"));
    array->Set(139, String("Crossover"));
    array->Set(140, String("Contemporary Christian"));
    array->Set(141, String("Christian Rock"));
    array->Set(142, String("Merengue"));
    array->Set(143, String("Salsa"));
    array->Set(144, String("Thrash Metal"));
    array->Set(145, String("Anime"));
    array->Set(146, String("JPop"));
    array->Set(147, String("Synthpop"));
    //148 and up don't seem to have been defined yet.

    return array;
}

AutoPtr< ArrayOf<String> > CMediaScanner::FILES_PRESCAN_PROJECTION = InitFILES_PRESCAN_PROJECTION();
AutoPtr< ArrayOf<String> > CMediaScanner::ID_PROJECTION = InitID_PROJECTION();
AutoPtr< ArrayOf<String> > CMediaScanner::PLAYLIST_MEMBERS_PROJECTION = InitPLAYLIST_MEMBERS_PROJECTION();

static const Boolean DBG = TRUE;
const String CMediaScanner::TAG("MediaScanner");

const Int32 CMediaScanner::FILES_PRESCAN_ID_COLUMN_INDEX = 0;
const Int32 CMediaScanner::FILES_PRESCAN_PATH_COLUMN_INDEX = 1;
const Int32 CMediaScanner::FILES_PRESCAN_FORMAT_COLUMN_INDEX = 2;
const Int32 CMediaScanner::FILES_PRESCAN_DATE_MODIFIED_COLUMN_INDEX = 3;

const String CMediaScanner::RINGTONES_DIR("/ringtones/");
const String CMediaScanner::NOTIFICATIONS_DIR("/notifications/");
const String CMediaScanner::ALARMS_DIR("/alarms/");
const String CMediaScanner::MUSIC_DIR("/music/");
const String CMediaScanner::PODCAST_DIR("/podcasts/");

AutoPtr< ArrayOf<String> > CMediaScanner::ID3_GENRES = InitID3_GENRES();

const Boolean CMediaScanner::ENABLE_BULK_INSERTS = TRUE;
const String CMediaScanner::DEFAULT_RINGTONE_PROPERTY_PREFIX("ro.config.");
const Int32 CMediaScanner::DEFAULT_SIM_INDEX;
Object CMediaScanner::mLock;
HashMap<String, String> CMediaScanner::mNoMediaPaths;
HashMap<String, String> CMediaScanner::mMediaPaths;

//-----------------------------
//    CMediaScanner::WplHandler
//-----------------------------
CAR_INTERFACE_IMPL_2(CMediaScanner::WplHandler, Object, IStartElementListener, IEndElementListener)

CMediaScanner::WplHandler::WplHandler(
    /* [in] */ const String& playListDirectory,
    /* [in] */ IUri* uri,
    /* [in] */ ICursor* fileList)
    : mPlayListDirectory(playListDirectory)
{

    AutoPtr<IRootElement> root;
    CRootElement::New(String("smil"), (IRootElement**)&root);

    AutoPtr<IElement> body, seq, media;
    IElement::Probe(root)->GetChild(String("body"), (IElement**)&body);
    IElement::Probe(root)->GetChild(String("seq"), (IElement**)&seq);
    IElement::Probe(root)->GetChild(String("media"), (IElement**)&media);

    media->SetElementListener(IElementListener::Probe(this));

    root->GetContentHandler((IContentHandler**)&mHandler);
}

ECode CMediaScanner::WplHandler::Start(
   /* [in] */ IAttributes* attributes)
{
   String path;
   attributes->GetValue(String(""), String("src"), &path);
   if (!path.IsNull()) {
       mHost->CachePlaylistEntry(path, mPlayListDirectory);
   }
   return NOERROR;
}

ECode CMediaScanner::WplHandler::End()
{
    return NOERROR;
}

AutoPtr<IContentHandler> CMediaScanner::WplHandler::GetContentHandler()
{
   return mHandler;
}

//----------------------------
//    CMediaScanner::FileEntry
//----------------------------


CMediaScanner::FileEntry::FileEntry(
    /* [in] */ Int64 rowId,
    /* [in] */ const String& path,
    /* [in] */ Int64 lastModified,
    /* [in] */ Int32 format)
    : mRowId(rowId)
    , mPath(path)
    , mLastModified(lastModified)
    , mFormat(format)
    , mLastModifiedChanged(FALSE)
{}

String CMediaScanner::FileEntry::ToString()
{
    String tempText;
    tempText.AppendFormat("%s mRowId: %lld", mPath.string(), mRowId);
    return tempText;
}

//---------------------------------------
//    CMediaScanner::MyMediaScannerClient
//---------------------------------------

CAR_INTERFACE_IMPL(CMediaScanner::MyMediaScannerClient, Object, IMediaScannerClient);

CMediaScanner::MyMediaScannerClient::MyMediaScannerClient(
    /* [in] */ CMediaScanner* owner)
    : mFileType(0)
    , mTrack(0)
    , mYear(0)
    , mDuration(0)
    , mLastModified(0)
    , mFileSize(0)
    , mCompilation(0)
    , mIsDrm(0)
    , mNoMedia(0)
    , mWidth(0)
    , mHeight(0)
    , mHost(owner)
{}

AutoPtr<CMediaScanner::FileEntry> CMediaScanner::MyMediaScannerClient::BeginFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType,
    /* [in] */ Int64 lastModified,
    /* [in] */ Int64 fileSize,
    /* [in] */ Boolean isDirectory,
    /* [in] */ Boolean noMedia)
{
    mMimeType = mimeType;
    mFileType = 0;
    mFileSize = fileSize;
    mIsDrm = FALSE;

    if (!isDirectory) {
        if (!noMedia && IsNoMediaFile(path)) {
            noMedia = TRUE;
        }
        mNoMedia = noMedia;

        AutoPtr<IMediaFile> mediaFile;
        CMediaFile::AcquireSingleton((IMediaFile**)&mediaFile);

        // try mimeType first, if it is specified
        if (mimeType != NULL) {
            mediaFile->GetFileTypeForMimeType(mimeType, &mFileType);
        }

        // if mimeType was not specified, compute file type based on file extension.
        if (mFileType == 0) {
            AutoPtr<IMediaFileType> mediaFileType;
            mediaFile->GetFileType(path, (IMediaFileType**)&mediaFileType);

            if (mediaFileType != NULL) {
                mediaFileType->GetFileType(&mFileType);
                if (mMimeType == NULL) {
                    mediaFileType->GetMimeType(&mMimeType);
                }
            }
        }


        Boolean tempState;
        mediaFile->IsDrmFileType(mFileType, &tempState);
        if (mHost->IsDrmEnabled() && tempState) {
            mFileType = GetFileTypeFromDrm(path);
        }
    }

    AutoPtr<FileEntry> entry = mHost->MakeEntryFor(path);
    // add some slack to avoid a rounding error
    Int64 delta = (entry != NULL) ? (lastModified - entry->mLastModified) : 0;
    Boolean wasModified = delta > 1 || delta < -1;
    if (entry == NULL || wasModified) {
        if (wasModified) {
            entry->mLastModified = lastModified;
        }
        else {
            entry = new FileEntry(0, path, lastModified,
                    (isDirectory ? (Int64)IMtpConstants::FORMAT_ASSOCIATION : 0));
        }
        entry->mLastModifiedChanged = TRUE;
    }

    AutoPtr<IMediaFile> mediaFile;
    CMediaFile::AcquireSingleton((IMediaFile**)&mediaFile);
    Boolean tempState;
    mediaFile->IsPlayListFileType(mFileType, &tempState);
    if (mHost->mProcessPlaylists && tempState) {
        mHost->mPlayLists.PushBack(entry);
        // we don't process playlists in the main scan, so return null
        return NULL;
    }

    // clear all the metadata
    mArtist = NULL;
    mAlbumArtist = NULL;
    mAlbum = NULL;
    mTitle = NULL;
    mComposer = NULL;
    mGenre = NULL;
    mTrack = 0;
    mYear = 0;
    mDuration = 0;
    mPath = path;
    mLastModified = lastModified;
    mWriter = NULL;
    mCompilation = 0;
    mWidth = 0;
    mHeight = 0;

    return entry;
}

ECode CMediaScanner::MyMediaScannerClient::ScanFile(
    /* [in] */ const String& path,
    /* [in] */ Int64 lastModified,
    /* [in] */ Int64 fileSize,
    /* [in] */ Boolean isDirectory,
    /* [in] */ Boolean noMedia)
{
    // This is the callback funtion from native codes.
    String nullStr;
    DoScanFile(path, nullStr, lastModified, fileSize, isDirectory, FALSE, noMedia);
    return NOERROR;
}

AutoPtr<IUri> CMediaScanner::MyMediaScannerClient::DoScanFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType,
    /* [in] */ Int64 lastModified,
    /* [in] */ Int64 fileSize,
    /* [in] */ Boolean isDirectory,
    /* [in] */ Boolean scanAlways,
    /* [in] */ Boolean noMedia)
{
    if (DBG) {
        Logger::V(TAG, ">> ScanFile:[%s], mimeType:%s, isDirectory:%d, scanAlways:%d, noMedia:%d",
            path.string(), mimeType.string(), isDirectory, scanAlways, noMedia);
    }
    AutoPtr<IUri> result = NULL;
    String dpath = path;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 t1;
    sys->GetCurrentTimeMillis(&t1);

    AutoPtr<FileEntry> entry = BeginFile(dpath, mimeType, lastModified, fileSize, isDirectory, noMedia);

    // if this file was just inserted via mtp, set the rowid to zero
    // (even though it already exists in the database), to trigger
    // the correct code path for updating its entry
    if (mHost->mMtpObjectHandle != 0 && entry != NULL) {
        entry->mRowId = 0;
    }

    // rescan for metadata if file was modified since last scan
    if (entry != NULL && (entry->mLastModifiedChanged || scanAlways)) {
        if (noMedia) {
            result = EndFile(entry, FALSE, FALSE, FALSE, FALSE, FALSE);
        }
        else {
            String lowpath = dpath.ToLowerCase();
            Boolean ringtones = (lowpath.IndexOf(RINGTONES_DIR) > 0);
            Boolean notifications = (lowpath.IndexOf(NOTIFICATIONS_DIR) > 0);
            Boolean alarms = (lowpath.IndexOf(ALARMS_DIR) > 0);
            Boolean podcasts = (lowpath.IndexOf(PODCAST_DIR) > 0);
            Boolean music = (lowpath.IndexOf(MUSIC_DIR) > 0) ||
                (!ringtones && !notifications && !alarms && !podcasts);

            AutoPtr<IMediaFile> mediaFile;
            CMediaFile::AcquireSingleton((IMediaFile**)&mediaFile);
            Boolean isaudio, isvideo, isimage;
            mediaFile->IsAudioFileType(mFileType, &isaudio);
            mediaFile->IsVideoFileType(mFileType, &isvideo);
            mediaFile->IsImageFileType(mFileType, &isimage);
            if (isaudio || isvideo || isimage) {
                if (mHost->mExternalIsEmulated && dpath.StartWith(mHost->mExternalStoragePath)) {
                    // try to rewrite the path to bypass the sd card fuse layer
                    AutoPtr<IFile> file = Environment::GetExternalStorageDirectory();
                    String directPath;
                    file->ToString(&directPath);
                    directPath += dpath.Substring(mHost->mExternalStoragePath.GetLength());
                    AutoPtr<IFile> f;
                    CFile::New(directPath,(IFile**)&f);
                    Boolean tempState;
                    if ((f->Exists(&tempState), tempState)) {
                        dpath = directPath;
                    }
                }
            }

            // we only extract metadata for audio and video files
            if (isaudio || isvideo) {
                mHost->ProcessFile(dpath, mimeType, this);
            }

            if (isimage) {
                ProcessImageFile(dpath);
            }

            result = EndFile(entry, ringtones, notifications, alarms, music, podcasts);
        }
    }

    Int64 t2;
    sys->GetCurrentTimeMillis(&t2);
    if (DBG) {
        Logger::V(TAG, " << ScanFile:[%s], mMimeType:%s, mFileType:%d, took %lld ms.",
            path.string(), mMimeType.string(), mFileType, t2-t1);
    }
    return result;
}

ECode CMediaScanner::MyMediaScannerClient::HandleStringTag(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (name.EqualsIgnoreCase("title") || name.StartWith("title;")) {
        // Don't trim() here, to preserve the special \001 character
        // used to force sorting. The media provider will trim() before
        // inserting the title in to the database.
        mTitle = value;
    }
    else if (name.EqualsIgnoreCase("artist") || name.StartWith("artist;")) {
        mArtist = value.Trim();
    }
    else if (name.EqualsIgnoreCase("albumartist") || name.StartWith("albumartist;")
        || name.EqualsIgnoreCase("band") || name.StartWith("band;")) {
        mAlbumArtist = value.Trim();
    }
    else if (name.EqualsIgnoreCase("album") || name.StartWith("album;")) {
        mAlbum = value.Trim();
    }
    else if (name.EqualsIgnoreCase("composer") || name.StartWith("composer;")) {
        mComposer = value.Trim();
    }
    else if (mHost->mProcessGenres && (name.EqualsIgnoreCase("genre") || name.StartWith("genre;"))) {
        GetGenreName(value, &mGenre);
    }
    else if (name.EqualsIgnoreCase("year") || name.StartWith("year;")) {
        mYear = ParseSubstring(value, 0, 0);
    }
    else if (name.EqualsIgnoreCase("tracknumber") || name.StartWith("tracknumber;")) {
        // track number might be of the form "2/12"
        // we just read the number before the slash
        Int32 num = ParseSubstring(value, 0, 0);
        mTrack = (mTrack / 1000) * 1000 + num;
    }
    else if (name.EqualsIgnoreCase("discnumber") ||
            name.Equals("set") || name.StartWith("set;")) {
        // set number might be of the form "1/3"
        // we just read the number before the slash
        Int32 num = ParseSubstring(value, 0, 0);
        mTrack = (num * 1000) + (mTrack % 1000);
    }
    else if (name.EqualsIgnoreCase("duration")) {
        mDuration = ParseSubstring(value, 0, 0);
    }
    else if (name.EqualsIgnoreCase("writer") || name.StartWith("writer;")) {
        mWriter = value.Trim();
    }
    else if (name.EqualsIgnoreCase("compilation")) {
        mCompilation = ParseSubstring(value, 0, 0);
    }
    else if (name.EqualsIgnoreCase("isdrm")) {
        mIsDrm = (ParseSubstring(value, 0, 0) == 1);
    }
    else if (name.EqualsIgnoreCase("width")) {
        mWidth = ParseSubstring(value, 0, 0);
    }
    else if (name.EqualsIgnoreCase("height")) {
        mHeight = ParseSubstring(value, 0, 0);
    }
    else {
        Logger::V(TAG, "unknown tag: name:%s, value:%s,  (%d)",
            name.string(), value.string(), mHost->mProcessGenres);
    }
    return NOERROR;
}

ECode CMediaScanner::MyMediaScannerClient::GetGenreName(
    /* [in] */ const String& genreTagValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    if (genreTagValue == NULL) {
        *result = NULL;
        return NOERROR;
    }
    Int32 length = genreTagValue.GetLength();

    if (length > 0) {
        Boolean parenthesized = FALSE;
        StringBuffer number;

        Int32 i = 0;
        for (; i < length; ++i) {
            AutoPtr<ICharSequence> charSequence;
            CString::New(genreTagValue, (ICharSequence**)&charSequence);
            Char32 c;
            charSequence->GetCharAt(i, &c);
            if (i == 0 && c == '(') {
                parenthesized = TRUE;
            }
            else if (Character::IsDigit(c)) {
                number += c;
            }
            else {
                break;
            }
        }
        AutoPtr<ICharSequence> charSequence;
        CString::New(genreTagValue, (ICharSequence**)&charSequence);
        Char32 charAfterNumber;
        if (i < length) {
            charSequence->GetCharAt(i, &charAfterNumber);
        }
        else {
            charAfterNumber = ' ';
        }
        if ((parenthesized && (charAfterNumber == ')'))
                || (!parenthesized) && (Character::IsWhitespace(charAfterNumber))) {
//            try {
                String tempText;
                number.ToString(&tempText);

               Int16 genreIndex = (Int16)tempText.GetLength();
               if (genreIndex >= 0) {
                   if (genreIndex < ID3_GENRES->GetLength()) {
                       *result = (*ID3_GENRES)[genreIndex];
                       return NOERROR;
                   }
                   else if (genreIndex == 0xFF) {
                       *result = NULL;
                       return NOERROR;
                   }
                   else if (genreIndex < 0xFF && (i + 1) < length) {
                       // genre is valid but unknown,
                       // if there is a string after the value we take it
                       if (parenthesized && charAfterNumber == ')') {
                           i++;
                       }
                       String ret = genreTagValue.Substring(i).Trim();
                       if (!ret.IsNullOrEmpty()) {
                           *result = ret;
                            return NOERROR;
                       }
                   }
                   else {
                       // else return the number, without parentheses
                       number.ToString(result);
                       return NOERROR;
                   }
               }
//            } catch (NumberFormatException e) {
//            }
        }
    }
    *result = genreTagValue;
    return NOERROR;
}

ECode CMediaScanner::MyMediaScannerClient::SetMimeType(
    /* [in] */ const String& mimeType)
{
    if (mMimeType.Equals("audio/mp4") && mimeType.StartWith("video")) {
        // for feature parity with Donut, we force m4a files to keep the
        // audio/mp4 mimetype, even if they are really "enhanced podcasts"
        // with a video track
        return NOERROR;
    }
    mMimeType = mimeType;
    AutoPtr<IMediaFile> mediaFile;
    CMediaFile::AcquireSingleton((IMediaFile**)&mediaFile);
    mediaFile->GetFileTypeForMimeType(mimeType, &mFileType);
    return NOERROR;
}

Int32 CMediaScanner::MyMediaScannerClient::ParseSubstring(
    /* [in] */ const String& s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 defaultValue)
{
    AutoPtr<ArrayOf<Char32> > chars = s.GetChars();
    Int32 length = chars->GetLength();
    if (start == length) {
        return defaultValue;
    }

    Char32 ch = (*chars)[start++];
    // return defaultValue if we have no integer at all
    if (ch < '0' || ch > '9') {
        return defaultValue;
    }

    Int32 result = ch - '0';
    while (start < length) {
        ch = (*chars)[start++];
        if (ch < '0' || ch > '9') {
            return result;
        }
        result = result * 10 + (ch - '0');
    }

    return result;
}

Boolean CMediaScanner::MyMediaScannerClient::ConvertGenreCode(
    /* [in] */ const String& input,
    /* [in] */ const String& expected)
{
    String output;
    GetGenreName(input, &output);
    if (output.Equals(expected)) {
        return TRUE;
    }
    else {
        Logger::D(TAG, "input '%s' -> output '%s', expected '%s'", input.string(), output.string(), expected.string());
        return FALSE;
    }
}

void CMediaScanner::MyMediaScannerClient::TestGenreNameConverter()
{
    ConvertGenreCode( String("2") , String("Country") );
    ConvertGenreCode( String("(2)"), String("Country") );
    ConvertGenreCode( String("(2"), String("(2") );
    ConvertGenreCode( String("2 Foo"), String("Country") );
    ConvertGenreCode( String("(2) Foo"), String("Country") );
    ConvertGenreCode( String("(2 Foo"), String("(2 Foo") );
    ConvertGenreCode( String("2Foo"), String("2Foo") );
    ConvertGenreCode( String("(2)Foo"), String("Country") );
    ConvertGenreCode( String("200 Foo"), String("Foo") );
    ConvertGenreCode( String("(200) Foo"), String("Foo") );
    ConvertGenreCode( String("200Foo"), String("200Foo") );
    ConvertGenreCode( String("(200)Foo"), String("Foo") );
    ConvertGenreCode( String("200)Foo"), String("200)Foo") );
    ConvertGenreCode( String("200) Foo"), String("200) Foo") );
}

void CMediaScanner::MyMediaScannerClient::ProcessImageFile(
    /* [in] */ const String& path)
{
//    try {
        mHost->mBitmapOptions->SetOutWidth(0);
        mHost->mBitmapOptions->SetOutHeight(0);

        AutoPtr<IBitmapFactory> bf;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bf);
        AutoPtr<IBitmap> result;
        bf->DecodeFile(path, mHost->mBitmapOptions, (IBitmap**)&result);

        mHost->mBitmapOptions->GetOutWidth(&mWidth);
        mHost->mBitmapOptions->GetOutHeight(&mHeight);
//    } catch (Throwable th) {
//        // ignore;
//    }
}

AutoPtr<IContentValues> CMediaScanner::MyMediaScannerClient::ToValues()
{
    AutoPtr<IContentValues> map;
    CContentValues::New((IContentValues**)&map);

    map->Put(IMediaStoreMediaColumns::DATA, mPath);
    map->Put(IMediaStoreMediaColumns::TITLE, mTitle);
    map->Put(IMediaStoreMediaColumns::DATE_MODIFIED, mLastModified);
    map->Put(IMediaStoreMediaColumns::SIZE, mFileSize);
    map->Put(IMediaStoreMediaColumns::MIME_TYPE, mMimeType);
    map->Put(IMediaStoreMediaColumns::IS_DRM, mIsDrm);

    String resolution;
    if (mWidth > 0 && mHeight > 0) {
        map->Put(IMediaStoreMediaColumns::WIDTH, mWidth);
        map->Put(IMediaStoreMediaColumns::HEIGHT, mHeight);
        StringBuilder sb;
        sb += mWidth; sb += "x"; sb += mHeight;
        resolution = sb.ToString();
    }

    if (!mNoMedia) {
        AutoPtr<IMediaFile> mediaFile;
        CMediaFile::AcquireSingleton((IMediaFile**)&mediaFile);
        Boolean tempState;
        mediaFile->IsVideoFileType(mFileType, &tempState);
        if (tempState) {
            if (!mArtist.IsNullOrEmpty()) {
                map->Put(IMediaStoreVideoVideoColumns::ARTIST, mArtist);
            }
            else {
                map->Put(IMediaStoreVideoVideoColumns::ARTIST, IMediaStore::UNKNOWN_STRING);
            }

            if (!mAlbum.IsNullOrEmpty()) {
                map->Put(IMediaStoreVideoVideoColumns::ALBUM, mAlbum);
            }
            else {
                map->Put(IMediaStoreVideoVideoColumns::ALBUM, IMediaStore::UNKNOWN_STRING);
            }

            map->Put(IMediaStoreVideoVideoColumns::DURATION, mDuration);
            if (resolution != NULL) {
                map->Put(IMediaStoreVideoVideoColumns::RESOLUTION, resolution);
            }
        }
        else if ((mediaFile->IsImageFileType(mFileType, &tempState), tempState)) {
            // FIXME - add DESCRIPTION
        }
        else if ((mediaFile->IsAudioFileType(mFileType, &tempState), tempState)) {
            if (!mArtist.IsNullOrEmpty()) {
                map->Put(IMediaStoreAudioAudioColumns::ARTIST, mArtist);
            }
            else {
                map->Put(IMediaStoreAudioAudioColumns::ARTIST, IMediaStore::UNKNOWN_STRING);
            }

            if (!mAlbumArtist.IsNullOrEmpty()) {
                map->Put(IMediaStoreAudioAudioColumns::ALBUM_ARTIST, mAlbumArtist);
            }
            else {
                map->Put(IMediaStoreAudioAudioColumns::ALBUM_ARTIST, IMediaStore::UNKNOWN_STRING);
            }

            if (!mAlbum.IsNullOrEmpty()) {
                map->Put(IMediaStoreAudioAudioColumns::ALBUM, mAlbum);
            }
            else {
                map->Put(IMediaStoreAudioAudioColumns::ALBUM, IMediaStore::UNKNOWN_STRING);
            }

            map->Put(IMediaStoreAudioAudioColumns::COMPOSER, mComposer);
            map->Put(IMediaStoreAudioAudioColumns::GENRE, mGenre);

            if (mYear != 0) {
                map->Put(IMediaStoreAudioAudioColumns::YEAR, mYear);
            }
            map->Put(IMediaStoreAudioAudioColumns::TRACK, mTrack);
            map->Put(IMediaStoreAudioAudioColumns::DURATION, mDuration);
            map->Put(IMediaStoreAudioAudioColumns::COMPILATION, mCompilation);
        }
    }
    return map;
}

AutoPtr<IUri> CMediaScanner::MyMediaScannerClient::EndFile( // throws RemoteException
    /* [in] */ FileEntry* entry,
    /* [in] */ Boolean ringtones,
    /* [in] */ Boolean notifications,
    /* [in] */ Boolean alarms,
    /* [in] */ Boolean music,
    /* [in] */ Boolean podcasts)
{
    // update database

    // use album artist if artist is missing
    if (mArtist.IsNullOrEmpty()) {
        mArtist = mAlbumArtist;
    }

    AutoPtr<IContentValues> values = ToValues();
    String title;
    values->GetAsString(IMediaStoreMediaColumns::TITLE, &title);

    AutoPtr<IMediaFile> mediaFile;
    CMediaFile::AcquireSingleton((IMediaFile**)&mediaFile);
    if (title.IsNull() || TextUtils::IsEmpty(title.Trim())) {
        String tempText;
        values->GetAsString(IMediaStoreMediaColumns::DATA, &tempText);
        mediaFile->GetFileTitle(tempText, &title);
        values->Put(IMediaStoreMediaColumns::TITLE, title);
    }
    String album;
    values->GetAsString(IMediaStoreAudioAudioColumns::ALBUM, &album);
    if (IMediaStore::UNKNOWN_STRING.Equals(album)) {
        values->GetAsString(IMediaStoreMediaColumns::DATA, &album);
        // extract last path segment before file name
        Int32 lastSlash = album.LastIndexOf('/');
        if (lastSlash >= 0) {
            Int32 previousSlash = 0;
            while (TRUE) {
                Int32 idx = album.IndexOf('/', previousSlash + 1);
                if (idx < 0 || idx >= lastSlash) {
                    break;
                }
                previousSlash = idx;
            }
            if (previousSlash != 0) {
                album = album.Substring(previousSlash + 1, lastSlash);
                values->Put(IMediaStoreAudioAudioColumns::ALBUM, album);
            }
        }
    }
    Int64 rowId = entry->mRowId;

    Boolean tempState;
    mediaFile->IsAudioFileType(mFileType, &tempState);
    if (tempState && (rowId == 0 || mHost->mMtpObjectHandle != 0)) {
        // Only set these for new entries. For existing entries, they
        // may have been modified later, and we want to keep the current
        // values so that custom ringtones still show up in the ringtone
        // picker.
        values->Put(IMediaStoreAudioAudioColumns::IS_RINGTONE, ringtones);
        values->Put(IMediaStoreAudioAudioColumns::IS_NOTIFICATION, notifications);
        values->Put(IMediaStoreAudioAudioColumns::IS_ALARM, alarms);
        values->Put(IMediaStoreAudioAudioColumns::IS_MUSIC, music);
        values->Put(IMediaStoreAudioAudioColumns::IS_PODCAST, podcasts);
    }
    else if (mFileType == IMediaFile::FILE_TYPE_JPEG && !mNoMedia) {
        AutoPtr<IExifInterface> exif;
        CExifInterface::New(entry->mPath, (IExifInterface**)&exif);
        if (exif != NULL) {
            AutoPtr< ArrayOf<Float> > latlng = ArrayOf<Float>::Alloc(2);
            Boolean tempState;
            if ((exif->GetLatLong(latlng, &tempState), tempState)) {
                values->Put(IMediaStoreImagesImageColumns::LATITUDE, (*latlng)[0]);
                values->Put(IMediaStoreImagesImageColumns::LONGITUDE, (*latlng)[1]);
            }
            Int64 time;
            exif->GetGpsDateTime(&time);
            if (time != -1) {
                values->Put(IMediaStoreImagesImageColumns::DATE_TAKEN, time);
            }
            else {
                // If no time zone information is available, we should consider using
                // EXIF local time as taken time if the difference between file time
                // and EXIF local time is not less than 1 Day, otherwise MediaProvider
                // will use file time as taken time.
                exif->GetDateTime(&time);
                if (time != -1 && Elastos::Core::Math::Abs(mLastModified * 1000 - time) >= 86400000) {
                    values->Put(IMediaStoreImagesImageColumns::DATE_TAKEN, time);
                }
            }

            Int32 orientation;
            exif->GetAttributeInt32(IExifInterface::TAG_ORIENTATION, -1, &orientation);
            if (orientation != -1) {
                // We only recognize a subset of orientation tag values.
                Int32 degree = 0;
                switch(orientation) {
                case IExifInterface::ORIENTATION_ROTATE_90:
                    degree = 90;
                    break;
                case IExifInterface::ORIENTATION_ROTATE_180:
                    degree = 180;
                    break;
                case IExifInterface::ORIENTATION_ROTATE_270:
                    degree = 270;
                    break;
                default:
                    degree = 0;
                    break;
                }
                values->Put(IMediaStoreImagesImageColumns::ORIENTATION, degree);
            }
        }
    }

    AutoPtr<IUri> tableUri = mHost->mFilesUri;
    AutoPtr<IMediaInserter> inserter = mHost->mMediaInserter;
    if (!mNoMedia) {
        Boolean tempState;
        mediaFile->IsVideoFileType(mFileType, &tempState);
        if (tempState) {
            tableUri = mHost->mVideoUri;
        }
        else if ((mediaFile->IsImageFileType(mFileType, &tempState), tempState)) {
            tableUri = mHost->mImagesUri;
        }
        else if ((mediaFile->IsAudioFileType(mFileType, &tempState), tempState)) {
            tableUri = mHost->mAudioUri;
        }
    }

    AutoPtr<IUri> result = NULL;
    Boolean needToSetSettings = FALSE;
    if (rowId == 0) {
        if (mHost->mMtpObjectHandle != 0) {
            AutoPtr<IInteger32> interger32;
            CInteger32::New(mHost->mMtpObjectHandle, (IInteger32**)&interger32);
            values->Put(IMediaStoreMediaColumns::MEDIA_SCANNER_NEW_OBJECT_ID, interger32);
        }
        if (tableUri == mHost->mFilesUri) {
            Int32 format = entry->mFormat;
            if (format == 0) {
                mediaFile->GetFormatCode(entry->mPath, mMimeType, &format);
            }
            values->Put(IMediaStoreFilesFileColumns::FORMAT, format);
        }
        // Setting a flag in order not to use bulk insert for the file related with
        // notifications, ringtones, and alarms, because the rowId of the inserted file is
        // needed.
        if (mHost->mWasEmptyPriorToScan) {
            if (notifications && !mHost->mDefaultNotificationSet) {
                if (TextUtils::IsEmpty(mHost->mDefaultNotificationFilename) ||
                        DoesPathHaveFilename(entry->mPath, mHost->mDefaultNotificationFilename)) {
                    needToSetSettings = TRUE;
                }
            }
            else if (ringtones && !RingtoneDefaultsSet()) {
                AutoPtr<ITelephonyManager> defaultTelephonyManager;
                CTelephonyManager::GetDefault((ITelephonyManager**)&defaultTelephonyManager);
                if (defaultTelephonyManager) {
                    Int32 phoneCount = 0;
                    defaultTelephonyManager->GetPhoneCount(&phoneCount);
                    for (Int32 i = 0; i < phoneCount; ++i) {
                        String rin = (*(mHost->mDefaultRingtoneFilenames))[i];
                        // Check if ringtone matches default ringtone
                        if (TextUtils::IsEmpty(rin) ||
                                DoesPathHaveFilename(entry->mPath, rin)) {
                            needToSetSettings = TRUE;
                            break;
                        }
                    }
                }
            }
            else if (alarms && !mHost->mDefaultAlarmSet) {
                if (TextUtils::IsEmpty(mHost->mDefaultAlarmAlertFilename) ||
                        DoesPathHaveFilename(entry->mPath, mHost->mDefaultAlarmAlertFilename)) {
                    needToSetSettings = TRUE;
                }
            }
        }

        // New file, insert it.
        // Directories need to be inserted before the files they contain, so they
        // get priority when bulk inserting.
        // If the rowId of the inserted file is needed, it gets inserted immediately,
        // bypassing the bulk inserter.
        if (inserter == NULL || needToSetSettings) {
            if (inserter != NULL) {
                inserter->FlushAll();
            }
            mHost->mMediaProvider->Insert(mHost->mPackageName, tableUri, values, (IUri**)&result);
        }
        else if (entry->mFormat == (Int32)IMtpConstants::FORMAT_ASSOCIATION) {
            inserter->InsertwithPriority(tableUri, values);
        }
        else {
            inserter->Insert(tableUri, values);
        }

        if (result != NULL) {
            AutoPtr<IContentUris> contentUris;
            CContentUris::AcquireSingleton((IContentUris**)&contentUris);
            contentUris->ParseId(result, &rowId);
            entry->mRowId = rowId;
        }
    }
    else {
        // updated file
        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        contentUris->WithAppendedId(tableUri, rowId, (IUri**)&result);
        // path should never change, and we want to avoid replacing mixed cased paths
        // with squashed lower case paths
        values->Remove(IMediaStoreMediaColumns::DATA);

        Int32 mediaType = 0;
        Boolean tempState;
        if (!(IsNoMediaPath(entry->mPath, &tempState), tempState)) {
            Int32 fileType;
            mediaFile->GetFileTypeForMimeType(mMimeType, &fileType);
            if ((mediaFile->IsAudioFileType(fileType, &tempState), tempState)) {
                mediaType = IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO;
            }
            else if ((mediaFile->IsVideoFileType(fileType, &tempState), tempState)) {
                mediaType = IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO;
            }
            else if ((mediaFile->IsImageFileType(fileType, &tempState), tempState)) {
                mediaType = IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE;
            }
            else if ((mediaFile->IsPlayListFileType(fileType, &tempState), tempState)) {
                mediaType = IMediaStoreFilesFileColumns::MEDIA_TYPE_PLAYLIST;
            }
            values->Put(IMediaStoreFilesFileColumns::MEDIA_TYPE, mediaType);
        }
        Int32 tempValue;
        String nullStr;
        mHost->mMediaProvider->Update(mHost->mPackageName, result, values, nullStr, NULL, &tempValue);
    }

    if (needToSetSettings) {
        if (notifications) {
            SetSettingIfNotSet(ISettingsSystem::NOTIFICATION_SOUND, tableUri, rowId);
            mHost->mDefaultNotificationSet = TRUE;
        }
        else if (ringtones) {
            // memorize default system ringtone persistently
            SetSettingIfNotSet(ISettingsSystem::DEFAULT_RINGTONE, tableUri, rowId);
            AutoPtr<ITelephonyManager> defaultTelephonyManager;
            CTelephonyManager::GetDefault((ITelephonyManager**)&defaultTelephonyManager);
            if (defaultTelephonyManager != NULL) {
                Int32 phoneCount = 0;
                defaultTelephonyManager->GetPhoneCount(&phoneCount);
                String uri;
                for (Int32 i = 0; i < phoneCount; ++i) {
                    if ((*(mHost->mDefaultRingtonesSet))[i]) {
                        continue;
                    }

                    // Check if ringtone matches default ringtone
                    String rin = (*(mHost->mDefaultRingtoneFilenames))[i];
                    if (!TextUtils::IsEmpty(rin) &&
                            !DoesPathHaveFilename(entry->mPath, rin)) {
                        continue;
                    }
                    if (i == DEFAULT_SIM_INDEX) {
                        uri = ISettingsSystem::RINGTONE;
                    }
                    else {
                        StringBuilder sb(ISettingsSystem::RINGTONE); sb += "_"; sb += (i + 1);
                        uri = sb.ToString();
                    }

                    // Set default ringtone
                    SetSettingIfNotSet(uri, tableUri, rowId);
                    (*(mHost->mDefaultRingtonesSet))[i] = TRUE;
                }
            }
        }
        else if (alarms) {
            SetSettingIfNotSet(ISettingsSystem::ALARM_ALERT, tableUri, rowId);
            mHost->mDefaultAlarmSet = TRUE;
        }
    }
    return result;
}

Boolean CMediaScanner::MyMediaScannerClient::RingtoneDefaultsSet()
{
    // If not multisim, just check default sim's default
    AutoPtr<ITelephonyManager> defaultTelephonyManager;
    CTelephonyManager::GetDefault((ITelephonyManager**)&defaultTelephonyManager);
    Boolean isMultiSimEnabled = FALSE;
    defaultTelephonyManager->IsMultiSimEnabled(&isMultiSimEnabled);

    if (!isMultiSimEnabled) {
        return (*(mHost->mDefaultRingtonesSet))[DEFAULT_SIM_INDEX];
    }
    // Otherwise check if all defaults were accounted for
    Boolean defaultSet = FALSE;
    for (Int32 i=0; i<mHost->mDefaultRingtonesSet->GetLength(); ++i) {
        defaultSet = (*(mHost->mDefaultRingtonesSet))[i];
        if (!defaultSet) {
            return FALSE;
        }
    }
    return TRUE;
}

Boolean CMediaScanner::MyMediaScannerClient::DoesPathHaveFilename(
    /* [in] */ const String& path,
    /* [in] */ const String& filename)
{
    AutoPtr<IFileHelper> fileHelper;
    CFileHelper::AcquireSingleton((IFileHelper**)&fileHelper);
    Char32 tempChar;
    fileHelper->GetSeparatorChar(&tempChar);
    Int32 pathFilenameStart = path.LastIndexOf(tempChar) + 1;
    Int32 filenameLength = filename.GetLength();
    return path.RegionMatches(pathFilenameStart, filename, 0, filenameLength) &&
            pathFilenameStart + filenameLength == path.GetLength();
}

void CMediaScanner::MyMediaScannerClient::SetSettingIfNotSet(
    /* [in] */ const String& settingName,
    /* [in] */ IUri* uri,
    /* [in] */ Int64 rowId)
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);

    String value;
    Settings::System::GetString(cr, settingName, &value);
    if (TextUtils::IsEmpty(value)) {
        // Set the setting to the given URI
        cr = NULL;
        mHost->mContext->GetContentResolver((IContentResolver**)&cr);

        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        AutoPtr<IUri> uri;
        contentUris->WithAppendedId(uri, rowId, (IUri**)&uri);
        String tempText = Object::ToString(uri.Get());
        Boolean result;
        Settings::System::PutString(cr, settingName, tempText, &result);
    }
}

Int32 CMediaScanner::MyMediaScannerClient::GetFileTypeFromDrm(
    /* [in] */ const String& path)
{
    if (!mHost->IsDrmEnabled()) {
        return 0;
    }

    Int32 resultFileType = 0;
    assert(0 && "TODO");
/*Eddie(E_NOT_IMPLEMENTED)*/
//    if (mDrmManagerClient == NULL) {
//        //mDrmManagerClient = new DrmManagerClient(mContext);
//        CDrmManagerClient::New(mContext,(IDrmManagerClient**)&mDrmManagerClient);
//    }
//
//    if (mDrmManagerClient->CanHandle(path, NULL)) {
//        mIsDrm = TRUE;
//        String drmMimetype = mDrmManagerClient->GetOriginalMimeType(path);
//        if (drmMimetype != NULL) {
//            mMimeType = drmMimetype;
//            resultFileType = MediaFile->GetFileTypeForMimeType(drmMimetype);
//        }
//    }
    return resultFileType;
}

//-----------------------------------
//    CMediaScanner::MediaBulkDeleter
//-----------------------------------

CMediaScanner::MediaBulkDeleter::MediaBulkDeleter(
    /* [in] */ IIContentProvider* provider,
    /* [in] */ const String& packageName,
    /* [in] */ IUri* baseUri)
    : mProvider(provider)
    , mPackageName(packageName)
    , mBaseUri(baseUri)
{
}

ECode CMediaScanner::MediaBulkDeleter::Delete( // throws RemoteException
    /* [in] */ Int64 id)
{
    Int32 r;
    mWhereClause.GetLength(&r);
    if (r != 0) {
        mWhereClause += ",";
    }
    mWhereClause += "?";
    mWhereArgs.PushBack(StringUtils::ToString(id));
    if (mWhereArgs.GetSize() > 100) {
        Flush();
    }
    return NOERROR;
}

ECode CMediaScanner::MediaBulkDeleter::Flush() // throws RemoteException
{
    Int32 size = mWhereArgs.GetSize();
    if (size > 0) {
        List<String>::Iterator it = mWhereArgs.Begin();
        AutoPtr< ArrayOf<String> > foo = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; it != mWhereArgs.End(); ++it, ++i) {
            foo->Set(i, *it);
        }

        Int32 numrows;
        StringBuilder sb("_id"); sb += " IN ("; sb += mWhereClause.ToString(); sb += ")";
        mProvider->Delete(mPackageName, mBaseUri, sb.ToString(), foo, &numrows);
        Logger::V("CMediaScanner", "BulkDeleter %d rows deleted: ", numrows);
        mWhereClause.Reset();
        mWhereArgs.Clear();
    }
    return NOERROR;
}

//-----------------------------------
CMediaScanner::CMediaScanner()
    : mNativeContext(NULL)
    , mProcessPlaylists(FALSE)
    , mProcessGenres(FALSE)
    , mMtpObjectHandle(0)
    , mExternalIsEmulated(FALSE)
    , mOriginalCount(0)
    , mWasEmptyPriorToScan(FALSE)
    , mDefaultNotificationSet(FALSE)
    , mDefaultAlarmSet(FALSE)
    , mCaseInsensitivePaths(FALSE)
    , mStopScan(FALSE)
    , mScanCount(0)
{
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&mBitmapOptions);
/*Eddie(E_NOT_IMPLEMENTED)*/
//    mDrmManagerClient = NULL;
    mClient = new MyMediaScannerClient(this);
}

CMediaScanner::~CMediaScanner()
{
    Finalize();
}

CAR_INTERFACE_IMPL(CMediaScanner, Object, IMediaScanner)

CAR_OBJECT_IMPL(CMediaScanner)

ECode CMediaScanner::constructor(
    /* [in] */ IContext * c)
{
    FAIL_RETURN(NativeSetup());

    mContext = c;
    c->GetPackageName(&mPackageName);
    mBitmapOptions->SetInSampleSize(1);
    mBitmapOptions->SetInJustDecodeBounds(TRUE);

    SetDefaultRingtoneFileNames();

    AutoPtr<IFile> file;
    file = Environment::GetExternalStorageDirectory();
    file->GetAbsolutePath(&mExternalStoragePath);

    mExternalIsEmulated = Environment::IsExternalStorageEmulated();
    //mClient.testGenreNameConverter();
    return NOERROR;
}

ECode CMediaScanner::StopScan()
{
    {
        AutoLock lock(mLock);

        if (mScanCount > 0) {
            mStopScan = TRUE;
        }
    }
    return NOERROR;
}

ECode CMediaScanner::ClearMediaPathCache(
    /* [in] */ Boolean clearMediaPaths,
    /* [in] */ Boolean clearNoMediaPaths)
{
    {
        AutoLock lock(mLock);
        if (clearMediaPaths) {
            mMediaPaths.Clear();
        }
        if (clearNoMediaPaths) {
            mNoMediaPaths.Clear();
        }
    }
    return NOERROR;
}

void CMediaScanner::SetDefaultRingtoneFileNames()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String defaultAllSimRingtone;
    sp->Get(DEFAULT_RINGTONE_PROPERTY_PREFIX + ISettingsSystem::RINGTONE, &defaultAllSimRingtone);

    AutoPtr<ITelephonyManager> defaultTelephonyManager;
    CTelephonyManager::GetDefault((ITelephonyManager**)&defaultTelephonyManager);
    Int32 phoneCount = 0;
    defaultTelephonyManager->GetPhoneCount(&phoneCount);

    mDefaultRingtoneFilenames = ArrayOf<String>::Alloc(phoneCount);
    mDefaultRingtonesSet = ArrayOf<Boolean>::Alloc(phoneCount);

    String strTmp;
    SystemProperties::Get(DEFAULT_RINGTONE_PROPERTY_PREFIX + ISettingsSystem::RINGTONE, &strTmp);
    (*mDefaultRingtoneFilenames)[DEFAULT_SIM_INDEX] = strTmp;

    Boolean boolTmp = FALSE;
    defaultTelephonyManager->IsMultiSimEnabled(&boolTmp);
    if (boolTmp) {
        for (Int32 i = IPhoneConstants::SUB2; i < phoneCount; ++i) {
            String defaultIterSimRingtone;
            SystemProperties::Get(DEFAULT_RINGTONE_PROPERTY_PREFIX + ISettingsSystem::RINGTONE
                + String("_") + StringUtils::ToString(i + 1), defaultAllSimRingtone, &defaultIterSimRingtone);
            (*mDefaultRingtoneFilenames)[i] = defaultIterSimRingtone;
        }
    }

    sp->Get(DEFAULT_RINGTONE_PROPERTY_PREFIX
            + ISettingsSystem::NOTIFICATION_SOUND, &mDefaultNotificationFilename);
    sp->Get(DEFAULT_RINGTONE_PROPERTY_PREFIX
            + ISettingsSystem::ALARM_ALERT, &mDefaultAlarmAlertFilename);
}

Boolean CMediaScanner::IsDrmEnabled()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String prop;
    sp->Get(String("drm.service.enabled"), &prop);
    return prop != NULL && prop.Equals("true");
}

/*native method*/
ECode CMediaScanner::SetLocale(
    /* [in] */ const String& locale)
{
    android::MediaScanner *mp = mNativeContext;
    if (mp == NULL) {
        Logger::E(TAG, "No scanner available");
        return E_RUNTIME_EXCEPTION;
    }

    if (locale.IsNull()) {
        //jniThrowException(env, kIllegalArgumentException, NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mp->setLocale(locale.string());

    return NOERROR;
}

/*native method*/
ECode CMediaScanner::ExtractAlbumArt(
    /* [in] */ IFileDescriptor* fd,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

//    Logger::V("CMediaScanner", "extractAlbumArt");
    android::MediaScanner *mp = mNativeContext;
    if (mp == NULL) {
        Logger::E(TAG, "No scanner available");
        return E_RUNTIME_EXCEPTION;
    }

    if (fd == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 fdd;
    fd->GetDescriptor(&fdd);
    android::MediaAlbumArt* data = mp->extractAlbumArt(fdd);
    if (!data) {
        return NOERROR;
    }

    Int64 len = *((Int64*)data);
    AutoPtr< ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(len);
    if (array != NULL) {
        memcpy(array, data + 4, len);
    }

done:
    free(data);
    // if NewByteArray() returned NULL, an out-of-memory
    // exception will have been raised. I just want to
    // return null in that case.
    *result = array;
    REFCOUNT_ADD(*result);

    return NOERROR;
}

ECode CMediaScanner::ReleaseReSources()
{
    NativeFinalize();
    return NOERROR;
}

void CMediaScanner::Finalize()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean res;
    cr->ReleaseProvider(mMediaProvider, &res);
    NativeFinalize();
}

ECode CMediaScanner::Prescan( //throws RemoteException
    /* [in] */ const String& filePath,
    /* [in] */ Boolean prescanFiles)
{
    AutoPtr<ICursor> c;
    StringBuilder sb;
    AutoPtr< ArrayOf<String> > selectionArgs;

    mPlayLists.Clear();

    if (!filePath.IsNull()) {
        // query for only one file
        sb += IMediaStoreAudioPlaylistsMembers::ID;  sb += ">? AND ";
        sb += IMediaStoreMediaColumns::DATA; sb += "=?";

        selectionArgs = ArrayOf<String>::Alloc(2);
        selectionArgs->Set(0, String(""));
        selectionArgs->Set(0, filePath);
    }
    else {
        sb += IMediaStoreAudioPlaylistsMembers::ID;  sb += ">?";
        selectionArgs = ArrayOf<String>::Alloc(1);
        selectionArgs->Set(0, String(""));
    }
    String where = sb.ToString();

    // Tell the provider to not delete the file.
    // If the file is truly gone the delete is unnecessary, and we want to avoid
    // accidentally deleting files that are really there (this may happen if the
    // filesystem is mounted and unmounted while the scanner is running).
    AutoPtr<IUriBuilder> builder;
    mFilesUri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendQueryParameter(String("deletedata"), String("false"));

    String nullStr;
    ECode ec = NOERROR;
    AutoPtr<IUri> uri;
    builder->Build((IUri**)&uri);
    AutoPtr<MediaBulkDeleter> deleter = new MediaBulkDeleter(mMediaProvider, mPackageName, uri);

    // Build the list of files from the content provider
//    try {
    if (prescanFiles) {
        // First read existing files from the files table.
        // Because we'll be deleting entries for missing files as we go,
        // we need to query the database in small batches, to avoid problems
        // with CursorWindow positioning.
        Int32 num, format, fileType;
        Int64 lastId = Elastos::Core::Math::INT64_MIN_VALUE;
        Boolean hasNext, exists, bval;
        Int64 rowId, lastModified;
        String path, parentPath;
        AutoPtr<IFile> file;
        AutoPtr<IBundle> bundle;

        AutoPtr<ILibcore> libcore;
        CLibcore::AcquireSingleton((ILibcore**)&libcore);
        AutoPtr<IOs> ios;
        libcore->GetOs((IOs**)&ios);

        AutoPtr<IMtpConstants> mtpConstants;
        CMtpConstants::AcquireSingleton((IMtpConstants**)&mtpConstants);
        AutoPtr<IMediaFileType> mediaFileType;

        AutoPtr<IMediaFile> mfHelper;
        CMediaFile::AcquireSingleton((IMediaFile**)&mfHelper);

        AutoPtr<IUri> limitUri;
        AutoPtr<IUriBuilder> tempBuilder;
        ec = mFilesUri->BuildUpon((IUriBuilder**)&tempBuilder);
        FAIL_GOTO(ec, _EXIT_)

        ec = tempBuilder->AppendQueryParameter(String("limit"), String("1000"));
        FAIL_GOTO(ec, _EXIT_)

        ec = tempBuilder->Build((IUri**)&limitUri);
        FAIL_GOTO(ec, _EXIT_)

        mWasEmptyPriorToScan = TRUE;

        while (TRUE) {
            selectionArgs->Set(0, StringUtils::ToString(lastId));
            if (c != NULL) {
                ICloseable::Probe(c)->Close();
                c = NULL;
            }

            ec = mMediaProvider->Query(mPackageName, limitUri, FILES_PRESCAN_PROJECTION,
                where, selectionArgs, String("_id"), NULL, (ICursor**)&c);
            FAIL_GOTO(ec, _EXIT_)

            if (c == NULL) {
                break;
            }

            c->GetCount(&num);
            if (num == 0) {
                break;
            }

            mWasEmptyPriorToScan = FALSE;
            while ((c->MoveToNext(&hasNext), hasNext)) {
                ec = c->GetInt64(FILES_PRESCAN_ID_COLUMN_INDEX, &rowId);
                FAIL_GOTO(ec, _EXIT_)
                ec = c->GetString(FILES_PRESCAN_PATH_COLUMN_INDEX, &path);
                FAIL_GOTO(ec, _EXIT_)
                ec = c->GetInt32(FILES_PRESCAN_FORMAT_COLUMN_INDEX, &format);
                FAIL_GOTO(ec, _EXIT_)
                ec = c->GetInt64(FILES_PRESCAN_DATE_MODIFIED_COLUMN_INDEX, &lastModified);
                FAIL_GOTO(ec, _EXIT_)

                lastId = rowId;

                // Only consider entries with absolute path names.
                // This allows storing URIs in the database without the
                // media scanner removing them.
                if (path != NULL && path.StartWith("/")) {
                    exists = FALSE;

                    ios->Access(path, OsConstants::_F_OK, &exists);
                    if (!exists && (mtpConstants->IsAbstractObject(format, &bval), !bval)) {
                        // do not delete missing playlists, since they may have been
                        // modified by the user.
                        // The user can delete them in the media player instead.
                        // instead, clear the path and lastModified fields in the row
                        mediaFileType = NULL;
                        mfHelper->GetFileType(path, (IMediaFileType**)&mediaFileType);
                        fileType = 0;
                        if (mediaFileType != NULL) {
                            mediaFileType->GetFileType(&fileType);
                        }

                        mfHelper->IsPlayListFileType(fileType, &bval);
                        if (!bval) {
                        Logger::V("CMediaScanner", " > Prescan %s: row: %lld, path: %s", path.string(), rowId, filePath.string());
                            deleter->Delete(rowId);
                            if (path.ToLowerCase().EndWith("/.nomedia")) {
                                deleter->Flush();
                                file = NULL;
                                CFile::New(path, (IFile**)&file);
                                file->GetParent(&parentPath);
                                bundle = NULL;
                                mMediaProvider->Call(mPackageName, IMediaStore::UNHIDE_CALL,
                                    parentPath, NULL, (IBundle**)&bundle);
                            }
                        }
                    }
                }
            }
        }
    }

_EXIT_:
    if (c != NULL) {
        ICloseable::Probe(c)->Close();
        c = NULL;
    }
    deleter->Flush();

    if (FAILED(ec)) {
        return ec;
    }

    // compute original size of images
    mOriginalCount = 0;
    c = NULL;
    ec = mMediaProvider->Query(mPackageName, mImagesUri, ID_PROJECTION,
        nullStr, NULL, nullStr, NULL, (ICursor**)&c);
    if (c != NULL) {
        c->GetCount(&mOriginalCount);
        ICloseable::Probe(c)->Close();
    }

    return ec;
}

Boolean CMediaScanner::InScanDirectory(
    /* [in] */ const String& path,
    /* [in] */ ArrayOf<String>* directories)
{
    for (Int32 i = 0; i < directories->GetLength(); i++) {
        String directory = (*directories)[i];
        if (path.StartWith(directory)) {
            return TRUE;
        }
    }
    return FALSE;
}

void CMediaScanner::PruneDeadThumbnailFiles()
{
    AutoPtr<HashSet<String> > existingFiles = new HashSet<String>();
    String directory("/sdcard/DCIM/.thumbnails");
    AutoPtr<IFile> file;
    CFile::New(directory, (IFile**)&file);
    AutoPtr< ArrayOf<String> > files;
    file->List((ArrayOf<String>**)&files);
    AutoPtr<ICursor> c;
    if (files == NULL) {
        files = ArrayOf<String>::Alloc(0);
    }

    for (Int32 i = 0; i < files->GetLength(); i++) {
        String fullPathString = directory + "/" + (*files)[i];
        existingFiles->Insert(fullPathString);
    }

//    try {
        AutoPtr< ArrayOf<String> > tempArray = ArrayOf<String>::Alloc(1);
        tempArray->Set(0, IMediaStoreMediaColumns::DATA);
        String nullStr;
        mMediaProvider->Query(mPackageName, mThumbsUri, tempArray, nullStr, NULL, nullStr, NULL, (ICursor**)&c);
        Logger::V(TAG, "pruneDeadThumbnailFiles... " /*+ c*/);
        Boolean tempState;
        if (c != NULL && (c->MoveToFirst(&tempState), tempState)) {
            HashSet<String>::Iterator it;
            String fullPathString;
            do {
                c->GetString(0, &fullPathString);
                it = Find(existingFiles->Begin(), existingFiles->End(), fullPathString);
                if (it != existingFiles->End())
                    existingFiles->Erase(it);
            } while ((c->MoveToNext(&tempState), tempState));
        }

        HashSet<String>::Iterator it = existingFiles->Begin();
        for (Int32 i = 0; i < existingFiles->GetSize(); ++i, ++it) {
            String fileToDelete = (*it);
            if (FALSE)
                Logger::V(TAG, String("fileToDelete is ") + fileToDelete);
//            try {
                AutoPtr<IFile> tempFile;
                CFile::New(fileToDelete, (IFile**)&tempFile);
                tempFile->Delete(&tempState);
//            } catch (SecurityException ex) {
//            }
        }
        Logger::V(TAG, "/pruneDeadThumbnailFiles... " /*+ c*/);
        if (c != NULL) {
            ICloseable::Probe(c)->Close();
        }
//    } catch (RemoteException e) {
//        // We will soon be killed...
//    }
}

ECode CMediaScanner::Postscan(
    /* [in] */ ArrayOf<String>* directories)
{
    // handle playlists last, after we know what media files are on the storage.
    if (mProcessPlaylists) {
        ProcessPlayLists();
    }

    AutoPtr<IMediaStoreImagesMedia> media;
    CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&media);
    AutoPtr<IUri> uri;
    media->GetContentUri(String("external"), (IUri**)&uri);
    Boolean equals;
    if (mOriginalCount == 0 && mImagesUri == uri) {
        PruneDeadThumbnailFiles();
    }

    // allow GC to clean up
    mPlayLists.Clear();
    mMediaProvider = NULL;

    return NOERROR;
}

ECode CMediaScanner::Initialize(
    /* [in] */ const String& volumeName)
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    contentResolver->AcquireProvider(String("media"), (IIContentProvider**)&mMediaProvider);

    AutoPtr<IMediaStoreImagesMedia> images;
    CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&images);
    AutoPtr<IMediaStoreVideoMedia> video;
    CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&video);
    AutoPtr<IMediaStoreAudioMedia> audio;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&audio);
    AutoPtr<IMediaStoreFiles> files;
    CMediaStoreFiles::AcquireSingleton((IMediaStoreFiles**)&files);
    AutoPtr<IMediaStoreImagesThumbnails> thumbnails;
    CMediaStoreImagesThumbnails::AcquireSingleton((IMediaStoreImagesThumbnails**)&thumbnails);

    audio->GetContentUri(volumeName, (IUri**)&mAudioUri);
    video->GetContentUri(volumeName, (IUri**)&mVideoUri);
    images->GetContentUri(volumeName, (IUri**)&mImagesUri);
    thumbnails->GetContentUri(volumeName, (IUri**)&mThumbsUri);
    files->GetContentUri(volumeName, (IUri**)&mFilesUri);
    AutoPtr<IUriBuilder> iub;
    mFilesUri->BuildUpon((IUriBuilder**)&iub);
    iub->AppendQueryParameter(String("nonotify"), String("1"));
    iub->Build((IUri**)&mFilesUriNoNotify);

    if (!volumeName.Equals(String("internal"))) {
        // we only support playlists on external media
        mProcessPlaylists = TRUE;
        mProcessGenres = TRUE;
        AutoPtr<IMediaStoreAudioPlaylists> playlists;
        CMediaStoreAudioPlaylists::AcquireSingleton((IMediaStoreAudioPlaylists**)&playlists);
        playlists->GetContentUri(volumeName, (IUri**)&mPlaylistsUri);

        mCaseInsensitivePaths = TRUE;
    }

    return NOERROR;
}

ECode CMediaScanner::ScanDirectories(
    /* [in] */ ArrayOf<String>* directories,
    /* [in] */ const String& volumeName)
{
//    try {
    Int64 start, prescan, scan, end;
    String nullStr;

    StartScan();

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&start);
    ECode ec = Initialize(volumeName);
    FAIL_GOTO(ec, _EXIT_)
    ec = Prescan(nullStr, TRUE);
    FAIL_GOTO(ec, _EXIT_)
    system->GetCurrentTimeMillis(&prescan);

    if (ENABLE_BULK_INSERTS) {
        mMediaInserter = NULL;
        // create MediaInserter for bulk inserts
        CMediaInserter::New(mMediaProvider, mPackageName, 500, (IMediaInserter**)&mMediaInserter);
    }

    for (Int32 i = 0; i < directories->GetLength(); i++) {
        {
            AutoLock lock(mLock);
            if (mStopScan == TRUE) {
                break;
            }
        }
        ec = ProcessDirectory((*directories)[i], mClient);
        FAIL_GOTO(ec, _EXIT_)
    }

    if (ENABLE_BULK_INSERTS) {
        // flush remaining inserts
        mMediaInserter->FlushAll();
        mMediaInserter = NULL;
    }

    system->GetCurrentTimeMillis(&scan);
    ec = Postscan(directories);
    FAIL_GOTO(ec, _EXIT_)
    system->GetCurrentTimeMillis(&end);

    if (FALSE) {
        Logger::D(TAG, " prescan time: %lld ms", prescan - start);
        Logger::D(TAG, "    scan time: %lld ms", scan - prescan);
        Logger::D(TAG, "postscan time: %lld ms", end - scan);
        Logger::D(TAG, "   total time: %lld ms", end - start);
    }

_EXIT_:
    if (ec == (ECode)E_SQL_EXCEPTION) {
        // this might happen if the SD card is removed while the media scanner is running
        Logger::E(TAG, String("SQLException in CMediaScanner.scan()")/*, e*/);
    }
    else if (ec == (ECode)E_UNSUPPORTED_OPERATION_EXCEPTION) {
       // this might happen if the SD card is removed while the media scanner is running
       Logger::E(TAG, "UnsupportedOperationException in CMediaScanner.scan()");
    }
    else if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "RemoteException in CMediaScanner.scan()");
    }
    ReleaseReSources();
    ExitScan();
    return NOERROR;
}

ECode CMediaScanner::ScanSingleFile(
    /* [in] */ const String& path,
    /* [in] */ const String& volumeName,
    /* [in] */ const String& mimeType,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IFile> file;
    Int64 tempValue, lastModifiedSeconds;
    AutoPtr<IUri> temp;

    ECode ec = Initialize(volumeName);
    if (SUCCEEDED(ec)) {
        ec = Prescan(path, TRUE);
        if (SUCCEEDED(ec)) {
            CFile::New(path, (IFile**)&file);
            Boolean isExist;
            file->Exists(&isExist);
            if (isExist) {
                *result = NULL;
                return ReleaseReSources();
            }

            // lastModified is in milliseconds on Files.
            file->GetLastModified(&tempValue);
            lastModifiedSeconds = tempValue / 1000;

            // always scan the file, so we can return the content://media Uri for existing files
            file->GetLength(&tempValue);
            Boolean isNoMediaPath;
            IsNoMediaPath(path, &isNoMediaPath);
            Boolean isDirectory = FALSE;
            file->IsDirectory(&isDirectory);
            temp = mClient->DoScanFile(path, mimeType, lastModifiedSeconds, tempValue, isDirectory, TRUE, isNoMediaPath);
            *result = temp;
            REFCOUNT_ADD(*result);
            return ReleaseReSources();
        }
    }
    ReleaseReSources();
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "RemoteException in CMediaScanner.scanFile()");
    }
    return ec;
}

void CMediaScanner::StartScan()
{
    {
        AutoLock lock(mLock);
        mScanCount++;
    }
}

void CMediaScanner::ExitScan()
{
    {
        AutoLock lock(mLock);
        mScanCount--;
        if ( mScanCount == 0 ){
            if (mStopScan == TRUE) {
                mStopScan = FALSE;
            }
        }
    }
}

/*static*/
Boolean CMediaScanner::IsNoMediaFile(
    /* [in] */ const String& path)
{
    AutoPtr<IFile> file;
    CFile::New(path,(IFile**)&file);

    Boolean bval;
    if ((file->IsDirectory(&bval), bval)) {
        return FALSE;
    }

    // special case certain file names
    // I use regionMatches() instead of substring() below
    // to avoid memory allocation
    Int32 lastSlash = path.LastIndexOf('/');
    if (lastSlash >= 0 && (lastSlash + 2) < path.GetLength()) {
        // ignore those ._* files created by MacOS
        if (path.RegionMatches(lastSlash + 1, String("._") , 0, 2)) {
            return TRUE;
        }

        // ignore album art files created by Windows Media Player:
        // Folder.jpg, AlbumArtSmall.jpg, AlbumArt_{...}_Large.jpg
        // and AlbumArt_{...}_Small.jpg
        if (path.RegionMatchesIgnoreCase(path.GetLength() - 4, String(".jpg") , 0, 4)) {
            if (path.RegionMatchesIgnoreCase(lastSlash + 1, String("AlbumArt_{") , 0, 10) ||
                path.RegionMatchesIgnoreCase(lastSlash + 1, String("AlbumArt.") , 0, 9)) {
                return TRUE;
            }

            Int32 length = path.GetLength() - lastSlash - 1;
            if ((length == 17 && path.RegionMatchesIgnoreCase(
                    lastSlash + 1, String("AlbumArtSmall") , 0, 13))
                || (length == 10 && path.RegionMatchesIgnoreCase(
                    lastSlash + 1, String("Folder") , 0, 6))) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*static*/
ECode CMediaScanner::IsNoMediaPath(
    /* [in] */ const String& path,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (path.IsNull()) {
        return NOERROR;
    }

    // return true if file or any parent directory has name starting with a dot
    if (path.IndexOf(String("/.")) >= 0) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 firstSlash = path.LastIndexOf('/');
    if (firstSlash <= 0) {
        *result = FALSE;
    }
    String parent = path.Substring(0, firstSlash);
    {
        AutoLock lock(mLock);
        HashMap<String, String>::Iterator it = mNoMediaPaths.Find(parent);
        if (it != mNoMediaPaths.End()) {
            *result = TRUE;
            return NOERROR;
        }
        else {
            // check to see if any parent directories have a ".nomedia" file
            // start from 1 so we don't bother checking in the root directory
            Int32 offset = 1;
            Boolean isExist = FALSE;
            while (offset >= 0) {
                Int32 slashIndex = path.IndexOf('/', offset);
                if (slashIndex > offset) {
                    slashIndex++; // move past slash
                    AutoPtr<IFile> file;
                    CFile::New(String(path.Substring(0, slashIndex)) + String(".nomedia"), (IFile**)&file);
                    file->Exists(&isExist);
                    if (isExist) {
                        // we have a .nomedia in one of the parent directories
                        mNoMediaPaths[parent] = String("");
                        *result = TRUE;
                        return NOERROR;
                    }
                }
                offset = slashIndex;
            }
            mMediaPaths[parent] = String("");
        }
    }

    *result = IsNoMediaFile(path);
    return NOERROR;
}

ECode CMediaScanner::ScanMtpFile(
    /* [in] */ const String& path,
    /* [in] */ const String& volumeName,
    /* [in] */ Int32 objectHandle,
    /* [in] */ Int32 format)
{
    Initialize(volumeName);

    AutoPtr<IMediaFile> mediaFile;
    CMediaFile::AcquireSingleton((IMediaFile**)&mediaFile);
    AutoPtr<IMediaFileType> mediaFileType;
    mediaFile->GetFileType(path, (IMediaFileType**)&mediaFileType);

    Int32 fileType = 0;
    if (mediaFileType != NULL) {
        mediaFileType->GetFileType(&fileType);
    }

    AutoPtr<IFile> file;
    CFile::New(path,(IFile**)&file);

    Int64 tempInt64;
    file->GetLastModified(&tempInt64);
    Int64 lastModifiedSeconds = tempInt64 / 1000;

    Boolean tempState1, tempState2, tempState3, tempState4, tempState5;
    if (!(mediaFile->IsAudioFileType(fileType, &tempState1), tempState1)
        && !(mediaFile->IsVideoFileType(fileType, &tempState2), tempState2)
        && !(mediaFile->IsImageFileType(fileType, &tempState3), tempState3)
        && !(mediaFile->IsPlayListFileType(fileType, &tempState4), tempState4)
        && !(mediaFile->IsDrmFileType(fileType, &tempState5), tempState5)) {

        // no need to use the media scanner, but we need to update last modified and file size
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);

        AutoPtr<IInteger64> integer1, integer2;
        file->GetLength(&tempInt64);
        values->Put(String("_size"), tempInt64);
        values->Put(String("date_modified"), lastModifiedSeconds);

//        try {
            AutoPtr< ArrayOf<String> > whereArgs = ArrayOf<String>::Alloc(1);
            String tempText = StringUtils::ToString(objectHandle);
            whereArgs->Set(0, tempText);

            AutoPtr<IMediaStoreFiles> files;
            CMediaStoreFiles::AcquireSingleton((IMediaStoreFiles**)&files);
            AutoPtr<IUri> uri;
            files->GetMtpObjectsUri(volumeName, (IUri**)&uri);
            Int32 retValue;
            mMediaProvider->Update(mPackageName, uri, values, String("_id=?"), whereArgs, &retValue);
//        } catch (RemoteException e) {
//            Logger::E(TAG, "RemoteException in scanMtpFile"/*, e*/);
//        }
        return NOERROR;
    }

    mMtpObjectHandle = objectHandle;
    AutoPtr<ICursor> fileList;
//    try {
        Boolean tempState;
        mediaFile->IsPlayListFileType(fileType, &tempState);
        if (tempState) {
            // build file cache so we can look up tracks in the playlist
            String nullStr;
            Prescan(nullStr, TRUE);

            AutoPtr<FileEntry> entry = MakeEntryFor(path);
            if (entry != NULL) {
                mMediaProvider->Query(mPackageName, mFilesUri, FILES_PRESCAN_PROJECTION,
                        nullStr, NULL, nullStr, NULL, (ICursor**)&fileList);
                ProcessPlayList(entry, fileList);
            }
        }
        else {
            // MTP will create a file entry for us so we don't want to do it in prescan
            Prescan(path, FALSE);

            // always scan the file, so we can return the content://media Uri for existing files
            String tempText;
            mediaFileType->GetMimeType(&tempText);
            IsNoMediaPath(path, &tempState);
            file->GetLength(&tempInt64);
            mClient->DoScanFile(path, tempText, lastModifiedSeconds, tempInt64,
                (format == (Int32)IMtpConstants::FORMAT_ASSOCIATION), TRUE, tempState);
        }
//    } catch (RemoteException e) {
//        Logger::E(TAG, "RemoteException in MediaScanner.scanFile()"/*, e*/);
//    } finally {
        mMtpObjectHandle = 0;
        if (fileList != NULL) {
            ICloseable::Probe(fileList)->Close();
        }
       return ReleaseReSources();
//    }
}

AutoPtr<CMediaScanner::FileEntry> CMediaScanner::MakeEntryFor(
    /* [in] */ const String& path)
{
    AutoPtr<FileEntry> fileEntry;

    StringBuilder sb(IMediaStoreMediaColumns::DATA); sb += "=?";
    String where = sb.ToString();
    AutoPtr< ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    (*selectionArgs)[0] = path;
    String nullStr;

    AutoPtr<ICursor> c;
    mMediaProvider->Query(mPackageName, mFilesUriNoNotify, FILES_PRESCAN_PROJECTION, where, selectionArgs,
        nullStr, NULL, (ICursor**)&c);
    Boolean bval;
    if ((c->MoveToFirst(&bval), bval)) {
        Int32 format;
        Int64 rowId, lastModified;
        c->GetInt64(FILES_PRESCAN_ID_COLUMN_INDEX, &rowId);
        c->GetInt32(FILES_PRESCAN_FORMAT_COLUMN_INDEX, &format);
        c->GetInt64(FILES_PRESCAN_DATE_MODIFIED_COLUMN_INDEX, &lastModified);
        fileEntry = new FileEntry(rowId, path, lastModified, format);
    }

    if (c != NULL) {
        ICloseable::Probe(c)->Close();
        c = NULL;
    }

    return fileEntry;
}

// returns the number of matching file/directory names, starting from the right
Int32 CMediaScanner::MatchPaths(
    /* [in] */ const String& path1,
    /* [in] */ const String& path2)
{
    Int32 result = 0;
    Int32 end1 = path1.GetLength();
    Int32 end2 = path2.GetLength();

    Int32 slash1, slash2, backSlash1, backSlash2, start1, start2;
    Int32 length;
    while (end1 > 0 && end2 > 0) {
        slash1 = path1.LastIndexOf('/', end1 - 1);
        slash2 = path2.LastIndexOf('/', end2 - 1);
        backSlash1 = path1.LastIndexOf('\\', end1 - 1);
        backSlash2 = path2.LastIndexOf('\\', end2 - 1);
        start1 = (slash1 > backSlash1 ? slash1 : backSlash1);
        start2 = (slash2 > backSlash2 ? slash2 : backSlash2);
        if (start1 < 0) {
            start1 = 0;
        }
        else {
            start1++;
        }

        if (start2 < 0) {
            start2 = 0;
        }
        else {
            start2++;
        }
        length = end1 - start1;
        if (end2 - start2 != length) {
            break;
        }

        if (path1.RegionMatchesIgnoreCase(start1, path2, start2, length)) {
            result++;
            end1 = start1 - 1;
            end2 = start2 - 1;
        }
        else {
            break;
        }
    }
    return result;
}

Boolean CMediaScanner::MatchEntries(
    /* [in] */ Int64 rowId,
    /* [in] */ const String& data)
{
    Boolean done = TRUE;
    List< AutoPtr<PlaylistEntry> >::Iterator it = mPlaylistEntries.Begin();
    for (; it != mPlaylistEntries.End(); ++it) {
        AutoPtr<PlaylistEntry> entry = *it;
        if (entry->mBestmatchlevel == Elastos::Core::Math::INT32_MAX_VALUE) {
            continue; // this entry has been matched already
        }
        done = FALSE;
        if (data.EqualsIgnoreCase(entry->mPath)) {
            entry->mBestmatchid = rowId;
            entry->mBestmatchlevel = Elastos::Core::Math::INT32_MAX_VALUE;
            continue; // no need for path matching
        }

        Int32 matchLength = MatchPaths(data, entry->mPath);
        if (matchLength > entry->mBestmatchlevel) {
            entry->mBestmatchid = rowId;
            entry->mBestmatchlevel = matchLength;
        }
    }
    return done;
}

void CMediaScanner::CachePlaylistEntry(
    /* [in] */ const String& line,
    /* [in] */ const String& playListDirectory)
{
    AutoPtr<PlaylistEntry> entry = new PlaylistEntry();
    // watch for trailing whitespace
    String tempLine = line;
    AutoPtr<ArrayOf<Char32> > chars = tempLine.GetChars();
    Int32 entryLength = chars->GetLength();
    while (entryLength > 0 && Character::IsWhitespace((*chars)[entryLength - 1])) {
        entryLength--;
    }
    // path should be longer than 3 characters.
    // avoid index out of bounds errors below by returning here.
    if (entryLength < 3) {
        return;
    }
    if (entryLength < chars->GetLength()) {
        tempLine = tempLine.Substring(0, entryLength);
    }

    // does entry appear to be an absolute path?
    // look for Unix or DOS absolute paths
    Char32 ch1 = tempLine.GetChar(0);
    Boolean fullPath = (ch1 == '/' ||
            (Character::IsLetter(ch1) && tempLine.GetChar(1) == ':' && tempLine.GetChar(2) == '\\'));
    // if we have a relative path, combine entry with playListDirectory
    if (!fullPath) {
        tempLine = playListDirectory + tempLine;
    }
    entry->mPath = tempLine;
    //FIXME - should we look for "../" within the path?

    mPlaylistEntries.PushBack(entry);

    return;
}

ECode CMediaScanner::ProcessCachedPlaylist(
    /* [in] */ ICursor* fileList,
    /* [in] */ IContentValues* values,
    /* [in] */ IUri* playlistUri)
{
    Boolean succeeded;
    fileList->MoveToPosition(-1, &succeeded);
    Boolean su;
    while ((fileList->MoveToNext(&su), su)) {
        Int64 rowId;
        fileList->GetInt64(FILES_PRESCAN_ID_COLUMN_INDEX, &rowId);
        String data;
        fileList->GetString(FILES_PRESCAN_PATH_COLUMN_INDEX, &data);
        if (MatchEntries(rowId, data)) {
            break;
        }
    }

    Int32 index = 0;
    List< AutoPtr<PlaylistEntry> >::Iterator it = mPlaylistEntries.Begin();
    for (; it != mPlaylistEntries.End(); ++it) {
        AutoPtr<PlaylistEntry> entry = *it;
        if (entry->mBestmatchlevel > 0) {
//            try {
            FAIL_RETURN(values->Clear());
            FAIL_RETURN(values->Put(String("play_order"), index++));
            FAIL_RETURN(values->Put(String("audio_id"), entry->mBestmatchid));
            AutoPtr<IUri> tempUri;
            FAIL_RETURN(mMediaProvider->Insert(mPackageName, playlistUri, values, (IUri**)&tempUri));
//            } catch (RemoteException e) {
//                Logger::E(TAG, "RemoteException in MediaScanner.processCachedPlaylist()"/*, e*/);
//                return;
//            }
        }
    }
    mPlaylistEntries.Clear();
    return NOERROR;
}

void CMediaScanner::ProcessM3uPlayList(
    /* [in] */ const String& path,
    /* [in] */ const String& playListDirectory,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ ICursor* fileList)
{
    AutoPtr<IBufferedReader> reader;
//    try {
        AutoPtr<IFile> f;
        CFile::New(path, (IFile**)&f);
        Boolean isExists;
        if ((f->Exists(&isExists), isExists)) {
            AutoPtr<IFileInputStream> fs;
            CFileInputStream::New(f.Get(), (IFileInputStream**)&fs);
            AutoPtr<IInputStreamReader> ir;
            CInputStreamReader::New(IInputStream::Probe(fs), (IInputStreamReader**)&ir);
            CBufferedReader::New(IReader::Probe(ir), 8192, (IBufferedReader**)&reader);

            String line;
            reader->ReadLine(&line);
            mPlaylistEntries.Clear();
            while (!line.IsNull()) {
                // ignore comment lines, which begin with '#'
                if (!line.IsEmpty() && line.GetChar(0) != '#') {
                    CachePlaylistEntry(line, playListDirectory);
                }
                reader->ReadLine(&line);
            }
            ProcessCachedPlaylist(fileList, values, uri);
        }
//    } catch (IOException e) {
//        Logger::E(TAG, "IOException in CMediaScanner.processM3uPlayList()"/*, e*/);
//    } finally {
//        try {
        if (reader != NULL) {
            ICloseable::Probe(reader.Get())->Close();
        }
//        } catch (IOException e) {
//            Logger::E(TAG, "IOException in CMediaScanner.processM3uPlayList()"/*, e*/);
//        }
//    }
}

void CMediaScanner::ProcessPlsPlayList(
    /* [in] */ const String& path,
    /* [in] */ const String& playListDirectory,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ ICursor* fileList)
{
    AutoPtr<IBufferedReader> reader;
//    try {
        AutoPtr<IFile> f;
        CFile::New(path, (IFile**)&f);
        Boolean isExists;
        f->Exists(&isExists);
        if (isExists) {
            AutoPtr<IFileInputStream> fs;
            CFileInputStream::New(f, (IFileInputStream**)&fs);

            AutoPtr<IInputStreamReader> ir;
            CInputStreamReader::New(IInputStream::Probe(fs), (IInputStreamReader**)&ir);

            CBufferedReader::New(IReader::Probe(ir), 8192, (IBufferedReader**)&reader);

            String line;
            reader->ReadLine(&line);
            mPlaylistEntries.Clear();
            while (line != NULL) {
                // ignore comment lines, which begin with '#'
                if (line.StartWith("File")) {
                    Int32 equals = line.IndexOf('=');
                    if (equals > 0) {
                        CachePlaylistEntry(line.Substring(equals + 1), playListDirectory);
                    }
                }
                reader->ReadLine(&line);
            }
            ProcessCachedPlaylist(fileList, values, uri);
        }
//    } catch (IOException e) {
//        Logger::E(TAG, "IOException in CMediaScanner.processPlsPlayList()"/*, e*/);
//    } finally {
//        try {
        if (reader != NULL) {
            ICloseable::Probe(reader.Get())->Close();
        }
//        } catch (IOException e) {
//            Logger::E(TAG, "IOException in CMediaScanner.processPlsPlayList()"/*, e*/);
//        }
//    }
}

void CMediaScanner::ProcessWplPlayList(
    /* [in] */ const String& path,
    /* [in] */ const String& playListDirectory,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ ICursor* fileList)
{
    AutoPtr<IFileInputStream> fis;
//    try {
        AutoPtr<IFile> f;
        CFile::New(path, (IFile**)&f);
        Boolean isExists;
        f->Exists(&isExists);
        if (isExists) {
            CFileInputStream::New(f, (IFileInputStream**)&fis);

            mPlaylistEntries.Clear();
            Xml::Encoding encoding = Xml::FindEncodingByName(String("UTF-8"));

            AutoPtr<IContentHandler> handler;
            AutoPtr<WplHandler> wpl = new WplHandler(playListDirectory, uri, fileList);
            handler = wpl->GetContentHandler();
            Xml::Parse(IInputStream::Probe(fis), encoding, handler.Get());

            ProcessCachedPlaylist(fileList, values, uri);
        }
//    } catch (SAXException e) {
//        e.printStackTrace();
//    } catch (IOException e) {
//        e.printStackTrace();
//    } finally {
//        try {
            if (fis != NULL) {
                ICloseable::Probe(fis)->Close();
            }
//        } catch (IOException e) {
//            Logger::E(TAG, "IOException in CMediaScanner.processWplPlayList()"/*, e*/);
//        }
//    }
}

ECode CMediaScanner::ProcessPlayList( // throws RemoteException
    /* [in] */ FileEntry* entry,
    /* [in] */ ICursor* fileList)
{
    String path = entry->mPath;

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    Int32 lastSlash = path.LastIndexOf('/');
    if (lastSlash < 0) {
        Logger::E(TAG, "bad path %s", path.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IUri> uri, membersUri;
    Int64 rowId = entry->mRowId;
    // make sure we have a name
    String name;
    values->GetAsString(String("name"), &name);
    if (name.IsNull()) {
        values->GetAsString(String("title"), &name);
        if (name.IsNull()) {
            // extract name from file name

            Int32 lastDot = path.LastIndexOf('.');
            name = (lastDot < 0 ? path.Substring(lastSlash + 1)
                    : path.Substring(lastSlash + 1, lastDot));
        }
    }
    values->Put(String("name"), name);
    values->Put(String("date_modified"), entry->mLastModified);

    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    if (rowId == 0) {
        values->Put(IMediaStoreMediaColumns::DATA, path);

        mMediaProvider->Insert(mPackageName, mPlaylistsUri, values, (IUri**)&uri);
        contentUris->ParseId(uri, &rowId);

        Uri::WithAppendedPath(uri, IMediaStoreAudioPlaylistsMembers::CONTENT_DIRECTORY,
            (IUri**)&membersUri);
    }
    else {
        contentUris->WithAppendedId(mPlaylistsUri, rowId, (IUri**)&uri);
        String nullStr;
        Int32 tempValue;
        mMediaProvider->Update(mPackageName, uri, values, nullStr, NULL, &tempValue);

        // delete members of existing playlist
        Uri::WithAppendedPath(uri, IMediaStoreAudioPlaylistsMembers::CONTENT_DIRECTORY,
            (IUri**)&membersUri);
        mMediaProvider->Delete(mPackageName, membersUri, nullStr, NULL, &tempValue);
    }

    String playListDirectory = path.Substring(0, lastSlash + 1);
    AutoPtr<IMediaFile> mediaFile;
    CMediaFile::AcquireSingleton((IMediaFile**)&mediaFile);
    AutoPtr<IMediaFileType> mediaFileType;
    mediaFile->GetFileType(path, (IMediaFileType**)&mediaFileType);

    Int32 fileType = 0;
    if (mediaFileType != NULL) {
        mediaFileType->GetFileType(&fileType);
    }

    if (fileType == IMediaFile::FILE_TYPE_M3U) {
        ProcessM3uPlayList(path, playListDirectory, membersUri, values, fileList);
    }
    else if (fileType == IMediaFile::FILE_TYPE_PLS) {
        ProcessPlsPlayList(path, playListDirectory, membersUri, values, fileList);
    }
    else if (fileType == IMediaFile::FILE_TYPE_WPL) {
        ProcessWplPlayList(path, playListDirectory, membersUri, values, fileList);
    }
    return NOERROR;
}

ECode CMediaScanner::ProcessPlayLists() //throws RemoteException
{
    List< AutoPtr<FileEntry> >::Iterator iterator = mPlayLists.Begin();
    AutoPtr<ICursor> fileList;
//    try {
        // use the files uri and projection because we need the format column,
        // but restrict the query to just audio files
        String nullStr;
        mMediaProvider->Query(mPackageName, mFilesUri, FILES_PRESCAN_PROJECTION, String("media_type=2") ,
            NULL, nullStr, NULL, (ICursor**)&fileList);
        while (iterator != mPlayLists.End()) {
            AutoPtr<FileEntry> entry = *iterator;
            // only process playlist files if they are new or have been modified since the last scan
            if (entry->mLastModifiedChanged) {
                ProcessPlayList(entry, fileList);
            }
            ++iterator;
        }
//    } catch (RemoteException e1) {
//    } finally {
        if (fileList != NULL) {
            ICloseable::Probe(fileList)->Close();
        }
//    }
    return NOERROR;
}

//-----------------
//    Native method
//-----------------
static android::status_t checkAndClearExceptionFromCallback(ECode ec, const char* methodName) {
    if (FAILED(ec)) {
        Logger::E("CMediaScanner", "An exception was thrown by callback '%s'.", methodName);
        return android::UNKNOWN_ERROR;
    }
    return android::OK;
}

// stolen from dalvik/vm/checkJni.cpp
static bool isValidUtf8(const char* bytes) {
    while (*bytes != '\0') {
        unsigned char utf8 = *(bytes++);
        // Switch on the high four bits.
        switch (utf8 >> 4) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            // Bit pattern 0xxx. No need for any extra bytes.
            break;
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0f:
            /*
             * Bit pattern 10xx or 1111, which are illegal start bytes.
             * Note: 1111 is valid for normal UTF-8, but not the
             * modified UTF-8 used here.
             */
            return false;
        case 0x0e:
            // Bit pattern 1110, so there are two additional bytes.
            utf8 = *(bytes++);
            if ((utf8 & 0xc0) != 0x80) {
                return false;
            }
            // Fall through to take care of the final byte.
        case 0x0c:
        case 0x0d:
            // Bit pattern 110x, so there is one additional byte.
            utf8 = *(bytes++);
            if ((utf8 & 0xc0) != 0x80) {
                return false;
            }
            break;
        }
    }
    return true;
}

class NativeMyMediaScannerClient : public android::MediaScannerClient
{
public:
    NativeMyMediaScannerClient(IMediaScannerClient* client)
        : mClient(client)
    {
    }

    virtual ~NativeMyMediaScannerClient()
    {
    }

    virtual android::status_t scanFile(const char* path, long long lastModified,
            long long fileSize, bool isDirectory, bool noMedia)
    {
        if (DBG)
            Logger::V("CMediaScanner", "scanFile: path(%s), time(%lld), size(%lld) and isDir(%d)",
                path, lastModified, fileSize, isDirectory);

        String pathStr(path);
        ECode ec = mClient->ScanFile(pathStr, lastModified,
            fileSize, isDirectory, noMedia);
        return checkAndClearExceptionFromCallback(ec, "scanFile");
    }

    virtual android::status_t handleStringTag(const char* name, const char* value)
    {
        if (DBG)
            Logger::V("CMediaScanner", "handleStringTag: name(%s) and value(%s)", name, value);
        String nameStr(name);
        char *cleaned = NULL;
        if (!isValidUtf8(value)) {
            cleaned = strdup(value);
            char *chp = cleaned;
            char ch;
            while ((ch = *chp)) {
                if (ch & 0x80) {
                    *chp = '?';
                }
                chp++;
            }
            value = cleaned;
        }

        String valueStr(value);
        free(cleaned);

        ECode ec = mClient->HandleStringTag(nameStr, valueStr);
        return checkAndClearExceptionFromCallback(ec, "handleStringTag");
    }

    virtual android::status_t setMimeType(const char* mimeType)
    {
        if (DBG)
            Logger::V("CMediaScanner", "setMimeType: %s", mimeType);
        String mimeTypeStr(mimeType);
        ECode ec = mClient->SetMimeType(mimeTypeStr);
        return checkAndClearExceptionFromCallback(ec, "setMimeType");
    }

private:
    AutoPtr<IMediaScannerClient> mClient;
};

ECode CMediaScanner::ProcessDirectory(
    /* [in] */ const String& path,
    /* [in] */ IMediaScannerClient* client)
{
//    Logger::V("CMediaScanner", "processDirectory");
    android::MediaScanner *mp = mNativeContext;
    if (mp == NULL) {
        //jniThrowException(env, kRunTimeException, "No scanner available");
        return E_RUNTIME_EXCEPTION;
    }

    if (path.IsNull()) {
        //jniThrowException(env, kIllegalArgumentException, NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    NativeMyMediaScannerClient myClient(client);
    android::MediaScanResult result = mp->processDirectory(path.string(), myClient);
    if (result == android::MEDIA_SCAN_RESULT_ERROR) {
        Logger::E(TAG, "An error occurred while scanning directory '%s'.", path.string());
    }

    return NOERROR;
}

ECode CMediaScanner::ProcessFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType,
    /* [in] */ IMediaScannerClient* client)
{
    Logger::I(TAG, " >> ProcessFile: %s, mimeType:%s", path.string(), mimeType.string());
    // Lock already hold by processDirectory
    android::MediaScanner *mp = mNativeContext;
    if (mp == NULL) {
        Logger::E(TAG, "No scanner available");
        return E_RUNTIME_EXCEPTION;
    }

    if (path.IsNull()) {
        Logger::E(TAG, "process file path is null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    NativeMyMediaScannerClient myClient(client);
    android::MediaScanResult result = mp->processFile(path.string(), mimeType.string(), myClient);
    if (result == android::MEDIA_SCAN_RESULT_ERROR) {
        Logger::E(TAG, "An error occurred while scanning file '%s'.", path.string());
    }
    return NOERROR;
}

ECode CMediaScanner::NativeSetup()
{
//    Logger::V("CMediaScanner", "native_setup");
    android::MediaScanner *mp = new android::StagefrightMediaScanner;
    if (mp == NULL) {
        //jniThrowException(env, kRunTimeException, "Out of memory");
        return E_RUNTIME_EXCEPTION;
    }

    mNativeContext = mp;
    return NOERROR;
}

void CMediaScanner::NativeFinalize()
{
    if (mNativeContext != NULL) {
        delete mNativeContext;
        mNativeContext = NULL;
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
