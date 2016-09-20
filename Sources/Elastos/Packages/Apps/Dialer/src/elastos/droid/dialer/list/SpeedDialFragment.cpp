
#include "elastos/droid/dialer/list/SpeedDialFragment.h"
#include "elastos/utility/logging/Logger.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::ILoader;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::ILayoutAnimationController;
using Elastos::Droid::View::Animation::CLayoutAnimationController;
using Elastos::Droid::View::Animation::IViewGroupLayoutParams;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// SpeedDialFragment::InnerListener
//=================================================================
CAR_INTERFACE_IMPL(SpeedDialFragment::InnerListener, Object, IAdapterViewOnItemClickListener);

SpeedDialFragment::InnerListener::InnerListener(
    /* [in] */ SpeedDialFragment* host)
    : mHost(host)
{}


ECode SpeedDialFragment::InnerListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}

//=================================================================
// SpeedDialFragment::ContactTileLoaderListener
//=================================================================
CAR_INTERFACE_IMPL(SpeedDialFragment::ContactTileLoaderListener, Object, ILoaderManagerLoaderCallbacks);

SpeedDialFragment::ContactTileLoaderListener::ContactTileLoaderListener(
    /* [in] */ SpeedDialFragment* host)
    : mHost(host)
{}

ECode SpeedDialFragment::ContactTileLoaderListener::OnCreateLoader(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ ICursorLoader** loader)
{
    VALIDATE_NOT_NULL(loader);
    if (DEBUG) {
        Logger::D(TAG, "ContactTileLoaderListener#onCreateLoader.");
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    assert(0 && "TODO");
    // AutoPtr<ICursorLoader> cursorLoader;
    // ContactTileLoaderFactory::CreateStrequentPhoneOnlyLoader(
    //         activity, (ICursorLoader**)&cursorLoader);
    // *loader = cursorLoader;
    // REFCOUNT_ADD(*loader);
    return NOERROR;
}

ECode SpeedDialFragment::ContactTileLoaderListener::OnLoadFinished(
    /* [in] */ ILoader* loader,
    /* [in] */ ICursor* data)
{
    if (DEBUG) {
        Logger::D(TAG, "ContactTileLoaderListener#onLoadFinished");
    }
    mHost->mContactTileAdapter->SetContactCursor(data);
    Int32 count;
    mHost->mContactTileAdapter->GetCount(&count);
    mHost->SetEmptyViewVisibility(count == 0);
    return NOERROR;
}

ECode SpeedDialFragment::ContactTileLoaderListener::OnLoaderReset(
    /* [in] */ ILoader* loader)
{
    if (DEBUG) {
        Logger::D(TAG, "ContactTileLoaderListener#onLoaderReset. ");
    }
    return NOERROR;
}

//=================================================================
// SpeedDialFragment::ContactTileAdapterListener
//=================================================================
// TODO:
// CAR_INTERFACE_IMPL(SpeedDialFragment::ContactTileAdapterListener, Object, IContactTileViewListener)

// SpeedDialFragment::ContactTileAdapterListener::ContactTileAdapterListener(
//     /* [in] */ SpeedDialFragment* host)
//     : mHost(host)
// {}

// ECode SpeedDialFragment::ContactTileAdapterListener::OnContactSelected(
//     /* [in] */ IUri* contactUri,
//     /* [in] */ IRect* targetRect)
// {
//     if (mHost->mPhoneNumberPickerActionListener != NULL) {
//         mHost->mPhoneNumberPickerActionListener->OnPickPhoneNumberAction(contactUri);
//     }
//     return NOERROR;
// }

// ECode SpeedDialFragment::ContactTileAdapterListener::OnCallNumberDirectly(
//     /* [in] */ const String& phoneNumber)
// {
//     if (mHost->mPhoneNumberPickerActionListener != NULL) {
//         mHost->mPhoneNumberPickerActionListener->OnCallNumberDirectly(phoneNumber);
//     }
//     return NOERROR;
// }

// ECode SpeedDialFragment::ContactTileAdapterListener::GetApproximateTileWidth(
//     /* [out] */ Int32* width)
// {
//     VALIDATE_NOT_NULL(width);
//     AutoPtr<IView> view;
//     mHost->GetView((IView**)&view);
//     return view->GetWidth(width);
// }

//=================================================================
// SpeedDialFragment::ScrollListener
//=================================================================
CAR_INTERFACE_IMPL(SpeedDialFragment::ScrollListener, Object, IListViewOnScrollListener)

SpeedDialFragment::ScrollListener::ScrollListener(
    /* [in] */ SpeedDialFragment* host)
    : mHost(host)
{}

ECode SpeedDialFragment::ScrollListener::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    if (mHost->mActivityScrollListener != NULL) {
        mHost->mActivityScrollListener->OnListFragmentScroll(
                firstVisibleItem, visibleItemCount, totalItemCount);
    }
    return NOERROR;
}

