
#include "elastos/droid/systemui/recent/RecentsPanelView.h"
#include "elastos/droid/systemui/recent/RecentTasksLoader.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/systemui/recent/FirstFrameAnimatorHelper.h"
#include "elastos/droid/systemui/recent/RecentsActivity.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::CTaskStackBuilderHelper;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IActivityOptions;;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::ITaskStackBuilder;
using Elastos::Droid::App::ITaskStackBuilderHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::ShaderTileMode_REPEAT;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::SystemUI::Recent::EIID_IRecentsCallback;
using Elastos::Droid::SystemUI::Recent::EIID_ITaskDescriptionAdapter;
using Elastos::Droid::SystemUI::Recent::IRecentsActivity;
using Elastos::Droid::SystemUI::Recent::RecentTasksLoader;
using Elastos::Droid::SystemUI::StatusBar::EIID_IStatusBarPanel;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Phone::IPhoneStatusBar;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::Widget::CPopupMenu;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IPopupMenuOnDismissListener;
using Elastos::Droid::Widget::EIID_IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Widget::ImageViewScaleType_MATRIX;
using Elastos::Droid::Widget::ImageViewScaleType_FIT_XY;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::Math;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {


//=================================================================================
// RecentsPanelView::InnerListener
//=================================================================================
CAR_INTERFACE_IMPL(RecentsPanelView::InnerListener, Object, IRecentsCallback)

RecentsPanelView::InnerListener::InnerListener(
    /* [in] */ RecentsPanelView* host)
    : mHost(host)
{}

ECode RecentsPanelView::InnerListener::HandleOnClick(
    /* [in] */ IView* view)
{
    return mHost->HandleOnClick(view);
}

ECode RecentsPanelView::InnerListener::HandleSwipe(
    /* [in] */ IView* view)
{
    return mHost->HandleSwipe(view);
}

ECode RecentsPanelView::InnerListener::HandleLongPress(
    /* [in] */ IView* selectedView,
    /* [in] */ IView* anchorView,
    /* [in] */ IView* thumbnailView)
{
    return mHost->HandleLongPress(selectedView, anchorView, thumbnailView);
}

ECode RecentsPanelView::InnerListener::Dismiss()
{
    return mHost->Dismiss();
}

//=================================================================================
// RecentsPanelView::ViewHolder
//=================================================================================

RecentsPanelView::ViewHolder::ViewHolder()
    : mLoadedThumbnailAndIcon(FALSE)
{}

ECode RecentsPanelView::ViewHolder::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("ViewHolder{0x");
    sb += StringUtils::ToHexString((Int32)this);

    if (mTaskDescription != NULL) {
        TaskDescription* td = (TaskDescription*)mTaskDescription.Get();
        sb += ", packageName:";
        sb += td->mPackageName;
    }

    if (mLabelView != NULL) {
        AutoPtr<ICharSequence> csq;
        mLabelView->GetText((ICharSequence**)&csq);
        sb += ", label:";
        sb += TO_CSTR(csq);
    }

    if (mDescriptionView != NULL) {
        AutoPtr<ICharSequence> csq;
        mDescriptionView->GetText((ICharSequence**)&csq);
        sb += ", description:";
        sb += TO_CSTR(csq);
    }

    if (mIconView != NULL) {
        sb += ", iconView:";
        sb += TO_CSTR(mIconView);
    }

    if (mThumbnailViewImage != NULL) {
        sb += ", iconView:";
        sb += TO_CSTR(mThumbnailViewImage);
    }

    if (mThumbnailViewDrawable != NULL) {
        sb += ", thumbnailViewDrawable:";
        sb += TO_CSTR(mThumbnailViewDrawable);
    }

    *str = sb.ToString();
    return NOERROR;
}

//=================================================================================
// RecentsPanelView::OnLongClickDelegate
//=================================================================================

CAR_INTERFACE_IMPL(RecentsPanelView::OnLongClickDelegate, Object, IViewOnLongClickListener)

RecentsPanelView::OnLongClickDelegate::OnLongClickDelegate(
    /* [in] */ IView* other)
    : mOtherView(other)
{}

ECode RecentsPanelView::OnLongClickDelegate::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    return mOtherView->PerformLongClick(result);
}

//=================================================================================
// RecentsPanelView::TaskDescriptionAdapter
//=================================================================================

CAR_INTERFACE_IMPL(RecentsPanelView::TaskDescriptionAdapter, BaseAdapter, ITaskDescriptionAdapter)

RecentsPanelView::TaskDescriptionAdapter::TaskDescriptionAdapter(
    /* [in] */ IContext* context,
    /* [in] */ RecentsPanelView* host)
    : mHost(host)
{
    LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
}

Int32 RecentsPanelView::TaskDescriptionAdapter::GetCount()
{
    Int32 size = 0;
    if (mHost->mRecentTaskDescriptions != NULL) {
        mHost->mRecentTaskDescriptions->GetSize(&size);
    }
    return size;
}

ECode RecentsPanelView::TaskDescriptionAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = GetCount();
    return NOERROR;
}

AutoPtr<IInterface> RecentsPanelView::TaskDescriptionAdapter::GetItem(
    /* [in] */ Int32 position)
{
    AutoPtr<IInteger32> item;
    CInteger32::New(position, (IInteger32**)&item);
    return item;
}

ECode RecentsPanelView::TaskDescriptionAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    AutoPtr<IInterface> i = GetItem(position);
    *item = i;
    REFCOUNT_ADD(*item);
    return NOERROR;
}

Int64 RecentsPanelView::TaskDescriptionAdapter::GetItemId(
    /* [in] */ Int32 position)
{
    return position;
}

ECode RecentsPanelView::TaskDescriptionAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = GetItemId(position);
    return NOERROR;
}

ECode RecentsPanelView::TaskDescriptionAdapter::CreateView(
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> convertView;
    mInflater->Inflate(mHost->mRecentItemLayoutId, parent, FALSE, (IView**)&convertView);
    AutoPtr<ViewHolder> holder = new ViewHolder();
    AutoPtr<IView> v;
    convertView->FindViewById(R::id::app_thumbnail, (IView**)&v);
    holder->mThumbnailView = v;
    v = NULL;
    convertView->FindViewById(R::id::app_thumbnail_image, (IView**)&v);
    holder->mThumbnailViewImage = IImageView::Probe(v);
    // If we set the default thumbnail now, we avoid an onLayout when we update
    // the thumbnail later (if they both have the same dimensions)
    AutoPtr<IDrawable> drawable;
    mHost->mRecentTasksLoader->GetDefaultThumbnail((IDrawable**)&drawable);
    mHost->UpdateThumbnail(holder, drawable, FALSE, FALSE);
    v = NULL;
    convertView->FindViewById(R::id::app_icon, (IView**)&v);
    holder->mIconView = IImageView::Probe(v);
    AutoPtr<IDrawable> icon;
    mHost->mRecentTasksLoader->GetDefaultIcon((IDrawable**)&icon);
    holder->mIconView->SetImageDrawable(icon);
    v = NULL;
    convertView->FindViewById(R::id::app_label, (IView**)&v);
    holder->mLabelView = ITextView::Probe(v);
    v = NULL;
    convertView->FindViewById(R::id::recents_callout_line, (IView**)&v);
    holder->mCalloutLine = v;
    v = NULL;
    convertView->FindViewById(R::id::app_description, (IView**)&v);
    holder->mDescriptionView = ITextView::Probe(v);

    convertView->SetTag((IObject*)holder);
    *view = convertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

AutoPtr<IView> RecentsPanelView::TaskDescriptionAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> convertView = _convertView;
    if (convertView == NULL) {
        CreateView(parent, (IView**)&convertView);
    }
    AutoPtr<IInterface> tag;
    convertView->GetTag((IInterface**)&tag);
    AutoPtr<ViewHolder> holder = (ViewHolder*)(IObject::Probe(tag));

    // index is reverse since most recent appears at the bottom...
    Int32 size;
    mHost->mRecentTaskDescriptions->GetSize(&size);
    Int32 index = size - position - 1;

    AutoPtr<IInterface> tdObj;
    mHost->mRecentTaskDescriptions->Get(index, (IInterface**)&tdObj);
    AutoPtr<ITaskDescription> td = ITaskDescription::Probe(tdObj);

    AutoPtr<ICharSequence> cs;
    td->GetLabel((ICharSequence**)&cs);
    holder->mLabelView->SetText(cs);
    holder->mThumbnailView->SetContentDescription(cs);

    td->IsLoaded(&holder->mLoadedThumbnailAndIcon);
    if (holder->mLoadedThumbnailAndIcon) {
        AutoPtr<IDrawable> tn;
        td->GetThumbnail((IDrawable**)&tn);
        AutoPtr<IDrawable> dr;
        td->GetIcon((IDrawable**)&dr);
        mHost->UpdateThumbnail(holder, tn, TRUE, FALSE);
        mHost->UpdateIcon(holder, dr, TRUE, FALSE);
    }

    if (index == 0) {
        if (mHost->mAnimateIconOfFirstTask) {
            if (mHost->mItemToAnimateInWhenWindowAnimationIsFinished != NULL) {
                AutoPtr<ViewHolder> oldHolder = mHost->mItemToAnimateInWhenWindowAnimationIsFinished;
                if (oldHolder != NULL) {
                    oldHolder->mIconView->SetAlpha(1);
                    IView* iconView = IView::Probe(oldHolder->mIconView);
                    iconView->SetTranslationX(0);
                    iconView->SetTranslationY(0);
                    IView* lableView = IView::Probe(oldHolder->mLabelView);
                    lableView->SetAlpha(1);
                    lableView->SetTranslationX(0);
                    lableView->SetTranslationY(0);
                    if (oldHolder->mCalloutLine != NULL) {
                        oldHolder->mCalloutLine->SetAlpha(1);
                        oldHolder->mCalloutLine->SetTranslationX(0);
                        oldHolder->mCalloutLine->SetTranslationY(0);
                    }
                }
            }

            mHost->mItemToAnimateInWhenWindowAnimationIsFinished = holder;
            AutoPtr<IResources> resources;
            mHost->GetResources((IResources**)&resources);
            Int32 ival;
            resources->GetDimensionPixelSize(
                R::dimen::status_bar_recents_app_icon_translate_distance, &ival);
            Int32 translation = -ival;
            AutoPtr<IConfiguration> config;
            resources->GetConfiguration((IConfiguration**)&config);
            Int32 orientation;
            config->GetOrientation(&orientation);
            if (orientation == IConfiguration::ORIENTATION_PORTRAIT) {
                Int32 direction;
                mHost->GetLayoutDirection(&direction);
                if (direction == IView::LAYOUT_DIRECTION_RTL) {
                    translation = -translation;
                }
                IView* iconView = IView::Probe(holder->mIconView);
                holder->mIconView->SetAlpha(0);
                iconView->SetTranslationX(translation);
                IView* lableView = IView::Probe(holder->mLabelView);
                lableView->SetAlpha(0);
                lableView->SetTranslationX(translation);
                holder->mCalloutLine->SetAlpha(0);
                holder->mCalloutLine->SetTranslationX(translation);
            }
            else {
                holder->mIconView->SetAlpha(0);
                IView::Probe(holder->mIconView)->SetTranslationY(translation);
            }
            if (!mHost->mWaitingForWindowAnimation) {
                mHost->AnimateInIconOfFirstTask();
            }
        }
    }

    holder->mThumbnailView->SetTag(td);
    AutoPtr<OnLongClickDelegate> delegate = new OnLongClickDelegate(convertView);
    holder->mThumbnailView->SetOnLongClickListener(delegate);
    holder->mTaskDescription = td;
    return convertView;
}

