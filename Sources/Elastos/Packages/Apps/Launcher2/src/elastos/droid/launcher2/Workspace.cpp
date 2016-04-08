
#include "elastos/droid/launcher2/Workspace.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

Workspace::WallpaperOffsetInterpolator::WallpaperOffsetInterpolator(
    /* [in] */ Workspace* host)
    : mHost(host)
    , mFinalHorizontalWallpaperOffset(0.0f)
    , mFinalVerticalWallpaperOffset(0.5f)
    , mHorizontalWallpaperOffset(0.0f)
    , mVerticalWallpaperOffset(0.5f)
    , mLastWallpaperOffsetUpdateTime(0)
    , mIsMovingFast(FALSE)
    , mOverrideHorizontalCatchupConstant(FALSE)
    , mHorizontalCatchupConstant(0.35f)
    , mVerticalCatchupConstant(0.35f)
{

}

ECode Workspace::WallpaperOffsetInterpolator::SetOverrideHorizontalCatchupConstant(
    /* [in] */ Boolean override)
{
    mOverrideHorizontalCatchupConstant = override;
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::SetHorizontalCatchupConstant(
    /* [in] */ Float f)
{
    mHorizontalCatchupConstant = f;
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::SetVerticalCatchupConstant(
    /* [in] */ Float f)
{
    mVerticalCatchupConstant = f;
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::ComputeScrollOffset(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mHorizontalWallpaperOffset == mFinalHorizontalWallpaperOffset&&
            mVerticalWallpaperOffset == mFinalVerticalWallpaperOffset) {
        mIsMovingFast = FALSE;
        *result = FALSE;
        return NOERROR;
    }
    Int32 x;
    mHost->mDisplaySize->GerX(&x);
    Int32 y;
    mHost->mDisplaySize->GetY(&y);
    Boolean isLandscape = x > y;

    Int64 currentTime = System::CurrentTimeMillis();
    Int64 timeSinceLastUpdate = currentTime - mLastWallpaperOffsetUpdateTime;
    timeSinceLastUpdate = Math::Min((Int64)(1000/30f), timeSinceLastUpdate);
    timeSinceLastUpdate = Math::Max(1L, timeSinceLastUpdate);

    Float xdiff = Math::Abs(mFinalHorizontalWallpaperOffset - mHorizontalWallpaperOffset);
    if (!mIsMovingFast && xdiff > 0.07) {
        mIsMovingFast = TRUE;
    }

    Float fractionToCatchUpIn1MsHorizontal;
    if (mOverrideHorizontalCatchupConstant) {
        fractionToCatchUpIn1MsHorizontal = mHorizontalCatchupConstant;
    }
    else if (mIsMovingFast) {
        fractionToCatchUpIn1MsHorizontal = isLandscape ? 0.5f : 0.75f;
    }
    else {
        // slow
        fractionToCatchUpIn1MsHorizontal = isLandscape ? 0.27f : 0.5f;
    }
    Float fractionToCatchUpIn1MsVertical = mVerticalCatchupConstant;

    fractionToCatchUpIn1MsHorizontal /= 33f;
    fractionToCatchUpIn1MsVertical /= 33f;

    const Float UPDATE_THRESHOLD = 0.00001f;
    Float hOffsetDelta = mFinalHorizontalWallpaperOffset - mHorizontalWallpaperOffset;
    Float vOffsetDelta = mFinalVerticalWallpaperOffset - mVerticalWallpaperOffset;
    Boolean jumpToFinalValue = Math::Abs(hOffsetDelta) < UPDATE_THRESHOLD &&
        Math::Abs(vOffsetDelta) < UPDATE_THRESHOLD;

    // Don't have any lag between workspace and wallpaper on non-large devices
    Boolean res;
    LauncherApplication->IsScreenLarge(&res);
    if (!res || jumpToFinalValue) {
        mHorizontalWallpaperOffset = mFinalHorizontalWallpaperOffset;
        mVerticalWallpaperOffset = mFinalVerticalWallpaperOffset;
    }
    else {
        Float percentToCatchUpVertical =
            Math::Min(1.0f, timeSinceLastUpdate * fractionToCatchUpIn1MsVertical);
        Float percentToCatchUpHorizontal =
            Math::Min(1.0f, timeSinceLastUpdate * fractionToCatchUpIn1MsHorizontal);
        mHorizontalWallpaperOffset += percentToCatchUpHorizontal * hOffsetDelta;
        mVerticalWallpaperOffset += percentToCatchUpVertical * vOffsetDelta;
    }

    mLastWallpaperOffsetUpdateTime = System::CurrentTimeMillis();
    *result = TRUE;
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::GetCurrX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);

    *x = mHorizontalWallpaperOffset;
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::GetFinalX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);

    *x = mFinalHorizontalWallpaperOffset;
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::GetCurrY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);

    *y = mVerticalWallpaperOffset;
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::GetFinalY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);

    *y = mFinalVerticalWallpaperOffset;
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::SetFinalX(
    /* [in] */ Float x)
{
    mFinalHorizontalWallpaperOffset = Math::Max(0f, Math::Min(x, 1.0f));
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::SetFinalY(
    /* [in] */ Float y)
{
    mFinalVerticalWallpaperOffset = Math::Max(0f, Math::Min(y, 1.0f));
    return NOERROR;
}

ECode Workspace::WallpaperOffsetInterpolator::JumpToFinal()
{
    mHorizontalWallpaperOffset = mFinalHorizontalWallpaperOffset;
    mVerticalWallpaperOffset = mFinalVerticalWallpaperOffset;
    return NOERROR;
}


CAR_INTERFACE_IMPL(Workspace::ZInterpolator, Object, ITimeInterpolator);

Workspace::ZInterpolator::ZInterpolator(
    /* [in] */ Float foc)
    : mFocalLength(foc)
{
}

ECode Workspace::ZInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (1.0f - mFocalLength / (mFocalLength + input)) /
            (1.0f - mFocalLength / (mFocalLength + 1.0f));
    return NOERROR;
}

CAR_INTERFACE_IMPL(Workspace::InverseZInterpolator, Object, ITimeInterpolator);

Workspace::InverseZInterpolator::InverseZInterpolator(
    /* [in] */ Float foc)
{
    mZInterpolator = new ZInterpolator(foc);
}

ECode Workspace::InverseZInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    Float tmp;
    mZInterpolator->GetInterpolation(1 - input, &tmp);
    *result = 1 - tmp;
    return NOERROR;
}

CAR_INTERFACE_IMPL(Workspace::ZoomOutInterpolator, Object, ITimeInterpolator);

Workspace::ZoomOutInterpolator::ZoomOutInterpolator()
{
    CDecelerateInterpolator::New(0.75f, (IDecelerateInterpolator**)&mDecelerate);
    mZInterpolator = new ZInterpolator(0.13f);
}

ECode Workspace::ZoomOutInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    Float tmp;
    mZInterpolator->GetInterpolation(input, &tmp);
    return mDecelerate->GetInterpolation(tmp, result);
}

CAR_INTERFACE_IMPL(Workspace::ZoomInInterpolator, Object, ITimeInterpolator);

Workspace::ZoomInInterpolator::ZoomInInterpolator()
{
    mInverseZInterpolator = new InverseZInterpolator(0.35f);
    CDecelerateInterpolator::New(3.0f, (IDecelerateInterpolator**)&mDecelerate);
}

ECode Workspace::ZoomInInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    Float tmp;
    mInverseZInterpolator->GetInterpolation(input, &tmp);
    return decelerate->GetInterpolation(tmp, result);
}

CAR_INTERFACE_IMPL(Workspace::FolderCreationAlarmListener, Object, IOnAlarmListener);

Workspace::FolderCreationAlarmListener::FolderCreationAlarmListener(
    /* [in] */ ICellLayout* layout,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Workspace* host)
    : mLayout(layout)
    , mCellX(cellX)
    , mCellY(cellY)
    , mHost(host)
{
}

Ecode Workspace::FolderCreationAlarmListener::OnAlarm(
    /* [in] */ IAlarm* alarm)
{
    if (mHost->mDragFolderRingAnimator == NULL) {
        mHost->mDragFolderRingAnimator = new FolderRingAnimator(mLauncher, NULL);
    }
    mHost->mDragFolderRingAnimator->SetCell(mCellX, mCellY);
    mHost->mDragFolderRingAnimator->SetCellLayout(mLayout);
    mHost->mDragFolderRingAnimator->AnimateToAcceptState();
    mLayout->ShowFolderAccept(mHost->mDragFolderRingAnimator);
    mLayout->ClearDragOutlines();
    return mHost->SetDragMode(DRAG_MODE_CREATE_FOLDER);
}

CAR_INTERFACE_IMPL(Workspace::ReorderAlarmListener, Object, IOnAlarmListener);

Workspace::ReorderAlarmListener::ReorderAlarmListener(
    /* [in] */ ArrayOf<Float>* dragViewCenter,
    /* [in] */ Int32 minSpanX,
    /* [in] */ Int32 minSpanY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IDragView* dragView,
    /* [in] */ IView* child,
    /* [in] */ Workspace* host)
    : mDragViewCenter(dragViewCenter)
    , mMinSpanX(minSpanX)
    , mMinSpanY(minSpanY)
    , mSpanX(spanX)
    , mSpanY(spanY)
    , mChild(child)
    , mDragView(dragView)
    , mHost(host)
{
}

ECode Workspace::ReorderAlarmListener::OnAlarm(
    /* [in] */ IAlarm* alarm)
{
    AutoPtr<ArrayOf<Int32> > resultSpan = ArrayOf<Int32>::Alloc(2);
    mHost->mTargetCell = FindNearestArea((Int32)(*(mHost->mDragViewVisualCenter))[0],
            (Int32)(*(mHost->mDragViewVisualCenter))[1], mSpanX, mSpanY,
            mHost->mDragTargetLayout, mHost->mTargetCell);
    mHost->mLastReorderX = (*(mHost->mTargetCell))[0];
    mHost->mLastReorderY = (*(mHost->mTargetCell))[1];

    mHost->mTargetCell = mHost->mDragTargetLayout->CreateArea(
        (Int32)(*(mHost->mDragViewVisualCenter))[0], (Int32)(*(mHost->mDragViewVisualCenter))[1],
        mMinSpanX, mMinSpanY, mSpanX, mSpanY,
        mChild, mHost->mTargetCell, resultSpan, ICellLayout::MODE_DRAG_OVER);

    if ((*(mHost->mTargetCell))[0] < 0 || (*(mHost->mTargetCell))[1] < 0) {
        mHost->mDragTargetLayout->RevertTempState();
    }
    else {
        SetDragMode(DRAG_MODE_REORDER);
    }

    Boolean resize = (*resultSpan)[0] != mSpanX || (*resultSpan)[1] != mSpanY;
    AutoPtr<IPoint> p = mDragView->GetDragVisualizeOffset()
    AutoPtr<IRect> r = mDragView->GetDragRegion()
    return mHost->mDragTargetLayout->VisualizeDropLocation(mChild, mHost->mDragOutline,
        (Int32)(*(mHost->mDragViewVisualCenter))[0], (Int32)(*(mHost->mDragViewVisualCenter))[1],
        (*(mHost->mTargetCell))[0], (*(mHost->mTargetCell))[1],
        (*resultSpan)[0], (*resultSpan)[1], resize, p, r);
}

Workspace::MyRunnable::MyRunnable(
    /* [in] */ Workspace* host)
    : mHost(host)
{
}

ECode Workspace::MyRunnable::Run()
{
    AutoPtr<ILauncherModel> launcherModel;
    mHost->mLauncher->GetModel((ILauncherModel**)&launcherModel);
    return launcherModel->BindRemainingSynchronousPages();
}

Workspace::MyThread::MyThread(
    /* [in] */ const String& name,
    /* [in] */ Workspace* host)
    : Thread(Srting("setWallpaperDimension"))
    , mHost(host)
{
}

ECode Workspace::MyThread::Run()
{
    return mHost->mWallpaperManager->SuggestDesiredDimensions(mHost->mWallpaperWidth,
            mHost->mWallpaperHeight);
}

CAR_INTERFACE_IMPL(Workspace::MyAnimatorUpdateListener, Object, IAnimatorUpdateListener);

Workspace::MyAnimatorUpdateListener::MyAnimatorUpdateListener(
    /* [in] */ Workspace* host)
    : mHost(host)
{
}

ECode Workspace::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> fvalue = IFloat::Probe(obj);
    Float value;
    fvalue->GetValue(&value);
    mHost->setBackgroundAlpha(value);
}

Workspace::MyLauncherAnimatorUpdateListener::MyLauncherAnimatorUpdateListener(
    /* [in] */ ICellLayout* cl,
    /* [in] */ Float _old,
    /* [in] */ Float _new)
    : mCl(cl)
    , mOld(_old)
    , mNew(_new)
{
}

ECode Workspace::MyLauncherAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ Float a,
    /* [in] */ Float b)
{
    return mCl->SetBackgroundAlpha(a * mOld + b * mNew);
}

Workspace::MyRunnable2::MyRunnable2(
    /* [in] */ Workspace* host,
    /* [in] */ ItemInfo* info,
    /* [in] */ ILauncherAppWidgetHostView* hostView,
    /* [in] */ ICellLayout* cellLayout)
    : mHost(host)
    , mInfo(info)
    , mHostView(hostView)
    , mCellLayout(cellLayout)
{
}

ECode Workspace::MyRunnable2::Run()
{
    AutoPtr<IDragLayer> dragLayer;
    mHost->mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    return dragLayer->AddResizeFrame(mInfo, mHostView, mCellLayout);
}

Workspace::MyRunnable3::MyRunnable3(
    /* [in] */ Workspace* host,
    /* [in] */ IRunnable* addResizeFrame)
    : mHost(host)
    , mAddResizeFrame(addResizeFrame)
{
}

ECode Workspace::MyRunnable3::Run()
{
    Boolean res;
    mHost->IsPageMoving(&res);
    if (!res) {
        mAddResizeFrame->Run();
    }
    else {
        mHost->mDelayedResizeRunnable = mAddResizeFrame;
    }
    return NOERROR;
}

Workspace::MyRunnable4::MyRunnable4(
    /* [in] */ Workspace* host,
    /* [in] */ IRunnable* finalResizeRunnable)
    : mHost(host)
    , mFinalResizeRunnable(finalResizeRunnable)
{
}

ECode Workspace::MyRunnable4::Run()
{
    mHost->mAnimatingViewIntoPlace = FALSE;
    mHost->UpdateChildrenLayersEnabled(FALSE);
    if (mFinalResizeRunnable != NULL) {
        mFinalResizeRunnable->Run();
    }
    return NOERROR;
}

Workspace::MyRunnable5::MyRunnable5(
    /* [in] */ Workspace* host)
    : mHost(host)
{
}

ECode Workspace::MyRunnable5::Run()
{
    return mHost->mLauncher->ExitSpringLoadedDragModeDelayed(TRUE, FALSE, NULL);
}

Workspace::MyRunnabl6::MyRunnabl6(
    /* [in] */ Workspace* host,
    /* [in] */ PendingAddItemInfo* pendingInfo,
    /* [in] */ ItemInfo* item,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ ArrayOf<Int32>* span)
    : mHost(host)
    , mPendingInfo(pendingInfo)
    , mItem(item)
    , mContainer(container)
    , mScreen(screen)
    , mSpan(span)
{
}

ECode Workspace::MyRunnabl6::Run()
{
    // When dragging and dropping from customization tray, we deal with creating
    // widgets/shortcuts/folders in a slightly different way
    switch (mPendingInfo->mItemType) {
        case LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET:
        {
            AutoPtr<ArrayOf<Int32> > span = ArrayOf<Int32>::Alloc(2);
            (*span)[0] = mItem->mSpanX;
            (*span)[1] = mItem->mSpanY;
            mHost->mLauncher->AddAppWidgetFromDrop(IPendingAddWidgetInfo::Probe(pendingInfo),
                    mContainer, mScreen, mHost->mTargetCell, mSpan, NULL);
            break;
        }
        case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
            mHost->mLauncher->ProcessShortcutFromDrop(mPendingInfo->mComponentName,
                    mContainer, mScreen, mHost->mTargetCell, NULL);
            break;
        default:
            // throw new IllegalStateException("Unknown item type: " +
            //         pendingInfo.itemType);
            Logger::E(TAG, "Unknown item type: " + pendingInfo.itemType);
            return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Workspace::MyRunnabl7::MyRunnabl7(
    /* [in] */ Workspace* host,
    /* [in] */ IView* finalView,
    /* [in] */ IRunnable* onCompleteRunnable)
    : mFinalView(finalView)
    , mOnCompleteRunnable(onCompleteRunnable)
{

}

ECode Workspace::MyRunnabl7::Run()
{
    if (mFinalView != NULL) {
        mFinalView->SetVisibility(VISIBLE);
    }
    if (mOnCompleteRunnable != NULL) {
        mOnCompleteRunnable->Run();
    }
    return NOERROR;
}

Workspace::MyRunnabl8::MyRunnabl8(
    /* [in] */ Workspace* host,
    /* [in] */ IViewGroup layout,
    /* [in] */ IHashSet* componentNames,
    /* [in] */ IUserHandle* user,
    /* [in] */ ICellLayout* layoutParent)
    : mHost(host)
    , mLayout(layout)
    , mComponentNames(componentNames)
    , mUser(user)
    , mLayoutParent(layoutParent)
{
}

ECode Workspace::MyRunnabl8::Run()
{
    AutoPtr<IArrayList> childrenToRemove;
    CArrayList::New((IArrayList**)&childrenToRemove);
    childrenToRemove->Clear();

    Int32 childCount;
    mLayout->GetChildCount(&childCount);
    for (Int32 j = 0; j < childCount; j++) {
        AutoPtr<IView> view;
        mLayout->GetChildAt(j, (IView**)&view);
        AutoPtr<IInterface> tag;
        view->GetTag((IInterface**)&tag);
        if ((IShortcutInfo::Probe(tag) != NULL || ILauncherAppWidgetInfo::Probe(tag) != NULL)) {
            AutoPtr<ItemInfo> info = (ItemInfo*)IItemInfo::Probe(tag);
            Boolean res;
            info->mUser->Equals(mUser, &res);
            if (!res) {
                continue;
            }
        }
        if (IShortcutInfo::Probe(tag) != NULL) {
            AutoPtr<ShortcutInfo> info = (ShortcutInfo*)IShortcutInfo::Probe(tag);
            AutoPtr<IIntent> intent = info->mIntent;
            AutoPtr<IComponentName> name;
            intent->GetComponent((IComponentName**)&name);

            if (name != NULL) {
                Boolean res;
                mComponentNames->Contains(name, &res);
                if (res) {
                    LauncherModel::DeleteItemFromDatabase(mHost->mLauncher, info);
                    childrenToRemove->Add(TO_IINTERFACE(view));
                }
            }
        }
        else if (IFolderInfo::Probe(tag) != NULL) {
            AutoPtr<FolderInfo> info = (FolderInfo*)IFolderInfo::Probe(tag)
            AutoPtr<IArrayList> contents = info->mContents;
            Int32 contentsCount;
            contents->GetSize(&contentsCount);
            AutoPtr<IArrayList> appsToRemoveFromFolder;
            CArrayList::New((IArrayList**)&appsToRemoveFromFolder);

            for (Int32 k = 0; k < contentsCount; k++) {
                AutoPtr<IInterface> obj;
                contents->Get(k, (IInterface**)&obj);
                AutoPtr<ShortcutInfo> appInfo = (ShortcutInfo*)IShortcutInfo::Probe(obj);
                AutoPtr<IIntent> intent = appInfo->mIntent;
                AutoPtr<IComponentName> name;
                intent->GetComponent((IComponentName**)&name);

                if (name != NULL) {
                    Boolean res;
                    mComponentNames->Contains(name, &res);
                    Boolean res2;
                    mUser->Equals(appInfo->mUser, &res2);
                    if (res && res2) {
                        appsToRemoveFromFolder->Add(appInfo);
                    }
                }
            }

            Int32 size;
            appsToRemoveFromFolder->GetSize(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                appsToRemoveFromFolder->Get(i, (IInterface**)&obj);
                AutoPtr<IShortcutInfo> item = IShortcutInfo::Probe(obj);

                info->Remove(item);
                LauncherModel::DeleteItemFromDatabase(mHost->mLauncher, item);

            }
        }
        else if (ILauncherAppWidgetInfo::Probe(tag) != NULL) {
            AutoPtr<LauncherAppWidgetInfo> info = (LauncherAppWidgetInfo*)ILauncherAppWidgetInfo::Probe(tag);
            AutoPtr<IComponentName> provider = info->mProviderName;
            if (provider != NULL) {
                Boolean res;
                mComponentNames->Contains(provider, &res);
                if (res) {
                    LauncherModel::DeleteItemFromDatabase(mHost->mLauncher, info);
                    childrenToRemove->Add(view);
                }
            }
        }
    }

    childrenToRemove->GetSize(&childCount);
    for (Int32 j = 0; j < childCount; j++) {
        AutoPtr<IInterface> obj;
        childrenToRemove->Get(j, (IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);
        // Note: We can not remove the view directly from CellLayoutChildren as this
        // does not re-mark the spaces as unoccupied.
        mLayoutParent->RemoveViewInLayout(child);
        if (IDropTarget::Probe(child) != NULL) {
            mHost->mDragController->RemoveDropTarget(IDropTarget::Probe(child));
        }
    }

    if (childCount > 0) {
        mLayout->RequestLayout();
        mLayout->Invalidate();
    }
    return NOERROR;
}

Workspace::MyRunnabl9::MyRunnabl9(
    /* [in] */ IContext* context,
    /* [in] */ IHashSet* componentNames)
    : mContext(context)
    , mComponentNames(componentNames)
{

}

ECode Workspace::MyRunnabl9::Run()
{
    String spKey;
    LauncherApplication::GetSharedPreferencesKey(&spKey);
    AutoPtr<ISharedPreferences> sp;
    mContext->GetSharedPreferences(spKey,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
    AutoPtr<ISet> newApps;
    sp->GetStringSet(IInstallShortcutReceiver::NEW_APPS_LIST_KEY, NULL, (ISet**)&newApps);

    // Remove all queued items that match the same package
    if (newApps != NULL) {
        synchronized(newApps) {
            AutoPtr<IIterator> iter = newApps->GetIterator((IIterator**)&iter);
            Boolean res;
            while (iter->HasNext(&res), res) {
                //try {
                AutoPtr<IInterface> obj;
                FAIL_GOTO(iter->Next((IInterface**)&obj), ERROR)
                AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
                String str;
                cchar->ToString(&str);
                AutoPtr<IIntent> intent;
                FAIL_GOTO(Intent::ParseUri(str, 0, (IIntent**)&intent), ERROR)

                AutoPtr<IComponentName> name;
                FAIL_GOTO(intent->GetComponent((IComponentName**)&name), ERROR)
                Boolean res;
                FAIL_GOTO(mComponentNames->Contains(TO_IINTERFACE(name), &res), ERROR)
                if (res) {
                    FAIL_GOTO(iter->Remove(), ERROR)
                }

                // It is possible that we've queued an item to be loaded, yet it has
                // not been added to the workspace, so remove those items as well.
                AutoPtr<IArrayList> shortcuts;
                FAIL_GOTO(LauncherModel::GetWorkspaceShortcutItemInfosWithIntent(
                        intent, (IArrayList**)&shortcuts), ERROR)
                Int32 size;
                FAIL_GOTO(shortcuts->GetSize(&size), ERROR)
                for (Int32 i = 0; i , size; i++) {
                    AutoPtr<IInterface> obj;
                    FAIL_GOTO(shortcuts->Get(i, (IInterface**)&obj), ERROR)
                    AutoPtr<IItemInfo> info = IItemInfo::Probe(obj);
                    FAIL_GOTO(LauncherModel::DeleteItemFromDatabase(mContext, info), ERROR)
                }
ERROR:
                //} catch (URISyntaxException e) {}
            }
        }
    }
    return NOERROR;
}


const String Workspace::TAG("Launcher.Workspace");

const Float Workspace::WORKSPACE_OVERSCROLL_ROTATION = 24f;

const Int32 Workspace::CHILDREN_OUTLINE_FADE_OUT_DELAY = 0;
const Int32 Workspace::CHILDREN_OUTLINE_FADE_OUT_DURATION = 375;
const Int32 Workspace::CHILDREN_OUTLINE_FADE_IN_DURATION = 100;

const Int32 Workspace::BACKGROUND_FADE_OUT_DURATION = 350;
const Int32 Workspace::ADJACENT_SCREEN_DROP_DURATION = 300;
const Int32 Workspace::FLING_THRESHOLD_VELOCITY = 500;

const Float Workspace::WALLPAPER_SCREENS_SPAN = 2f;

AutoPtr<IRect> Workspace::mLandscapeCellLayoutMetrics;
AutoPtr<IRect> Workspace::mPortraitCellLayoutMetrics;

const Int32 Workspace::DEFAULT_CELL_COUNT_X = 4;
const Int32 Workspace::DEFAULT_CELL_COUNT_Y = 4;

const Int32 Workspace::FOLDER_CREATION_TIMEOUT = 0;
const Int32 Workspace::REORDER_TIMEOUT = 250;

Float Workspace::START_DAMPING_TOUCH_SLOP_ANGLE = (Float) Math.PI / 6;
Float Workspace::MAX_SWIPE_ANGLE = (Float) Math.PI / 3;
Float Workspace::TOUCH_SLOP_DAMPING_FACTOR = 4;

// Related to dragging, folder creation and reordering
const Int32 Workspace::DRAG_MODE_NONE = 0;
const Int32 Workspace::DRAG_MODE_CREATE_FOLDER = 1;
const Int32 Workspace::DRAG_MODE_ADD_TO_FOLDER = 2;
const Int32 Workspace::DRAG_MODE_REORDER = 3;

CAR_INTERFACE_IMPL_7(Workspace, SmoothPagedView, IDropTarget, IDragSource, IDragScroller,
        IViewOnTouchListener, IDragControllerDragListener, ILauncherTransitionable,
        IViewGroupOnHierarchyChangeListener);

Workspace::Workspace()
    : mChildrenOutlineAlpha(0)
    , mDrawBackground(TRUE)
    , mBackgroundAlpha(0)
    , mWallpaperScrollRatio(1.0f)
    , mOriginalPageSpacing(0)
    , mDefaultPage(0)
    , mDragOverX(-1)
    , mDragOverY(-1)
    , mSpringLoadedShrinkFactor(0)
    , mIsSwitchingState(FALSE)
    , mAnimatingViewIntoPlace(FALSE)
    , mIsDragOccuring(FALSE)
    , mChildrenLayersEnabled(TRUE)
    , mInScrollArea(FALSE)
    , mOverscrollFade(0)
    , mOverscrollTransformsSet(FALSE)
    , mWorkspaceFadeInAdjacentScreens(FALSE)
    , mWallpaperWidth(0)
    , mWallpaperHeight(0)
    , mUpdateWallpaperOffsetImmediately(FALSE)
    , mIsStaticWallpaper(FALSE)
    , mWallpaperTravelWidth(0)
    , mSpringLoadedPageSpacing(0)
    , mCameraDistance(0)
    , mCreateUserFolderOnDrop(FALSE)
    , mAddToExistingFolderOnDrop(FALSE)
    , mMaxDistanceForFolderCreation(0)
    , mXDown(0)
    , mYDown(0)
    , mDragMode(DRAG_MODE_NONE)
    , mLastReorderX(-1)
    , mLastReorderY(-1)
    , mSavedScrollX(0)
    , mSavedRotationY(0)
    , mSavedTranslationX(0)
    , mCurrentScaleX(0)
    , mCurrentScaleY(0)
    , mCurrentRotationY(0)
    . mCurrentTranslationX(0)
    , mCurrentTranslationY(0)
    , mTransitionProgress(0)
{
    mTargetCell = ArrayOf<Int32>::Alloc(2);

    mTempCell = ArrayOf<Int32>::Alloc(2);
    mTempEstimate = ArrayOf<Int32>::Alloc(2);
    mDragViewVisualCenter = ArrayOf<Float>::Alloc(2);
    mTempDragCoordinates = ArrayOf<Float>::Alloc(2);
    mTempCellLayoutCenterCoordinates = ArrayOf<Float>::Alloc(2);
    mTempDragBottomRightCoordinates = ArrayOf<Float>::Alloc(2);
    CMatrix::New((IMatrix**)&mTempInverseMatrix);

    mState = State::NORMAL;

    mOutlineHelper = new HolographicOutlineHelper();
    CRect::New((IRect**)&mTempRect);
    mTempXY = ArrayOf<Int32>::Alloc(2);
    mTempVisiblePagesRange = ArrayOf<Int32>::Alloc(2);

    CPoint::New((IPoint**)&mDisplaySize);

    mFolderCreationAlarm = new Alarm();
    mReorderAlarm = new Alarm();

    CArrayList::New((IArrayList**)&mRestoredPages);

    mBindPages = new MyRunnable(this);

    mZoomInInterpolator = new ZoomInInterpolator();
}

ECode Workspace::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode Workspace::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    SmoothPagedView::constructor(context, attrs, defStyle);
    mContentIsRefreshable = FALSE;
    mOriginalPageSpacing = mPageSpacing;

    mDragEnforcer = new DropTarget::DragEnforcer(context);
    // With workspace, data is available straight from the get-go
    SetDataIsReady();

    mLauncher = ILauncher::Probe(context);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetBoolean(
            Elastos::Droid::Launcher2::R::bool::config_workspaceFadeAdjacentScreens,
            &mWorkspaceFadeInAdjacentScreens);
    mFadeInAdjacentScreens = FALSE;
    AutoPtr<IWallpaperManager> wallpaperManager;
    WallpaperManager::GetInstance((IWallpaperManager**)&wallpaperManager);

    Int32 cellCountX = DEFAULT_CELL_COUNT_X;
    Int32 cellCountY = DEFAULT_CELL_COUNT_Y;

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs,
            Elastos::Droid::Launcher2::R::styleable::Workspace, defStyle, 0,
            (ITypedArray**)&a);

    Boolean res;
    LauncherApplication->IsScreenLarge(&res);
    if (res) {
                // Determine number of rows/columns dynamically
        // TODO: This code currently fails on tablets with an aspect ratio < 1.3.
        // Around that ratio we should make cells the same size in portrait and
        // landscape
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
        (*array)[0] = android.R.attr.actionBarSize;
        AutoPtr<ITypedArray> actionBarSizeTypedArray;
        context->ObtainStyledAttributes(array, (ITypedArray**)&actionBarSizeTypedArray);
        actionBarSizeTypedArray->GetDimension(0, 0f, &actionBarHeight);

        AutoPtr<IPoint> minDims;
        CPoint::New((IPoint**)&minDims);
        AutoPtr<IPoint> maxDims;
        CPoint::New((IPoint**)&minDims);
        AutoPtr<IWindowManager> windowManager;
        mLauncher->GetWindowManager((IWindowManager**)&windowManager);
        AutoPtr<IDisplay> display;
        windowManager->GetDefaultDisplay((IDisplay**)&display);
        display>GetCurrentSizeRange(minDims, maxDims);

        cellCountX = 1;
        Int32 width;
        CellLayout::WidthInPortrait(res, cellCountX + 1, &width);
        Int32 x;
        minDims->GetX(&x);
        while (width <= x) {
            cellCountX++;
        }

        cellCountY = 1;
        Int32 height;
        CellLayout::HeightInLandscape(res, cellCountY + 1, &height);
        Int32 y;
        minDims->GetY(&y);
        while (actionBarHeight + height <= y) {
            cellCountY++;
        }
    }

    Int32 tmp;
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_workspaceSpringLoadShrinkPercentage,
            &tmp);
    mSpringLoadedShrinkFactor = tmp / 100.0f;
    res->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::workspace_spring_loaded_page_spacing,
            &mSpringLoadedPageSpacing);
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_cameraDistance,
            &mCameraDistance);

    // if the value is manually specified, use that instead
    a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::Workspace_cellCountX,
            cellCountX, &cellCountX);
    a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::Workspace_cellCountY,
            cellCountY, &cellCountY);
    a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::Workspace_defaultScreen,
            1, &mDefaultPage);
    a->Recycle();

    SetOnHierarchyChangeListener(this);

    LauncherModel::UpdateWorkspaceLayoutCells(cellCountX, cellCountY);
    SetHapticFeedbackEnabled(FALSE);

    InitWorkspace();

    // Disable multitouch across the workspace/all apps/customize tray
    SetMotionEventSplittingEnabled(TRUE);

    // Unless otherwise specified this view is important for accessibility.
    Int32 accessibility;
    GetImportantForAccessibility(&accessibility);
    if (accessibility == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
    return NOERROR;
}

ECode Workspace::EstimateItemSize(
    /* [in] */ Int32 hSpan,
    /* [in] */ Int32 vSpan,
    /* [in] */ IItemInfo* itemInfo,
    /* [in] */ Boolean springLoaded,
    /* [out, callee] */ ArrayOf<Int32>** outarray)
{
    VALIDATE_NOT_NULL(outarray);
    *outarray = NULL;

    AutoPtr<ArrayOf<Int32> > size = ArrayOf<Int32>::Alloc(2);
    Int32 count;
    GetChildCount(&count);
    if (count > 0) {
        AutoPtr<IWorkspace> workspace;
        mLauncher->GetWorkspace((IWorkspace**)&workspace);
        AutoPtr<IView> view;
        workspace->GetChildAt(0, (IView**)&view);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
        AutoPtr<IRect> r;
        EstimateItemPosition(cl, itemInfo, 0, 0, hSpan, vSpan, (IRect**)&r);
        r->GetWidth(&((*size)[0]));
        r->GetHeight(&((*size)[1]));
        if (springLoaded) {
            (*size)[0] *= mSpringLoadedShrinkFactor;
            (*size)[1] *= mSpringLoadedShrinkFactor;
        }
        *outarray = size;
        REFCOUNT_ADD(*outarray);
        return NOERROR;
    }
    else {
        (*size)[0] = Integer.MAX_VALUE;
        (*size)[1] = Integer.MAX_VALUE;
        *outarray = size;
        REFCOUNT_ADD(*outarray);
        return NOERROR;
    }
    return NOERROR;
}

ECode Workspace::EstimateItemPosition(
    /* [in] */ ICellLayout* cl,
    /* [in] */ IItemInfo* pendingInfo,
    /* [in] */ Int32 hCell,
    /* [in] */ Int32 vCell,
    /* [in] */ Int32 hSpan,
    /* [in] */ Int32 vSpan,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    cl->CellToRect(hCell, vCell, hSpan, vSpan, r);
    *rect = r;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode Workspace::OnDragStart(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info,
    /* [in] */ Int32 dragAction)
{
    mIsDragOccuring = TRUE;
    UpdateChildrenLayersEnabled(FALSE);
    mLauncher->LockScreenOrientation();
    SetChildrenBackgroundAlphaMultipliers(1f);
    // Prevent any Un/InstallShortcutReceivers from updating the db while we are dragging
    InstallShortcutReceiver::EnableInstallQueue();
    return UninstallShortcutReceiver::EnableUninstallQueue();
}

ECode Workspace::OnDragEnd()
{
    mIsDragOccuring = FALSE;
    UpdateChildrenLayersEnabled(FALSE);
    mLauncher->UnlockScreenOrientation(FALSE);

    // Re-enable any Un/InstallShortcutReceiver and now process any queued items
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    InstallShortcutReceiver::DisableAndFlushInstallQueue(context);
    return UninstallShortcutReceiver::DisableAndFlushUninstallQueue(context);
}

ECode Workspace::InitWorkspace()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    mCurrentPage = mDefaultPage;
    Launcher::SetScreen(mCurrentPage);
    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
    app->GetIconCache((IIconCache**)&mIconCache);
    SetWillNotDraw(FALSE);
    SetClipChildren(FALSE);
    SetClipToPadding(FALSE);
    SetChildrenDrawnWithCacheEnabled(TRUE);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    //try {
    res->GetDrawable(
            Elastos::Droid::Launcher2::R::drawable::apps_customize_bg,
            (IDrawable**&mBackground);
    //} catch (Resources.NotFoundException e) {
        // In this case, we will skip drawing background protection
    //}

    mWallpaperOffset = new WallpaperOffsetInterpolator();
    AutoPtr<IWindowManager> windowManager;
    mLauncher->GetWindowManager((IWindowManager**)&windowManager);
    AutoPtr<IDisplay> display;
    windowManager->GetDefaultDisplay((IDisplay**)&display);
    display->GetSize(mDisplaySize);
    Int32 x;
    mDisplaySize->GetX(&x);
    Int32 y;
    mDisplaySize->GetY(&y);
    mWallpaperTravelWidth = (int32)(x * WallpaperTravelToScreenWidthRatio(x, y));

    Int32 size;
    res->GetDimensionPixelSize(
        Elastos::Droid::Launcher2::R::dimen::app_icon_size,
        &size)
    mMaxDistanceForFolderCreation = (0.55f * size);
    mFlingThresholdVelocity = (Int32)(FLING_THRESHOLD_VELOCITY * mDensity);
    return NOERROR;
}

ECode Workspace::GetScrollMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);

    *mode = SmoothPagedView::X_LARGE_MODE;
    return NOERROR;
}

ECode Workspace::OnChildViewAdded(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    if (ICellLayout::Probe(child) == NULL) {
        //throw new IllegalArgumentException("A Workspace can only have CellLayout children.");
        Slogger::E(TAG, "A Workspace can only have CellLayout children.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICellLayout> cl = ICellLayout::Probe(child);
    cl->SetOnInterceptTouchListener(this);
    cl->SetClickable(TRUE);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 count;
    GetChildCount(&count);
    String str;
    context->GetString(
            Elastos::Droid::Launcher2::R::string::workspace_description_format,
            count, &str);
    return cl->SetContentDescription(str);
}

ECode Workspace::OnChildViewRemoved(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    return NOERROR;
}

ECode Workspace::ShouldDrawChild(
    /* [in] */ IView* child,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICellLayout> cl = ICellLayout::Probe(child);
    Boolean res;
    SmoothPagedView::ShouldDrawChild(child, &res);
    if (res) {
        AutoPtr<IShortcutAndWidgetContainer> container;
        cl->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
        Float value;
        container->GetAlpha(&value);
        if (value > 0) {
            *result = TRUE;
            return NOERROR;
        }
        cl->GetBackgroundAlpha(&value);
        if (value > 0) {
            *result = TRUE;
            return NOERROR;
        }
        *result = FALSE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Workspace::GetOpenFolder(
    /* [out] */ IFolder** outfolder)
{
    VALIDATE_NOT_NULL(outfolder);

    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    Int32 count;
    dragLayer->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        dragLayer->GetChildAt(i, (IView**)&child);
        if (IFolder::Probe(child) != NULL) {
            AutoPtr<IFolder> folder = IFolder::Probe(child);
            AutoPtr<IFolderInfo> info;
            folder->GetInfo((IFolderInfo**)&info);
            if (info->mOpened) {
                *outfolder = folder;
                REFCOUNT_ADD(*outfolder);
                return NOERROR;
            }
        }
    }
    *outfolder = NULL;
    return NOERROR;
}

ECode Workspace::IsTouchActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTouchState != TOUCH_STATE_REST;
    return NOERROR;
}

ECode Workspace::AddInScreen(
    /* [in] */ IView* child,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY)
{
    return AddInScreen(child, container, screen, x, y, spanX, spanY, FALSE);
}

ECode Workspace::AddInScreen(
    /* [in] */ IView* child,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ Boolean insert)
{
    if (container == LauncherSettings::Favorites::CONTAINER_DESKTOP) {
        if (screen < 0) {
            Slogger::E(TAG, "The screen must be >= 0 and < " + getChildCount()
                + " (was " + screen + "); skipping child");
            return NOERROR;
        }
        Int32 count;
        GetChildCount(&count);
        if (screen >= count) {
            Slogger::E(TAG, "The screen must be >= 0 and < " + getChildCount()
                + " (was " + screen + "); skipping child");
            return NOERROR;
        }
    }

    AutoPtr<ICellLayout> layout;
    if (container == LauncherSettings::Favorites::CONTAINER_HOTSEAT) {
        AutoPtr<IHotseat> hotseat;
        mLauncher->GetHotseat((IHotseat**)&hotseat);
        hotseat->GetLayout((ICellLayout**)&layout);
        child->SetOnKeyListener(NULL);

        // Hide folder title in the hotseat
        if (IFolderIcon::Probe(child) != NULL) {
            IFolderIcon::Probe(child)->SetTextVisible(FALSE);
        }

        if (screen < 0) {
            AutoPtr<IHotseat> hotseat;
            mLauncher->GetHotseat((IHotseat**)&hotseat);
            hotseat->GetOrderInHotseat(x, y, &screen);
        }
        else {
            // Note: We do this to ensure that the hotseat is always laid out in the orientation
            // of the hotseat in order regardless of which orientation they were added
            AutoPtr<IHotseat> hotseat;
            mLauncher->GetHotseat((IHotseat**)&hotseat);
            hotseat->GetCellXFromOrder(screen, &x);
            hotseat->GetCellYFromOrder(screen, &y);
        }
    }
    else {
        // Show folder title if not in the hotseat
        if (IFolderIcon::Probe(child) != NULL) {
            IFolderIcon::Probe(child)->SetTextVisible(TRUE);
        }

        AutoPtr<IView> view;
        GetChildAt(screen, (IView**)&view);
        layout = ICellLayout::Probe(view);
        AutoPtr<IViewOnKeyListener> lis = new IconKeyEventListener();
        child->SetOnKeyListener(lis);
    }

    AutoPtr<ILayoutParams> genericLp;
    child->GetLayoutParams((ILayoutParams**)&genericLp);
    AutoPtr<CellLayout::LayoutParams> lp;
    if (genericLp == NULL || ICellLayoutLayoutParams::Probe(genericLp) == NULL) {
        lp = new CellLayout::LayoutParams(x, y, spanX, spanY);
    }
    else {
        lp = (CellLayout::LayoutParams*)ICellLayoutLayoutParams::Probe(genericLp);
        lp->mCellX = x;
        lp->mCellY = y;
        lp->mCellHSpan = spanX;
        lp->mCellVSpan = spanY;
    }

    if (spanX < 0 && spanY < 0) {
        lp->mIsLockedToGrid = FALSE;
    }

    // Get the canonical child id to uniquely represent this view in this screen
    Int32 childId;
    LauncherModel::GetCellLayoutChildId(container, screen, x, y, spanX, spanY, &childId);
    Boolean markCellsAsOccupied = IFolder::Probe(child) == NULL;
    Boolean res;
    layout->AddViewToCellLayout(child, insert ? 0 : -1, childId, lp, markCellsAsOccupied, &res);
    if (!res) {
        // TODO: This branch occurs when the workspace is adding views
        // outside of the defined grid
        // maybe we should be deleting these items from the LauncherModel?
        Slogger::W(TAG, "Failed to add to item at (" + lp.cellX + "," + lp.cellY + ") to CellLayout");
    }

    if (IFolder::Probe(child) == NULL) {
        child->SetHapticFeedbackEnabled(FALSE);
        child->SetOnLongClickListener(mLongClickListener);
    }
    if (IDropTarget::Probe(child) != NULL) {
        mDragController->AddDropTarget(IDropTarget::Probe(child));
    }
    return NOERROR;
}

Boolean Workspace::HitsPage(
    /* [in] */ Int32 index,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<IView> page;
    GetChildAt(index, (IView**)&page);
    if (page != NULL) {
        AutoPtr<ArrayOf<Float> > localXY = ArrayOf<Float>::Alloc(2);
        (*localXY)[0] = x;
        (*localXY)[1] = y;
        MapPointFromSelfToChild(page, localXY);
        Int32 width;
        page->GetWidth(&width);
        Int32 height;
        page->GetHeight(&height);
        return ((*localXY)[0] >= 0 && (*localXY)[0] < width
                && (*localXY)[1] >= 0 && (*localXY)[1] < height);
    }
    return FALSE;
}

ECode Workspace::HitsPreviousPage(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // mNextPage is set to INVALID_PAGE whenever we are stationary.
    // Calculating "next page" this way ensures that you scroll to whatever page you tap on
    Int32 current = (mNextPage == INVALID_PAGE) ? mCurrentPage : mNextPage;

    // Only allow tap to next page on large devices, where there's significant margin outside
    // the active workspace
    Boolean res;
    LauncherApplication::IsScreenLarge(&res);

    *result = res && HitsPage(current - 1, x, y);
    return NOERROR;
}

ECode Workspace::HitsNextPage(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // mNextPage is set to INVALID_PAGE whenever we are stationary.
    // Calculating "next page" this way ensures that you scroll to whatever page you tap on
    Int32 current = (mNextPage == INVALID_PAGE) ? mCurrentPage : mNextPage;

    // Only allow tap to next page on large devices, where there's significant margin outside
    // the active workspace
    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    return res && HitsPage(current + 1, x, y);
}

ECode Workspace::OnTouch(
    /* [in] */ View v,
    /* [in] */ MotionEvent event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (IsSmall() || !IsFinishedSwitchingState());
    return NOERROR;
}

ECode Workspace::IsSwitchingState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsSwitchingState;
    return NOERROR;
}

ECode Workspace::IsFinishedSwitchingState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = !mIsSwitchingState || (mTransitionProgress > 0.5f);\
    return NOERROR;
}

ECode Workspace::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    return mLauncher->OnWindowVisibilityChanged(visibility);
}

ECode Workspace::DispatchUnhandledMove(
    /* [in] */ IView* focused,
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (IsSmall() || !IsFinishedSwitchingState()) {
        // when the home screens are shrunken, shouldn't allow side-scrolling
        *result = FALSE;
        return NOERROR;
    }
    return SmoothPagedView::DispatchUnhandledMove(focused, direction, result);
}

ECode Workspace::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    ev->GetAction(&action);
    switch (action & IMotionEvent::ACTION_MASK) {
        case MotionEvent::ACTION_DOWN:
            ev->GetX(&mXDown);
            ev->GetY(&mYDown);
            break;
        case MotionEvent::ACTION_POINTER_UP:
        case MotionEvent::ACTION_UP:
            if (mTouchState == TOUCH_STATE_REST) {
                AutoPtr<IView> view;
                GetChildAt(mCurrentPage, (IView**)&view);
                AutoPtr<ICellLayout> currentPage = ICellLayout::Probe(view);
                Boolean res;
                currentPage->LastDownOnOccupiedCell(&res);
                if (!res) {
                    OnWallpaperTap(ev);
                }
            }
    }
    return SmoothPagedView::OnInterceptTouchEvent(ev, result);
}

ECode Workspace::ReinflateWidgetsIfNecessary()
{
    Int32 clCount;
    GetChildCount(&clCount);
    for (Int32 i = 0; i < clCount; i++) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
        AutoPtr<IShortcutAndWidgetContainer> swc;
        cl->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&swc);
        Int32 itemCount;
        swc->GetChildCount(&itemCount);
        for (Int32 j = 0; j < itemCount; j++) {
            AutoPtr<IView> v;
            swc->GetChildAt(j, (IView**)&v);

            AutoPtr<IInterface> tag;
            v->GetTag((IInterface**)&tag);
            if (ILauncherAppWidgetInfo::Probe(tag) ! = NULL) {
                AutoPtr<ILauncherAppWidgetInfo> info = ILauncherAppWidgetInfo::Probe(tag);
                AutoPtr<ILauncherAppWidgetHostView> lahv =
                        ILauncherAppWidgetHostView::Probe(info.hostView);

                if (lahv != NULL) {
                    Boolean res;
                    lahv->OrientationChangedSincedInflation(&res);
                    if (res) {
                        mLauncher->RemoveAppWidget(info);
                        // Remove the current widget which is inflated with the wrong orientation
                        cl->RemoveView(lahv);
                        mLauncher->BindAppWidget(info);
                    }
                }
            }
        }
    }
    return NOERROR;
}

ECode Workspace::DetermineScrollingStart(
    /* [in] */ IMotionEvent* ev)
{
    if (IsSmall()) return NOERROR;
    if (!IsFinishedSwitchingState()) return NOERROR;

    Float x;
    v->GetX(&x)
    Float y;
    v->GetY(&y)
    Float deltaX = Math::Abs(x - mXDown);
    Float deltaY = Math::Abs(y - mYDown);

    if (deltaX == 0f) return NOERROR;

    Float slope = deltaY / deltaX;
    Float theta = (Float)Math::Atan(slope);

    if (deltaX > mTouchSlop || deltaY > mTouchSlop) {
        CancelCurrentPageLongPress();
    }

    if (theta > MAX_SWIPE_ANGLE) {
        // Above MAX_SWIPE_ANGLE, we don't want to ever start scrolling the workspace
        return NOERROR;
    }
    else if (theta > START_DAMPING_TOUCH_SLOP_ANGLE) {
        // Above START_DAMPING_TOUCH_SLOP_ANGLE and below MAX_SWIPE_ANGLE, we want to
        // increase the touch slop to make it harder to begin scrolling the workspace. This
        // results in vertically scrolling widgets to more easily. The higher the angle, the
        // more we increase touch slop.
        theta -= START_DAMPING_TOUCH_SLOP_ANGLE;
        Float extraRatio = (Float)
                Math::Sqrt((theta / (MAX_SWIPE_ANGLE - START_DAMPING_TOUCH_SLOP_ANGLE)));
        return SmoothPagedView::DetermineScrollingStart(ev, 1 + TOUCH_SLOP_DAMPING_FACTOR * extraRatio);
    }
    else {
        // Below START_DAMPING_TOUCH_SLOP_ANGLE, we don't do anything special
        return SmoothPagedView::DetermineScrollingStart(ev);
    }
    return NOERROR;
}

ECode Workspace::OnPageBeginMoving()
{
    SmoothPagedView::OnPageBeginMoving();

    if (IsHardwareAccelerated()) {
        UpdateChildrenLayersEnabled(FALSE);
    }
    else {
        if (mNextPage != INVALID_PAGE) {
            // we're snapping to a particular screen
            EnableChildrenCache(mCurrentPage, mNextPage);
        }
        else {
            // this is when user is actively dragging a particular screen, they might
            // swipe it either left or right (but we won't advance by more than one screen)
            EnableChildrenCache(mCurrentPage - 1, mCurrentPage + 1);
        }
    }

    // Only show page outlines as we pan if we are on large screen
    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    if (res) {
        ShowOutlines();
        AutoPtr<IWallpaperInfo> info;
        mWallpaperManager->GetWallpaperInfo((IWallpaperInfo**)&info);
        mIsStaticWallpaper = info == NULL;
    }

    // If we are not fading in adjacent screens, we still need to restore the alpha in case the
    // user scrolls while we are transitioning (should not affect dispatchDraw optimizations)
    if (!mWorkspaceFadeInAdjacentScreens) {
        Int32 count;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; ++i) {
            AutoPtr<IView> view;
            GetPageAt(i, (IView**)&view);
            ICellLayout::Probe(view)->SetShortcutAndWidgetAlpha(1f);
        }
    }

    // Show the scroll indicator as you pan the page
    return ShowScrollingIndicator(FALSE);
}

ECode Workspace::OnPageEndMoving()
{
    SmoothPagedView::OnPageEndMoving();

    if (IsHardwareAccelerated()) {
        UpdateChildrenLayersEnabled(FALSE);
    }
    else {
        ClearChildrenCache();
    }

    Boolean res;
    mDragController->IsDragging(&res);
    if (res) {
        if (IsSmall()) {
            // If we are in springloaded mode, then force an event to check if the current touch
            // is under a new page (to scroll to)
            mDragController->ForceTouchMove();
        }
    }
    else {
        // If we are not mid-dragging, hide the page outlines if we are on a large screen
        Boolean tmp;
        LauncherApplication::IsScreenLarge(&tmp);
        if (tmp) {
            HideOutlines();
        }

        // Hide the scroll indicator as you pan the page
        mDragController->IsDragging(&tmp)
        if (!tmp) {
            HideScrollingIndicator(FALSE);
        }
    }

    if (mDelayedResizeRunnable != NULL) {
        mDelayedResizeRunnable->Run();
        mDelayedResizeRunnable = NULL;
    }

    if (mDelayedSnapToPageRunnable != NULL) {
        mDelayedSnapToPageRunnable->Run();
        mDelayedSnapToPageRunnable = NULL;
    }
    return NOERROR;
}

ECode Workspace::NotifyPageSwitchListener()
{
    SmoothPagedView::NotifyPageSwitchListener();
    return Launcher::SetScreen(mCurrentPage);
}

Float Workspace::WallpaperTravelToScreenWidthRatio(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Float aspectRatio = width / (Float)height;

    // At an aspect ratio of 16/10, the wallpaper parallax effect should span 1.5 * screen width
    // At an aspect ratio of 10/16, the wallpaper parallax effect should span 1.2 * screen width
    // We will use these two data points to extrapolate how much the wallpaper parallax effect
    // to span (ie travel) at any aspect ratio:

    Float ASPECT_RATIO_LANDSCAPE = 16/10f;
    Float ASPECT_RATIO_PORTRAIT = 10/16f;
    Float WALLPAPER_WIDTH_TO_SCREEN_RATIO_LANDSCAPE = 1.5f;
    Float WALLPAPER_WIDTH_TO_SCREEN_RATIO_PORTRAIT = 1.2f;

    // To find out the desired width at different aspect ratios, we use the following two
    // formulas, where the coefficient on x is the aspect ratio (width/height):
    //   (16/10)x + y = 1.5
    //   (10/16)x + y = 1.2
    // We solve for x and y and end up with a final formula:
    final Float x =
        (WALLPAPER_WIDTH_TO_SCREEN_RATIO_LANDSCAPE - WALLPAPER_WIDTH_TO_SCREEN_RATIO_PORTRAIT) /
        (ASPECT_RATIO_LANDSCAPE - ASPECT_RATIO_PORTRAIT);
    final Float y = WALLPAPER_WIDTH_TO_SCREEN_RATIO_PORTRAIT - x * ASPECT_RATIO_PORTRAIT;
    return x * aspectRatio + y;
}

Int32 Workspace::GetScrollRange()
{
    Int32 count;
    GetChildCount(&count);
    Int32 offset;
    GetChildOffset(count - 1, &offset)
    Int32 offset2;
    GetChildOffset(0, &offset2);
    return offset - offset2;
}

ECode Workspace::SetWallpaperDimension()
{
    AutoPtr<IPoint> minDims;
    CPoint::New((IPoint**)&minDims);
    AutoPtr<IPoint> maxDims;
    CPoint::New((IPoint**)&maxDims);

    AutoPtr<IWindowManager> windowManager;
    mLauncher->GetWindowManager((IWindowManager**)&windowManager);
    AutoPtr<IDisplay> display;
    windowManager->GetDefaultDisplay((IDisplay**)&display);
    display->GetCurrentSizeRange(minDims, maxDims);

    Int32 maxx;
    maxDims->GetX(&maxx);
    Int32 maxy;
    maxDims->GetY(&maxy);
    Int32 maxDim = Math::Max(maxx, maxy);

    Int32 minx;
    minDims->GetX(&minx);
    Int32 miny;
    minDims->GetY(&miny);
    Int32 minDim = Math::Min(minx, miny);

    // We need to ensure that there is enough extra space in the wallpaper for the intended
    // parallax effects
    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    if (Lres) {
        mWallpaperWidth = (Int32)(maxDim * WallpaperTravelToScreenWidthRatio(maxDim, minDim));
        mWallpaperHeight = maxDim;
    }
    else {
        mWallpaperWidth = Math::Max((Int32)(minDim * WALLPAPER_SCREENS_SPAN), maxDim);
        mWallpaperHeight = maxDim;
    }
    AutoPtr<IThread> t = new MyThread(String("setWallpaperDimension"), this);
    return t->Start();
}

Float Workspace::WallpaperOffsetForCurrentScroll()
{
    // Set wallpaper offset steps (1 / (number of screens - 1))
    Int32 count;
    GetChildCount(&count);
    mWallpaperManager->SetWallpaperOffsetSteps(1.0f / (count - 1), 1.0f);

    // For the purposes of computing the scrollRange and overScrollOffset, we assume
    // that mLayoutScale is 1. This means that when we're in spring-loaded mode,
    // there's no discrepancy between the wallpaper offset for a given page.
    Float layoutScale = mLayoutScale;
    mLayoutScale = 1f;
    Int32 scrollRange;
    GetScrollRange(&scrollRange);

    // Again, we adjust the wallpaper offset to be consistent between values of mLayoutScale
    Int32 x;
    GetScrollX(&x);
    Float adjustedScrollX = Math::Max(0, Math::Min(x, mMaxScrollX));
    adjustedScrollX *= mWallpaperScrollRatio;
    mLayoutScale = layoutScale;

    Float scrollProgress =
        adjustedScrollX / (Float)scrollRange;

    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    if (res && mIsStaticWallpaper) {
        // The wallpaper travel width is how far, from left to right, the wallpaper will move
        // at this orientation. On tablets in portrait mode we don't move all the way to the
        // edges of the wallpaper, or otherwise the parallax effect would be too strong.
        Int32 wallpaperTravelWidth = Math::Min(mWallpaperTravelWidth, mWallpaperWidth);

        Float offsetInDips = wallpaperTravelWidth * scrollProgress +
            (mWallpaperWidth - wallpaperTravelWidth) / 2; // center it
        Float offset = offsetInDips / (Float)mWallpaperWidth;
        return offset;
    }
    else {
        return scrollProgress;
    }
}

void Workspace::SyncWallpaperOffsetWithScroll()
{
    Boolean enableWallpaperEffects = IsHardwareAccelerated();
    if (enableWallpaperEffects) {
        mWallpaperOffset->SetFinalX(WallpaperOffsetForCurrentScroll());
    }
}

ECode Workspace::UpdateWallpaperOffsetImmediately()
{
    mUpdateWallpaperOffsetImmediately = TRUE;
    return NOERROR;
}

void Workspace::UpdateWallpaperOffsets()
{
    Boolean updateNow = FALSE;
    Boolean keepUpdating = TRUE;
    if (mUpdateWallpaperOffsetImmediately) {
        updateNow = TRUE;
        keepUpdating = FALSE;
        mWallpaperOffset->JumpToFinal();
        mUpdateWallpaperOffsetImmediately = FALSE;
    }
    else {
        mWallpaperOffset->ComputeScrollOffset(&keepUpdating);
        updateNow = keepUpdating;
    }
    if (updateNow) {
        if (mWindowToken != NULL) {
            Int32 x;
            mWallpaperOffset->GetCurrX(&x);
            Int32 y;
            mWallpaperOffset->GetCurrY(&y);
            mWallpaperManager->SetWallpaperOffsets(mWindowToken, x, y);
        }
    }
    if (keepUpdating) {
        Invalidate();
    }
}

ECode Workspace::UpdateCurrentPageScroll()
{
    SmoothPagedView::UpdateCurrentPageScroll();
    return ComputeWallpaperScrollRatio(mCurrentPage);
}

ECode Workspace::SnapToPage(
    /* [in] */ Int32 whichPage)
{
    SmoothPagedView::SnapToPage(whichPage);
    return ComputeWallpaperScrollRatio(whichPage);
}

ECode Workspace::SnapToPage(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 duration)
{
    SmoothPagedView::SnapToPage(whichPage, duration);
    return ComputeWallpaperScrollRatio(whichPage);
}

ECode Workspace::SnapToPage(
    /* [in] */ Int32 whichPage,
    /* [in] */ IRunnable* r)
{
    if (mDelayedSnapToPageRunnable != NULL) {
        mDelayedSnapToPageRunnable->Run();
    }
    mDelayedSnapToPageRunnable = r;
    return SnapToPage(whichPage, SLOW_PAGE_SNAP_ANIMATION_DURATION);
}

void Workspace::ComputeWallpaperScrollRatio(
    /* [in] */ Int32 page)
{
    // Here, we determine what the desired scroll would be with and without a layout scale,
    // and compute a ratio between the two. This allows us to adjust the wallpaper offset
    // as though there is no layout scale.
    Float layoutScale = mLayoutScale;
    Int32 childOffset;
    GetChildOffset(page, &childOffset);
    Int32 relativeChildOffset;
    GetRelativeChildOffset(page, &relativeChildOffset);
    Int32 scaled = childOffset - relativeChildOffset;
    mLayoutScale = 1.0f;
    Float unscaled = childOffset - relativeChildOffset;
    mLayoutScale = layoutScale;
    if (scaled > 0) {
        mWallpaperScrollRatio = (1.0f * unscaled) / scaled;
    }
    else {
        mWallpaperScrollRatio = 1f;
    }
}

ECode Workspace::ComputeScroll()
{
    SmoothPagedView::ComputeScroll();
    return SyncWallpaperOffsetWithScroll();
}

ECode Workspace::ShowOutlines()
{
    if (!IsSmall() && !mIsSwitchingState) {
        if (mChildrenOutlineFadeOutAnimation != NULL) {
            mChildrenOutlineFadeOutAnimation->Cancel();
        }
        if (mChildrenOutlineFadeInAnimation != NULL) {
            mChildrenOutlineFadeInAnimation->Cancel();
        }
        LauncherAnimUtils::OfFloat(this, String("childrenOutlineAlpha"), 1.0f,
                &mChildrenOutlineFadeInAnimation);
        mChildrenOutlineFadeInAnimation->SetDuration(CHILDREN_OUTLINE_FADE_IN_DURATION);
        return mChildrenOutlineFadeInAnimation->Start();
    }
    return NOERROR;
}

ECode Workspace::HideOutlines()
{
    if (!IsSmall() && !mIsSwitchingState) {
        if (mChildrenOutlineFadeInAnimation != NULL) {
            mChildrenOutlineFadeInAnimation->Cancel();
        }
        if (mChildrenOutlineFadeOutAnimation != NULL) {
            mChildrenOutlineFadeOutAnimation->Cancel();
        }
        LauncherAnimUtils::OfFloat(this, String("childrenOutlineAlpha"), 0.0f,
                &mChildrenOutlineFadeOutAnimation);
        mChildrenOutlineFadeOutAnimation->SetDuration(CHILDREN_OUTLINE_FADE_OUT_DURATION);
        mChildrenOutlineFadeOutAnimation->SetStartDelay(CHILDREN_OUTLINE_FADE_OUT_DELAY);
        return mChildrenOutlineFadeOutAnimation->Start();
    }
}

ECode Workspace::ShowOutlinesTemporarily()
{
    if (!mIsPageMoving && !IsTouchActive()) {
        return SnapToPage(mCurrentPage);
    }
    return NOERROR;
}

ECode Workspace::SetChildrenOutlineAlpha(
    /* [in] */ Float alpha)
{
    mChildrenOutlineAlpha = alpha;
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
        cl->SetBackgroundAlpha(alpha);
    }
    return NOERROR;
}

ECode Workspace::GetChildrenOutlineAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mChildrenOutlineAlpha;
    return NOERROR;
}

ECode Workspace::DisableBackground()
{
    mDrawBackground = FALSE;
    return NOERROR;
}

ECode Workspace::EnableBackground()
{
    mDrawBackground = TRUE;
    return NOERROR;
}

void Workspace::AnimateBackgroundGradient(
    /* [in] */ Float finalAlpha,
    /* [in] */ Boolean animated)
{
    if (mBackground == NULL) return;
    if (mBackgroundFadeInAnimation != NULL) {
        mBackgroundFadeInAnimation->Cancel();
        mBackgroundFadeInAnimation = NULL;
    }
    if (mBackgroundFadeOutAnimation != NULL) {
        mBackgroundFadeOutAnimation->Cancel();
        mBackgroundFadeOutAnimation = NULL;
    }
    Float startAlpha;
    GetBackgroundAlpha(&startAlpha);
    if (finalAlpha != startAlpha) {
        if (animated) {
            LauncherAnimUtils::OfFloat(this, startAlpha, finalAlpha, &mBackgroundFadeOutAnimation);
            AutoPtr<IAnimatorUpdateListener> lis = new MyAnimatorUpdateListener(this);
            mBackgroundFadeOutAnimation->AddUpdateListener(lis);
            AutoPtr<IDecelerateInterpolator> polator;
            CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&polator);
            mBackgroundFadeOutAnimation->SetInterpolator(polator);
            mBackgroundFadeOutAnimation->SetDuration(BACKGROUND_FADE_OUT_DURATION);
            mBackgroundFadeOutAnimation->Start();
        }
        else {
            SetBackgroundAlpha(finalAlpha);
        }
    }
}

ECode Workspace::SetBackgroundAlpha(
    /* [in] */ Float alpha)
{
    if (alpha != mBackgroundAlpha) {
        mBackgroundAlpha = alpha;
        Invalidate();
    }
    return NOERROR;
}

ECode Workspace::GetBackgroundAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mBackgroundAlpha;
    return NOERROR;
}

ECode Workspace::BackgroundAlphaInterpolator(
    /* [in] */ Float r,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Float pivotA = 0.1f;
    Float pivotB = 0.4f;
    if (r < pivotA) {
        *result = 0;
        return NOERROR;
    }
    else if (r > pivotB) {
        *result = 1.0f;
        return NOERROR;
    }
    else {
        *result = (r - pivotA)/(pivotB - pivotA);
        return NOERROR;
    }
    return NOERROR;
}

void Workspace::UpdatePageAlphaValues(
    /* [in] */ Int32 screenCenter)
{
    Boolean isInOverscroll = mOverScrollX < 0 || mOverScrollX > mMaxScrollX;
    if (mWorkspaceFadeInAdjacentScreens &&
            mState == State::NORMAL &&
            !mIsSwitchingState &&
            !isInOverscroll) {
        Int32 count;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> view;
            GetChildAt(i, (IView**)&view);
            AutoPtr<ICellLayout> child = ICellLayout::Probe(view);
            if (child != NULL) {
                Float scrollProgress;
                GetScrollProgress(screenCenter, child, i, &scrollProgress);
                Float alpha = 1 - Math::Abs(scrollProgress);
                AutoPtr<IShortcutAndWidgetContainer> container;
                child->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
                container->SetAlpha(alpha);
                if (!mIsDragOccuring) {
                    Float result;
                    BackgroundAlphaInterpolator(Math::Abs(scrollProgress), &result);
                    child->SetBackgroundAlphaMultiplier(result);
                }
                else {
                    child->SetBackgroundAlphaMultiplier(1f);
                }
            }
        }
    }
}

void Workspace::SetChildrenBackgroundAlphaMultipliers(
    /* [in] */ Float a)
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        AutoPtr<ICellLayout> child = ICellLayout::Probe(view);
        child->SetBackgroundAlphaMultiplier(a);
    }
}

ECode Workspace::ScreenScrolled(
    /* [in] */ Int32 screenCenter)
{
    Boolean isRtl;
    IsLayoutRtl(&isRtl);
    SmoothPagedView::ScreenScrolled(screenCenter);

    UpdatePageAlphaValues(screenCenter);
    EnableHwLayersOnVisiblePages();

    if (mOverScrollX < 0 || mOverScrollX > mMaxScrollX) {
        Int32 index = 0;
        Float pivotX = 0f;
        const Float leftBiasedPivot = 0.25f;
        const Float rightBiasedPivot = 0.75f;
        const Int32 lowerIndex = 0;
        Int32 count;
        GetChildCount(&count);
        const Int32 upperIndex = count - 1;
        if (isRtl) {
            index = mOverScrollX < 0 ? upperIndex : lowerIndex;
            pivotX = (index == 0 ? leftBiasedPivot : rightBiasedPivot);
        }
        else {
            index = mOverScrollX < 0 ? lowerIndex : upperIndex;
            pivotX = (index == 0 ? rightBiasedPivot : leftBiasedPivot);
        }

        AutoPtr<IView> view;
        GetChildAt(index, (IView**)&view);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
        Float scrollProgress;
        GetScrollProgress(screenCenter, cl, index, &scrollProgress);
        Boolean isLeftPage = (isRtl ? index > 0 : index == 0);
        cl->SetOverScrollAmount(Math::Abs(scrollProgress), isLeftPage);
        Float rotation = -WORKSPACE_OVERSCROLL_ROTATION * scrollProgress;
        cl->SetRotationY(rotation);
        SetFadeForOverScroll(Math::Abs(scrollProgress));
        if (!mOverscrollTransformsSet) {
            mOverscrollTransformsSet = TRUE;
            cl->SetCameraDistance(mDensity * mCameraDistance);
            Int32 width;
            cl->GetMeasuredWidth(&width);
            cl->SetPivotX(width * pivotX);
            Int32 height;
            cl->GetMeasuredHeight(&height);
            cl->SetPivotY(height) * 0.5f);
            cl->SetOverscrollTransformsDirty(TRUE);
        }
    }
    else {
        if (mOverscrollFade != 0) {
            SetFadeForOverScroll(0);
        }
        if (mOverscrollTransformsSet) {
            mOverscrollTransformsSet = FALSE;
            AutoPtr<IView> view;
            GetChildAt(0, (IView**)&view);
            ICellLayout::Probe(view)->ResetOverscrollTransforms();

            Int32 count;
            GetChildCount(&count);
            AutoPtr<IView> view2;
            GetChildAt(count - 1, (IView**)&view2);
            ICellLayout::Probe(view2)->ResetOverscrollTransforms();
        }
    }
    return NOERROR;
}

ECode Workspace::OverScroll(
    /* [in] */ Float amount)
{
    return AcceleratedOverScroll(amount);
}

ECode Workspace::OnAttachedToWindow()
{
    SmoothPagedView::OnAttachedToWindow();
    GetWindowToken((IBinder**)&mWindowToken);
    ComputeScroll();
    return mDragController->SetWindowToken(mWindowToken);
}

ECode Workspace::OnDetachedFromWindow()
{
    SmoothPagedView::OnDetachedFromWindow();
    mWindowToken = NULL;
    return NOERROR;
}

ECode Workspace::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Int32 count;
    GetChildCount(&count);
    if (mFirstLayout && mCurrentPage >= 0 && mCurrentPage < count) {
        mUpdateWallpaperOffsetImmediately = TRUE;
    }
    return SmoothPagedView::OnLayout(changed, left, top, right, bottom);
}

ECode Workspace::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    UpdateWallpaperOffsets();

    // Draw the background gradient if necessary
    if (mBackground != NULL && mBackgroundAlpha > 0.0f && mDrawBackground) {
        Int32 alpha = (Int32)(mBackgroundAlpha * 255);
        mBackground->SetAlpha(alpha);
        Int32 x;
        GetScrollX(&x);
        Int32 width;
        GetMeasuredWidth(&width);
        Int32 height;
        GetMeasuredHeight(&height);
        mBackground->SetBounds(x, 0, x + width, height);
        mBackground->Draw(canvas);
    }

    SmoothPagedView::OnDraw(canvas);

    // Call back to LauncherModel to finish binding after the first draw
    Boolean res;
    return Post(mBindPages, &res);
}

ECode Workspace::IsDrawingBackgroundGradient(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (mBackground != NULL && mBackgroundAlpha > 0.0f && mDrawBackground);
    return NOERROR;

}

ECode Workspace::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    mLauncher->IsAllAppsVisible(&res);
    if (!res) {
        AutoPtr<IFolder> openFolder;
        GetOpenFolder((IFolder**)&openFolder);
        if (openFolder != NULL) {
            return openFolder->RequestFocus(direction, previouslyFocusedRect, result);
        }
        else {
            return SmoothPagedView::OnRequestFocusInDescendants(direction,
                    previouslyFocusedRect, result);
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode Workspace::GetDescendantFocusability(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    IsSmall(&res);
    if (res) {
        *result = IViewGroup::FOCUS_BLOCK_DESCENDANTS;
        return NOERROR;
    }
    return SmoothPagedView::GetDescendantFocusability(result);
}

ECode Workspace::AddFocusables(
    /* [in] */ IArrayList* views,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 focusableMode)
{
    Boolean res;
    mLauncher->IsAllAppsVisible(&res);
    if (!res) {
        AutoPtr<IFolder> openFolder;
        GetOpenFolder((IFolder**)&openFolder);
        if (openFolder != NULL) {
            return openFolder->AddFocusables(views, direction);
        }
        else {
            return SmoothPagedView::AddFocusables(views, direction, focusableMode);
        }
    }
    return NOERROR;
}

ECode Workspace::IsSmall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mState == State::SMALL || mState == State::SPRING_LOADED;
    return NOERROR;
}

ECode Workspace::EnableChildrenCache(
    /* [in] */ Int32 fromPage,
    /* [in] */ Int32 toPage)
{
    if (fromPage > toPage) {
        Int32 temp = fromPage;
        fromPage = toPage;
        toPage = temp;
    }

    Int32 screenCount;
    GetChildCount(&screenCount);

    fromPage = Math::Max(fromPage, 0);
    toPage = Math::Min(toPage, screenCount - 1);

    for (Int32 i = fromPage; i <= toPage; i++) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        AutoPtr<ICellLayout> layout = ICellLayout::Probe(view);
        layout->SetChildrenDrawnWithCacheEnabled(TRUE);
        layout->SetChildrenDrawingCacheEnabled(TRUE);
    }
    return NOERROR;
}

ECode Workspace::ClearChildrenCache()
{
    Int32 screenCount;
    GetChildCount(&screenCount);
    for (Int32 i = 0; i < screenCount; i++) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        AutoPtr<ICellLayout> layout = ICellLayout::Probe(view);
        layout->SetChildrenDrawnWithCacheEnabled(FALSE);
        // In software mode, we don't want the items to continue to be drawn into bitmaps
        Boolean res;
        IsHardwareAccelerated(&res);
        if (!res) {
            layout->SetChildrenDrawingCacheEnabled(FALSE);
        }
    }
    return NOERROR;
}

void Workspace::UpdateChildrenLayersEnabled(
    /* [in] */ Boolean force)
{
    Boolean small = mState == State::SMALL || mIsSwitchingState;
    Boolean res;
    IsPageMoving(&res);
    Boolean enableChildrenLayers = force || small || mAnimatingViewIntoPlace || res;

    if (enableChildrenLayers != mChildrenLayersEnabled) {
        mChildrenLayersEnabled = enableChildrenLayers;
        if (mChildrenLayersEnabled) {
            EnableHwLayersOnVisiblePages();
        }
        else {
            Int32 count;
            GetPageCount(&count);
            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IView> view;
                GetChildAt(i, (IView**)&view);
                AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
                cl->DisableHardwareLayers();
            }
        }
    }
}

void Workspace::EnableHwLayersOnVisiblePages()
{
    if (mChildrenLayersEnabled) {
        Int32 screenCount;
        GetChildCount(&screenCount);
        GetVisiblePages(mTempVisiblePagesRange);
        Int32 leftScreen = (*mTempVisiblePagesRange)[0];
        Int32 rightScreen = (*mTempVisiblePagesRange)[1];
        if (leftScreen == rightScreen) {
            // make sure we're caching at least two pages always
            if (rightScreen < screenCount - 1) {
                rightScreen++;
            }
            else if (leftScreen > 0) {
                leftScreen--;
            }
        }
        for (Int32 i = 0; i < screenCount; i++) {
            AutoPtr<IView> view;
            GetPageAt(i, (IView**)&view);
            AutoPtr<ICellLayout> layout = ICellLayout::Probe(view);
            Boolean res;
            ShouldDrawChild(layout, &res);
            if (!(leftScreen <= i && i <= rightScreen && res)) {
                layout->DisableHardwareLayers();
            }
        }
        for (Int32 i = 0; i < screenCount; i++) {
            AutoPtr<IView> view;
            GetPageAt(i, (IView**)&view);
            AutoPtr<ICellLayout> layout = ICellLayout::Probe(view);
            Boolean res;
            ShouldDrawChild(layout, &res);
            if (leftScreen <= i && i <= rightScreen && res) {
                layout->EnableHardwareLayers();
            }
        }
    }
}

ECode Workspace::BuildPageHardwareLayers()
{
    // force layers to be enabled just for the call to buildLayer
    UpdateChildrenLayersEnabled(TRUE);

    AutoPtr<IBinder> binder;
    GetWindowToken((IBinder**)&binder);
    if (binder != NULL) {
        Int32 childCount;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> view;
            GetChildAt(i, (IView**)&view);
            AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
            cl->BuildHardwareLayer();
        }
    }
    return UpdateChildrenLayersEnabled(FALSE);
}

ECode Workspace::OnWallpaperTap(
        /* [in] */ IMotionEvent* ev)
{
    AutoPtr<ArrayOf<Int32> > position = mTempCell;
    GetLocationOnScreen(position);

    Int32 pointerIndex;
    ev->GetActionIndex(&pointerIndex);
    Float x;
    ev->getX(pointerIndex, &x)
    (*position)[0] += (Int32)x;
    Float y
    ev->GetY(pointerIndex, &y);
    (*position)[1] += (Int32)y;

    Int32 action;
    ev->GetAction(&action);
    AutoPtr<IBinder> binder;
    GetWindowToken((IBinder**)&binder);
    return mWallpaperManager->SendWallpaperCommand(binder,
            action == IMotionEvent::ACTION_UP
            ? IWallpaperManager::COMMAND_TAP : IWallpaperManager::COMMAND_SECONDARY_TAP,
            (*position)[0], (*position)[1], 0, NULL);
}

ECode Workspace::OnDragStartedWithItem(
    /* [in] */ IView* v)
{
    AutoPtr<ICanvas> canvas;
    CCanvas::New((ICanvas**)&canvas);

    // The outline is used to visualize where the item will land if dropped
    return CreateDragOutline(v, canvas, DRAG_BITMAP_PADDING, (IBitmap**)&mDragOutline);
}

ECode Workspace::OnDragStartedWithItem(
    /* [in] */ IPendingAddItemInfo* info,
    /* [in] */ IBitmap* b,
    /* [in] */ Boolean clipAlpha)
{
    AutoPtr<ICanvas> canvas;
    CCanvas::New((ICanvas**)&canvas);

    AutoPtr<ArrayOf<Int32> > size;
    EstimateItemSize(info.spanX, info.spanY, info, FALSE, (ArrayOf<Int32>**)&size);

    // The outline is used to visualize where the item will land if dropped
    return CreateDragOutline(b, canvas, DRAG_BITMAP_PADDING, (*size)[0],
            (*size)[1], clipAlpha, (IBitmap**)&mDragOutline);
}

ECode Workspace::ExitWidgetResizeMode()
{
    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    return dragLayer->ClearAllResizeFrames();
}

void Workspace::InitAnimationArrays()
{
    Int32 childCount;
    GetChildCount(&childCount);
    if (mOldTranslationXs != NULL) return;
    mOldTranslationXs = ArrayOf<Float>Alloc(childCount);
    mOldTranslationYs = ArrayOf<Float>Alloc(childCount);
    mOldScaleXs = ArrayOf<Float>Alloc(childCount);
    mOldScaleYs = ArrayOf<Float>Alloc(childCount);
    mOldBackgroundAlphas = ArrayOf<Float>Alloc(childCount);
    mOldAlphas = ArrayOf<Float>Alloc(childCount);
    mNewTranslationXs = ArrayOf<Float>Alloc(childCount);
    mNewTranslationYs = ArrayOf<Float>Alloc(childCount);
    mNewScaleXs = ArrayOf<Float>Alloc(childCount);
    mNewScaleYs = ArrayOf<Float>Alloc(childCount);
    mNewBackgroundAlphas = ArrayOf<Float>Alloc(childCount);
    mNewAlphas = ArrayOf<Float>Alloc(childCount);
    mNewRotationYs = ArrayOf<Float>Alloc(childCount);
}

ECode Workspace::GetChangeStateAnimation(
    /* [in] */ State state,
    /* [in] */ Boolean animated,
    /* [out] */ IAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);

    return GetChangeStateAnimation(state, animated, 0, animator);
}

ECode Workspace::GetChangeStateAnimation(
    /* [in] */ State state,
    /* [in] */ Boolean animated,
    /* [in] */ Int32 delay,
    /* [out] */ IAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);
    *animator = NULL;

    if (mState == state) {
        *animator = NULL;
        return NOERROR;
    }

    // Initialize animation arrays for the first time if necessary
    InitAnimationArrays();

    AutoPtr<IAnimatorSet> anim;
    if (animated) {
        LauncherAnimUtils::CreateAnimatorSet((IAnimatorSet**)&anim);
    }
    else {
        anim = NULL;
    }

    // Stop any scrolling, move to the current page right away
    Int32 page;
    GetNextPage(&page);
    SetCurrentPage(page);

    State oldState = mState;
    const Boolean oldStateIsNormal = (oldState == State::NORMAL);
    const Boolean oldStateIsSpringLoaded = (oldState == State::SPRING_LOADED);
    const Boolean oldStateIsSmall = (oldState == State::SMALL);
    mState = state;
    const Boolean stateIsNormal = (state == State::NORMAL);
    const Boolean stateIsSpringLoaded = (state == State::SPRING_LOADED);
    const Boolean stateIsSmall = (state == State::SMALL);
    Float finalScaleFactor = 1.0f;
    Float finalBackgroundAlpha = stateIsSpringLoaded ? 1.0f : 0f;
    Float translationX = 0;
    Float translationY = 0;
    Boolean zoomIn = TRUE;

    if (state != State::NORMAL) {
        finalScaleFactor = mSpringLoadedShrinkFactor - (stateIsSmall ? 0.1f : 0);
        SetPageSpacing(mSpringLoadedPageSpacing);
        if (oldStateIsNormal && stateIsSmall) {
            zoomIn = FALSE;
            SetLayoutScale(finalScaleFactor);
            UpdateChildrenLayersEnabled(FALSE);
        }
        else {
            finalBackgroundAlpha = 1.0f;
            SetLayoutScale(finalScaleFactor);
        }
    }
    else {
        SetPageSpacing(mOriginalPageSpacing);
        SetLayoutScale(1.0f);
    }

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Int32 duration;
    if (zoomIn) {
        resources->GetInteger(
                Elastos::Droid::Launcher2::R::integer::config_workspaceUnshrinkTime,
                &duration);
    }
    else {
        resources->GetInteger(
                Elastos::Droid::Launcher2::R::integer::config_appsCustomizeWorkspaceShrinkTime,
                &duration);
    }

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
        Float finalAlpha = (!mWorkspaceFadeInAdjacentScreens || stateIsSpringLoaded ||
                (i == mCurrentPage)) ? 1f : 0f;
        AutoPtr<IShortcutAndWidgetContainer> container;
        cl->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
        Float currentAlpha;
        container->GetAlpha(&currentAlpha);
        Float initialAlpha = currentAlpha;

        // Determine the pages alpha during the state transition
        if ((oldStateIsSmall && stateIsNormal) ||
            (oldStateIsNormal && stateIsSmall)) {
            // To/from workspace - only show the current page unless the transition is not
            //                     animated and the animation end callback below doesn't run;
            //                     or, if we're in spring-loaded mode
            if (i == mCurrentPage || !animated || oldStateIsSpringLoaded) {
                finalAlpha = 1f;
            }
            else {
                initialAlpha = 0f;
                finalAlpha = 0f;
            }
        }

        (mOldAlphas)[i] = initialAlpha;
        (mNewAlphas)[i] = finalAlpha;
        if (animated) {
            cl->GetTranslationX(&((*mOldTranslationXs)[i]));
            cl->GetTranslationY(&((*mOldTranslationYs)[i]));
            cl->GetScaleX(&((*mOldScaleXs)[i]));
            cl->GetScaleY(&((*mOldScaleYs)[i]));
            cl->GetBackgroundAlpha(&((*mOldBackgroundAlphas)[i]));

            (*mNewTranslationXs)[i] = translationX;
            (*mNewTranslationYs)[i] = translationY;
            (*mNewScaleXs)[i] = finalScaleFactor;
            (*mNewScaleYs)[i] = finalScaleFactor;
            (*mNewBackgroundAlphas)[i] = finalBackgroundAlpha;
        }
        else {
            cl->SetTranslationX(translationX);
            cl->SetTranslationY(translationY);
            cl->SetScaleX(finalScaleFactor);
            cl->SetScaleY(finalScaleFactor);
            cl->SetBackgroundAlpha(finalBackgroundAlpha);
            cl->SetShortcutAndWidgetAlpha(finalAlpha);
        }
    }

    if (animated) {
        Int32 count;
        GetChildCount(&count);
        for (Int32 index = 0; index < count; index++) {
            Int32 i = index;
            AutoPtr<IView> view;
            GetChildAt(i, (IView**)&view);
            AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
            AutoPtr<IShortcutAndWidgetContainer> container;
            cl->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
            Float currentAlpha;
            container->GetAlpha(&currentAlpha);

            if ((*mOldAlphas)[i] == 0 && (*mNewAlphas)[i] == 0) {
                cl->SetTranslationX((*mNewTranslationXs)[i]);
                cl->SetTranslationY((*mNewTranslationYs)[i]);
                cl->SetScaleX((*mNewScaleXs)[i]);
                cl->SetScaleY((*mNewScaleYs)[i]);
                cl->SetBackgroundAlpha((*mNewBackgroundAlphas)[i]);
                cl->SetShortcutAndWidgetAlpha((*mNewAlphas)[i]);
                cl->SetRotationY((*mNewRotationYs)[i]);
            }
            else {
                AutoPtr<IAnimator> a = new LauncherViewPropertyAnimator(cl);
                a->TranslationX((*mNewTranslationXs)[i])
                a->TranslationY((*mNewTranslationYs)[i])
                a->ScaleX((*mNewScaleXs)[i])
                a->ScaleY((*mNewScaleYs)[i])
                a->SetDuration(duration)
                a->SetInterpolator(mZoomInInterpolator);
                AutoPtr<IAnimatorSetBuilder> builder;
                anim->Play(a, (IAnimatorSetBuilder**)&builder);

                if ((*mOldAlphas)[i] != (*mNewAlphas)[i] || currentAlpha != (*mNewAlphas)[i]) {
                    AutoPtr<IShortcutAndWidgetContainer> container;
                    cl->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
                    AutoPtr<IAnimator> alphaAnim = new LauncherViewPropertyAnimator(container);
                    alphaAnim->Alpha((*mNewAlphas)[i])
                    alphaAnim->SetDuration(duration)
                    alphaAnim->SetInterpolator(mZoomInInterpolator);
                    AutoPtr<IAnimatorSetBuilder> builder;
                    anim->Play(alphaAnim, (IAnimatorSetBuilder**)&builder);
                }
                if ((*mOldBackgroundAlphas)[i] != 0 ||
                    (*mNewBackgroundAlphas)[i] != 0) {
                    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
                    (*array)[0] = 0f;
                    (*array)[1] = 1f;
                    AutoPtr<IValueAnimator> bgAnim = LauncherAnimUtils::OfFloat(cl, array);
                    bgAnim->SetDuration(duration);
                    bgAnim->SetInterpolator(mZoomInInterpolator);
                    AutoPtr<IAnimatorUpdateListener> lis =
                            new MyLauncherAnimatorUpdateListener(cl, (*mOldBackgroundAlphas)[i],
                            (*mNewBackgroundAlphas)[i]);
                    bgAnim->AddUpdateListener(lis);
                    AutoPtr<IAnimatorSetBuilder> builder;
                    anim->Play(bgAnim, (IAnimatorSetBuilder**)&builder);
                }
            }
        }
        anim->SetStartDelay(delay);
    }

    if (stateIsSpringLoaded) {
        // Right now we're covered by Apps Customize
        // Show the background gradient immediately, so the gradient will
        // be showing once AppsCustomize disappears
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        Int32 value;
        resources->GetInteger(
                Elastos::Droid::Launcher2::R::integer::config_appsCustomizeSpringLoadedBgAlpha,
                &value);
        AnimateBackgroundGradient(value / 100f, FALSE);
    }
    else {
        // Fade the background gradient away
        AnimateBackgroundGradient(0f, true);
    }
    *animator = anim;
    REFCOUNT_ADD(*animator);
    return NOERROR;
}

ECode Workspace::OnLauncherTransitionPrepare(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    mIsSwitchingState = TRUE;
    UpdateChildrenLayersEnabled(FALSE);
    return CancelScrollingIndicatorAnimations();
}

ECode Workspace::OnLauncherTransitionStart(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    return NOERROR;
}

ECode Workspace::OnLauncherTransitionStep(
    /* [in] */ ILauncher* l,
    /* [in] */ Float t)
{
    mTransitionProgress = t;
    return NOERROR;
}

ECode Workspace::OnLauncherTransitionEnd(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    mIsSwitchingState = FALSE;
    mWallpaperOffset->SetOverrideHorizontalCatchupConstant(FALSE);
    UpdateChildrenLayersEnabled(FALSE);
    // The code in getChangeStateAnimation to determine initialAlpha and finalAlpha will ensure
    // ensure that only the current page is visible during (and subsequently, after) the
    // transition animation.  If fade adjacent pages is disabled, then re-enable the page
    // visibility after the transition animation.
    if (!mWorkspaceFadeInAdjacentScreens) {
        Int32 count;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> view;
            GetChildAt(i, (IView**)&view);
            AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
            cl->SetShortcutAndWidgetAlpha(1f);
        }
    }
    return NOERROR;
}

ECode Workspace::GetContent(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = IView::Probe(this);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

void Workspace::DrawDragView(
    /* [in] */ IView* v,
    /* [in] */ ICanvas* destCanvas,
    /* [in] */ Int32 padding,
    /* [in] */ Boolean pruneToDrawable)
{
    AutoPtr<IRect> clipRect = mTempRect;
    v->GetDrawingRect(clipRect);

    Boolean textVisible = FALSE;

    destCanvas->Save();
    if (ITextView::Probe(v) != NULL && pruneToDrawable) {
        AutoPtr<ArrayOf<IDrawable*> > drawables;
        ITextView::Probe(v)->GetCompoundDrawables((ArrayOf<IDrawable*>**)&drawables);
        AutoPtr<IDrawable> d = (*drawables)[1];
        Int32 width;
        d->GetIntrinsicWidth(&width);
        Int32 height;
        d->GetIntrinsicHeight(&height);
        clipRect->Set(0, 0, width + padding, height + padding);
        destCanvas->Translate(padding / 2, padding / 2);
        d->Draw(destCanvas);
    }
    else {
        if (IFolderIcon::Probe(v) != NULL) {
            // For FolderIcons the text can bleed into the icon area, and so we need to
            // hide the text completely (which can't be achieved by clipping).
            Boolean res;
            IFolderIcon::Probe(v)->GetTextVisible(&res);
            if (res) {
                IFolderIcon::Probe(v)->SetTextVisible(FALSE);
                textVisible = true;
            }
        }
        else if (IBubbleTextView::Probe(v) != NULL) {
            AutoPtr<IBubbleTextView> tv = IBubbleTextView::Probe(v);
            Int32 top;
            tv->GetExtendedPaddingTop(&top);
            AutoPtr<ILayout> layout;
            ITextView::Probe(tv)->GetLayout((ILayout**)&layout);
            Int32 ltop;
            layout->GetLineTop(0, &ltop);
            clipRect->SetBottom(top - (Int32)IBubbleTextView::PADDING_V + ltop);
        }
        else if (ITextView::Probe(v) != NULL) {
            AutoPtr<ITextView> tv = ITextView::Probe(v);
            Int32 top;
            tv->GetExtendedPaddingTop(&top);
            Int32 padding;
            tv->GetCompoundDrawablePadding(&padding);
            AutoPtr<ILayout> layout;
            tv->GetLayout((ILayout**)&layout);
            Int32 ltop;
            layout->GetLineTop(0, &ltop);
            clipRect->SetBottom(top - padding + ltop);
        }
        Int32 x;
        v->GetScrollX(&x);
        Int32 y;
        v->GetScrollY(&y);
        destCanvas->Translate(-x + padding / 2, -y + padding / 2);
        destCanvas->ClipRect(clipRect, Op.REPLACE);
        v->Draw(destCanvas);

        // Restore text visibility of FolderIcon if necessary
        if (textVisible) {
            IFolderIcon::Probe(v)->SetTextVisible(TRUE);
        }
    }
    destCanvas->Restore();
}

ECode Workspace::CreateDragBitmap(
    /* [in] */ IView* v,
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 padding,
    /* [out] */ IBitmap** map)
{
    VALIDATE_NOT_NULL(map);

    AutoPtr<IBitmap> b;

    if (ITextView::Probe(v) != NULL) {
        AutoPtr<ArrayOf<IDrawable*> > drawables;
        ITextView::Probe(v)->GetCompoundDrawables((ArrayOf<IDrawable*>**)&drawables);
        AutoPtr<IDrawable> d = (*drawables)[1];

        Int32 width;
        d->GetIntrinsicWidth(&width);
        Int32 height;
        d->GetIntrinsicHeight(&height);
        b = Bitmap::CreateBitmap(width + padding, height + padding,
                IBitmap::Config::ARGB_8888, (IBitmap**)&b);
    }
    else {
        Int32 width;
        v->GetWidth(&width);
        Int32 height;
        v->GetHeight(&height);
        b = Bitmap::CreateBitmap(width + padding, height + padding,
                IBitmap::Config::ARGB_8888, (IBitmap**)&b);
    }

    canvas->SetBitmap(b);
    DrawDragView(v, canvas, padding, TRUE);
    canvas->SetBitmap(NULL);

    *map = b;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

AutoPtr<IBitmap> Workspace::CreateDragOutline(
    /* [in] */ IView* v,
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 padding)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Int32 outlineColor;
    resources->GetColor(
        android.R.color.white,
        &outlineColor);

    Int32 width;
    v->GetWidth(&width);
    Int32 height;
    v->GetHeight(&height);
    AutoPtr<IBitmap> b;
    Bitmap::CreateBitmap(width + padding, height + padding,
            IBitmap::Config::ARGB_8888, (IBitmap**)&b);

    canvas->SetBitmap(b);
    DrawDragView(v, canvas, padding, TRUE);
    mOutlineHelper->ApplyMediumExpensiveOutlineWithBlur(b, canvas, outlineColor, outlineColor);
    canvas->SetBitmap(NULL);
    return b;
}

AutoPtr<IBitmap> Workspace::CreateDragOutline(
    /* [in] */ IBitmap* orig,
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 padding,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Boolean clipAlpha)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Int32 outlineColor;
    resources->GetColor(
        android.R.color.white,
        &outlineColor);

    AutoPtr<IBitmap> b;
    Bitmap::CreateBitmap(w, h, IBitmap::Config::ARGB_8888, (IBitmap**)&b);
    canvas->SetBitmap(b);

    Int32 width;
    orig->GetWidth(&width);
    Int32 height;
    orig->GetHeight(&height);
    AutoPtr<IRect> src;
    CRect::New(0, 0, width, height, (IRect**)&src);

    Float scaleFactor = Math::Min((w - padding) / (Float)width,
            (h - padding) / (Float)height);
    Int32 scaledWidth = (Int32)(scaleFactor * width);
    Int32 scaledHeight = (Int32)(scaleFactor * height);
    AutoPtr<IRect> dst;
    CRect::New(0, 0, scaledWidth, scaledHeight, (IRect**)&dst);

    // center the image
    dst->Offset((w - scaledWidth) / 2, (h - scaledHeight) / 2);

    canvas->DrawBitmap(orig, src, dst, NULL);
    mOutlineHelper->ApplyMediumExpensiveOutlineWithBlur(b, canvas, outlineColor, outlineColor,
            clipAlpha);
    canvas->SetBitmap(NULL);

    return b;
}

ECode Workspace::StartDrag(
    /* [in] */ ICellLayoutCellInfo* cellInfo)
{
    AutoPtr<IView> child = cellInfo->mCell;

    // Make sure the drag was started by a long press as opposed to a long click.
    Boolean res;
    child->IsInTouchMode(&res);
    if (!res) {
        return NOERROR;
    }

    mDragInfo = cellInfo;
    child->SetVisibility(INVISIBLE);
    AutoPtr<IViewParent> parent;
    child->GetParent((IViewParent**)&parent);
    AutoPtr<IViewParent> parent2;
    parent->GetParent((IViewParent**)&parent2);
    AutoPtr<ICellLayout> layout = ICellLayout::Probe(parent2);
    layout->PrepareChildForDrag(child);

    child->ClearFocus();
    child->SetPressed(FALSE);

    AutoPtr<ICanvas> canvas;
    CCanvas::New((ICanvas**)&canvas);

    // The outline is used to visualize where the item will land if dropped
    CreateDragOutline(child, canvas, DRAG_BITMAP_PADDING, (IBitmap**)&mDragOutline);
    return BeginDragShared(child, this);
}


ECode Workspace::BeginDragShared(
    /* [in] */ IView* child,
    /* [in] */ IDragSource* source)
{
    AutoPtr<IResources> r;
    GetResources((IResources**)&r);

    // The drag bitmap follows the touch point around on the screen
    AutoPtr<ICanvas> canvas;
    CCanvas::New((ICanvas**)&canvas);
    AutoPtr<IBitmap> b;
    CreateDragBitmap(child, canvas, DRAG_BITMAP_PADDING, (IBitmap**)&b);

    Int32 bmpWidth;
    b->GetWidth(&bmpWidth);
    Int32 bmpHeight;
    b->GetHeight(&bmpHeight);

    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    Float scale;
    dragLayer->GetLocationInDragLayer(child, mTempXY, &scale);
    Int32 width;
    child->GetWidth(&width);
    Int32 dragLayerX =
            Math::Round((*mTempXY)[0] - (bmpWidth - scale * width) / 2);
    Int32 dragLayerY =
            Math::Round((*mTempXY)[1] - (bmpHeight - scale * bmpHeight) / 2
                    - DRAG_BITMAP_PADDING / 2);

    AutoPtr<IPoint> dragVisualizeOffset;
    AutoPtr<IRect> dragRect;
    if (IBubbleTextView::Probe(child) != NULL || IPagedViewIcon::Probe(child) != NULL) {
        Int32 iconSize;
        r->GetDimensionPixelSize(
                Elastos::Droid::Launcher2::R::dimen::app_icon_size,
                &iconSize);
        Int32 iconPaddingTop;
        r->GetDimensionPixelSize(
                Elastos::Droid::Launcher2::R::dimen::app_icon_padding_top,
                &iconPaddingTop);
        Int32 top;
        child->GetPaddingTop(&top);
        Int32 left = (bmpWidth - iconSize) / 2;
        Int32 right = left + iconSize;
        Int32 bottom = top + iconSize;
        dragLayerY += top;
        // Note: The drag region is used to calculate drag layer offsets, but the
        // dragVisualizeOffset in addition to the dragRect (the size) to position the outline.
        dragVisualizeOffset;
        CPoint::New(-DRAG_BITMAP_PADDING / 2,
                iconPaddingTop - DRAG_BITMAP_PADDING / 2, (Point**)&dragVisualizeOffset);
        CRect::(left, top, right, bottom, (Rect**)&dragRect);
    }
    else if (IFolderIcon::Probe(child) != NULL) {
        Int32 previewSize;
        r->GetDimensionPixelSize(
                Elastos::Droid::Launcher2::R::dimen::folder_preview_size,
                &previewSize);
        Int32 width;
        child->GetWidth(&width);
        CRect::New(0, 0, width, previewSize, (IRect**)&dragRect);
    }

    // Clear the pressed state if necessary
    if (IBubbleTextView::Probe(child) != NULL) {
        AutoPtr<IBubbleTextView> icon = IBubbleTextView::Probe(child);
        icon->ClearPressedOrFocusedBackground();
    }

    AutoPtr<IInterface> tag;
    child->GetTag((IInterface**)&tag);
    mDragController->StartDrag(b, dragLayerX, dragLayerY, source, tag,
            IDragController::DRAG_ACTION_MOVE, dragVisualizeOffset, dragRect, scale);
    b->Recycle();

    // Show the scrolling indicator when you pick up an item
    return ShowScrollingIndicator(FALSE);
}

ECode Workspace::AddApplicationShortcut(
    /* [in] */ IShortcutInfo* info,
    /* [in] */ ICellLayout* target,
    /* [in] */ Int64 container,
    /* [in] */ Int32 screen,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY,
    /* [in] */ Boolean insertAtFirst,
    /* [in] */ Int32 intersectX,
    /* [in] */ Int32 intersectY)
{
    AutoPtr<IView> view;
    mLauncher->CreateShortcut(Elastos::Droid::Launcher2::R::layout:application, target,
        IShortcutInfo::Probe(info), (IView**)&view);

    AutoPtr<ArrayOf<Int32> > cellXY = ArrayOf<Int32>::Alloc(2);
    target->FindCellForSpanThatIntersects(cellXY, 1, 1, intersectX, intersectY);
    AddInScreen(view, container, screen, (*cellXY)[0], (*cellXY)[1], 1, 1, insertAtFirst);
    return LauncherModel::AddOrMoveItemInDatabase(mLauncher, info, container, screen, (*cellXY)[0],
            (*cellXY)[1]);
}

ECode Workspace::TransitionStateShouldAllowDrop(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    IsSwitchingState(&res);
    *result = ((!res || mTransitionProgress > 0.5f) && mState != State::SMALL);
    return NOERROR;
}

ECode Workspace::AcceptDrop(
    /* [in] */ IDragObject* d,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // If it's an external drop (e.g. from All Apps), check if it should be accepted
    AutoPtr<ICellLayout> dropTargetLayout = mDropToLayout;
    if (TO_IINTERFACE(d->mDragSource) != TO_IINTERFACE(this)) {
        // Don't accept the drop if we're not over a screen at time of drop
        if (dropTargetLayout == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        Boolean res;
        TransitionStateShouldAllowDrop(&res);
        if (!res) {
            *result = FALSE;
            return NOERROR;
        }

        mDragViewVisualCenter = GetDragViewVisualCenter(d->mX, d->mY, d->mXOffset, d->mYOffset,
                d.->mDragView, mDragViewVisualCenter);

        // We want the point to be mapped to the dragTarget.
        mLauncher->IsHotseatLayout(dropTargetLayout, &res);
        if (res) {
            AutoPtr<IHotseat> hotseat;
            mLauncher->GetHotseat((IHotseat**)&hotseat);
            MapPointFromSelfToHotseatLayout(hotseat, mDragViewVisualCenter);
        }
        else {
            MapPointFromSelfToChild(dropTargetLayout, mDragViewVisualCenter, NULL);
        }

        Int32 spanX = 1;
        Int32 spanY = 1;
        if (mDragInfo != NULL) {
            AutoPtr<CellLayout::CellInfo> dragCellInfo = mDragInfo;
            spanX = dragCellInfo->mSpanX;
            spanY = dragCellInfo->mSpanY;
        }
        else {
            AutoPtr<IItemInfo> dragInfo = IItemInfo::Probe(d->mDragInfo);
            spanX = dragInfo->mSpanX;
            spanY = dragInfo->mSpanY;
        }

        Int32 minSpanX = spanX;
        Int32 minSpanY = spanY;
        if (IPendingAddWidgetInfo::Probe(d->mDragInfo) != NULL) {
            minSpanX = IPendingAddWidgetInfo::Probe(d->mDragInfo)->mMinSpanX;
            minSpanY = IPendingAddWidgetInfo::Probe(d->mDragInfo)->mMinSpanY;
        }

        mTargetCell = FindNearestArea((Int32)(*mDragViewVisualCenter)[0],
                (Int32)(*mDragViewVisualCenter)[1], minSpanX, minSpanY, dropTargetLayout,
                mTargetCell);
        Float distance;
        dropTargetLayout->GetDistanceFromCell((*mDragViewVisualCenter)[0],
                (*mDragViewVisualCenter)[1], mTargetCell, &distance);

        WillCreateUserFolder(IItemInfo::Probe(d->mDragInfo), dropTargetLayout,
                mTargetCell, distance, TRUE, &res)
        if (res) {
            *result = TRUE;
            return NOERROR;
        }
        WillAddToExistingUserFolder(IItemInfo::Probe(d->mDragInfo), dropTargetLayout,
                mTargetCell, distance, &res);
        if (res) {
            *result = TRUE;
            return NOERROR;
        }

        AutoPtr<ArrayOf<Int32> > resultSpan = ArrayOf<Int32>::Alloc(2);
        dropTargetLayout->CreateArea((Int32)(*mDragViewVisualCenter)[0],
                (Int32)(*mDragViewVisualCenter)[1], minSpanX, minSpanY, spanX, spanY,
                NULL, mTargetCell, resultSpan, ICellLayout::MODE_ACCEPT_DROP,
                (ArrayOf<Int32>**)&mTargetCell);
        Boolean foundCell = (*mTargetCell)[0] >= 0 && (*mTargetCell)[1] >= 0;

        // Don't accept the drop if there's no room for the item
        if (!foundCell) {
            // Don't show the message if we are dropping on the AllApps button and the hotseat
            // is full
            Boolean isHotseat;
            mLauncher->IsHotseatLayout(dropTargetLayout, &isHotseat);
            if (mTargetCell != NULL && isHotseat) {
                AutoPtr<IHotseat> hotseat;
                mLauncher->GetHotseat((IHotseat**)&hotseat);
                Int32 seat;
                hotseat->GetOrderInHotseat((*mTargetCell)[0], (*mTargetCell)[1], &seat);
                Boolean res;
                hotseat->IsAllAppsButtonRank(seat, &res);
                if (res) {
                    *result = FALSE;
                    return NOERROR;
                }
            }

            mLauncher->ShowOutOfSpaceMessage(isHotseat);
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode Workspace::WillCreateUserFolder(
    /* [in] */ IItemInfo* info,
    /* [in] */ ICellLayout* target,
    /* [in] */ ArrayOf<Int32>* targetCell,
    /* [in] */ Float distance,
    /* [in] */ Boolean considerTimeout,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (distance > mMaxDistanceForFolderCreation) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IView> dropOverView;
    target->GetChildAt((*targetCell)[0], (*targetCell)[1], (IView**)&dropOverView);

    if (dropOverView != NULL) {
        AutoPtr<ICellLayoutLayoutParams> params;
        dropOverView->GetLayoutParams((ICellLayoutLayoutParams**)&params);
        AutoPtr<CellLayout::LayoutParams> lp = (CellLayout::LayoutParams*)IObject::Probe(params);
        if (lp->mUseTmpCoords && (lp->mTmpCellX != lp->mCellX || lp->mTmpCellY != lp->mTmpCellY)) {
            *result = FALSE;
            return NOERROR;
        }
    }

    Boolean hasntMoved = FALSE;
    if (mDragInfo != NULL) {
        hasntMoved = dropOverView == mDragInfo->mCell;
    }

    if (dropOverView == NULL || hasntMoved || (considerTimeout && !mCreateUserFolderOnDrop)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IInterface> tag;
    dropOverView->GetTag((IInterface**)&tag);
    Boolean aboveShortcut = (IShortcutInfo::Probe(tag) != NULL);
    Boolean willBecomeShortcut =
            (info->mItemType == LauncherSettings::Favorites::ITEM_TYPE_APPLICATION ||
            info->mItemType == LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT);

    *result = (aboveShortcut && willBecomeShortcut);
    return NOERROR;
}

ECode Workspace::WillAddToExistingUserFolder(
    /* [in] */ IInterface* dragInfo,
    /* [in] */ ICellLayout* target,
    /* [in] */ ArrayOf<Int32>* targetCell,
    /* [in] */ Float distance,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (distance > mMaxDistanceForFolderCreation) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IView> dropOverView;
    target->GetChildAt((*targetCell)[0], (*targetCell)[1], (IView**)&dropOverView);

    if (dropOverView != NULL) {
        AutoPtr<ICellLayoutLayoutParams> params;
        dropOverView->GetLayoutParams((ICellLayoutLayoutParams**)&params);
        AutoPtr<CellLayout::LayoutParams> lp = (CellLayout::LayoutParams*)IObject::Probe(params);
        if (lp->mUseTmpCoords && (lp->mTmpCellX != lp->mCellX || lp->mTmpCellY != lp->mTmpCellY)) {
            *result = FALSE;
            return NOERROR;
        }
    }

    if (IFolderIcon::Probe(dropOverView) != NULL) {
        AutoPtr<IFolderIcon> fi = IFolderIcon::Probe(dropOverView);
        Boolean res;
        fi->AcceptDrop(dragInfo, &res);
        if (res) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode Workspace::CreateUserFolderIfNecessary(
    /* [in] */ IView* newView,
    /* [in] */ Int64 container,
    /* [in] */ ICellLayout* target,
    /* [in] */ ArrayOf<Int32>* targetCell,
    /* [in] */ Float distance,
    /* [in] */ Boolean external,
    /* [in] */ IDragView* dragView,
    /* [in] */ IRunnable* postAnimationRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (distance > mMaxDistanceForFolderCreation) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IView> v;
    target->GetChildAt((*targetCell)[0], (*targetCell)[1], (IView**)&v);

    Boolean hasntMoved = FALSE;
    if (mDragInfo != NULL) {
        AutoPtr<ICellLayout> cellParent;
        GetParentCellLayoutForView(mDragInfo->mCell, (ICellLayout**)&cellParent);
        hasntMoved = (mDragInfo->mCellX == (*targetCell)[0] &&
                mDragInfo->mCellY == (*targetCell)[1]) && (cellParent == target);
    }

    if (v == NULL || hasntMoved || !mCreateUserFolderOnDrop) {
        *result = FALSE;
        return NOERROR;
    }
    mCreateUserFolderOnDrop = FALSE;
    Int32 screen;
    if (targetCell == NULL) {
        screen = mDragInfo->mScreen;
    }
    else {
        IndexOfChild(target, &screen);
    }

    AutoPtr<IInterface> tag;
    v->GetTag((IInterface**)&tag);
    Boolean aboveShortcut = (IShortcutInfo::Probe(tag) != NULL);

    AutoPtr<IInterface> tag2;
    newView->GetTag((IInterface**)&tag2);
    Boolean willBecomeShortcut = (IShortcutInfo::Probe(tag2) != NULL);

    if (aboveShortcut && willBecomeShortcut) {
        AutoPtr<IShortcutInfo> sourceInfo = IShortcutInfo::Probe(tag2);
        AutoPtr<IShortcutInfo> destInfo = IShortcutInfo::Probe(tag);
        // if the drag started here, we need to remove it from the workspace
        if (!external) {
            AutoPtr<ICellLayout> cellLayout;
            GetParentCellLayoutForView(mDragInfo->mCell, (ICellLayout**)&cellLayout);
            cellLayout->RemoveView(mDragInfo->mCell);
        }

        AutoPtr<IRect> folderLocation;
        CRect::New((IRect**)&folderLocation);
        AutoPtr<IDragLayer> dragLayer;
        mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
        Float scale;
        dragLayer->GetDescendantRectRelativeToSelf(v, folderLocation, &scale);
        target->RemoveView(v);

        AutoPtr<IFolderIcon> fi;
        mLauncher->AddFolder(target, container, screen, (*targetCell)[0],
                (*targetCell)[1], (IFolderIcon**)&fi);
        destInfo->mCellX = -1;
        destInfo->mCellY = -1;
        sourceInfo->mCellX = -1;
        sourceInfo->mCellY = -1;

        // If the dragView is null, we can't animate
        Boolean animate = dragView != NULL;
        if (animate) {
            fi->PerformCreateAnimation(destInfo, v, sourceInfo, dragView, folderLocation, scale,
                    postAnimationRunnable);
        }
        else {
            fi->AddItem(destInfo);
            fi->AddItem(sourceInfo);
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Workspace::AddToExistingFolderIfNecessary(
    /* [in] */ IView* newView,
    /* [in] */ ICellLayout* target,
    /* [in] */ ArrayOf<Int32>* targetCell,
    /* [in] */ Float distance,
    /* [in] */ IDragObject* d,
    /* [in] */ Boolean external,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (distance > mMaxDistanceForFolderCreation) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IView> dropOverView;
    target->GetChildAt((*targetCell)[0], (*targetCell)[1], (IView**)&dropOverView);
    if (!mAddToExistingFolderOnDrop) {
        *result = FALSE;
        return NOERROR;
    }
    mAddToExistingFolderOnDrop = FALSE;

    if (IFolderIcon::Probe(dropOverView) != NULL) {
        AutoPtr<IFolderIcon> fi = IFolderIcon::Probe(dropOverView);
        Boolean res;
        fi->AcceptDrop(d->mDragInfo, &res);
        if (res) {
            fi->OnDrop(d);

            // if the drag started here, we need to remove it from the workspace
            if (!external) {
                AutoPtr<ICellLayout> cellParent;
                GetParentCellLayoutForView(mDragInfo->mCell, (ICellLayout**)&cellParent);
                cellParent->RemoveView(mDragInfo->mCell);
            }
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode Workspace::OnDrop(
    /* [in] */ IDragObject* d)
{
    mDragViewVisualCenter = GetDragViewVisualCenter(d->mX, d->mY, d->mXOffset, d->mYOffset,
            d->mDragView, mDragViewVisualCenter);

    CellLayout dropTargetLayout = mDropToLayout;

    // We want the point to be mapped to the dragTarget.
    if (dropTargetLayout != NULL) {
        Boolean res;
        mLauncher->IsHotseatLayout(dropTargetLayout, &res);
        if (res) {
            AutoPtr<IHotseat> hotseat;
            mLauncher->GetHotseat((IHotseat**)&hotseat);
            MapPointFromSelfToHotseatLayout(hotseat, mDragViewVisualCenter);
        }
        else {
            MapPointFromSelfToChild(dropTargetLayout, mDragViewVisualCenter, NULL);
        }
    }

    Int32 snapScreen = -1;
    Boolean resizeOnDrop = FALSE;
    if (TO_IINTERFACE(d->mDragSource) != TO_IINTERFACE(this)) {
        AutoPtr<ArrayOf<Int32> > touchXY = ArrayOf<Int32>::Alloc(2);
        (*touchXY)[0] = (Int32)(*mDragViewVisualCenter)[0];
        (*touchXY)[1] = (Int32)(*mDragViewVisualCenter)[1];
        OnDropExternal(touchXY, d->mDragInfo, dropTargetLayout, FALSE, d);
    }
    else if (mDragInfo != NULL) {
        AutoPtr<IView> cell = mDragInfo->mCell;

        AutoPtr<IRunnable> resizeRunnable;
        if (dropTargetLayout != NULL) {
            // Move internally
            AutoPtr<ICellLayout> cellLayout;
            GetParentCellLayoutForView(cell, (ICellLayout**)&cellLayout);
            Boolean hasMovedLayouts = (TO_IINTERFACE(cellLayout) != TO_IINTERFACE(dropTargetLayout));
            Boolean hasMovedIntoHotseat;
            mLauncher->IsHotseatLayout(dropTargetLayout, &hasMovedIntoHotseat);
            Int64 container = hasMovedIntoHotseat ?
                    LauncherSettings::Favorites::CONTAINER_HOTSEAT :
                    LauncherSettings::Favorites::CONTAINER_DESKTOP;
            Int32 screen;
            if ((*mTargetCell)[0] < 0) {
                screen = mDragInfo->mScreen;
            }
            else {
                IndexOfChild(dropTargetLayout, &screen);
            }
            Int32 spanX = mDragInfo != NULL ? mDragInfo->mSpanX : 1;
            Int32 spanY = mDragInfo != NULL ? mDragInfo->mSpanY : 1;
            // First we find the cell nearest to point at which the item is
            // dropped, without any consideration to whether there is an item there.

            mTargetCell = FindNearestArea((Int32)(*mDragViewVisualCenter)[0],
                    (Int32)(*mDragViewVisualCenter)[1], spanX, spanY,
                    dropTargetLayout, mTargetCell);
            Float distance;
            dropTargetLayout->GetDistanceFromCell((*mDragViewVisualCenter)[0],
                    (*mDragViewVisualCenter)[1], mTargetCell, &distance);

            // If the item being dropped is a shortcut and the nearest drop
            // cell also contains a shortcut, then create a folder with the two shortcuts.

            if (!mInScrollArea) {
                Boolean res;
                CreateUserFolderIfNecessary(cell, container,
                        dropTargetLayout, mTargetCell, distance, FALSE, d->mDragView, NULL, res);
                if (res) {
                    return NOERROR;
                }
            }
            Boolean res;
            AddToExistingFolderIfNecessary(cell, dropTargetLayout, mTargetCell,
                    distance, d, FALSE, &res)
            if (res) {
                return NOERROR;
            }

            // Aside from the special case where we're dropping a shortcut onto a shortcut,
            // we need to find the nearest cell location that is vacant
            AutoPtr<ItemInfo> item = (ItemInfo*)IObject::Probe(d->mDragInfo);
            Int32 minSpanX = item->mSpanX;
            Int32 minSpanY = item->mSpanY;
            if (item->mMinSpanX > 0 && item->mMinSpanY > 0) {
                minSpanX = item->mMinSpanX;
                minSpanY = item->mMinSpanY;
            }

            AutoPtr<ArrayOf<Int32> > resultSpan = ArrayOf<Int32>::Alloc(2);
            dropTargetLayout->CreateArea((Int32)(*mDragViewVisualCenter)[0],
                    (Int32)(*mDragViewVisualCenter)[1], minSpanX, minSpanY, spanX, spanY, cell,
                    mTargetCell, resultSpan, ICellLayout::MODE_ON_DROP,
                    (ArrayOf<Int32>**)&mTargetCell);

            Boolean foundCell = (*mTargetCell)[0] >= 0 && (*mTargetCell)[1] >= 0;

            // if the widget resizes on drop
            if (foundCell && (IAppWidgetHostView::Probe(cell) != NULL) &&
                    ((*resultSpan)[0] != item->mSpanX || (*resultSpan)[1] != item->mSpanY)) {
                resizeOnDrop = TRUE;
                item->mSpanX = (*esultSpan)[0];
                item->mSpanY = (*esultSpan)[1];
                AutoPtr<IAppWidgetHostView> awhv = IAppWidgetHostView::Probe(cell);
                AppWidgetResizeFrame->UpdateWidgetSizeRanges(awhv, mLauncher, (*resultSpan)[0],
                        (*resultSpan)[1]);
            }

            if (mCurrentPage != screen && !hasMovedIntoHotseat) {
                snapScreen = screen;
                SnapToPage(screen);
            }

            if (foundCell) {
                AutoPtr<IInterface> obj;
                cell->GetTag((IInterface**)&obj);
                AutoPtr<ItemInfo> info = (ItemInfo*)IObject::Probe(obj);
                if (hasMovedLayouts) {
                    // Reparent the view
                    AutoPtr<ICellLayout> cellLayout;
                    GetParentCellLayoutForView(cell, (ICellLayout**)&cellLayout);
                    cellLayout->RemoveView(cell);
                    AddInScreen(cell, container, screen, (*mTargetCell)[0], (*mTargetCell)[1],
                            info->mSpanX, info->mSpanY);
                }

                // update the item's position after drop
                AutoPtr<ICellLayoutLayoutParams> params;
                cell->GetLayoutParams((ICellLayoutLayoutParams**)&params);
                AutoPtr<CellLayout::LayoutParams> lp =
                        (CellLayout::LayoutParams*)IObject::Probe(params);
                lp->mCellX = lp->mTmpCellX = (*mTargetCell)[0];
                lp->mCellY = lp->mTmpCellY = (*mTargetCell)[1];
                lp->mCellHSpan = item->mSpanX;
                lp->mCellVSpan = item->mSpanY;
                lp->mIsLockedToGrid = TRUE;
                Int32 childId;
                LauncherModel::GetCellLayoutChildId(container, mDragInfo.screen,
                        (*mTargetCell)[0], (*mTargetCell)[1], mDragInfo->mSpanX,
                        mDragInfo->mSpanY, &childId);
                cell->SetId(childId);

                if (container != LauncherSettings::Favorites::CONTAINER_HOTSEAT &&
                        ILauncherAppWidgetHostView::Probe(cell) != NULL) {
                    AutoPtr<ICellLayout> cellLayout = dropTargetLayout;
                    // We post this call so that the widget has a chance to be placed
                    // in its final location

                    AutoPtr<ILauncherAppWidgetHostView> hostView =
                            ILauncherAppWidgetHostView::Probe(cell);
                    AutoPtr<IAppWidgetProviderInfo> pinfo;
                    hostView->GetAppWidgetInfo((IAppWidgetProviderInfo**)&pinfo);
                    if (pinfo != NULL &&
                            pinfo->mResizeMode != IAppWidgetProviderInfo::RESIZE_NONE) {
                        AutoPtr<IRunnable> addResizeFrame = new MyRunnable2(this, info,
                                hostView, cellLayout);
                        resizeRunnable = new MyRunnable3(this, addResizeFrame);
                    }
                }

                LauncherModel::MoveItemInDatabase(mLauncher, info, container, screen, lp->mCellX,
                        lp->mCellY);
            }
            else {
                // If we can't find a drop location, we return the item to its original position
                AutoPtr<ICellLayoutLayoutParams> params;
                cell->GetLayoutParams((ICellLayoutLayoutParams**)&params);
                AutoPtr<CellLayout::LayoutParams> lp =
                        (CellLayout::LayoutParams*)IObject::Probe(params);
                (*mTargetCell)[0] = lp->mCellX;
                (*mTargetCell)[1] = lp->mCellY;
                CellLayout layout = (CellLayout) cell.getParent().getParent();
                layout.markCellsAsOccupiedForView(cell);
            }
        }

        AutoPtr<IViewParent> _parent;
        cell->GetParent((IViewParent**)&_parent);
        AutoPtr<IViewParent> _parent2;
        _parent->GetParent((IViewParent**)&_parent2);
        AutoPtr<ICellLayout> parent = ICellLayout::Probe(_parent2);
        AutoPtr<IRunnable> finalResizeRunnable = resizeRunnable;
        // Prepare it to be animated into its new position
        // This must be called after the view has been re-parented
        AutoPtr<IRunnable> onCompleteRunnable = new MyRunnable3(this, finalResizeRunnable);
        mAnimatingViewIntoPlace = TRUE;
        Boolean res;
        d->mDragView->HasDrawn(&res);
        if (res) {
            AutoPtr<IInterface> tag;
            cell->GetTag((IInterface**)&tag);
            AutoPtr<ItemInfo> info = (ItemInfo*)IObject::Probe(tag);
            if (info->mItemType == LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET) {
                Int32 animationType = resizeOnDrop ? ANIMATE_INTO_POSITION_AND_RESIZE :
                        ANIMATE_INTO_POSITION_AND_DISAPPEAR;
                AnimateWidgetDrop(info, parent, d->mDragView,
                        onCompleteRunnable, animationType, cell, FALSE);
            }
            else {
                Int32 duration = snapScreen < 0 ? -1 : ADJACENT_SCREEN_DROP_DURATION;
                AutoPtr<IDragLayer> dragLayer;
                mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
                dragLayer->AnimateViewIntoPosition(d->mDragView, cell, duration,
                        onCompleteRunnable, this);
            }
        }
        else {
            d->mDeferDragViewCleanupPostAnimation = FALSE;
            cell->SetVisibility(VISIBLE);
        }
        parent->OnDropChild(cell);
    }
    return NOERROR;
}

ECode Workspace::SetFinalScrollForPageChange(
    /* [in] */ Int32 screen)
{
    if (screen >= 0) {
        GetScrollX(&mSavedScrollX);
        AutoPtr<IView> view;
        GetChildAt(screen, (IView**)&view);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
        cl->GetTranslationX(&mSavedTranslationX);
        cl->GetRotationY(&mSavedRotationY);
        Int32 offset1;
        GetChildOffset(screen, &offset1);
        Int32 offset2;
        GetRelativeChildOffset(screen, &offset2);
        Int32 newX = offset1 - offset2;
        SetScrollX(newX);
        cl->SetTranslationX(0f);
        return cl->SetRotationY(0f);
    }
    return NOERROR;
}

ECode Workspace::ResetFinalScrollForPageChange(
    /* [in] */ Int32 screen)
{
    if (screen >= 0) {
        AutoPtr<IView> view;
        GetChildAt(screen, (IView**)&view);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
        SetScrollX(mSavedScrollX);
        cl->SetTranslationX(mSavedTranslationX);
        cl->SetRotationY(mSavedRotationY);
    }
    return NOERROR;
}

ECode Workspace::GetViewLocationRelativeToSelf(
    /* [in] */ IView* v,
    /* [in] */ ArrayOf<Int32>* location)
{
    GetLocationInWindow(location);
    Int32 x = (*location)[0];
    Int32 y = (*location)[1];

    v->GetLocationInWindow(location);
    Int32 vX = (*location)[0];
    Int32 vY = (*location)[1];

    (*location)[0] = vX - x;
    (*location)[1] = vY - y;
    return NOERROR;
}

ECode Workspace::OnDragEnter(
    /* [in] */ IDragObject* d)
{
    mDragEnforcer->OnDragEnter();
    mCreateUserFolderOnDrop = FALSE;
    mAddToExistingFolderOnDrop = FALSE;

    mDropToLayout = NULL;
    AutoPtr<ICellLayout> layout;
    GetCurrentDropLayout((ICellLayout**)&layout);
    SetCurrentDropLayout(layout);
    SetCurrentDragOverlappingLayout(layout);

    // Because we don't have space in the Phone UI (the CellLayouts run to the edge) we
    // don't need to show the outlines
    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    if (res) {
        ShowOutlines();
    }
    return NOERROR;
}

ECode Workspace::GetCellLayoutMetrics(
    /* [in] */ ILauncher* launcher,
    /* [in] */ Int32 orientation,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    AutoPtr<IResources> res;
    launcher->GetResources((IResources**)&res);
    AutoPtr<IWindowManager> windowManager;
    launcher->GetWindowManager((IWindowManager**)&windowManager);
    AutoPtr<IDisplay> display;
    windowManager->GetDefaultDisplay((IDisplay**)&display);
    AutoPtr<IPoint> smallestSize;
    CPoint::New((IPoint**)&smallestSize);
    AutoPtr<IPoint> largestSize;
    CPoint::New((IPoint**)&largestSize);
    display->GetCurrentSizeRange(smallestSize, largestSize);
    if (orientation == ICellLayout::LANDSCAPE) {
        if (mLandscapeCellLayoutMetrics == NULL) {
            Int32 paddingLeft;
            res->GetDimensionPixelSize(
                    Elastos::Droid::Launcher2::R::dimen::workspace_left_padding_land,
                    &paddingLeft);
            Int32 paddingRight;
            res->GetDimensionPixelSize(
                    Elastos::Droid::Launcher2::R::dimen::workspace_right_padding_land,
                    &paddingRight);
            Int32 paddingTop;
            res->GetDimensionPixelSize(
                    Elastos::Droid::Launcher2::R::dimen::workspace_top_padding_land,
                    &paddingTop);
            Int32 paddingBottom;
            res->GetDimensionPixelSize(
                    Elastos::Droid::Launcher2::R::dimen::workspace_bottom_padding_land,
                    &paddingBottom);
            Int32 x;
            largestSize->GetX(&x);
            Int32 y;
            smallestSize->GetY(&y);
            Int32 width = x - paddingLeft - paddingRight;
            Int32 height = y - paddingTop - paddingBottom;
            CRect::New((Rect**)&mLandscapeCellLayoutMetrics);
            Int32 cx;
            LauncherModel::GetCellCountX(&cx);
            Int32 cy;
            LauncherModel::GetCellCountY(&cy);
            CellLayout::GetMetrics(mLandscapeCellLayoutMetrics, res,
                    width, height, cx, cy, orientation);
        }
        *rect = mLandscapeCellLayoutMetrics;
        REFCOUNT_ADD(*rect);
        return NOERROR;
    }
    else if (orientation == ICellLayout::PORTRAIT) {
        if (mPortraitCellLayoutMetrics == NULL) {
            Int32 paddingLeft;
            res->GetDimensionPixelSize(
                    Elastos::Droid::Launcher2::R::dimen::workspace_left_padding_land,
                    &paddingLeft);
            Int32 paddingRight;
            res->GetDimensionPixelSize(
                    Elastos::Droid::Launcher2::R::dimen::workspace_right_padding_land,
                    &)paddingRight;
            Int32 paddingTop;
            res->GetDimensionPixelSize(
                    Elastos::Droid::Launcher2::R::dimen::workspace_top_padding_land,
                    &paddingTop);
            Int32 paddingBottom;
            res->GetDimensionPixelSize(
                    Elastos::Droid::Launcher2::R::dimen::workspace_bottom_padding_land,
                    &paddingBottom);
            Int32 x;
            smallestSize->GetX(&x);
            Int32 y;
            largestSize->GetY(&y);
            Int32 width = x - paddingLeft - paddingRight;
            Int32 height = y - paddingTop - paddingBottom;
            CRect::New((IRect**)&mPortraitCellLayoutMetrics);
            Int32 cx;
            LauncherModel::GetCellCountX(&cx);
            Int32 cy;
            LauncherModel::GetCellCountY(&cy);
            CellLayout::GetMetrics(mPortraitCellLayoutMetrics, res,
                    width, height, cx, cy,
                    orientation);
        }
        *rect = mPortraitCellLayoutMetrics;
        REFCOUNT_ADD(*rect);
        return NOERROR;
    }
    *rect = NULL;
    return NOERROR;
}

ECode Workspace::OnDragExit(
    /* [in] */ IDragObject* d)
{
    mDragEnforcer->OnDragExit();

    // Here we store the final page that will be dropped to, if the workspace in fact
    // receives the drop
    if (mInScrollArea) {
        Boolean res;
        IsPageMoving(&res);
        if (res) {
            // If the user drops while the page is scrolling, we should use that page as the
            // destination instead of the page that is being hovered over.
            Int32 page;
            GetNextPage(&page);
            AutoPtr<IView> view;
            GetPageAt(page, (IView**)&view);
            mDropToLayout = ICellLayout::Probe(view);
        }
        else {
            mDropToLayout = mDragOverlappingLayout;
        }
    }
    else {
        mDropToLayout = mDragTargetLayout;
    }

    if (mDragMode == DRAG_MODE_CREATE_FOLDER) {
        mCreateUserFolderOnDrop = TRUE;
    }
    else if (mDragMode == DRAG_MODE_ADD_TO_FOLDER) {
        mAddToExistingFolderOnDrop = TRUE;
    }

    // Reset the scroll area and previous drag target
    OnResetScrollArea();
    SetCurrentDropLayout(NULL);
    SetCurrentDragOverlappingLayout(NULL);

    mSpringLoadedDragController->Cancel();

    if (!mIsPageMoving) {
        HideOutlines();
    }
    return NOERROR;
}

ECode Workspace::SetCurrentDropLayout(
    /* [in] */ ICellLayout* layout)
{
    if (mDragTargetLayout != NULL) {
        mDragTargetLayout->RevertTempState();
        mDragTargetLayout->OnDragExit();
    }
    mDragTargetLayout = layout;
    if (mDragTargetLayout != NULL) {
        mDragTargetLayout->OnDragEnter();
    }
    CleanupReorder(TRUE);
    CleanupFolderCreation();
    return SetCurrentDropOverCell(-1, -1);
}

ECode Workspace::SetCurrentDragOverlappingLayout(
    /* [in] */ ICellLayout* layout)
{
    if (mDragOverlappingLayout != NULL) {
        mDragOverlappingLayout->SetIsDragOverlapping(FALSE);
    }
    mDragOverlappingLayout = layout;
    if (mDragOverlappingLayout != NULL) {
        mDragOverlappingLayout->SetIsDragOverlapping(TRUE);
    }
    return Invalidate();
}

ECode Workspace::SetCurrentDropOverCell(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (x != mDragOverX || y != mDragOverY) {
        mDragOverX = x;
        mDragOverY = y;
        return SetDragMode(DRAG_MODE_NONE);
    }
    return NOERROR;
}

ECode Workspace::SetDragMode(
    /* [in] */ Int32 dragMode)
{
    if (dragMode != mDragMode) {
        if (dragMode == DRAG_MODE_NONE) {
            CleanupAddToFolder();
            // We don't want to cancel the re-order alarm every time the target cell changes
            // as this feels to slow / unresponsive.
            CleanupReorder(FALSE);
            CleanupFolderCreation();
        }
        else if (dragMode == DRAG_MODE_ADD_TO_FOLDER) {
            CleanupReorder(true);
            CleanupFolderCreation();
        }
        else if (dragMode == DRAG_MODE_CREATE_FOLDER) {
            CleanupAddToFolder();
            CleanupReorder(true);
        }
        else if (dragMode == DRAG_MODE_REORDER) {
            CleanupAddToFolder();
            CleanupFolderCreation();
        }
        mDragMode = dragMode;
    }
    return NOERROR;
}

void Workspace::CleanupFolderCreation()
{
    if (mDragFolderRingAnimator != NULL) {
        mDragFolderRingAnimator->AnimateToNaturalState();
    }
    mFolderCreationAlarm->CancelAlarm();
}

void Workspace::CleanupAddToFolder()
{
    if (mDragOverFolderIcon != NULL) {
        mDragOverFolderIcon->OnDragExit(NULL);
        mDragOverFolderIcon = NULL;
    }
}

void Workspace::CleanupReorder(
    /* [in] */ Boolean cancelAlarm)
{
    // Any pending reorders are canceled
    if (cancelAlarm) {
        mReorderAlarm->CancelAlarm();
    }
    mLastReorderX = -1;
    mLastReorderY = -1;
}

ECode Workspace::GetDropTargetDelegate(
    /* [in] */ IDragObject* d,
    /* [out] */ IDropTarget** target)
{
    VALIDATE_NOT_NULL(target);

    *target = NULL;
    return NOERROR;
}

ECode Workspace::MapPointFromSelfToChild(
    /* [in] */ IView* v,
    /* [in] */ ArrayOf<Float>* xy)
{
    return MapPointFromSelfToChild(v, xy, NULL);
}

ECode Workspace::MapPointFromSelfToChild(
    /* [in] */ IView* v,
    /* [in] */ ArrayOf<Float>* xy,
    /* [in] */ IMatrix* cachedInverseMatrix)
{
    if (cachedInverseMatrix == NULL) {
        AutoPtr<IMatrix> matrix;
        v->GetMatrix((IMatrix**)&matrix);
        matrix->Invert(mTempInverseMatrix);
        cachedInverseMatrix = mTempInverseMatrix;
    }
    Int32 scrollX;
    GetScrollX(&scrollX);
    if (mNextPage != INVALID_PAGE) {
        mScroller->GetFinalX(&scrollX);
    }
    Int32 left;
    v->GetLeft(&left);
    (*xy)[0] = (*xy)[0] + scrollX - left;
    Int32 y;
    GetScrollY(&y);
    Int32 top;
    v->GetTop(&top);
    (*xy)[1] = (*xy)[1] + y - top;
    return cachedInverseMatrix->MapPoints(xy);
}


ECode Workspace::MapPointFromSelfToHotseatLayout(
    /* [in] */ IHotseat* hotseat,
    /* [in] */ ArrayOf<Float>* xy)
{
    AutoPtr<ILayout> layout;
    hotseat->GetLayout((ILayout**)&layout);
    AutoPtr<IMatrix> matrix;
    layout->GetMatrix((IMatrix**)&matrix);
    matrix->Invert(mTempInverseMatrix);
    Int32 left;
    hotseat->GetLeft(&left);
    Int32 layoutLeft;
    layout->GetLeft(&layoutLeft);
    (*xy)[0] = (*xy)[0] - left - layoutLeft;
    Int32 top;
    hotseat->GetTop(&top);
    Int32 layoutTop;
    layout->GetTop(&layoutTop);
    (*xy)[1] = (*xy)[1] - top - layoutTop;
    return mTempInverseMatrix->MapPoints(xy);
}

ECode Workspace::MapPointFromChildToSelf(
    /* [in] */ IView* v,
    /* [in] */ ArrayOf<Float>* xy)
{
    AutoPtr<IMatrix> matrix;
    v->GetMatrix((IMatrix**)&matrix);
    matrix->MapPoints(xy);
    Int32 scrollX;
    GetScrollX(&scrollX);
    if (mNextPage != INVALID_PAGE) {
        mScroller->GetFinalX(&scrollX);
    }
    Int32 left;
    v->GetLeft(&left);
    (*xy)[0] -= (scrollX - left);
    Int32 y;
    GetScrollY(&y);
    Int32 top;
    v->GetTop(&top);
    (*xy)[1] -= (y - top);
    return NOERROR;
}

Float Workspace::SquaredDistance(
    /* [in] */ ArrayOf<Float>* point1,
    /* [in] */ ArrayOf<Float>* point2)
{
    Float distanceX = (*point1)[0] - (*point2)[0];
    Float distanceY = (*point2)[1] - (*point2)[1];
    return distanceX * distanceX + distanceY * distanceY;
}

ECode Workspace::Overlaps(
    /* [in] */ ICellLayout* cl,
    /* [in] */ IDragView* dragView,
    /* [in] */ Int32 dragViewX,
    /* [in] */ Int32 dragViewY,
    /* [in] */ IMatrix* cachedInverseMatrix,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Transform the coordinates of the item being dragged to the CellLayout's coordinates
    AutoPtr<ArrayOf<Float> > draggedItemTopLeft = mTempDragCoordinates;
    (*draggedItemTopLeft)[0] = dragViewX;
    (*draggedItemTopLeft)[1] = dragViewY;
    AutoPtr<ArrayOf<Float> > draggedItemBottomRight = mTempDragBottomRightCoordinates;
    Int32 width;
    dragView->GetDragRegionWidth(&width);
    (*draggedItemBottomRight)[0] = (*draggedItemTopLeft)[0] + width;
    Int32 height;
    dragView->GetDragRegionHeight(&height);
    (*draggedItemBottomRight)[1] = (*draggedItemTopLeft)[1] + height;

    // Transform the dragged item's top left coordinates
    // to the CellLayout's local coordinates
    MapPointFromSelfToChild(cl, draggedItemTopLeft, cachedInverseMatrix);
    Float overlapRegionLeft = Math::Max(0f, (*draggedItemTopLeft)[0]);
    Float overlapRegionTop = Math::Max(0f, (*draggedItemTopLeft)[1]);

    Int32 width2;
    cl->GetWidth(&width2);
    if (overlapRegionLeft <= width2 && overlapRegionTop >= 0) {
        // Transform the dragged item's bottom right coordinates
        // to the CellLayout's local coordinates
        MapPointFromSelfToChild(cl, draggedItemBottomRight, cachedInverseMatrix);
        Int32 height2;
        cl->GetHeight(&height2);
        Float overlapRegionRight = Math::Min(width2, (*draggedItemBottomRight)[0]);
        Float overlapRegionBottom = Math::Min(height2, (*draggedItemBottomRight)[1]);

        if (overlapRegionRight >= 0 && overlapRegionBottom <= height2) {
            Float overlap = (overlapRegionRight - overlapRegionLeft) *
                     (overlapRegionBottom - overlapRegionTop);
            if (overlap > 0) {
                *result = TRUE;
                return NOERROR
            }
         }
    }
    *result = FALSE;
    return NOERROR
}

AutoPtr<ICellLayout> Workspace::FindMatchingPageForDragOver(
    /* [in] */ IDragView* dragView,
    /* [in] */ Float originX,
    /* [in] */ Float originY,
    /* [in] */ Boolean exact)
{
    // We loop through all the screens (ie CellLayouts) and see which ones overlap
    // with the item being dragged and then choose the one that's closest to the touch point
    Int32 screenCount;
    GetChildCount(&screenCount);
    AutoPtr<ICellLayout> bestMatchingScreen;
    Float smallestDistSoFar = Float.MAX_VALUE;

    for (Int32 i = 0; i < screenCount; i++) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);

        AutoPtr<ArrayOf<Float> > touchXy = ArrayOf<Float>::Alloc(2);
        (*touchXy)[0] = originX;
        (*touchXy)[1] = originY;
        // Transform the touch coordinates to the CellLayout's local coordinates
        // If the touch point is within the bounds of the cell layout, we can return immediately
        AutoPtr<IMatrix> matrix;
        cl->GetMatrix((IMatrix**)&matrix);
        matrix->Invert(mTempInverseMatrix);
        MapPointFromSelfToChild(cl, touchXy, mTempInverseMatrix);

        Int32 width;
        cl->GetWidth(&width);
        Int32 height;
        cl->GetHeight(&height);
        if ((*touchXy)[0] >= 0 && (*touchXy)[0] <= width &&
                (*touchXy)[1] >= 0 && (*touchXy)[1] <= height) {
            return cl;
        }

        if (!exact) {
            // Get the center of the cell layout in screen coordinates
            AutoPtr<ArrayOf<Float> > cellLayoutCenter = mTempCellLayoutCenterCoordinates;
            Int32 width;
            cl->GetWidth(&width);
            Int32 height;
            cl->GetHeight(&height);
            (*cellLayoutCenter)[0] = width / 2;
            (*cellLayoutCenter)[1] = height / 2;
            MapPointFromChildToSelf(cl, cellLayoutCenter);

            (*touchXy)[0] = originX;
            (*touchXy)[1] = originY;

            // Calculate the distance between the center of the CellLayout
            // and the touch point
            Float dist = SquaredDistance(touchXy, cellLayoutCenter);

            if (dist < smallestDistSoFar) {
                smallestDistSoFar = dist;
                bestMatchingScreen = cl;
            }
        }
    }
    return bestMatchingScreen;
}

AutoPtr<ArrayOf<Float> > Workspace::GetDragViewVisualCenter(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 xOffset,
    /* [in] */ Int32 yOffset,
    /* [in] */ IDragView* dragView,
    /* [in] */ ArrayOf<Float>* recycle)
{
    AutoPtr<ArrayOf<Float> > res;
    if (recycle == NULL) {
        res = ArrayOf<Float>::(2);
    }
    else {
        res = recycle;
    }

    // First off, the drag view has been shifted in a way that is not represented in the
    // x and y values or the x/yOffsets. Here we account for that shift.
    AutoPtr<IResources> resources;
    GetResources(&resources);
    Int32 sizeX;
    resources->GetDimensionPixelSize(
        Elastos::Droid::Launcher2::R::dimen::dragViewOffsetX, &sizeX);
    x += sizeX;
    Int32 sizeY;
    resources->GetDimensionPixelSize(
        Elastos::Droid::Launcher2::R::dimen::dragViewOffsetY, &sizeY);
    y += sizeY;

    // These represent the visual top and left of drag view if a dragRect was provided.
    // If a dragRect was not provided, then they correspond to the actual view left and
    // top, as the dragRect is in that case taken to be the entire dragView.
    // R.dimen.dragViewOffsetY.
    Int32 left = x - xOffset;
    Int32 top = y - yOffset;

    // In order to find the visual center, we shift by half the dragRect
    AutoPtr<IRect> rect = dragView->GetDragRegion();
    Int32 width;
    rect->GetWidth(&width);
    (*res)[0] = left + width / 2;
    Int32 height;
    rect->GeHeight(&height);
    (*res)[1] = top + height / 2;

    return res;
}

Boolean Workspace::IsDragWidget(
    /* [in] */ DragObject* d)
{
    return (ILauncherAppWidgetInfo::Probe(d->mDragInfo) != NULL ||
            IPendingAddWidgetInfo::Probe(d->mDragInfo) != NULL);
}

Boolean Workspace::IsExternalDragWidget(
    /* [in] */ DragObject* d)
{
    return TO_IINTERFACE(d->mDragSource) != TO_IINTERFACE(this) && IsDragWidget(d);
}

ECode Workspace::OnDragOver(
    /* [in] */ IDragObject* d)
{
    // Skip drag over events while we are dragging over side pages
    if (mInScrollArea || mIsSwitchingState || mState == State::SMALL) return NOERROR;

    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    AutoPtr<ICellLayout> layout;
    AutoPtr<ItemInfo> item = (ItemInfo*)IObject::Probe(d->mDragInfo);

    // Ensure that we have proper spans for the item that we are dropping
    if (item->mSpanX < 0 || item->mSpanY < 0) {
        //throw new RuntimeException("Improper spans found");
        Slogger::E("Workspace", "Improper spans found");
        return E_RUNTIME_EXCEPTION;
    }
    mDragViewVisualCenter = GetDragViewVisualCenter(d->mX, d->mY, d->mXOffset, d->mYOffset,
        d->mDragView, mDragViewVisualCenter);

    AutoPtr<IView> child = (mDragInfo == NULL) ? NULL : mDragInfo->mCell;
    // Identify whether we have dragged over a side page
    Boolean res;
    IsSmall(&res);
    if (res) {
        AutoPtr<IHotseat> hotseat;
        mLauncher->GetHotseat((IHotseat**)&hotseat);
        if (hotseat != NULL && !IsExternalDragWidget(d)) {
            hotseat->GetHitRect(r);
            Boolean res;
            r->Contains(d->mXx, d->mY, &res);
            if (res) {
                hotseat->GetLayout((ICellLayout**)&layout);
            }
        }
        if (layout == NULL) {
            layout = FindMatchingPageForDragOver(d->mDragView, d->mX, d->mY, FALSE);
        }
        if (layout != mDragTargetLayout) {

            SetCurrentDropLayout(layout);
            SetCurrentDragOverlappingLayout(layout);

            Boolean isInSpringLoadedMode = (mState == State::SPRING_LOADED);
            if (isInSpringLoadedMode) {
                Boolean res;
                mLauncher->IsHotseatLayout(layout, &res);
                if (res) {
                    mSpringLoadedDragController->Cancel();
                }
                else {
                    mSpringLoadedDragController->SetAlarm(mDragTargetLayout);
                }
            }
        }
    }
    else {
        // Test to see if we are over the hotseat otherwise just use the current page
        AutoPtr<IHotseat> hotseat;
        mLauncher->GetHotseat((IHotseat**)&hotseat);
        if (hotseat) != NULL && !IsDragWidget(d)) {
            hotseat->GetHitRect(r);
            Boolean res;
            r->Contains(d->mX, d->mY, &res);
            if (res)) {
                hotseat->GetLayout((ICellLayout**)&layout);
            }
        }
        if (layout == NULL) {
            GetCurrentDropLayout((ICellLayout**)&layout);
        }
        if (layout != mDragTargetLayout) {
            SetCurrentDropLayout(layout);
            SetCurrentDragOverlappingLayout(layout);
        }
    }

    // Handle the drag over
    if (mDragTargetLayout != NULL) {
        // We want the point to be mapped to the dragTarget.
        Boolean res;
        mLauncher->IsHotseatLayout(mDragTargetLayout, &res);
        if (res) {
            AutoPtr<IHotseat> hotseat;
            mLauncher->GetHotseat((IHotseat**)&hotseat);
            MapPointFromSelfToHotseatLayout(hotseat, mDragViewVisualCenter);
        }
        else {
            MapPointFromSelfToChild(mDragTargetLayout, mDragViewVisualCenter, NULL);
        }

        AutoPtr<ItemInfo> info = (ItemInfo*)IObject::Probe(d->mDragInfo);

        mTargetCell = FindNearestArea((Int32)(*mDragViewVisualCenter)[0],
                (Int32)(*mDragViewVisualCenter)[1], item->mSpanX, item->mSpanY,
                mDragTargetLayout, mTargetCell);

        SetCurrentDropOverCell((*mTargetCell)[0], (*mTargetCell)[1]);

        Float targetCellDistance;
        mDragTargetLayout->GetDistanceFromCell(
                (*mDragViewVisualCenter)[0], (*mDragViewVisualCenter)[1],
                mTargetCell, &targetCellDistance);

        AutoPtr<IView> dragOverView;
        mDragTargetLayout->GetChildAt((*mTargetCell)[0],
                (*mTargetCell)[1], (IView**)&dragOverView);

        ManageFolderFeedback(info, mDragTargetLayout, mTargetCell,
                targetCellDistance, dragOverView);

        int32 minSpanX = item->mSpanX;
        int32 minSpanY = item->mSpanY;
        if (item::mMinSpanX > 0 && item::mMinSpanY > 0) {
            minSpanX = item->mMinSpanX;
            minSpanY = item->mMinSpanY;
        }

        Boolean nearestDropOccupied;
        mDragTargetLayout->IsNearestDropLocationOccupied(
            (Int32)(*mDragViewVisualCenter)[0], (Int32)(*mDragViewVisualCenter)[1], item->mSpanX,
                item->mSpanY, child, mTargetCell, &nearestDropOccupied);

        Boolean tmp;
        if (!nearestDropOccupied) {
            AutoPtr<IPoint> p = d->mDragView->GetDragVisualizeOffset();
            AutoPtr<IRect> r = d->mDragView->GetDragRegion();
            mDragTargetLayout->VisualizeDropLocation(child, mDragOutline,
                    (Int32)(*mDragViewVisualCenter)[0], (Int32)(*mDragViewVisualCenter)[1],
                    (*mTargetCell)[0], (*mTargetCell)[1], item->mSpanX, item->mSpanY, FALSE,
                    p, r);
        }
        else if ((mDragMode == DRAG_MODE_NONE || mDragMode == DRAG_MODE_REORDER)
                && (mReorderAlarm->AlarmPending(&tmp), !tmp) &&
                (mLastReorderX != (*mTargetCell)[0] || mLastReorderY != (*mTargetCell)[1])) {

            // Otherwise, if we aren't adding to or creating a folder and there's no pending
            // reorder, then we schedule a reorder
            AutoPtr<IOnAlarmListener> listener = new ReorderAlarmListener(mDragViewVisualCenter,
                    minSpanX, minSpanY, item->mSpanX, item->mSpanY, d->mDragView, child);
            mReorderAlarm->SetOnAlarmListener(listener);
            mReorderAlarm->SetAlarm(REORDER_TIMEOUT);
        }

        if (mDragMode == DRAG_MODE_CREATE_FOLDER || mDragMode == DRAG_MODE_ADD_TO_FOLDER ||
                !nearestDropOccupied) {
            if (mDragTargetLayout != NULL) {
                mDragTargetLayout->RevertTempState();
            }
        }
    }
    return NOERROR;
}

void Workspace::ManageFolderFeedback(
    /* [in] */ ItemInfo* info,
    /* [in] */ ICellLayout* targetLayout,
    /* [in] */ ArrayOf<Int32> targetCell,
    /* [in] */ Float distance,
    /* [in] */ IView* dragOverView)
{
    Boolean userFolderPending;
    WillCreateUserFolder(info, targetLayout, targetCell, distance, FALSE, &userFolderPending);

    if (mDragMode == DRAG_MODE_NONE && userFolderPending) {
        Boolean res;
        mFolderCreationAlarm->AlarmPending(&res);
        if (!res) {
            AutoPtr<IOnAlarmListener> lis = new FolderCreationAlarmListener(
                    targetLayout, (*targetCell)[0], (*targetCell)[1]);
            mFolderCreationAlarm->SetOnAlarmListener(lis);
            mFolderCreationAlarm->SetAlarm(FOLDER_CREATION_TIMEOUT);
            return;
       }
    }

    Boolean willAddToFolderp;
    WillAddToExistingUserFolder(info, targetLayout, targetCell,
            distance, &willAddToFolderp);

    if (willAddToFolder && mDragMode == DRAG_MODE_NONE) {
        mDragOverFolderIcon = IFolderIcon::Probe(dragOverView);
        mDragOverFolderIcon->OnDragEnter(info);
        if (targetLayout != NULL) {
            targetLayout->ClearDragOutlines();
        }
        SetDragMode(DRAG_MODE_ADD_TO_FOLDER);
        return;
    }

    if (mDragMode == DRAG_MODE_ADD_TO_FOLDER && !willAddToFolder) {
        SetDragMode(DRAG_MODE_NONE);
    }
    if (mDragMode == DRAG_MODE_CREATE_FOLDER && !userFolderPending) {
        SetDragMode(DRAG_MODE_NONE);
    }

    return;
}

Ecode Workspace::GetHitRect(
    /* [in] */ IRect* outRect)
{
    // We want the workspace to have the whole area of the display (it will find the correct
    // cell layout to drop to in the existing drag/drop logic.
    Int32 x;
    mDisplaySize->GetX(&x);
    Int32 y;
    mDisplaySize->GetY(&y);
    return outRect->Set(0, 0, x, y);
}

Ecode Workspace::AddExternalItemToScreen(
    /* [in] */ IItemInfo* dragInfo,
    /* [in] */ ICellLayout* layout,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ItemInfo> _info = (ItemInfo*)dragInfo;
    Boolean res;
    layout->FindCellForSpan(mTempEstimate, _info->mSpanX, _info->mSpanY, &res);
    if (res) {
        OnDropExternal(_info->mDropPos, dragInfo, layout, FALSE);
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    mLauncher->IsHotseatLayout(layout, &res);
    mLauncher->ShowOutOfSpaceMessage(res);
    *result = FALSE;
    return NOERROR;
}

void Workspace::OnDropExternal(
    /* [in] */ ArrayOf<Int32>* touchXY,
    /* [in] */ IInterface* dragInfo,
    /* [in] */ ICellLayout* cellLayout,
    /* [in] */ Boolean insertAtFirst)
{
    OnDropExternal(touchXY, dragInfo, cellLayout, insertAtFirst, NULL);
}

ECode Workspace::OnDropExternal(
    /* [in] */ ArrayOf<Int32>* touchXY,
    /* [in] */ IInterface* dragInfo,
    /* [in] */ ICellLayout* cellLayout,
    /* [in] */ Boolean insertAtFirst,
    /* [in] */ IDragObject* d)
{
    AutoPtr<IRunnable> exitSpringLoadedRunnable = new MyRunnable5(this);

    AutoPtr<ItemInfo> info = (ItemInfo*)IObject::Probe(dragInfo);
    Int32 spanX = info->mSpanX;
    Int32 spanY = info->mSpanY;
    if (mDragInfo != NULL) {
        spanX = mDragInfo->mSpanX;
        spanY = mDragInfo->mSpanY;
    }

    Int64 container;
    Boolean res;
    mLauncher->IsHotseatLayout(cellLayout, &res);
    if (res) {
        container = LauncherSettings::Favorites::CONTAINER_HOTSEAT;
    }
    else {
        container = LauncherSettings::Favorites::CONTAINER_DESKTOP;
    }
    Int32 screen;
    IndexOfChild(cellLayout, &screen);
    mLauncher->IsHotseatLayout(cellLayout, &res);
    if (!res && screen != mCurrentPage && mState != State::SPRING_LOADED) {
        SnapToPage(screen);
    }

    if (IPendingAddItemInfo::Probe(info) != NULL) {
        AutoPtr<PendingAddItemInfo> pendingInfo =
                (PendingAddItemInfo*)IPendingAddItemInfo::Probe(dragInfo);

        Boolean findNearestVacantCell = TRUE;
        if (pendingInfo->mItemType == LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT) {
            mTargetCell = FindNearestArea((Int32)(*touchXY)[0], (Int32)(*touchXY)[1], spanX, spanY,
                    cellLayout, mTargetCell);
            Float distance;
            cellLayout->GetDistanceFromCell((*mDragViewVisualCenter)[0],
                    (*mDragViewVisualCenter)[1], mTargetCell, &distance);
            Boolean res;
            WillCreateUserFolder(IItemInfo::Probe(d->mDragInfo), cellLayout, mTargetCell,
                    distance, TRUE, &res);
            Boolean res2;
            WillAddToExistingUserFolder(IItemInfo::Probe(d->mDragInfo),
                    cellLayout, mTargetCell, distance, &res2);
            if (res || res2) {
                findNearestVacantCell = FALSE;
            }
        }

        AutoPtr<ItemInfo> item = (ItemInfo*)IObject::Probe(d->mDragInfo);
        Boolean updateWidgetSize = FALSE;
        if (findNearestVacantCell) {
            Int32 minSpanX = item->mSpanX;
            Int32 minSpanY = item->mSpanY;
            if (item->mMinSpanX > 0 && item->mMinSpanY > 0) {
                minSpanX = item->mMinSpanX;
                minSpanY = item->mMinSpanY;
            }
            AutoPtr<ArrayOf<Int32> > resultSpan = ArrayOf<Int32>::Alloc(2);
            assert(0, "in out: mTargetCell");
            cellLayout->CreateArea((Int32)(*mDragViewVisualCenter)[0],
                    (Int32)(*mDragViewVisualCenter)[1], minSpanX, minSpanY,
                    info->mSpanX, info->mSpanY, NULL, mTargetCell, resultSpan,
                    ICellLayout::MODE_ON_DROP_EXTERNAL, (ArrayOf<Int32>**)&mTargetCell);

            if ((*resultSpan)[0] != item->mSpanX || (*resultSpan)[1] != item->mSpanY) {
                updateWidgetSize = TRUE;
            }
            item->mSpanX = (*resultSpan)[0];
            item->mSpanY = (*resultSpan)[1];
        }

        AutoPtr<IRunnable> onAnimationCompleteRunnable = new MyRunnabl6(
                this, pendingInfo, item, container, screen, span);
        AutoPtr<IView> finalView ;
        if (pendingInfo->mItemType == LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET) {
            finalView = IPendingAddWidgetInfo::Probe(pendingInfo)->mBoundWidget;
        }
        else {
            finalView = NULL;
        }

        if (IAppWidgetHostView::Probe(finalView) != NULL && updateWidgetSize) {
            AutoPtr<IAppWidgetHostView> awhv = IAppWidgetHostView::Probe(finalView);
            AppWidgetResizeFrame::UpdateWidgetSizeRanges(awhv, mLauncher, item->mSpanX,
                    item->mSpanY);
        }

        Int32 animationStyle = ANIMATE_INTO_POSITION_AND_DISAPPEAR;
        if (pendingInfo->mItemType == LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET &&
                IPendingAddWidgetInfo::Probe(pendingInfo)->mInfo->mConfigure != NULL) {
            animationStyle = ANIMATE_INTO_POSITION_AND_REMAIN;
        }
        AnimateWidgetDrop(info, cellLayout, d->mDragView, onAnimationCompleteRunnable,
                animationStyle, finalView, TRUE);
    }
    else {
        // This is for other drag/drop cases, like dragging from All Apps
        AutoPtr<IView> view;

        switch (info->mItemType) {
            case LauncherSettings::Favorites::ITEM_TYPE_APPLICATION:
            case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
            {
                if (info->mContainer == NO_ID && IApplicationInfo::Probe(info) != NULL) {
                    // Came from all apps -- make a copy
                    info = new ShortcutInfo(IApplicationInfo::Probe(info));
                }
                mLauncher->CreateShortcut(
                        Elastos::Droid::Launcher2::R::layout.::application, cellLayout,
                        IShortcutInfo::Probe(info), (IView**)&view);
                break;
            }
            case LauncherSettings::Favorites::ITEM_TYPE_FOLDER:
                FolderIcon::FromXml(
                        Elastos::Droid::Launcher2::R::layout::folder_icon, mLauncher,
                        cellLayout, IFolderInfo::Probe(info), mIconCache, (IView**)&view);
                break;
            default:
                //throw new IllegalStateException("Unknown item type: " + info.itemType);
                Logger::E(TAG, "Unknown item type: " + info.itemType);
                return E_ILLEGAL_STATE_EXCEPTION;
        }

        // First we find the cell nearest to point at which the item is
        // dropped, without any consideration to whether there is an item there.
        if (touchXY != NULL) {
            mTargetCell = FindNearestArea((Int32)(*touchXY)[0], (Int32)(*touchXY)[1], spanX, spanY,
                    cellLayout, mTargetCell);
            Float distance;
            cellLayout->GetDistanceFromCell((*mDragViewVisualCenter)[0],
                    (*mDragViewVisualCenter)[1], mTargetCell, &distance);
            d->mPostAnimationRunnable = exitSpringLoadedRunnable;
            Boolean res;
            CreateUserFolderIfNecessary(view, container, cellLayout, mTargetCell, distance,
                    TRUE, d->mDragView, d->mPostAnimationRunnable, &res)
            if (res) {
                return NOERROR;
            }
            AddToExistingFolderIfNecessary(view, cellLayout, mTargetCell, distance, d,
                    TRUE, &res);
            if (res) {
                return NOERROR;
            }
        }

        if (touchXY != NULL) {
            // when dragging and dropping, just find the closest free spot
            mTargetCell = cellLayout->CreateArea((Int32)(*mDragViewVisualCenter)[0],
                    (Int32)(*mDragViewVisualCenter)[1], 1, 1, 1, 1,
                    NULL, mTargetCell, NULL, ICellLayout::MODE_ON_DROP_EXTERNAL);
        }
        else {
            cellLayout->FindCellForSpan(mTargetCell, 1, 1);
        }
        AddInScreen(view, container, screen, (*mTargetCell)[0], (*mTargetCell)[1], info->mSpanX,
                info->mSpanY, insertAtFirst);
        cellLayout->OnDropChild(view);
        AutoPtr<ICellLayoutLayoutParams> params;
        view->GetLayoutParams((ICellLayoutLayoutParams**)&params);
        AutoPtr<CellLayout::LayoutParams> lp = (CellLayout::LayoutParams*)IObject::Probe(params);
        AutoPtr<IShortcutAndWidgetContainer> container;
        cellLayout->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
        container->MeasureChild(view);


        LauncherModel::AddOrMoveItemInDatabase(mLauncher, info, container, screen,
                lp->mCellX, lp->mCellY);

        if (d->mDragView != NULL) {
            // We wrap the animation call in the temporary set and reset of the current
            // cellLayout to its final transform -- this means we animate the drag view to
            // the correct final location.
            SetFinalTransitionTransform(cellLayout);
            AutoPtr<IDragLayer> dragLayer;
            mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
            dragLayer->AnimateViewIntoPosition(d->mDragView, view,
                    exitSpringLoadedRunnable);
            ResetTransitionTransform(cellLayout);
        }
    }
    return NOERROR;
}

ECode Workspace::CreateWidgetBitmap(
    /* [in] */ IItemInfo* widgetInfo,
    /* [in] */ IView* layout,
    /* [out] */ IBitmap** map)
{
    VALIDATE_NOT_NULL(map);

    AutoPtr<ArrayOf<Int32> > unScaledSize;
    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    workspace->EstimateItemSize(widgetInfo->mSpanX,
            widgetInfo->mSpanY, widgetInfo, FALSE, (ArrayOf<Int32>**)&unScaledSize);
    Int32 visibility;
    layout->GetVisibility(&visibility);
    layout->SetVisibility(VISIBLE);

    Int32 width;
    MeasureSpec::MakeMeasureSpec((*unScaledSize)[0], IMeasureSpec::EXACTLY, &width);
    Int32 height;
    MeasureSpec::MakeMeasureSpec((*unScaledSize)[1], IMeasureSpec::EXACTLY, &height);
    AutoPtr<IBitmap> b;
    Bitmap::CreateBitmap((*unScaledSize)[0], (*unScaledSize)[1],
            Bitmap::Config::ARGB_8888, IBitmap**()&b);
    AutoPtr<ICanvas> c;
    CCanvas::New(b, (ICanvas**)*c);

    layout->Measure(width, height);
    layout->Layout(0, 0, (*unScaledSize)[0], (*unScaledSize)[1]);
    layout->Draw(c);
    c->SetBitmap(null);
    layout->SetVisibility(visibility);
    *map = b;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

void Workspace::GetFinalPositionForDropAnimation(
    /* [in] */ ArrayOf<Int32>* loc,
    /* [in] */ ArrayOf<Float>* scaleXY,
    /* [in] */ IDragView* dragView,
    /* [in] */ ICellLayout* layout,
    /* [in] */ ItemInfo* info,
    /* [in] */ ArrayOf<Int32>* targetCell,
    /* [in] */ Boolean external,
    /* [in] */ Boolean scale)
{
    // Now we animate the dragView, (ie. the widget or shortcut preview) into its final
    // location and size on the home screen.
    Int32 spanX = info->mSpanX;
    Int32 spanY = info->mSpanY;

    AutoPtr<IRect> r;
    EstimateItemPosition(layout, info, (*targetCell)[0], (*targetCell)[1],
            spanX, spanY, (IRect**)&r);
    r->GetLeft(&((*loc)[0]));
    r->GetTop(&((*loc)[1]));

    SetFinalTransitionTransform(layout);
    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    Float cellLayoutScale;
    dragLayer->GetDescendantCoordRelativeToSelf(layout, loc, &cellLayoutScale);
    ResetTransitionTransform(layout);

    Float dragViewScaleX;
    Float dragViewScaleY;
    if (scale) {
        Int32 width;
        r->GetWidth(&width);
        Int32 mwidth;
        dragView->GetMeasuredWidth(&mwidth);
        dragViewScaleX = (1.0f * width) / mwidth;
        Int32 height;
        r->GetHeight(&height);
        Int32 mheight;
        dragView->GetMeasuredHeight(&mheight);
        dragViewScaleY = (1.0f * height) / mheight;
    }
    else {
        dragViewScaleX = 1f;
        dragViewScaleY = 1f;
    }

    // The animation will scale the dragView about its center, so we need to center about
    // the final location.
    Int32 mwidth;
    dragView->GetMeasuredWidth(&mwidth);
    Int32 width;
    r->GetWidth(&width);
    (*loc)[0] -= (mwidth - cellLayoutScale * width) / 2;
    Int32 mheight;
    dragView->GetMeasuredHeight(&mheight);
    Int32 height;
    r->GetHeight(&height);
    (*loc)[1] -= (mheight - cellLayoutScale * height) / 2;

    (*scaleXY)[0] = dragViewScaleX * cellLayoutScale;
    (*scaleXY)[1] = dragViewScaleY * cellLayoutScale;
}

ECode Workspace::AnimateWidgetDrop(
    /* [in] */ IItemInfo* info,
    /* [in] */ ICellLayout* cellLayout,
    /* [in] */ IDragView* dragView,
    /* [in] */ IRunnable* onCompleteRunnable,
    /* [in] */ Int32 animationType,
    /* [in] */ IView* finalView,
    /* [in] */ Boolean external)
{
    AutoPtr<IRect> from;
    CRect::New((IRect**)&from);
    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    dragLayer->GetViewRectRelativeToSelf(dragView, from);

    AutoPtr<ArrayOf<Int32> > finalPos = ArrayOf<Int32>::Alloc(2);
    AutoPtr<ArrayOf<Float> > scaleXY[] = ArrayOf<Float>::Alloc(2);
    Boolean scalePreview = (IPendingAddShortcutInfo::Probe(info) == NULL);
    GetFinalPositionForDropAnimation(finalPos, scaleXY, dragView, cellLayout, info, mTargetCell,
            external, scalePreview);

    AutoPtr<IResources> res;
    mLauncher->GetResources((IResources**)&res);
    Int32 duration;
    Int32 tmp;
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_dropAnimMaxDuration,
            &tmp);
    Int32 duration = tmp - 200;

    // In the case where we've prebound the widget, we remove it from the DragLayer
    if (IAppWidgetHostView::Probe(finalView) != NULL && external) {
        Slogger::D(TAG, "6557954 Animate widget drop, final view is appWidgetHostView");
        AutoPtr<IDragLayer> dragLayer;
        mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
        dragLayer->RemoveView(finalView);
    }
    if ((animationType == ANIMATE_INTO_POSITION_AND_RESIZE || external) && finalView != NULL) {
        AutoPtr<IBitmap> crossFadeBitmap;
        CreateWidgetBitmap(info, finalView, (IBitmap**)&crossFadeBitmap);
        dragView->SetCrossFadeBitmap(crossFadeBitmap);
        dragView->CrossFade((Int32)(duration * 0.8f));
    }
    else if (info->mItemType == LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET && external) {
        (*scaleXY0[0] = (*scaleXY0[1] = Math::min((*scaleXY)[0],  (*scaleXY)[1]);
    }

    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    if (animationType == CANCEL_TWO_STAGE_WIDGET_DROP_ANIMATION) {
        AutoPtr<IDragLayer> _dragLayer;
        mLauncher->GetDragLayer((IDragLayer**)&_dragLayer);
        _dragLayer->AnimateViewIntoPosition(dragView, finalPos, 0f, 0.1f, 0.1f,
                IDragLayer::ANIMATION_END_DISAPPEAR, onCompleteRunnable, duration);
    }
    else {
        Int32 endStyle;
        if (animationType == ANIMATE_INTO_POSITION_AND_REMAIN) {
            endStyle = IDragLayer::ANIMATION_END_REMAIN_VISIBLE;
        }
        else {
            endStyle = IDragLayer::ANIMATION_END_DISAPPEAR;;
        }

        AutoPtr<IRunnable> onComplete = new MyRunnabl7(finalView, onCompleteRunnable);
        Int32 left;
        from->GetLeft(&left);
        Int32 top;
        from->GetTop(&top);
        dragLayer->AnimateViewIntoPosition(dragView, left, top, (*finalPos)[0],
                (*finalPos)[1], 1, 1, 1, (*scaleXY)[0], (*scaleXY)[1], onComplete, endStyle,
                duration, this);
    }
    return NOERROR;
}

ECode Workspace::SetFinalTransitionTransform(
    /* [in] */ ICellLayout* layout)
{
    Boolean res;
    IsSwitchingState(&res);
    if (res) {
        Int32 index;
        IndexOfChild(layout, &index);
        layout->GetScaleX(&mCurrentScaleX);
        layout->GetScaleY(&mCurrentScaleY);
        layout->GetTranslationX(&mCurrentTranslationX);
        layout->GetTranslationY(&mCurrentTranslationY);
        layout->GetRotationY(&mCurrentRotationY);
        layout->SetScaleX((*mNewScaleXs)[index]);
        layout->SetScaleY((*mNewScaleYs)[index]);
        layout->SetTranslationX((*mNewTranslationXs)[index]);
        layout->SetTranslationY((*mNewTranslationYs)[index]);
        layout->SetRotationY((*mNewRotationYs)[index]);
    }
    return NOERROR;
}

ECode Workspace::ResetTransitionTransform(
    /* [in] */ ICellLayout* layout)
{
    Boolean res;
    IsSwitchingState(&res);
    if (res) {
        layout->GetScaleX(&mCurrentScaleX);
        layout->GetScaleY(&mCurrentScaleY);
        layout->GetTranslationX(&mCurrentTranslationX);
        layout->GetTranslationY(&mCurrentTranslationY);
        layout->GetRotationY(&mCurrentRotationY);
        layout->SetScaleX(mCurrentScaleX);
        layout->SetScaleY(mCurrentScaleY);
        layout->SetTranslationX(mCurrentTranslationX);
        layout->SetTranslationY(mCurrentTranslationY);
        layout->SetRotationY(mCurrentRotationY);
    }
    return NOERROR;
}

ECode Workspace::GetCurrentDropLayout(
    /* [out] */ ICellLayout** layout)
{
    VALIDATE_NOT_NULL(layout);

    Int32 page;
    GetNextPage(&page);
    AutoPtr<IView> view;
    GetChildAt(page, (IView**)&view);
    *layout = ICellLayout::Probe(view);
    REFCOUNT_ADD(*layout);
    return NOERROR;
}

ECode Workspace::GetDragInfo(
    /* [out] */ ICellLayoutCellInfo** info)
{
    VALIDATE_NOT_NULL(info);

    *info = mDragInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode Workspace::FindNearestArea(
    /* [in] */ Int32 pixelX,
    /* [in] */ Int32 pixelY,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ ICellLayout* layout,
    /* [in] */ ArrayOf<Int32>* recycle,
    /* [out] */ ArrayOf<Int32>** array)
{
    return layout->FindNearestArea(
            pixelX, pixelY, spanX, spanY, recycle, array);
}

ECode Workspace::Setup(
    /* [in] */ IDragController* dragController)
{
    mSpringLoadedDragController = new SpringLoadedDragController(mLauncher);
    mDragController = dragController;

    // hardware layers on children are enabled on startup, but should be disabled until
    // needed
    UpdateChildrenLayersEnabled(FALSE);
    return SetWallpaperDimension();
}

ECode Workspace::OnDropCompleted(
    /* [in] */ IView* target,
    /* [in] */ IDragObject* d,
    /* [in] */ Boolean isFlingToDelete,
    /* [in] */ Boolean success)
{
    if (success) {
        if (TO_IINTERFACE(target) != TO_IINTERFACE(this)) {
            if (mDragInfo != NULL) {
                AutoPtr<ICellLayout> cellParent;
                GetParentCellLayoutForView(mDragInfo->mCell, (ICellLayout**)&cellParent);
                cellParent->RemoveView(mDragInfo->mCell);
                if (IDropTarget::Probe(mDragInfo->mCell) != NULL) {
                    mDragController->RemoveDropTarget(IDropTarget::Probe(mDragInfo->mCell));
                }
            }
        }
    }
    else if (mDragInfo != NULL) {
        AutoPtr<ICellLayout> cellLayout;
        Boolean res;
        mLauncher->IsHotseatLayout(target, &res);
        if (res) {
            AutoPtr<IHotseat> hotseat;
            mLauncher->GetHotseat((IHotseat**)&hotseat);
            hotseat->GetLayout((ICellLayout**)&cellLayout);
        }
        else {
            AutoPtr<IView> view;
            GetChildAt(mDragInfo->mScreen, (IView**)&view);
            cellLayout = ICellLayout::Probe(view);
        }
        cellLayout->OnDropChild(mDragInfo->mCell);
    }
    if (d->mCancelled &&  mDragInfo->mCell != NULL) {
            mDragInfo->mCell->SetVisibility(VISIBLE);
    }
    mDragOutline = NULL;
    mDragInfo = NULL;

    // Hide the scrolling indicator after you pick up an item
    return HideScrollingIndicator(FALSE);
}

ECode Workspace::UpdateItemLocationsInDatabase(
    /* [in] */ ICellLayout* cl)
{
    AutoPtr<IShortcutAndWidgetContainer> container;
    cl->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
    Int32 count;
    container->GetChildCount(&count);

    Int32 screen;
    IndexOfChild(cl, &screen);
    Int32 container = IFavorites::CONTAINER_DESKTOP;

    Boolean res;
    mLauncher->IsHotseatLayout(cl, &res);
    if (res) {
        screen = -1;
        container = IFavorites::CONTAINER_HOTSEAT;
    }

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IShortcutAndWidgetContainer> container;
        cl->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
        AutoPtr<IView> v;
        container->GetChildAt(i, (IView**)&v);
        AutoPtr<IInterface> tag;
        v->GetTag((IInterface**)&tag);
        AutoPtr<ItemInfo> info = (ItemInfo*)IObject::Probe(tag);
        // Null check required as the AllApps button doesn't have an item info
        if (info != NULL && info->mRequiresDbUpdate) {
            info->mRequiresDbUpdate = FALSE;
            LauncherModel::ModifyItemInDatabase(mLauncher, info, container, screen, info->mCellX,
                    info->mCellY, info->mSpanX, info->mSpanY);
        }
    }
    return NOERROR;
}

ECode Workspace::SupportsFlingToDelete(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode Workspace::OnFlingToDelete(
    /* [in] */ IDragObject* d,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPointF* vec)
{
    // Do nothing
    return NOERROR;
}

ECode Workspace::OnFlingToDeleteCompleted()
{
    // Do nothing
    return NOERROR;
}

ECode Workspace::IsDropEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode Workspace::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    SmoothPagedView::OnRestoreInstanceState(state);
    return Launcher::SetScreen(mCurrentPage);
}

ECode Workspace::DispatchRestoreInstanceState(
    /* [in] */ ISparseArray* container)
{
    // We don't dispatch restoreInstanceState to our children using this code path.
    // Some pages will be restored immediately as their items are bound immediately, and
    // others we will need to wait until after their items are bound.
    mSavedStates = container;
    return NOERROR;
}

ECode Workspace::RestoreInstanceStateForChild(
    /* [in] */ Int32 child)
{
    if (mSavedStates != NULL) {
        mRestoredPages->Add(child);
        AutoPtr<IView> view;
        GetChildAt(child, (IView**)&view);
        AutoPtr<ICellLayout> cl = ICellLayout::Probe(view);
        cl->RestoreInstanceState(mSavedStates);
    }
    return NOERROR;
}

ECode Workspace::RestoreInstanceStateForRemainingPages()
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        Boolean res;
        mRestoredPages->Contains(i, &res);
        if (!res) {
            RestoreInstanceStateForChild(i);
        }
    }
    return mRestoredPages->Clear();
}

ECode Workspace::ScrollLeft()
{
    Boolean res;
    IsSmall(&res);
    if (!res && !mIsSwitchingState) {
        SmoothPagedView::ScrollLeft();
    }
    AutoPtr<IFolder> openFolder;
    GetOpenFolder((IFolder**)&openFolder);
    if (openFolder != NULL) {
        openFolder->CompleteDragExit();
    }
    return NOERROR;
}

ECode Workspace::ScrollRight()
{
    Boolean res;
    IsSmall(&res);
    if (!res && !mIsSwitchingState) {
        SmoothPagedView::ScrollRight();
    }
    AutoPtr<IFolder> openFolder;
    GetOpenFolder((IFolder**)&openFolder);
    if (openFolder != NULL) {
        openFolder->CompleteDragExit();
    }
    return NOERROR;
}

ECode Workspace::OnEnterScrollArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* outresult)
{
    VALIDATE_NOT_NULL(outresult);

    // Ignore the scroll area if we are dragging over the hot seat
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Boolean res;
    LauncherApplication::IsScreenLandscape(context, &res);
    Boolean isPortrait = !res;

    AutoPtr<IHotseat> hotseat;
    mLauncher->GetHotseat((IHotseat**)&hotseat);
    if (hotseat != NULL && isPortrait) {
        AutoPtr<IRect> r;
        CRect::New((IRect**)&r);
        hotseat->GetHitRect(r);
        Boolean tmp;
        r->Contains(x, y, &tmp)
        if (tmp) {
            *outresult = FALSE;
            return NOERROR;
        }
    }

    Boolean result = FALSE;
    IsSmall(&res);
    if (!res && !mIsSwitchingState) {
        mInScrollArea = TRUE;

        Int32 page;
        GetNextPage(&page);
        Int32 page = page +
                   (direction == IDragController::SCROLL_LEFT ? -1 : 1);

        // We always want to exit the current layout to ensure parity of enter / exit
        SetCurrentDropLayout(NULL);

        Int32 count;
        GetChildCount(&count);
        if (0 <= page && page < count) {
            AutoPtr<IView> view;
            GetChildAt(page, (IView**)&view);
            AutoPtr<ICellLayout> layout = ICellLayout::Probe(view);
            SetCurrentDragOverlappingLayout(layout);

            // Workspace is responsible for drawing the edge glow on adjacent pages,
            // so we need to redraw the workspace when this may have changed.
            Invalidate();
            result = TRUE;
        }
    }
    *outresult = result;
    return NOERROR;
}

ECode Workspace::OnExitScrollArea(
    /* [out] */ Boolean* outresult)
{
    VALIDATE_NOT_NULL(outresult);

    Boolean result = FALSE;
    if (mInScrollArea) {
        Invalidate();
        AutoPtr<ICellLayout> layout;
        GetCurrentDropLayout((ICellLayout**)&layout);
        SetCurrentDropLayout(layout);
        SetCurrentDragOverlappingLayout(layout);

        result = TRUE;
        mInScrollArea = FALSE;
    }
    *outresult = result;
    return NOERROR;
}

void Workspace::OnResetScrollArea()
{
    SetCurrentDragOverlappingLayout(null);
    mInScrollArea = FALSE;
}

ECode Workspace::GetParentCellLayoutForView(
    /* [in] */ IView* v,
    /* [out] */ ICellLayout** cellLayout)
{
    VALIDATE_NOT_NULL(cellLayout);

    AutoPtr<IArrayList> layouts;
    GetWorkspaceAndHotseatCellLayouts((IArrayList**)&layouts);
    Int32 size;
    layouts->GetSize(&size);
    for (Int32 i = 0; i > size; i++) {
        AutoPtr<IInterface> obj;
        layouts->Get(i, (IInterface**)&obj);

        AutoPtr<ICellLayout> layout = ICellLayout::Probe(obj);
        AutoPtr<IShortcutAndWidgetContainer> container;
        layout->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
        Int32 index;
        container->IndexOfChild(v, &index);
        if (index > -1) {
            *cellLayout = layout;
            REFCOUNT_ADD(*cellLayout);
            return NOERROR;
        }
    }

    *cellLayout = NULL;
    return NOERROR;
}

ECode Workspace::GetWorkspaceAndHotseatCellLayouts(
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IArrayList> layouts;
    CArrayList::New((IArrayList**)&layouts);
    Int32 screenCount;
    GetChildCount(&screenCount);
    for (Int32 screen = 0; screen < screenCount; screen++) {
        AutoPtr<IView> view;
        GetChildAt(screen, (IView**)&view);
        layouts->Add(TO_IINTERFACE(ICellLayout::Probe(view)));
    }

    AutoPtr<IHotseat> hotseat;
    mLauncher->GetHotseat((IHotseat**)&hotseat);
    if (hotseat != NULL) {
        AutoPtr<ICellLayout> cellLayout;
        hotseat->GetLayout((ICellLayout**)&cellLayout);
        layouts->Add(TO_IINTERFACE(cellLayout));
    }
    *list = layouts;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode Workspace::GetAllShortcutAndWidgetContainers(
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IArrayList> childrenLayouts;
    CArrayList::New((IArrayList**)&childrenLayouts);
    Int32 screenCount;
    GetChildCount(&screenCount);
    for (Int32 screen = 0; screen < screenCount; screen++) {
        AutoPtr<IView> view;
        GetChildAt(screen, (IView**)&view);
        AutoPtr<IShortcutAndWidgetContainer> container;
        ICellLayout::Probe(view)->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
        childrenLayouts->Add(TO_IINTERFACE(container));
    }
    AutoPtr<IHotseat> hotseat;
    mLauncher->GetHotseat((IHotseat**)&hotseat);
    if (hotseat != null) {
        AutoPtr<ICellLayout> cellLayout;
        hotseat->GetLayout((ICellLayout**)&cellLayout);
        AutoPtr<IShortcutAndWidgetContainer> container;
        cellLayout->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container)
        childrenLayouts->Add(TO_IINTERFACE(container));
    }
    *list = childrenLayouts;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode Workspace::GetFolderForTag(
    /* [in] */ IInterface* tag,
    /* [out] */ IFolder** folder)
{
    VALIDATE_NOT_NULL(folder);

    AutoPtr<IArrayList> childrenLayouts;
    GetAllShortcutAndWidgetContainers((IArrayList**)&childrenLayouts);

    Int32 size;
    childrenLayouts->GetSize(&size);
    for (Inty32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        childrenLayouts->Get(i, (IInterface**)&obj);
        AutoPtr<IShortcutAndWidgetContainer> layout =
                IShortcutAndWidgetContainer::Probe(obj);

        Int32 count;
        layout->GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            layout->GetChildAt(i, (IView**)&child);
            if (IFolder::Probe(child) != NULL) {
                AutoPtr<IFolder> f = IFolder::Probe(child);
                AutoPtr<IFolderInfo> _info;
                f->GetInfo((IFolderInfo**)&_info);
                if (TO_IINTERFACE(_info) == TO_IINTERFACE(tag) && _info->mOpened) {
                    *folder = f;
                    REFCOUNT_ADD(*folder);
                    return NOERROR;
                }
            }
        }

    }

    *folder = NULL;
    return NOERROR;
}

ECode Workspace::GetViewForTag(
    /* [in] */ IInterface* tag,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IArrayList> childrenLayouts;
    GetAllShortcutAndWidgetContainers((IArrayList**)&childrenLayouts);

    Int32 size;
    childrenLayouts->GetSize(&size);
    for (Int32 i = 0; i < size;; i++) {
        AutoPtr<IInterface> obj;
        childrenLayouts->Get(i, (IInterface**)&obj);
        AutoPtr<IShortcutAndWidgetContainer> layout =
                IShortcutAndWidgetContainer::Probe(obj);

        Int32 count;
        layout->GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            layout->GetChildAt(i, (IView**)&child);

            AutoPtr<IInterface> _tag;
            child->GetTag((IInterface**)&_tag);
            if (TO_IINTERFACE(_tag) == TO_IINTERFACE(tag)) {
                *view = child;
                REFCOUNT_ADD(*view);
                return NOERROR;
            }
        }
    }

    *view = NULL;
    return NOERROR;
}

ECode Workspace::ClearDropTargets()
{
    AutoPtr<IArrayList> childrenLayouts;
    GetAllShortcutAndWidgetContainers((IArrayList**)&childrenLayouts);

    Int32 size;
    childrenLayouts->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        childrenLayouts->Get(i, (IInterface**)&obj);
        AutoPtr<IShortcutAndWidgetContainer> layout =
                IShortcutAndWidgetContainer::Probe(obj);

        Int32 childCount;
        layout->GetChildCount(&childCount);
        for (Int32 j = 0; j < childCount; j++) {
            AutoPtr<IView> v;
            layout->GetChildAt(j, (IView**)&v);
            if (IDropTarget::Probe(v) != NULL) {
                mDragController->RemoveDropTarget(IDropTarget::Probe(v));
            }
        }
    }
    return NOERROR;
}

ECode Workspace::RemoveItemsByPackageName(
        /* [in] */ IArrayList* packages,
        /* [in] */ IUserHandle* user)
{
    AutoPtr<IHashSet> packageNames;
    CHashSet::New((IHashSet**)&packageNames);
    packageNames->AddAll(packages);

    // Just create a hash table of all the specific components that this will affect
    AutoPtr<IHashSet> cns;
    CHashSet::New((IHashSet**)&cns);
    AutoPtr<IArrayList> cellLayouts;
    GetWorkspaceAndHotseatCellLayouts((IArrayList**)&cellLayouts);

    Int32 size;
    cellLayouts->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        cellLayouts->Get(i, (IInterface**)&obj);
        AutoPtr<ICellLayout> layoutParent = ICellLayout::Probe(obj);

        AutoPtr<IViewGroup> layout;
        layoutParent->GetShortcutsAndWidgets((IViewGroup**)&layout);
        Int32 childCount;
        layout->GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; ++i) {
            AutoPtr<IView> view;
            layout->GetChildAt(i, (IView**)&view);
            AutoPtr<IInterface> tag;
            view->GetTag((IInterface**)&tag);

            if (IShortcutInfo::Probe(tag) != NULL) {
                AutoPtr<ShortcutInfo> info = (ShortcutInfo*)IShortcutInfo::Probe(tag);
                AutoPtr<IComponentName> cn;
                info->mIntent->GetComponent((IComponentName**)&cn);
                String pname;
                cn->GetPackageName(&pname);
                Boolean res1, res2;
                if ((cn != NULL) && (packageNames->Contains(pname, &res1), res1)
                        && (info->mUser->Equals(user, &res2), res2)) {
                    cns->Add(TO_IINTERFACE(cn));
                }
            }
            else if (IFolderInfo::Probe(tag) != NULL) {
                AutoPtr<FolderInfo> info = (FolderInfo*)IFolderInfo::Probe(tag);
                Int32 size;
                info->mContents->GetSize(&size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    info->mContents->Get((IInterface**)&obj);
                    AutoPtr<ShortcutInfo> s = (ShortcutInfo*)IShortcutInfo::Probe(obj);
                    AutoPtr<IComponentName> cn;
                    s->mIntent->GetComponent((IComponentName**)&cn);
                    String pname;
                    cn->GetPackageName(&pname);
                    Boolean res1, res2;
                    if ((cn != NULL) && (packageNames->Contains(cpname, &res1), res1)
                            && (info->mUser->Equals(user, &res2), res2)) {
                        cns->Add(TO_IINTERFACE(cn));
                    }
                }
            }
            else if (ILauncherAppWidgetInfo::Probe(tag) != NULL) {
                AutoPtr<LauncherAppWidgetInfo> info =
                        (LauncherAppWidgetInfo*)ILauncherAppWidgetInfo::Probe(tag)
                AutoPtr<IComponentName> cn = info->mProviderName;
                String pname;
                cn->GetPackageName(&pname);
                Boolean res1, res2;
                if ((cn != NULL) && (packageNames->Contains(pname, &res1), res1)
                        && (info->mUser->Equals(user, &res2), res2)) {
                    cns->Add(TO_IINTERFACE(cn));
                }
            }
        }
    }

    // Remove all the things
    return RemoveItemsByComponentName(cns, user);
}

ECode Workspace::RemoveItemsByApplicationInfo(
    /* [in] */ IArrayList* appInfos,
    /* [in] */ IUserHandle* user)
{
    // Just create a hash table of all the specific components that this will affect
    AutoPtr<IHashSet> cns;
    CHashSet::New((IHashSet**)&cns);

    Int32 size;
    appInfos->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        appInfos->Get(i, (IInterface**)&obj);
        AutoPtr<ApplicationInfo> info = (ApplicationInfo*)IApplicationInfo::Probe(obj);
        cns->Add(TO_IINTERFACE(info->mComponentName));
    }

    // Remove all the things
    return RemoveItemsByComponentName(cns, user);
}

ECode Workspace::RemoveItemsByComponentName(
    /* [in] */ IHashSet* componentNames,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IArrayList> cellLayouts;
    GetWorkspaceAndHotseatCellLayouts((IArrayList**)&cellLayouts);

    Int32 size;
    cellLayouts->GetSize(&size);
    for (Int32 i = 0; i <size; i++) {
        AutoPtr<IInterface> obj;
        cellLayouts->Get(i, (IInterface**)&obj);
        AutoPtr<ICellLayout> layoutParent = ICellLayout::Probe(obj);

        AutoPtr<IViewGroup> layout;
        layoutParent->GetShortcutsAndWidgets((IViewGroup**)&layout);

        // Avoid ANRs by treating each screen separately
        AutoPtr<IRunnable> r = new MyRunnabl8(this, layout, componentNames, user, layoutParent);
        Boolean res;
        Post(r, &res);
    }

    // Clean up new-apps animation list
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IRunnable> r = new MyRunnabl9(context, componentNames);
    Boolean res;
    return Post(r, &res);
}

ECode Workspace::UpdateShortcuts(
    /* [in] */ IArrayList* apps)
{
    AutoPtr<IArrayList> childrenLayouts;
    GetAllShortcutAndWidgetContainers((IArrayList**)&childrenLayouts);

    Int32 size;
    childrenLayouts->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        childrenLayouts->Get(i, (IInterface**)&obj);
        AutoPtr<IShortcutAndWidgetContainer> layout = IShortcutAndWidgetContainer::Probe(obj);

        Int32 childCount;
        layout->GetChildCount(&childCount);
        for (Int32 j = 0; j < childCount; j++) {
            AutoPtr<IView> view;
            layout->GetChildAt(j, (IView**)&view);
            AutoPtr<IInterface> tag;
            view->GetTag((IInterface**)&tag);

            if (IShortcutInfo::Probe(tag) != NULL) {
                AutoPtr<ShortcutInfo> info = (ShortcutInfo*)IShortcutInfo::Probe(tag);
                // We need to check for ACTION_MAIN otherwise getComponent() might
                // return null for some shortcuts (for instance, for shortcuts to
                // web pages.)
                AutoPtr<IIntent> intent = info->mIntent;
                AutoPtr<IComponentName> name;
                intent->GetComponent((IComponentName**)&name);
                if (info->mItemType == LauncherSettings::Favorites::ITEM_TYPE_APPLICATION) {
                    String action;
                    intent->GetAction(&action);
                    Boolean res;
                    IIntent::ACTION_MAIN.Equals(action, &res);
                    if (res && name != NULL) {
                        Int32 appCount;
                        apps->GetSize(&appCount);
                        for (Int32 k = 0; k < appCount; k++) {
                            AutoPtr<IInterface> obj;
                            apps->Get(k, (IInterface**)&obj);
                            AutoPtr<ApplicationInfo> app = (ApplicationInfo*)IApplicationInfo::Probe(obj);
                            Boolean tmp;
                            app->mComponentName.Equals(name, &tmp);
                            if (tmp) {
                                AutoPtr<IBubbleTextView> shortcut = IBubbleTextView::Probe(view);
                                info->UpdateIcon(mIconCache);
                                app->mTitle.ToString(&(info->mTitle));
                                shortcut->ApplyFromShortcutInfo(info, mIconCache);
                            }
                        }
                    }
                }
            }
        }
    }
    return NOERROR;
}

ECode Workspace::MoveToDefaultScreen(
    /* [in] */ Boolean animate)
{
    Boolean res;
    IsSmall(&res);
    if (!res) {
        if (animate) {
            SnapToPage(mDefaultPage);
        }
        else {
            SetCurrentPage(mDefaultPage);
        }
    }
    AutoPtr<IView> view;
    GetChildAt(mDefaultPage, (IView**)&view);
    return view->RequestFocus();
}

ECode Workspace::SyncPages()
{
    return NOERROR
}

ECode Workspace::SyncPageItems(
    /* [in] */ Int32 page,
    /* [in] */ Boolean immediate)
{
    return NOERROR
}

ECode Workspace::GetCurrentPageDescription(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    Int32 page = (mNextPage != INVALID_PAGE) ? mNextPage : mCurrentPage;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(
            Elastos::Droid::Launcher2::R::string::workspace_scroll_format, &str);
    Int32 count;
    GetChildCount(&count);
    *str = String::Format(str, page + 1, count);
    return NOERROR;
}

ECode Workspace::GetLocationInDragLayer(
    /* [in] */ ArrayOf<Int32>* loc)
{
    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    return dragLayer->GetLocationInDragLayer(this, loc);
}

ECode Workspace::SetFadeForOverScroll(
    /* [in] */ Float fade)
{
    Boolean res;
    IsScrollingIndicatorEnabled(&res);
    if (!res) return NOERROR;

    mOverscrollFade = fade;
    Float reducedFade = 0.5f + 0.5f * (1 - fade);
    AutoPtr<IViewParent> _parent;
    GetParent((IViewParent**)&_parent);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(_parent);
    AutoPtr<IView> view;
    parent->FindViewById(
            Elastos::Droid::Launcher2::R::id::qsb_divider, (IView**)&view);
    AutoPtr<IImageView> qsbDivider = IImageView::Probe(view);

    AutoPtr<IView> view2;
    parent->FindViewById(
            Elastos::Droid::Launcher2:R::id::dock_divider, (IView**)&view2);
    AutoPtr<IImageView> dockDivider = ImageView::Probe(view2);
    AutoPtr<IView> scrollIndicator;
    GetScrollingIndicator((IView**)&scrollIndicator);

    CancelScrollingIndicatorAnimations();
    if (qsbDivider != NULL) qsbDivider->SetAlpha(reducedFade);
    if (dockDivider != NULL) dockDivider->SetAlpha(reducedFade);
    return scrollIndicator->SetAlpha(1 - fade);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos