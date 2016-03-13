#ifndef  __ELASTOS_DROID_LAUNCHER2_HOTSEAT_H__
#define  __ELASTOS_DROID_LAUNCHER2_HOTSEAT_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class Hotseat
    : public FrameLayout
    , public IHotseat
{
private:
    class MyOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyOnTouchListener(
            /* [in] */ ILauncher* launcher);

        //@Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<ILauncher> mLauncher;
    };

    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyOnClickListener(
            /* [in] */ ILauncher* launcher);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<ILauncher> mLauncher;
    };

public:
    CAR_INTERFACE_DECL();

    Hotseat();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI Setup(
        /* [in] */ ILauncher* launcher);

    CARAPI GetLayout(
        /* [out] */ ICellLayout** cout);

    /* Get the orientation invariant order of the item in the hotseat for persistence. */
    CARAPI GetOrderInHotseat(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Int32* result);

    /* Get the orientation specific coordinates given an invariant order in the hotseat. */
    CARAPI GetCellXFromOrder(
        /* [in] */ Int32 rank,
        /* [out] */ Int32* result);

    CARAPI GetCellYFromOrder(
        /* [in] */ Int32 rank,
        /* [out] */ Int32* result);

    CARAPI IsAllAppsButtonRank(
        /* [in] */ Int32 rank,
        /* [out] */ Boolean* result);

    CARAPI ResetLayout();

protected:
    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(Boolean) HasVerticalHotseat();

private:
    //@SuppressWarnings("unused")
    static const String TAG;

    AutoPtr<ILauncher> mLauncher;
    AutoPtr<ICellLayout> mContent;

    Int32 mCellCountX;
    Int32 mCellCountY;
    Int32 mAllAppsButtonRank;

    Boolean mTransposeLayoutWithOrientation;
    Boolean mIsLandscape;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_HOTSEAT_H__