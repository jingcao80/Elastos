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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Zip.h>
#include "Elastos.Droid.View.h"
#include "elastos/droid/content/pm/ThemeUtils.h"
#include "elastos/droid/content/res/ThemeConfig.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/provider/ThemesContract.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#ifdef DROID_CORE
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/database/sqlite/CSQLiteDatabaseHelper.h"
#include "elastos/droid/media/CRingtoneManagerHelper.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/provider/CMediaStoreAudioMedia.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#endif

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ThemeConfig;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabaseHelper;
using Elastos::Droid::Database::Sqlite::CSQLiteDatabaseHelper;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
using Elastos::Droid::Provider::CMediaStoreAudioMedia;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::IThemesContractThemesColumns;
using Elastos::Droid::Provider::IMediaStoreAudioAudioColumns;
using Elastos::Droid::Provider::ThemesContract;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBuffer;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Zip::IZipOutputStream;
using Elastos::Utility::Zip::CZipOutputStream;
using Elastos::Utility::Zip::IZipInputStream;
using Elastos::Utility::Zip::CZipInputStream;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::CZipEntry;
using Elastos::Utility::Zip::ICRC32;
using Elastos::Utility::Zip::CCRC32;
using Elastos::Utility::Zip::IChecksum;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

//==============================================================================
//                  ThemeUtils::ThemedUiContext
//==============================================================================

ThemeUtils::ThemedUiContext::ThemedUiContext(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName)
    : mPackageName(packageName)
{
    ContextWrapper::constructor(context);
}

ECode ThemeUtils::ThemedUiContext::GetPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mPackageName;
    return NOERROR;
}


//==============================================================================
//                  ThemeUtils
//==============================================================================

const String ThemeUtils::TAG("ThemeUtils");
const String ThemeUtils::OVERLAY_PATH("assets/overlays/");
const String ThemeUtils::ICONS_PATH("assets/icons/");
const String ThemeUtils::COMMON_RES_PATH("assets/overlays/common/");
const String ThemeUtils::FONT_XML("fonts.xml");
const String ThemeUtils::RESOURCE_CACHE_DIR("/data/resource-cache/");
const String ThemeUtils::IDMAP_SUFFIX("@idmap");
const String ThemeUtils::COMMON_RES_SUFFIX(".common");
const String ThemeUtils::COMMON_RES_TARGET("common");
const String ThemeUtils::ICON_HASH_FILENAME("hash");
const String ThemeUtils::SYSTEM_THEME_PATH("/data/system/theme");

static String InitSystemThemePath(
    /* [in] */ const char* str)
{
    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    String separator;
    helper->GetSeparator(&separator);
    return ThemeUtils::SYSTEM_THEME_PATH + separator + str;
}

const String ThemeUtils::SYSTEM_THEME_FONT_PATH = InitSystemThemePath("fonts");
const String ThemeUtils::SYSTEM_THEME_RINGTONE_PATH = InitSystemThemePath("ringtones");
const String ThemeUtils::SYSTEM_THEME_NOTIFICATION_PATH = InitSystemThemePath("notifications");
const String ThemeUtils::SYSTEM_THEME_ALARM_PATH = InitSystemThemePath("alarms");
const String ThemeUtils::SYSTEM_THEME_ICON_CACHE_DIR = InitSystemThemePath("icons");
const String ThemeUtils::THEME_BOOTANIMATION_PATH("assets/bootanimation/bootanimation.zip");
const String ThemeUtils::SYSTEM_MEDIA_PATH("/system/media/audio");

static String InitSystemMediaPath(
    /* [in] */ const char* str)
{
    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    String separator;
    helper->GetSeparator(&separator);
    return ThemeUtils::SYSTEM_MEDIA_PATH + separator + str;
}

const String ThemeUtils::SYSTEM_ALARMS_PATH = InitSystemMediaPath("alarms");
const String ThemeUtils::SYSTEM_RINGTONES_PATH = InitSystemMediaPath("ringtones");
const String ThemeUtils::SYSTEM_NOTIFICATIONS_PATH = InitSystemMediaPath("notifications");
const String ThemeUtils::MEDIA_CONTENT_URI("content://media/internal/audio/media");
const String ThemeUtils::ACTION_THEME_CHANGED("org.cyanogenmod.intent.action.THEME_CHANGED");
const String ThemeUtils::CATEGORY_THEME_COMPONENT_PREFIX("org.cyanogenmod.intent.category.");
const String ThemeUtils::EXTRA_COMPONENTS("components");
const String ThemeUtils::EXTRA_REQUEST_TYPE("request_type");
const String ThemeUtils::EXTRA_UPDATE_TIME("update_time");
const Int32 ThemeUtils::SYSTEM_TARGET_API;
const String ThemeUtils::DEFAULT_PKG("default");
const String ThemeUtils::SETTINGS_DB("/data/data/Elastos.Droid.SettingsProvider/databases/settings.db");
const String ThemeUtils::SETTINGS_SECURE_TABLE("secure");

