#include "TransportControlView.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/text/style/CForegroundColorSpan.h"
#include "CTransportControlViewSavedState.h"
#include "elastos/droid/view/CKeyEvent.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::Math;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Media::IMediaMetadataRetriever;
using Elastos::Droid::Media::IRemoteControlClient;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Text::Style::IForegroundColorSpan;
using Elastos::Droid::Text::Style::CForegroundColorSpan;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

const Boolean TransportControlView::DEBUG;
const String TransportControlView::TAG("TransportControlView");
const Int32 TransportControlView::MSG_UPDATE_STATE;
const Int32 TransportControlView::MSG_SET_METADATA;
const Int32 TransportControlView::MSG_SET_TRANSPORT_CONTROLS;
const Int32 TransportControlView::MSG_SET_ARTWORK;
const Int32 TransportControlView::MSG_SET_GENERATION_ID;
const Int32 TransportControlView::MAXDIM;
const Int32 TransportControlView::DISPLAY_TIMEOUT_MS;

ECode TransportControlView::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    switch(what) {
        case MSG_UPDATE_STATE:
            if (mHost->mClientGeneration == arg1) mHost->UpdatePlayPauseState(arg2);
            break;

        case MSG_SET_METADATA:
            if (mHost->mClientGeneration == arg1) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                mHost->UpdateMetadata(IBundle::Probe(obj));
            }
            break;

        case MSG_SET_TRANSPORT_CONTROLS:
            if (mHost->mClientGeneration == arg1) mHost->UpdateTransportControls(arg2);
            break;

        case MSG_SET_ARTWORK:
            if (mHost->mClientGeneration == arg1) {
                if (mHost->mMetadata->mBitmap != NULL) {
                    mHost->mMetadata->mBitmap->Recycle();
                }
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                mHost->mMetadata->mBitmap = IBitmap::Probe(obj);
                mHost->mAlbumArt->SetImageBitmap(mHost->mMetadata->mBitmap);
            }
            break;

        case MSG_SET_GENERATION_ID: {
            if (arg2 != 0) {
                // This means nobody is currently registered. Hide the view.
                if (mHost->mWidgetCallbacks != NULL) {
                    mHost->mWidgetCallbacks->RequestHide(
                        (ITransportControlView*)mHost->Probe(EIID_ITransportControlView));
                }
            }
            // if (DEBUG) Log.v(TAG, "New genId = " + arg1 + ", clearing = " + arg2);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->mClientGeneration = arg1;
            mHost->mClientIntent = IPendingIntent::Probe(obj);
            break;
        }
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(TransportControlView::IRemoteControlDisplayWeak, IIRemoteControlDisplay)

TransportControlView::IRemoteControlDisplayWeak::IRemoteControlDisplayWeak(
    /* [in] */ IHandler* handler,
    /* [in] */ TransportControlView* host)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(handler);
    wrs->GetWeakReference((IWeakReference**)&mLocalHandler);
    mHost = host;
}

ECode TransportControlView::IRemoteControlDisplayWeak::SetPlaybackState(
    /* [in] */ Int32 generationId,
    /* [in] */ Int32 state,
    /* [in] */ Int64 stateChangeTimeMs)
{
    AutoPtr<IInterface> obj;
    mLocalHandler->Resolve(EIID_IHandler, (IInterface**)&obj);
    if (obj) {
        AutoPtr<IHandler> handler = IHandler::Probe(obj);
        AutoPtr<IMessage> msg;
        handler->ObtainMessage(
            TransportControlView::MSG_UPDATE_STATE,
            generationId, state, (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode TransportControlView::IRemoteControlDisplayWeak::SetMetadata(
    /* [in] */ Int32 generationId,
    /* [in] */ IBundle* metadata)
{
    AutoPtr<IInterface> obj;
    mLocalHandler->Resolve(EIID_IHandler, (IInterface**)&obj);
    if (obj) {
        AutoPtr<IHandler> handler = IHandler::Probe(obj);
        AutoPtr<IMessage> msg;
        handler->ObtainMessage(
            TransportControlView::MSG_SET_METADATA,
            generationId, 0, metadata, (IMessage**)&msg);
        msg->SendToTarget();
    }

    return NOERROR;
}

ECode TransportControlView::IRemoteControlDisplayWeak::SetTransportControlFlags(
    /* [in] */ Int32 generationId,
    /* [in] */ Int32 flags)
{
    AutoPtr<IInterface> obj;
    mLocalHandler->Resolve(EIID_IHandler, (IInterface**)&obj);
    if (obj) {
        AutoPtr<IHandler> handler = IHandler::Probe(obj);
        AutoPtr<IMessage> msg;
        handler->ObtainMessage(
            TransportControlView::MSG_SET_TRANSPORT_CONTROLS,
            generationId, flags, (IMessage**)&msg);
        msg->SendToTarget();
    }

    return NOERROR;
}

ECode TransportControlView::IRemoteControlDisplayWeak::SetArtwork(
    /* [in] */ Int32 generationId,
    /* [in] */ IBitmap* bitmap)
{
    AutoPtr<IInterface> obj;
    mLocalHandler->Resolve(EIID_IHandler, (IInterface**)&obj);
    if (obj) {
        AutoPtr<IHandler> handler = IHandler::Probe(obj);
        AutoPtr<IMessage> msg;
        handler->ObtainMessage(
            TransportControlView::MSG_SET_ARTWORK,
            generationId, 0, bitmap, (IMessage**)&msg);
        msg->SendToTarget();
    }

    return NOERROR;
}

ECode TransportControlView::IRemoteControlDisplayWeak::SetAllMetadata(
    /* [in] */ Int32 generationId,
    /* [in] */ IBundle* metadata,
    /* [in] */ IBitmap* bitmap)
{
    AutoPtr<IInterface> obj;
    mLocalHandler->Resolve(EIID_IHandler, (IInterface**)&obj);
    if (obj) {
        AutoPtr<IHandler> handler = IHandler::Probe(obj);
        AutoPtr<IMessage> msg;
        handler->ObtainMessage(
            TransportControlView::MSG_SET_METADATA,
            generationId, 0, metadata, (IMessage**)&msg);
        msg->SendToTarget();

        AutoPtr<IMessage> msg2;
        handler->ObtainMessage(
            TransportControlView::MSG_SET_ARTWORK,
            generationId, 0, bitmap, (IMessage**)&msg2);
        msg2->SendToTarget();
    }

    return NOERROR;
}

ECode TransportControlView::IRemoteControlDisplayWeak::SetCurrentClientId(
        /* [in] */ Int32 clientGeneration,
        /* [in] */ IPendingIntent* mediaIntent,
        /* [in] */ Boolean clearing)
{
    AutoPtr<IInterface> obj;
    mLocalHandler->Resolve(EIID_IHandler, (IInterface**)&obj);
    if (obj) {
        AutoPtr<IHandler> handler = IHandler::Probe(obj);
        AutoPtr<IMessage> msg;
        handler->ObtainMessage(
            TransportControlView::MSG_SET_GENERATION_ID,
            clientGeneration, (clearing ? 1 : 0), mediaIntent, (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode TransportControlView::InitFromResource(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
//    if (DEBUG)
//        Log.v(TAG, "Create TCV " + this);
    mHandler = new MyHandler(this);
    mMetadata = new Metadata();
    CAudioManager::New(mContext, (IAudioManager**)&mAudioManager);
    mCurrentPlayState = IRemoteControlClient::PLAYSTATE_NONE; // until we get a callback
    mIRCD = new IRemoteControlDisplayWeak(mHandler, this);
    return NOERROR;
}

TransportControlView::TransportControlView()
    : mClientGeneration(0)
    , mAttached(FALSE)
    , mTransportControlFlags(0)
    , mCurrentPlayState(0)
{}

TransportControlView::TransportControlView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
    , mClientGeneration(0)
    , mAttached(FALSE)
    , mTransportControlFlags(0)
    , mCurrentPlayState(0)
{
    InitFromResource(context, attrs);
}

ECode TransportControlView::Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::Init(context, attrs);
    return InitFromResource(context, attrs);
}

void TransportControlView::UpdateTransportControls(
        /* [in] */ Int32 transportControlFlags)
{
    mTransportControlFlags = transportControlFlags;
}

ECode TransportControlView::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    mTrackTitle = (ITextView*) FindViewById(R::id::title)->Probe(EIID_ITextView);
    mTrackTitle->SetSelected(TRUE); // enable marquee
    mAlbumArt = (IImageView*) FindViewById(R::id::albumart)->Probe(EIID_IImageView);
    mBtnPrev = (IImageView*) FindViewById(R::id::btn_prev)->Probe(EIID_IImageView);
    mBtnPlay = (IImageView*) FindViewById(R::id::btn_play)->Probe(EIID_IImageView);
    mBtnNext = (IImageView*) FindViewById(R::id::btn_next)->Probe(EIID_IImageView);
    mBtnPrev->SetOnClickListener((IViewOnClickListener*)this->Probe(EIID_IViewOnClickListener));
    mBtnPlay->SetOnClickListener((IViewOnClickListener*)this->Probe(EIID_IViewOnClickListener));
    mBtnNext->SetOnClickListener((IViewOnClickListener*)this->Probe(EIID_IViewOnClickListener));
    return NOERROR;
}

ECode TransportControlView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    if (mPopulateMetadataWhenAttached != NULL)
{
        UpdateMetadata(mPopulateMetadataWhenAttached);
        mPopulateMetadataWhenAttached = NULL;
    }
    if (!mAttached)
{
//        if (DEBUG)
//            Log.v(TAG, "Registering TCV " + this);
        mAudioManager->RegisterRemoteControlDisplay(mIRCD);
    }
    mAttached = TRUE;
    return NOERROR;
}

ECode TransportControlView::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();
    if (mAttached)
    {
//        if (DEBUG)
//            Log.v(TAG, "Unregistering TCV " + this);
        mAudioManager->UnregisterRemoteControlDisplay(mIRCD);
    }
    mAttached = FALSE;
    return NOERROR;
}

void TransportControlView::OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec)
{
    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
//    Int32 dim = Math::Min(MAXDIM, Math::Max(GetWidth(), GetHeight()));
//        Log.v(TAG, "setting max bitmap size: " + dim + "x" + dim);
//        mAudioManager.remoteControlDisplayUsesBitmapSize(mIRCD, dim, dim);
}

String TransportControlView::Metadata::ToString()
{
    return String("Metadata[artist=") + mArtist + " trackTitle=" + mTrackTitle
            + " albumTitle=" + mAlbumTitle + "]";
}

String TransportControlView::GetMdString(
        /* [in] */ IBundle* data,
        /* [in] */ Int32 id)
{
    String sId = StringUtils::ToString(id);
    String result;
    data->GetString(sId, &result);
    return result;
}

void TransportControlView::UpdateMetadata(
        /* [in] */ IBundle* data)
{
    if (mAttached)
    {
        mMetadata->mArtist = GetMdString(data,
                IMediaMetadataRetriever::METADATA_KEY_ALBUMARTIST);
        mMetadata->mTrackTitle = GetMdString(data,
                IMediaMetadataRetriever::METADATA_KEY_TITLE);
        mMetadata->mAlbumTitle = GetMdString(data,
                IMediaMetadataRetriever::METADATA_KEY_ALBUM);
        PopulateMetadata();
    } else {
        mPopulateMetadataWhenAttached = data;
    }
}

/**
 * Populates the given metadata into the view
 */
void TransportControlView::PopulateMetadata()
{
    StringBuilder sb;
    Int32 trackTitleLength = 0;
    if (!mMetadata->mTrackTitle.IsNullOrEmpty()) {
        sb += (mMetadata->mTrackTitle);
        trackTitleLength = mMetadata->mTrackTitle.GetLength();
    }

    if (!mMetadata->mArtist.IsNullOrEmpty()) {
        if (sb.GetLength() != 0) {
            sb += " - ";
        }
        sb += mMetadata->mArtist;
    }

    if (!mMetadata->mAlbumTitle.IsNullOrEmpty()) {
        if (sb.GetLength() != 0) {
            sb += " - ";
        }
        sb += mMetadata->mAlbumTitle;
    }
    String temp = sb.ToString();
    AutoPtr<ICharSequence> sequence;
    CStringWrapper::New(temp, (ICharSequence**)&sequence);
    mTrackTitle->SetText(sequence, BufferType_SPANNABLE);
    AutoPtr<ISpannable> str;
    mTrackTitle->GetText((ICharSequence**)&str);
    if (trackTitleLength != 0) {
        AutoPtr<IForegroundColorSpan> fcs;
        CForegroundColorSpan::New(0xffffffff, (IForegroundColorSpan**)&fcs);
        str->SetSpan(fcs, 0, trackTitleLength, ISpannable::SPAN_EXCLUSIVE_EXCLUSIVE);
        trackTitleLength++;
    }

    if (sb.GetLength() > trackTitleLength) {
        AutoPtr<IForegroundColorSpan> fcs;
        CForegroundColorSpan::New(0xffffffff, (IForegroundColorSpan**)&fcs);
        str->SetSpan(fcs, trackTitleLength, sb.GetLength(),
                ISpannable::SPAN_EXCLUSIVE_EXCLUSIVE);
    }

    mAlbumArt->SetImageBitmap(mMetadata->mBitmap);
    Int32 flags = mTransportControlFlags;
    SetVisibilityBasedOnFlag(mBtnPrev, flags,
            IRemoteControlClient::FLAG_KEY_MEDIA_PREVIOUS);
    SetVisibilityBasedOnFlag(mBtnNext, flags,
            IRemoteControlClient::FLAG_KEY_MEDIA_NEXT);
    SetVisibilityBasedOnFlag(mBtnPlay, flags,
            IRemoteControlClient::FLAG_KEY_MEDIA_PLAY
                    | IRemoteControlClient::FLAG_KEY_MEDIA_PAUSE
                    | IRemoteControlClient::FLAG_KEY_MEDIA_PLAY_PAUSE
                    | IRemoteControlClient::FLAG_KEY_MEDIA_STOP);

    UpdatePlayPauseState(mCurrentPlayState);
}

void TransportControlView::SetVisibilityBasedOnFlag(
        /* [in] */ IView* view,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 flag)
{
    if ((flags & flag) != 0)
{
        view->SetVisibility(IView::VISIBLE);
    } else {
        view->SetVisibility(IView::GONE);
    }
}

void TransportControlView::UpdatePlayPauseState(
        /* [in] */ Int32 state)
{
//    if (DEBUG)
//        Log.v(TAG,
//                "updatePlayPauseState(), old=" + mCurrentPlayState + ", state="
//                        + state);
    if (state == mCurrentPlayState)
    {
        return;
    }
    Int32 imageResId;
    Int32 imageDescId;
    Boolean showIfHidden = FALSE;
    switch (state)
    {
    case IRemoteControlClient::PLAYSTATE_ERROR:
        imageResId = R::drawable::stat_sys_warning;
        // TODO use more specific image description string for warning, but here the "play"
        //      message is still valid because this button triggers a play command.
        imageDescId = R::string::lockscreen_transport_play_description;
        break;

    case IRemoteControlClient::PLAYSTATE_PLAYING:
        imageResId = R::drawable::ic_media_pause;
        imageDescId = R::string::lockscreen_transport_pause_description;
        showIfHidden = TRUE;
        break;

    case IRemoteControlClient::PLAYSTATE_BUFFERING:
        imageResId = R::drawable::ic_media_stop;
        imageDescId = R::string::lockscreen_transport_stop_description;
        showIfHidden = TRUE;
        break;

    case IRemoteControlClient::PLAYSTATE_PAUSED:
    default:
        imageResId = R::drawable::ic_media_play;
        imageDescId = R::string::lockscreen_transport_play_description;
        showIfHidden = FALSE;
        break;
    }
    mBtnPlay->SetImageResource(imageResId);
    String str;
    GetResources()->GetString(imageDescId, &str);
    AutoPtr<ICharSequence> strTemp;
    CStringWrapper::New(str, (ICharSequence**)&strTemp);
    mBtnPlay->SetContentDescription(strTemp);

    Boolean res;
    if (showIfHidden && mWidgetCallbacks != NULL
            && !(mWidgetCallbacks->IsVisible((IView*)this->Probe(EIID_IView), &res), res))
    {
        mWidgetCallbacks->RequestShow((IView*)this->Probe(EIID_IView));
    }
    mCurrentPlayState = state;
}

AutoPtr<IParcelable> TransportControlView::OnSaveInstanceState()
{
//    if (DEBUG)
//        Log.v(TAG, "onSaveInstanceState()");
    AutoPtr<IParcelable> superState = FrameLayout::OnSaveInstanceState();

    AutoPtr<ITransportControlViewSavedState> ss;
    CTransportControlViewSavedState::New(superState, (ITransportControlViewSavedState**)&ss);

    Boolean visible;
    mWidgetCallbacks->IsVisible((IView*)this->Probe(EIID_IView), &visible);
    ((CTransportControlViewSavedState*)ss.Get())->wasShowing = mWidgetCallbacks != NULL && visible;

    return (IParcelable*)ss->Probe(EIID_IParcelable);
}

void TransportControlView::OnRestoreInstanceState(
        /* [in] */ IParcelable* state)
{
//    if (DEBUG)
//        Log.v(TAG, "onRestoreInstanceState()");
    if (!(state->Probe(EIID_ITransportControlViewSavedState)))
    {
        FrameLayout::OnRestoreInstanceState(state);
        return;
    }

    AutoPtr<ITransportControlViewSavedState> ss =
            (ITransportControlViewSavedState*) state->Probe(EIID_ITransportControlViewSavedState);
    AutoPtr<IParcelable> p;
    ss->GetSuperState((IParcelable**)&p);
    FrameLayout::OnRestoreInstanceState(p);
    if (((CTransportControlViewSavedState*)ss.Get())->wasShowing
            && mWidgetCallbacks != NULL)
    {
        mWidgetCallbacks->RequestShow(THIS_PROBE(IView));
    }
}

ECode TransportControlView::OnClick(
        /* [in] */ IView* v)
{
    Int32 keyCode = -1;
    if (v == (IView*)mBtnPrev->Probe(EIID_IView))
    {
        keyCode = IKeyEvent::KEYCODE_MEDIA_PREVIOUS;
    } else if (v == (IView*)mBtnNext->Probe(EIID_IView))
    {
        keyCode = IKeyEvent::KEYCODE_MEDIA_NEXT;
    } else if (v == (IView*)mBtnPlay->Probe(EIID_IView))
    {
        keyCode = IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE;

    }
    if (keyCode != -1)
    {
        SendMediaButtonClick(keyCode);
        if (mWidgetCallbacks != NULL)
        {
            mWidgetCallbacks->UserActivity(THIS_PROBE(IView));
        }
    }

    return NOERROR;
}

void TransportControlView::SendMediaButtonClick(
        /* [in] */ Int32 keyCode)
{
    if (mClientIntent == NULL)
    {
        // Shouldn't be possible because this view should be hidden in this case.
//        Log.e(TAG, "sendMediaButtonClick(): No client is currently registered");
        return;
    }
    // use the registered PendingIntent that will be processed by the registered
    //    media button event receiver, which is the component of mClientIntent
    AutoPtr<IKeyEvent> keyEvent;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, keyCode, (IKeyEvent**)&keyEvent);

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, (IIntent**)&intent);
    intent->PutExtra(IIntent::EXTRA_KEY_EVENT, (IParcelable*)keyEvent->Probe(EIID_IParcelable));

    //try {
    mClientIntent->Send(GetContext(), 0, intent);
    /*} catch (CanceledException e)
    {
     Log.e(TAG, "Error sending intent for media button down: "+e);
     e.printStackTrace();
     }*/

    keyEvent = NULL;
    CKeyEvent::New(IKeyEvent::ACTION_UP, keyCode, (IKeyEvent**)&keyEvent);
    intent = NULL;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, (IIntent**)&intent);
    intent->PutExtra(IIntent::EXTRA_KEY_EVENT, (IParcelable*)keyEvent->Probe(EIID_IParcelable));
    //try {
    mClientIntent->Send(GetContext(), 0, intent);
    /*} catch (CanceledException e)
{
     Log.e(TAG, "Error sending intent for media button up: "+e);
     e.printStackTrace();
     }*/
}

ECode TransportControlView::SetCallback(
        /* [in] */ ILockScreenWidgetCallback* callback)
{
    mWidgetCallbacks = callback;

    return NOERROR;
}

Boolean TransportControlView::ProvidesClock()
{
    return FALSE;
}

Boolean TransportControlView::WasPlayingRecently(
        /* [in] */ Int32 state,
        /* [in] */ Int64 stateChangeTimeMs)
{
    switch (state)
{
    case IRemoteControlClient::PLAYSTATE_PLAYING:
    case IRemoteControlClient::PLAYSTATE_FAST_FORWARDING:
    case IRemoteControlClient::PLAYSTATE_REWINDING:
    case IRemoteControlClient::PLAYSTATE_SKIPPING_FORWARDS:
    case IRemoteControlClient::PLAYSTATE_SKIPPING_BACKWARDS:
    case IRemoteControlClient::PLAYSTATE_BUFFERING:
        // actively playing or about to play
        return TRUE;
    case IRemoteControlClient::PLAYSTATE_NONE:
        return FALSE;
    case IRemoteControlClient::PLAYSTATE_STOPPED:
    case IRemoteControlClient::PLAYSTATE_PAUSED:
    case IRemoteControlClient::PLAYSTATE_ERROR:
        // we have stopped playing, check how Int64 ago
        if (DEBUG)
        {
            if ((SystemClock::GetElapsedRealtime() - stateChangeTimeMs)
                    < DISPLAY_TIMEOUT_MS)
            {
//                Log.v(TAG,
//                        "wasPlayingRecently: time < TIMEOUT was playing recently");
            } else {
//                Log.v(TAG, "wasPlayingRecently: time > TIMEOUT");
            }
        }
        return ((SystemClock::GetElapsedRealtime() - stateChangeTimeMs)
                < DISPLAY_TIMEOUT_MS);
    default:
//        Log.e(TAG,
//                "Unknown playback state " + state + " in wasPlayingRecently()");
        return FALSE;
    }
}

}    // namespace Internal
}    // namespace Widget
}    // namespace Droid
}    // namespace Elastos
