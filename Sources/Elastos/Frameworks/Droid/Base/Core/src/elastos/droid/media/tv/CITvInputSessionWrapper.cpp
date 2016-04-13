#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/tv/CITvInputSessionWrapper.h"
#include "elastos/droid/media/tv/TvInputServiceSession.h"
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Os::CHandlerCaller;
using Elastos::Droid::Internal::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::View::EIID_IInputEventReceiver;
using Elastos::Droid::View::IInputEventReceiver;
using Elastos::Droid::View::ISurface;
using Elastos::Core::CBoolean;
using Elastos::Core::CFloat;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;
using Elastos::Core::ISystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

String CITvInputSessionWrapper::TAG("TvInputSessionWrapper");

Int32 CITvInputSessionWrapper::MESSAGE_HANDLING_DURATION_THRESHOLD_MILLIS = 50;

const Int32 CITvInputSessionWrapper::DO_RELEASE = 1;
const Int32 CITvInputSessionWrapper::DO_SET_MAIN = 2;
const Int32 CITvInputSessionWrapper::DO_SET_SURFACE = 3;
const Int32 CITvInputSessionWrapper::DO_DISPATCH_SURFACE_CHANGED = 4;
const Int32 CITvInputSessionWrapper::DO_SET_STREAM_VOLUME = 5;
const Int32 CITvInputSessionWrapper::DO_TUNE = 6;
const Int32 CITvInputSessionWrapper::DO_SET_CAPTION_ENABLED = 7;
const Int32 CITvInputSessionWrapper::DO_SELECT_TRACK = 8;
const Int32 CITvInputSessionWrapper::DO_APP_PRIVATE_COMMAND = 9;
const Int32 CITvInputSessionWrapper::DO_CREATE_OVERLAY_VIEW = 10;
const Int32 CITvInputSessionWrapper::DO_RELAYOUT_OVERLAY_VIEW = 11;
const Int32 CITvInputSessionWrapper::DO_REMOVE_OVERLAY_VIEW = 12;
const Int32 CITvInputSessionWrapper::DO_REQUEST_UNBLOCK_CONTENT = 13;

CAR_INTERFACE_IMPL_2(CITvInputSessionWrapper, Object, IITvInputSession, IHandlerCallerCallback)

CAR_OBJECT_IMPL(CITvInputSessionWrapper)

ECode CITvInputSessionWrapper::TvInputEventReceiver::constructor(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper,
    /* [in] */ ITvInputServiceSession* impl)
{
    mImpl = impl;
    return InputEventReceiver::constructor(inputChannel, looper);
}

ECode CITvInputSessionWrapper::TvInputEventReceiver::OnInputEvent(
    /* [in] */ IInputEvent* event)
{
    if (mImpl == NULL) {
        // The session has been finished.
        return FinishInputEvent(event, FALSE);
    }

    Int32 handled;
    ((TvInputServiceSession*)mImpl.Get())->DispatchInputEvent(event, this, &handled);
    if (handled != ITvInputManagerSession::DISPATCH_IN_PROGRESS) {
        FinishInputEvent(event, handled == ITvInputManagerSession::DISPATCH_HANDLED);
    }
    return NOERROR;
}

CITvInputSessionWrapper::CITvInputSessionWrapper()
{
}

CITvInputSessionWrapper::~CITvInputSessionWrapper()
{
}

ECode CITvInputSessionWrapper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ITvInputServiceSession* sessionImpl,
    /* [in] */ IInputChannel* channel)
{
    CHandlerCaller::New(context, NULL, this, TRUE /* asyncHandler */, TRUE, (IHandlerCaller**)&mCaller);
    mTvInputSessionImpl = sessionImpl;
    mChannel = channel;
    if (channel != NULL) {
        AutoPtr<ILooper> mainLooper;
        context->GetMainLooper((ILooper**)&mainLooper);
        AutoPtr<TvInputEventReceiver> ti = new TvInputEventReceiver();
        ti->constructor(channel, mainLooper, mTvInputSessionImpl);
        mReceiver = (TvInputEventReceiver*)ti.Get();
    }
    return NOERROR;
}

