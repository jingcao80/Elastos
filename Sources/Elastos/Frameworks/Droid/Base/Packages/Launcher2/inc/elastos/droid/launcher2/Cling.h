#ifndef  __ELASTOS_DROID_LAUNCHER2_CLING_H__
#define  __ELASTOS_DROID_LAUNCHER2_CLING_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class Cling
    : public FrameLayout
{
public:
    Cling(
        /* [in] */ IContext* context);

    Cling(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    Cling(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI_(void) Init(
        /* [in] */ ILauncher* l,
        /* [in] */ ArrayOf<Int32>* positionData);

    CARAPI_(void) Cleanup();

    CARAPI_(String) GetDrawIdentifier();

    //@Override
    CARAPI FocusSearch(
        /* [in] */ Int32 direction,
        /* [out] */ IView** res);

    //@Override
    CARAPI FocusSearch(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ IView** res);

    //@Override
    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

protected:
    //@Override
    CARAPI_(void) DispatchDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetPunchThroughPositions();

protected:
    static const String WORKSPACE_CLING_DISMISSED_KEY;
    static const String ALLAPPS_CLING_DISMISSED_KEY;
    static const String FOLDER_CLING_DISMISSED_KEY;

private:
    static String WORKSPACE_PORTRAIT;
    static String WORKSPACE_LANDSCAPE;
    static String WORKSPACE_LARGE;
    static String WORKSPACE_CUSTOM;

    static String ALLAPPS_PORTRAIT;
    static String ALLAPPS_LANDSCAPE;
    static String ALLAPPS_LARGE;

    static String FOLDER_PORTRAIT;
    static String FOLDER_LANDSCAPE;
    static String FOLDER_LARGE;

    AutoPtr<ILauncher> mLauncher;
    Boolean mIsInitialized;
    String mDrawIdentifier;
    AutoPtr<IDrawable> mBackground;
    AutoPtr<IDrawable> mPunchThroughGraphic;
    AutoPtr<IDrawable> mHandTouchGraphic;
    Int32 mPunchThroughGraphicCenterRadius;
    Int32 mAppIconSize;
    Int32 mButtonBarHeight;
    Float mRevealRadius;
    AutoPtr<ArrayOf<Int32> > mPositionData;

    AutoPtr<IPaint> mErasePaint;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CLING_H__