static AutoPtr<ArrayOf<String> > InitSupportedActions()
{
    AutoPtr<ArrayOf<String> > actions = ArrayOf<String>::Alloc(2);
    (*actions)[0] = String("org.adw.launcher.THEMES");
    (*actions)[1] = String("com.gau.go.launcherex.theme");
    return actions;
}
const AutoPtr<ArrayOf<String> > ThemeUtils::sSupportedActions = InitSupportedActions();

static AutoPtr<ArrayOf<String> > InitSupportedCategories()
{
    AutoPtr<ArrayOf<String> > categories = ArrayOf<String>::Alloc(3);
    (*categories)[0] = String("com.fede.launcher.THEME_ICONPACK");
    (*categories)[1] = String("com.anddoes.launcher.THEME");
    (*categories)[2] = String("com.teslacoilsw.launcher.THEME");
    return categories;
}
const AutoPtr<ArrayOf<String> > ThemeUtils::sSupportedCategories = InitSupportedCategories();

String ThemeUtils::GetOverlayResourceCacheDir(
    /* [in] */ const String& themePkgName)
{
    return RESOURCE_CACHE_DIR + themePkgName;
}

String ThemeUtils::GetTargetCacheDir(
    /* [in] */ const String& targetPkgName,
    /* [in] */ IPackageInfo* themePkg)
{
    String packageName;
    themePkg->GetPackageName(&packageName);
    return GetTargetCacheDir(targetPkgName, packageName);
}

String ThemeUtils::GetTargetCacheDir(
    /* [in] */ const String& targetPkgName,
    /* [in] */ PackageParser::Package* themePkg)
{
    return GetTargetCacheDir(targetPkgName, themePkg->mPackageName);
}

String ThemeUtils::GetTargetCacheDir(
    /* [in] */ const String& targetPkgName,
    /* [in] */ const String& themePkgName)
{
    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    String separator;
    helper->GetSeparator(&separator);
    return GetOverlayResourceCacheDir(themePkgName) + separator + targetPkgName;
}

String ThemeUtils::GetIconPackDir(
    /* [in] */ const String& pkgName)
{
    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    String separator;
    helper->GetSeparator(&separator);
    return GetOverlayResourceCacheDir(pkgName) + separator + "icons";
}

String ThemeUtils::GetIconHashFile(
    /* [in] */ const String& pkgName)
{
    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    String separator;
    helper->GetSeparator(&separator);
    return GetIconPackDir(pkgName) + separator  +  ICON_HASH_FILENAME;
}

String ThemeUtils::GetIconPackPkgPath(
    /* [in] */ const String& pkgName)
{
    return GetIconPackDir(pkgName) + "/resources.apk";
}

String ThemeUtils::GetIconPackResPath(
    /* [in] */ const String& pkgName)
{
    return GetIconPackDir(pkgName) + "/resources.arsc";
}

String ThemeUtils::GetIdmapPath(
    /* [in] */ const String& targetPkgName,
    /* [in] */ const String& overlayPkgName)
{
    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    String separator;
    helper->GetSeparator(&separator);
    return GetTargetCacheDir(targetPkgName, overlayPkgName) + separator + "idmap";
}

String ThemeUtils::GetOverlayPathToTarget(
    /* [in] */ const String& targetPkgName)
{
    StringBuilder sb(OVERLAY_PATH);
    sb.Append(targetPkgName);
    sb.AppendChar('/');
    return sb.ToString();
}

String ThemeUtils::GetCommonPackageName(
    /* [in] */ const String& themePackageName)
{
    if (TextUtils::IsEmpty(themePackageName)) return String(NULL);

    return COMMON_RES_TARGET;
}

ECode ThemeUtils::CreateCacheDirIfNotExists()
{
    AutoPtr<IFile> file;
    CFile::New(RESOURCE_CACHE_DIR, (IFile**)&file);
    Boolean exists, mkdir;
    if ((file->Exists(&exists), !exists) && (file->Mkdir(&mkdir), !mkdir)) {
        String str;
        file->ToString(&str);
        Slogger::E(TAG, "Could not create dir: %s", str.string());
        return E_IO_EXCEPTION;
    }
    FileUtils::SetPermissions(file, FileUtils::sS_IRWXU
            | FileUtils::sS_IRWXG | FileUtils::sS_IROTH | FileUtils::sS_IXOTH, -1, -1);
    return NOERROR;
}

ECode ThemeUtils::CreateResourcesDirIfNotExists(
    /* [in] */ const String& targetPkgName,
    /* [in] */ const String& overlayPkgName)
{
    AutoPtr<IFile> file;
    CFile::New(GetTargetCacheDir(targetPkgName, overlayPkgName), (IFile**)&file);
    Boolean exists, mkdir;
    if ((file->Exists(&exists), !exists) && (file->Mkdir(&mkdir), !mkdir)) {
        String str;
        file->ToString(&str);
        Slogger::E(TAG, "Could not create dir: %s", str.string());
        return E_IO_EXCEPTION;
    }
    FileUtils::SetPermissions(file, FileUtils::sS_IRWXU
            | FileUtils::sS_IRWXG | FileUtils::sS_IROTH | FileUtils::sS_IXOTH, -1, -1);
    return NOERROR;
}

ECode ThemeUtils::CreateIconDirIfNotExists(
    /* [in] */ const String& pkgName)
{
    CreateDirIfNotExists(GetOverlayResourceCacheDir(pkgName));
    AutoPtr<IFile> file;
    CFile::New(GetIconPackDir(pkgName), (IFile**)&file);
    Boolean exists, mkdir;
    if ((file->Exists(&exists), !exists) && (file->Mkdir(&mkdir), !mkdir)) {
        String str;
        file->ToString(&str);
        Slogger::E(TAG, "Could not create dir: %s", str.string());
        return E_IO_EXCEPTION;
    }
    FileUtils::SetPermissions(file, FileUtils::sS_IRWXU
            | FileUtils::sS_IRWXG | FileUtils::sS_IROTH | FileUtils::sS_IXOTH, -1, -1);
    return NOERROR;
}

Boolean ThemeUtils::DirExists(
    /* [in] */ const String& dirPath)
{
    AutoPtr<IFile> dir;
    CFile::New(dirPath, (IFile**)&dir);
    Boolean exists, isDirectory;
    return (dir->Exists(&exists), exists) && (dir->IsDirectory(&isDirectory), isDirectory);
}

void ThemeUtils::CreateDirIfNotExists(
    /* [in] */ const String& dirPath)
{
    if (!DirExists(dirPath)) {
        AutoPtr<IFile> dir;
        CFile::New(dirPath, (IFile**)&dir);
        Boolean mkdir;
        if (dir->Mkdir(&mkdir), mkdir) {
            FileUtils::SetPermissions(dir, FileUtils::sS_IRWXU |
                    FileUtils::sS_IRWXG| FileUtils::sS_IROTH | FileUtils::sS_IXOTH, -1, -1);
        }
    }
}

void ThemeUtils::CreateThemeDirIfNotExists()
{
    CreateDirIfNotExists(SYSTEM_THEME_PATH);
}

void ThemeUtils::CreateFontDirIfNotExists()
{
    CreateDirIfNotExists(SYSTEM_THEME_FONT_PATH);
}

void ThemeUtils::CreateRingtoneDirIfNotExists()
{
    CreateDirIfNotExists(SYSTEM_THEME_RINGTONE_PATH);
}

void ThemeUtils::CreateNotificationDirIfNotExists()
{
    CreateDirIfNotExists(SYSTEM_THEME_NOTIFICATION_PATH);
}

void ThemeUtils::CreateAlarmDirIfNotExists()
{
    CreateDirIfNotExists(SYSTEM_THEME_ALARM_PATH);
}

void ThemeUtils::CreateIconCacheDirIfNotExists()
{
    CreateDirIfNotExists(SYSTEM_THEME_ICON_CACHE_DIR);
}

void ThemeUtils::ClearIconCache()
{
    AutoPtr<IFile> dir;
    CFile::New(SYSTEM_THEME_ICON_CACHE_DIR, (IFile**)&dir);
    FileUtils::DeleteContents(dir);
}

ECode ThemeUtils::GetInputStreamFromAsset(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& _path,
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    *inputStream = NULL;
    String path = _path;
    if (ctx == NULL || path.IsNull())
        return NOERROR;
    AutoPtr<IInputStream> is;
    String ASSET_BASE("file:///android_asset/");
    path = path.Substring(ASSET_BASE.GetLength());
    AutoPtr<IAssetManager> assets;
    ctx->GetAssets((IAssetManager**)&assets);
    return assets->Open(path, inputStream);
}

