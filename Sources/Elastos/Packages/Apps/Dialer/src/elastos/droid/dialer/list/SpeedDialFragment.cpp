
#include "elastos/droid/dialer/list/SpeedDialFragment.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
#include "elastos/droid/contacts/common/ContactTileLoaderFactory.h"
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/core/Math.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::App::EIID_ILoaderManagerLoaderCallbacks;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::Contacts::Common::ContactTileLoaderFactory;
using Elastos::Droid::Contacts::Common::ContactPhotoManager;
using Elastos::Droid::Contacts::Common::List::EIID_IContactTileViewListener;
using Elastos::Droid::Dialer::List::EIID_ISpeedDialFragment;
using Elastos::Droid::Dialer::List::EIID_IOnDataSetChangedForAnimationListener;
using Elastos::Droid::Dialer::Util::DialerUtils;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::ILayoutAnimationController;
using Elastos::Droid::View::Animation::CLayoutAnimationController;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::EIID_IAbsListViewOnScrollListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Core::CInteger32;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// SpeedDialFragment::InnerOnItemClickListener
//=================================================================
CAR_INTERFACE_IMPL(SpeedDialFragment::InnerOnItemClickListener, Object, IAdapterViewOnItemClickListener);

ECode SpeedDialFragment::InnerOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}


//=================================================================
// SpeedDialFragment::InnerOnDataSetChangedListener
//=================================================================
CAR_INTERFACE_IMPL(SpeedDialFragment::InnerOnDataSetChangedListener, Object, IOnDataSetChangedForAnimationListener);

ECode SpeedDialFragment::InnerOnDataSetChangedListener::OnDataSetChangedForAnimation(
    /* [in] */ ArrayOf<Int64>* idsInPlace)
{
    return mHost->OnDataSetChangedForAnimation(idsInPlace);
}

ECode SpeedDialFragment::InnerOnDataSetChangedListener::CacheOffsetsForDatasetChange()
{
    return mHost->CacheOffsetsForDatasetChange();
}


//=================================================================
// SpeedDialFragment::ContactTileLoaderListener
//=================================================================
CAR_INTERFACE_IMPL(SpeedDialFragment::ContactTileLoaderListener, Object, ILoaderManagerLoaderCallbacks);

ECode SpeedDialFragment::ContactTileLoaderListener::OnCreateLoader(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ ILoader** loader)
{
    VALIDATE_NOT_NULL(loader)
    if (DEBUG) Logger::D(TAG, "ContactTileLoaderListener#onCreateLoader.");
    AutoPtr<IActivity> a;
    mHost->GetActivity((IActivity**)&a);
    *loader = ILoader::Probe(ContactTileLoaderFactory::CreateStrequentPhoneOnlyLoader(IContext::Probe(a)));
    REFCOUNT_ADD(*loader)
    return NOERROR;
}

ECode SpeedDialFragment::ContactTileLoaderListener::OnLoadFinished(
    /* [in] */ ILoader* loader,
    /* [in] */ IInterface* data)
{
    if (DEBUG) {
        Logger::D(TAG, "ContactTileLoaderListener#onLoadFinished");
    }
    mHost->mContactTileAdapter->SetContactCursor(ICursor::Probe(data));
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

// =================================================================
// SpeedDialFragment::ContactTileAdapterListener
// =================================================================
CAR_INTERFACE_IMPL(SpeedDialFragment::ContactTileAdapterListener, Object, IContactTileViewListener)

ECode SpeedDialFragment::ContactTileAdapterListener::OnContactSelected(
    /* [in] */ IUri* contactUri,
    /* [in] */ IRect* targetRect)
{
    if (mHost->mPhoneNumberPickerActionListener != NULL) {
        mHost->mPhoneNumberPickerActionListener->OnPickPhoneNumberAction(contactUri);
    }
    return NOERROR;
}

ECode SpeedDialFragment::ContactTileAdapterListener::OnCallNumberDirectly(
    /* [in] */ const String& phoneNumber)
{
    if (mHost->mPhoneNumberPickerActionListener != NULL) {
        mHost->mPhoneNumberPickerActionListener->OnCallNumberDirectly(phoneNumber);
    }
    return NOERROR;
}

ECode SpeedDialFragment::ContactTileAdapterListener::GetApproximateTileWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    AutoPtr<IView> view;
    mHost->GetView((IView**)&view);
    return view->GetWidth(width);
}


//=================================================================
// SpeedDialFragment::ScrollListener
//=================================================================
CAR_INTERFACE_IMPL(SpeedDialFragment::ScrollListener, Object, IAbsListViewOnScrollListener)

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
CAR_INTERFACE_IMPL(SpeedDialFragment::MyViewTreeObserverOnPreDrawListener, Object, IOnPreDrawListener)

ECode SpeedDialFragment::MyViewTreeObserverOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mObserver->RemoveOnPreDrawListener(this);
    Int32 firstVisiblePosition;
    IAdapterView::Probe(mHost->mListView)->GetFirstVisiblePosition(&firstVisiblePosition);

    AutoPtr<IAnimatorSet> animSet;
    CAnimatorSet::New((IAnimatorSet**)&animSet);
    Elastos::Utility::Etl::List<AutoPtr<IAnimator> > animators;
    Int32 childCount;
    AutoPtr<IViewGroup> listView = IViewGroup::Probe(mHost->mListView);
    listView->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        listView->GetChildAt(i, (IView**)&child);
        Int32 position = firstVisiblePosition + i;

        Int64 itemId;
        IAdapter::Probe(mHost->mContactTileAdapter)->GetItemId(position, &itemId);

        if (mHost->ContainsId(mIdsInPlace, itemId)) {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
            (*values)[0] = 0.0;
            (*values)[1] = 1.0;
            AutoPtr<IObjectAnimator> animator;
            helper->OfFloat(child, String("alpha"), values, (IObjectAnimator**)&animator);
            animators.PushBack(IAnimator::Probe(animator));
            break;
        }
        else {
            AutoPtr<IInteger32> startTop;
            HashMap<Int64, AutoPtr<IInteger32> >::Iterator topIt = mHost->mItemIdTopMap.Find(itemId);
            if (topIt != mHost->mItemIdTopMap.End()) {
                startTop = topIt->mSecond;
            }
            AutoPtr<IInteger32> startLeft;
            HashMap<Int64, AutoPtr<IInteger32> >::Iterator leftIt = mHost->mItemIdLeftMap.Find(itemId);
            if (leftIt != mHost->mItemIdLeftMap.End()) {
                startLeft = leftIt->mSecond;
            }
            Int32 top;
            child->GetTop(&top);
            Int32 left;
            child->GetLeft(&left);
            Int32 deltaX = 0;
            Int32 deltaY = 0;

            if (startLeft != NULL) {
                Int32 leftValue;
                startLeft->GetValue(&leftValue);
                if (leftValue != left) {
                    deltaX = leftValue - left;
                    AutoPtr<IObjectAnimatorHelper> helper;
                    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
                    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
                    (*values)[0] = (Float)deltaX;
                    (*values)[1] = 0.0;
                    AutoPtr<IObjectAnimator> animator;
                    helper->OfFloat(child, String("translationX"), values, (IObjectAnimator**)&animator);
                    animators.PushBack(IAnimator::Probe(animator));
                }
            }

            if (startTop != NULL) {
                Int32 topValue;
                startTop->GetValue(&topValue);
                if (topValue != top) {
                    deltaY = topValue - top;
                    AutoPtr<IObjectAnimatorHelper> helper;
                    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
                    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
                    (*values)[0] = (Float)deltaY;
                    (*values)[1] = 0.0;
                    AutoPtr<IObjectAnimator> animator;
                    helper->OfFloat(child, String("translationY"), values, (IObjectAnimator**)&animator);
                    animators.PushBack(IAnimator::Probe(animator));
                }
            }

            if (DEBUG) {
                Logger::D(TAG, "Found itemId: %d for listview child %d Top: %d Delta: %f",
                        itemId, i, top, deltaY);
            }
        }
    }

    if (animators.Begin() != animators.End()) {
        IAnimator::Probe(animSet)->SetDuration(mHost->mAnimationDuration);
        AutoPtr<ArrayOf<IAnimator*> > attrs = ArrayOf<IAnimator*>::Alloc(animators.GetSize());
        Elastos::Utility::Etl::List<AutoPtr<IAnimator> >::Iterator it = animators.Begin();
        for (Int32 i = 0; it != animators.End(); ++it, ++i) {
            attrs->Set(i, *it);
        }
        animSet->PlayTogether(attrs);
        IAnimator::Probe(animSet)->Start();
    }

    mHost->mItemIdTopMap.Clear();
    mHost->mItemIdLeftMap.Clear();
    *result = TRUE;
    return NOERROR;
}

//=================================================================
// SpeedDialFragment
//=================================================================
const Int64 SpeedDialFragment::KEY_REMOVED_ITEM_HEIGHT = Elastos::Core::Math::INT64_MAX_VALUE;
const String SpeedDialFragment::TAG("SpeedDialFragment");
const Boolean SpeedDialFragment::DEBUG;
const Int32 SpeedDialFragment::LOADER_ID_CONTACT_TILE;

CAR_INTERFACE_IMPL(SpeedDialFragment, AnalyticsFragment, ISpeedDialFragment)

SpeedDialFragment::SpeedDialFragment()
{
    CHashMap::New((IHashMap**)&mItemIdTopMap);
    CHashMap::New((IHashMap**)&mItemIdLeftMap);
    mContactTileAdapterListener = (IContactTileViewListener*)new ContactTileAdapterListener(this);
    mContactTileLoaderListener = (ILoaderManagerLoaderCallbacks*)new ContactTileLoaderListener(this);
    mScrollListener = new ScrollListener(this);
}

ECode SpeedDialFragment::constructor()
{
    return AnalyticsFragment::constructor();
}

ECode SpeedDialFragment::OnAttach(
    /* [in] */ IActivity* activity)
{
    if (DEBUG) {
        Logger::D(TAG, "onAttach()");
    }
    FAIL_RETURN(AnalyticsFragment::OnAttach(activity))

    // Construct two base adapters which will become part of PhoneFavoriteMergedAdapter.
    // We don't construct the resultant adapter at this moment since it requires LayoutInflater
    // that will be available on onCreateView().
    AutoPtr<IOnDataSetChangedForAnimationListener> listener = new InnerOnDataSetChangedListener(this);
    mContactTileAdapter = new PhoneFavoritesTileAdapter(IContext::Probe(activity), mContactTileAdapterListener,
            listener);
    AutoPtr<IContactPhotoManager> cpm = ContactPhotoManager::GetInstance(IContext::Probe(activity));
    mContactTileAdapter->SetPhotoLoader(cpm);
    return NOERROR;
}

ECode SpeedDialFragment::OnCreate(
    /* [in] */ IBundle* savedState)
{
    if (DEBUG) {
        Logger::D(TAG, "onCreate()");
    }
    FAIL_RETURN(AnalyticsFragment::OnCreate(savedState))

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetInteger(R::integer::fade_duration, &mAnimationDuration);
    return NOERROR;
}

ECode SpeedDialFragment::OnResume()
{
    FAIL_RETURN(AnalyticsFragment::OnResume())
    // TODO
    // AutoPtr<ILoaderManager> manager;
    // GetLoaderManager((ILoaderManager**)&manager);
    // AutoPtr<ILoader> loader;
    // manager->GetLoader(LOADER_ID_CONTACT_TILE, (ILoader**)&loader);
    // loader->ForceLoad();
    return NOERROR;
}