ECode SpeedDialFragment::ScrollListener::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    return mHost->mActivityScrollListener->OnListFragmentScrollStateChange(scrollState);
}

//=================================================================
// SpeedDialFragment::MyViewTreeObserverOnPreDrawListener
//=================================================================
CAR_INTERFACE_IMPL(SpeedDialFragment::MyViewTreeObserverOnPreDrawListener,
            Object, IOnPreDrawListener)

SpeedDialFragment::MyViewTreeObserverOnPreDrawListener::MyViewTreeObserverOnPreDrawListener(
    /* [in] */ SpeedDialFragment* host,
    /* [in] */ IViewTreeObserver* observer,
    /* [in] */ ArrayOf<Int64>* idsInPlace)
    : mHost(host)
    , mObserver(observer)
    , mIdsInPlace(idsInPlace)
{}

ECode SpeedDialFragment::MyViewTreeObserverOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    mObserver->RemoveOnPreDrawListener(this);
    Int32 firstVisiblePosition;
    mHost->mListView->GetFirstVisiblePosition(&firstVisiblePosition);

    AutoPtr<IAnimatorSet> animSet;
    CAnimatorSet::New((IAnimatorSet**)&animSet);
    AutoPtr<IArrayList> animators;
    CArrayList::New((IArrayList**)&animators);

    Int32 count;
    mHost->mListView->GetChildCount(&count);
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        mListView->GetChildAt(i, (IView**)&child);
        Int32 position = firstVisiblePosition + i;

        Int64 itemId;
        mContactTileAdapter->GetItemId(position, &itemId);

        if (mHost->ContainsId(mIdsInPlace, itemId)) {
            AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
            values->Set(0, 0.0);
            values->Set(1, 1.0);
            AutoPtr<IObjectAnimator> animator;
            helper->OfFloat(child, String("alpha"), values, (IObjectAnimator**)&animator);
            animators->Add(animator);
            break;
        }
        else {
            AutoPtr<IInterface> startTop;
            mItemIdTopMap->Get(itemId, (IInterface**)&startTop);
            AutoPtr<IInterface> startLeft;
            mItemIdLeftMap->Get(itemId, (IInterface**)&startLeft);
            Int32 top;
            child->GetTop(&top);
            Int32 left;
            child->GetLeft(&left);
            Int32 deltaX = 0;
            Int32 deltaY = 0;

            if (startLeft != NULL) {
                if (CoreUtils::Unbox(IInteger32::Probe(startLeft)) != left) {
                    deltaX = CoreUtils::Unbox(startLeft) - left;
                    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
                    values->Set(0, deltaX);
                    values->Set(1, 0.0);
                    AutoPtr<IObjectAnimator> animator;
                    helper->OfFloat(child, String("translationX"),
                            values, (IObjectAnimator**)&animator);
                    animators->Add(animator);
                }
            }

            if (startTop != NULL) {
                if (CoreUtils::Unbox(startTop) != top) {
                    deltaY = CoreUtils::Unbox(startTop) - top;
                    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
                    values->Set(0, deltaY);
                    values->Set(1, 0.0f);
                    AutoPtr<IObjectAnimator> animator;
                    helper->OfFloat(child, String("translationY"),
                            values, (IObjectAnimator**)&animator);
                    animators->Add(animator);
                }
            }

            if (DEBUG) {
                Logger::D(TAG, "Found itemId: %d  for listview child %d Top: %d Delta: ",
                        itemId, i, top, deltaY);
            }
        }
    }

    Int32 size;
    if (animators->GetSize(&size), size > 0) {
        IAnimator::Probe(animSet)->SetDuration(mAnimationDuration)
        animSet->PlayTogether(animators);
        IAnimator::Probe(animSet)->Start();
    }

    mHost->mItemIdTopMap->Clear();
    mHost->mItemIdLeftMap->Clear();
    *result = TRUE;
    return NOERROR;
}