void ThemeUtils::CloseQuietly(
    /* [in] */ IInputStream* stream)
{
    if (stream == NULL)
        return;
    // try {
    stream->Close();
    // } catch (IOException e) {
    // }
}

void ThemeUtils::CloseQuietly(
    /* [in] */ IOutputStream* stream)
{
    if (stream == NULL)
        return;
    // try {
    stream->Close();
    // } catch (IOException e) {
    // }
}

ECode ThemeUtils::CopyAndScaleBootAnimation(
    /* [in] */ IContext* context,
    /* [in] */ IInputStream* input,
    /* [in] */ const String& dst)
{
    AutoPtr<IOutputStream> os;
    CFileOutputStream::New(dst, (IOutputStream**)&os);
    AutoPtr<IOutputStream> bos;
    CBufferedOutputStream::New(os, (IOutputStream**)&bos);
    AutoPtr<IZipOutputStream> zos;
    CZipOutputStream::New(bos, (IZipOutputStream**)&zos);
    AutoPtr<IInputStream> bis;
    CBufferedInputStream::New(input, (IInputStream**)&bis);
    AutoPtr<IZipInputStream> bootAni;
    CZipInputStream::New(bis, (IZipInputStream**)&bootAni);
    AutoPtr<IZipEntry> ze;

    zos->SetMethod(IZipOutputStream::STORED);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(4096);
    Int32 len;
    while (bootAni->GetNextEntry((IZipEntry**)&ze), ze != NULL) {
        String name;
        ze->GetName(&name);
        AutoPtr<IZipEntry> entry;
        CZipEntry::New(name, (IZipEntry**)&entry);
        entry->SetMethod(IZipEntry::STORED);
        Int64 crc;
        ze->GetCrc(&crc);
        entry->SetCrc(crc);
        Int64 size;
        ze->GetSize(&size);
        entry->SetSize(size);
        entry->SetCompressedSize(size);
        if (!name.Equals("desc.txt")) {
            // just copy this entry straight over into the output zip
            zos->PutNextEntry(entry);
            while (IInputStream::Probe(bootAni)->Read(bytes, &len), len > 0) {
                IOutputStream::Probe(zos)->Write(bytes, 0, len);
            }
        }
        else {
            String line;
            AutoPtr<IReader> isReader;
            CInputStreamReader::New(IInputStream::Probe(bootAni), (IReader**)&isReader);
            AutoPtr<IBufferedReader> reader;
            CBufferedReader::New(isReader, (IBufferedReader**)&reader);
            String l;
            reader->ReadLine(&l);
            AutoPtr<ArrayOf<String> > info;
            StringUtils::Split(l, " ", (ArrayOf<String>**)&info);

            Int32 scaledWidth;
            Int32 scaledHeight;
            AutoPtr<IInterface> service;
            context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
            AutoPtr<IWindowManager> wm = IWindowManager::Probe(service);
            AutoPtr<IDisplayMetrics> dm;
            CDisplayMetrics::New((IDisplayMetrics**)&dm);
            AutoPtr<IDisplay> display;
            wm->GetDefaultDisplay((IDisplay**)&display);
            display->GetRealMetrics(dm);
            // just in case the device is in landscape orientation we will
            // swap the values since most (if not all) animations are portrait
            Int32 widthPixels, heightPixels;
            dm->GetWidthPixels(&widthPixels);
            dm->GetHeightPixels(&heightPixels);
            if (widthPixels > heightPixels) {
                scaledWidth = heightPixels;
                scaledHeight = widthPixels;
            }
            else {
                scaledWidth = widthPixels;
                scaledHeight = heightPixels;
            }

            Int32 width = StringUtils::ParseInt32((*info)[0]);
            Int32 height = StringUtils::ParseInt32((*info)[1]);

            if (width == height)
                scaledHeight = scaledWidth;
            else {
                // adjust scaledHeight to retain original aspect ratio
                Float scale = (Float)scaledWidth / (Float)width;
                Int32 newHeight = (Int32)((Float)height * scale);
                if (newHeight < scaledHeight)
                    scaledHeight = newHeight;
            }

            AutoPtr<ICRC32> crc32;
            CCRC32::New((ICRC32**)&crc32);
            Int32 size = 0;
            AutoPtr<IByteBufferHelper> bufferH;
            CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bufferH);
            AutoPtr<IByteBuffer> buffer;
            bufferH->Wrap(bytes, (IByteBuffer**)&buffer);
            AutoPtr<IInteger32> scaledWidthInt, scaledHeightInt;
            CInteger32::New(scaledWidth, (IInteger32**)&scaledWidthInt);
            CInteger32::New(scaledHeight, (IInteger32**)&scaledHeightInt);
            AutoPtr<ICharSequence> cs;
            CString::New((*info)[2], (ICharSequence**)&cs);
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
            args->Set(0, scaledWidthInt);
            args->Set(1, scaledHeightInt);
            args->Set(2, cs);
            line = StringUtils::Format(String("%d %d %s\n"), args);
            buffer->Put(line.GetBytes());
            size += line.GetBytes()->GetLength();
            AutoPtr<IChecksum> checksum = IChecksum::Probe(crc32);
            checksum->Update(line.GetBytes());
            while (reader->ReadLine(&line), !line.IsNull()) {
                AutoPtr<ICharSequence> lineCs;
                CString::New(line, (ICharSequence**)&lineCs);
                AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
                params->Set(0, lineCs);
                line = StringUtils::Format(String("%s\n"), params);
                buffer->Put(line.GetBytes());
                size += line.GetBytes()->GetLength();
                checksum->Update(line.GetBytes());
            }
            Int64 v;
            checksum->GetValue(&v);
            entry->SetCrc(v);
            entry->SetSize(size);
            entry->SetCompressedSize(size);
            zos->PutNextEntry(entry);
            AutoPtr<ArrayOf<Byte> > array;
            buffer->GetArray((ArrayOf<Byte>**)&array);
            IOutputStream::Probe(zos)->Write(array, 0, size);
        }
        zos->CloseEntry();
    }
    ICloseable::Probe(zos)->Close();
    return NOERROR;
}

