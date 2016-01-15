
#ifdef DROID_CORE
#include "elastos/droid/inputmethodservice/AbstractInputMethodSessionImpl.h"
#else
#include "elastos/droid/inputmethodservice/AbstractInputMethodSessionImpl.h"
#endif

using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::InputMethod::EIID_IInputMethodSession;

namespace Elastos {
namespace Droid {
namespace InputMethodService {
CAR_INTERFACE_IMPL(AbstractInputMethodSessionImpl, Object, IAbstractInputMethodSessionImpl);

AbstractInputMethodSessionImpl::AbstractInputMethodSessionImpl(
    /* [in] */ AbstractInputMethodService* host)
    : mEnabled(TRUE)
    , mRevoked(FALSE)
    , mAbstractHost(host)
{}

ECode AbstractInputMethodSessionImpl::IsEnabled(
    /* [out] */ Boolean* enabled)
{
    assert(enabled != NULL);
    *enabled = mEnabled;

    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::IsRevoked(
    /* [out] */ Boolean* revoked)
{
    assert(revoked != NULL);
    *revoked = mRevoked;

    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::SetEnabled(
    /* [in] */ Boolean enabled)
{
    if (!mRevoked) {
        mEnabled = enabled;
    }

    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::RevokeSelf()
{
    mRevoked = TRUE;
    mEnabled = FALSE;

    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::DispatchKeyEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IKeyEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* callback)
{
    assert(event != NULL && mAbstractHost != NULL);

    Boolean handled = FALSE;
    event->Dispatch((IKeyEventCallback*)mAbstractHost->Probe(EIID_IKeyEventCallback),
            mAbstractHost->mDispatcherState, this->Probe(EIID_IInputMethodSession), &handled);
    if (callback != NULL) {
        return callback->FinishedEvent(seq, handled);
    }

    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::DispatchTrackballEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* callback)
{
    assert(mAbstractHost != NULL);
    Boolean handled = FALSE;
    mAbstractHost->OnTrackballEvent(event, &handled);
    if (callback != NULL) {
        return callback->FinishedEvent(seq, handled);
    }

    return NOERROR;
}

ECode AbstractInputMethodSessionImpl::DispatchGenericMotionEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* callback)
{
    Boolean handled = FALSE;
    mAbstractHost->OnGenericMotionEvent(event, &handled);
    if (callback != NULL) {
        return callback->FinishedEvent(seq, handled);
    }

    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
