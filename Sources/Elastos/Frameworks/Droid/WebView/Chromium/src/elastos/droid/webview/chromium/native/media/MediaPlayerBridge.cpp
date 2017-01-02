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
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/media/MediaPlayerBridge.h"
#include "elastos/droid/webkit/webview/chromium/native/media/api/MediaPlayerBridge_dec.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/Build.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IBase64InputStream;
using Elastos::Droid::Utility::CBase64InputStream;
using Elastos::Droid::Utility::IBase64;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::CMediaPlayer;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::IMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

//===============================================================
//             MediaPlayerBridge::LoadDataUriTask
//===============================================================

MediaPlayerBridge::LoadDataUriTask::LoadDataUriTask(
    /* [in] */ MediaPlayerBridge* owner,
    /* [in] */ IContext* context,
    /* [in] */ const String& data)
    : mOwner(owner)
    , mData(data)
    , mContext(context)
{
}

//@Override
ECode MediaPlayerBridge::LoadDataUriTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Boolean runResult = FALSE;//update runResult when fail
    AutoPtr<IFileOutputStream> fos;
    // try {
    AutoPtr<IFileHelper> fileHelper;
    CFileHelper::AcquireSingleton((IFileHelper**)&fileHelper);
    fileHelper->CreateTempFile(String("decoded"), String("mediadata"), (IFile**)&mTempFile);
    CFileOutputStream::New(mTempFile, (IFileOutputStream**)&fos);
    AutoPtr<ArrayOf<Byte> > data = mData.GetBytes();
    AutoPtr<IInputStream> stream;
    CByteArrayInputStream::New(data, (IInputStream**)&stream);
    AutoPtr<IBase64InputStream> decoder;
    CBase64InputStream::New(stream, IBase64::DEFAULT, (IBase64InputStream**)&decoder);
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1024);
    Int32 len;
    AutoPtr<IInputStream> decoderIS = IInputStream::Probe(decoder);
    AutoPtr<IOutputStream> fosOS = IOutputStream::Probe(fos);
    while ((decoderIS->Read(buffer, &len), len) != -1) {
        fosOS->Write(buffer, 0, len);
    }
    decoderIS->Close();
    //    return true;
    // } catch (IOException e) {
    //     return false;
    // } finally {
    //     try {
    if (fosOS != NULL) fosOS->Close();
    //     } catch (IOException e) {
    //         // Can't do anything.
    //     }
    // }

    runResult = TRUE;
    AutoPtr<IBoolean> res;
    CBoolean::New(runResult, (IBoolean**)&res);
    *result = res;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//@Override
ECode MediaPlayerBridge::LoadDataUriTask::OnPostExecute(
    /* [in] */ IInterface* iresult)
{
    Boolean result;
    AutoPtr<IBoolean> ib = IBoolean::Probe(iresult);
    ib->GetValue(&result);
    if (IsCancelled()) {
        DeleteFile();
        return NOERROR;
    }

    // try {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromFile(mTempFile, (IUri**)&uri);
        mOwner->GetLocalPlayer()->SetDataSource(mContext, uri);
    // } catch (IOException e) {
    //     result = false;
    // }

    DeleteFile();
    assert (mOwner->mNativeMediaPlayerBridge != 0);
    mOwner->NativeOnDidSetDataUriDataSource(mOwner->mNativeMediaPlayerBridge, result);
    return NOERROR;
}

void MediaPlayerBridge::LoadDataUriTask::DeleteFile()
{
    if (mTempFile == NULL) return;

    Boolean bDelete = FALSE;
    mTempFile->Delete(&bDelete);
    String file;
    mTempFile->ToString(&file);
    if (!bDelete) {
        // File will be deleted when MediaPlayer releases its handler.
        Logger::E(MediaPlayerBridge::TAG, "Failed to delete temporary file: %s", file.string());
        assert (FALSE);
    }
}

//===============================================================
//             MediaPlayerBridge::AllowedOperations
//===============================================================

MediaPlayerBridge::AllowedOperations::AllowedOperations(
    /* [in] */ Boolean canPause,
    /* [in] */ Boolean canSeekForward,
    /* [in] */ Boolean canSeekBackward)
    : mCanPause(canPause)
    , mCanSeekForward(canSeekForward)
    , mCanSeekBackward(canSeekBackward)
{
}

//@CalledByNative("AllowedOperations")
Boolean MediaPlayerBridge::AllowedOperations::CanPause()
{
    return mCanPause;
}

//@CalledByNative("AllowedOperations")
Boolean MediaPlayerBridge::AllowedOperations::CanSeekForward()
{
    return mCanSeekForward;
}

//@CalledByNative("AllowedOperations")
Boolean MediaPlayerBridge::AllowedOperations::CanSeekBackward()
{
    return mCanSeekBackward;
}

Boolean MediaPlayerBridge::AllowedOperations::CanPause(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge::AllowedOperations> mObj = (MediaPlayerBridge::AllowedOperations*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::AllowedOperations::CanPause, mObj is NULL");
        return FALSE;
    }
    return mObj->CanPause();
}

Boolean MediaPlayerBridge::AllowedOperations::CanSeekForward(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge::AllowedOperations> mObj = (MediaPlayerBridge::AllowedOperations*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::AllowedOperations::CanSeekForward, mObj is NULL");
        return FALSE;
    }
    return mObj->CanSeekForward();
}

Boolean MediaPlayerBridge::AllowedOperations::CanSeekBackward(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge::AllowedOperations> mObj = (MediaPlayerBridge::AllowedOperations*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::AllowedOperations::CanSeekBackward, mObj is NULL");
        return FALSE;
    }
    return mObj->CanSeekBackward();
}

//===============================================================
//                      MediaPlayerBridge
//===============================================================

const String MediaPlayerBridge::TAG("MediaPlayerBridge");

MediaPlayerBridge::MediaPlayerBridge()
    : mNativeMediaPlayerBridge(0)
{
}

MediaPlayerBridge::MediaPlayerBridge(
    /* [in] */ Handle64 nativeMediaPlayerBridge)
    : mNativeMediaPlayerBridge(nativeMediaPlayerBridge)
{
}

//@CalledByNative return MediaPlayerBridge
AutoPtr<IInterface> MediaPlayerBridge::Create(
    /* [in] */ Handle64 nativeMediaPlayerBridge)
{
    AutoPtr<MediaPlayerBridge> mpb = new MediaPlayerBridge(nativeMediaPlayerBridge);
    AutoPtr<IInterface> result = TO_IINTERFACE(mpb);
    return result;
}

//@CalledByNative
void MediaPlayerBridge::Destroy()
{
    if (mLoadDataUriTask != NULL) {
        mLoadDataUriTask->Cancel(TRUE);
        mLoadDataUriTask = NULL;
    }
    mNativeMediaPlayerBridge = 0;
}

AutoPtr<IMediaPlayer> MediaPlayerBridge::GetLocalPlayer()
{
    if (mPlayer == NULL) {
        //mPlayer = new MediaPlayer();
        CMediaPlayer::New((IMediaPlayer**)&mPlayer);
    }
    return mPlayer;
}

//@CalledByNative
void MediaPlayerBridge::SetSurface(
    /* [in] */ ISurface* surface)
{
    GetLocalPlayer()->SetSurface(surface);
}

//@CalledByNative
Boolean MediaPlayerBridge::PrepareAsync()
{
    // try {
        ECode ecode = GetLocalPlayer()->PrepareAsync();
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "Unable to prepare MediaPlayer.", e);
    //     return false;
    // }
    if (FAILED(ecode))
    {
        Logger::E(TAG, "Unable to prepare MediaPlayer.ecode:0x%x", ecode);
        return FALSE;
    }

    return TRUE;
}

//@CalledByNative
Boolean MediaPlayerBridge::IsPlaying()
{
    Boolean isPlaying;
    GetLocalPlayer()->IsPlaying(&isPlaying);
    return isPlaying;
}

//@CalledByNative
Int32 MediaPlayerBridge::GetVideoWidth()
{
    Int32 width;
    GetLocalPlayer()->GetVideoWidth(&width);
    return width;
}

//@CalledByNative
Int32 MediaPlayerBridge::GetVideoHeight()
{
    Int32 height;
    GetLocalPlayer()->GetVideoHeight(&height);
    return height;
}

//@CalledByNative
Int32 MediaPlayerBridge::GetCurrentPosition()
{
    Int32 position;
    GetLocalPlayer()->GetCurrentPosition(&position);
    return position;
}

//@CalledByNative
Int32 MediaPlayerBridge::GetDuration()
{
    Int32 duration;
    GetLocalPlayer()->GetDuration(&duration);
    return duration;
}

//@CalledByNative
void MediaPlayerBridge::ReleaseReSources()
{
    GetLocalPlayer()->ReleaseResources();
}

//@CalledByNative
void MediaPlayerBridge::SetVolume(
    /* [in] */ Double volume)
{
    GetLocalPlayer()->SetVolume((Float) volume, (Float) volume);
}

//@CalledByNative
void MediaPlayerBridge::Start()
{
    GetLocalPlayer()->Start();
}

//@CalledByNative
void MediaPlayerBridge::Pause()
{
    GetLocalPlayer()->Pause();
}

//@CalledByNative
void MediaPlayerBridge::SeekTo(
    /* [in] */ Int32 msec)
{
    GetLocalPlayer()->SeekTo(msec);
}

//@CalledByNative
Boolean MediaPlayerBridge::SetDataSource(
    /* [in] */ IContext* context,
    /* [in] */ const String& url,
    /* [in] */ const String& cookies,
    /* [in] */ const String& userAgent,
    /* [in] */ Boolean hideUrlLog)
{
    //Uri uri = Uri.parse(url);
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(url, (IUri**)&uri);
    //HashMap<String, String> headersMap = new HashMap<String, String>();
    AutoPtr<IMap> headersMap;
    CHashMap::New((IMap**)&headersMap);

    if (hideUrlLog)
    {
        AutoPtr<ICharSequence> key;
        AutoPtr<ICharSequence> value;
        CString::New(String("x-hide-urls-from-log"), (ICharSequence**)&key);
        CString::New(String("true"), (ICharSequence**)&value);
        headersMap->Put(key, value);
    }
    if (!TextUtils::IsEmpty(cookies))
    {
        AutoPtr<ICharSequence> key;
        AutoPtr<ICharSequence> value;
        CString::New(String("Cookie"), (ICharSequence**)&key);
        CString::New(cookies, (ICharSequence**)&value);
        headersMap->Put(key, value);
    }
    if (!TextUtils::IsEmpty(userAgent))
    {
        AutoPtr<ICharSequence> key;
        AutoPtr<ICharSequence> value;
        CString::New(String("User-Agent"), (ICharSequence**)&key);
        CString::New(userAgent, (ICharSequence**)&value);
        headersMap->Put(key, value);
    }
    // The security origin check is enforced for devices above K. For devices below K,
    // only anonymous media HTTP request (no cookies) may be considered same-origin.
    // Note that if the server rejects the request we must not consider it same-origin.
    if (Build::VERSION::SDK_INT > Build::VERSION_CODES::KITKAT) {
        //headersMap.put("allow-cross-domain-redirect", "false");
        AutoPtr<ICharSequence> key;
        AutoPtr<ICharSequence> value;
        CString::New(String("allow-cross-domain-redirect"), (ICharSequence**)&key);
        CString::New(String("false"), (ICharSequence**)&value);
        headersMap->Put(key, value);
    }
    //try {
        ECode ecode = GetLocalPlayer()->SetDataSource(context, uri, headersMap);
        if (FAILED(ecode))
        {
            return FALSE;
        }
        return TRUE;
    //} catch (Exception e) {
    //    return false;
    //}
}

//@CalledByNative
Boolean MediaPlayerBridge::SetDataSourceFromFd(
    /* [in] */ Int32 fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    // try {
        AutoPtr<IParcelFileDescriptorHelper> helper;
        CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
        AutoPtr<IParcelFileDescriptor> parcelFd;
        ECode ecode = helper->AdoptFd(fd, (IParcelFileDescriptor**)&parcelFd);
        if (FAILED(ecode))
        {
            Logger::E(TAG, "Failed to set data source from file descriptor: ecode:0x%x", ecode);
            return FALSE;
        }
        AutoPtr<IFileDescriptor> des;
        ecode = parcelFd->GetFileDescriptor((IFileDescriptor**)&des);
        if (FAILED(ecode))
        {
            Logger::E(TAG, "Failed to set data source from file descriptor: ecode:0x%x", ecode);
            return FALSE;
        }
        ecode = GetLocalPlayer()->SetDataSource(des, offset, length);
        if (FAILED(ecode))
        {
            Logger::E(TAG, "Failed to set data source from file descriptor: ecode:0x%x", ecode);
            return FALSE;
        }
        ecode = parcelFd->Close();
        if (FAILED(ecode))
        {
            Logger::E(TAG, "Failed to set data source from file descriptor: ecode:0x%x", ecode);
            return FALSE;
        }
        return TRUE;
    // } catch (IOException e) {
    //     Log.e(TAG, "Failed to set data source from file descriptor: " + e);
    //     return false;
    // }
}

//@CalledByNative
Boolean MediaPlayerBridge::SetDataUriDataSource(
    /* [in] */ IContext* context,
    /* [in] */ const String& url)
{
    if (mLoadDataUriTask != NULL) {
        mLoadDataUriTask->Cancel(TRUE);
        mLoadDataUriTask = NULL;
    }

    if (!url.StartWith("data:")) return FALSE;
    Int32 headerStop = url.IndexOf(',');
    if (headerStop == -1) return FALSE;
    String header = url.Substring(0, headerStop);
    const String data = url.Substring(headerStop + 1);

    String headerContent = header.Substring(5);
    //AutoPtr< ArrayOf<String> > headerInfo = headerContent.Split(";");
    //if (headerInfo->GetLength() != 2) return FALSE;
    //if (!(*headerInfo)[1].Equals("base64")) return FALSE;
    //TODO pay attention here
    Int32 scStop = headerContent.IndexOf(";");
    if (scStop == -1) return FALSE;
    String codec = headerContent.Substring(scStop + 1);
    scStop = codec.IndexOf(";");
    if (scStop != -1)
    {
        codec = codec.Substring(0, scStop);
    }
    if (!codec.Equals("base64")) return FALSE;

    mLoadDataUriTask = new LoadDataUriTask(this, context, data);
    //TODO is this OK, the params no use here
    AutoPtr<ArrayOf<IInterface*> > params;
    mLoadDataUriTask->Execute(params);
    return TRUE;
}


void MediaPlayerBridge::SetOnBufferingUpdateListener(
    /* [in] */ IMediaPlayerOnBufferingUpdateListener* listener)
{
    GetLocalPlayer()->SetOnBufferingUpdateListener(listener);
}

void MediaPlayerBridge::SetOnCompletionListener(
    /* [in] */ IMediaPlayerOnCompletionListener* listener)
{
    GetLocalPlayer()->SetOnCompletionListener(listener);
}

void MediaPlayerBridge::SetOnErrorListener(
    /* [in] */ IMediaPlayerOnErrorListener* listener)
{
    GetLocalPlayer()->SetOnErrorListener(listener);
}

void MediaPlayerBridge::SetOnPreparedListener(
    /* [in] */ IMediaPlayerOnPreparedListener* listener)
{
    GetLocalPlayer()->SetOnPreparedListener(listener);
}

void MediaPlayerBridge::SetOnSeekCompleteListener(
    /* [in] */ IMediaPlayerOnSeekCompleteListener* listener)
{
    GetLocalPlayer()->SetOnSeekCompleteListener(listener);
}

void MediaPlayerBridge::SetOnVideoSizeChangedListener(
    /* [in] */ IMediaPlayerOnVideoSizeChangedListener* listener)
{
    GetLocalPlayer()->SetOnVideoSizeChangedListener(listener);
}

/**
 * Returns an AllowedOperations object to show all the operations that are
 * allowed on the media player.
 */
//@CalledByNative
AutoPtr<MediaPlayerBridge::AllowedOperations> MediaPlayerBridge::GetAllowedOperations()
{
    AutoPtr<IMediaPlayer> player = GetLocalPlayer();
    Boolean canPause = TRUE;
    Boolean canSeekForward = TRUE;
    Boolean canSeekBackward = TRUE;
    // try {
    /*TODO
      //more check on this, because below code use the reflection function
        Method getMetadata = player.getClass().getDeclaredMethod(
                "getMetadata", boolean.class, boolean.class);
        getMetadata.setAccessible(true);
        Object data = getMetadata.invoke(player, false, false);
        if (data != null) {
            Class<?> metadataClass = data.getClass();
            Method hasMethod = metadataClass.getDeclaredMethod("has", int.class);
            Method getBooleanMethod = metadataClass.getDeclaredMethod("getBoolean", int.class);

            int pause = (Integer) metadataClass.getField("PAUSE_AVAILABLE").get(null);
            int seekForward =
                (Integer) metadataClass.getField("SEEK_FORWARD_AVAILABLE").get(null);
            int seekBackward =
                    (Integer) metadataClass.getField("SEEK_BACKWARD_AVAILABLE").get(null);
            hasMethod.setAccessible(true);
            getBooleanMethod.setAccessible(true);
            canPause = !((Boolean) hasMethod.invoke(data, pause))
                    || ((Boolean) getBooleanMethod.invoke(data, pause));
            canSeekForward = !((Boolean) hasMethod.invoke(data, seekForward))
                    || ((Boolean) getBooleanMethod.invoke(data, seekForward));
            canSeekBackward = !((Boolean) hasMethod.invoke(data, seekBackward))
                    || ((Boolean) getBooleanMethod.invoke(data, seekBackward));
        }
    // } catch (NoSuchMethodException e) {
    //     Log.e(TAG, "Cannot find getMetadata() method: " + e);
    // } catch (InvocationTargetException e) {
    //     Log.e(TAG, "Cannot invoke MediaPlayer.getMetadata() method: " + e);
    // } catch (IllegalAccessException e) {
    //     Log.e(TAG, "Cannot access metadata: " + e);
    // } catch (NoSuchFieldException e) {
    //     Log.e(TAG, "Cannot find matching fields in Metadata class: " + e);
    // }

    */
    AutoPtr<AllowedOperations> ret = new AllowedOperations(canPause, canSeekForward, canSeekBackward);

    return ret;
}

void MediaPlayerBridge::NativeOnDidSetDataUriDataSource(
    /* [in] */ Handle64 nativeMediaPlayerBridge,
    /* [in] */ Boolean success)
{
    Elastos_MediaPlayerBridge_nativeOnDidSetDataUriDataSource(TO_IINTERFACE(this), nativeMediaPlayerBridge, success);
}

void MediaPlayerBridge::Destroy(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::Destroy, mObj is NULL");
        return;
    }
    mObj->Destroy();
}

void MediaPlayerBridge::SetSurface(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* surface)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::SetSurface, mObj is NULL");
        return;
    }
    AutoPtr<ISurface> s = ISurface::Probe(surface);
    mObj->SetSurface(s);
}

Boolean MediaPlayerBridge::PrepareAsync(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::PrepareAsync, mObj is NULL");
        return FALSE;
    }
    return mObj->PrepareAsync();
}

Boolean MediaPlayerBridge::IsPlaying(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::IsPlaying, mObj is NULL");
        return FALSE;
    }
    return mObj->IsPlaying();
}

Int32 MediaPlayerBridge::GetVideoWidth(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::GetVideoWidth, mObj is NULL");
        return 0;
    }
    return mObj->GetVideoWidth();
}

Int32 MediaPlayerBridge::GetVideoHeight(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::GetVideoHeight, mObj is NULL");
        return 0;
    }
    return mObj->GetVideoHeight();
}

Int32 MediaPlayerBridge::GetCurrentPosition(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::GetCurrentPosition, mObj is NULL");
        return 0;
    }
    return mObj->GetCurrentPosition();
}

Int32 MediaPlayerBridge::GetDuration(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::GetDuration, mObj is NULL");
        return 0;
    }
    return mObj->GetDuration();
}

void MediaPlayerBridge::ReleaseResources(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::ReleaseResources, mObj is NULL");
        return;
    }
    mObj->ReleaseReSources();
}

void MediaPlayerBridge::SetVolume(
    /* [in] */ IInterface* obj,
    /* [in] */ Double volume)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::SetVolume, mObj is NULL");
        return;
    }
    mObj->SetVolume(volume);
}

void MediaPlayerBridge::Start(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::Start, mObj is NULL");
        return;
    }
    mObj->Start();
}

void MediaPlayerBridge::Pause(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::Pause, mObj is NULL");
        return;
    }
    mObj->Pause();
}

void MediaPlayerBridge::SeekTo(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 msec)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::SeekTo, mObj is NULL");
        return;
    }
    mObj->SeekTo(msec);
}

Boolean MediaPlayerBridge::SetDataSource(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* context,
    /* [in] */ const String& url,
    /* [in] */ const String& cookies,
    /* [in] */ const String& userAgent,
    /* [in] */ Boolean hideUrlLog)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::SetDataSource, mObj is NULL");
        return FALSE;
    }
    AutoPtr<IContext> c = IContext::Probe(context);
    return mObj->SetDataSource(c, url, cookies, userAgent, hideUrlLog);
}

Boolean MediaPlayerBridge::SetDataSourceFromFd(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::SetDataSourceFromFd, mObj is NULL");
        return FALSE;
    }
    return mObj->SetDataSourceFromFd(fd, offset, length);
}

Boolean MediaPlayerBridge::SetDataUriDataSource(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* context,
    /* [in] */ const String& url)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::SetDataUriDataSource, mObj is NULL");
        return FALSE;
    }
    AutoPtr<IContext> c = IContext::Probe(context);
    return mObj->SetDataUriDataSource(c, url);
}

AutoPtr<IInterface> MediaPlayerBridge::GetAllowedOperations(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerBridge> mObj = (MediaPlayerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaPlayerBridge::GetAllowedOperations, mObj is NULL");
        return NULL;
    }
    AutoPtr<MediaPlayerBridge::AllowedOperations> ao= mObj->GetAllowedOperations();
    return TO_IINTERFACE(ao);
} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
} // namespace Elastos
