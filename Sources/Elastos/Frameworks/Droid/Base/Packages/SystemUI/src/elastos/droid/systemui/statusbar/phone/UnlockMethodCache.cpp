
#include "elastos/droid/systemui/statusbar/phone/UnlockMethodCache.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

AutoPtr<UnlockMethodCache> UnlockMethodCache::sInstance;
UnlockMethodCache::UnlockMethodCache(
    /* [in] */ IContext* ctx)
{
    CArrayList::New((IArrayList**)&mListeners);
    CLockPatternUtils::New(ctx, (ILockPatternUtils**)&mLockPatternUtils);
    Logger::D("UnlockMethodCache", "TODO: Not implement the app Keyguard.");
    // mKeyguardUpdateMonitor = KeyguardUpdateMonitor.getInstance(ctx);
    // KeyguardUpdateMonitor.getInstance(ctx).registerCallback(mCallback);
    UpdateMethodSecure(TRUE /* updateAlways */);
}

AutoPtr<UnlockMethodCache> UnlockMethodCache::GetInstance(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        sInstance = new UnlockMethodCache(context);
    }
    return sInstance;
}

Boolean UnlockMethodCache::IsMethodInsecure()
{
    return mMethodInsecure;
}

void UnlockMethodCache::AddListener(
    /* [in] */ IOnUnlockMethodChangedListener* listener)
{
    mListeners->Add(listener);
}

void UnlockMethodCache::RemoveListener(
    /* [in] */ IOnUnlockMethodChangedListener* listener)
{
    mListeners->Remove(listener);
}

void UnlockMethodCache::UpdateMethodSecure(
    /* [in] */ Boolean updateAlways)
{
    Int32 user = 0;
    mLockPatternUtils->GetCurrentUser(&user);
    Boolean secure = FALSE;
    mLockPatternUtils->IsSecure(&secure);
    Boolean hasTrust = TRUE;
    Logger::D("UnlockMethodCache", "TODO [UpdateMethodSecure 1] : Not implement the app Keyguard.");
    // mKeyguardUpdateMonitor->GetUserHasTrust(user, &hasTrust);
    Boolean methodInsecure = !secure || hasTrust;
    Boolean trustManaged = TRUE;
    Logger::D("UnlockMethodCache", "TODO [UpdateMethodSecure 2] : Not implement the app Keyguard.");
    // mKeyguardUpdateMonitor->GetUserTrustIsManaged(user, &trustManaged);
    Boolean running = FALSE;
    Logger::D("UnlockMethodCache", "TODO [UpdateMethodSecure 3] : Not implement the app Keyguard.");
    // mKeyguardUpdateMonitor->IsFaceUnlockRunning(user, &running);
    Boolean faceUnlockRunning = running && trustManaged;
    Boolean changed = methodInsecure != mMethodInsecure || trustManaged != mTrustManaged
            || faceUnlockRunning != mFaceUnlockRunning;
    if (changed || updateAlways) {
        mMethodInsecure = methodInsecure;
        mTrustManaged = trustManaged;
        mFaceUnlockRunning = faceUnlockRunning;
        NotifyListeners(mMethodInsecure);
    }
}

void UnlockMethodCache::NotifyListeners(
    /* [in] */ Boolean secure)
{
    AutoPtr<IIterator> ator;
    mListeners->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> listener;
        ator->GetNext((IInterface**)&listener);
        IOnUnlockMethodChangedListener::Probe(listener)->OnMethodSecureChanged(secure);
    }
}

Boolean UnlockMethodCache::IsTrustManaged()
{
    return mTrustManaged;
}

Boolean UnlockMethodCache::IsFaceUnlockRunning()
{
    return mFaceUnlockRunning;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
