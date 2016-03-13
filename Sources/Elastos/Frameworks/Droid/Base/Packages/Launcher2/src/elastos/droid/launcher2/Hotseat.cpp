
#include "elastos/droid/launcher2/Hotseat.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(Hotseat::MyOnTouchListener, Object, IViewOnTouchListener);

Hotseat::MyOnTouchListener::MyOnTouchListener(
    /* [in] */ ILauncher* launcher)
    : mLauncher(launcher)
{
}

ECode Hotseat::MyOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mLauncher != NULL) {
        Int32 action;
        event->GetAction(&action);
        if ((action & IMotionEvent::ACTION_MASK) == IMotionEvent::ACTION_DOWN) {
            mLauncher->OnTouchDownAllAppsButton(v, &result);
        }
    }
    *result = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(Hotseat::MyOnClickListener, Object, IViewOnClickListener);

Hotseat::MyOnClickListener::MyOnClickListener(
    /* [in] */ ILauncher* launcher)
    : mLauncher(launcher)
{
}

ECode Hotseat::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (mLauncher != NULL) {
        return mLauncher->OnClickAllAppsButton(v);
    }
    return NOERROR;
}


const String Hotseat::TAG("Hotseat");

CAR_INTERFACE_IMPL(Hotseat, FrameLayout, IHotseat);

Hotseat::Hotseat()
    : mCellCountX(0)
    , mCellCountY(0)
    , mAllAppsButtonRank(0)
    , mTransposeLayoutWithOrientation(FALSE)
    , mIsLandscape(FALSE)
{
}

ECode Hotseat::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode Hotseat::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode Hotseat::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs,
            Elastos::Droid::Launcher2::R::styleable::Hotseat,
            defStyle, 0, (ITypedArray**)&a);
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    a->GetInt32(Elastos::Droid::Launcher2::R::styleable::Hotseat_cellCountX,
            -1, &mCellCountX);
    a->GetInt32(Elastos::Droid::Launcher2::R::styleable::Hotseat_cellCountY,
            -1, &mCellCountY);
    r->GetInteger(Elastos::Droid::Launcher2::R::integer::hotseat_all_apps_index,
            &mAllAppsButtonRank);
    r->GetBoolean(Elastos::Droid::Launcher2::R::bool::
            hotseat_transpose_layout_with_orientation, &mTransposeLayoutWithOrientation);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    mIsLandscape = orientation == IConfiguration::ORIENTATION_LANDSCAPE;
    return NOERROR;
}

ECode Hotseat::Setup(
    /* [in] */ ILauncher* launcher)
{
    mLauncher = launcher;
    AutoPtr<IViewOnKeyListener> listener = new HotseatIconKeyEventListener()
    return SetOnKeyListener(listener);
}

ECode Hotseat::GetLayout(
    /* [out] */ ICellLayout** cout)
{
    VALIDATE_NOT_NULL(cout);

    *cout = mContent;
    REFCOUNT_ADD(*cout);
    return NOERROR;
}

Boolean Hotseat::HasVerticalHotseat()
{
    return (mIsLandscape && mTransposeLayoutWithOrientation);
}

ECode Hotseat::GetOrderInHotseat(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    HasVerticalHotseat(&res);
    if (res) {
        Int32 countY;
        mContent->GetCountY(&countY);
        *result = countY - y - 1;
    }
    else {
        *result = x;
    }
    return NOERROR;
}

ECode Hotseat::GetCellXFromOrder(
    /* [in] */ Int32 rank,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    HasVerticalHotseat(&res);
    *result = res ? 0 : rank;
    return NOERROR;
}

ECode Hotseat::GetCellYFromOrder(
    /* [in] */ Int32 rank,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL();

    Boolean res;
    HasVerticalHotseat(&res);
    if (res) {
        Int32 countY;
        mContent->GetCountY(&countY);
        *result = countY - (rank + 1);
    }
    else {
        *result = 0;
    }
    return NOERROR;
}

ECode Hotseat::IsAllAppsButtonRank(
    /* [in] */ Int32 rank,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL();

    *result = rank == mAllAppsButtonRank;
    return NOERROR;
}

ECode Hotseat::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    if (mCellCountX < 0) {
        LauncherModel::GetCellCountX(&mCellCountX);
    }
    if (mCellCountY < 0) {
        LauncherModel::GetCellCountY(&mCellCountY);
    }
    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Launcher2::R::id::layout, (IView**)&view);
    mContent = ICellLayout::Probe(view);
    mContent->SetGridSize(mCellCountX, mCellCountY);
    mContent->SetIsHotseat(TRUE);

    return ResetLayout();
}

ECode Hotseat::ResetLayout()
{
    mContent->RemoveAllViewsInLayout();

    // Add the Apps button
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context);

    AutoPtr<IView> view;
    inflater->Inflate(Elastos::Droid::Launcher2::R::layout::application,
            mContent, FALSE, (IView**)&view);
    AutoPtr<IBubbleTextView> allAppsButton = IBubbleTextView::Probe(view);

    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    AutoPtr<IDrawable> drawable;
    r->GetDrawable(Elastos::Droid::Launcher2::R::
            drawable::all_apps_button_icon, (IDrawable**)&drawable);
    allAppsButton->SetCompoundDrawablesWithIntrinsicBounds(NULL, drawable, NULL, NULL);

    String lable;
    context->GetString(Elastos::Droid::Launcher2::R::
            string::all_apps_button_label, &lable)
    allAppsButton->SetContentDescription(lable);

    AutoPtr<MyOnTouchListener> listener1 = new MyOnTouchListener(mLauncher);
    allAppsButton->SetOnTouchListener(IViewOnTouchListener::Probe(listener1));

    AutoPtr<MyOnClickListener> listener2 = new MyOnClickListener(mLauncher);
    allAppsButton->SetOnClickListener(IViewOnClickListener::Probe(listener2));

    // Note: We do this to ensure that the hotseat is always laid out in the orientation of
    // the hotseat in order regardless of which orientation they were added
    Int32 x;
    GetCellXFromOrder(mAllAppsButtonRank, &x);
    Int32 y;
    GetCellYFromOrder(mAllAppsButtonRank, &y);
    AutoPtr<CellLayout::LayoutParams> lp = new CellLayout::LayoutParams(x,y,1,1);
    lp->mCanReorder = FALSE;
    return mContent->AddViewToCellLayout(allAppsButton, -1, 0, lp, TRUE);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos