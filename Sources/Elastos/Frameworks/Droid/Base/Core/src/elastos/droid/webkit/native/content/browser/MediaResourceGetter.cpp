
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/native/content/browser/MediaResourceGetter.h"
#include "elastos/droid/webkit/native/content/api/MediaResourceGetter_dec.h"
#include "elastos/droid/webkit/native/base/PathUtils.h"
//TODO #include <elastos/core/CInteger32.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
//TODO #include "elastos/utility/CArrayList.h"
#include "elastos/droid/os/Build.h"
//TODO #include "elastos/droid/os/CParcelFileDescriptorHelper.h"
#include "elastos/droid/text/TextUtils.h"
//TODO #include "elastos/droid/utility/CHashMap.h"
#include "elastos/droid/Manifest.h"

//TODO using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
//TODO using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Webkit::Base::PathUtils;
using Elastos::Utility::IIterator;
using Elastos::Utility::IIterable;
using Elastos::Utility::EIID_IIterable;
//TODO using Elastos::Utility::CArrayList;
//TODO using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//=====================================================================
//                  MediaResourceGetter::MediaMetadata
//=====================================================================
MediaResourceGetter::MediaMetadata::MediaMetadata(
    /* [in] */ Int32 durationInMilliseconds,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean success)
    : mDurationInMilliseconds(durationInMilliseconds)
    , mWidth(width)
    , mHeight(height)
    , mSuccess(success)
{
}

Int32 MediaResourceGetter::MediaMetadata::GetDurationInMilliseconds()
{
    return mDurationInMilliseconds;
}

Int32 MediaResourceGetter::MediaMetadata::GetWidth()
{
    return mWidth;
}

Int32 MediaResourceGetter::MediaMetadata::GetHeight()
{
    return mHeight;
}

Boolean MediaResourceGetter::MediaMetadata::IsSuccess()
{
    return mSuccess;
}

String MediaResourceGetter::MediaMetadata::ToString()
{
    String strRet("MediaMetadata[");
        strRet += "durationInMilliseconds=";
        strRet += StringUtils::ToString(mDurationInMilliseconds);
        strRet += ", width=";
        strRet += StringUtils::ToString(mWidth);
        strRet += ", height=";
        strRet += StringUtils::ToString(mHeight);
        strRet += ", success=";
        strRet += StringUtils::ToString(mSuccess);
        strRet += "]";

    return strRet;
}

Int32 MediaResourceGetter::MediaMetadata::HashCode()
{
    const Int32 prime = 31;
    Int32 result = 1;
    result = prime * result + mDurationInMilliseconds;
    result = prime * result + mHeight;
    result = prime * result + (mSuccess ? 1231 : 1237);
    result = prime * result + mWidth;
    return result;
}

Boolean MediaResourceGetter::MediaMetadata::Equals(
    /* [in] */ Object* obj)
{
    if (this == obj)
        return TRUE;
    if (obj == NULL)
        return FALSE;

    assert(0);
    // TODO
    // if (getClass() != obj.getClass())
    //     return FALSE;

    AutoPtr<MediaMetadata> other = (MediaMetadata*)obj;
    if (mDurationInMilliseconds != other->mDurationInMilliseconds)
        return FALSE;
    if (mHeight != other->mHeight)
        return FALSE;
    if (mSuccess != other->mSuccess)
        return FALSE;
    if (mWidth != other->mWidth)
        return FALSE;

    return TRUE;
}

Int32 MediaResourceGetter::MediaMetadata::GetDurationInMilliseconds(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaResourceGetter::MediaMetadata> mObj = (MediaResourceGetter::MediaMetadata*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "MediaResourceGetter::MediaMetadata::GetDurationInMilliseconds, mObj is NULL");
        return 0;
    }
    return mObj->GetDurationInMilliseconds();
}

Int32 MediaResourceGetter::MediaMetadata::GetWidth(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaResourceGetter::MediaMetadata> mObj = (MediaResourceGetter::MediaMetadata*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "MediaResourceGetter::MediaMetadata::GetWidth, mObj is NULL");
        return 0;
    }
    return mObj->GetWidth();
}

Int32 MediaResourceGetter::MediaMetadata::GetHeight(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaResourceGetter::MediaMetadata> mObj = (MediaResourceGetter::MediaMetadata*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "MediaResourceGetter::MediaMetadata::GetHeight, mObj is NULL");
        return 0;
    }
    return mObj->GetHeight();
}

Boolean MediaResourceGetter::MediaMetadata::IsSuccess(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaResourceGetter::MediaMetadata> mObj = (MediaResourceGetter::MediaMetadata*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "MediaResourceGetter::MediaMetadata::IsSuccess, mObj is NULL");
        return FALSE;
    }
    return mObj->IsSuccess();
}


//=====================================================================
//                         MediaResourceGetter
//=====================================================================

const String MediaResourceGetter::TAG("MediaResourceGetter");

AutoPtr<MediaResourceGetter::MediaMetadata> MediaResourceGetter::Extract(
    /* [in] */ Int32 fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    if (!AndroidDeviceOk(Build::MODEL, Build::VERSION::SDK_INT)) {
        return EMPTY_METADATA;
    }

    Configure(fd, offset, length);

    return DoExtractMetadata();
}

AutoPtr<MediaResourceGetter::MediaMetadata> MediaResourceGetter::Extract(
    /* [in] */ /*const*/ IContext* context,
    /* [in] */ const String& url,
    /* [in] */ const String& cookies,
    /* [in] */ const String& userAgent)
{
    if (!AndroidDeviceOk(Build::MODEL, Build::VERSION::SDK_INT)) {
        return EMPTY_METADATA;
    }

    if (!Configure(context, url, cookies, userAgent)) {
        Slogger::E(TAG, "Unable to configure metadata extractor");
        return EMPTY_METADATA;
    }

    return DoExtractMetadata();
}

Boolean MediaResourceGetter::Configure(
    /* [in] */ IContext* context,
    /* [in] */ const String& url,
    /* [in] */ const String& cookies,
    /* [in] */ const String& userAgent)
{
    AutoPtr<IURI> uri;
    // try {
        assert(0);
        // TODO
        // uri = URI.create(url);
    // } catch (IllegalArgumentException  e) {
    //     Log.e(TAG, "Cannot parse uri.", e);
    //     return false;
    // }

    String scheme;
    uri->GetScheme(&scheme);
    if (scheme == NULL || scheme.Equals("file")) {
        String path;
        uri->GetPath(&path);
        AutoPtr<IFile> file = UriToFile(path);
        Boolean bExists = FALSE;
        file->Exists(&bExists);
        if (!bExists) {
            Slogger::E(TAG, "File does not exist.");
            return FALSE;
        }

        if (!FilePathAcceptable(file)) {
            Slogger::E(TAG, "Refusing to read from unsafe file location.");
            return FALSE;
        }
    //     try {
            String p;
            file->GetAbsolutePath(&p);
            Configure(p);
            return TRUE;
    //     } catch (RuntimeException e) {
    //         Log.e(TAG, "Error configuring data source", e);
    //         return false;
    //    }
    } else {
        String host;
        uri->GetHost(&host);
        if (!IsLoopbackAddress(host) && !IsNetworkReliable(context)) {
            Slogger::W(TAG, "non-file URI can't be read due to unsuitable network conditions");
           return FALSE;
        }

        AutoPtr<IMap> headersMap;
        assert(0);
        // TODO
        // CHashMap::New((IMap**)&headersMap);
        if (!TextUtils::IsEmpty(cookies)) {
            assert(0);
            // TODO
            // headersMap.put("Cookie", cookies);
        }

        if (!TextUtils::IsEmpty(userAgent)) {
            assert(0);
            // TODO
            // headersMap.put("User-Agent", userAgent);
        }
    //     try {
            Configure(url, headersMap);
            return TRUE;
    //     } catch (RuntimeException e) {
    //         Log.e(TAG, "Error configuring data source", e);
    //         return false;
    //     }
    }
}

Boolean MediaResourceGetter::IsNetworkReliable(
    /* [in] */ IContext* context)
{
    Int32  permission;
    assert(0 && "TODO");
    // context->CheckCallingOrSelfPermission(
    //         Elastos::Droid::Manifest::permission::ACCESS_NETWORK_STATE, &permission);
    if (permission != IPackageManager::PERMISSION_GRANTED) {
        Slogger::W(TAG, "permission denied to access network state");
        return FALSE;
    }

    AutoPtr<IInteger32> networkType = GetNetworkType(context);
    if (networkType == NULL) {
        return FALSE;
    }

    Int32 intValue;
    networkType->GetValue(&intValue);
    switch (intValue) {
        case IConnectivityManager::TYPE_ETHERNET:
        case IConnectivityManager::TYPE_WIFI:
            Slogger::D(TAG, "ethernet/wifi connection detected");
            return TRUE;
        case IConnectivityManager::TYPE_WIMAX:
        case IConnectivityManager::TYPE_MOBILE:
        default:
            Slogger::D(TAG, "no ethernet/wifi connection detected");
            return FALSE;
    }
}

Boolean MediaResourceGetter::FilePathAcceptable(
    /* [in] */ IFile* file)
{
    String path;
    // try {
        file->GetCanonicalPath(&path);
    // } catch (IOException e) {
    //     // Canonicalization has failed. Assume malicious, give up.
    //     Log.w(TAG, "canonicalization of file path failed");
    //     return false;
    // }

    // In order to properly match the roots we must also canonicalize the
    // well-known paths we are matching against. If we don't, then we can
    // get unusual results in testing systems or possibly on rooted devices.
    // Note that canonicalized directory paths always end with '/'.
    AutoPtr<IList> acceptablePaths = Canonicalize(GetRawAcceptableDirectories());
    assert(0);
    // TODO
    // acceptablePaths->Add(GetExternalStorageDirectory());
    String pathLog("canonicalized file path: ");
    pathLog += path;
    Slogger::D(TAG, pathLog);
    AutoPtr<IIterable> iterable = IIterable::Probe(acceptablePaths);
    AutoPtr<IIterator> iter;
    iterable->GetIterator((IIterator**)&iter);
    Boolean bNext = FALSE;
    for (iter->HasNext(&bNext); bNext; iter->HasNext(&bNext)) {
        AutoPtr<ICharSequence> cs;
        iter->GetNext((IInterface**)&cs);
        String acceptablePath;
        cs->ToString(&acceptablePath);
        if (path.StartWith(acceptablePath)) {
            return TRUE;
        }
    }

    return FALSE;
}

Boolean MediaResourceGetter::AndroidDeviceOk(
    /* [in] */ const String& model,
    /* [in] */ const Int32& sdkVersion)
{
    return !(String("GT-I9100")./*Content*/Equals(model) &&
              sdkVersion < Build::VERSION_CODES::JELLY_BEAN);
}

AutoPtr<IFile> MediaResourceGetter::UriToFile(
    /* [in] */ const String& path)
{
    AutoPtr<IFile> file;
    assert(0);
    // TODO
    // CFile::New(path, (IFile**)&file);
    return file;
}

AutoPtr<IInteger32> MediaResourceGetter::GetNetworkType(
    /* [in] */ IContext* context)
{
    // TODO(qinmin): use ConnectionTypeObserver to listen to the network type change.
    AutoPtr<IConnectivityManager> mConnectivityManager;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&mConnectivityManager);
    if (mConnectivityManager == NULL) {
        Slogger::W(TAG, "no connectivity manager available");
        return NULL;
    }

    AutoPtr<INetworkInfo> info;
    mConnectivityManager->GetActiveNetworkInfo((INetworkInfo**)&info);
    if (info == NULL) {
        Slogger::D(TAG, "no active network");
        return NULL;
    }

    Int32 type;
    info->GetType(&type);
    AutoPtr<IInteger32> iType;
    assert(0);
    // TODO
    // CInteger32::New(type, (IInteger32**)&iType);

    return iType;
}

String MediaResourceGetter::GetExternalStorageDirectory()
{
    return PathUtils::GetExternalStorageDirectory();
}

ECode MediaResourceGetter::Configure(
    /* [in] */ Int32 fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    AutoPtr<IParcelFileDescriptorHelper> helper;
    assert(0);
    // TODO
    // CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
    AutoPtr<IParcelFileDescriptor> parcelFd;
    helper->AdoptFd(fd, (IParcelFileDescriptor**)&parcelFd);

    // try {
        AutoPtr<IFileDescriptor> fileDescriptor;
        parcelFd->GetFileDescriptor((IFileDescriptor**)&fileDescriptor);
        mRetriever->SetDataSource(fileDescriptor,
                offset, length);
    // } finally {
    //     try {
            parcelFd->Close();
    //     } catch (IOException e) {
    //         Log.e(TAG, "Failed to close file descriptor: " + e);
    //     }
    // }

    return NOERROR;
}

ECode MediaResourceGetter::Configure(
    /* [in] */ const String& url,
    /* [in] */ IMap* headers)
{
    mRetriever->SetDataSource(url, headers);
    return NOERROR;
}

ECode MediaResourceGetter::Configure(
    /* [in] */ const String& path)
{
    mRetriever->SetDataSource(path);
    return NOERROR;
}

String MediaResourceGetter::ExtractMetadata(
    /* [in] */ Int32 key)
{
    String Metadata;
    mRetriever->ExtractMetadata(key, &Metadata);
    return Metadata;
}

AutoPtr<MediaResourceGetter::MediaMetadata> MediaResourceGetter::ExtractMediaMetadata(
    /* [in] */ /*const*/ IContext* context,
    /* [in] */ const String& url,
    /* [in] */ const String& cookies,
    /* [in] */ const String& userAgent)
{
    AutoPtr<MediaResourceGetter> getter = new MediaResourceGetter();
    return getter->Extract(
             context, url, cookies, userAgent);
}

AutoPtr<IInterface> MediaResourceGetter::ExtractMediaMetadataFromFd(
    /* [in] */ Int32 fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    AutoPtr<MediaResourceGetter> getter = new MediaResourceGetter();
    return TO_IINTERFACE(getter->Extract(fd, offset, length));
}