ECode RecentsPanelView::TaskDescriptionAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> v = GetView(position, convertView, parent);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode RecentsPanelView::TaskDescriptionAdapter::RecycleView(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> tag;
    v->GetTag((IInterface**)&tag);
    AutoPtr<ViewHolder> holder = (ViewHolder*)(IObject::Probe(tag));
    AutoPtr<IDrawable> drawable;
    mHost->mRecentTasksLoader->GetDefaultThumbnail((IDrawable**)&drawable);
    mHost->UpdateThumbnail(holder, drawable, FALSE, FALSE);
    AutoPtr<IDrawable> icon;
    mHost->mRecentTasksLoader->GetDefaultIcon((IDrawable**)&icon);
    holder->mIconView->SetImageDrawable(icon);
    IView* iconView = IView::Probe(holder->mIconView);
    iconView->SetVisibility(IView::INVISIBLE);
    AutoPtr<IViewPropertyAnimator> animator;
    iconView->Animate((IViewPropertyAnimator**)&animator);
    animator->Cancel();
    holder->mLabelView->SetText((ICharSequence*)NULL);
    animator = NULL;
    IView* labelView = IView::Probe(holder->mLabelView);
    labelView->Animate((IViewPropertyAnimator**)&animator);
    animator->Cancel();
    holder->mThumbnailView->SetContentDescription(NULL);
    holder->mThumbnailView->SetTag(NULL);
    holder->mThumbnailView->SetOnLongClickListener(NULL);
    holder->mThumbnailView->SetVisibility(IView::INVISIBLE);
    holder->mIconView->SetAlpha(1.f);
    iconView->SetTranslationX(0);
    iconView->SetTranslationY(0);
    labelView->SetAlpha(1);
    labelView->SetTranslationX(0);
    labelView->SetTranslationY(0);
    if (holder->mCalloutLine != NULL) {
        holder->mCalloutLine->SetAlpha(1);
        holder->mCalloutLine->SetTranslationX(0);
        holder->mCalloutLine->SetTranslationY(0);
        animator = NULL;
        holder->mCalloutLine->Animate((IViewPropertyAnimator**)&animator);
        animator->Cancel();
    }
    holder->mTaskDescription = NULL;
    holder->mLoadedThumbnailAndIcon = FALSE;
    return NOERROR;
}

//=================================================================================
// RecentsPanelView::OnMenuItemClickListener
//=================================================================================

CAR_INTERFACE_IMPL(RecentsPanelView::OnMenuItemClickListener, Object, IPopupMenuOnMenuItemClickListener)

RecentsPanelView::OnMenuItemClickListener::OnMenuItemClickListener(
    /* [in] */ RecentsPanelView* host,
    /* [in] */ IView* selectedView)
    : mHost(host)
    , mSelectedView(selectedView)
{}

ECode RecentsPanelView::OnMenuItemClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 id;
    item->GetItemId(&id);
        if (id == R::id::recent_remove_item) {
        IViewGroup::Probe(mHost->mRecentsContainer)->RemoveViewInLayout(mSelectedView);
        *result = TRUE;
        return NOERROR;
    }
    else if (id == R::id::recent_inspect_item) {
        AutoPtr<IInterface> tag;
        mSelectedView->GetTag((IInterface**)&tag);
        if (tag != NULL) {
            ViewHolder* viewHolder = (RecentsPanelView::ViewHolder*)(IObject::Probe(tag));
            AutoPtr<TaskDescription> ad = (TaskDescription*)viewHolder->mTaskDescription.Get();
            mHost->StartApplicationDetailsActivity(ad->mPackageName, ad->mUserId);
            mHost->Show(FALSE);
            *result = TRUE;
            return NOERROR;
        }
        else {
            // throw new IllegalStateException("Oops, no tag on view " + mSelectedView);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    return NOERROR;
}

//=================================================================================
// RecentsPanelView::OnDismissListener
//=================================================================================

CAR_INTERFACE_IMPL(RecentsPanelView::OnDismissListener, Object, IPopupMenuOnDismissListener)

RecentsPanelView::OnDismissListener::OnDismissListener(
    /* [in] */ RecentsPanelView* host,
    /* [in] */ IView* thumbnailView)
    : mHost(host)
    , mThumbnailView(thumbnailView)
{}

ECode RecentsPanelView::OnDismissListener::OnDismiss(
    /* [in] */ IPopupMenu* item)
{
    mThumbnailView->SetSelected(FALSE);
    mHost->mPopup = NULL;
    return NOERROR;
}

//=================================================================================
// RecentsPanelView::OnScrollListenerRunnable
//=================================================================================

RecentsPanelView::OnScrollListenerRunnable::OnScrollListenerRunnable(
    /* [in] */ RecentsPanelView* host)
    : mHost(host)
{}

ECode RecentsPanelView::OnScrollListenerRunnable::Run()
{
    // need to redraw the faded edges
    return mHost->Invalidate();
}

//=================================================================================
// RecentsPanelView
//=================================================================================

const String RecentsPanelView::TAG("RecentsPanelView");
const Boolean RecentsPanelView::DEBUG = FALSE;

CAR_INTERFACE_IMPL_4(RecentsPanelView, FrameLayout, IAdapterViewOnItemClickListener, \
    IStatusBarPanel, IAnimatorListener, IRecentsPanelView)

RecentsPanelView::RecentsPanelView()
    : mShowing(FALSE)
    , mWaitingToShow(FALSE)
    , mAnimateIconOfFirstTask(FALSE)
    , mWaitingForWindowAnimation(FALSE)
    , mWindowAnimationStartTime(0)
    , mThumbnailWidth(0)
    , mFitThumbnailToXY(FALSE)
    , mRecentItemLayoutId(0)
    , mHighEndGfx(FALSE)
{}

ECode RecentsPanelView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(ctx, attrs, 0);
}

