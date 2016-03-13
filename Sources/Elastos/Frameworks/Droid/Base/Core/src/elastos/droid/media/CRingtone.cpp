
#include "elastos/droid/media/CAudioAttributes.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/media/CAudioAttributesHelper.h"
#include "elastos/droid/media/CMediaPlayer.h"
#include "elastos/droid/media/CRingtone.h"
#include "elastos/droid/media/CRingtoneManager.h"
#include "elastos/droid/media/CRingtoneManagerHelper.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Database.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/StringUtils.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreAudioPlaylistsMembers;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::R;
using Elastos::IO::ICloseable;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

const String CRingtone::TAG("CRingtone");
const Boolean CRingtone::LOGD = TRUE;

static AutoPtr<ArrayOf<String> > InitMEDIA_COLUMNS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    array->Set(0, IMediaStoreAudioPlaylistsMembers::ID);
    array->Set(1, IMediaStoreMediaColumns::DATA);
    array->Set(2, IMediaStoreMediaColumns::TITLE);
    return array;
}

AutoPtr<ArrayOf<String> > CRingtone::MEDIA_COLUMNS = InitMEDIA_COLUMNS();

CAR_OBJECT_IMPL(CRingtone)

CAR_INTERFACE_IMPL(CRingtone, Object, IRingtone)

CRingtone::CRingtone()
    : mAllowRemote(FALSE)
    // , mStreamType(IAudioManager::STREAM_RING)
{}

CRingtone::~CRingtone()
{}

ECode CRingtone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean allowRemote)
{
    mContext = context;

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);

    mAllowRemote = allowRemote;
    if (allowRemote) {
        mAudioManager->GetRingtonePlayer((IIRingtonePlayer**)&mRemotePlayer);
        CBinder::New((IBinder**)&mRemoteToken);
    }

    AutoPtr<IAudioAttributesBuilder> ab;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&ab);
    ab->SetUsage(IAudioAttributes::USAGE_NOTIFICATION_RINGTONE);
    ab->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);

    return ab->Build((IAudioAttributes**)&mAudioAttributes);
}

ECode CRingtone::SetStreamType(
    /* [in] */ Int32 streamType)
{
    AutoPtr<IAudioAttributesBuilder> ab;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&ab);
    ab->SetInternalLegacyStreamType(streamType);
    AutoPtr<IAudioAttributes> attributes;
    ab->Build((IAudioAttributes**)&attributes);

    return SetAudioAttributes(attributes);
}

ECode CRingtone::GetStreamType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    AutoPtr<IAudioAttributesHelper> aa;
    CAudioAttributesHelper::AcquireSingleton((IAudioAttributesHelper**)&aa);
    return aa->ToLegacyStreamType(mAudioAttributes.Get(), type);
}

ECode CRingtone::SetAudioAttributes(
    /* [in] */ IAudioAttributes* attributes)
{
    if (attributes == NULL) {
        Slogger::E(TAG, "Invalid NULL AudioAttributes for Ringtone");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAudioAttributes = attributes;
    // The audio attributes have to be set before the media player is prepared.
    // Re-initialize it.
    return SetUri(mUri);
}

ECode CRingtone::GetAudioAttributes(
    /* [out] */ IAudioAttributes** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAudioAttributes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRingtone::GetTitle(
    /* [in] */ IContext* context,
    /* [out] */ String* title)
{
    VALIDATE_NOT_NULL(title);
    *title = NULL;

    if (!mTitle.IsNull()) {
        *title = mTitle;
        return NOERROR;
    }

    mTitle = GetTitle(context, mUri, TRUE);
    *title = mTitle;
    return NOERROR;
}

String CRingtone::GetTitle(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ Boolean followSettingsUri)
{
    AutoPtr<ICursor> cursor;
    AutoPtr<IContentResolver> res;
    context->GetContentResolver((IContentResolver**)&res);

    String title;
    if (uri != NULL) {
        String authority, nullStr;
        uri->GetAuthority(&authority);

        if (ISettings::AUTHORITY.Equals(authority)) {
            if (followSettingsUri) {
                AutoPtr<IRingtoneManagerHelper> helper;
                CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);

                Int32 type;
                helper->GetDefaultType(uri, &type);

                AutoPtr<IUri> actualUri;
                helper->GetActualDefaultRingtoneUri(context, type, (IUri**)&actualUri);
                String actualTitle = GetTitle(context, actualUri, FALSE);

                AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
                array->Set(0, StringUtils::ParseCharSequence(actualTitle).Get());
                context->GetString(R::string::ringtone_default_with_actual, array, &title);
            }
        }
        else {
            // try {
            if (IMediaStore::AUTHORITY.Equals(authority)) {
                res->Query(uri, MEDIA_COLUMNS, nullStr, NULL, nullStr, (ICursor**)&cursor);
            }
            // } catch (SecurityException e) {
                // missing cursor is handled below
            // }

            // try {
            Boolean result;
            Int32 count = 0;
            if (cursor) {
                cursor->GetCount(&count);
            }

            if (count == 1) {
                cursor->MoveToFirst(&result);
                cursor->GetString(2, &title);
            } else {
                uri->GetLastPathSegment(&title);
            }

            if (cursor != NULL) {
                ICloseable::Probe(cursor)->Close();
                cursor = NULL;
            }
        }
    }

    if (title.IsNull()) {
        context->GetString(R::string::ringtone_unknown, &title);

        if (title.IsNull()) {
            title = String("");
        }
    }

    return title;
}

ECode CRingtone::SetUri(
    /* [in] */ IUri* uri)
{
    DestroyLocalPlayer();

    mUri = uri;
    if (mUri == NULL) {
        return NOERROR;
    }

    // TODO: detect READ_EXTERNAL and specific content provider case, instead of relying on throwing
    // try opening uri locally before delegating to remote player
    CMediaPlayer::New((IMediaPlayer**)&mLocalPlayer);
    //try {
    ECode ec = mLocalPlayer->SetDataSource(mContext, mUri);
    FAIL_GOTO(ec, _EXIT_);

    ec = mLocalPlayer->SetAudioAttributes(mAudioAttributes);
    FAIL_GOTO(ec, _EXIT_);

    ec = mLocalPlayer->Prepare();
    FAIL_GOTO(ec, _EXIT_);

_EXIT_:
    //} catch (SecurityException e) {
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        DestroyLocalPlayer();
        if (!mAllowRemote) {
            Logger::E(TAG, "Remote playback not allowed: E_SECURITY_EXCEPTION");
        }
    }
    //} catch (IOException e) {
    else if (ec == (ECode)E_IO_EXCEPTION) {
        DestroyLocalPlayer();
        if (!mAllowRemote) {
            Logger::E(TAG, "Remote playback not allowed: E_IO_EXCEPTION");
        }
    }

    if (LOGD) {
        if (mLocalPlayer != NULL) {
            Logger::D(TAG, "Successfully created local player");
        }
        else {
            Logger::D(TAG, "Problem opening; delegating to remote player");
        }
    }

    return ec;
}

ECode CRingtone::GetUri(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUri;
    return NOERROR;
}

ECode CRingtone::Play()
{
    if (mLocalPlayer != NULL) {
        // do not play ringtones if stream volume is 0
        // (typically because ringer mode is silent).
        Int32 vol;
        CAudioAttributes::ToLegacyStreamType(mAudioAttributes, &vol);
        Int32 vol_;
        mAudioManager->GetStreamVolume(vol != 0, &vol_);
        if (vol_) {
            mLocalPlayer->Start();
        }
    }
    else if (mAllowRemote && mRemotePlayer != NULL) {
        AutoPtr<IUri> canonicalUri;
        mUri->GetCanonicalUri((IUri**)&canonicalUri);
        //try {
        ECode ec = mRemotePlayer->Play(mRemoteToken, canonicalUri, mAudioAttributes);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            if (!PlayFallbackRingtone()) {
                Logger::W(TAG, "Problem playing ringtone: E_REMOTE_EXCEPTION");
            }
        }
    }
    else {
        if (!PlayFallbackRingtone()) {
            Logger::W(TAG, "Neither local nor remote playback available");
        }
    }

    return NOERROR;
}

ECode CRingtone::Stop()
{
    if (mLocalPlayer != NULL) {
        DestroyLocalPlayer();
    }
    else if (mAllowRemote && (mRemotePlayer != NULL)) {
        //try {
        ECode ec = mRemotePlayer->Stop(mRemoteToken);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "Problem playing ringtone: E_REMOTE_EXCEPTION");
        }
        //} catch (RemoteException e) {
        //    Log.w(TAG, "Problem stopping ringtone: " + e);
        //}
    }

    return NOERROR;
}

void CRingtone::DestroyLocalPlayer()
{
    if (mLocalPlayer != NULL) {
        mLocalPlayer->Reset();
        mLocalPlayer->ReleaseResources();
        mLocalPlayer = NULL;
    }
}

ECode CRingtone::IsPlaying(
    /* [out] */ Boolean* playing)
{
    VALIDATE_NOT_NULL(playing);
    *playing = FALSE;

    if (mLocalPlayer != NULL) {
        return mLocalPlayer->IsPlaying(playing);
    }
    else if (mAllowRemote && (mRemotePlayer != NULL)) {
        //try {
        return mRemotePlayer->IsPlaying(mRemoteToken, playing);
        //} catch (RemoteException e) {
        //    Log.w(TAG, "Problem checking ringtone: " + e);
        //}
    }
    else {
        //Log.w(TAG, "Neither local nor remote playback available");
        *playing = FALSE;
        return NOERROR;
    }

    return NOERROR;
}

Boolean CRingtone::PlayFallbackRingtone()
{
    Int32 vol;
    CAudioAttributes::ToLegacyStreamType(mAudioAttributes, &vol);
    Int32 r;
    mAudioManager->GetStreamVolume(vol, &r);
    if (r != 0) {
        Int32 ringtoneType;
        CRingtoneManager::GetDefaultType(mUri, &ringtoneType);
        AutoPtr<IUri> uri;
        CRingtoneManager::GetActualDefaultRingtoneUri(mContext, ringtoneType, (IUri**)&uri);
        if (ringtoneType == -1 || uri != NULL) {
            // Default ringtone, try fallback ringtone.
            // try {
            AutoPtr<IResources> res;
            mContext->GetResources((IResources**)&res);
            AutoPtr<IAssetFileDescriptor> afd;
            res->OpenRawResourceFd(R::raw::fallbackring, (IAssetFileDescriptor**)&afd);
            if (afd != NULL) {
                CMediaPlayer::New((IMediaPlayer**)&mLocalPlayer);
                Int64 declaredLength;
                afd->GetDeclaredLength(&declaredLength);
                AutoPtr<IFileDescriptor> fd;
                afd->GetFileDescriptor((IFileDescriptor**)&fd);

                if (declaredLength < 0) {
                    mLocalPlayer->SetDataSource(fd);
                } else {
                    Int64 startOffset;
                    afd->GetStartOffset(&startOffset);
                    mLocalPlayer->SetDataSource(fd.Get(), startOffset, declaredLength);
                }
                mLocalPlayer->SetAudioAttributes(mAudioAttributes);
                mLocalPlayer->Prepare();
                mLocalPlayer->Start();
                ICloseable::Probe(afd)->Close();
                return TRUE;
            } else {
                Logger::E(TAG, "Could not load fallback ringtone");
            }
            // } catch (IOException ioe) {
                DestroyLocalPlayer();
                Logger::E(TAG, "Failed to open fallback ringtone");
            // } catch (NotFoundException nfe) {
                Logger::E(TAG, "Fallback ringtone does not exist");
            // }
        } else {
            Logger::W(TAG, "not playing fallback for %p", mUri.Get());
        }
    }
    return FALSE;
}

void CRingtone::SetTitle(
    /* [in] */ const String& title)
{
    mTitle = title;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