AutoPtr<MediaResourceGetter::MediaMetadata> MediaResourceGetter::DoExtractMetadata()
{
    // try {
        String durationString = ExtractMetadata(
                IMediaMetadataRetriever::METADATA_KEY_DURATION);
        if (durationString == NULL) {
            Slogger::W(TAG, "missing duration metadata");
            return EMPTY_METADATA;
        }

        Int32 durationMillis = 0;
    //     try {
            durationMillis = StringUtils::ParseInt32(durationString);
    //     } catch (NumberFormatException e) {
    //         Log.w(TAG, "non-numeric duration: " + durationString);
    //         return EMPTY_METADATA;
    //     }
    //
        Int32 width = 0;
        Int32 height = 0;
        Boolean hasVideo = String("yes").Equals(ExtractMetadata(
                IMediaMetadataRetriever::METADATA_KEY_HAS_VIDEO));
        Slogger::D(TAG, (hasVideo ? "resource has video" : "resource doesn't have video"));
        if (hasVideo) {
            String widthString = ExtractMetadata(
                    IMediaMetadataRetriever::METADATA_KEY_VIDEO_WIDTH);
            if (widthString == NULL) {
                Slogger::W(TAG, "missing video width metadata");
                return EMPTY_METADATA;
            }
    //         try {
                width = StringUtils::ParseInt32(widthString);
    //         } catch (NumberFormatException e) {
    //             Log.w(TAG, "non-numeric width: " + widthString);
    //             return EMPTY_METADATA;
    //         }
    //
            String heightString = ExtractMetadata(
                    IMediaMetadataRetriever::METADATA_KEY_VIDEO_HEIGHT);
            if (heightString == NULL) {
                Slogger::W(TAG, "missing video height metadata");
                return EMPTY_METADATA;
            }
    //         try {
                height = StringUtils::ParseInt32(heightString);
    //         } catch (NumberFormatException e) {
    //             Log.w(TAG, "non-numeric height: " + heightString);
    //             return EMPTY_METADATA;
    //         }
        }
        AutoPtr<MediaMetadata> result = new MediaMetadata(durationMillis, width, height, true);
        String strLog("extracted valid metadata: ");
        strLog += result->ToString();
        Slogger::D(TAG, strLog);
        return result;
    // } catch (RuntimeException e) {
    //     Log.e(TAG, "Unable to extract medata", e);
    //     return EMPTY_METADATA;
    // }
}

Boolean MediaResourceGetter::IsLoopbackAddress(
    /* [in] */ const String& host)
{
    return host != NULL && (host.EqualsIgnoreCase("localhost")  // typical hostname
            || host.Equals("127.0.0.1")  // typical IP v4 expression
            || host.Equals("[::1]"));  // typical IP v6 expression
}

AutoPtr<IList> MediaResourceGetter::GetRawAcceptableDirectories()
{
    AutoPtr<IList> result;
    assert(0);
    // TODO
    // CArrayList::New((IList**)&result);
    // result.add("/mnt/sdcard/");
    // result.add("/sdcard/");
    return result;
}

AutoPtr<IList> MediaResourceGetter::Canonicalize(
    /* [in] */ IList* paths)
{
    AutoPtr<IList> result;
    Int32 size;
    paths->GetSize(&size);
    assert(0);
    // TODO
    // CArrayList::New(size, (IList**)&result);

    // try {
        AutoPtr<IIterable> iterable = IIterable::Probe(paths);
        AutoPtr<IIterator> iter;
        paths->GetIterator((IIterator**)&iter);
        AutoPtr<ICharSequence> pathCS;
        Boolean bNext;
        for (iter->HasNext(&bNext); bNext; iter->HasNext(&bNext)) {
            iter->GetNext((IInterface**)&pathCS);
            String path;
            pathCS->ToString(&path);
            AutoPtr<IFile> file;
            assert(0);
            // TODO
            // CFile::New(path, (IFile**)&file);
            String pathfile;
            file->GetCanonicalPath(&pathfile);
            assert(0);
            // TODO
            // result->Add(pathfile);
        }
        return result;
    // } catch (IOException e) {
    //     // Canonicalization has failed. Assume malicious, give up.
    //     Log.w(TAG, "canonicalization of file path failed");
    // }
    // return result;
}

AutoPtr<IInterface> MediaResourceGetter::ExtractMediaMetadata(
    /* [in] */ IInterface* context,
    /* [in] */ const String& url,
    /* [in] */ const String& cookies,
    /* [in] */ const String& userAgent)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    return TO_IINTERFACE(ExtractMediaMetadata(context, url, cookies, userAgent));
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
