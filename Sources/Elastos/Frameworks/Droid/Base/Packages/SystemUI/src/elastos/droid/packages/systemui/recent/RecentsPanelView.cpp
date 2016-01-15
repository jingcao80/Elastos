
#include "elastos/droid/systemui/recent/RecentsPanelView.h"
#include "elastos/droid/systemui/recent/CRecentTasksLoader.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/app/CTaskStackBuilderHelper.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityOptionsHelper.h"
#include "elastos/droid/animation/CLayoutTransition.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/widget/CPopupMenu.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Droid::R;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;
using Elastos::Droid::SystemUI::Recent::CRecentTasksLoader;
using Elastos::Droid::SystemUI::Recent::IRecentsActivity;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::ITaskStackBuilder;
using Elastos::Droid::App::ITaskStackBuilderHelper;
using Elastos::Droid::App::CTaskStackBuilderHelper;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::Widget::EIID_IBaseAdapter;
using Elastos::Droid::Widget::EIID_IListAdapter;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Droid::Widget::IPopupMenu;
using Elastos::Droid::Widget::CPopupMenu;
using Elastos::Droid::Widget::EIID_IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Widget::EIID_IPopupMenuOnDismissListener;
using Elastos::Droid::Widget::ImageViewScaleType_FIT_XY;
using Elastos::Droid::Widget::ImageViewScaleType_MATRIX;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Graphics::ShaderTileMode;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IBitmapDrawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

//=================================================================================
// RecentsPanelView::OnLongClickDelegate
//=================================================================================
CAR_INTERFACE_IMPL(RecentsPanelView::OnLongClickDelegate, IViewOnLongClickListener)

ECode RecentsPanelView::OnLongClickDelegate::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    return mOtherView->PerformLongClick(result);
}


//=================================================================================
// RecentsPanelView::ViewHolder
//=================================================================================
CAR_INTERFACE_IMPL_LIGHT(RecentsPanelView::ViewHolder, IInterface)

//=================================================================================
// RecentsPanelView::TaskDescriptionAdapter
//=================================================================================
RecentsPanelView::TaskDescriptionAdapter::TaskDescriptionAdapter(
    /* [in] */ IContext* context,
    /* [in] */ RecentsPanelView* host)
    : mHost(host)
{
    LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
}

RecentsPanelView::TaskDescriptionAdapter::~TaskDescriptionAdapter()
{
    mInflater = NULL;
}

PInterface RecentsPanelView::TaskDescriptionAdapter::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    else if (riid == EIID_ITaskDescriptionAdapter) {
        return (ITaskDescriptionAdapter*)this;
    }
    else if (riid == EIID_IBaseAdapter) {
        return (IBaseAdapter*)this;
    }
    else if (riid == EIID_IListAdapter) {
        return (IListAdapter*)this;
    }
    else if (riid == EIID_IAdapter) {
        return (IAdapter*)this;
    }
    return NULL;
}

UInt32 RecentsPanelView::TaskDescriptionAdapter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 RecentsPanelView::TaskDescriptionAdapter::Release()
{
    return ElRefBase::Release();
}

ECode RecentsPanelView::TaskDescriptionAdapter::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)this) {
        *iid = EIID_ITaskDescriptionAdapter ;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Int32 RecentsPanelView::TaskDescriptionAdapter::GetCount()
{
    return mHost->mRecentTaskDescriptions != NULL ? mHost->mRecentTaskDescriptions->GetSize() : 0;
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
    convertView->FindViewById(SystemUIR::id::app_thumbnail, (IView**)&v);
    holder->mThumbnailView = v;
    v = NULL;
    convertView->FindViewById(SystemUIR::id::app_thumbnail_image, (IView**)&v);
    holder->mThumbnailViewImage = IImageView::Probe(v);
    // If we set the default thumbnail now, we avoid an onLayout when we update
    // the thumbnail later (if they both have the same dimensions)
    AutoPtr<IBitmap> bmp;
    mHost->mRecentTasksLoader->GetDefaultThumbnail((IBitmap**)&bmp);
    mHost->UpdateThumbnail(holder, bmp, FALSE, FALSE);
    v = NULL;
    convertView->FindViewById(SystemUIR::id::app_icon, (IView**)&v);
    holder->mIconView = IImageView::Probe(v);
    AutoPtr<IBitmap> icon;
    mHost->mRecentTasksLoader->GetDefaultIcon((IBitmap**)&icon);
    holder->mIconView->SetImageBitmap(icon);
    v = NULL;
    convertView->FindViewById(SystemUIR::id::app_label, (IView**)&v);
    holder->mLabelView = ITextView::Probe(v);
    v = NULL;
    convertView->FindViewById(SystemUIR::id::recents_callout_line, (IView**)&v);
    holder->mCalloutLine = v;
    v = NULL;
    convertView->FindViewById(SystemUIR::id::app_description, (IView**)&v);
    holder->mDescriptionView = ITextView::Probe(v);

    convertView->SetTag(holder);
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
    AutoPtr<RecentsPanelView::ViewHolder> holder = (RecentsPanelView::ViewHolder*)tag.Get();

    // index is reverse since most recent appears at the bottom...
    Int32 index = mHost->mRecentTaskDescriptions->GetSize() - position - 1;

    AutoPtr<ITaskDescription> td = (*mHost->mRecentTaskDescriptions)[index];

    AutoPtr<ICharSequence> cs;
    td->GetLabel((ICharSequence**)&cs);
    holder->mLabelView->SetText(cs);
    holder->mThumbnailView->SetContentDescription(cs);
    td->IsLoaded(&holder->mLoadedThumbnailAndIcon);
    if (holder->mLoadedThumbnailAndIcon) {
        AutoPtr<IBitmap> tn;
        td->GetThumbnail((IBitmap**)&tn);
        AutoPtr<IDrawable> dr;
        td->GetIcon((IDrawable**)&dr);
        mHost->UpdateThumbnail(holder, tn, TRUE, FALSE);
        mHost->UpdateIcon(holder, dr, TRUE, FALSE);
    }
    if (index == 0) {
        if (mHost->mAnimateIconOfFirstTask) {
            if (mHost->mItemToAnimateInWhenWindowAnimationIsFinished != NULL) {
                holder->mIconView->SetAlpha(1);
                holder->mIconView->SetTranslationX(0);
                holder->mIconView->SetTranslationY(0);
                holder->mLabelView->SetAlpha(1);
                holder->mLabelView->SetTranslationX(0);
                holder->mLabelView->SetTranslationY(0);
                if (holder->mCalloutLine != NULL) {
                    holder->mCalloutLine->SetAlpha(1);
                    holder->mCalloutLine->SetTranslationX(0);
                    holder->mCalloutLine->SetTranslationY(0);
                }
            }
            mHost->mItemToAnimateInWhenWindowAnimationIsFinished = holder;
            Int32 translation;
            mHost->GetResources()->GetDimensionPixelSize(
                    SystemUIR::dimen::status_bar_recents_app_icon_translate_distance, &translation);
            translation = -translation;
            AutoPtr<IConfiguration> config;
            mHost->GetResources()->GetConfiguration((IConfiguration**)&config);
            Int32 orientation;
            config->GetOrientation(&orientation);
            if (orientation == IConfiguration::ORIENTATION_PORTRAIT) {
                holder->mIconView->SetAlpha(0);
                holder->mIconView->SetTranslationX(translation);
                holder->mLabelView->SetAlpha(0);
                holder->mLabelView->SetTranslationX(translation);
                holder->mCalloutLine->SetAlpha(0);
                holder->mCalloutLine->SetTranslationX(translation);
            }
            else {
                holder->mIconView->SetAlpha(0);
                holder->mIconView->SetTranslationY(translation);
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
    AutoPtr<RecentsPanelView::ViewHolder> holder = (RecentsPanelView::ViewHolder*)tag.Get();
    AutoPtr<IBitmap> bmp;
    mHost->mRecentTasksLoader->GetDefaultThumbnail((IBitmap**)&bmp);
    mHost->UpdateThumbnail(holder, bmp, FALSE, FALSE);
    AutoPtr<IBitmap> icon;
    mHost->mRecentTasksLoader->GetDefaultIcon((IBitmap**)&icon);
    holder->mIconView->SetImageBitmap(icon);
    holder->mIconView->SetVisibility(IView::INVISIBLE);
    AutoPtr<IViewPropertyAnimator> animator;
    holder->mIconView->Animate((IViewPropertyAnimator**)&animator);
    animator->Cancel();
    holder->mLabelView->SetText(NULL);
    animator = NULL;
    holder->mLabelView->Animate((IViewPropertyAnimator**)&animator);
    animator->Cancel();
    holder->mThumbnailView->SetContentDescription(NULL);
    holder->mThumbnailView->SetTag(NULL);
    holder->mThumbnailView->SetOnLongClickListener(NULL);
    holder->mThumbnailView->SetVisibility(IView::INVISIBLE);
    holder->mIconView->SetAlpha(1.f);
    holder->mIconView->SetTranslationX(0);
    holder->mIconView->SetTranslationY(0);
    holder->mLabelView->SetAlpha(1);
    holder->mLabelView->SetTranslationX(0);
    holder->mLabelView->SetTranslationY(0);
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

ECode RecentsPanelView::TaskDescriptionAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return BaseAdapter::RegisterDataSetObserver(observer);
}

ECode RecentsPanelView::TaskDescriptionAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return BaseAdapter::UnregisterDataSetObserver(observer);
}

ECode RecentsPanelView::TaskDescriptionAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);
    *hasStableIds = BaseAdapter::HasStableIds();
    return NOERROR;
}

ECode RecentsPanelView::TaskDescriptionAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    VALIDATE_NOT_NULL(viewType);
    *viewType = BaseAdapter::GetItemViewType(position);
    return NOERROR;
}

ECode RecentsPanelView::TaskDescriptionAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = BaseAdapter::GetViewTypeCount();
    return NOERROR;
}

ECode RecentsPanelView::TaskDescriptionAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    *isEmpty = BaseAdapter::IsEmpty();
    return NOERROR;
}

ECode RecentsPanelView::TaskDescriptionAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = BaseAdapter::AreAllItemsEnabled();
    return NOERROR;
}

ECode RecentsPanelView::TaskDescriptionAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = BaseAdapter::IsEnabled(position);
    return NOERROR;
}

ECode RecentsPanelView::TaskDescriptionAdapter::NotifyDataSetChanged()
{
    return BaseAdapter::NotifyDataSetChanged();
}

ECode RecentsPanelView::TaskDescriptionAdapter::NotifyDataSetInvalidated()
{
    return BaseAdapter::NotifyDataSetInvalidated();
}


//=================================================================================
// RecentsPanelView::OnMenuItemClickListener
//=================================================================================
CAR_INTERFACE_IMPL(RecentsPanelView::OnMenuItemClickListener, IPopupMenuOnMenuItemClickListener)

ECode RecentsPanelView::OnMenuItemClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    Int32 id;
    item->GetItemId(&id);
    if (id == SystemUIR::id::recent_remove_item) {
        mHost->mRecentsContainer->RemoveViewInLayout(mSelectedView);
    }
    else if (id == SystemUIR::id::recent_inspect_item) {
        AutoPtr<IInterface> tag;
        mSelectedView->GetTag((IInterface**)&tag);
        AutoPtr<ViewHolder> viewHolder;
        if (tag != NULL) {
            viewHolder = (RecentsPanelView::ViewHolder*)tag.Get();
        }
        if (viewHolder != NULL) {
            AutoPtr<ITaskDescription> ad = viewHolder->mTaskDescription;
            String name;
            ad->GetPackageName(&name);
            mHost->StartApplicationDetailsActivity(name);
            mHost->Show(FALSE);
        }
        else {
            // throw new IllegalStateException("Oops, no tag on view " + mSelectedView);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}


//=================================================================================
// RecentsPanelView::OnDismissListener
//=================================================================================
CAR_INTERFACE_IMPL(RecentsPanelView::OnDismissListener, IPopupMenuOnDismissListener)

ECode RecentsPanelView::OnDismissListener::OnDismiss(
    /* [in] */ IPopupMenu* item)
{
    mThumbnailView->SetSelected(FALSE);
    mHost->mPopup = NULL;
    return NOERROR;
}


//=================================================================================
// RecentsPanelView
//=================================================================================
const String RecentsPanelView::TAG("RecentsPanelView");
const Boolean RecentsPanelView::DEBUG = FALSE; //TabletStatusBar.DEBUG || PhoneStatusBar.DEBUG || false

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

RecentsPanelView::~RecentsPanelView()
{
    mListAdapter = NULL;
    mRecentTaskDescriptions = NULL;
}

ECode RecentsPanelView::Init(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(ctx, attrs, 0);
}

ECode RecentsPanelView::Init(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FAIL_RETURN(FrameLayout::Init(ctx, attrs, defStyle));
    UpdateValuesFromResources();

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(SystemUIR::styleable::RecentsPanelView),
        ArraySize(SystemUIR::styleable::RecentsPanelView));
    AutoPtr<ITypedArray> a;
    ctx->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);

    a->GetResourceId(SystemUIR::styleable::RecentsPanelView_recentItemLayout, 0, &mRecentItemLayoutId);
    mRecentTasksLoader = CRecentTasksLoader::GetInstance(ctx);
    a->Recycle();
    return NOERROR;
}

ECode RecentsPanelView::NumItemsInOneScreenful(
    /* [out] */ Int32* count)
{
    AutoPtr<IRecentsScrollView> scrollView = IRecentsScrollView::Probe(mRecentsContainer);
    if (scrollView != NULL) {
        return scrollView->NumItemsInOneScreenful(count);
    }
    else {
        // throw new IllegalArgumentException("missing Recents[Horizontal]ScrollView");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
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

Boolean RecentsPanelView::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (PointInside(x, y, mRecentsContainer)) {
        return TRUE;
    }
    else if (mStatusBarTouchProxy != NULL &&
            PointInside(x, y, mStatusBarTouchProxy)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

ECode RecentsPanelView::Show(
    /* [in] */ Boolean show)
{
    return Show(show, NULL, FALSE, FALSE);
}

ECode RecentsPanelView::Show(
    /* [in] */ Boolean show,
    /* [in] */ IObjectContainer* recentTaskDescriptions,
    /* [in] */ Boolean firstScreenful,
    /* [in] */ Boolean animateIconOfFirstTask)
{
    mAnimateIconOfFirstTask = animateIconOfFirstTask;
    mWaitingForWindowAnimation = animateIconOfFirstTask;
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
        // try {
        ActivityManagerNative::GetDefault()->CloseSystemDialogs(reason);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

void RecentsPanelView::ShowImpl(
    /* [in] */ Boolean show)
{
    SendCloseSystemWindows(mContext, IBaseStatusBar::SYSTEM_DIALOG_REASON_RECENT_APPS);

    mShowing = show;

    if (show) {
        // if there are no apps, bring up a "No recent apps" message
        Boolean noApps = mRecentTaskDescriptions != NULL && mRecentTaskDescriptions->IsEmpty();
        mRecentsNoApps->SetAlpha(1);
        mRecentsNoApps->SetVisibility(noApps ? IView::VISIBLE : IView::INVISIBLE);

        OnAnimationEnd(NULL);
        SetFocusable(TRUE);
        SetFocusableInTouchMode(TRUE);
        RequestFocus();
    }
    else {
        mWaitingToShow = FALSE;
        // call onAnimationEnd() and clearRecentTasksList() in onUiHidden()
        if (mPopup != NULL) {
            mPopup->Dismiss();
        }
    }
}

ECode RecentsPanelView::OnUiHidden()
{
    if (!mShowing && mRecentTaskDescriptions != NULL) {
        OnAnimationEnd(NULL);
        ClearRecentTasksList();
    }
    mShowing = FALSE;
    return NOERROR;
}

ECode RecentsPanelView::Dismiss()
{
    IRecentsActivity* ra = IRecentsActivity::Probe(mContext);
    ra->DismissAndGoHome();
    return NOERROR;
}

ECode RecentsPanelView::DismissAndGoBack()
{
    IRecentsActivity* ra = IRecentsActivity::Probe(mContext);
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

Boolean RecentsPanelView::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    // Ignore hover events outside of this panel bounds since such events
    // generate spurious accessibility events with the panel content when
    // tapping outside of it, thus confusing the user.
    Float x, y;
    event->GetX(&x);
    event->GetY(&y);
    if (x >= 0 && x < GetWidth() && y >= 0 && y < GetHeight()) {
        return FrameLayout::DispatchHoverEvent(event);
    }
    return TRUE;
}

Boolean RecentsPanelView::IsShowing()
{
    return mShowing;
}

ECode RecentsPanelView::SetStatusBarView(
    /* [in] */ IView* statusBarView)
{
    if (mStatusBarTouchProxy != NULL) {
        mStatusBarTouchProxy->SetStatusBar(statusBarView);
    }
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
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Float dim;
    res->GetDimension(SystemUIR::dimen::status_bar_recents_thumbnail_width, &dim);
    mThumbnailWidth = Elastos::Core::Math::Round(dim);
    res->GetBoolean(SystemUIR::bool_::config_recents_thumbnail_image_fits_to_xy, &mFitThumbnailToXY);
    return NOERROR;
}

ECode RecentsPanelView::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();

    AutoPtr<IView> v = FindViewById(SystemUIR::id::recents_container);
    mRecentsContainer = IViewGroup::Probe(v);
    v = FindViewById(SystemUIR::id::status_bar_touch_proxy);
    mStatusBarTouchProxy = IStatusBarTouchProxy::Probe(v);
    mListAdapter = new TaskDescriptionAdapter(mContext, this);
    AutoPtr<IRecentsScrollView> scrollView = IRecentsScrollView::Probe(mRecentsContainer);
    if (scrollView != NULL) {
        scrollView->SetAdapter(mListAdapter);
        scrollView->SetCallback(THIS_PROBE(IRecentsCallback));
    }
    else {
        // throw new IllegalArgumentException("missing Recents[Horizontal]ScrollView");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mRecentsScrim = FindViewById(SystemUIR::id::recents_bg_protect);
    mRecentsNoApps = FindViewById(SystemUIR::id::recents_no_apps);

    if (mRecentsScrim != NULL) {
        mHighEndGfx = CActivityManager::IsHighEndGfx();
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
    AutoPtr<IRecentsScrollView> scrollView = IRecentsScrollView::Probe(mRecentsContainer);
    if (scrollView != NULL) {
        scrollView->SetMinSwipeAlpha(minAlpha);
    }
    return NOERROR;
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
        h->mIconView->SetImageDrawable(icon);
        Int32 v;
        h->mIconView->GetVisibility(&v);
        if (show && v != IView::VISIBLE) {
            if (anim) {
                AutoPtr<IAnimation> animation;
                AnimationUtils::LoadAnimation(mContext, SystemUIR::anim::recent_appear, (IAnimation**)&animation);
                h->mIconView->SetAnimation(animation);
            }
            h->mIconView->SetVisibility(IView::VISIBLE);
        }
    }
}

void RecentsPanelView::UpdateThumbnail(
    /* [in] */ ViewHolder* h,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Boolean show,
    /* [in] */ Boolean anim)
{
    if (thumbnail != NULL) {
        // Should remove the default image in the frame
        // that this now covers, to improve scrolling speed.
        // That can't be done until the anim is complete though.
        h->mThumbnailViewImage->SetImageBitmap(thumbnail);

        // scale the image to fill the full width of the ImageView. do this only if
        // we haven't set a bitmap before, or if the bitmap size has changed
        Int32 w1 = 0, w2 = 0, h1 = 0, h2 = 0;
        if(h->mThumbnailViewImageBitmap != NULL) {
            h->mThumbnailViewImageBitmap->GetWidth(&w1);
            h->mThumbnailViewImageBitmap->GetHeight(&h1);
        }
        thumbnail->GetWidth(&w2);
        thumbnail->GetHeight(&h2);
        if (h->mThumbnailViewImageBitmap == NULL ||
            w1 != w2 || h1 != h2) {
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
                AutoPtr<IAnimation> animation;
                AnimationUtils::LoadAnimation(mContext, SystemUIR::anim::recent_appear, (IAnimation**)&animation);
                h->mThumbnailView->SetAnimation(animation);
            }
            h->mThumbnailView->SetVisibility(IView::VISIBLE);
        }
        h->mThumbnailViewImageBitmap = thumbnail;
    }
}

ECode RecentsPanelView::OnTaskThumbnailLoaded(
    /* [in] */ ITaskDescription* td)
{
    {
        td->Lock();
        if (mRecentsContainer != NULL) {
            AutoPtr<IViewGroup> container = mRecentsContainer;
            AutoPtr<IRecentsScrollView> scrollView = IRecentsScrollView::Probe(container);
            if (scrollView != NULL) {
                AutoPtr<IView> v;
                container->FindViewById(SystemUIR::id::recents_linear_layout, (IView**)&v);
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
                    h = (RecentsPanelView::ViewHolder*)tag.Get();
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
                        AutoPtr<IBitmap> thumbnail;
                        td->GetThumbnail((IBitmap**)&thumbnail);
                        UpdateThumbnail(h, thumbnail, TRUE, animateShow);
                        h->mLoadedThumbnailAndIcon = TRUE;
                    }
                }
            }
        }
        td->Unlock();
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
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
        Int32 timeSinceWindowAnimation = (Int32)(now - mWindowAnimationStartTime);
        Int32 minStartDelay = 150;
        Int32 startDelay = Math::Max(0, Math::Min(minStartDelay - timeSinceWindowAnimation, minStartDelay));
        Int32 duration = 250;
        AutoPtr<ViewHolder> holder = mItemToAnimateInWhenWindowAnimationIsFinished;
        AutoPtr<IDecelerateInterpolator> cubic;
        CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&cubic);
        AutoPtr<ArrayOf<IView*> > views = ArrayOf<IView*>::Alloc(3);
        views->Set(0, holder->mIconView);
        views->Set(1, holder->mLabelView);
        views->Set(2, holder->mCalloutLine);
        for (Int32 i = 0; i < views->GetLength(); i++) {
            AutoPtr<IView> v = (*views)[i];
            if (v != NULL) {
                AutoPtr<IViewPropertyAnimator> animator;
                v->Animate((IViewPropertyAnimator**)&animator);
                animator->TranslationX(0);
                animator->TranslationY(0);
                animator->Alpha(1);
                animator->SetStartDelay(startDelay);
                animator->SetDuration(duration);
                animator->SetInterpolator(cubic);
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
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mWindowAnimationStartTime);
    AnimateInIconOfFirstTask();
    return NOERROR;
}

ECode RecentsPanelView::ClearRecentTasksList()
{
    // Clear memory used by screenshots
    if (mRecentTaskDescriptions != NULL) {
        mRecentTasksLoader->CancelLoadingThumbnailsAndIcons(THIS_PROBE(IRecentsPanelView));
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
    /* [in] */ IObjectContainer* recentTasksList,
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
    /* [in] */ IObjectContainer* tasks,
    /* [in] */ Boolean firstScreenful)
{
    if (mRecentTaskDescriptions == NULL) {
        mRecentTaskDescriptions = new List<AutoPtr<ITaskDescription> >();
    }
    if (tasks != NULL) {
        AutoPtr<IObjectEnumerator> enumerator;
        tasks->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> td;
            enumerator->Current((IInterface**)&td);
            mRecentTaskDescriptions->PushBack(ITaskDescription::Probe(td));
        }
    }
    Boolean b;
    IRecentsActivity::Probe(mContext)->IsActivityShowing(&b);
    if (b) {
        RefreshViews();
    }
    return NOERROR;
}

void RecentsPanelView::UpdateUiElements()
{
    Int32 items = mRecentTaskDescriptions != NULL
            ? mRecentTaskDescriptions->GetSize() : 0;

    mRecentsContainer->SetVisibility(items > 0 ? IView::VISIBLE : IView::GONE);

    // Set description for accessibility
    Int32 numRecentApps = items;
    String recentAppsAccessibilityDescription;
    if (numRecentApps == 0) {
        GetResources()->GetString(SystemUIR::string::status_bar_no_recent_apps, &recentAppsAccessibilityDescription);
    }
    else {
//TODO: Need GetQuantityString(int id, int quantity, Object... formatArgs)
        // res->GetQuantityString(SystemUIR::plurals::status_bar_accessibility_recent_apps, numRecentApps, numRecentApps, &recentAppsAccessibilityDescription);
//TODO: GetQuantityString has bug in finding plurals string
        // <plurals name="status_bar_accessibility_recent_apps">
        //     <item quantity="one">1 recent app</item>
        //     <item quantity="other">%d recent apps</item>
        // </plurals>
        // GetResources()->GetQuantityString(SystemUIR::plurals::status_bar_accessibility_recent_apps, numRecentApps, &recentAppsAccessibilityDescription);
        if (numRecentApps == 1) recentAppsAccessibilityDescription = "1 recent app";
        else {
            StringBuilder sb(StringUtils::Int32ToString(numRecentApps));
            sb += " recent apps";
            recentAppsAccessibilityDescription = sb.ToString();
        }
    }
    AutoPtr<ICharSequence> cs;
    CString::New(recentAppsAccessibilityDescription, (ICharSequence**)&cs);
    SetContentDescription(cs);
}

Boolean RecentsPanelView::SimulateClick(
    /* [in] */ Int32 persistentTaskId)
{
    AutoPtr<IRecentsScrollView> scrollView = IRecentsScrollView::Probe(mRecentsContainer);
    if (scrollView != NULL) {
        AutoPtr<IView> v;
        scrollView->FindViewForTask(persistentTaskId, (IView**)&v);
        if (v != NULL) {
            HandleOnClick(v);
            return TRUE;
        }
    }
    return FALSE;
}

ECode RecentsPanelView::HandleOnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> tag;
    view->GetTag((IInterface**)&tag);
    AutoPtr<ViewHolder> holder = (RecentsPanelView::ViewHolder*)tag.Get();
    AutoPtr<ITaskDescription> ad = holder->mTaskDescription;
    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);
    AutoPtr<IActivityManager> am;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);
    AutoPtr<IBitmap> bm = holder->mThumbnailViewImageBitmap;
    Boolean usingDrawingCache;
    Int32 w1, w2, h1, h2;
    bm->GetWidth(&w1);
    bm->GetHeight(&h1);
    holder->mThumbnailViewImage->GetWidth(&w2);
    holder->mThumbnailViewImage->GetHeight(&h2);
    if (w1 == w2 && h1 == h2) {
        usingDrawingCache = FALSE;
    }
    else {
        holder->mThumbnailViewImage->SetDrawingCacheEnabled(TRUE);
        bm = NULL;
        holder->mThumbnailViewImage->GetDrawingCache((IBitmap**)&bm);
        usingDrawingCache = TRUE;
    }

    AutoPtr<IBundle> opts;
    if (bm != NULL) {
        AutoPtr<IActivityOptionsHelper> helper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
        AutoPtr<IActivityOptions> activtiyOptions;
        helper->MakeThumbnailScaleUpAnimation(
                    holder->mThumbnailViewImage, bm, 0, 0, NULL, (IActivityOptions**)&activtiyOptions);
        activtiyOptions->ToBundle((IBundle**)&opts);
    }

    Show(FALSE);

    Int32 taskId;
    ad->GetTaskId(&taskId);
    if (taskId >= 0) {
        // This is an active task; it should just go to the foreground.
        am->MoveTaskToFront(taskId, IActivityManager::MOVE_TASK_WITH_HOME, opts);
    }
    else {
        AutoPtr<IIntent> intent;
        ad->GetIntent((IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_LAUNCHED_FROM_HISTORY
                | IIntent::FLAG_ACTIVITY_TASK_ON_HOME
                | IIntent::FLAG_ACTIVITY_NEW_TASK);
        if (DEBUG) {
            String istr;
            intent->ToString(&istr);
            Logger::V(TAG, "Starting activity %s", istr.string());
        }
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&userHandle);
        context->StartActivityAsUser(intent, opts, userHandle);
    }
    if (usingDrawingCache) {
        holder->mThumbnailViewImage->SetDrawingCacheEnabled(FALSE);
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
    AutoPtr<ViewHolder> holder = (RecentsPanelView::ViewHolder*)tag.Get();
    AutoPtr<ITaskDescription> ad = holder->mTaskDescription;
    if (ad == NULL) {
        // Log.v(TAG, "Not able to find activity description for swiped task; view=" + view +
        //         " tag=" + view.getTag());
        return NOERROR;
    }
    // if (DEBUG) Log.v(TAG, "Jettison " + ad.getLabel());
    mRecentTaskDescriptions->Remove(ad);
    mRecentTasksLoader->Remove(ad);

    // Handled by widget containers to enable LayoutTransitions properly
    // mListAdapter.notifyDataSetChanged();

    if (mRecentTaskDescriptions->IsEmpty()) {
        DismissAndGoBack();
    }

    // Currently, either direction means the same thing, so ignore direction and remove
    // the task.
    AutoPtr<IActivityManager> am;
    mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);
    if (am != NULL) {
        Int32 persistentTaskId;
        ad->GetPersistentTaskId(&persistentTaskId);
        Boolean b;
        am->RemoveTask(persistentTaskId, IActivityManager::REMOVE_TASK_KILL_PROCESS, &b);

        // Accessibility feedback
        AutoPtr<ICharSequence> cs;
        ad->GetLabel((ICharSequence**)&cs);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, cs.Get());
        String str;
        mContext->GetString(SystemUIR::string::accessibility_recents_item_dismissed, args, &str);
        cs = NULL;
        CString::New(str, (ICharSequence**)&cs);
        SetContentDescription(cs);
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
        SetContentDescription(NULL);
    }
    return NOERROR;
}

void RecentsPanelView::StartApplicationDetailsActivity(
    /* [in] */ const String& packageName)
{
    AutoPtr<IUri> uri;
    Uri::FromParts(String("package"), packageName, String(NULL), (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_APPLICATION_DETAILS_SETTINGS, uri, (IIntent**)&intent);
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IComponentName> cn;
    intent->ResolveActivity(pm, (IComponentName**)&cn);
    intent->SetComponent(cn);
    AutoPtr<ITaskStackBuilderHelper> helper;
    CTaskStackBuilderHelper::AcquireSingleton((ITaskStackBuilderHelper**)&helper);
    AutoPtr<ITaskStackBuilder> builder;
    AutoPtr<IContext> ctx = GetContext();
    helper->Create(ctx, (ITaskStackBuilder**)&builder);
    builder->AddNextIntentWithParentStack(intent);
    builder->StartActivities();
}

Boolean RecentsPanelView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    if (mPopup != NULL) {
        return TRUE;
    }
    else {
        return FrameLayout::OnInterceptTouchEvent(ev);
    }
}

ECode RecentsPanelView::HandleLongPress(
    /* [in] */ IView* selectedView,
    /* [in] */ IView* anchorView,
    /* [in] */ IView* thumbnailView)
{
    thumbnailView->SetSelected(TRUE);
    AutoPtr<IPopupMenu> popup;
    CPopupMenu::New(mContext, anchorView == NULL ? selectedView : anchorView, (IPopupMenu**)&popup);
    mPopup = popup;
    AutoPtr<IMenuInflater> flater;
    popup->GetMenuInflater((IMenuInflater**)&flater);
    AutoPtr<IMenu> menu;
    popup->GetMenu((IMenu**)&menu);
    flater->Inflate(SystemUIR::menu::recent_popup_menu, menu);
    AutoPtr<OnMenuItemClickListener> menuItemClickListener = new OnMenuItemClickListener(this, selectedView);
    popup->SetOnMenuItemClickListener(menuItemClickListener);
    AutoPtr<OnDismissListener> dismissListener = new OnDismissListener(this, thumbnailView);
    popup->SetOnDismissListener(dismissListener);
    popup->Show();
    return NOERROR;
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
