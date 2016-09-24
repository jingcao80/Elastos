#include "CNodeListener.h"

#define JSActCarClassHead EE(JSPkgName,JSActName)

#include JSActCarClassHead

#include <elastos/droid/view/ViewGroup.h>

//#include "elastos/droid/support/v4/view/ViewPager.h"
//#include "elastos/droid/support/v4/view/PagerAdapter.h"

using Elastos::Droid::View::ViewGroup;

using Elastos::Utility::IArrayList;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IMotionEvent;

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::IViewGroupLayoutParams;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

JSCarClass(JSActName)
    , public ViewGroup
    , public IViewPager
    , public ICalculatorPadViewPager
{
public:
    CAR_INTERFACE_DECL()
    CAR_OBJECT_DECL()

private:
    class SuperObject
        : public Object
        , public ICalculatorPadViewPagerSuperObject
    {
    public:
        SuperObject(
            /* [in] */ JSActName* host)
            : mHost(host)
        {};

        CAR_INTERFACE_DECL()

        CARAPI Test();

    private:
        AutoPtr<JSActName> mHost;
    };

public:
    CCalculatorPadViewPager();

    virtual ~CCalculatorPadViewPager();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI _constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

//------------------------------------------
protected:
    CARAPI _OnFinishInflate();
    CARAPI OnFinishInflate();

protected:
    CARAPI _OnDetachedFromWindow();
    CARAPI OnDetachedFromWindow();

    CARAPI _GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i,
        /* [out] */ Int32* result);
    CARAPI_(Int32) GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i);

    CARAPI _VerifyDrawable(
        /* [in] */ IDrawable* who,
        /* [out] */ Boolean* result);
    //CARAPI VerifyDrawable(
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    CARAPI _DrawableStateChanged();
    CARAPI DrawableStateChanged();

public:
    CARAPI _OnSaveInstanceState(
        /* [out] */ IParcelable** result);
    //CARAPI OnSaveInstanceState(
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    CARAPI _OnRestoreInstanceState(
        /* [in] */ IParcelable* state);
    //CARAPI OnRestoreInstanceState(
    //    /* [in] */ IParcelable* state);
    CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    CARAPI _AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);
    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI _RemoveView(
        /* [in] */ IView* view);
    CARAPI RemoveView(
        /* [in] */ IView* view);

protected:
    CARAPI _OnAttachedToWindow();
    CARAPI OnAttachedToWindow();

    CARAPI _OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI _OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);
    //CARAPI OnSizeChanged(
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    CARAPI _OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

public:
    CARAPI _ComputeScroll();
    CARAPI ComputeScroll();

    CARAPI _OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    CARAPI _OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    CARAPI _Draw(
        /* [in] */ ICanvas* canvas);
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI _OnDraw(
        /* [in] */ ICanvas* canvas);
    //CARAPI OnDraw(
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI _DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    CARAPI _AddFocusables(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode);
    CARAPI AddFocusables(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode);

    CARAPI _AddTouchables(
        /* [in] */ IArrayList* views);
    CARAPI AddTouchables(
        /* [in] */ IArrayList* views);

protected:
    CARAPI _OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);
    //CARAPI OnRequestFocusInDescendants(
    CARAPI_(Boolean) OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

public:
    CARAPI _DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res);
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res);

protected:
    CARAPI _GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    CARAPI _GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ IViewGroupLayoutParams** params);
    //CARAPI GenerateLayoutParams(
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    CARAPI _CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ Boolean* result);
    //CARAPI CheckLayoutParams(
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

public:
    CARAPI _GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

//------------------------------------------

public:
    CARAPI GetSuperObject(
        /* [out] */ ICalculatorPadViewPagerSuperObject** ppSuperObject);

private:
        AutoPtr<ICalculatorPadViewPagerListener> mListener;
        AutoPtr<ICalculatorPadViewPagerSuperObject> mSuperObject;
};

} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos