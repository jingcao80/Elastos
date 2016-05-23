#include "elastos/droid/media/SubtitleController.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/CHandler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::CVector;
using Elastos::Utility::ILocaleHelper;
using Elastos::Droid::View::Accessibility::EIID_ICaptioningManagerCaptioningChangeListener;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;

namespace Elastos {
namespace Droid {
namespace Media {
//===============================================================
            // SubtitleController::MyHandlerCallback
//===============================================================
CAR_INTERFACE_IMPL(SubtitleController::MyHandlerCallback, Object, IHandlerCallback)

ECode SubtitleController::MyHandlerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 val;
    msg->GetWhat(&val);
    switch(val) {
    case 1:
        {
            mHost->DoShow();
            *result = TRUE;
            break;
        }

    case 2:
        {
            mHost->DoHide();
            *result = TRUE;
            break;
        }

    case 3:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ISubtitleTrack> st = ISubtitleTrack::Probe(obj);
            mHost->DoSelectTrack(st.Get());
            *result = TRUE;
            break;
        }

    case 4:
        {
            mHost->DoSelectDefaultTrack();
            *result = TRUE;
            break;
        }
    default:
        {
            *result = FALSE;
            break;
        }
    }

    return NOERROR;
}

//===============================================================
            // SubtitleController::MyCaptioningChangeListener
//===============================================================
CAR_INTERFACE_IMPL(SubtitleController::MyCaptioningChangeListener, Object, ICaptioningManagerCaptioningChangeListener);

ECode SubtitleController::MyCaptioningChangeListener::OnEnabledChanged(
    /* [in] */ Boolean enabled)
{
    return mHost->SelectDefaultTrack();
}

ECode SubtitleController::MyCaptioningChangeListener::OnLocaleChanged(
    /* [in] */ ILocale* locale)
{
    return mHost->SelectDefaultTrack();
}

ECode SubtitleController::MyCaptioningChangeListener::OnUserStyleChanged(
    /* [in] */ ICaptioningManagerCaptionStyle* userStyle)
{
    return NOERROR;
}

ECode SubtitleController::MyCaptioningChangeListener::OnFontScaleChanged(
    /* [in] */ Float fontScale)
{
    return NOERROR;
}

//===============================================================
            // SubtitleController
//===============================================================
const Int32 SubtitleController::WHAT_SHOW = 1;
const Int32 SubtitleController::WHAT_HIDE = 2;
const Int32 SubtitleController::WHAT_SELECT_TRACK = 3;
const Int32 SubtitleController::WHAT_SELECT_DEFAULT_TRACK = 4;

CAR_INTERFACE_IMPL(SubtitleController, Object, ISubtitleController);

SubtitleController::SubtitleController()
    : mTrackIsExplicit(FALSE)
    , mVisibilityIsExplicit(FALSE)
{
    mCallback = new MyHandlerCallback(this);
    mCaptioningChangeListener = new MyCaptioningChangeListener(this);
}

SubtitleController::~SubtitleController()
{}

ECode SubtitleController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IMediaTimeProvider* timeProvider,
    /* [in] */ ISubtitleControllerListener* listener)
{
    mTimeProvider = timeProvider;
    mListener = listener;

    CVector::New((IVector**)&mRenderers);
    mShowing = FALSE;
    CVector::New((IVector**)&mTracks);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CAPTIONING_SERVICE, (IInterface**)&obj);
    AutoPtr<ICaptioningManager> mCaptioningManager = ICaptioningManager::Probe(obj.Get());
    return NOERROR;
}

ECode SubtitleController::Finalize()
{
    return mCaptioningManager->RemoveCaptioningChangeListener(
            mCaptioningChangeListener.Get());
    // super->Finalize();
}

ECode SubtitleController::GetTracks(
    /* [out, callee] */ ArrayOf<ISubtitleTrack*>** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mTracks->GetSize(&size);
    {    AutoLock syncLock(mTracks);
        AutoPtr<ArrayOf<ISubtitleTrack*> > tracks = ArrayOf<ISubtitleTrack*>::Alloc(size);
        mTracks->ToArray((ArrayOf<IInterface *>**)&tracks);
        *result = tracks;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode SubtitleController::GetSelectedTrack(
    /* [out] */ ISubtitleTrack** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSelectedTrack.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SubtitleController::GetRenderingWidget(
    /* [out] */ ISubtitleTrackRenderingWidget** result)
{
    if (mSelectedTrack == NULL) {
        *result = NULL;
        return NOERROR;
    }
    return mSelectedTrack->GetRenderingWidget(result);
}

ECode SubtitleController::SelectTrack(
    /* [in] */ ISubtitleTrack* track,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean flag = FALSE;
    mTracks->Contains(track, &flag);
    if (track != NULL && !flag) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(SubtitleController::WHAT_SELECT_TRACK, track, (IMessage**)&msg);
    ProcessOnAnchor(msg.Get());
    *result = TRUE;
    return NOERROR;
}

void SubtitleController::DoSelectTrack(
    /* [in] */ ISubtitleTrack* track)
{
    mTrackIsExplicit = TRUE;
    if (mSelectedTrack.Get() == track) {
        return;
    }

    if (mSelectedTrack != NULL) {
        mSelectedTrack->Hide();
        mSelectedTrack->SetTimeProvider(NULL);
    }

    mSelectedTrack = track;
    if (mAnchor != NULL) {
        AutoPtr<ISubtitleTrackRenderingWidget> strw;
        GetRenderingWidget((ISubtitleTrackRenderingWidget**)&strw);
        mAnchor->SetSubtitleWidget(strw.Get());
    }

    if (mSelectedTrack != NULL) {
        mSelectedTrack->SetTimeProvider(mTimeProvider.Get());
        mSelectedTrack->Show();
    }

    if (mListener != NULL) {
        mListener->OnSubtitleTrackSelected(track);
    }
}

ECode SubtitleController::GetDefaultTrack(
    /* [out] */ ISubtitleTrack** result)
{
    AutoPtr<ISubtitleTrack> bestTrack;
    Int32 bestScore = -1;

    AutoPtr<ILocale> selectedLocale;
    mCaptioningManager->GetLocale((ILocale**)&selectedLocale);
    AutoPtr<ILocale> locale = selectedLocale;
    if (locale == NULL) {
        AutoPtr<ILocaleHelper> localeHelper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
        AutoPtr<ILocale> defaultLocale;
        localeHelper->GetDefault((ILocale**)&defaultLocale);
        locale = defaultLocale.Get();
    }
    Boolean flag = FALSE;
    mCaptioningManager->IsEnabled(&flag);
    Boolean selectForced = !flag;

    {    AutoLock syncLock(mTracks);
        Int32 size;
        mTracks->GetSize(&size);
        AutoPtr<IInterface> interf;
        for (Int32 i = 0; i < size; i++) {
            interf = NULL;
            mTracks->Get(i, (IInterface**)&interf);
            AutoPtr<ISubtitleTrack> track = ISubtitleTrack::Probe(interf);
            AutoPtr<IMediaFormat> format;
            track->GetFormat((IMediaFormat**)&format);
            String language;
            format->GetString(IMediaFormat::KEY_LANGUAGE, &language);

            Boolean forced = FALSE;
            Int32 iRet;
            format->GetInt32(IMediaFormat::KEY_IS_FORCED_SUBTITLE, 0, &iRet);
            forced = iRet != 0;
            Boolean autoselect = FALSE;
            format->GetInt32(IMediaFormat::KEY_IS_AUTOSELECT, 1, &iRet);
            autoselect = iRet != 0;
            Boolean is_default = FALSE;
            format->GetInt32(IMediaFormat::KEY_IS_DEFAULT, 0, &iRet);
            is_default = iRet != 0;

            String language_;
            locale->GetLanguage(&language_);
            String isoLanguage;
            locale->GetISO3Language(&isoLanguage);
            Boolean languageMatches =
                (locale == NULL ||
                language_.Equals(String("")) ||
                isoLanguage.Equals(language) ||
                language_.Equals(language));
            // is_default is meaningless unless caption language is 'default'
            Int32 score = (forced ? 0 : 8) +
                (((selectedLocale == NULL) && is_default) ? 4 : 0) +
                (autoselect ? 0 : 2) + (languageMatches ? 1 : 0);

            if (selectForced && !forced) {
                continue;
            }

            // we treat null locale/language as matching any language
            if ((selectedLocale == NULL && is_default) ||
                (languageMatches &&
                 (autoselect || forced || selectedLocale != NULL))) {
                if (score > bestScore) {
                    bestScore = score;
                    bestTrack = track;
                }
            }
        }
    }

    *result = bestTrack;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SubtitleController::SelectDefaultTrack()
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(SubtitleController::WHAT_SELECT_DEFAULT_TRACK, (IMessage**)&msg);
    ProcessOnAnchor(msg.Get());
    return NOERROR;
}

void SubtitleController::DoSelectDefaultTrack()
{
    if (mTrackIsExplicit) {
    // If track selection is explicit, but visibility
    // is not, it falls back to the captioning setting
    if (!mVisibilityIsExplicit) {
        Boolean flag = FALSE;
        mCaptioningManager->IsEnabled(&flag);
        AutoPtr<IMediaFormat> format;
        mSelectedTrack->GetFormat((IMediaFormat**)&format);
        Int32 result = 0;
        format->GetInt32(IMediaFormat::KEY_IS_FORCED_SUBTITLE, 0, &result);

        if (flag || (mSelectedTrack != NULL && result != 0)) {
            Show();
        } else if (mSelectedTrack != NULL && !(mSelectedTrack->IsTimedText(&flag), flag)) {
            Hide();
        }
        mVisibilityIsExplicit = FALSE;
    }
    return;
    }

    // We can have a default (forced) track even if captioning
    // is not enabled.  This is handled by getDefaultTrack().
    // Show this track unless subtitles were explicitly hidden.
    AutoPtr<ISubtitleTrack> track;
    GetDefaultTrack((ISubtitleTrack**)&track);
    if (track != NULL) {
        Boolean result = FALSE;
        SelectTrack(track, &result);
        mTrackIsExplicit = FALSE;
        if (!mVisibilityIsExplicit) {
            Show();
            mVisibilityIsExplicit = FALSE;
        }
    }
}

ECode SubtitleController::Reset()
{
    CheckAnchorLooper();
    Hide();
    Boolean flag = FALSE;
    SelectTrack(NULL, &flag);
    mTracks->Clear();
    mTrackIsExplicit = FALSE;
    mVisibilityIsExplicit = FALSE;
    mCaptioningManager->RemoveCaptioningChangeListener(
            mCaptioningChangeListener);
    return NOERROR;
}

ECode SubtitleController::AddTrack(
    /* [in] */ IMediaFormat* format,
    /* [out] */ ISubtitleTrack** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mRenderers->GetSize(&size);
    {    AutoLock syncLock(mRenderers);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> ife;
            mRenderers->Get(i, (IInterface**)&ife);
            ISubtitleControllerRenderer* renderer = ISubtitleControllerRenderer::Probe(ife);
            Boolean flag = FALSE;
            if ((renderer->Supports(format, &flag), flag)) {
                AutoPtr<ISubtitleTrack> track;
                renderer->CreateTrack(format, (ISubtitleTrack**)&track);
                if (track != NULL) {
                    {
                        AutoLock lock(mTracks);
                        Int32 length;
                        mTracks->GetSize(&length);

                        if (length == 0) {
                            mCaptioningManager->AddCaptioningChangeListener(
                                mCaptioningChangeListener);
                        }
                        mTracks->Add(track);
                    }

                    *result = track;
                    REFCOUNT_ADD(*result);
                    return NOERROR;
                }
            }
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode SubtitleController::Show()
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(SubtitleController::WHAT_SHOW, (IMessage**)&msg);
    ProcessOnAnchor(msg.Get());
    return NOERROR;
}

void SubtitleController::DoShow()
{
    mShowing = TRUE;
    mVisibilityIsExplicit = TRUE;
    if (mSelectedTrack != NULL) {
        mSelectedTrack->Show();
    }
}

ECode SubtitleController::Hide()
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(SubtitleController::WHAT_HIDE, (IMessage**)&msg);
    ProcessOnAnchor(msg.Get());
    return NOERROR;
}

void SubtitleController::DoHide()
{
    mVisibilityIsExplicit = TRUE;
    if (mSelectedTrack != NULL) {
        mSelectedTrack->Hide();
    }
    mShowing = FALSE;
}

ECode SubtitleController::RegisterRenderer(
    /* [in] */ ISubtitleControllerRenderer* renderer)
{
    {    AutoLock syncLock(mRenderers);
    // TODO how to get available renderers in the system
    Boolean flag = FALSE;
    mRenderers->Contains(renderer, &flag);
    if (!flag) {
        // TODO should added renderers override existing ones (to allow replacing?)
        mRenderers->Add(renderer);
    }
    }
    return NOERROR;
}

ECode SubtitleController::HasRendererFor(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mRenderers->GetSize(&size);
    {    AutoLock syncLock(mRenderers);
        // TODO how to get available renderers in the system
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> interf;
            mRenderers->Get(i,  (IInterface**)&interf);
            ISubtitleControllerRenderer* renderer = ISubtitleControllerRenderer::Probe(interf);
            Boolean flag = FALSE;
            renderer->Supports(format, &flag);
            if(flag) {
                *result = TRUE;
                return NOERROR;
            }
        }

        *result = FALSE;
    }
    return NOERROR;
}

ECode SubtitleController::SetAnchor(
    /* [in] */ ISubtitleControllerAnchor* anchor)
{
    if (mAnchor.Get() == anchor) {
        return NOERROR;
    }

    if (mAnchor != NULL) {
        CheckAnchorLooper();
        mAnchor->SetSubtitleWidget(NULL);
    }

    mAnchor = anchor;
    mHandler = NULL;
    if (mAnchor != NULL) {
        AutoPtr<ILooper> looper;
        mAnchor->GetSubtitleLooper((ILooper**)&looper);
        AutoPtr<IHandlerCallback> mCallback;
        // CHandler::New(looper, mCallback.Get(), (IHandler**)&mHandler);
        CheckAnchorLooper();
        AutoPtr<ISubtitleTrackRenderingWidget> strw;
        GetRenderingWidget((ISubtitleTrackRenderingWidget**)&strw);
        mAnchor->SetSubtitleWidget(strw.Get());
    }
    return NOERROR;
}

void SubtitleController::CheckAnchorLooper()
{
    // assert mHandler != NULL : "Should have a looper already";
    AutoPtr<ILooper> looper;
    mHandler->GetLooper((ILooper**)&looper);
    // assert Looper::GetMyLooper().Get() == looper : "Must be called from the anchor's looper";
}

void SubtitleController::ProcessOnAnchor(
    /* [in] */ IMessage* m)
{
    // assert mHandler != NULL : "Should have a looper already";
    AutoPtr<ILooper> looper;
    mHandler->GetLooper((ILooper**)&looper);
    if (Looper::GetMyLooper().Get() == looper) {
        mHandler->DispatchMessage(m);
    } else {
        Boolean flag = FALSE;
        mHandler->SendMessage(m, &flag);
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos