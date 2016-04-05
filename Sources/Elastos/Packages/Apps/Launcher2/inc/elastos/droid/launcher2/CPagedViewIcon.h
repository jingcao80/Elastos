#ifndef  __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWICON_H__
#define  __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWICON_H__

#include "_Launcher2.h"
#include "_Elastos_Droid_Launcher2_CPagedViewIcon.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::TextView;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CPagedViewIcon)
    , public TextView
    , public IPagedViewIcon
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CPagedViewIcon* host);

        CARAPI Run();

    private:
        AutoPtr<CPagedViewIcon> mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CPagedViewIcon();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI ApplyFromApplicationInfo(
        /* [in] */ IApplicationInfo* info,
        /* [in] */ Boolean scaleUp,
        /* [in] */ IPagedViewIconPressedCallback* cb);

    CARAPI LockDrawableState();

    CARAPI ResetDrawableState();

protected:
    CARAPI DrawableStateChanged();

private:
    //@SuppressWarnings("unused")
    static const String TAG;
    static const Float PRESS_ALPHA;

    AutoPtr<IPagedViewIconPressedCallback> mPressedCallback;
    Boolean mLockDrawableState;

    AutoPtr<IBitmap> mIcon;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWICON_H__