ECode SpeedDialFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    mParentView = NULL;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::speed_dial_fragment,
            container, FALSE, (IView**)&mParentView);

    AutoPtr<InnerOnItemClickListener> listener = new InnerOnItemClickListener(this);

    AutoPtr<IView> listView;
    mParentView->FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_list, (IView**)&listView);
    mListView = IPhoneFavoriteListView::Probe(listView);
    AutoPtr<IAdapterView> av = IAdapterView::Probe(mListView);
    av->SetOnItemClickListener(listener);
    AutoPtr<IView> lv = IView::Probe(mListView);
    lv->SetVerticalScrollBarEnabled(FALSE);
    lv->SetVerticalScrollbarPosition(IView::SCROLLBAR_POSITION_RIGHT);
    lv->SetScrollBarStyle(IView::SCROLLBARS_OUTSIDE_OVERLAY);
    AutoPtr<IDragDropController> controller;
    mListView->GetDragDropController((IDragDropController**)&controller);
    controller->AddOnDragDropListener(mContactTileAdapter);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IView> dragShadowOverlay;
    activity->FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_drag_shadow_overlay, (IView**)&dragShadowOverlay);
    mListView->SetDragShadowOverlay(IImageView::Probe(dragShadowOverlay));

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    mEmptyView = NULL;
    mParentView->FindViewById(Elastos::Droid::Dialer::R::id::empty_list_view, (IView**)&mEmptyView);
    DialerUtils::ConfigureEmptyListView(mEmptyView, Elastos::Droid::Dialer::R::drawable::empty_speed_dial,
            Elastos::Droid::Dialer::R::string::speed_dial_empty, resources);

    mContactTileFrame = NULL;
    mParentView->FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_frame, (IView**)&mContactTileFrame);

    AutoPtr<IView> teaserView;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::tile_interactions_teaser_view,
            IViewGroup::Probe(mListView), FALSE, (IView**)&teaserView);
    mTileInteractionTeaserView = ITileInteractionTeaserView::Probe(teaserView);

    AutoPtr<IAnimationUtils> utils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
    AutoPtr<IAnimation> animation;
    utils->LoadAnimation(IContext::Probe(activity),
            Elastos::Droid::R::anim::fade_in, (IAnimation**)&animation);
    AutoPtr<ILayoutAnimationController> animationController;
    CLayoutAnimationController::New(animation, (ILayoutAnimationController**)&animationController);
    animationController->SetDelay(0);
    IViewGroup::Probe(mListView)->SetLayoutAnimation(animationController);
    av->SetAdapter(IAdapter::Probe(mContactTileAdapter));

    AutoPtr<IAbsListView> absLV = IAbsListView::Probe(mListView);
    absLV->SetOnScrollListener(mScrollListener);
    absLV->SetFastScrollEnabled(FALSE);
    absLV->SetFastScrollAlwaysVisible(FALSE);

    *view = mParentView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode SpeedDialFragment::HasFrequents(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mContactTileAdapter == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    *result = mContactTileAdapter->GetNumFrequents() > 0;
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
    FAIL_RETURN(AnalyticsFragment::OnStart())

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    // try {
    mActivityScrollListener = IOnListFragmentScrolledListener::Probe(activity);
    if (mActivityScrollListener == NULL) {
        Logger::E(TAG, "%s must implement OnListFragmentScrolledListener", TO_CSTR(activity));
        return E_CLASS_CAST_EXCEPTION;
    }
    // } catch (ClassCastException e) {
    //     throw new ClassCastException(activity.toString()
    //             + " must implement OnListFragmentScrolledListener");
    // }

    // try {
    AutoPtr<IOnDragDropListener> listener = IOnDragDropListener::Probe(activity);
    if (listener == NULL) {
        Logger::E(TAG, "%s must implement OnDragDropListener", TO_CSTR(activity));
        return E_CLASS_CAST_EXCEPTION;
    }
    AutoPtr<IDragDropController> controller;
    mListView->GetDragDropController((IDragDropController**)&controller);
    controller->AddOnDragDropListener(listener);
    AutoPtr<ISpeedDialFragmentHostInterface> hostInterface = ISpeedDialFragmentHostInterface::Probe(activity);
    if (hostInterface == NULL) {
        Logger::E(TAG, "%s must implement HostInterface", TO_CSTR(activity));
        return E_CLASS_CAST_EXCEPTION;
    }
    hostInterface->SetDragDropController(controller);
    // } catch (ClassCastException e) {
    //     throw new ClassCastException(activity.toString()
    //             + " must implement OnDragDropListener and HostInterface");
    // }

    // try {
    mPhoneNumberPickerActionListener = IOnPhoneNumberPickerActionListener::Probe(activity);
    if (mPhoneNumberPickerActionListener == NULL) {
        Logger::E(TAG, "%s must implement PhoneFavoritesFragment.listener", TO_CSTR(activity));
        return E_CLASS_CAST_EXCEPTION;
    }
    // } catch (ClassCastException e) {
    //     throw new ClassCastException(activity.toString()
    //             + " must implement PhoneFavoritesFragment.listener");
    // }

    // Use initLoader() instead of restartLoader() to refraining unnecessary reload.
    // This method call implicitly assures ContactTileLoaderListener's onLoadFinished() will
    // be called, on which we'll check if "all" contacts should be reloaded again or not.
    AutoPtr<ILoaderManager> manager;
    GetLoaderManager((ILoaderManager**)&manager);
    AutoPtr<ILoader> l;
    manager->InitLoader(LOADER_ID_CONTACT_TILE, NULL, mContactTileLoaderListener, (ILoader**)&l);

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
    IAdapterView::Probe(mListView)->GetFirstVisiblePosition(&firstVisiblePosition);
    AutoPtr<IViewGroup> listView = IViewGroup::Probe(mListView);
    Int32 count;
    listView->GetChildCount(&count);
    if (DEBUG) {
        Logger::D(TAG, "Child count : %d", count);
    }
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        listView->GetChildAt(i, (IView**)&child);
        Int32 position = firstVisiblePosition + i;
        Int64 itemId;
        mContactTileAdapter->GetItemId(position, &itemId);
        Int32 top, left;
        child->GetTop(&top);
        child->GetLeft(&left);
        if (DEBUG) {
            Logger::D(TAG, "Saving itemId: %d for listview child %d Top: ", itemId, i, top);
        }
        AutoPtr<IInteger32> integerT;
        CInteger32::New(top, (IInteger32**)&integerT);
        mItemIdTopMap[itemId] = integerT;
        AutoPtr<IInteger32> integerL;
        CInteger32::New(left, (IInteger32**)&integerL);
        mItemIdLeftMap[itemId] = integerL;
    }

    AutoPtr<IInteger32> integer;
    CInteger32::New(removedItemHeight, (IInteger32**)&integer);
    mItemIdTopMap[KEY_REMOVED_ITEM_HEIGHT] = integer;
}

void SpeedDialFragment::AnimateGridView(
    /* [in] */ ArrayOf<Int64>* idsInPlace)
{
    if (mItemIdTopMap.Begin() == mItemIdTopMap.End()) {
        // Don't do animations if the database is being queried for the first time and
        // the previous item offsets have not been cached, or the user hasn't done anything
        // (dragging, swiping etc) that requires an animation.
        return;
    }

    AutoPtr<IViewTreeObserver> observer;
    IView::Probe(mListView)->GetViewTreeObserver((IViewTreeObserver**)&observer);
    AutoPtr<IOnPreDrawListener> listener =
            (IOnPreDrawListener*)new MyViewTreeObserverOnPreDrawListener(this, observer, idsInPlace);
    observer->AddOnPreDrawListener(listener);
}

Boolean SpeedDialFragment::ContainsId(
    /* [in] */ ArrayOf<Int64>* ids,
    /* [in] */ Int64 target)
{
    // Linear search on array is fine because this is typically only 0-1 elements long
    for (Int32 i = 0; i < ids->GetLength(); i++) {
        if ((*ids)[i] == target) {
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
    VALIDATE_NOT_NULL(listView)
    *listView = IAbsListView::Probe(mListView);
    REFCOUNT_ADD(*listView)
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