//=================================================================
// SpeedDialFragment
//=================================================================
const Int64 KEY_REMOVED_ITEM_HEIGHT; // = Long.MAX_VALUE;

const String SpeedDialFragment::TAG("SpeedDialFragment");
const Boolean SpeedDialFragment::DEBUG = FALSE;
Int32 SpeedDialFragment::LOADER_ID_CONTACT_TILE = 1;

// TODO:
CAR_INTERFACE_IMPL_2(SpeedDialFragment, /*AnalyticsFragment*/Fragment, ISpeedDialFragment,
    IOnDataSetChangedForAnimationListener)

SpeedDialFragment::SpeedDialFragment()
{
}

ECode SpeedDialFragment::OnAttach(
    /* [in] */ IActivity* activity)
{
    if (DEBUG) {
        Logger::D(TAG, "onAttach()");
    }
    assert(0 && "TODO");
    // AnalyticsFragment::OnAttach(activity);

    // Construct two base adapters which will become part of PhoneFavoriteMergedAdapter.
    // We don't construct the resultant adapter at this moment since it requires LayoutInflater
    // that will be available on onCreateView().
    assert(0 && "TODO");
    // CPhoneFavoritesTileAdapter::New(activity, mContactTileAdapterListener,
    //         this, (IPhoneFavoritesTileAdapter**)&mContactTileAdapter);
    // mContactTileAdapter->SetPhotoLoader(ContactPhotoManager::GetInstance(activity));
    return NOERROR;
}

ECode SpeedDialFragment::OnCreate(
    /* [in] */ IBundle* savedState)
{
    if (DEBUG) {
        Logger::D(TAG, "onCreate()");
    }
    assert(0 && "TODO");
    // AnalyticsFragment::OnCreate(savedState);

    CHashMap::New((IHashMap**)&mItemIdTopMap);
    CHashMap::New((IHashMap**)&mItemIdLeftMap);

    assert(0 && "TODO");
    // mContactTileAdapterListener = (IContactTileViewListener*)new ContactTileAdapterListener(this);
    mContactTileLoaderListener = (ILoaderManagerLoaderCallbacks*)new ContactTileLoaderListener(this);
    mScrollListener = new ScrollListener(this);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetInteger(R::integer::fade_duration, &mAnimationDuration);
    return NOERROR;
}

ECode SpeedDialFragment::OnResume()
{
    // assert(0 && "TODO");
    // AnalyticsFragment::OnResume();

    AutoPtr<ILoaderManager> manager;
    GetLoaderManager((ILoaderManager**)&manager);
    AutoPtr<ILoader> loader;
    manager->GetLoader(LOADER_ID_CONTACT_TILE, (ILoader**)&loader);
    loader->ForceLoad();
    return NOERROR;
}