ECode CITvInputSessionWrapper::ExecuteMessage(
    /* [in] */ IMessage* msg)
{
    if (mTvInputSessionImpl == NULL) {
        return NOERROR;
    }

    Int64 startTime;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&startTime);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 what;
    msg->GetWhat(&what);

    AutoPtr<TvInputServiceSession> impl = (TvInputServiceSession*)mTvInputSessionImpl.Get();
    switch (what) {
        case DO_RELEASE: {
            impl->ReleaseResources();
            mTvInputSessionImpl = NULL;
            if (mReceiver != NULL) {
                mReceiver->Dispose();
                mReceiver = NULL;
            }
            if (mChannel != NULL) {
                mChannel->Dispose();
                mChannel = NULL;
            }
            break;
        }
        case DO_SET_MAIN: {
            Boolean b;
            IBoolean::Probe(obj)->GetValue(&b);
            impl->SetMain(b);
            break;
        }
        case DO_SET_SURFACE: {
            impl->SetSurface(ISurface::Probe(obj));
            break;
        }
        case DO_DISPATCH_SURFACE_CHANGED: {
            AutoPtr<SomeArgs> args = (SomeArgs*)ISomeArgs::Probe(obj);
            impl->DispatchSurfaceChanged(args->mArgi1, args->mArgi2, args->mArgi3);
            args->Recycle();
            break;
        }
        case DO_SET_STREAM_VOLUME: {
            Float f;
            IFloat::Probe(obj)->GetValue(&f);
            impl->SetStreamVolume(f);
            break;
        }
        case DO_TUNE: {
            AutoPtr<SomeArgs> args = (SomeArgs*)ISomeArgs::Probe(obj);
            impl->Tune(IUri::Probe(args->mArg1), IBundle::Probe(args->mArg2));
            args->Recycle();
            break;
        }
        case DO_SET_CAPTION_ENABLED: {
            Boolean b;
            IBoolean::Probe(obj)->GetValue(&b);
            impl->SetCaptionEnabled(b);
            break;
        }
        case DO_SELECT_TRACK: {
            AutoPtr<SomeArgs> args = (SomeArgs*)ISomeArgs::Probe(obj);
            Int32 val;
            IInteger32::Probe(args->mArg1)->GetValue(&val);
            String str;
            ICharSequence::Probe(args->mArg2)->ToString(&str);
            impl->SelectTrack(val, str);
            args->Recycle();
            break;
        }
        case DO_APP_PRIVATE_COMMAND: {
            AutoPtr<SomeArgs> args = (SomeArgs*)ISomeArgs::Probe(obj);
            String str;
            ICharSequence::Probe(args->mArg1)->ToString(&str);
            impl->AppPrivateCommand(str, IBundle::Probe(args->mArg2));
            args->Recycle();
            break;
        }
        case DO_CREATE_OVERLAY_VIEW: {
            AutoPtr<SomeArgs> args = (SomeArgs*)ISomeArgs::Probe(obj);
            impl->CreateOverlayView(IBinder::Probe(args->mArg1), IRect::Probe(args->mArg2));
            args->Recycle();
            break;
        }
        case DO_RELAYOUT_OVERLAY_VIEW: {
            impl->RelayoutOverlayView(IRect::Probe(obj));
            break;
        }
        case DO_REMOVE_OVERLAY_VIEW: {
            impl->RemoveOverlayView(TRUE);
            break;
        }
        case DO_REQUEST_UNBLOCK_CONTENT: {
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            impl->UnblockContent(str);
            break;
        }
        default: {
            Logger::W(TAG, "Unhandled message code: " + what);
            break;
        }
    }
    Int64 endTime;
    system->GetCurrentTimeMillis(&endTime);
    Int64 duration = endTime - startTime;
    if (duration > MESSAGE_HANDLING_DURATION_THRESHOLD_MILLIS) {
        Logger::W(TAG, "Handling message (%d) took too long time (duration=%ld ms)", what, duration);
    }
    return NOERROR;
}

ECode CITvInputSessionWrapper::ReleaseResources()
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessage(DO_RELEASE, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::SetMain(
    /* [in] */ Boolean isMain)
{
    AutoPtr<IBoolean> b;
    CBoolean::New(isMain, (IBoolean**)&b);
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_SET_MAIN, b, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::SetSurface(
    /* [in] */ ISurface* surface)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_SET_SURFACE, surface, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::DispatchSurfaceChanged(
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageIIII(DO_DISPATCH_SURFACE_CHANGED,
                format, width, height, 0, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::SetVolume(
    /* [in] */ Float volume)
{
    AutoPtr<IFloat> f;
    CFloat::New(volume, (IFloat**)&f);
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_SET_STREAM_VOLUME, f, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::Tune(
    /* [in] */ IUri* channelUri,
    /* [in] */ IBundle* params)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageOO(DO_TUNE, channelUri, params, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::SetCaptionEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IBoolean> b;
    CBoolean::New(enabled, (IBoolean**)&b);
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_SET_CAPTION_ENABLED, b, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::SelectTrack(
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    AutoPtr<ICharSequence> csq;
    CString::New(trackId, (ICharSequence**)&csq);
    AutoPtr<IInteger32> i32;
    CInteger32::New(type, (IInteger32**)&i32);
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageOO(DO_SELECT_TRACK, i32, csq, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::AppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    AutoPtr<ICharSequence> csq;
    CString::New(action, (ICharSequence**)&csq);
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageOO(DO_APP_PRIVATE_COMMAND, csq,
                data, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::CreateOverlayView(
    /* [in] */ IBinder* windowToken,
    /* [in] */ IRect* frame)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageOO(DO_CREATE_OVERLAY_VIEW, windowToken,
                frame, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::RelayoutOverlayView(
    /* [in] */ IRect* frame)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_RELAYOUT_OVERLAY_VIEW, frame, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::RemoveOverlayView()
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessage(DO_REMOVE_OVERLAY_VIEW, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CITvInputSessionWrapper::RequestUnblockContent(
    /* [in] */ const String& unblockedRating)
{
    AutoPtr<ICharSequence> csq;
    CString::New(unblockedRating, (ICharSequence**)&csq);
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_REQUEST_UNBLOCK_CONTENT, csq, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
