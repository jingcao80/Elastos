#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOWROTATIONWATCHER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOWROTATIONWATCHER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>
//#include "elastos/droid/internal/policy/impl/PhoneWindow.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IRotationWatcher;
using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class PhoneWindowRotationWatcher
    : public Object
    , public IBinder
    , public IPhoneWindowRotationWatcher
    , public IRotationWatcher
{
private:
    class InnerRunnable1
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRunnable1(
            /* [in] */ PhoneWindowRotationWatcher* owner);

        CARAPI Run();

    private:
        PhoneWindowRotationWatcher* mOwner;
    };


public:
    CAR_INTERFACE_DECL()

    PhoneWindowRotationWatcher();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IPhoneWindow* owner);

    // @Override
    CARAPI OnRotationChanged(
        /* [in] */ Int32 rotation);

    virtual CARAPI AddWindow(
        /* [in] */ IPhoneWindow* phoneWindow);

    virtual CARAPI RemoveWindow(
        /* [in] */ IPhoneWindow* phoneWindow);

    virtual CARAPI DispatchRotationChanged();

    CARAPI ToString(
        /* [out] */ String* info);
private:
    AutoPtr<IHandler> mHandler;
    /*const*/ AutoPtr<IRunnable> mRotationChanged;
    ///*const*/ AutoPtr< ArrayList< AutoPtr< IWeakReference<PhoneWindow> > > > mWindows;
    //store IPhoneWindow
    List<AutoPtr<IWeakReference> > mWindows;
    Boolean mIsWatching;
    IPhoneWindow* mOwner;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOWROTATIONWATCHER_H__