ECode SpeedDialFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    inflater->Inflate(R::layout::speed_dial_fragment,
            container, FALSE, (IView**)&mParentView);

    AutoPtr<InnerListener> listener = new InnerListener(this);

    AutoPtr<IView> listView;
    mParentView->FindViewById(R::id::contact_tile_list, (IView**)&listView);
    mListView = IPhoneFavoriteListView::Probe(listView);
    mListView->SetOnItemClickListener(listener);
    mListView->SetVerticalScrollBarEnabled(FALSE);
    mListView->SetVerticalScrollbarPosition(IView::SCROLLBAR_POSITION_RIGHT);
    mListView->SetScrollBarStyle(IListView::SCROLLBARS_OUTSIDE_OVERLAY);
    AutoPtr<IDragDropController> controller;
    mListView->GetDragDropController((IDragDropController**)&controller);
    controller->AddOnDragDropListener(mContactTileAdapter);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IView> dragShadowOverlay;
    activity->FindViewById(R::id::contact_tile_drag_shadow_overlay, (IView**)&dragShadowOverlay);
    mListView->SetDragShadowOverlay(IImageView::Probe(dragShadowOverlay));

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    mParentView->FindViewById(R::id::empty_list_view, (IView**)&mEmptyView);
    DialerUtils::ConfigureEmptyListView(
            mEmptyView, R::drawable::empty_speed_dial,
            R::string::speed_dial_empty, resources);

    mParentView->FindViewById(R::id::contact_tile_frame, (IView**)&mContactTileFrame);

    AutoPtr<IView> teaserView;
    inflater->Inflate(R::layout::tile_interactions_teaser_view,
            mListView, FALSE, (IView**)&teaserView);
    mTileInteractionTeaserView = ITileInteractionTeaserView::Probe(teaserView);

    AutoPtr<IAnimationUtils> utils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
    AutoPtr<IAnimation> animation;
    utils->LoadAnimation(IContext::Probe(activity),
            Elastos::Droid::R::anim.fade_in, (IAnimation**)&animation);
    AutoPtr<ILayoutAnimationController> controller;
    CLayoutAnimationController::New(animation, (ILayoutAnimationController**)&controller);
    controller->SetDelay(0);
    mListView->SetLayoutAnimation(controller);
    mListView->SetAdapter(mContactTileAdapter);

    mListView->SetOnScrollListener(mScrollListener);
    mListView->SetFastScrollEnabled(FALSE);
    mListView->SetFastScrollAlwaysVisible(FALSE);

    *view = mParentView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode SpeedDialFragment::HasFrequents(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mContactTileAdapter == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 num;
    mContactTileAdapter->GetNumFrequents(&num);
    *result = num > 0;
    return NOERROR;
}

void SpeedDialFragment::SetEmptyViewVisibility(
    /* [in] */ Boolean visible)
{
    Int32 previousVisibility;
    mEmptyView->GetVisibility(&previousVisibility);
    Int32 newVisibility = visible ? IView::VISIBLE : IView::GONE;

    if (previousVisibility != newVisibility) {
        AutoPtr<IViewGroupLayoutParams> params;
        mContactTileFrame->GetLayoutParams((IViewGroupLayoutParams**)&params);
        params->SetHeight(visible ?
                IViewGroupLayoutParams::WRAP_CONTENT : IViewGroupLayoutParams::MATCH_PARENT);
        mContactTileFrame->SetLayoutParams(params);
        mEmptyView->SetVisibility(newVisibility);
    }
}

ECode SpeedDialFragment::OnStart()
{
    assert(0 && "TODO");
    // AnalyticsFragment::OnStart();

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    // try {
    mActivityScrollListener = IOnListFragmentScrolledListener::Probe(activity);
    if (mActivityScrollListener == NULL) {
        String str;
        IObject::Probe(activity)->ToString(&str);
        Logger::E(TAG, "%s must implement OnListFragmentScrolledListener", str.string());
        return E_CLASS_CAST_EXCEPTION;
    }
    // } catch (ClassCastException e) {
    //     throw new ClassCastException(activity.toString()
    //             + " must implement OnListFragmentScrolledListener");
    // }

    // try {
    IOnDragDropListener* listener = IOnDragDropListener::Probe(activity);
    if (listener == NULL) {
        String str;
        activity->ToString(&str);
        Logger::E(TAG, "%s must implement OnDragDropListener", str.string());
        return E_CLASS_CAST_EXCEPTION;
    }
    AutoPtr<IDragDropController> controller;
    mListView->GetDragDropController((IDragDropController**)&controller);
    controller->AddOnDragDropListener(listener);

    IHostInterface* hostInterface = IHostInterface::Probe(activity);
    if (hostInterface == NULL) {
        String str;
        IObject::Probe(activity)->ToString(&str);
        Logger::E(TAG, "%s must implement HostInterface", str.string());
        return E_CLASS_CAST_EXCEPTION;
    }
    hostInterface->SetDragDropController(controller);
    // } catch (ClassCastException e) {
    //     throw new ClassCastException(activity.toString()
    //             + " must implement OnDragDropListener and HostInterface");
    // }

    // try {
    assert(0 && "TODO");
    // mPhoneNumberPickerActionListener = IOnPhoneNumberPickerActionListener::Probe(activity);
    // if (mPhoneNumberPickerActionListener == NULL) {
    //     String str;
    //     activity->ToString(&str);
    //     Logger::E(TAG, "%s must implement PhoneFavoritesFragment.listener", str.string());
    //     return E_CLASS_CAST_EXCEPTION;
    // }
    // } catch (ClassCastException e) {
    //     throw new ClassCastException(activity.toString()
    //             + " must implement PhoneFavoritesFragment.listener");
    // }

    // Use initLoader() instead of restartLoader() to refraining unnecessary reload.
    // This method call implicitly assures ContactTileLoaderListener's onLoadFinished() will
    // be called, on which we'll check if "all" contacts should be reloaded again or not.
    AutoPtr<ILoaderManager> manager;
    GetLoaderManager((ILoaderManager**)&manager);
    manager->InitLoader(LOADER_ID_CONTACT_TILE, NULL, mContactTileLoaderListener);

    return NOERROR;
}

ECode SpeedDialFragment::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Int32 contactTileAdapterCount;
    mContactTileAdapter->GetCount(&contactTileAdapterCount);
    if (position <= contactTileAdapterCount) {
        Logger::E(TAG, "onItemClick() event for unexpected position. "
                "The position %d is before \"all\" section. Ignored.", position);
    }
    return NOERROR;
}

void SpeedDialFragment::SaveOffsets(
    /* [in] */ Int32 removedItemHeight)
{
    Int32 firstVisiblePosition;
    mListView->GetFirstVisiblePosition(&firstVisiblePosition);
    Int32 count;
    mListView->GetChildCount(&count);
    if (DEBUG) {
        Logger::D(TAG, "Child count : %d", count);
    }
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        mListView->GetChildAt(i, &child);
        Int32 position = firstVisiblePosition + i;
        Int64 itemId;
        mContactTileAdapter->GetItemId(position, &itemId);
        Int32 top, left;
        child->GetTop(&top);
        child->GetLeft(&left);
        if (DEBUG) {
            Logger::D(TAG, "Saving itemId: %d for listview child %d Top: ", itemId, i, top);
        }
        mItemIdTopMap->Put(CoreUtils::Convert(itemId), CoreUtils::Convert(top));
        mItemIdLeftMap->Put(CoreUtils::Convert(itemId), CoreUtils::Convert(left));
    }

    mItemIdTopMap->Put(CoreUtils::Convert(KEY_REMOVED_ITEM_HEIGHT),
            CoreUtils::Convert(removedItemHeight));
}

void SpeedDialFragment::AnimateGridView(
    /* [in] */ ArrayOf<Int64>* idsInPlace)
{
    Boolean result;
    if (mItemIdTopMap->IsEmpty(&result), result) {
        // Don't do animations if the database is being queried for the first time and
        // the previous item offsets have not been cached, or the user hasn't done anything
        // (dragging, swiping etc) that requires an animation.
        return;
    }

    AutoPtr<IViewTreeObserver> observer;
    mListView->GetViewTreeObserver((IViewTreeObserver**)observer);
    AutoPtr<MyViewTreeObserverOnPreDrawListener> listener =
            new MyViewTreeObserverOnPreDrawListener(this, observer, idsInPlace);
    observer->AddOnPreDrawListener((IOnPreDrawListener*)listener);
}

Boolean SpeedDialFragment::ContainsId(
    /* [in] */ ArrayOf<Int64>* ids,
    /* [in] */ Int64 target)
{
    // Linear search on array is fine because this is typically only 0-1 elements long
    for (Int32 i = 0; i < ids->GetLength(); i++) {
        if (ids[i] == target) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode SpeedDialFragment::OnDataSetChangedForAnimation(
    /* [in] */ ArrayOf<Int64>* idsInPlace)
{
    AnimateGridView(idsInPlace);
    return NOERROR;
}

ECode SpeedDialFragment::CacheOffsetsForDatasetChange()
{
    SaveOffsets(0);
    return NOERROR;
}

ECode SpeedDialFragment::GetListView(
    /* [out] */ IAbsListView** listView)
{
    VALIDATE_NOT_NULL(listview);
    *listView = mListView;
    REFCOUNT_ADD(*listView);
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
