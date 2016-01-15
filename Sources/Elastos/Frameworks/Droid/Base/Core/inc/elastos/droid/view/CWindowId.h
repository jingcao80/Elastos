
#ifndef __ELASTOS_DROID_VIEW_CWINDOWID_H__
#define __ELASTOS_DROID_VIEW_CWINDOWID_H__

#include "_Elastos_Droid_View_CWindowId.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Safe identifier for a window.  This currently allows you to retrieve and observe
 * the input focus state of the window.  Most applications will
 * not use this, instead relying on the simpler (and more efficient) methods available
 * on {@link View}.  This classes is useful when window input interactions need to be
 * done across processes: the class itself is a Parcelable that can be passed to other
 * processes for them to interact with your window, and it provides a limited safe API
 * that doesn't allow the other process to negatively harm your window.
 */
CarClass(CWindowId)
    , public Object
    , public IWindowId
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor();

    /**
     * Retrieve the current focus state of the associated window.
     */
    CARAPI IsFocused(
        /* [out] */ Boolean* focused);

    /**
     * Start monitoring for changes in the focus state of the window.
     */
    CARAPI RegisterFocusObserver(
        /* [in] */ IWindowIdFocusObserver* observer);

    /**
     * Stop monitoring changes in the focus state of the window.
     */
    CARAPI UnregisterFocusObserver(
        /* [in] */ IWindowIdFocusObserver* observer);

    /**
     * Comparison operator on two IntentSender objects, such that true
     * is returned then they both represent the same operation from the
     * same package.
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* otherObj,
        /* [out] */ Boolean* e);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /** @hide */
    CARAPI GetTarget(
        /* [out] */ IIWindowId** id);

    /** @hide */
    CARAPI constructor(
        /* [in] */ IIWindowId* target);

    /** @hide */
    CARAPI constructor(
        /* [in] */ IBinder* target);

private:
    AutoPtr<IIWindowId> mToken;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CWINDOWID_H__
