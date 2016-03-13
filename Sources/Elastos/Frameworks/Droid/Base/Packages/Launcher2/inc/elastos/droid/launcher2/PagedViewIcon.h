#ifndef  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWICON_H__
#define  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWICON_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class PagedViewIcon
    : public TextView
    , public IPagedViewIcon
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ PagedViewIcon* host);

        CARAPI Run();

    private:
        PagedViewIcon* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    PagedViewIcon();

    virtual ~PagedViewIcon() {}

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
    Boolean mLockDrawableState = FALSE;

    AutoPtr<IBitmap> mIcon;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWICON_H__