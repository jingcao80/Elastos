
#include "elastos/droid/R.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/widget/ActivityChooserView.h"
//#include "elastos/droid/widget/Toast.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::EIID_IListPopupWindowForwardingListener;
//using Elastos::Droid::Widget::IToast;
//using Elastos::Droid::Widget::Toast;
using Elastos::Core::CString;
using Elastos::Utility::IObservable;
using Elastos::Utility::IObservable;
using Elastos::Utility::IObserver;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//            ActivityChooserView::InnerNotifyDataSetObserver
//=====================================================================
CAR_INTERFACE_IMPL(ActivityChooserView::InnerNotifyDataSetObserver, Object, IDataSetObserver)

ActivityChooserView::InnerNotifyDataSetObserver::InnerNotifyDataSetObserver(
    /* [in] */ ActivityChooserView* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ActivityChooserView::InnerNotifyDataSetObserver::OnChanged()
{
    // ==================before translated======================
    // super.onChanged();
    // mAdapter.notifyDataSetChanged();

    DataSetObserver::OnChanged();
    mOwner->mAdapter->NotifyDataSetChanged();
    return NOERROR;
}

ECode ActivityChooserView::InnerNotifyDataSetObserver::OnInvalidated()
{
    // ==================before translated======================
    // super.onInvalidated();
    // mAdapter.notifyDataSetInvalidated();

    DataSetObserver::OnInvalidated();
    mOwner->mAdapter->NotifyDataSetInvalidated();
    return NOERROR;
}

//=====================================================================
//           ActivityChooserView::InnerOnGlobalLayoutListener
//=====================================================================
CAR_INTERFACE_IMPL(ActivityChooserView::InnerOnGlobalLayoutListener, Object, IOnGlobalLayoutListener)

ActivityChooserView::InnerOnGlobalLayoutListener::InnerOnGlobalLayoutListener(
    /* [in] */ ActivityChooserView* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ActivityChooserView::InnerOnGlobalLayoutListener::OnGlobalLayout()
{
    // ==================before translated======================
    // if (isShowingPopup()) {
    //     if (!isShown()) {
    //         getListPopupWindow().dismiss();
    //     } else {
    //         getListPopupWindow().show();
    //         if (mProvider != null) {
    //             mProvider.subUiVisibilityChanged(true);
    //         }
    //     }
    // }

    Boolean isPopup = FALSE;
    mOwner->IsShowingPopup(&isPopup);
    if (isPopup) {
        Boolean isShown = FALSE;
        mOwner->IsShown(&isShown);
        if (!isShown) {
            mOwner->GetListPopupWindow()->Dismiss();
        }
        else {
            mOwner->GetListPopupWindow()->Show();
            if (mOwner->mProvider != NULL) {
                mOwner->mProvider->SubUiVisibilityChanged(TRUE);
            }
        }
    }
    return NOERROR;
}

//=====================================================================
//           ActivityChooserView::InnerAccessibilityDelegate
//=====================================================================
ActivityChooserView::InnerAccessibilityDelegate::InnerAccessibilityDelegate(
    /* [in] */ ActivityChooserView* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ActivityChooserView::InnerAccessibilityDelegate::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IView* host,
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(host);
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // super.onInitializeAccessibilityNodeInfo(host, info);
    // info.setCanOpenPopup(true);

    Elastos::Droid::View::View::AccessibilityDelegate::OnInitializeAccessibilityNodeInfo(host, info);
    info->SetCanOpenPopup(TRUE);
    return NOERROR;
}

//=====================================================================
//            ActivityChooserView::InnerForwardingListener
//=====================================================================
CAR_INTERFACE_IMPL(ActivityChooserView::InnerForwardingListener, Object, IListPopupWindowForwardingListener)

ActivityChooserView::InnerForwardingListener::InnerForwardingListener(
    /* [in] */ ActivityChooserView* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ActivityChooserView::InnerForwardingListener::GetPopup(
    /* [out] */ IListPopupWindow** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IListPopupWindow> temp = mOwner->GetListPopupWindow();
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean ActivityChooserView::InnerForwardingListener::OnForwardingStarted()
{
    Boolean resTmp;
    mOwner->ShowPopup(&resTmp);
    return TRUE;
}

Boolean ActivityChooserView::InnerForwardingListener::OnForwardingStopped()
{
    Boolean resTmp;
    mOwner->DismissPopup(&resTmp);
    return TRUE;
}

//=====================================================================
//           ActivityChooserView::InnerUpdateDataSetObserver
//=====================================================================
ActivityChooserView::InnerUpdateDataSetObserver::InnerUpdateDataSetObserver(
    /* [in] */ ActivityChooserView* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ActivityChooserView::InnerUpdateDataSetObserver::OnChanged()
{
    // ==================before translated======================
    // super.onChanged();
    // updateAppearance();

    DataSetObserver::OnChanged();
    mOwner->UpdateAppearance();
    return NOERROR;
}

ECode ActivityChooserView::InnerUpdateDataSetObserver::OnInvalidated()
{
    return NOERROR;
}

//=====================================================================
//                    ActivityChooserView::Callbacks
//=====================================================================
CAR_INTERFACE_IMPL_4(ActivityChooserView::Callbacks, Object, IAdapterViewOnItemClickListener,
    IViewOnClickListener, IViewOnLongClickListener, IPopupWindowOnDismissListener)

ActivityChooserView::Callbacks::Callbacks(
    /* [in] */ ActivityChooserView* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ActivityChooserView::Callbacks::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    VALIDATE_NOT_NULL(parent);
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // ActivityChooserViewAdapter adapter = (ActivityChooserViewAdapter) parent.getAdapter();
    // final int itemViewType = adapter.getItemViewType(position);
    // switch (itemViewType) {
    //     case ActivityChooserViewAdapter.ITEM_VIEW_TYPE_FOOTER: {
    //         showPopupUnchecked(ActivityChooserViewAdapter.MAX_ACTIVITY_COUNT_UNLIMITED);
    //     } break;
    //     case ActivityChooserViewAdapter.ITEM_VIEW_TYPE_ACTIVITY: {
    //         dismissPopup();
    //         if (mIsSelectingDefaultActivity) {
    //             // The item at position zero is the default already.
    //             if (position > 0) {
    //                 mAdapter.getDataModel().setDefaultActivity(position);
    //             }
    //         } else {
    //             // If the default target is not shown in the list, the first
    //             // item in the model is default action => adjust index
    //             position = mAdapter.getShowDefaultActivity() ? position : position + 1;
    //             Intent launchIntent = mAdapter.getDataModel().chooseActivity(position);
    //             if (launchIntent != null) {
    //                 launchIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
    //                 ResolveInfo resolveInfo = mAdapter.getDataModel().getActivity(position);
    //                 startActivity(launchIntent, resolveInfo);
    //             }
    //         }
    //     } break;
    //     default:
    //         throw new IllegalArgumentException();
    // }

    AutoPtr<IAdapter> adapterTmp;
    parent->GetAdapter((IAdapter**)&adapterTmp);
    ActivityChooserViewAdapter* adapter = (ActivityChooserViewAdapter*)adapterTmp.Get();

    Int32 itemViewType = 0;
    adapter->GetItemViewType(position, &itemViewType);
    switch (itemViewType) {
        case ActivityChooserViewAdapter::ITEM_VIEW_TYPE_FOOTER:
            mOwner->ShowPopupUnchecked(ActivityChooserView::ActivityChooserViewAdapter::MAX_ACTIVITY_COUNT_UNLIMITED);
            break;
        case ActivityChooserViewAdapter::ITEM_VIEW_TYPE_ACTIVITY:
            {
                Boolean resTmp;
                mOwner->DismissPopup(&resTmp);
                if (mOwner->mIsSelectingDefaultActivity) {
                    // The item at position zero is the default already.
                    if (position > 0) {
                        AutoPtr<IActivityChooserModel> activityChooserModel;
                        mOwner->mAdapter->GetDataModel((IActivityChooserModel**)&activityChooserModel);
                        activityChooserModel->SetDefaultActivity(position);
                    }
                }
                else {
                    // If the default target is not shown in the list, the first
                    // item in the model is default action => adjust index
                    Boolean resTmp = FALSE;
                    mOwner->mAdapter->GetShowDefaultActivity(&resTmp);
                    position = resTmp ? position : position + 1;

                    AutoPtr<IActivityChooserModel> activityChooserModel;
                    mOwner->mAdapter->GetDataModel((IActivityChooserModel**)&activityChooserModel);
                    AutoPtr<IIntent> launchIntent;
                    activityChooserModel->ChooseActivity(position, (IIntent**)&launchIntent);
                    if (launchIntent != NULL) {
                        launchIntent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
                        AutoPtr<IResolveInfo> resolveInfo;
                        activityChooserModel->GetActivity(position, (IResolveInfo**)&resolveInfo);
                        StartActivity(launchIntent, resolveInfo);
                    }
                }
            }
            break;
        default:
            {
                //throw new IllegalArgumentException();
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
    }
    return NOERROR;
}

ECode ActivityChooserView::Callbacks::OnClick(
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // if (view == mDefaultActivityButton) {
    //     dismissPopup();
    //     ResolveInfo defaultActivity = mAdapter.getDefaultActivity();
    //     final int index = mAdapter.getDataModel().getActivityIndex(defaultActivity);
    //     Intent launchIntent = mAdapter.getDataModel().chooseActivity(index);
    //     if (launchIntent != null) {
    //         launchIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
    //         startActivity(launchIntent, defaultActivity);
    //     }
    // } else if (view == mExpandActivityOverflowButton) {
    //     mIsSelectingDefaultActivity = false;
    //     showPopupUnchecked(mInitialActivityCount);
    // } else {
    //     throw new IllegalArgumentException();
    // }

    if (TO_IINTERFACE(view )== TO_IINTERFACE(mOwner->mDefaultActivityButton)) {
        Boolean resTmp;
        mOwner->DismissPopup(&resTmp);
        AutoPtr<IResolveInfo> defaultActivity;
        mOwner->mAdapter->GetDefaultActivity((IResolveInfo**)&defaultActivity);

        AutoPtr<IActivityChooserModel> activityChooserModel;
        mOwner->mAdapter->GetDataModel((IActivityChooserModel**)&activityChooserModel);

        Int32 index = 0;
        activityChooserModel->GetActivityIndex(defaultActivity, &index);
        AutoPtr<IIntent> launchIntent;
        activityChooserModel->ChooseActivity(index, (IIntent**)&launchIntent);
        if (launchIntent != NULL) {
            launchIntent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
            StartActivity(launchIntent, defaultActivity);
        }
    }
    else if (TO_IINTERFACE(view) == TO_IINTERFACE(mOwner->mExpandActivityOverflowButton)) {
        mOwner->mIsSelectingDefaultActivity = FALSE;
        mOwner->ShowPopupUnchecked(mOwner->mInitialActivityCount);
    }
    else {
        //throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ActivityChooserView::Callbacks::OnLongClick(
    /* [in] */ IView* view,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (view == mDefaultActivityButton) {
    //     if (mAdapter.getCount() > 0) {
    //         mIsSelectingDefaultActivity = true;
    //         showPopupUnchecked(mInitialActivityCount);
    //     }
    // } else {
    //     throw new IllegalArgumentException();
    // }
    // return true;

    if (TO_IINTERFACE(view) == TO_IINTERFACE(mOwner->mDefaultActivityButton)) {
        Int32 count = 0;
        mOwner->mAdapter->GetCount(&count);
        if (count > 0) {
            mOwner->mIsSelectingDefaultActivity = TRUE;
            mOwner->ShowPopupUnchecked(mOwner->mInitialActivityCount);
        }
    }
    else {
        //throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = TRUE;
    return NOERROR;
}

ECode ActivityChooserView::Callbacks::OnDismiss()
{
    // ==================before translated======================
    // notifyOnDismissListener();
    // if (mProvider != null) {
    //     mProvider.subUiVisibilityChanged(false);
    // }

    NotifyOnDismissListener();
    if (mOwner->mProvider != NULL) {
        mOwner->mProvider->SubUiVisibilityChanged(FALSE);
    }
    return NOERROR;
}

void ActivityChooserView::Callbacks::NotifyOnDismissListener()
{
    // ==================before translated======================
    // if (mOnDismissListener != null) {
    //     mOnDismissListener.onDismiss();
    // }

    if (mOwner->mOnDismissListener != NULL) {
        mOwner->mOnDismissListener->OnDismiss();
    }
}

void ActivityChooserView::Callbacks::StartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IResolveInfo* resolveInfo)
{
    // ==================before translated======================
    // try {
    //     mContext.startActivity(intent);
    // } catch (RuntimeException re) {
    //     CharSequence appLabel = resolveInfo.loadLabel(mContext.getPackageManager());
    //     String message = mContext.getString(
    //             R.string.activitychooserview_choose_application_error, appLabel);
    //     Log.e(LOG_TAG, message);
    //     Toast.makeText(mContext, message, Toast.LENGTH_SHORT).show();
    // }

    assert(0);
    //try {
        mOwner->mContext->StartActivity(intent);
    //}
    //catch (RuntimeException re) {
        AutoPtr<IPackageManager> packageManager;
        mOwner->mContext->GetPackageManager((IPackageManager**)&packageManager);
        AutoPtr<ICharSequence> appLabel;
        resolveInfo->LoadLabel(packageManager, (ICharSequence**)&appLabel);

        AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
        formatArgs->Set(0, TO_IINTERFACE(appLabel));
        String message;
        mOwner->mContext->GetString(R::string::activitychooserview_choose_application_error, formatArgs, &message);
        Logger::E(TAG, message);

        AutoPtr<ICharSequence> charSequenceMsg;
        CString::New(message, (ICharSequence**)&charSequenceMsg);

        //AutoPtr<IToast> toast;
        //Toast::MakeText(mOwner->mContext, charSequenceMsg, IToast::LENGTH_SHORT, (IToast**)&toast);
        //toast->Show();
    //}
}

//=====================================================================
//           ActivityChooserView::ActivityChooserViewAdapter
//=====================================================================
const Int32 ActivityChooserView::ActivityChooserViewAdapter::MAX_ACTIVITY_COUNT_UNLIMITED = Elastos::Core::Math::INT32_MAX_VALUE;
const Int32 ActivityChooserView::ActivityChooserViewAdapter::MAX_ACTIVITY_COUNT_DEFAULT;
const Int32 ActivityChooserView::ActivityChooserViewAdapter::ITEM_VIEW_TYPE_ACTIVITY;
const Int32 ActivityChooserView::ActivityChooserViewAdapter::ITEM_VIEW_TYPE_FOOTER;
const Int32 ActivityChooserView::ActivityChooserViewAdapter::ITEM_VIEW_TYPE_COUNT;

ActivityChooserView::ActivityChooserViewAdapter::ActivityChooserViewAdapter(
    /* [in] */ ActivityChooserView* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ActivityChooserView::ActivityChooserViewAdapter::SetDataModel(
    /* [in] */ IActivityChooserModel* dataModel)
{
    VALIDATE_NOT_NULL(dataModel);
    // ==================before translated======================
    // ActivityChooserModel oldDataModel = mAdapter.getDataModel();
    // if (oldDataModel != null && isShown()) {
    //     oldDataModel.unregisterObserver(mModelDataSetOberver);
    // }
    // mDataModel = dataModel;
    // if (dataModel != null && isShown()) {
    //     dataModel.registerObserver(mModelDataSetOberver);
    // }
    // notifyDataSetChanged();

    AutoPtr<IActivityChooserModel> oldDataModel;
    mOwner->mAdapter->GetDataModel((IActivityChooserModel**)&oldDataModel);

    Boolean resTmp = FALSE;
    mOwner->IsShown(&resTmp);
    if (oldDataModel != NULL && resTmp) {
        (IObservable::Probe(oldDataModel))->DeleteObserver(IObserver::Probe(mOwner->mModelDataSetOberver));
    }

    mDataModel = dataModel;
    if (dataModel != NULL && resTmp) {
        (IObservable::Probe(dataModel))->AddObserver(IObserver::Probe(mOwner->mModelDataSetOberver));
    }

    NotifyDataSetChanged();
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mShowFooterView && position == getCount() - 1) {
    //     return ITEM_VIEW_TYPE_FOOTER;
    // } else {
    //     return ITEM_VIEW_TYPE_ACTIVITY;
    // }

    Int32 count = 0;
    GetCount(&count);
    if (mShowFooterView && position == count - 1) {
        *result = ITEM_VIEW_TYPE_FOOTER;
    }
    else {
        *result = ITEM_VIEW_TYPE_ACTIVITY;
    }
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetViewTypeCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ITEM_VIEW_TYPE_COUNT;

    *result = ITEM_VIEW_TYPE_COUNT;
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int count = 0;
    // int activityCount = mDataModel.getActivityCount();
    // if (!mShowDefaultActivity && mDataModel.getDefaultActivity() != null) {
    //     activityCount--;
    // }
    // count = Math.min(activityCount, mMaxActivityCount);
    // if (mShowFooterView) {
    //     count++;
    // }

    Int32 count = 0;
    Int32 activityCount = 0;
    mDataModel->GetActivityCount(&activityCount);

    AutoPtr<IResolveInfo> resolveInfo;
    mDataModel->GetDefaultActivity((IResolveInfo**)&resolveInfo);
    if (!mShowDefaultActivity && resolveInfo != NULL) {
        --activityCount;
    }
    count = Elastos::Core::Math::Min(activityCount, mMaxActivityCount);
    if (mShowFooterView) {
        ++count;
    }
    *result = count;
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final int itemViewType = getItemViewType(position);
    // switch (itemViewType) {
    //     case ITEM_VIEW_TYPE_FOOTER:
    //         return null;
    //     case ITEM_VIEW_TYPE_ACTIVITY:
    //         if (!mShowDefaultActivity && mDataModel.getDefaultActivity() != null) {
    //             position++;
    //         }
    //         return mDataModel.getActivity(position);
    //     default:
    //         throw new IllegalArgumentException();
    // }

    Int32 itemViewType = 0;
    GetItemViewType(position, &itemViewType);
    switch (itemViewType) {
        case ITEM_VIEW_TYPE_FOOTER:
            *result = NULL;
            return NOERROR;
        case ITEM_VIEW_TYPE_ACTIVITY:
            {
                AutoPtr<IResolveInfo> defaultResolveInfo;
                mDataModel->GetDefaultActivity((IResolveInfo**)&defaultResolveInfo);
                if (!mShowDefaultActivity && defaultResolveInfo != NULL) {
                    ++position;
                }

                AutoPtr<IResolveInfo> activityResolveInfo;
                mDataModel->GetActivity(position, (IResolveInfo**)&activityResolveInfo);
            }
            return NOERROR;
        default:
            //throw new IllegalArgumentException();
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return position;

    *result = position;
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(convertView);
    VALIDATE_NOT_NULL(parent);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final int itemViewType = getItemViewType(position);
    // switch (itemViewType) {
    //     case ITEM_VIEW_TYPE_FOOTER:
    //         if (convertView == null || convertView.getId() != ITEM_VIEW_TYPE_FOOTER) {
    //             convertView = LayoutInflater.from(getContext()).inflate(
    //                     R.layout.activity_chooser_view_list_item, parent, false);
    //             convertView.setId(ITEM_VIEW_TYPE_FOOTER);
    //             TextView titleView = (TextView) convertView.findViewById(R.id.title);
    //             titleView.setText(mContext.getString(
    //                     R.string.activity_chooser_view_see_all));
    //         }
    //         return convertView;
    //     case ITEM_VIEW_TYPE_ACTIVITY:
    //         if (convertView == null || convertView.getId() != R.id.list_item) {
    //             convertView = LayoutInflater.from(getContext()).inflate(
    //                     R.layout.activity_chooser_view_list_item, parent, false);
    //         }
    //         PackageManager packageManager = mContext.getPackageManager();
    //         // Set the icon
    //         ImageView iconView = (ImageView) convertView.findViewById(R.id.icon);
    //         ResolveInfo activity = (ResolveInfo) getItem(position);
    //         iconView.setImageDrawable(activity.loadIcon(packageManager));
    //         // Set the title.
    //         TextView titleView = (TextView) convertView.findViewById(R.id.title);
    //         titleView.setText(activity.loadLabel(packageManager));
    //         // Highlight the default.
    //         if (mShowDefaultActivity && position == 0 && mHighlightDefaultActivity) {
    //             convertView.setActivated(true);
    //         } else {
    //             convertView.setActivated(false);
    //         }
    //         return convertView;
    //     default:
    //         throw new IllegalArgumentException();
    // }

    Int32 itemViewType = 0;
    GetItemViewType(position, &itemViewType);
    switch (itemViewType) {
        case ITEM_VIEW_TYPE_FOOTER:
            {
                AutoPtr<IView> convertViewTmp = convertView;
                Int32 id = 0;
                if (convertView == NULL || (convertView->GetId(&id), id) != ITEM_VIEW_TYPE_FOOTER) {
                    AutoPtr<IContext> context;
                    mOwner->GetContext((IContext**)&context);

                    AutoPtr<ILayoutInflater> layoutInflater;
                    LayoutInflater::From(context, (ILayoutInflater**)&layoutInflater);

                    AutoPtr<IView> viewTmp;
                    layoutInflater->Inflate(R::layout::activity_chooser_view_list_item, parent, FALSE, (IView**)&viewTmp);
                    viewTmp->SetId(ITEM_VIEW_TYPE_FOOTER);

                    convertViewTmp = NULL;
                    convertViewTmp = viewTmp;

                    AutoPtr<IView> viewTmp1;
                    viewTmp->FindViewById(R::id::title, (IView**)&viewTmp1);
                    ITextView* titleView = ITextView::Probe(viewTmp1);

                    String str;
                    mOwner->mContext->GetString(R::string::activity_chooser_view_see_all, &str);
                    AutoPtr<ICharSequence> charSequenceTmp;
                    CString::New(str, (ICharSequence**)&charSequenceTmp);
                    titleView->SetText(charSequenceTmp);

                    *result = convertViewTmp;
                    REFCOUNT_ADD(*result);
                    return NOERROR;
                }
                *result = convertViewTmp;
                REFCOUNT_ADD(*result);
            }
            return NOERROR;
        case ITEM_VIEW_TYPE_ACTIVITY:
            {
                AutoPtr<IView> convertViewTmp = convertView;
                Int32 id = 0;
                if (convertView == NULL || (convertView->GetId(&id), id) != R::id::list_item) {
                    AutoPtr<IContext> context;
                    mOwner->GetContext((IContext**)&context);

                    AutoPtr<ILayoutInflater> layoutInflater;
                    LayoutInflater::From(context, (ILayoutInflater**)&layoutInflater);

                    AutoPtr<IView> viewTmp;
                    layoutInflater->Inflate(R::layout::activity_chooser_view_list_item, parent, FALSE, (IView**)&viewTmp);
                    viewTmp->SetId(ITEM_VIEW_TYPE_FOOTER);

                    convertViewTmp = NULL;
                    convertViewTmp = viewTmp;
                }

                AutoPtr<IPackageManager> packageManager;
                mOwner->mContext->GetPackageManager((IPackageManager**)&packageManager);

                // Set the icon
                AutoPtr<IView> viewTmp;
                convertViewTmp->FindViewById(R::id::icon, (IView**)&viewTmp);
                IImageView* iconView = IImageView::Probe(viewTmp);

                AutoPtr<IInterface> interfaceTmp;
                GetItem(position, (IInterface**)&interfaceTmp);
                IResolveInfo* activity = IResolveInfo::Probe(interfaceTmp);

                AutoPtr<IDrawable> drawable;
                activity->LoadIcon(packageManager, (IDrawable**)&drawable);
                iconView->SetImageDrawable(drawable);
                // Set the title.
                AutoPtr<IView> viewTmp1;
                convertViewTmp->FindViewById(R::id::title, (IView**)&viewTmp1);
                ITextView* titleView = ITextView::Probe(viewTmp1);

                AutoPtr<ICharSequence> label;
                activity->LoadLabel(packageManager, (ICharSequence**)&label);
                titleView->SetText(label);
                // Highlight the default.
                if (mShowDefaultActivity && position == 0 && mHighlightDefaultActivity) {
                    convertView->SetActivated(TRUE);
                }
                else {
                    convertView->SetActivated(FALSE);
                }

                *result = convertViewTmp;
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
        default:
            //throw new IllegalArgumentException();
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::MeasureContentWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // The user may have specified some of the target not to be shown but we
    // // want to measure all of them since after expansion they should fit.
    // final int oldMaxActivityCount = mMaxActivityCount;
    // mMaxActivityCount = MAX_ACTIVITY_COUNT_UNLIMITED;
    //
    // int contentWidth = 0;
    // View itemView = null;
    //
    // final int widthMeasureSpec = MeasureSpec.makeMeasureSpec(0, MeasureSpec.UNSPECIFIED);
    // final int heightMeasureSpec = MeasureSpec.makeMeasureSpec(0, MeasureSpec.UNSPECIFIED);
    // final int count = getCount();
    //
    // for (int i = 0; i < count; i++) {
    //     itemView = getView(i, itemView, null);
    //     itemView.measure(widthMeasureSpec, heightMeasureSpec);
    //     contentWidth = Math.max(contentWidth, itemView.getMeasuredWidth());
    // }
    //
    // mMaxActivityCount = oldMaxActivityCount;
    //
    // return contentWidth;

    Int32 oldMaxActivityCount = mMaxActivityCount;
    mMaxActivityCount = MAX_ACTIVITY_COUNT_UNLIMITED;
    AutoPtr<IView> itemView;

    Int32 contentWidth = 0;
    Int32 widthMeasureSpec = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);
    Int32 heightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);
    Int32 count = 0;
    GetCount(&count);
    Int32 measureWidth = 0;
    for (int i = 0; i < count; i++) {
        AutoPtr<IView> itemViewtmp = itemView;
        GetView(i, itemViewtmp, NULL, (IView**)&itemView);
        itemView->Measure(widthMeasureSpec, heightMeasureSpec);
        itemView->GetMeasuredWidth(&measureWidth);
        contentWidth = Elastos::Core::Math::Max(contentWidth, measureWidth);
    }

    mMaxActivityCount = oldMaxActivityCount;
    *result = contentWidth;
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::SetMaxActivityCount(
    /* [in] */ Int32 maxActivityCount)
{
    // ==================before translated======================
    // if (mMaxActivityCount != maxActivityCount) {
    //     mMaxActivityCount = maxActivityCount;
    //     notifyDataSetChanged();
    // }

    if (mMaxActivityCount != maxActivityCount) {
        mMaxActivityCount = maxActivityCount;
        NotifyDataSetChanged();
    }
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetDefaultActivity(
    /* [out] */ IResolveInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDataModel.getDefaultActivity();

    mDataModel->GetDefaultActivity(result);
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::SetShowFooterView(
    /* [in] */ Boolean showFooterView)
{
    // ==================before translated======================
    // if (mShowFooterView != showFooterView) {
    //     mShowFooterView = showFooterView;
    //     notifyDataSetChanged();
    // }

    if (mShowFooterView != showFooterView) {
        mShowFooterView = showFooterView;
        NotifyDataSetChanged();
    }
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetActivityCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDataModel.getActivityCount();

    mDataModel->GetActivityCount(result);
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetHistorySize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDataModel.getHistorySize();

    mDataModel->GetHistorySize(result);
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetDataModel(
    /* [out] */ IActivityChooserModel** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDataModel;

    *result = mDataModel;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::SetShowDefaultActivity(
    /* [in] */ Boolean showDefaultActivity,
    /* [in] */ Boolean highlightDefaultActivity)
{
    // ==================before translated======================
    // if (mShowDefaultActivity != showDefaultActivity
    //         || mHighlightDefaultActivity != highlightDefaultActivity) {
    //     mShowDefaultActivity = showDefaultActivity;
    //     mHighlightDefaultActivity = highlightDefaultActivity;
    //     notifyDataSetChanged();
    // }

    if (mShowDefaultActivity != showDefaultActivity || mHighlightDefaultActivity != highlightDefaultActivity) {
        mShowDefaultActivity = showDefaultActivity;
        mHighlightDefaultActivity = highlightDefaultActivity;
        NotifyDataSetChanged();
    }
    return NOERROR;
}

ECode ActivityChooserView::ActivityChooserViewAdapter::GetShowDefaultActivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mShowDefaultActivity;

    *result = mShowDefaultActivity;
    return NOERROR;
}

//=====================================================================
//                         ActivityChooserView
//=====================================================================
const String ActivityChooserView::TAG("ActivityChooserView");

CAR_INTERFACE_IMPL_2(ActivityChooserView, ViewGroup, IActivityChooserView, IActivityChooserModelClient)

ActivityChooserView::ActivityChooserView()
    : mListPopupMaxWidth(0)
    , mIsSelectingDefaultActivity(FALSE)
    , mInitialActivityCount(0)
    , mIsAttachedToWindow(FALSE)
    , mDefaultActionButtonContentDescription(0)
{
}

ECode ActivityChooserView::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // this(context, null);

    return constructor(context, NULL);
}

ECode ActivityChooserView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // this(context, attrs, 0);

    return constructor(context, attrs, 0);
}

ECode ActivityChooserView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    // ==================before translated======================
    // this(context, attrs, defStyleAttr, 0);

    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ActivityChooserView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    // ==================before translated======================
    // super(context, attrs, defStyleAttr, defStyleRes);
    //
    // TypedArray attributesArray = context.obtainStyledAttributes(attrs,
    //         R.styleable.ActivityChooserView, defStyleAttr, defStyleRes);
    //
    // mInitialActivityCount = attributesArray.getInt(
    //         R.styleable.ActivityChooserView_initialActivityCount,
    //         ActivityChooserViewAdapter.MAX_ACTIVITY_COUNT_DEFAULT);
    //
    // Drawable expandActivityOverflowButtonDrawable = attributesArray.getDrawable(
    //         R.styleable.ActivityChooserView_expandActivityOverflowButtonDrawable);
    //
    // attributesArray.recycle();
    //
    // LayoutInflater inflater = LayoutInflater.from(mContext);
    // inflater.inflate(R.layout.activity_chooser_view, this, true);
    //
    // mCallbacks = new Callbacks();
    //
    // mActivityChooserContent = (LinearLayout) findViewById(R.id.activity_chooser_view_content);
    // mActivityChooserContentBackground = mActivityChooserContent.getBackground();
    //
    // mDefaultActivityButton = (FrameLayout) findViewById(R.id.default_activity_button);
    // mDefaultActivityButton.setOnClickListener(mCallbacks);
    // mDefaultActivityButton.setOnLongClickListener(mCallbacks);
    // mDefaultActivityButtonImage = (ImageView) mDefaultActivityButton.findViewById(R.id.image);
    //
    // final FrameLayout expandButton = (FrameLayout) findViewById(R.id.expand_activities_button);
    // expandButton.setOnClickListener(mCallbacks);
    // expandButton.setAccessibilityDelegate(new AccessibilityDelegate() {
    //     @Override
    //     public void onInitializeAccessibilityNodeInfo(View host, AccessibilityNodeInfo info) {
    //         super.onInitializeAccessibilityNodeInfo(host, info);
    //         info.setCanOpenPopup(true);
    //     }
    // });
    // expandButton.setOnTouchListener(new ForwardingListener(expandButton) {
    //     @Override
    //     public ListPopupWindow getPopup() {
    //         return getListPopupWindow();
    //     }
    //
    //     @Override
    //     protected boolean onForwardingStarted() {
    //         showPopup();
    //         return true;
    //     }
    //
    //     @Override
    //     protected boolean onForwardingStopped() {
    //         dismissPopup();
    //         return true;
    //     }
    // });
    // mExpandActivityOverflowButton = expandButton;
    //
    // mExpandActivityOverflowButtonImage =
    //     (ImageView) expandButton.findViewById(R.id.image);
    // mExpandActivityOverflowButtonImage.setImageDrawable(expandActivityOverflowButtonDrawable);
    //
    // mAdapter = new ActivityChooserViewAdapter();
    // mAdapter.registerDataSetObserver(new DataSetObserver() {
    //     @Override
    //     public void onChanged() {
    //         super.onChanged();
    //         updateAppearance();
    //     }
    // });
    //
    // Resources resources = context.getResources();
    // mListPopupMaxWidth = Math.max(resources.getDisplayMetrics().widthPixels / 2,
    //       resources.getDimensionPixelSize(com.android.internal.R.dimen.config_prefDialogWidth));

    assert(0 && "TODO");
    ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes);
    AutoPtr< ArrayOf<Int32> > resAttrs = TO_ATTRS_ARRAYOF(R::styleable::ActivityChooserView);
    AutoPtr<ITypedArray> attributesArray;
    context->ObtainStyledAttributes(attrs, resAttrs, defStyleAttr, defStyleRes, (ITypedArray**)&attributesArray);

    attributesArray->GetInt32(R::styleable::ActivityChooserView_initialActivityCount,
        ActivityChooserViewAdapter::MAX_ACTIVITY_COUNT_DEFAULT, &mInitialActivityCount);

    AutoPtr<IDrawable> expandActivityOverflowButtonDrawable;
    attributesArray->GetDrawable(R::styleable::ActivityChooserView_expandActivityOverflowButtonDrawable, (IDrawable**)&expandActivityOverflowButtonDrawable);

    attributesArray->Recycle();

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> viewTmp;
    inflater->Inflate(R::layout::activity_chooser_view, this, TRUE, (IView**)&viewTmp);

    mCallbacks = new Callbacks(this);

    AutoPtr<IView> viewTmp1;
    FindViewById(R::id::activity_chooser_view_content, (IView**)&viewTmp1);
    mActivityChooserContent = (LinearLayout*)viewTmp1.Get();
    mActivityChooserContent->GetBackground((IDrawable**)&mActivityChooserContentBackground);

    AutoPtr<IView> viewTmp2;
    FindViewById(R::id::default_activity_button, (IView**)&viewTmp2);
    mDefaultActivityButton = (FrameLayout*)viewTmp2.Get();
    mDefaultActivityButton->SetOnClickListener(mCallbacks);
    mDefaultActivityButton->SetOnLongClickListener(mCallbacks);

    AutoPtr<IView> viewTmp3;
    mDefaultActivityButton->FindViewById(R::id::image, (IView**)&viewTmp3);
    mDefaultActivityButtonImage = IImageView::Probe(viewTmp3);

    AutoPtr<IView> viewTmp4;
    FindViewById(R::id::expand_activities_button, (IView**)&viewTmp4);
    FrameLayout* expandButton = (FrameLayout*)viewTmp4.Get();
    expandButton->SetOnClickListener(mCallbacks);

    AutoPtr<InnerAccessibilityDelegate> innerDelegate = new InnerAccessibilityDelegate(this);
    expandButton->SetAccessibilityDelegate(innerDelegate);

    AutoPtr<InnerForwardingListener> innerForwardListener = new InnerForwardingListener(this);
    expandButton->SetOnTouchListener(IViewOnTouchListener::Probe(innerForwardListener));
    mExpandActivityOverflowButton = expandButton;

    AutoPtr<IView> viewTmp5;
    expandButton->FindViewById(R::id::image, (IView**)&viewTmp5);
    mExpandActivityOverflowButtonImage = IImageView::Probe(viewTmp5);
    mExpandActivityOverflowButtonImage->SetImageDrawable(expandActivityOverflowButtonDrawable);

    mAdapter = new ActivityChooserViewAdapter(this);
    AutoPtr<InnerUpdateDataSetObserver> innerDataSetObserver = new InnerUpdateDataSetObserver(this);
    mAdapter->RegisterDataSetObserver(innerDataSetObserver);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);

    AutoPtr<IDisplayMetrics> displayMetrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    Int32 widthPixels = 0;
    displayMetrics->GetWidthPixels(&widthPixels);

    Int32 pixelSize = 0;
    resources->GetDimensionPixelSize(R::dimen::config_prefDialogWidth, &pixelSize);
    mListPopupMaxWidth = Elastos::Core::Math::Max(widthPixels / 2, pixelSize);
    return NOERROR;
}

ECode ActivityChooserView::SetActivityChooserModel(
    /* [in] */ IActivityChooserModel* dataModel)
{
    VALIDATE_NOT_NULL(dataModel);
    // ==================before translated======================
    // mAdapter.setDataModel(dataModel);
    // if (isShowingPopup()) {
    //     dismissPopup();
    //     showPopup();
    // }

    mAdapter->SetDataModel(dataModel);
    Boolean isShowingPopup = FALSE;
    IsShowingPopup(&isShowingPopup);
    if (isShowingPopup) {
        Boolean resTmp;
        DismissPopup(&resTmp);
        ShowPopup(&resTmp);
    }
    return NOERROR;
}

ECode ActivityChooserView::SetExpandActivityOverflowButtonDrawable(
    /* [in] */ IDrawable* drawable)
{
    VALIDATE_NOT_NULL(drawable);
    // ==================before translated======================
    // mExpandActivityOverflowButtonImage.setImageDrawable(drawable);

    mExpandActivityOverflowButtonImage->SetImageDrawable(drawable);
    return NOERROR;
}

ECode ActivityChooserView::SetExpandActivityOverflowButtonContentDescription(
    /* [in] */ Int32 resourceId)
{
    // ==================before translated======================
    // CharSequence contentDescription = mContext.getString(resourceId);
    // mExpandActivityOverflowButtonImage.setContentDescription(contentDescription);

    String str;
    mContext->GetString(resourceId, &str);
    AutoPtr<ICharSequence> contentDescription;
    CString::New(str, (ICharSequence**)&contentDescription);
    assert(0 && "TODO");
    // mExpandActivityOverflowButtonImage->SetContentDescription(contentDescription);
    return NOERROR;
}

ECode ActivityChooserView::SetProvider(
    /* [in] */ IActionProvider* provider)
{
    VALIDATE_NOT_NULL(provider);
    // ==================before translated======================
    // mProvider = provider;

    mProvider = provider;
    return NOERROR;
}

ECode ActivityChooserView::ShowPopup(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (isShowingPopup() || !mIsAttachedToWindow) {
    //     return false;
    // }
    // mIsSelectingDefaultActivity = false;
    // showPopupUnchecked(mInitialActivityCount);
    // return true;

    Boolean isShowingPopup = FALSE;
    IsShowingPopup(&isShowingPopup);
    if (isShowingPopup || !mIsAttachedToWindow) {
        *result = FALSE;
        return NOERROR;
    }
    mIsSelectingDefaultActivity = FALSE;
    ShowPopupUnchecked(mInitialActivityCount);
    *result = TRUE;
    return NOERROR;
}

ECode ActivityChooserView::DismissPopup(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (isShowingPopup()) {
    //     getListPopupWindow().dismiss();
    //     ViewTreeObserver viewTreeObserver = getViewTreeObserver();
    //     if (viewTreeObserver.isAlive()) {
    //         viewTreeObserver.removeOnGlobalLayoutListener(mOnGlobalLayoutListener);
    //     }
    // }
    // return true;

    Boolean isShowingPopup = FALSE;
    IsShowingPopup(&isShowingPopup);
    if (isShowingPopup) {
        GetListPopupWindow()->Dismiss();
        AutoPtr<IViewTreeObserver> viewTreeObserver;
        GetViewTreeObserver((IViewTreeObserver**)&viewTreeObserver);
        Boolean isAlive = FALSE;
        viewTreeObserver->IsAlive(&isAlive);
        if (isAlive) {
            viewTreeObserver->RemoveOnGlobalLayoutListener(mOnGlobalLayoutListener);
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode ActivityChooserView::IsShowingPopup(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getListPopupWindow().isShowing();

    GetListPopupWindow()->IsShowing(result);
    return NOERROR;
}

ECode ActivityChooserView::GetDataModel(
    /* [out] */ IActivityChooserModel** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAdapter.getDataModel();

    mAdapter->GetDataModel(result);
    return NOERROR;
}

ECode ActivityChooserView::SetOnDismissListener(
    /* [in] */ IPopupWindowOnDismissListener* listener)
{
    // ==================before translated======================
    // mOnDismissListener = listener;

    mOnDismissListener = listener;
    return NOERROR;
}

ECode ActivityChooserView::SetInitialActivityCount(
    /* [in] */ Int32 itemCount)
{
    // ==================before translated======================
    // mInitialActivityCount = itemCount;

    mInitialActivityCount = itemCount;
    return NOERROR;
}

ECode ActivityChooserView::SetDefaultActionButtonContentDescription(
    /* [in] */ Int32 resourceId)
{
    // ==================before translated======================
    // mDefaultActionButtonContentDescription = resourceId;

    mDefaultActionButtonContentDescription = resourceId;
    return NOERROR;
}

ECode ActivityChooserView::OnAttachedToWindow()
{
    // ==================before translated======================
    // super.onAttachedToWindow();
    // ActivityChooserModel dataModel = mAdapter.getDataModel();
    // if (dataModel != null) {
    //     dataModel.registerObserver(mModelDataSetOberver);
    // }
    // mIsAttachedToWindow = true;

    ViewGroup::OnAttachedToWindow();
    AutoPtr<IActivityChooserModel> dataModel;
    mAdapter->GetDataModel((IActivityChooserModel**)&dataModel);
    if (dataModel != NULL) {
        (IObservable::Probe(dataModel))->AddObserver(IObserver::Probe(mModelDataSetOberver));
    }
    mIsAttachedToWindow = TRUE;
    return NOERROR;
}

ECode ActivityChooserView::OnDetachedFromWindow()
{
    // ==================before translated======================
    // super.onDetachedFromWindow();
    // ActivityChooserModel dataModel = mAdapter.getDataModel();
    // if (dataModel != null) {
    //     dataModel.unregisterObserver(mModelDataSetOberver);
    // }
    // ViewTreeObserver viewTreeObserver = getViewTreeObserver();
    // if (viewTreeObserver.isAlive()) {
    //     viewTreeObserver.removeOnGlobalLayoutListener(mOnGlobalLayoutListener);
    // }
    // if (isShowingPopup()) {
    //     dismissPopup();
    // }
    // mIsAttachedToWindow = false;

    ViewGroup::OnDetachedFromWindow();
    AutoPtr<IActivityChooserModel> dataModel;
    mAdapter->GetDataModel((IActivityChooserModel**)&dataModel);
    if (dataModel != NULL) {
        (IObservable::Probe(dataModel))->DeleteObserver(IObserver::Probe(mModelDataSetOberver));
    }

    AutoPtr<IViewTreeObserver> viewTreeObserver;
    GetViewTreeObserver((IViewTreeObserver**)&viewTreeObserver);
    Boolean isAlive = FALSE;
    viewTreeObserver->IsAlive(&isAlive);
    if (isAlive) {
        viewTreeObserver->RemoveOnGlobalLayoutListener(mOnGlobalLayoutListener);
    }

    Boolean isShowingPopup = FALSE;
    IsShowingPopup(&isShowingPopup);
    if (isShowingPopup) {
        Boolean resTmp;
        DismissPopup(&resTmp);
    }

    mIsAttachedToWindow = FALSE;
    return NOERROR;
}

ECode ActivityChooserView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // View child = mActivityChooserContent;
    // // If the default action is not visible we want to be as tall as the
    // // ActionBar so if this widget is used in the latter it will look as
    // // a normal action button.
    // if (mDefaultActivityButton.getVisibility() != VISIBLE) {
    //     heightMeasureSpec = MeasureSpec.makeMeasureSpec(MeasureSpec.getSize(heightMeasureSpec),
    //             MeasureSpec.EXACTLY);
    // }
    // measureChild(child, widthMeasureSpec, heightMeasureSpec);
    // setMeasuredDimension(child.getMeasuredWidth(), child.getMeasuredHeight());

    AutoPtr<IView> child = mActivityChooserContent;
    Int32 visibility = 0;
    mDefaultActivityButton->GetVisibility(&visibility);
    if (visibility != VISIBLE) {
        heightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(View::MeasureSpec::GetSize(heightMeasureSpec),
            View::MeasureSpec::EXACTLY);
    }

    MeasureChild(child, widthMeasureSpec, heightMeasureSpec);
    Int32 measuredWidth = 0, measuredHeight = 0;
    child->GetMeasuredWidth(&measuredWidth);
    child->GetMeasuredHeight(&measuredHeight);
    SetMeasuredDimension(measuredWidth, measuredHeight);
    return NOERROR;
}

ECode ActivityChooserView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    // ==================before translated======================
    // mActivityChooserContent.layout(0, 0, right - left, bottom - top);
    // if (!isShowingPopup()) {
    //     dismissPopup();
    // }

    mActivityChooserContent->Layout(0, 0, right - left, bottom - top);
    Boolean isShowingPopup = FALSE;
    IsShowingPopup(&isShowingPopup);
    if (!isShowingPopup) {
        Boolean resTmp;
        DismissPopup(&resTmp);
    }
    return NOERROR;
}

ECode ActivityChooserView::ShowPopupUnchecked(
    /* [in] */ Int32 maxActivityCount)
{
    // ==================before translated======================
    // if (mAdapter.getDataModel() == null) {
    //     throw new IllegalStateException("No data model. Did you call #setDataModel?");
    // }
    //
    // getViewTreeObserver().addOnGlobalLayoutListener(mOnGlobalLayoutListener);
    //
    // final boolean defaultActivityButtonShown =
    //     mDefaultActivityButton.getVisibility() == VISIBLE;
    //
    // final int activityCount = mAdapter.getActivityCount();
    // final int maxActivityCountOffset = defaultActivityButtonShown ? 1 : 0;
    // if (maxActivityCount != ActivityChooserViewAdapter.MAX_ACTIVITY_COUNT_UNLIMITED
    //         && activityCount > maxActivityCount + maxActivityCountOffset) {
    //     mAdapter.setShowFooterView(true);
    //     mAdapter.setMaxActivityCount(maxActivityCount - 1);
    // } else {
    //     mAdapter.setShowFooterView(false);
    //     mAdapter.setMaxActivityCount(maxActivityCount);
    // }
    //
    // ListPopupWindow popupWindow = getListPopupWindow();
    // if (!popupWindow.isShowing()) {
    //     if (mIsSelectingDefaultActivity || !defaultActivityButtonShown) {
    //         mAdapter.setShowDefaultActivity(true, defaultActivityButtonShown);
    //     } else {
    //         mAdapter.setShowDefaultActivity(false, false);
    //     }
    //     final int contentWidth = Math.min(mAdapter.measureContentWidth(), mListPopupMaxWidth);
    //     popupWindow.setContentWidth(contentWidth);
    //     popupWindow.show();
    //     if (mProvider != null) {
    //         mProvider.subUiVisibilityChanged(true);
    //     }
    //     popupWindow.getListView().setContentDescription(mContext.getString(
    //             R.string.activitychooserview_choose_application));
    // }

    AutoPtr<IActivityChooserModel> dataModel;
    mAdapter->GetDataModel((IActivityChooserModel**)&dataModel);
    if (dataModel == NULL) {
        //throw new IllegalStateException("No data model. Did you call #setDataModel?");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IViewTreeObserver> viewTreeObserver;
    GetViewTreeObserver((IViewTreeObserver**)&viewTreeObserver);
    viewTreeObserver->AddOnGlobalLayoutListener(mOnGlobalLayoutListener);

    Int32 visibility = 0;
    mDefaultActivityButton->GetVisibility(&visibility);
    Boolean defaultActivityButtonShown = (visibility == VISIBLE);

    Int32 activityCount = 0;
    mAdapter->GetActivityCount(&activityCount);
    Int32 maxActivityCountOffset = defaultActivityButtonShown ? 1 : 0;
    if (maxActivityCount != ActivityChooserViewAdapter::MAX_ACTIVITY_COUNT_UNLIMITED
            && activityCount > maxActivityCount + maxActivityCountOffset) {
        mAdapter->SetShowFooterView(TRUE);
        mAdapter->SetMaxActivityCount(maxActivityCount - 1);
    }
    else {
        mAdapter->SetShowFooterView(FALSE);
        mAdapter->SetMaxActivityCount(maxActivityCount);
    }

    AutoPtr<ListPopupWindow> popupWindow = GetListPopupWindow();
    Boolean isShowing = FALSE;
    popupWindow->IsShowing(&isShowing);
    if (!isShowing) {
        if (mIsSelectingDefaultActivity || !defaultActivityButtonShown) {
            mAdapter->SetShowDefaultActivity(TRUE, defaultActivityButtonShown);
        }
        else {
            mAdapter->SetShowDefaultActivity(FALSE, FALSE);
        }

        Int32 measureWidth = 0;
        mAdapter->MeasureContentWidth(&measureWidth);
        Int32 contentWidth = Elastos::Core::Math::Min(measureWidth, mListPopupMaxWidth);
        popupWindow->SetContentWidth(contentWidth);
        popupWindow->Show();
        if (mProvider != NULL) {
            mProvider->SubUiVisibilityChanged(TRUE);
        }

        AutoPtr<IListView> listView;
        popupWindow->GetListView((IListView**)&listView);

        String str;
        mContext->GetString(R::string::activitychooserview_choose_application, &str);
        AutoPtr<ICharSequence> charSequence;
        CString::New(str, (ICharSequence**)&charSequence);
        (IView::Probe(listView))->SetContentDescription(charSequence);
    }
    return NOERROR;
}

AutoPtr<ListPopupWindow> ActivityChooserView::GetListPopupWindow()
{
    // ==================before translated======================
    // if (mListPopupWindow == null) {
    //     mListPopupWindow = new ListPopupWindow(getContext());
    //     mListPopupWindow.setAdapter(mAdapter);
    //     mListPopupWindow.setAnchorView(ActivityChooserView.this);
    //     mListPopupWindow.setModal(true);
    //     mListPopupWindow.setOnItemClickListener(mCallbacks);
    //     mListPopupWindow.setOnDismissListener(mCallbacks);
    // }
    // return mListPopupWindow;

    if (mListPopupWindow == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        mListPopupWindow = new ListPopupWindow();
        mListPopupWindow->constructor(context);
        mListPopupWindow->SetAdapter(mAdapter);
        mListPopupWindow->SetAnchorView(this);
        mListPopupWindow->SetModal(TRUE);
        mListPopupWindow->SetOnItemClickListener(mCallbacks);
        mListPopupWindow->SetOnDismissListener(mCallbacks);
    }
    return mListPopupWindow;
}

void ActivityChooserView::UpdateAppearance()
{
    // ==================before translated======================
    // // Expand overflow button.
    // if (mAdapter.getCount() > 0) {
    //     mExpandActivityOverflowButton.setEnabled(true);
    // } else {
    //     mExpandActivityOverflowButton.setEnabled(false);
    // }
    // // Default activity button.
    // final int activityCount = mAdapter.getActivityCount();
    // final int historySize = mAdapter.getHistorySize();
    // if (activityCount==1 || activityCount > 1 && historySize > 0) {
    //     mDefaultActivityButton.setVisibility(VISIBLE);
    //     ResolveInfo activity = mAdapter.getDefaultActivity();
    //     PackageManager packageManager = mContext.getPackageManager();
    //     mDefaultActivityButtonImage.setImageDrawable(activity.loadIcon(packageManager));
    //     if (mDefaultActionButtonContentDescription != 0) {
    //         CharSequence label = activity.loadLabel(packageManager);
    //         String contentDescription = mContext.getString(
    //                 mDefaultActionButtonContentDescription, label);
    //         mDefaultActivityButton.setContentDescription(contentDescription);
    //     }
    // } else {
    //     mDefaultActivityButton.setVisibility(View.GONE);
    // }
    // // Activity chooser content.
    // if (mDefaultActivityButton.getVisibility() == VISIBLE) {
    //     mActivityChooserContent.setBackground(mActivityChooserContentBackground);
    // } else {
    //     mActivityChooserContent.setBackground(null);
    // }

    Int32 count = 0;
    mAdapter->GetCount(&count);
    if (count > 0) {
        mExpandActivityOverflowButton->SetEnabled(TRUE);
    }
    else {
        mExpandActivityOverflowButton->SetEnabled(FALSE);
    }
    // Default activity button.
    Int32 activityCount = 0, historySize = 0;
    mAdapter->GetActivityCount(&activityCount);
    mAdapter->GetHistorySize(&historySize);
    if ((activityCount == 1 || activityCount > 1) && historySize > 0) {
        mDefaultActivityButton->SetVisibility(VISIBLE);
        AutoPtr<IResolveInfo> activity;
        mAdapter->GetDefaultActivity((IResolveInfo**)&activity);

        AutoPtr<IPackageManager> packageManager;
        mContext->GetPackageManager((IPackageManager**)&packageManager);

        AutoPtr<IDrawable> icon;
        activity->LoadIcon(packageManager, (IDrawable**)&icon);
        mDefaultActivityButtonImage->SetImageDrawable(icon);
        if (mDefaultActionButtonContentDescription != 0) {
            AutoPtr<ICharSequence> label;
            activity->LoadLabel(packageManager, (ICharSequence**)&label);

            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
            formatArgs->Set(0, TO_IINTERFACE(label));
            String contentDescription;
            mContext->GetString(mDefaultActionButtonContentDescription, formatArgs, &contentDescription);

            AutoPtr<ICharSequence> charSequence;
            CString::New(contentDescription, (ICharSequence**)&charSequence);
            mDefaultActivityButton->SetContentDescription(charSequence);
        }
    }
    else {
        mDefaultActivityButton->SetVisibility(IView::GONE);
    }
    // Activity chooser content.
    Int32 visibliity = 0;
    mDefaultActivityButton->GetVisibility(&visibliity);
    if (visibliity == VISIBLE) {
        mActivityChooserContent->SetBackground(mActivityChooserContentBackground);
    }
    else {
        mActivityChooserContent->SetBackground(NULL);
    }
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos


