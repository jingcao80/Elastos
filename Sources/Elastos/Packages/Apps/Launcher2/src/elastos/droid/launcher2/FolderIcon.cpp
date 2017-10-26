//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/launcher2/FolderIcon.h"
#include "elastos/droid/launcher2/Launcher.h"
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/launcher2/DropTarget.h"
#include "elastos/droid/launcher2/ApplicationInfo.h"
#include "elastos/droid/launcher2/Folder.h"
#include "elastos/droid/launcher2/FolderInfo.h"
#include "elastos/droid/launcher2/CellLayout.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::IFloat;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(FolderIcon::FolderListener, Object, IFolderListener)

FolderIcon::FolderListener::FolderListener(
    /* [in] */ FolderIcon* host)
    : mHost(host)
{}

ECode FolderIcon::FolderListener::OnItemsChanged()
{
    return mHost->OnItemsChanged();
}

ECode FolderIcon::FolderListener::OnAdd(
    /* [in] */ IShortcutInfo* item)
{
    return mHost->OnAdd(item);
}

ECode FolderIcon::FolderListener::OnRemove(
    /* [in] */ IShortcutInfo* item)
{
    return mHost->OnRemove(item);
}

ECode FolderIcon::FolderListener::OnTitleChanged(
    /* [in] */ ICharSequence* title)
{
    return mHost->OnTitleChanged(title);
}

//=============================================================
// FolderIcon::FolderRingAnimator::AcceptAnimatorUpdateListener
//=============================================================
CAR_INTERFACE_IMPL(FolderIcon::FolderRingAnimator::AcceptAnimatorUpdateListener, Object,
        IAnimatorUpdateListener);

FolderIcon::FolderRingAnimator::AcceptAnimatorUpdateListener::AcceptAnimatorUpdateListener(
    /* [in] */ FolderIcon::FolderRingAnimator* host,
    /* [in] */ Int32 previewSize)
    : mHost(host)
    , mPreviewSize(previewSize)
{
}

ECode FolderIcon::FolderRingAnimator::AcceptAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> obj = IFloat::Probe(value);
    Float percent;
    obj->GetValue(&percent);
    mHost->mOuterRingSize = (1 + percent * OUTER_RING_GROWTH_FACTOR) * mPreviewSize;
    mHost->mInnerRingSize = (1 + percent * INNER_RING_GROWTH_FACTOR) * mPreviewSize;
    if (mHost->mCellLayout != NULL) {
        IView::Probe(mHost->mCellLayout)->Invalidate();
    }
    return NOERROR;
}

FolderIcon::FolderRingAnimator::AcceptAnimatorListenerAdapter::AcceptAnimatorListenerAdapter(
    /* [in] */ FolderIcon::FolderRingAnimator* host)
    : mHost(host)
{
}

ECode FolderIcon::FolderRingAnimator::AcceptAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mFolderIcon != NULL) {
        return IView::Probe(mHost->mFolderIcon->mPreviewBackground)->SetVisibility(INVISIBLE);
    }
    return NOERROR;
}

//=============================================================
// FolderIcon::FolderRingAnimator::NeutralAnimatorUpdateListener
//=============================================================
CAR_INTERFACE_IMPL(FolderIcon::FolderRingAnimator::NeutralAnimatorUpdateListener, Object,
        IAnimatorUpdateListener);

FolderIcon::FolderRingAnimator::NeutralAnimatorUpdateListener::NeutralAnimatorUpdateListener(
    /* [in] */ FolderIcon::FolderRingAnimator* host,
    /* [in] */ Int32 previewSize)
    : mHost(host)
    , mPreviewSize(previewSize)
{
}

ECode FolderIcon::FolderRingAnimator::NeutralAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> obj = IFloat::Probe(value);
    Float percent;
    obj->GetValue(&percent);

    mHost->mOuterRingSize = (1 + (1 - percent) * OUTER_RING_GROWTH_FACTOR) * mPreviewSize;
    mHost->mInnerRingSize = (1 + (1 - percent) * INNER_RING_GROWTH_FACTOR) * mPreviewSize;
    if (mHost->mCellLayout != NULL) {
        IView::Probe(mHost->mCellLayout)->Invalidate();
    }
    return NOERROR;
}

FolderIcon::FolderRingAnimator::NeutralAnimatorListenerAdapter::NeutralAnimatorListenerAdapter(
    /* [in] */ FolderIcon::FolderRingAnimator* host)
    : mHost(host)
{
}

ECode FolderIcon::FolderRingAnimator::NeutralAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mCellLayout != NULL) {
        return mHost->mCellLayout->HideFolderAccept(mHost);
    }
    if (mHost->mFolderIcon != NULL) {
        return IView::Probe(mHost->mFolderIcon->mPreviewBackground)->SetVisibility(VISIBLE);
    }
    return NOERROR;
}

AutoPtr<IDrawable> FolderIcon::FolderRingAnimator::sSharedOuterRingDrawable;
AutoPtr<IDrawable> FolderIcon::FolderRingAnimator::sSharedInnerRingDrawable;
Int32 FolderIcon::FolderRingAnimator::sPreviewSize = -1;
Int32 FolderIcon::FolderRingAnimator::sPreviewPadding = -1;

CAR_INTERFACE_IMPL(FolderIcon::FolderRingAnimator, Object,
        IFolderIconFolderRingAnimator);

//=============================================================
// FolderIcon::FolderRingAnimator
//=============================================================
FolderIcon::FolderRingAnimator::FolderRingAnimator(
    /* [in] */ ILauncher* launcher,
    /* [in] */ FolderIcon* folderIcon)
    : mOuterRingSize(0.0f)
    , mInnerRingSize(0.0f)
{
    mFolderIcon = folderIcon;
    AutoPtr<IResources> res;
    IContext::Probe(launcher)->GetResources((IResources**)&res);
    res->GetDrawable(
            Elastos::Droid::Launcher2::R::drawable::portal_ring_outer_holo,
            (IDrawable**)&mOuterRingDrawable);
    res->GetDrawable(
            Elastos::Droid::Launcher2::R::drawable::portal_ring_inner_holo,
            (IDrawable**)&mInnerRingDrawable);

    // We need to reload the static values when configuration changes in case they are
    // different in another configuration
    if (sStaticValuesDirty) {
        res->GetDimensionPixelSize(
                Elastos::Droid::Launcher2::R::dimen::folder_preview_size, &sPreviewSize);
        res->GetDimensionPixelSize(
                Elastos::Droid::Launcher2::R::dimen::folder_preview_padding, &sPreviewPadding);
        res->GetDrawable(
                Elastos::Droid::Launcher2::R::drawable::portal_ring_outer_holo,
                (IDrawable**)&sSharedOuterRingDrawable);
        res->GetDrawable(
                Elastos::Droid::Launcher2::R::drawable::portal_ring_inner_holo,
                (IDrawable**)&sSharedInnerRingDrawable);
        res->GetDrawable(
                Elastos::Droid::Launcher2::R::drawable::portal_ring_rest,
                (IDrawable**)&sSharedFolderLeaveBehind);
        sStaticValuesDirty = FALSE;
    }
}

ECode FolderIcon::FolderRingAnimator::AnimateToAcceptState()
{
    if (mNeutralAnimator != NULL) {
        IAnimator::Probe(mNeutralAnimator)->Cancel();
    }
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 0.0f;
    (*array)[1] = 1.0f;
    mAcceptAnimator = LauncherAnimUtils::OfFloat(IView::Probe(mCellLayout), array);
    mAcceptAnimator->SetDuration(CONSUMPTION_ANIMATION_DURATION);

    Int32 previewSize = sPreviewSize;
    AutoPtr<IAnimatorUpdateListener> listener = new AcceptAnimatorUpdateListener(this, previewSize);
    mAcceptAnimator->AddUpdateListener(listener);
    AutoPtr<IAnimatorListener> listener2 = new AcceptAnimatorListenerAdapter(this);
    IAnimator::Probe(mAcceptAnimator)->AddListener(listener2);
    return IAnimator::Probe(mAcceptAnimator)->Start();
}

ECode FolderIcon::FolderRingAnimator::AnimateToNaturalState()
{
    if (mAcceptAnimator != NULL) {
        IAnimator::Probe(mAcceptAnimator)->Cancel();
    }
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 0.0f;
    (*array)[1] = 1.0f;
    mNeutralAnimator = LauncherAnimUtils::OfFloat(IView::Probe(mCellLayout), array);
    mNeutralAnimator->SetDuration(CONSUMPTION_ANIMATION_DURATION);

    Int32 previewSize = sPreviewSize;
    AutoPtr<IAnimatorUpdateListener> listener = new NeutralAnimatorUpdateListener(this, previewSize);
    mNeutralAnimator->AddUpdateListener(listener);
    AutoPtr<IAnimatorListener> listener2 = new NeutralAnimatorListenerAdapter(this);
    IAnimator::Probe(mNeutralAnimator)->AddListener(listener2);
    return IAnimator::Probe(mNeutralAnimator)->Start();
}

ECode FolderIcon::FolderRingAnimator::GetCell(
    /* [in] */ ArrayOf<Int32>* loc)
{
    (*loc)[0] = mCellX;
    (*loc)[1] = mCellY;
    return NOERROR;
}

ECode FolderIcon::FolderRingAnimator::SetCell(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mCellX = x;
    mCellY = y;
    return NOERROR;
}

ECode FolderIcon::FolderRingAnimator::SetCellLayout(
    /* [in] */ ICellLayout* layout)
{
    mCellLayout = layout;
    return NOERROR;
}

ECode FolderIcon::FolderRingAnimator::GetOuterRingSize(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mOuterRingSize;
    return NOERROR;
}

ECode FolderIcon::FolderRingAnimator::GetInnerRingSize(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mInnerRingSize;
    return NOERROR;
}

//=============================================================
// FolderIcon::PreviewItemDrawingParams
//=============================================================
FolderIcon::PreviewItemDrawingParams::PreviewItemDrawingParams(
    /* [in] */ Float transX,
    /* [in] */ Float transY,
    /* [in] */ Float scale,
    /* [in] */ Int32 overlayAlpha)
    : mTransX(transX)
    , mTransY(transY)
    , mScale(scale)
    , mOverlayAlpha(overlayAlpha)
{
}

//=============================================================
// FolderIcon::MyRunnable
//=============================================================
FolderIcon::MyRunnable::MyRunnable(
    /* [in] */ FolderIcon* host,
    /* [in] */  IShortcutInfo* item)
    : mHost(host)
    , mItem(item)
{
}

ECode FolderIcon::MyRunnable::Run()
{
    mHost->mHiddenItems->Remove(mItem);
    mHost->mFolder->ShowItem(mItem);
    return mHost->Invalidate();
}

//=============================================================
// FolderIcon::MyAnimatorUpdateListener3
//=============================================================
CAR_INTERFACE_IMPL(FolderIcon::MyAnimatorUpdateListener3, Object, IAnimatorUpdateListener);

FolderIcon::MyAnimatorUpdateListener3::MyAnimatorUpdateListener3(
    /* [in] */ FolderIcon* host,
    /* [in] */ Boolean reverse,
    /* [in] */ Float transX0,
    /* [in] */ Float transY0,
    /* [in] */ Float scale0,
    /* [in] */ PreviewItemDrawingParams* finalParams)
    : mHost(host)
    , mReverse(reverse)
    , mTransX0(transX0)
    , mTransY0(transY0)
    , mScale0(scale0)
    , mFinalParams(finalParams)
{
}

ECode FolderIcon::MyAnimatorUpdateListener3::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> value = IFloat::Probe(obj);
    Float progress;
    value->GetValue(&progress);
    if (mReverse) {
        progress = 1 - progress;
        IView::Probe(mHost->mPreviewBackground)->SetAlpha(progress);
    }

    mHost->mAnimParams->mTransX = mTransX0 + progress * (mFinalParams->mTransX - mTransX0);
    mHost->mAnimParams->mTransY = mTransY0 + progress * (mFinalParams->mTransY - mTransY0);
    mHost->mAnimParams->mScale = mScale0 + progress * (mFinalParams->mScale - mScale0);
    return mHost->Invalidate();
}

FolderIcon::MyAnimatorListenerAdapter3::MyAnimatorListenerAdapter3(
    /* [in] */ FolderIcon* host,
    /* [in] */ IRunnable* onCompleteRunnable)
    : mHost(host)
    , mOnCompleteRunnable(onCompleteRunnable)
{
}

ECode FolderIcon::MyAnimatorListenerAdapter3::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mHost->mAnimating = TRUE;
    return NOERROR;
}

ECode FolderIcon::MyAnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mAnimating = FALSE;
    if (mOnCompleteRunnable != NULL) {
        mOnCompleteRunnable->Run();
    }
    return NOERROR;
}

//=============================================================
// FolderIcon
//=============================================================
AutoPtr<IDrawable> FolderIcon::sSharedFolderLeaveBehind;

Boolean FolderIcon::sStaticValuesDirty = TRUE;

const Int32 FolderIcon::NUM_ITEMS_IN_PREVIEW = 3;
const Int32 FolderIcon::CONSUMPTION_ANIMATION_DURATION = 100;
const Int32 FolderIcon::DROP_IN_ANIMATION_DURATION = 400;
const Int32 FolderIcon::INITIAL_ITEM_ANIMATION_DURATION = 350;
const Int32 FolderIcon::FINAL_ITEM_ANIMATION_DURATION = 200;

// The degree to which the inner ring grows when accepting drop
const Float FolderIcon::INNER_RING_GROWTH_FACTOR = 0.15f;

// The degree to which the outer ring is scaled in its natural state
const Float FolderIcon::OUTER_RING_GROWTH_FACTOR = 0.3f;

// The amount of vertical spread between items in the stack [0...1]
const Float FolderIcon::PERSPECTIVE_SHIFT_FACTOR = 0.24f;

// The degree to which the item in the back of the stack is scaled [0...1]
// (0 means it's not scaled at all, 1 means it's scaled to nothing)
const Float FolderIcon::PERSPECTIVE_SCALE_FACTOR = 0.35f;


CAR_INTERFACE_IMPL(FolderIcon, LinearLayout, IFolderIcon);

FolderIcon::FolderIcon()
    : mIntrinsicIconSize(0)
    , mBaselineIconScale(0.0f)
    , mBaselineIconSize(0)
    , mAvailableSpaceInPreview(0)
    , mTotalWidth(-1)
    , mPreviewOffsetX(0)
    , mPreviewOffsetY(0)
    , mMaxPerspectiveShift(0.0f)
    , mAnimating(FALSE)
{
}

ECode FolderIcon::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);
    Init();
    return NOERROR;
}

ECode FolderIcon::constructor(
    /* [in] */ IContext* context)
{
    LinearLayout::constructor(context);
    Init();
    return NOERROR;
}

void FolderIcon::Init()
{
    CArrayList::New((IArrayList**)&mHiddenItems);
    mParams = new PreviewItemDrawingParams(0, 0, 0, 0);
    mAnimParams = new PreviewItemDrawingParams(0, 0, 0, 0);
    mLongPressHelper = new CheckLongPressHelper(this);
    mFolderListener = new FolderListener(this);
}

ECode FolderIcon::IsDropEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IViewParent> parent;
    GetParent((IViewParent**)&parent);
    IView* cellLayoutChildren = IView::Probe(parent);

    AutoPtr<IViewParent> parent2;
    cellLayoutChildren->GetParent((IViewParent**)&parent2);
    IView* cellLayout = IView::Probe(parent2);

    AutoPtr<IViewParent> parent3;
    cellLayout->GetParent((IViewParent**)&parent3);
    IWorkspace* workspace = IWorkspace::Probe(parent3);
    Boolean res;
    workspace->IsSmall(&res);
    *result = !res;
    return NOERROR;
}

ECode FolderIcon::FromXml(
    /* [in] */ Int32 resId,
    /* [in] */ ILauncher* launcher,
    /* [in] */ IViewGroup* group,
    /* [in] */ IFolderInfo* folderInfo,
    /* [in] */ IIconCache* iconCache,
    /* [out] */ IFolderIcon** outicon)
{
    VALIDATE_NOT_NULL(outicon);

    //@SuppressWarnings("all") // suppress dead code warning
    Boolean error = INITIAL_ITEM_ANIMATION_DURATION >= DROP_IN_ANIMATION_DURATION;
    if (error) {
        // throw new IllegalStateException("DROP_IN_ANIMATION_DURATION must be greater than " +
        //         "INITIAL_ITEM_ANIMATION_DURATION, as sequencing of adding first two items " +
        //         "is dependent on this");
        Slogger::E("FolderIcon", "DROP_IN_ANIMATION_DURATION must be greater than "
                "INITIAL_ITEM_ANIMATION_DURATION, as sequencing of adding first two items "
                "is dependent on this");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(IContext::Probe(launcher), (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(resId, group, FALSE, (IView**)&view);
    AutoPtr<IFolderIcon> icon = IFolderIcon::Probe(view);

    FolderIcon* iconObj = (FolderIcon*)icon.Get();
    FolderInfo* folderInfoObj = (FolderInfo*)folderInfo;
    AutoPtr<IView> view2;
    view->FindViewById(Elastos::Droid::Launcher2::R::id::folder_icon_name, (IView**)&view2);
    iconObj->mFolderName = IBubbleTextView::Probe(view2);
    ITextView::Probe(iconObj->mFolderName)->SetText(folderInfoObj->mTitle);
    AutoPtr<IView> view3;
    view->FindViewById(Elastos::Droid::Launcher2::R::id::preview_background, (IView**)&view3);
    iconObj->mPreviewBackground = IImageView::Probe(view3);

    view->SetTag(TO_IINTERFACE(folderInfo));
    view->SetOnClickListener(((Launcher*)launcher)->mInnerListener);
    iconObj->mInfo = folderInfo;
    iconObj->mLauncher = launcher;

    String str;
    IContext::Probe(launcher)->GetString(
            Elastos::Droid::Launcher2::R::string::folder_name_format, &str);
    String title;
    folderInfoObj->mTitle->ToString(&title);
    String description;
    description.AppendFormat(str, title.string());
    AutoPtr<ICharSequence> cdescription = CoreUtils::Convert(description);
    iconObj->SetContentDescription(cdescription);
    AutoPtr<IFolder> folder;
    Folder::FromXml(IContext::Probe(launcher), (IFolder**)&folder);
    AutoPtr<IDragController> controller;
    launcher->GetDragController((IDragController**)&controller);
    folder->SetDragController(controller);
    folder->SetFolderIcon(icon);
    folder->Bind(folderInfo);
    iconObj->mFolder = folder;

    iconObj->mFolderRingAnimator = new FolderRingAnimator(launcher, iconObj);
    folderInfoObj->AddListener(iconObj->mFolderListener);

    *outicon = icon;
    REFCOUNT_ADD(*outicon);
    return NOERROR;
}

AutoPtr<IParcelable> FolderIcon::OnSaveInstanceState()
{
    sStaticValuesDirty = TRUE;
    return LinearLayout::OnSaveInstanceState();
}

ECode FolderIcon::GetFolder(
    /* [out] */ IFolder** folder)
{
    VALIDATE_NOT_NULL(folder);

    *folder = mFolder;
    REFCOUNT_ADD(*folder);
    return NOERROR;
}

ECode FolderIcon::GetFolderInfo(
    /* [out] */ IFolderInfo** info)
{
    VALIDATE_NOT_NULL(info);

    *info = mInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

Boolean FolderIcon::WillAcceptItem(
    /* [in] */ IItemInfo* item)
{
    ItemInfo* _item = (ItemInfo*)item;
    Int32 itemType = _item->mItemType;
    Boolean res;
    return ((itemType == LauncherSettings::Favorites::ITEM_TYPE_APPLICATION ||
            itemType == LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT) &&
            (mFolder->IsFull(&res), !res) && TO_IINTERFACE(item) != TO_IINTERFACE(mInfo)
            && !((FolderInfo*)mInfo.Get())->mOpened);
}

ECode FolderIcon::AcceptDrop(
    /* [in] */ IInterface* dragInfo,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IItemInfo> item = IItemInfo::Probe(dragInfo);
    Boolean res;
    *result = (mFolder->IsDestroyed(&res), !res) && WillAcceptItem(item);
    return NOERROR;
}

ECode FolderIcon::AddItem(
    /* [in] */ IShortcutInfo* item)
{
    ((FolderInfo*)mInfo.Get())->Add((ShortcutInfo*)item);
    return NOERROR;
}

ECode FolderIcon::OnDragEnter(
    /* [in] */ IInterface* dragInfo)
{
    Boolean res;
    mFolder->IsDestroyed(&res);
    if (res || !WillAcceptItem(IItemInfo::Probe(dragInfo))) {
        return NOERROR;
    }
    AutoPtr<IViewGroupLayoutParams> params;
    GetLayoutParams((IViewGroupLayoutParams**)&params);

    AutoPtr<CellLayout::CellLayoutLayoutParams> lp = (CellLayout::CellLayoutLayoutParams*)
            ICellLayoutLayoutParams::Probe(params);

    AutoPtr<IViewParent> parent1;
    GetParent((IViewParent**)&parent1);
    AutoPtr<IViewParent> parent2;
    parent1->GetParent((IViewParent**)&parent2);
    AutoPtr<ICellLayout> layout = ICellLayout::Probe(parent2);
    mFolderRingAnimator->SetCell(lp->mCellX, lp->mCellY);
    mFolderRingAnimator->SetCellLayout(layout);
    mFolderRingAnimator->AnimateToAcceptState();
    return layout->ShowFolderAccept(mFolderRingAnimator);
}

ECode FolderIcon::OnDragOver(
    /* [in] */ IInterface* dragInfo)
{
    return NOERROR;
}

ECode FolderIcon::PerformCreateAnimation(
    /* [in] */ IShortcutInfo* destInfo,
    /* [in] */ IView* destView,
    /* [in] */ IShortcutInfo* srcInfo,
    /* [in] */ IDragView* srcView,
    /* [in] */ IRect* dstRect,
    /* [in] */ Float scaleRelativeToDragLayer,
    /* [in] */ IRunnable* postAnimationRunnable)
{
    // These correspond two the drawable and view that the icon was dropped _onto_
    AutoPtr<ArrayOf<IDrawable*> > result;
    ITextView::Probe(destView)->GetCompoundDrawables((ArrayOf<IDrawable*>**)&result);
    AutoPtr<IDrawable> animateDrawable = (*result)[1];
    Int32 width;
    animateDrawable->GetIntrinsicWidth(&width);
    Int32 width2;
    destView->GetMeasuredWidth(&width2);
    ComputePreviewDrawingParams(width, width2);

    // This will animate the first item from it's position as an icon into its
    // position as the first item in the preview
    AnimateFirstItem(animateDrawable, INITIAL_ITEM_ANIMATION_DURATION, FALSE, NULL);
    AddItem(destInfo);

    // This will animate the dragView (srcView) into the new folder
    OnDrop(srcInfo, srcView, dstRect, scaleRelativeToDragLayer, 1,
            postAnimationRunnable, NULL);
    return NOERROR;
}

ECode FolderIcon::PerformDestroyAnimation(
    /* [in] */ IView* finalView,
    /* [in] */ IRunnable* onCompleteRunnable)
{
    AutoPtr<ArrayOf<IDrawable*> > result;
    ITextView::Probe(finalView)->GetCompoundDrawables((ArrayOf<IDrawable*>**)&result);
    AutoPtr<IDrawable> animateDrawable = (*result)[1];
    Int32 width;
    animateDrawable->GetIntrinsicWidth(&width);
    Int32 width2;
    finalView->GetMeasuredWidth(&width2);
    ComputePreviewDrawingParams(width, width2);

    // This will animate the first item from it's position as an icon into its
    // position as the first item in the preview
    AnimateFirstItem(animateDrawable, FINAL_ITEM_ANIMATION_DURATION, TRUE,
            onCompleteRunnable);
    return NOERROR;
}

ECode FolderIcon::OnDragExit(
    /* [in] */ IInterface* dragInfo)
{
    return OnDragExit();
}

ECode FolderIcon::OnDragExit()
{
    return mFolderRingAnimator->AnimateToNaturalState();
}

void FolderIcon::OnDrop(
    /* [in] */  IShortcutInfo* item,
    /* [in] */  IDragView* animateView,
    /* [in] */  IRect* finalRect,
    /* [in] */  Float scaleRelativeToDragLayer,
    /* [in] */  Int32 index,
    /* [in] */  IRunnable* postAnimationRunnable,
    /* [in] */  IDropTargetDragObject* d)
{
    ShortcutInfo* _item = (ShortcutInfo*)item;
    _item->mCellX = -1;
    _item->mCellY = -1;

    // Typically, the animateView corresponds to the DragView; however, if this is being done
    // after a configuration activity (ie. for a Shortcut being dragged from AllApps) we
    // will not have a view to animate
    if (animateView != NULL) {
        AutoPtr<IDragLayer> dragLayer;
        mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
        AutoPtr<IRect> from;
        CRect::New((IRect**)&from);
        dragLayer->GetViewRectRelativeToSelf(IView::Probe(animateView), from);
        AutoPtr<IRect> to = finalRect;
        if (to == NULL) {
            CRect::New((IRect**)&to);
            AutoPtr<IWorkspace> workspace;
            mLauncher->GetWorkspace((IWorkspace**)&workspace);

            // Set cellLayout and this to it's final state to compute final animation locations
            AutoPtr<IViewParent> parent1;
            GetParent((IViewParent**)&parent1);
            AutoPtr<IViewParent> parent2;
            parent1->GetParent((IViewParent**)&parent2);

            workspace->SetFinalTransitionTransform(ICellLayout::Probe(parent2));
            Float scaleX;
            GetScaleX(&scaleX);
            Float scaleY;
            GetScaleY(&scaleY);
            SetScaleX(1.0f);
            SetScaleY(1.0f);
            dragLayer->GetDescendantRectRelativeToSelf(this, to, &scaleRelativeToDragLayer);
            // Finished computing final animation locations, restore current state
            SetScaleX(scaleX);
            SetScaleY(scaleY);
            workspace->ResetTransitionTransform(ICellLayout::Probe(parent2));
        }

        AutoPtr<ArrayOf<Int32> > center = ArrayOf<Int32>::Alloc(2);
        Float scale = GetLocalCenterForIndex(index, center);
        (*center)[0] = (Int32)Elastos::Core::Math::Round(scaleRelativeToDragLayer * (*center)[0]);
        (*center)[1] = (Int32)Elastos::Core::Math::Round(scaleRelativeToDragLayer * (*center)[1]);

        Int32 width;
        IView::Probe(animateView)->GetMeasuredWidth(&width);
        Int32 height;
        IView::Probe(animateView)->GetMeasuredHeight(&height);
        to->Offset((*center)[0] - width / 2,
                (*center)[1] - height / 2);

        Float finalAlpha = index < NUM_ITEMS_IN_PREVIEW ? 0.5f : 0.0f;

        Float finalScale = scale * scaleRelativeToDragLayer;

        AutoPtr<IDecelerateInterpolator> polator;
        CDecelerateInterpolator::New(2, (IDecelerateInterpolator**)&polator);
        AutoPtr<IAccelerateInterpolator> polator2;
        CAccelerateInterpolator::New(2, (IAccelerateInterpolator**)&polator2);
        dragLayer->AnimateView(animateView, from, to, finalAlpha,
                1.0, 1.0, finalScale, finalScale, DROP_IN_ANIMATION_DURATION,
                IInterpolator::Probe(polator), IInterpolator::Probe(polator2),
                postAnimationRunnable, IDragLayer::ANIMATION_END_DISAPPEAR, NULL);
        AddItem(item);
        mHiddenItems->Add(TO_IINTERFACE(item));
        mFolder->HideItem(item);
        AutoPtr<IRunnable> r = new MyRunnable(this, item);
        Boolean res;
        PostDelayed(r, DROP_IN_ANIMATION_DURATION, &res);
    }
    else {
        AddItem(item);
    }
}

ECode FolderIcon::OnDrop(
    /* [in] */ IDropTargetDragObject* d)
{
    DragObject* _d = (DragObject*)d;
    AutoPtr<IShortcutInfo> item;
    if (IApplicationInfo::Probe(_d->mDragInfo) != NULL) {
        // Came from all apps -- make a copy
        ApplicationInfo* info = (ApplicationInfo*)IApplicationInfo::Probe(_d->mDragInfo);
        info->MakeShortcut((IShortcutInfo**)&item);
    } else {
        item = IShortcutInfo::Probe(_d->mDragInfo);
    }
    mFolder->NotifyDrop();
    FolderInfo* info = (FolderInfo*)mInfo.Get();
    Int32 size;
    info->mContents->GetSize(&size);
    OnDrop(item, _d->mDragView, NULL, 1.0f, size, _d->mPostAnimationRunnable, d);
    return NOERROR;
}

ECode FolderIcon::GetDropTargetDelegate(
    /* [in] */ IDropTargetDragObject* d,
    /* [out] */ IDropTarget** target)
{
    VALIDATE_NOT_NULL(target);

    *target = NULL;
    return NOERROR;
}

void FolderIcon::ComputePreviewDrawingParams(
    /* [in] */ Int32 drawableSize,
    /* [in] */ Int32 totalSize)
{
    if (mIntrinsicIconSize != drawableSize || mTotalWidth != totalSize) {
        mIntrinsicIconSize = drawableSize;
        mTotalWidth = totalSize;

        Int32 previewSize = FolderRingAnimator::sPreviewSize;
        Int32 previewPadding = FolderRingAnimator::sPreviewPadding;

        mAvailableSpaceInPreview = (previewSize - 2 * previewPadding);
        // cos(45) = 0.707  + ~= 0.1) = 0.8f
        Int32 adjustedAvailableSpace = (Int32)((mAvailableSpaceInPreview / 2) * (1 + 0.8f));

        Int32 unscaledHeight = (Int32)(mIntrinsicIconSize * (1 + PERSPECTIVE_SHIFT_FACTOR));
        mBaselineIconScale = (1.0f * adjustedAvailableSpace / unscaledHeight);

        mBaselineIconSize = (Int32)(mIntrinsicIconSize * mBaselineIconScale);
        mMaxPerspectiveShift = mBaselineIconSize * PERSPECTIVE_SHIFT_FACTOR;

        mPreviewOffsetX = (mTotalWidth - mAvailableSpaceInPreview) / 2;
        mPreviewOffsetY = previewPadding;
    }
}

void FolderIcon::ComputePreviewDrawingParams(
    /* [in] */ IDrawable* d)
{
    Int32 width;
    d->GetIntrinsicWidth(&width);
    Int32 width2;
    GetMeasuredWidth(&width2);
    ComputePreviewDrawingParams(width, width2);
}

Float FolderIcon::GetLocalCenterForIndex(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Int32>* center)
{
    mParams = ComputePreviewItemDrawingParams(Elastos::Core::Math::Min(NUM_ITEMS_IN_PREVIEW, index), mParams);

    mParams->mTransX += mPreviewOffsetX;
    mParams->mTransY += mPreviewOffsetY;
    Float offsetX = mParams->mTransX + (mParams->mScale * mIntrinsicIconSize) / 2;
    Float offsetY = mParams->mTransY + (mParams->mScale * mIntrinsicIconSize) / 2;

    (*center)[0] = (Int32)Elastos::Core::Math::Round(offsetX);
    (*center)[1] = (Int32)Elastos::Core::Math::Round(offsetY);
    return mParams->mScale;
}

AutoPtr<FolderIcon::PreviewItemDrawingParams> FolderIcon::ComputePreviewItemDrawingParams(
    /* [in] */ Int32 index,
    /* [in] */ FolderIcon::PreviewItemDrawingParams* params)
{
    index = NUM_ITEMS_IN_PREVIEW - index - 1;
    Float r = (index * 1.0f) / (NUM_ITEMS_IN_PREVIEW - 1);
    Float scale = (1 - PERSPECTIVE_SCALE_FACTOR * (1 - r));

    Float offset = (1 - r) * mMaxPerspectiveShift;
    Float scaledSize = scale * mBaselineIconSize;
    Float scaleOffsetCorrection = (1 - scale) * mBaselineIconSize;

    // We want to imagine our coordinates from the bottom left, growing up and to the
    // right. This is natural for the x-axis, but for the y-axis, we have to invert things.
    Float transY = mAvailableSpaceInPreview - (offset + scaledSize + scaleOffsetCorrection);
    Float transX = offset + scaleOffsetCorrection;
    Float totalScale = mBaselineIconScale * scale;
    const Int32 overlayAlpha = (Int32)(80 * (1 - r));

    if (params == NULL) {
        params = new FolderIcon::PreviewItemDrawingParams(transX, transY, totalScale, overlayAlpha);
    }
    else {
        params->mTransX = transX;
        params->mTransY = transY;
        params->mScale = totalScale;
        params->mOverlayAlpha = overlayAlpha;
    }
    return params;
}

void FolderIcon::DrawPreviewItem(
    /* [in] */ ICanvas* canvas,
    /* [in] */ FolderIcon::PreviewItemDrawingParams* params)
{
    Int32 tmp;
    canvas->Save(&tmp);
    canvas->Translate(params->mTransX + mPreviewOffsetX, params->mTransY + mPreviewOffsetY);
    canvas->Scale(params->mScale, params->mScale);
    AutoPtr<IDrawable> d = params->mDrawable;

    if (d != NULL) {
        d->SetBounds(0, 0, mIntrinsicIconSize, mIntrinsicIconSize);
        d->SetFilterBitmap(TRUE);
        AutoPtr<IColor> color;
        CColor::AcquireSingleton((IColor**)&color);
        Int32 argb;
        color->Argb(params->mOverlayAlpha, 0, 0, 0, &argb);
        d->SetColorFilter(argb, PorterDuffMode_SRC_ATOP);
        d->Draw(canvas);
        d->ClearColorFilter();
        d->SetFilterBitmap(FALSE);
    }
    canvas->Restore();
}

ECode FolderIcon::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    LinearLayout::DispatchDraw(canvas);

    if (mFolder == NULL) return NOERROR;
    Int32 count;
    mFolder->GetItemCount(&count);
    if (count == 0 && !mAnimating) return NOERROR;

    AutoPtr<IArrayList> items;
    mFolder->GetItemsInReadingOrder((IArrayList**)&items);
    AutoPtr<IDrawable> d;
    AutoPtr<ITextView> v;

    // Update our drawing parameters if necessary
    if (mAnimating) {
        ComputePreviewDrawingParams(mAnimParams->mDrawable);
    }
    else {
        AutoPtr<IInterface> obj;
        items->Get(0, (IInterface**)&obj);
        v = ITextView::Probe(obj);

        AutoPtr<ArrayOf<IDrawable*> > array;
        v->GetCompoundDrawables((ArrayOf<IDrawable*>**)&array);
        d = (*array)[1];
        ComputePreviewDrawingParams(d);
    }

    Int32 size;
    items->GetSize(&size);
    Int32 nItemsInPreview = Elastos::Core::Math::Min(size, NUM_ITEMS_IN_PREVIEW);
    if (!mAnimating) {
        for (Int32 i = nItemsInPreview - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            items->Get(i, (IInterface**)&obj);
            v = ITextView::Probe(obj);

            AutoPtr<IInterface> tag;
            IView::Probe(v)->GetTag((IInterface**)&tag);
            Boolean res;
            mHiddenItems->Contains(tag, &res);
            if (!res) {
                AutoPtr<ArrayOf<IDrawable*> > array;
                v->GetCompoundDrawables((ArrayOf<IDrawable*>**)&array);
                d = (*array)[1];

                mParams = ComputePreviewItemDrawingParams(i, mParams);
                mParams->mDrawable = d;
                DrawPreviewItem(canvas, mParams);
            }
        }
    }
    else {
        DrawPreviewItem(canvas, mAnimParams);
    }
    return NOERROR;
}

void FolderIcon::AnimateFirstItem(
    /* [in] */ IDrawable* d,
    /* [in] */ Int32 duration,
    /* [in] */ Boolean reverse,
    /* [in] */ IRunnable* onCompleteRunnable)
{
    AutoPtr<PreviewItemDrawingParams> finalParams = ComputePreviewItemDrawingParams(0, NULL);

    Float scale0 = 1.0f;
    Int32 width;
    d->GetIntrinsicWidth(&width);
    Float transX0 = (mAvailableSpaceInPreview - width) / 2;
    Int32 height;
    d->GetIntrinsicHeight(&height);
    Float transY0 = (mAvailableSpaceInPreview - height) / 2;
    mAnimParams->mDrawable = d;

    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 0.0f;
    (*array)[1] = 1.0f;
    AutoPtr<IValueAnimator> va = LauncherAnimUtils::OfFloat(this, array);

    AutoPtr<IAnimatorUpdateListener> listener = new MyAnimatorUpdateListener3(this,
            reverse, transX0, transY0, scale0, finalParams);
    va->AddUpdateListener(listener);

    AutoPtr<IAnimatorListener> listener2 = new MyAnimatorListenerAdapter3(this, onCompleteRunnable);
Slogger::D("FolderIcon", "=======FolderIcon::AnimateFirstItem listener2=%p",listener2.Get());
    IAnimator::Probe(va)->AddListener(listener2);
    va->SetDuration(duration);
    IAnimator::Probe(va)->Start();
}

ECode FolderIcon::SetTextVisible(
    /* [in] */ Boolean visible)
{
    if (visible) {
        return IView::Probe(mFolderName)->SetVisibility(VISIBLE);
    }
    else {
        return IView::Probe(mFolderName)->SetVisibility(INVISIBLE);
    }
    return NOERROR;
}

ECode FolderIcon::GetTextVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 visibility;
    IView::Probe(mFolderName)->GetVisibility(&visibility);
    *result = visibility == VISIBLE;
    return NOERROR;
}

ECode FolderIcon::OnItemsChanged()
{
    Invalidate();
    return RequestLayout();
}

ECode FolderIcon::OnAdd(
    /* [in] */ IShortcutInfo* item)
{
    Invalidate();
    return RequestLayout();
}

ECode FolderIcon::OnRemove(
    /* [in] */ IShortcutInfo* item)
{
    Invalidate();
    return RequestLayout();
}

ECode FolderIcon::OnTitleChanged(
    /* [in] */ ICharSequence* title)
{
    String str;
    title->ToString(&str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    ITextView::Probe(mFolderName)->SetText(cchar);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String format;
    context->GetString(
            Elastos::Droid::Launcher2::R::string::folder_name_format, &format);
    String _title;
    title->ToString(&_title);
    String description;
    description.AppendFormat(format, _title.string());
    AutoPtr<ICharSequence> cdescription = CoreUtils::Convert(description);
    return SetContentDescription(cdescription);
}

ECode FolderIcon::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* outresult)
{
    VALIDATE_NOT_NULL(outresult);

    // Call the superclass onTouchEvent first, because sometimes it changes the state to
    // isPressed() on an ACTION_UP
    Boolean result;
    LinearLayout::OnTouchEvent(event, &result);

    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            mLongPressHelper->PostCheckForLongPress();
            break;
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            mLongPressHelper->CancelLongPress();
            break;
    }
    *outresult = result;
    return NOERROR;
}

ECode FolderIcon::CancelLongPress()
{
    LinearLayout::CancelLongPress();

    mLongPressHelper->CancelLongPress();
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos