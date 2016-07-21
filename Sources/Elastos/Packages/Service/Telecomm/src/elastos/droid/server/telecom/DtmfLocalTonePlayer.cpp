
#include "elastos/droid/server/telecom/DtmfLocalTonePlayer.h"
#include "elastos/droid/server/telecom/Call.h"
#include "R.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Provider.h>
#include <elastos/droid/provider/Settings.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Media::CToneGenerator;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Core::CInteger32;
using Elastos::Core::CChar32;
using Elastos::Core::IChar32;
using Elastos::Core::IInteger32;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

AutoPtr<IMap> DtmfLocalTonePlayer::TONE_MAP = InitTONE_MAP();

ECode DtmfLocalTonePlayer::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode DtmfLocalTonePlayer::OnForegroundCallChanged(
    /* [in] */ ICall* oldForegroundCall,
    /* [in] */ ICall* newForegroundCall)
{
    EndDtmfSession(oldForegroundCall);
    StartDtmfSession(newForegroundCall);
    return NOERROR;
}

ECode DtmfLocalTonePlayer::PlayTone(
    /* [in] */ ICall* call,
    /* [in] */ Char32 c)
{
    // Do nothing if it is not the right call.
    if (mCall.Get() != call) {
        return NOERROR;
    }
    if (mToneGenerator == NULL) {
        Log::D("HeadsetMediaButton", "playTone: mToneGenerator == NULL, %c.", c);
    } else {
        Log::D("HeadsetMediaButton", "starting local tone: %c.", c);
        AutoPtr<IChar32> char32;
        CChar32::New(c, (IChar32**)&char32);
        Boolean isContainsKey;
        TONE_MAP->ContainsKey(char32, &isContainsKey);
        AutoPtr<IInterface> obj;
        if (isContainsKey) {
            TONE_MAP->Get(char32, (IInterface**)&obj);
            Int32 i32;
            IInteger32::Probe(obj)->GetValue(&i32);
            Boolean bNotUsed;
            mToneGenerator->StartTone(i32, -1 /* toneDuration */, &bNotUsed);
        }
    }
    return NOERROR;
}

ECode DtmfLocalTonePlayer::StopTone(
    /* [in] */ ICall* call)
{
    // Do nothing if it's not the right call.
    if (mCall.Get() != call) {
        return NOERROR;
    }
    if (mToneGenerator == NULL) {
        Log::D("HeadsetMediaButton", "stopTone: mToneGenerator == NULL.");
    } else {
        Log::D("HeadsetMediaButton", "stopping local tone.");
        mToneGenerator->StopTone();
    }
    return NOERROR;
}

ECode DtmfLocalTonePlayer::StartDtmfSession(
    /* [in] */ ICall* call)
{
    if (call == NULL) {
        return NOERROR;
    }
    AutoPtr<IContext> context;
    ((Call*) call)->GetContext((IContext**)&context);
    Boolean areLocalTonesEnabled;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean isallow_local_dtmf_tones;
    res->GetBoolean(R::bool_::allow_local_dtmf_tones, &isallow_local_dtmf_tones);
    if (isallow_local_dtmf_tones) {
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);
        Int32 dtmfToneWhelDialing;
        Settings::System::GetInt32(
                contentResolver, ISettingsSystem::DTMF_TONE_WHEN_DIALING, 1, &dtmfToneWhelDialing);
        areLocalTonesEnabled = dtmfToneWhelDialing == 1;
    } else {
        areLocalTonesEnabled = FALSE;
    }
    mCall = call;
    if (areLocalTonesEnabled) {
        if (mToneGenerator == NULL) {
            // try {
            ECode ec = CToneGenerator::New(IAudioManager::STREAM_DTMF, 80, (IToneGenerator**)&mToneGenerator);
            // } catch (RuntimeException e) {
            if (FAILED(ec)) {
                if ((ECode) E_RUNTIME_EXCEPTION == ec) {
                    Log::E("HeadsetMediaButton", "Error creating local tone generator.");
                    mToneGenerator = NULL;
                }
                else {
                    return ec;
                }

            }
            // }
        }
    }
    return NOERROR;
}

ECode DtmfLocalTonePlayer::EndDtmfSession(
    /* [in] */ ICall* call)
{
    if (call != NULL && mCall.Get() == call) {
        // Do a stopTone() in case the sessions ends before we are told to stop the tone.
        StopTone(call);
        mCall = NULL;
        if (mToneGenerator != NULL) {
            mToneGenerator->ReleaseResources();
            mToneGenerator = NULL;
        }
    }
    return NOERROR;
}

AutoPtr<IMap> DtmfLocalTonePlayer::InitTONE_MAP()
{
    AutoPtr<IMap> rev;
    CHashMap::New((IMap**)&rev);
    AutoPtr<IChar32> c32;
    CChar32::New('1', (IChar32**)&c32);
    AutoPtr<IInteger32> i32;
    CInteger32::New(IToneGenerator::TONE_DTMF_1, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('2', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_2, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('3', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_3, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('4', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_4, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('5', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_5, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('6', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_6, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('7', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_7, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('8', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_8, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('9', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_9, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('0', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_0, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('#', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_P, (IInteger32**)&i32);
    rev->Put(c32, i32);
    c32 = NULL;
    i32 = NULL;
    CChar32::New('*', (IChar32**)&c32);
    CInteger32::New(IToneGenerator::TONE_DTMF_S, (IInteger32**)&i32);
    rev->Put(c32, i32);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