ECode RecentsPanelView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(ctx, attrs, defStyle);
    UpdateValuesFromResources();

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::RecentsPanelView);
    AutoPtr<ITypedArray> a;
    ctx->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);

    a->GetResourceId(R::styleable::RecentsPanelView_recentItemLayout, 0, &mRecentItemLayoutId);
    mRecentTasksLoader = RecentTasksLoader::GetInstance(ctx);
    a->Recycle();
    return NOERROR;
}

ECode RecentsPanelView::NumItemsInOneScreenful(
    /* [out] */ Int32* count)
{
    return mRecentsContainer->NumItemsInOneScreenful(count);
}

Boolean RecentsPanelView::PointInside(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IView* v)
{
    Int32 l, r, t, b;
    v->GetLeft(&l);
    v->GetRight(&r);
    v->GetTop(&t);
    v->GetBottom(&b);
    return x >= l && x < r && y >= t && y < b;
}

ECode RecentsPanelView::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = PointInside(x, y, IView::Probe(mRecentsContainer));
    return NOERROR;
}

ECode RecentsPanelView::Show(
    /* [in] */ Boolean show)
{
    return Show(show, NULL, FALSE, FALSE);
}

ECode RecentsPanelView::Show(
    /* [in] */ Boolean show,
    /* [in] */ IArrayList* recentTaskDescriptions,
    /* [in] */ Boolean firstScreenful,
    /* [in] */ Boolean animateIconOfFirstTask)
{
    if (show && mCallUiHiddenBeforeNextReload) {
        OnUiHidden();
        recentTaskDescriptions = NULL;
        mAnimateIconOfFirstTask = FALSE;
        mWaitingForWindowAnimation = FALSE;
    }
    else {
        mAnimateIconOfFirstTask = animateIconOfFirstTask;
        mWaitingForWindowAnimation = animateIconOfFirstTask;
    }
    if (show) {
        mWaitingToShow = TRUE;
        RefreshRecentTasksList(recentTaskDescriptions, firstScreenful);
        ShowIfReady();
    }
    else {
        ShowImpl(FALSE);
    }
    return NOERROR;
}

void RecentsPanelView::ShowIfReady()
{
    // mWaitingToShow => there was a touch up on the recents button
    // (!mRecentTaskDescriptions.IsEmpty()) => we've created views for the first screenful of items
    if (mWaitingToShow && mRecentTaskDescriptions != NULL) {
        ShowImpl(TRUE);
    }
}

ECode RecentsPanelView::SendCloseSystemWindows(
    /* [in] */ IContext* context,
    /* [in] */ const String& reason)
{
    if (ActivityManagerNative::IsSystemReady()) {
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        ECode ec = am->CloseSystemDialogs(reason);
        if (FAILED(ec)) return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

void RecentsPanelView::ShowImpl(
    /* [in] */ Boolean show)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    SendCloseSystemWindows(context, IBaseStatusBar::SYSTEM_DIALOG_REASON_RECENT_APPS);

    mShowing = show;

    if (show) {
        // if there are no apps, bring up a "No recent apps" message
        Boolean noApps = TRUE;
        if (mRecentTaskDescriptions != NULL) {
            mRecentTaskDescriptions->IsEmpty(&noApps);
        }
        mRecentsNoApps->SetAlpha(1);
        mRecentsNoApps->SetVisibility(noApps ? IView::VISIBLE : IView::INVISIBLE);

        OnAnimationEnd(NULL);
        SetFocusable(TRUE);
        SetFocusableInTouchMode(TRUE);
        Boolean b;
        RequestFocus(&b);
    }
    else {
        mWaitingToShow = FALSE;
        // call onAnimationEnd() and clearRecentTasksList() in onUiHidden()
        mCallUiHiddenBeforeNextReload = TRUE;
        if (mPopup != NULL) {
            mPopup->Dismiss();
        }
    }
}

ECode RecentsPanelView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    AutoPtr<IViewRootImpl> root;
    GetViewRootImpl((IViewRootImpl**)&root);
    if (root != NULL) {
        root->SetDrawDuringWindowsAnimating(TRUE);
    }
    return NOERROR;
}

ECode RecentsPanelView::OnUiHidden()
{
    mCallUiHiddenBeforeNextReload = FALSE;
    if (!mShowing && mRecentTaskDescriptions != NULL) {
        OnAnimationEnd(NULL);
        ClearRecentTasksList();
    }
    mShowing = FALSE;
    return NOERROR;
}

ECode RecentsPanelView::Dismiss()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IRecentsActivity> ra = IRecentsActivity::Probe(context);
    ra->DismissAndGoHome();
    return NOERROR;
}

ECode RecentsPanelView::DismissAndGoBack()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IRecentsActivity> ra = IRecentsActivity::Probe(context);
    ra->DismissAndGoBack();
    return NOERROR;
}

ECode RecentsPanelView::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode RecentsPanelView::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mShowing) {
        AutoPtr<ILayoutTransition> transitioner;
        CLayoutTransition::New((ILayoutTransition**)&transitioner);
        IViewGroup::Probe(mRecentsContainer)->SetLayoutTransition(transitioner);
        CreateCustomAnimations(transitioner);
    }
    else {
        IViewGroup::Probe(mRecentsContainer)->SetLayoutTransition(NULL);
    }
    return NOERROR;
}

