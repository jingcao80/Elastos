
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.View.h>
#include "elastos/apps/packageinstaller/CaffeinatedScrollView.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

//=====================================================================
//                        CaffeinatedScrollView
//=====================================================================
CaffeinatedScrollView::CaffeinatedScrollView()
    : mBottomSlop(0)
{
}

ECode CaffeinatedScrollView::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // super(context);

    assert(0);
    //--: class donot export yet;
    return NOERROR;//ScrollView::constructor(context);
}

ECode CaffeinatedScrollView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // super(context, attrs);

    assert(0);
    //--: class donot export yet;
    return NOERROR;//ScrollView::constructor(context, attrs);
}

ECode CaffeinatedScrollView::AwakenScrollBars(
    /* [out] */ Boolean* result)
{
    // ==================before translated======================
    // return super.awakenScrollBars();

    assert(0);
    //--: class donot export yet;
    return NOERROR;//ScrollView::AwakenScrollBars();
}

ECode CaffeinatedScrollView::SetFullScrollAction(
    /* [in] */ IRunnable* action)
{
    VALIDATE_NOT_NULL(action);
    // ==================before translated======================
    // mFullScrollAction = action;
    // mBottomSlop = (int)(4 * getResources().getDisplayMetrics().density);

    assert(0);
    mFullScrollAction = action;
    AutoPtr<IResources> resources;
    //--: GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> displayMetrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    Float density;
    displayMetrics->GetDensity(&density);
    mBottomSlop = (Int32)(4 * density);
    return NOERROR;
}

void CaffeinatedScrollView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    // ==================before translated======================
    // super.onDraw(canvas);
    // checkFullScrollAction();

    assert(0);
    //--: class donot export yet; ScrollView::OnDraw(canvas);
    CheckFullScrollAction();
}

void CaffeinatedScrollView::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    // ==================before translated======================
    // super.onScrollChanged(l, t, oldl, oldt);
    // checkFullScrollAction();

    assert(0);
    //--: class donot export yet; ScrollView::OnScrollChanged(l, t, oldl, oldt);
    CheckFullScrollAction();
}

void CaffeinatedScrollView::CheckFullScrollAction()
{
    // ==================before translated======================
    // if (mFullScrollAction != null) {
    //     int daBottom = getChildAt(0).getBottom();
    //     int screenBottom = getScrollY() + getHeight() - getPaddingBottom();
    //     if ((daBottom - screenBottom) < mBottomSlop) {
    //         mFullScrollAction.run();
    //         mFullScrollAction = null;
    //     }
    // }

    assert(0);
    if (mFullScrollAction != NULL) {
        AutoPtr<IView> viewTmp;
        //--: GetChildAt(0, (IView**)&viewTmp);
        Int32 daBottom;
        viewTmp->GetBottom(&daBottom);

        //Int32 scrollY = 0;
        //--: GetScrollY(&scrollY);
        Int32 height = 0;
        //--: GetHeight(&height);
        Int32 paddingBottom = 0;
        //--: GetPaddingBottom(&paddingBottom);
        Int32 screenBottom =  + height - paddingBottom;
        if ((daBottom - screenBottom) < mBottomSlop) {
            mFullScrollAction->Run();
            mFullScrollAction = NULL;
        }
    }
}

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos


