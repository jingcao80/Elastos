
#include "elastos/droid/media/CRingtone.h"
#include "elastos/droid/media/CMediaPlayer.h"
#include "elastos/droid/media/CRingtoneManagerHelper.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using namespace Elastos::Core;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
// TODO using Elastos::Droid::Provider::IDrmStoreAudio;
// using Elastos::Droid::Provider::IDrmStore;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Media {

const String CRingtone::TAG("CRingtone");
const Boolean CRingtone::LOGD = TRUE;

static AutoPtr<ArrayOf<String> > InitMEDIA_COLUMNS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    array->Set(0, IMediaStoreAudioMedia::ID);
    array->Set(1, IMediaStoreAudioMedia::DATA);
    array->Set(2, IMediaStoreAudioMedia::TITLE);
    return array;
}

static AutoPtr<ArrayOf<String> > InitDRM_COLUMNS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    // TODO
    // array->Set(0, IDrmStoreAudio::ID);
    // array->Set(1, IDrmStoreAudio::DATA);
    // array->Set(2, IDrmStoreAudio::TITLE);
    return array;
}

AutoPtr<ArrayOf<String> > CRingtone::MEDIA_COLUMNS = InitMEDIA_COLUMNS();
AutoPtr<ArrayOf<String> > CRingtone::DRM_COLUMNS = InitDRM_COLUMNS();


CRingtone::CRingtone()
    : mAllowRemote(FALSE)
    , mStreamType(IAudioManager::STREAM_RING)
{}

CRingtone::~CRingtone()
{}

ECode CRingtone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean allowRemote)
{
    mContext = context;

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&mAudioManager);
    mAudioManager = IAudioManager::Probe(obj);

    mAllowRemote = allowRemote;
    if (allowRemote) {
        mAudioManager->GetRingtonePlayer((IIRingtonePlayer**)&mRemotePlayer);
        CBinder::New((IBinder**)&mRemoteToken);
    }

    return NOERROR;
}

ECode CRingtone::SetStreamType(
    /* [in] */ Int32 streamType)
{
    mStreamType = streamType;

    // The stream type has to be set before the media player is prepared.
    // Re-initialize it.
    SetUri(mUri);

    return NOERROR;
}

ECode CRingtone::GetStreamType(
    /* [out] */ Int32* type)
{
    *type = mStreamType;
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

                AutoPtr<ICharSequence> seq;
                CString::New(actualTitle, (ICharSequence**)&seq);
                AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
                array->Set(0, (IInterface*)seq.Get());
                context->GetString(R::string::ringtone_default_with_actual, array, &title);
            }
        }
        else {
            assert(0 && "TODO");
            /*
            if (IDrmStore::AUTHORITY.Equals(authority)) {
                res->Query(uri, DRM_COLUMNS, nullStr, NULL, nullStr, (ICursor**)&cursor);
            }
            else
            */
            if (IMediaStore::AUTHORITY.Equals(authority)) {
                res->Query(uri, MEDIA_COLUMNS, nullStr, NULL, nullStr, (ICursor**)&cursor);
            }

            // try {
            Boolean result;
            ECode ec = NOERROR;

            Int32 count = 0;
            if (cursor) {
                cursor->GetCount(&count);
            }

            if (count == 1) {
                ec = cursor->MoveToFirst(&result);
                FAIL_GOTO(ec, _EXIT_)

                ec = cursor->GetString(2, &title);
                FAIL_GOTO(ec, _EXIT_)

_EXIT_:
                if (cursor != NULL) {
                    cursor->Close();
                    cursor = NULL;
                }

                if (SUCCEEDED(ec)) {
                    return title;
                }
            }
            else {
                uri->GetLastPathSegment(&title);

                if (cursor != NULL) {
                    cursor->Close();
                    cursor = NULL;
                }
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

    ec = mLocalPlayer->SetAudioStreamType(mStreamType);
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
        if (mAudioManager->GetStreamVolume(mStreamType, &vol) != 0) {
            mLocalPlayer->Start();
        }
    }
    else if (mAllowRemote) {
        AutoPtr<IUri> canonicalUri;
        mUri->GetCanonicalUri((IUri**)&canonicalUri);
        //try {
        ECode ec = mRemotePlayer->Play(mRemoteToken, canonicalUri, mStreamType);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "Problem playing ringtone: E_REMOTE_EXCEPTION");
        }
    }
    else {
        Logger::W(TAG, "Neither local nor remote playback available");
    }

    return NOERROR;
}

ECode CRingtone::Stop()
{
    if (mLocalPlayer != NULL) {
        DestroyLocalPlayer();
    }
    else if (mAllowRemote) {
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
    else if (mAllowRemote) {
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

void CRingtone::SetTitle(
    /* [in] */ const String& title)
{
    mTitle = title;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