ECode RecentsPanelView::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode RecentsPanelView::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode RecentsPanelView::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // Ignore hover events outside of this panel bounds since such events
    // generate spurious accessibility events with the panel content when
    // tapping outside of it, thus confusing the user.
    Float x, y;
    event->GetX(&x);
    event->GetY(&y);
    Int32 w,h;
    GetWidth(&w);
    GetHeight(&h);
    if (x >= 0 && x < w && y >= 0 && y < h) {
        return DispatchHoverEvent(event, result);
    }
    *result = TRUE;
    return NOERROR;
}

ECode RecentsPanelView::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mShowing;
    return NOERROR;
}

ECode RecentsPanelView::SetRecentTasksLoader(
    /* [in] */ IRecentTasksLoader* loader)
{
    mRecentTasksLoader = loader;
    return NOERROR;
}

ECode RecentsPanelView::UpdateValuesFromResources()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Float dim;
    res->GetDimension(R::dimen::status_bar_recents_thumbnail_width, &dim);
    mThumbnailWidth = Elastos::Core::Math::Round(dim);
    res->GetBoolean(R::bool_::config_recents_thumbnail_image_fits_to_xy, &mFitThumbnailToXY);
    Logger::I(TAG, " UpdateValuesFromResources: mThumbnailWidth: (%.2f, %d), mFitThumbnailToXY:%d",
        dim, mThumbnailWidth, mFitThumbnailToXY);
    return NOERROR;
}

ECode RecentsPanelView::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    AutoPtr<IView> rsv;
    FindViewById(R::id::recents_container, (IView**)&rsv);
    mRecentsContainer = IRecentsScrollView::Probe(rsv);

    AutoPtr<OnScrollListenerRunnable> runnable = new OnScrollListenerRunnable(this);
    mRecentsContainer->SetOnScrollListener(runnable);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mListAdapter = new TaskDescriptionAdapter(context, this);
    mRecentsContainer->SetAdapter(mListAdapter);
    mRecentsContainer->SetCallback(listener);

    FindViewById(R::id::recents_bg_protect, (IView**)&mRecentsScrim);
    FindViewById(R::id::recents_no_apps, (IView**)&mRecentsNoApps);

    if (mRecentsScrim != NULL) {
        AutoPtr<IActivityManagerHelper> amh;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amh);
        amh->IsHighEndGfx(&mHighEndGfx);
        AutoPtr<IDrawable> bg;
        mRecentsScrim->GetBackground((IDrawable**)&bg);
        AutoPtr<IBitmapDrawable> bitmapDrawable = IBitmapDrawable::Probe(bg);
        if (!mHighEndGfx) {
            mRecentsScrim->SetBackground(NULL);
        }
        else if (bitmapDrawable != NULL) {
            // In order to save space, we make the background texture repeat in the Y direction
            bitmapDrawable->SetTileModeY(ShaderTileMode_REPEAT);
        }
    }
    return NOERROR;
}

ECode RecentsPanelView::SetMinSwipeAlpha(
    /* [in] */ Float minAlpha)
{
    return mRecentsContainer->SetMinSwipeAlpha(minAlpha);
}

void RecentsPanelView::CreateCustomAnimations(
    /* [in] */ ILayoutTransition* transitioner)
{
    transitioner->SetDuration(200);
    transitioner->SetStartDelay(ILayoutTransition::CHANGE_DISAPPEARING, 0);
    transitioner->SetAnimator(ILayoutTransition::DISAPPEARING, NULL);
}

void RecentsPanelView::UpdateIcon(
    /* [in] */ ViewHolder* h,
    /* [in] */ IDrawable* icon,
    /* [in] */ Boolean show,
    /* [in] */ Boolean anim)
{
    if (icon != NULL) {
        IView* iconView = IView::Probe(h->mIconView);
        h->mIconView->SetImageDrawable(icon);
        Int32 v;
        if (show && (iconView->GetVisibility(&v), v != IView::VISIBLE)) {
            if (anim) {
                AutoPtr<IContext> context;
                GetContext((IContext**)&context);
                AutoPtr<IAnimation> animation;
                AnimationUtils::LoadAnimation(context, R::anim::recent_appear, (IAnimation**)&animation);
                iconView->SetAnimation(animation);
            }
            iconView->SetVisibility(IView::VISIBLE);
        }
    }
}

void RecentsPanelView::UpdateThumbnail(
    /* [in] */ ViewHolder* h,
    /* [in] */ IDrawable* thumbnail,
    /* [in] */ Boolean show,
    /* [in] */ Boolean anim)
{
    if (thumbnail != NULL) {
        // Should remove the default image in the frame
        // that this now covers, to improve scrolling speed.
        // That can't be done until the anim is complete though.
        h->mThumbnailViewImage->SetImageDrawable(thumbnail);

        // scale the image to fill the full width of the ImageView. do this only if
        // we haven't set a bitmap before, or if the bitmap size has changed
        Int32 w1 = 0, w2 = 0, h1 = 0, h2 = 0;
        if (h->mThumbnailViewDrawable != NULL) {
            h->mThumbnailViewDrawable->GetIntrinsicWidth(&w1);
            h->mThumbnailViewDrawable->GetIntrinsicHeight(&h1);
        }
        thumbnail->GetIntrinsicWidth(&w2);
        thumbnail->GetIntrinsicHeight(&h2);
        if (h->mThumbnailViewDrawable == NULL || w1 != w2 || h1 != h2) {
            if (mFitThumbnailToXY) {
                h->mThumbnailViewImage->SetScaleType(ImageViewScaleType_FIT_XY);
            }
            else {
                AutoPtr<IMatrix> scaleMatrix;
                CMatrix::New((IMatrix**)&scaleMatrix);
                Float scale = mThumbnailWidth / (Float)w2;
                scaleMatrix->SetScale(scale, scale);
                h->mThumbnailViewImage->SetScaleType(ImageViewScaleType_MATRIX);
                h->mThumbnailViewImage->SetImageMatrix(scaleMatrix);
            }
        }
        Int32 visibility;
        h->mThumbnailView->GetVisibility(&visibility);
        if (show && visibility != IView::VISIBLE) {
            if (anim) {
                AutoPtr<IContext> context;
                GetContext((IContext**)&context);
                AutoPtr<IAnimation> animation;
                AnimationUtils::LoadAnimation(context, R::anim::recent_appear, (IAnimation**)&animation);
                h->mThumbnailView->SetAnimation(animation);
            }
            h->mThumbnailView->SetVisibility(IView::VISIBLE);
        }
        h->mThumbnailViewDrawable = thumbnail;
    }
}

ECode RecentsPanelView::OnTaskThumbnailLoaded(
    /* [in] */ ITaskDescription* td)
{
    AutoLock syncLock(td);
    if (mRecentsContainer != NULL) {
        AutoPtr<IViewGroup> container = IViewGroup::Probe(mRecentsContainer);
        AutoPtr<IRecentsScrollView> scrollView =
            IRecentsScrollView::Probe(container);
        if (scrollView != NULL) {
            AutoPtr<IView> v;
            IView::Probe(container)->FindViewById(R::id::recents_linear_layout, (IView**)&v);
            container = IViewGroup::Probe(v);
        }
        // Look for a view showing this thumbnail, to update.
        Int32 count;
        container->GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> v;
            container->GetChildAt(i, (IView**)&v);
            AutoPtr<IInterface> tag;
            v->GetTag((IInterface**)&tag);
            AutoPtr<ViewHolder> h;
            if (tag != NULL) {
                h = (ViewHolder*)(IObject::Probe(tag));
            }
            if (h != NULL) {
                if (!h->mLoadedThumbnailAndIcon && h->mTaskDescription.Get() == td) {
                    // only fade in the thumbnail if recents is already visible-- we
                    // show it immediately otherwise
                    //boolean animateShow = mShowing &&
                    //    mRecentsContainer.getAlpha() > ViewConfiguration.ALPHA_THRESHOLD;
                    Boolean animateShow = FALSE;
                    AutoPtr<IDrawable> icon;
                    td->GetIcon((IDrawable**)&icon);
                    UpdateIcon(h, icon, TRUE, animateShow);
                    AutoPtr<IDrawable> thumbnail;
                    td->GetThumbnail((IDrawable**)&thumbnail);
                    UpdateThumbnail(h, thumbnail, TRUE, animateShow);
                    h->mLoadedThumbnailAndIcon = TRUE;
                }
            }
        }
    }

    ShowIfReady();
    return NOERROR;
}

void RecentsPanelView::AnimateInIconOfFirstTask()
{
    using Elastos::Core::Math;

    Boolean b;
    if (mItemToAnimateInWhenWindowAnimationIsFinished != NULL && (
        mRecentTasksLoader->IsFirstScreenful(&b), !b)) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
        Int32 timeSinceWindowAnimation = (Int32)(now - mWindowAnimationStartTime);
        Int32 minStartDelay = 150;
        Int32 startDelay = Math::Max(0, Math::Min(
            minStartDelay - timeSinceWindowAnimation, minStartDelay));
        Int32 duration = 250;
        AutoPtr<ViewHolder> holder = mItemToAnimateInWhenWindowAnimationIsFinished;
        AutoPtr<IDecelerateInterpolator> cubic;
        CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&cubic);
        FirstFrameAnimatorHelper::InitializeDrawListener(IView::Probe(holder->mIconView));

        AutoPtr<ArrayOf<IView*> > views = ArrayOf<IView*>::Alloc(3);
        views->Set(0, IView::Probe(holder->mIconView));
        views->Set(1, IView::Probe(holder->mLabelView));
        views->Set(2, holder->mCalloutLine);
        for (Int32 i = 0; i < views->GetLength(); i++) {
            AutoPtr<IView> v = (*views)[i];
            if (v != NULL) {
                AutoPtr<IViewPropertyAnimator> vpa;
                v->Animate((IViewPropertyAnimator**)&vpa);
                vpa->TranslationX(0);
                vpa->TranslationY(0);
                vpa->Alpha(1);
                vpa->SetStartDelay(startDelay);
                vpa->SetDuration(duration);
                vpa->SetInterpolator(ITimeInterpolator::Probe(cubic));
                AutoPtr<FirstFrameAnimatorHelper> h = new FirstFrameAnimatorHelper(vpa, v);
            }
        }
        mItemToAnimateInWhenWindowAnimationIsFinished = NULL;
        mAnimateIconOfFirstTask = FALSE;
    }
}

ECode RecentsPanelView::OnWindowAnimationStart()
{
    mWaitingForWindowAnimation = FALSE;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mWindowAnimationStartTime);
    AnimateInIconOfFirstTask();
    return NOERROR;
}

ECode RecentsPanelView::ClearRecentTasksList()
{
    // Clear memory used by screenshots
    if (mRecentTaskDescriptions != NULL) {
        mRecentTasksLoader->CancelLoadingThumbnailsAndIcons(this);
        OnTaskLoadingCancelled();
    }
    return NOERROR;
}

ECode RecentsPanelView::OnTaskLoadingCancelled()
{
    // Gets called by RecentTasksLoader when it's cancelled
    if (mRecentTaskDescriptions != NULL) {
        mRecentTaskDescriptions = NULL;
        mListAdapter->NotifyDataSetInvalidated();
    }
    return NOERROR;
}

ECode RecentsPanelView::RefreshViews()
{
    mListAdapter->NotifyDataSetInvalidated();
    UpdateUiElements();
    ShowIfReady();
    return NOERROR;
}

ECode RecentsPanelView::RefreshRecentTasksList()
{
    RefreshRecentTasksList(NULL, FALSE);
    return NOERROR;
}

void RecentsPanelView::RefreshRecentTasksList(
    /* [in] */ IArrayList* recentTasksList,
    /* [in] */ Boolean firstScreenful)
{
    if (mRecentTaskDescriptions == NULL && recentTasksList != NULL) {
        OnTasksLoaded(recentTasksList, firstScreenful);
    }
    else {
        mRecentTasksLoader->LoadTasksInBackground();
    }
}

ECode RecentsPanelView::OnTasksLoaded(
    /* [in] */ IArrayList* tasks,
    /* [in] */ Boolean firstScreenful)
{
    if (mRecentTaskDescriptions == NULL) {
        CArrayList::New(ICollection::Probe(tasks), (IList**)&mRecentTaskDescriptions);
    }
    else {
        Boolean modified;
        mRecentTaskDescriptions->AddAll(ICollection::Probe(tasks), &modified);
    }
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    Boolean result;
    AutoPtr<IRecentsActivity> ra = IRecentsActivity::Probe(ctx);
    ra->IsActivityShowing(&result);
    if (result) {
        RefreshViews();
    }
    return NOERROR;
}

void RecentsPanelView::UpdateUiElements()
{
    Int32 numRecentApps = 0;
    if (mRecentTaskDescriptions != NULL) {
        mRecentTaskDescriptions->GetSize(&numRecentApps);
    }
    IView::Probe(mRecentsContainer)->SetVisibility(numRecentApps > 0 ? IView::VISIBLE : IView::GONE);

    // Set description for accessibility
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    String recentAppsAccessibilityDescription;
    if (numRecentApps == 0) {
        res->GetString(R::string::status_bar_no_recent_apps, &recentAppsAccessibilityDescription);
    }
    else {
        AutoPtr<IInteger32> i = CoreUtils::Convert(numRecentApps);
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        array->Set(0, i);
        res->GetQuantityString(R::plurals::status_bar_accessibility_recent_apps,
            numRecentApps, array, &recentAppsAccessibilityDescription);
    }

    AutoPtr<ICharSequence> cs = CoreUtils::Convert(recentAppsAccessibilityDescription);
    SetContentDescription(cs);
}

ECode RecentsPanelView::SimulateClick(
    /* [in] */ Int32 persistentTaskId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IView> v;
    mRecentsContainer->FindViewForTask(persistentTaskId, (IView**)&v);
    if (v != NULL) {
        HandleOnClick(v);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode RecentsPanelView::HandleOnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> tag;
    view->GetTag((IInterface**)&tag);
    AutoPtr<ViewHolder> holder = (ViewHolder*)(IObject::Probe(tag));
    AutoPtr<ITaskDescription> ad = holder->mTaskDescription;
    AutoPtr<TaskDescription> _ad = (TaskDescription*)ad.Get();
    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);
    AutoPtr<IInterface> amObj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&amObj);
    const AutoPtr<IActivityManager> am = IActivityManager::Probe(amObj);
    AutoPtr<IBitmap> bm;
    Boolean usingDrawingCache = TRUE;

    IView* thumbnailView = IView::Probe(holder->mThumbnailViewImage);
    AutoPtr<IBitmapDrawable> bd = IBitmapDrawable::Probe(holder->mThumbnailViewDrawable);
    if (bd != NULL) {
        bd->GetBitmap((IBitmap**)&bm);
        Int32 W1, W2;
        bm->GetWidth(&W1);
        thumbnailView->GetWidth(&W2);
        Int32 H1, H2;
        bm->GetHeight(&H1);
        thumbnailView->GetHeight(&H2);
        if (W1 == W2 && H1 == H2) {
            usingDrawingCache = FALSE;
        }
    }
    if (usingDrawingCache) {
        thumbnailView->SetDrawingCacheEnabled(TRUE);
        thumbnailView->GetDrawingCache((IBitmap**)&bm);
    }
    AutoPtr<IActivityOptionsHelper> helper;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
    AutoPtr<IActivityOptions> activtiyOptions;
    helper->MakeThumbnailScaleUpAnimation(
        thumbnailView, bm, 0, 0, NULL,
        (IActivityOptions**)&activtiyOptions);
    AutoPtr<IBundle> _opts;
    activtiyOptions->ToBundle((IBundle**)&_opts);

    AutoPtr<IBundle> opts = (bm == NULL) ? NULL : _opts;

    Show(FALSE);

    Int32 taskId = _ad->mTaskId;
    if (taskId >= 0) {
        // This is an active task; it should just go to the foreground.
        am->MoveTaskToFront(taskId, IActivityManager::MOVE_TASK_WITH_HOME, opts);
    }
    else {
        AutoPtr<IIntent> intent = _ad->mIntent;
        intent->AddFlags(IIntent::FLAG_ACTIVITY_LAUNCHED_FROM_HISTORY
                | IIntent::FLAG_ACTIVITY_TASK_ON_HOME
                | IIntent::FLAG_ACTIVITY_NEW_TASK);
        if (DEBUG) {
            Logger::V(TAG, "Starting activity %s", TO_CSTR(intent));
        }
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(_ad->mUserId, (IUserHandle**)&userHandle);
        ECode ec = context->StartActivityAsUser(intent, opts, userHandle);
        if (FAILED(ec)) {
            if (ec == (ECode)E_SECURITY_EXCEPTION) {
                Logger::E(TAG, "Recents does not have the permission to launch %s", TO_CSTR(intent));
                return E_SECURITY_EXCEPTION;
            }
            if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
                Logger::E(TAG, "Error launching activity %s", TO_CSTR(intent));
                return E_ACTIVITY_NOT_FOUND_EXCEPTION;
            }
        }
    }
    if (usingDrawingCache) {
        thumbnailView->SetDrawingCacheEnabled(FALSE);
    }
    return NOERROR;
}

ECode RecentsPanelView::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return HandleOnClick(view);
}

ECode RecentsPanelView::HandleSwipe(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> tag;
    view->GetTag((IInterface**)&tag);
    AutoPtr<ViewHolder> holder = (ViewHolder*)(IObject::Probe(tag));
    AutoPtr<ITaskDescription> ad = holder->mTaskDescription;
    if (ad == NULL) {
        Logger::V(TAG, "Not able to find activity description for swiped task; view=%s tag=%s",
            TO_CSTR(view), TO_CSTR(tag));
        return NOERROR;
    }
    AutoPtr<ICharSequence> label;
    ad->GetLabel((ICharSequence**)&label);
    if (DEBUG) Logger::V(TAG, "Jettison %s", TO_CSTR(label));
    mRecentTaskDescriptions->Remove(ad);
    mRecentTasksLoader->Remove(ad);

    // Handled by widget containers to enable LayoutTransitions properly
    // mListAdapter.notifyDataSetChanged();

    Boolean isEmpty;
    mRecentTaskDescriptions->IsEmpty(&isEmpty);
    if (isEmpty) {
        DismissAndGoBack();
    }

    // Currently, either direction means the same thing, so ignore direction and remove
    // the task.
    AutoPtr<IInterface> amObj;
    mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&amObj);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(amObj);
    if (am != NULL) {
        AutoPtr<TaskDescription> td = (TaskDescription*)ad.Get();
        Boolean b;
        am->RemoveTask(td->mPersistentTaskId, IActivityManager::REMOVE_TASK_KILL_PROCESS, &b);

        // Accessibility feedback
        AutoPtr<ICharSequence> cs;
        ad->GetLabel((ICharSequence**)&cs);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, cs.Get());
        String str;
        mContext->GetString(R::string::accessibility_recents_item_dismissed, args, &str);
        cs = CoreUtils::Convert(str);
        SetContentDescription(cs);
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
        SetContentDescription(NULL);
    }
    return NOERROR;
}

void RecentsPanelView::StartApplicationDetailsActivity(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->FromParts(String("package"), packageName, String(NULL), (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_APPLICATION_DETAILS_SETTINGS, uri, (IIntent**)&intent);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IPackageManager> pm;
    ctx->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IComponentName> cn;
    intent->ResolveActivity(pm, (IComponentName**)&cn);
    intent->SetComponent(cn);
    AutoPtr<ITaskStackBuilderHelper> helper;
    CTaskStackBuilderHelper::AcquireSingleton((ITaskStackBuilderHelper**)&helper);
    AutoPtr<ITaskStackBuilder> builder;
    helper->Create(ctx, (ITaskStackBuilder**)&builder);
    builder->AddNextIntentWithParentStack(intent);
    AutoPtr<IUserHandle> uh;
    CUserHandle::New(userId, (IUserHandle**)&uh);
    builder->StartActivities(NULL, uh);
}

ECode RecentsPanelView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mPopup != NULL) {
        *result = TRUE;
        return NOERROR;
    }
    else {
        return FrameLayout::OnInterceptTouchEvent(ev, result);
    }
}

ECode RecentsPanelView::HandleLongPress(
    /* [in] */ IView* selectedView,
    /* [in] */ IView* anchorView,
    /* [in] */ IView* thumbnailView)
{
    thumbnailView->SetSelected(TRUE);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IPopupMenu> popup;
    CPopupMenu::New(ctx, anchorView == NULL ? selectedView : anchorView, (IPopupMenu**)&popup);
    mPopup = popup;
    AutoPtr<IMenuInflater> flater;
    popup->GetMenuInflater((IMenuInflater**)&flater);
    AutoPtr<IMenu> menu;
    popup->GetMenu((IMenu**)&menu);
    flater->Inflate(R::menu::recent_popup_menu, menu);
    AutoPtr<OnMenuItemClickListener> menuItemClickListener = new OnMenuItemClickListener(this, selectedView);
    popup->SetOnMenuItemClickListener(menuItemClickListener);
    AutoPtr<OnDismissListener> dismissListener = new OnDismissListener(this, thumbnailView);
    popup->SetOnDismissListener(dismissListener);
    popup->Show();
    return NOERROR;
}

ECode RecentsPanelView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    FrameLayout::DispatchDraw(canvas);

    Int32 paddingLeft;
    GetPaddingLeft(&paddingLeft);
    Boolean offsetRequired = IsPaddingOffsetRequired();
    if (offsetRequired) {
        Int32 lpo =GetLeftPaddingOffset();
        paddingLeft += lpo;
    }

    Int32 left, left2, right, paddingRight;
    GetScrollX(&left);
    left += paddingLeft;
    GetRight(&right);
    GetLeft(&left2);
    GetPaddingRight(&paddingRight);
    right = left + right - left2 - paddingRight - paddingLeft;
    Int32 scrollY;
    GetScrollY(&scrollY);
    Int32 fadeTop = GetFadeTop(offsetRequired);
    Int32 fadeHeight = GetFadeHeight(offsetRequired);
    Int32 top = scrollY + fadeTop;
    Int32 bottom = top + fadeHeight;

    if (offsetRequired) {
        Int32 rpo, bpo;
        rpo = GetRightPaddingOffset();
        right += rpo;
        bpo = GetBottomPaddingOffset();
        bottom += bpo;
    }
    mRecentsContainer->DrawFadedEdges(canvas, left, right, top, bottom);
    return NOERROR;
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
