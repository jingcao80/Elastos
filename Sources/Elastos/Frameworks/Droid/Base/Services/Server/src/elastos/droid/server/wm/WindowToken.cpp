
#include "wm/WindowToken.h"
#include "wm/CWindowManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

CAR_INTERFACE_IMPL(WindowToken, IWeakReferenceSource)

ECode WindowToken::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference)
    *weakReference = new WeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

WindowToken::WindowToken(
    /* [in] */ CWindowManagerService* service,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 type,
    /* [in] */ Boolean _explicit)
    : mService(service)
    , mToken(token)
    , mWindowType(type)
    , mExplicit(_explicit)
    , mAppWindowToken(NULL)
    , mPaused(FALSE)
    , mHidden(FALSE)
    , mHasVisible(FALSE)
    , mWaitingToShow(FALSE)
    , mWaitingToHide(FALSE)
    , mSendingToBottom(FALSE)
    , mSendingToTop(FALSE)
{}


} // Wm
} // Server
} // Droid
} // Elastos