Boolean ThemeUtils::IsValidAudible(
    /* [in] */ const String& fileName)
{
    return (!fileName.IsNull() &&
            (fileName.EndWith(".mp3") || fileName.EndWith(".ogg")));
}

Boolean ThemeUtils::SetAudible(
    /* [in] */ IContext* context,
    /* [in] */ IFile* ringtone,
    /* [in] */ Int32 type,
    /* [in] */ const String& name)
{
    String path;
    ringtone->GetAbsolutePath(&path);
    String mimeType = name.EndWith(".ogg") ? String("audio/ogg") : String("audio/mp3");
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IMediaStoreMediaColumns::DATA, path);
    values->Put(IMediaStoreMediaColumns::TITLE, name);
    values->Put(IMediaStoreMediaColumns::MIME_TYPE, mimeType);
    Int64 len;
    ringtone->GetLength(&len);
    values->Put(IMediaStoreMediaColumns::SIZE, len);
    values->Put(IMediaStoreAudioAudioColumns::IS_RINGTONE, type == IRingtoneManager::TYPE_RINGTONE);
    values->Put(IMediaStoreAudioAudioColumns::IS_NOTIFICATION,
            type == IRingtoneManager::TYPE_NOTIFICATION);
    values->Put(IMediaStoreAudioAudioColumns::IS_ALARM, type == IRingtoneManager::TYPE_ALARM);
    values->Put(IMediaStoreAudioAudioColumns::IS_MUSIC, false);

    AutoPtr<IMediaStoreAudioMedia> audioMedia;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&audioMedia);
    AutoPtr<IUri> uri;
    audioMedia->GetContentUriForPath(path, (IUri**)&uri);
    AutoPtr<IUri> newUri;
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IBaseColumns::ID;
    AutoPtr<ICursor> c;
    contentResolver->Query(uri, args,
            IMediaStoreMediaColumns::DATA + "='" + path + "'",
            NULL, String(NULL), (ICursor**)&c);
    Int32 count;
    if (c != NULL && (c->GetCount(&count), count > 0)) {
        Boolean moveToFirst;
        c->MoveToFirst(&moveToFirst);
        Int64 id;
        c->GetInt64(0, &id);
        ICloseable::Probe(c)->Close();
        AutoPtr<IUriHelper> uriH;
        CUriHelper::AcquireSingleton((IUriHelper**)&uriH);
        AutoPtr<IUri> parseResult;
        uriH->Parse(MEDIA_CONTENT_URI, (IUri**)&parseResult);
        uriH->WithAppendedPath(parseResult, String("") + StringUtils::ToString(id), (IUri**)&newUri);
        Int32 rowsAffected;
        contentResolver->Update(uri, values,
                IBaseColumns::ID + "=" + StringUtils::ToString(id), NULL, &rowsAffected);
    }
    if (newUri == NULL) {
        contentResolver->Insert(uri, values, (IUri**)&newUri);
    }
    // try {
    AutoPtr<IRingtoneManagerHelper> rmH;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&rmH);
    if (FAILED(rmH->SetActualDefaultRingtoneUri(context, type, newUri))) {
        return FALSE;
    }
    // } catch (Exception e) {
    //     return false;
    // }
    return TRUE;
}

Boolean ThemeUtils::SetDefaultAudible(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type)
{
    String audiblePath = GetDefaultAudiblePath(type);
    if (!audiblePath.IsNull()) {
        AutoPtr<IMediaStoreAudioMedia> audioMedia;
        CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&audioMedia);
        AutoPtr<IUri> uri;
        audioMedia->GetContentUriForPath(audiblePath, (IUri**)&uri);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);
        AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = IBaseColumns::ID;
        AutoPtr<ICursor> c;
        contentResolver->Query(uri, args,
                IMediaStoreMediaColumns::DATA + "='" + audiblePath + "'",
                NULL, String(NULL), (ICursor**)&c);
        Int32 count;
        if (c != NULL && (c->GetCount(&count), count > 0)) {
            Boolean moveToFirst;
            c->MoveToFirst(&moveToFirst);
            Int64 id;
            c->GetInt64(0, &id);
            ICloseable::Probe(c)->Close();
            AutoPtr<IUriHelper> uriH;
            CUriHelper::AcquireSingleton((IUriHelper**)&uriH);
            AutoPtr<IUri> parseResult;
            uriH->Parse(MEDIA_CONTENT_URI, (IUri**)&parseResult);
            uri = NULL;
            uriH->WithAppendedPath(parseResult, String("") + StringUtils::ToString(id), (IUri**)&uri);
        }
        if (uri != NULL) {
            AutoPtr<IRingtoneManagerHelper> rmH;
            CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&rmH);
            rmH->SetActualDefaultRingtoneUri(context, type, uri);
        }
    }
    else {
        return FALSE;
    }
    return TRUE;
}

String ThemeUtils::GetDefaultAudiblePath(
    /* [in] */ Int32 type)
{
    String name;
    String path(NULL);
    switch (type) {
        case IRingtoneManager::TYPE_ALARM: {
            AutoPtr<IFileHelper> helper;
            CFileHelper::AcquireSingleton((IFileHelper**)&helper);
            String separator;
            helper->GetSeparator(&separator);
            SystemProperties::Get(String("ro.config.alarm_alert"), String(NULL), &name);
            path = !name.IsNull() ? SYSTEM_ALARMS_PATH + separator + name : String(NULL);
            break;
        }
        case IRingtoneManager::TYPE_NOTIFICATION: {
            AutoPtr<IFileHelper> helper;
            CFileHelper::AcquireSingleton((IFileHelper**)&helper);
            String separator;
            helper->GetSeparator(&separator);
            SystemProperties::Get(String("ro.config.notification_sound"), String(NULL), &name);
            path = !name.IsNull() ? SYSTEM_NOTIFICATIONS_PATH + separator + name : String(NULL);
            break;
        }
        case IRingtoneManager::TYPE_RINGTONE: {
            AutoPtr<IFileHelper> helper;
            CFileHelper::AcquireSingleton((IFileHelper**)&helper);
            String separator;
            helper->GetSeparator(&separator);
            SystemProperties::Get(String("ro.config.ringtone"), String(NULL), &name);
            path = !name.IsNull() ? SYSTEM_RINGTONES_PATH + separator + name : String(NULL);
            break;
        }
        default:
            break;
    }
    return path;
}

void ThemeUtils::ClearAudibles(
    /* [in] */ IContext* context,
    /* [in] */ const String& audiblePath)
{
    AutoPtr<IFile> audibleDir;
    CFile::New(audiblePath, (IFile**)&audibleDir);
    Boolean exists;
    if (audibleDir->Exists(&exists), exists) {
        AutoPtr<ArrayOf<String> > files;
        audibleDir->List((ArrayOf<String>**)&files);
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            AutoPtr<IFileHelper> helper;
            CFileHelper::AcquireSingleton((IFileHelper**)&helper);
            String separator;
            helper->GetSeparator(&separator);
            String filePath = audiblePath + separator + (*files)[i];
            AutoPtr<IMediaStoreAudioMedia> audioMedia;
            CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&audioMedia);
            AutoPtr<IUri> uri;
            audioMedia->GetContentUriForPath(filePath, (IUri**)&uri);
            Int32 rowsAffected;
            resolver->Delete(uri, IMediaStoreMediaColumns::DATA + "=\""
                    + filePath + "\"", NULL, &rowsAffected);
            AutoPtr<IFile> newF;
            CFile::New(filePath, (IFile**)&newF);
            newF->Delete();
        }
    }
}

AutoPtr<IContext> ThemeUtils::CreateUiContext(
    /* [in] */ IContext* context)
{
    // begin from this
    // try {
    AutoPtr<IContext> uiContext;
    if (FAILED(context->CreatePackageContext(String("Elastos.Droid.SystemUI"),
            IContext::CONTEXT_RESTRICTED, (IContext**)&uiContext))) {
        return NULL;
    }
    String pkgName;
    context->GetPackageName(&pkgName);
    AutoPtr<ThemedUiContext> ctx = new ThemedUiContext(uiContext, pkgName);
    return (IContext*)ctx.Get();
    // } catch (PackageManager.NameNotFoundException e) {
    // }
}

void ThemeUtils::RegisterThemeChangeReceiver(
    /* [in] */ IContext* context,
    /* [in] */ IBroadcastReceiver* receiver)
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(ACTION_THEME_CHANGED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(receiver, filter, (IIntent**)&intent);
}

ECode ThemeUtils::GetLockscreenWallpaperPath(
    /* [in] */ IAssetManager* assetManager,
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path)
    *path = String(NULL);
    AutoPtr<ArrayOf<String> > assets;
    FAIL_RETURN(assetManager->List(String("lockscreen"), (ArrayOf<String>**)&assets))
    String asset = GetFirstNonEmptyAsset(assets);
    if (asset.IsNull()) return NOERROR;
    *path = String("lockscreen/") + asset;
    return NOERROR;
}

ECode ThemeUtils::GetWallpaperPath(
    /* [in] */ IAssetManager* assetManager,
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path)
    *path = String(NULL);
    AutoPtr<ArrayOf<String> > assets;
    FAIL_RETURN(assetManager->List(String("wallpapers"), (ArrayOf<String>**)&assets))
    String asset = GetFirstNonEmptyAsset(assets);
    if (asset.IsNull()) return NOERROR;
    *path = String("wallpapers/") + asset;
    return NOERROR;
}

String ThemeUtils::GetFirstNonEmptyAsset(
    /* [in] */ ArrayOf<String>* assets)
{
    if (assets == NULL) return String(NULL);
    String filename(NULL);
    for (Int32 i = 0; i < assets->GetLength(); ++i) {
        if (!(*assets)[i].IsEmpty()) {
            filename = (*assets)[i];
            break;
        }
    }
    return filename;
}

String ThemeUtils::GetDefaultThemePackageName(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    String defaultThemePkg;
    Settings::Secure::GetString(resolver,
            ISettingsSecure::DEFAULT_THEME_PACKAGE, &defaultThemePkg);
    if (!TextUtils::IsEmpty(defaultThemePkg)) {
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        // try {
        AutoPtr<IPackageInfo> pkgInfo;
        if (pm->GetPackageInfo(defaultThemePkg, 0, (IPackageInfo**)&pkgInfo), pkgInfo != NULL) {
            return defaultThemePkg;
        }
        // } catch (PackageManager.NameNotFoundException e) {
        //     // doesn't exist so system will be default
        //     Log.w(TAG, "Default theme " + defaultThemePkg + " not found", e);
        // }
    }

    return IThemeConfig::SYSTEM_DEFAULT;
}

AutoPtr<List<String> > ThemeUtils::GetAllComponents()
{
    AutoPtr<List<String> > components = new List<String>();
    components->PushBack(IThemesContractThemesColumns::MODIFIES_FONTS);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_LAUNCHER);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_ALARMS);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_BOOT_ANIM);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_ICONS);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_LOCKSCREEN);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_NOTIFICATIONS);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_OVERLAYS);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_RINGTONES);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_STATUS_BAR);
    components->PushBack(IThemesContractThemesColumns::MODIFIES_NAVIGATION_BAR);
    return components;
}

AutoPtr<List<String> > ThemeUtils::GetSupportedComponents(
    /* [in] */ IContext* context,
    /* [in] */ const String& pkgName)
{
    AutoPtr<List<String> > supportedComponents = new List<String>();

    String selection = IThemesContractThemesColumns::PKG_NAME + "= ?";
    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    (*selectionArgs)[0] = pkgName;
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> c;
    resolver->Query(ThemesContract::ThemesColumns::CONTENT_URI,
            NULL, selection, selectionArgs, String(NULL), (ICursor**)&c);

    if (c != NULL) {
        Boolean moveToFirst;
        if (c->MoveToFirst(&moveToFirst), moveToFirst) {
            AutoPtr<List<String> > allComponents = GetAllComponents();
            List<String>::Iterator it = allComponents->Begin();
            for (; it != allComponents->End(); ++it) {
                String component = *it;
                Int32 index;
                c->GetColumnIndex(component, &index);
                Int32 value;
                if (c->GetInt32(index, &value), value == 1) {
                    supportedComponents->PushBack(component);
                }
            }
        }
        ICloseable::Probe(c)->Close();
    }
    return supportedComponents;
}

AutoPtr<HashMap<String, String> > ThemeUtils::GetDefaultComponents(
    /* [in] */ IContext* context)
{
    String defaultThemePkg = GetDefaultThemePackageName(context);
    AutoPtr<List<String> > defaultComponents;
    AutoPtr<List<String> > systemComponents = GetSupportedComponents(context, IThemeConfig::SYSTEM_DEFAULT);
    if (!IThemeConfig::SYSTEM_DEFAULT.Equals(defaultThemePkg)) {
        defaultComponents = GetSupportedComponents(context, defaultThemePkg);
    }

    AutoPtr<HashMap<String, String> > componentMap = new HashMap<String, String>(systemComponents->GetSize());
    if (defaultComponents != NULL) {
        List<String>::Iterator it = defaultComponents->Begin();
        for (; it != defaultComponents->End(); ++it) {
            (*componentMap)[*it] = defaultThemePkg;
        }
    }
    List<String>::Iterator it = systemComponents->Begin();
    for (; it != systemComponents->End(); ++it) {
        String component = *it;
        if (componentMap->Find(component) == componentMap->End()) {
            (*componentMap)[component] = IThemeConfig::SYSTEM_DEFAULT;
        }
    }

    return componentMap;
}

void ThemeUtils::CompleteComponentMap(
    /* [in] */ IContext* context,
    /* [in] */ HashMap<String, String>* componentMap)
{
    if (componentMap == NULL) return;

    AutoPtr<HashMap<String, String> > defaultComponents = GetDefaultComponents(context);
    HashMap<String, String>::Iterator it = defaultComponents->Begin();
    for (; it != defaultComponents->End(); ++it) {
        String component = it->mFirst;
        if (componentMap->Find(component) == componentMap->End()) {
            (*componentMap)[component] = defaultComponents->Find(component)->mSecond;
        }
    }
}

AutoPtr<IThemeConfig> ThemeUtils::GetBootThemeDirty()
{
    AutoPtr<IThemeConfig> config;
    AutoPtr<ISQLiteDatabase> db;
    String nullStr;
    // try {
    AutoPtr<ISQLiteDatabaseHelper> helper;
    CSQLiteDatabaseHelper::AcquireSingleton((ISQLiteDatabaseHelper**)&helper);
    ECode ec = helper->OpenDatabase(SETTINGS_DB, NULL, ISQLiteDatabase::OPEN_READONLY, (ISQLiteDatabase**)&db);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Unable to open %s, ox%08x", SETTINGS_DB.string(), ec);
        if (db != NULL) {
            ICloseable::Probe(db)->Close();
        }
        return config;
    }
    if (db != NULL) {
        String selection("name=?");
        AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = IConfiguration::THEME_PKG_CONFIGURATION_PERSISTENCE_PROPERTY;
        AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(1);
        (*columns)[0] = "value";
        AutoPtr<ICursor> c;
        ec = db->Query(SETTINGS_SECURE_TABLE, columns, selection, selectionArgs,
            nullStr, nullStr, nullStr, (ICursor**)&c);
        if (FAILED(ec)) {
            Slogger::W(TAG, "Unable to open %s, 0x%08x", SETTINGS_DB.string(), ec);
            if (db != NULL) {
                ICloseable::Probe(db)->Close();
            }
            return config;
        }
        if (c != NULL) {
            Int32 count;
            if (c->GetCount(&count), count > 0) {
                Boolean moveToFirst;
                c->MoveToFirst(&moveToFirst);
                String json;
                c->GetString(0, &json);
                if (!json.IsNull()) {
                    Slogger::I(TAG, " >> create theme config: [%s]", json.string());
                    config = ThemeConfig::FromJson(json);
                }
            }
            ICloseable::Probe(c)->Close();
        }
    }

    if (db != NULL) {
        ICloseable::Probe(db)->Close();
    }

    return config;
}

Boolean ThemeUtils::IsPerAppThemeComponent(
    /* [in] */ const String& component)
{
    return !(DEFAULT_PKG.Equals(component)
            || IThemeConfig::SYSTEMUI_STATUS_BAR_PKG.Equals(component)
            || IThemeConfig::SYSTEMUI_NAVBAR_PKG.Equals(component));
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
