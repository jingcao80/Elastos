
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/graphics/CRect.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/webkit/native/ui/DropdownPopupWindow.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::View;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::EIID_IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::IListView;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {

//=====================================================================
//           DropdownPopupWindow::InnerOnLayoutChangeListener
//=====================================================================
CAR_INTERFACE_IMPL(DropdownPopupWindow::InnerOnLayoutChangeListener, Object, IViewOnLayoutChangeListener)

DropdownPopupWindow::InnerOnLayoutChangeListener::InnerOnLayoutChangeListener(
    /* [in] */ DropdownPopupWindow* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode DropdownPopupWindow::InnerOnLayoutChangeListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // if (v == mAnchorView) DropdownPopupWindow.this.show();

    assert(NULL != mOwner);
    if (v == mOwner->mAnchorView)
        mOwner->Show();
    return NOERROR;
}

//=====================================================================
//        DropdownPopupWindow::InnerPopupWindowOnDismissListener
//=====================================================================
CAR_INTERFACE_IMPL(DropdownPopupWindow::InnerPopupWindowOnDismissListener, Object, IPopupWindowOnDismissListener)

DropdownPopupWindow::InnerPopupWindowOnDismissListener::InnerPopupWindowOnDismissListener(
    /* [in] */ DropdownPopupWindow* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode DropdownPopupWindow::InnerPopupWindowOnDismissListener::OnDismiss()
{
    // ==================before translated======================
    // if (mOnDismissListener != null) {
    //     mOnDismissListener.onDismiss();
    // }
    // mAnchorView.removeOnLayoutChangeListener(mLayoutChangeListener);
    // mAnchorView.setTag(null);
    // mViewAndroidDelegate.releaseAnchorView(mAnchorView);

    assert(NULL != mOwner);
    if (NULL != mOwner->mOnDismissListener) {
        mOwner->mOnDismissListener->OnDismiss();
    }
    mOwner->mAnchorView->RemoveOnLayoutChangeListener(mOwner->mLayoutChangeListener);
    mOwner->mAnchorView->SetTag(NULL);
    mOwner->mViewElastosDelegate->ReleaseAnchorView(mOwner->mAnchorView);
    return NOERROR;
}

//=====================================================================
//                         DropdownPopupWindow
//=====================================================================
DropdownPopupWindow::DropdownPopupWindow(
    /* [in] */ IContext* context,
    /* [in] */ ViewElastosDelegate* viewElastosDelegate)
    : mContext(context)
    , mViewElastosDelegate(viewElastosDelegate)
    , mAnchorView(mViewElastosDelegate->AcquireAnchorView())
    , mAnchorWidth(0.0f)
    , mAnchorHeight(0.0f)
    , mAnchorX(0.0f)
    , mAnchorY(0.0f)
{
    // ==================before translated======================
    // super(context, null, 0, R.style.DropdownPopupWindow);
    // mContext = context;
    // mViewAndroidDelegate = viewAndroidDelegate;
    //
    // mAnchorView = mViewAndroidDelegate.acquireAnchorView();
    // mAnchorView.setId(R.id.dropdown_popup_window);
    // mAnchorView.setTag(this);
    //
    // mLayoutChangeListener = new OnLayoutChangeListener() {
    //     @Override
    //     public void onLayoutChange(View v, int left, int top, int right, int bottom,
    //             int oldLeft, int oldTop, int oldRight, int oldBottom) {
    //         if (v == mAnchorView) DropdownPopupWindow.this.show();
    //     }
    // };
    // mAnchorView.addOnLayoutChangeListener(mLayoutChangeListener);
    //
    // super.setOnDismissListener(new PopupWindow.OnDismissListener() {
    //     @Override
    //     public void onDismiss() {
    //         if (mOnDismissListener != null) {
    //             mOnDismissListener.onDismiss();
    //         }
    //         mAnchorView.removeOnLayoutChangeListener(mLayoutChangeListener);
    //         mAnchorView.setTag(null);
    //         mViewAndroidDelegate.releaseAnchorView(mAnchorView);
    //     }
    // });
    //
    // setAnchorView(mAnchorView);

    assert(0);
    ListPopupWindow::constructor(context, NULL, 0, -1/*R::style::DropdownPopupWindow*/);
    mAnchorView->SetId(-1/*R::id::dropdown_popup_window*/);
    mAnchorView->SetTag(TO_IINTERFACE(this));

    mLayoutChangeListener = new InnerOnLayoutChangeListener(this);
    mAnchorView->AddOnLayoutChangeListener(mLayoutChangeListener);

    AutoPtr<IPopupWindowOnDismissListener> dismissListener = new InnerPopupWindowOnDismissListener(this);
    SetOnDismissListener(dismissListener);
    SetAnchorView(mAnchorView);
}

ECode DropdownPopupWindow::SetAnchorRect(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    // ==================before translated======================
    // mAnchorWidth = width;
    // mAnchorHeight = height;
    // mAnchorX = x;
    // mAnchorY = y;
    // if (mAnchorView != null) {
    //     mViewAndroidDelegate.setAnchorViewPosition(mAnchorView, mAnchorX, mAnchorY,
    //             mAnchorWidth, mAnchorHeight);
    // }

    mAnchorWidth = width;
    mAnchorHeight = height;
    mAnchorX = x;
    mAnchorY = y;
    if (NULL != mAnchorView) {
        mViewElastosDelegate->SetAnchorViewPosition(mAnchorView, mAnchorX, mAnchorY,mAnchorWidth, mAnchorHeight);
    }
    return NOERROR;
}

ECode DropdownPopupWindow::SetAdapter(
    /* [in] */ IListAdapter* adapter)
{
    VALIDATE_NOT_NULL(adapter);
    // ==================before translated======================
    // mAdapter = adapter;
    // super.setAdapter(adapter);

    mAdapter = adapter;
    ListPopupWindow::SetAdapter(adapter);
    return NOERROR;
}

ECode DropdownPopupWindow::Show()
{
    // ==================before translated======================
    // // An ugly hack to keep the popup from expanding on top of the keyboard.
    // setInputMethodMode(INPUT_METHOD_NEEDED);
    // int contentWidth = measureContentWidth();
    // float contentWidthInDip = contentWidth /
    //         mContext.getResources().getDisplayMetrics().density;
    // if (contentWidthInDip > mAnchorWidth) {
    //     setContentWidth(contentWidth);
    //     final Rect displayFrame = new Rect();
    //     mAnchorView.getWindowVisibleDisplayFrame(displayFrame);
    //     if (getWidth() > displayFrame.width()) {
    //         setWidth(displayFrame.width());
    //     }
    // } else {
    //     setWidth(ViewGroup.LayoutParams.WRAP_CONTENT);
    // }
    // mViewAndroidDelegate.setAnchorViewPosition(mAnchorView, mAnchorX, mAnchorY, mAnchorWidth,
    //         mAnchorHeight);
    // super.show();
    // getListView().setDividerHeight(0);

    assert(0);
    // An ugly hack to keep the popup from expanding on top of the keyboard.
    SetInputMethodMode(INPUT_METHOD_NEEDED);
    Int32 contentWidth = MeasureContentWidth();

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density = 0.0f;
    metrics->GetDensity(&density);
    Float contentWidthInDip = contentWidth / density;

    if (contentWidthInDip > mAnchorWidth) {
        SetContentWidth(contentWidth);
        AutoPtr<IRect> displayFrame;
        CRect::New((IRect**)&displayFrame);
        mAnchorView->GetWindowVisibleDisplayFrame(displayFrame);

        Int32 displayWidth = 0;
        displayFrame->GetWidth(&displayWidth);
        Int32 width = 0;
        GetWidth(&width);
        if (width > displayWidth) {
            SetWidth(displayWidth);
        }
    }
    else {
        SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
    }

    mViewElastosDelegate->SetAnchorViewPosition(mAnchorView, mAnchorX, mAnchorY, mAnchorWidth, mAnchorHeight);
    ListPopupWindow::Show();

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    listView->SetDividerHeight(0);
    return NOERROR;
}

ECode DropdownPopupWindow::SetOnDismissListener(
    /* [in] */ IPopupWindowOnDismissListener* listener)
{
    // ==================before translated======================
    // mOnDismissListener = listener;

    mOnDismissListener = listener;
    return NOERROR;
}

Int32 DropdownPopupWindow::MeasureContentWidth()
{
    // ==================before translated======================
    // int maxWidth = 0;
    // View itemView = null;
    // if (mAdapter == null)
    //   return 0;
    // final int widthMeasureSpec = MeasureSpec.makeMeasureSpec(0, MeasureSpec.UNSPECIFIED);
    // final int heightMeasureSpec = MeasureSpec.makeMeasureSpec(0, MeasureSpec.UNSPECIFIED);
    // for (int i = 0; i < mAdapter.getCount(); i++) {
    //     itemView = mAdapter.getView(i, itemView, null);
    //     LinearLayout.LayoutParams params =
    //             new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT,
    //                     LinearLayout.LayoutParams.WRAP_CONTENT);
    //     itemView.setLayoutParams(params);
    //     itemView.measure(widthMeasureSpec, heightMeasureSpec);
    //     maxWidth = Math.max(maxWidth, itemView.getMeasuredWidth());
    // }
    // return maxWidth;

    Int32 maxWidth = 0;
    AutoPtr<IView> itemView;
    if (NULL == mAdapter)
      return 0;

    Int32 widthMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(0, Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);
    Int32 heightMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(0, Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);

    Int32 count = 0;
    Int32 mearsureWidth = 0;
    IAdapter::Probe(mAdapter)->GetCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> itemViewTmp;
        IAdapter::Probe(mAdapter)->GetView(i, itemView, NULL, (IView**)&itemViewTmp);
        AutoPtr<ILinearLayoutLayoutParams> params;
        CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT, (ILinearLayoutLayoutParams**)&params);

        IViewGroupLayoutParams* layoutParamsTmp = IViewGroupLayoutParams::Probe(params);
        itemViewTmp->SetLayoutParams(layoutParamsTmp);
        itemViewTmp->Measure(widthMeasureSpec, heightMeasureSpec);
        itemViewTmp->GetMeasuredWidth(&mearsureWidth);
        maxWidth = Elastos::Core::Math::Max(maxWidth, mearsureWidth);
        itemView = NULL;
        itemView = itemViewTmp;
    }
    return maxWidth;
}

} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


