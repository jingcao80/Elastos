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

#include "elastos/droid/launcher2/CDeleteDropTarget.h"
#include "elastos/droid/launcher2/PendingAddItemInfo.h"
#include "elastos/droid/launcher2/ApplicationInfo.h"
#include "elastos/droid/launcher2/LauncherAppWidgetInfo.h"
#include "elastos/droid/launcher2/LauncherAppWidgetHost.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/launcher2/FolderInfo.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "elastos/droid/launcher2/LauncherModel.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include "R.h"

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Core::IFloat;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CDeleteDropTarget::FlingAlongVectorAnimatorUpdateListener,
        Object, IAnimatorUpdateListener);

CDeleteDropTarget::FlingAlongVectorAnimatorUpdateListener::FlingAlongVectorAnimatorUpdateListener(
    /* [in] */ IDragLayer* dragLayer,
    /* [in] */ IPointF* vel,
    /* [in] */ IRect* from,
    /* [in] */ Int64 startTime,
    /* [in] */ Float friction)
{
    mDragLayer = dragLayer;
    mVelocity = vel;
    mFrom = from;
    mPrevTime = startTime;
    mHasOffsetForScale = FALSE;

    AutoPtr<IResources> resources;
    IView::Probe(dragLayer)->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    mFriction = 1.0f - (density * friction);

    CDecelerateInterpolator::New(0.75f, (ITimeInterpolator**)&mAlphaInterpolator);
}

ECode CDeleteDropTarget::FlingAlongVectorAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IView> view;
    mDragLayer->GetAnimatedView((IView**)&view);
    AutoPtr<IDragView> dragView = IDragView::Probe(view);

    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> obj = IFloat::Probe(value);
    Float t;
    obj->GetValue(&t);

    Int64 curTime;
    AnimationUtils::CurrentAnimationTimeMillis(&curTime);

    if (!mHasOffsetForScale) {
        mHasOffsetForScale = TRUE;
        Float scale;
        IView::Probe(dragView)->GetScaleX(&scale);

        Int32 width;
        IView::Probe(dragView)->GetMeasuredWidth(&width);
        Float xOffset = ((scale - 1.0f) * width) / 2.0f;
        Int32 height;
        IView::Probe(dragView)->GetMeasuredHeight(&height);
        Float yOffset = ((scale - 1.0f) * height) / 2.0f;

        Int32 l;
        mFrom->GetLeft(&l);
        mFrom->SetLeft(l + xOffset);
        Int32 t;
        mFrom->GetTop(&t);
        mFrom->SetTop(t + yOffset);
    }

    Float x;
    mVelocity->GetX(&x);
    Int32 l;
    mFrom->GetLeft(&l);
    mFrom->SetLeft(l + (x * (curTime - mPrevTime) / 1000.0f));
    Float y;
    mVelocity->GetY(&y);
    Int32 top1;
    mFrom->GetTop(&top1);
    mFrom->SetTop(top1 + (y * (curTime - mPrevTime) / 1000.0f));

    Int32 left;
    mFrom->GetLeft(&left);
    IView::Probe(dragView)->SetTranslationX(left);
    Int32 top;
    mFrom->GetTop(&top);
    IView::Probe(dragView)->SetTranslationY(top);
    Float output;
    mAlphaInterpolator->GetInterpolation(t, &output);
    IView::Probe(dragView)->SetAlpha(1.0f - output);

    Float x2;
    mVelocity->GetX(&x2);
    mVelocity->SetX(x2 * mFriction);
    Float y2;
    mVelocity->GetY(&y2);
    mVelocity->SetY(y2 * mFriction);
    mPrevTime = curTime;
    return NOERROR;
}

CDeleteDropTarget::MyRunnable::MyRunnable(
    /* [in] */ CDeleteDropTarget* host,
    /* [in] */ DragObject* d)
    : mHost(host)
    , mD(d)
{
}

ECode CDeleteDropTarget::MyRunnable::Run()
{
    IDragControllerDragListener::Probe(mHost->mSearchDropTargetBar)->OnDragEnd();
    mHost->mLauncher->ExitSpringLoadedDragMode();
    mHost->CompleteDrop(mD);
    return NOERROR;
}

CDeleteDropTarget::MyThread::MyThread(
    /* [in] */ LauncherAppWidgetHost* appWidgetHost,
    /* [in] */ LauncherAppWidgetInfo* launcherAppWidgetInfo)
    : mAppWidgetHost(appWidgetHost)
    , mLauncherAppWidgetInfo(launcherAppWidgetInfo)
{
    Thread::constructor();
}

ECode CDeleteDropTarget::MyThread::Run()
{
    return mAppWidgetHost->DeleteAppWidgetId(mLauncherAppWidgetInfo->mAppWidgetId);
}

CAR_INTERFACE_IMPL(CDeleteDropTarget::FlingToTrashAnimatorInterpolator, Object, ITimeInterpolator);

ECode CDeleteDropTarget::FlingToTrashAnimatorInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    *result = t * t * t * t * t * t * t * t;
    return NOERROR;
}

ECode CDeleteDropTarget::FlingToTrashAnimatorInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CDeleteDropTarget::FlingToTrashAnimatorUpdateListener,
        Object, IAnimatorUpdateListener);

CDeleteDropTarget::FlingToTrashAnimatorUpdateListener::FlingToTrashAnimatorUpdateListener(
    /* [in] */ IDragLayer* dragLayer,
    /* [in] */ ITimeInterpolator* scaleAlphaInterpolator,
    /* [in] */ Float x1,
    /* [in] */ Float x2,
    /* [in] */ Float x3,
    /* [in] */ Float y1,
    /* [in] */ Float y2,
    /* [in] */ Float y3)
    : mDragLayer(dragLayer)
    , mScaleAlphaInterpolator(scaleAlphaInterpolator)
{
}

ECode CDeleteDropTarget::FlingToTrashAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IView> view;
    mDragLayer->GetAnimatedView((IView**)&view);
    AutoPtr<IDragView> dragView = IDragView::Probe(view);

    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> obj = IFloat::Probe(value);
    Float t;
    obj->GetValue(&t);

    Float tp;
    mScaleAlphaInterpolator->GetInterpolation(t, &tp);
    Float initialScale;
    dragView->GetInitialScale(&initialScale);
    Float finalAlpha = 0.5f;
    Float scale;
    view->GetScaleX(&scale);
    Int32 width;
    view->GetMeasuredWidth(&width);
    Float x1o = ((1.0f - scale) * width) / 2.0f;
    Int32 height;
    view->GetMeasuredHeight(&height);
    Float y1o = ((1.0f - scale) * height) / 2.0f;
    Float x = (1.0f - t) * (1.0f - t) * (mX1 - x1o) + 2 * (1.0f - t) * t * (mX2 - x1o) +
            (t * t) * mX3;
    Float y = (1.0f - t) * (1.0f - t) * (mY1 - y1o) + 2 * (1.0f - t) * t * (mY2 - x1o) +
            (t * t) * mY3;

    view->SetTranslationX(x);
    view->SetTranslationY(y);
    view->SetScaleX(initialScale * (1.0f - tp));
    view->SetScaleY(initialScale * (1.0f - tp));
    return view->SetAlpha(finalAlpha + (1.0f - finalAlpha) * (1.0f - tp));
}

CAR_INTERFACE_IMPL(CDeleteDropTarget::FlingToDeleteAnimatorInterpolator, Object, ITimeInterpolator);

CDeleteDropTarget::FlingToDeleteAnimatorInterpolator::FlingToDeleteAnimatorInterpolator(
    /* [in] */ Int64 startTime,
    /* [in] */ Int32 duration)
    : mCount(-1)
    , mOffset(0.0f)
    , mStartTime(startTime)
    , mDuration(duration)
{
}

ECode CDeleteDropTarget::FlingToDeleteAnimatorInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    if (mCount < 0) {
        mCount++;
    }
    else if (mCount == 0) {
        Int64 time;
        AnimationUtils::CurrentAnimationTimeMillis(&time);
        mOffset = Elastos::Core::Math::Min(0.5f, ((Float)(time - mStartTime)) / mDuration);
        mCount++;
    }
    *result = Elastos::Core::Math::Min(1.0f, mOffset + t);
    return NOERROR;
}

ECode CDeleteDropTarget::FlingToDeleteAnimatorInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

CDeleteDropTarget::MyRunnable2::MyRunnable2(
    /* [in] */ CDeleteDropTarget* host,
    /* [in] */ Boolean isAllApps,
    /* [in] */ DragObject* d)
    : mHost(host)
    , mIsAllApps(isAllApps)
    , mD(d)
{
}

ECode CDeleteDropTarget::MyRunnable2::Run()
{
    // If we are dragging from AllApps, then we allow AppsCustomizePagedView to clean up
    // itself, otherwise, complete the drop to initiate the deletion process
    if (!mIsAllApps) {
        mHost->mLauncher->ExitSpringLoadedDragMode();
        mHost->CompleteDrop(mD);
    }
    AutoPtr<IDragController> controller;
    mHost->mLauncher->GetDragController((IDragController**)&controller);
    return controller->OnDeferredEndFling(mD);
}

Int32 CDeleteDropTarget::DELETE_ANIMATION_DURATION = 285;
Int32 CDeleteDropTarget::FLING_DELETE_ANIMATION_DURATION = 350;
Float CDeleteDropTarget::FLING_TO_DELETE_FRICTION = 0.035f;
Int32 CDeleteDropTarget::MODE_FLING_DELETE_TO_TRASH = 0;
Int32 CDeleteDropTarget::MODE_FLING_DELETE_ALONG_VECTOR = 1;

CAR_INTERFACE_IMPL(CDeleteDropTarget, ButtonDropTarget, IDeleteDropTarget);

CAR_OBJECT_IMPL(CDeleteDropTarget);

CDeleteDropTarget::CDeleteDropTarget()
    : mFlingDeleteMode(MODE_FLING_DELETE_ALONG_VECTOR)
{
}

ECode CDeleteDropTarget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CDeleteDropTarget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return ButtonDropTarget::constructor(context, attrs, defStyle);
}

ECode CDeleteDropTarget::OnFinishInflate()
{
    ButtonDropTarget::OnFinishInflate();

    // Get the drawable
    GetTextColors((IColorStateList**)&mOriginalTextColor);

    // Get the hover color
    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    r->GetColor(Elastos::Droid::Launcher2::R::color::delete_target_hover_tint,
            &mHoverColor);
    AutoPtr<IDrawable> drawable;
    r->GetDrawable(Elastos::Droid::Launcher2::R::drawable::uninstall_target_selector,
            (IDrawable**)&drawable);
    mUninstallDrawable = ITransitionDrawable::Probe(drawable);

    AutoPtr<IDrawable> drawable2;
    r->GetDrawable(Elastos::Droid::Launcher2::R::drawable::remove_target_selector,
            (IDrawable**)&drawable2);
    mRemoveDrawable = ITransitionDrawable::Probe(drawable2);

    mRemoveDrawable->SetCrossFadeEnabled(TRUE);
    mUninstallDrawable->SetCrossFadeEnabled(TRUE);

    // The current drawable is set to either the remove drawable or the uninstall drawable
    // and is initially set to the remove drawable, as set in the layout xml.
    AutoPtr<IDrawable> drawable3;
    GetCurrentDrawable((IDrawable**)&drawable3);
    mCurrentDrawable = ITransitionDrawable::Probe(drawable3);

    // Remove the text in the Phone UI in landscape
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    if (orientation == IConfiguration::ORIENTATION_LANDSCAPE) {
        Boolean res;
        LauncherApplication::IsScreenLarge(&res);
        if (!res) {
            AutoPtr<ICharSequence> text = CoreUtils::Convert(String(""));
            SetText(text);
        }
    }
    return NOERROR;
}

Boolean CDeleteDropTarget::IsAllAppsApplication(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info)
{
    return (IAppsCustomizePagedView::Probe(source) != NULL) &&
           (IApplicationInfo::Probe(info) != NULL);
}

Boolean CDeleteDropTarget::IsAllAppsWidget(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info)
{
    if (IAppsCustomizePagedView::Probe(source) != NULL) {
        if (IPendingAddItemInfo::Probe(info) != NULL) {
            PendingAddItemInfo* addInfo = (PendingAddItemInfo*)IPendingAddItemInfo::Probe(info);
            switch (addInfo->mItemType) {
                case ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_SHORTCUT:
                case LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET:
                    return TRUE;
            }
        }
    }
    return FALSE;
}

Boolean CDeleteDropTarget::IsDragSourceWorkspaceOrFolder(
    /* [in] */ DragObject* d)
{
    return (IWorkspace::Probe(d->mDragSource) != NULL) ||
            (IFolder::Probe(d->mDragSource) != NULL);
}

Boolean CDeleteDropTarget::IsWorkspaceOrFolderApplication(
    /* [in] */ DragObject* d)
{
    return IsDragSourceWorkspaceOrFolder(d) &&
            (IShortcutInfo::Probe(d->mDragInfo) != NULL);
}

Boolean CDeleteDropTarget::IsWorkspaceOrFolderWidget(
    /* [in] */ DragObject* d)
{
    return IsDragSourceWorkspaceOrFolder(d) &&
            (ILauncherAppWidgetInfo::Probe(d->mDragInfo) != NULL);
}

Boolean CDeleteDropTarget::IsWorkspaceFolder(
    /* [in] */ DragObject* d)
{
    return (IWorkspace::Probe(d->mDragSource) != NULL) &&
            (IFolderInfo::Probe(d->mDragInfo) != NULL);
}

void CDeleteDropTarget::SetHoverColor()
{
    mCurrentDrawable->StartTransition(mTransitionDuration);
    SetTextColor(mHoverColor);
}

void CDeleteDropTarget::ResetHoverColor()
{
    mCurrentDrawable->ResetTransition();
    SetTextColor(mOriginalTextColor);
}

ECode CDeleteDropTarget::AcceptDrop(
    /* [in] */ IDropTargetDragObject* d,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    // We can remove everything including App shortcuts, folders, widgets, etc.
    *res = TRUE;
    return NOERROR;
}

ECode CDeleteDropTarget::OnDragStart(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info,
    /* [in] */ Int32 dragAction)
{
    Boolean isVisible = TRUE;
    Boolean isUninstall = FALSE;

    // If we are dragging a widget from AppsCustomize, hide the delete target
    if (IsAllAppsWidget(source, info)) {
        isVisible = FALSE;
    }

    // If we are dragging an application from AppsCustomize, only show the control if we can
    // delete the app (it was downloaded), and rename the string to "uninstall" in such a case
    if (IsAllAppsApplication(source, info)) {
        ApplicationInfo* appInfo = (ApplicationInfo*)IApplicationInfo::Probe(info);
        if ((appInfo->mFlags & ApplicationInfo::DOWNLOADED_FLAG) != 0) {
            isUninstall = TRUE;
        }
        else {
            isVisible = FALSE;
        }
        // If the user is not allowed to access the app details page or uninstall, then don't
        // let them uninstall from here either.
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
        AutoPtr<IUserManager> userManager = IUserManager::Probe(obj);
        Boolean res;
        userManager->HasUserRestriction(IUserManager::DISALLOW_APPS_CONTROL, &res);
        Boolean res2;
        userManager->HasUserRestriction(IUserManager::DISALLOW_UNINSTALL_APPS, &res2);
        if (res || res2) {
            isVisible = FALSE;
        }
    }

    if (isUninstall) {
        SetCompoundDrawablesRelativeWithIntrinsicBounds(IDrawable::Probe(mUninstallDrawable), NULL, NULL, NULL);
    }
    else {
        SetCompoundDrawablesRelativeWithIntrinsicBounds(IDrawable::Probe(mRemoveDrawable), NULL, NULL, NULL);
    }
    AutoPtr<IDrawable> drawable;
    GetCurrentDrawable((IDrawable**)&drawable);
    mCurrentDrawable = ITransitionDrawable::Probe(drawable);

    mActive = isVisible;
    ResetHoverColor();
    AutoPtr<IViewParent> parent;
    GetParent((IViewParent**)&parent);
    AutoPtr<IViewGroup> group = IViewGroup::Probe(parent);
    IView::Probe(group)->SetVisibility(isVisible ? IView::VISIBLE : IView::GONE);
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    Int32 length;
    text->GetLength(&length);
    if (length > 0) {
        SetText(isUninstall ?
                Elastos::Droid::Launcher2::R::string::delete_target_uninstall_label
                : Elastos::Droid::Launcher2::R::string::delete_target_label);
    }
    return NOERROR;
}

ECode CDeleteDropTarget::OnDragEnd()
{
    ButtonDropTarget::OnDragEnd();
    mActive = FALSE;
    return NOERROR;
}

ECode CDeleteDropTarget::OnDragEnter(
    /* [in] */ IDropTargetDragObject* d)
{
    ButtonDropTarget::OnDragEnter(d);

    SetHoverColor();
    return NOERROR;
}

ECode CDeleteDropTarget::OnDragExit(
    /* [in] */ IDropTargetDragObject* d)
{
    ButtonDropTarget::OnDragExit(d);

    DragObject* _d = (DragObject*)d;
    if (!_d->mDragComplete) {
        ResetHoverColor();
    }
    else {
        // Restore the hover color if we are deleting
        return _d->mDragView->SetColor(mHoverColor);
    }
    return NOERROR;
}

void CDeleteDropTarget::AnimateToTrashAndCompleteDrop(
    /* [in] */ DragObject* d)
{
    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    AutoPtr<IRect> from;
    CRect::New((IRect**)&from);
    dragLayer->GetViewRectRelativeToSelf(IView::Probe(d->mDragView), from);
    Int32 mWidth;
    IView::Probe(d->mDragView)->GetMeasuredWidth(&mWidth);
    Int32 mHeight;
    IView::Probe(d->mDragView)->GetMeasuredHeight(&mHeight);
    Int32 iWidth;
    IDrawable::Probe(mCurrentDrawable)->GetIntrinsicWidth(&iWidth);
    Int32 iHeight;
    IDrawable::Probe(mCurrentDrawable)->GetIntrinsicHeight(&iHeight);
    AutoPtr<IRect> to;
    GetIconRect(mWidth, mHeight, iWidth, iHeight, (IRect**)&to);

    Int32 width;
    to->GetWidth(&width);
    Int32 width2;
    from->GetWidth(&width2);
    Float scale = (Float)width / width2;

    mSearchDropTargetBar->DeferOnDragEnd();
    AutoPtr<IRunnable> onAnimationEndRunnable = new MyRunnable(this, d);
    AutoPtr<IInterpolator> polator;
    CDecelerateInterpolator::New(2, (IInterpolator**)&polator);
    AutoPtr<IInterpolator> polator2;
    CLinearInterpolator::New((IInterpolator**)&polator2);
    dragLayer->AnimateView(d->mDragView, from, to, scale, 1.0f, 1.0f, 0.1f, 0.1f,
            DELETE_ANIMATION_DURATION, polator,
            polator2, onAnimationEndRunnable,
            IDragLayer::ANIMATION_END_DISAPPEAR, NULL);
}

void CDeleteDropTarget::CompleteDrop(
    /* [in] */ DragObject* d)
{
    AutoPtr<IItemInfo> item = IItemInfo::Probe(d->mDragInfo);
    ItemInfo* _item = (ItemInfo*)item.Get();

    if (IsAllAppsApplication(d->mDragSource, item)) {
        // Uninstall the application if it is being dragged from AppsCustomize
        mLauncher->StartApplicationUninstallActivity(IApplicationInfo::Probe(item), _item->mUser);
    }
    else if (IsWorkspaceOrFolderApplication(d)) {
        LauncherModel::DeleteItemFromDatabase(IContext::Probe(mLauncher), _item);
    }
    else if (IsWorkspaceFolder(d)) {
        // Remove the folder from the workspace and delete the contents from launcher model
        AutoPtr<IFolderInfo> folderInfo = IFolderInfo::Probe(item);
        mLauncher->RemoveFolder(folderInfo);
        LauncherModel::DeleteFolderContentsFromDatabase(IContext::Probe(mLauncher),
                (FolderInfo*)folderInfo.Get());
    }
    else if (IsWorkspaceOrFolderWidget(d)) {
        // Remove the widget from the workspace
        mLauncher->RemoveAppWidget(ILauncherAppWidgetInfo::Probe(item));
        LauncherModel::DeleteItemFromDatabase(IContext::Probe(mLauncher), _item);

        LauncherAppWidgetInfo* launcherAppWidgetInfo = (LauncherAppWidgetInfo*)ILauncherAppWidgetInfo::Probe(item);
        AutoPtr<ILauncherAppWidgetHost> appWidgetHost;
        mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&appWidgetHost);
        if (appWidgetHost != NULL) {
            // Deleting an app widget ID is a void call but writes to disk before returning
            // to the caller...
            LauncherAppWidgetHost* _appWidgetHost = (LauncherAppWidgetHost*)appWidgetHost.Get();
            AutoPtr<MyThread> thread = new MyThread(_appWidgetHost, launcherAppWidgetInfo);
            thread->Start();
        }
    }
}

ECode CDeleteDropTarget::OnDrop(
    /* [in] */ IDropTargetDragObject* d)
{
    AnimateToTrashAndCompleteDrop((DragObject*)d);
    return NOERROR;
}

AutoPtr<IAnimatorUpdateListener> CDeleteDropTarget::CreateFlingToTrashAnimatorListener(
    /* [in] */ IDragLayer* dragLayer,
    /* [in] */ DragObject* d,
    /* [in] */ IPointF* vel,
    /* [in] */ IViewConfiguration* config)
{
    Int32 mWidth;
    IView::Probe(d->mDragView)->GetMeasuredWidth(&mWidth);
    Int32 mHeight;
    IView::Probe(d->mDragView)->GetMeasuredHeight(&mHeight);
    Int32 iWidth;
    IDrawable::Probe(mCurrentDrawable)->GetIntrinsicWidth(&iWidth);
    Int32 iHeight;
    IDrawable::Probe(mCurrentDrawable)->GetIntrinsicHeight(&iHeight);
    AutoPtr<IRect> to;
    GetIconRect(mWidth, mHeight, iWidth, iHeight, (IRect**)&to);
    AutoPtr<IRect> from;
    CRect::New((IRect**)&from);
    dragLayer->GetViewRectRelativeToSelf(IView::Probe(d->mDragView), from);

    // Calculate how far along the velocity vector we should put the intermediate point on
    // the bezier curve
    Float l;
    vel->Length(&l);
    Float velocity = Elastos::Core::Math::Abs(l);
    Int32 maximumFlingVelocity;
    config->GetScaledMaximumFlingVelocity(&maximumFlingVelocity);
    Float vp = Elastos::Core::Math::Min(1.0f, velocity / (maximumFlingVelocity / 2.0f));

    Int32 top;
    from->GetTop(&top);
    Int32 offsetY = (Int32)(-top * vp);
    Float y;
    vel->GetY(&y);
    Float x;
    vel->GetX(&x);
    Int32 offsetX = (Int32)(offsetY / (y / x));

    const Float y2 = top + offsetY;                        // intermediate t/l
    Int32 left;
    from->GetLeft(&left);
    const Float x2 = left + offsetX;
    const Float x1 = left;                                 // drag view t/l
    const Float y1 = top;
    Int32 toLeft;
    to->GetLeft(&toLeft);
    const Float x3 = toLeft;                                   // delete target t/l
    Int32 toTop;
    to->GetTop(&toTop);
    const Float y3 = toTop;

    AutoPtr<ITimeInterpolator> scaleAlphaInterpolator = new FlingToTrashAnimatorInterpolator();
    AutoPtr<IAnimatorUpdateListener> listener = new FlingToTrashAnimatorUpdateListener(
            dragLayer, scaleAlphaInterpolator, x1, x2, x3, y1, y2, y3);
    return listener;
}

AutoPtr<IAnimatorUpdateListener> CDeleteDropTarget::CreateFlingAlongVectorAnimatorListener(
    /* [in] */ IDragLayer* dragLayer,
    /* [in] */ DragObject* d,
    /* [in] */ IPointF* vel,
    /* [in] */ Int64 startTime,
    /* [in] */ Int32 duration,
    /* [in] */ IViewConfiguration* config)
{
    AutoPtr<IRect> from;
    CRect::New((IRect**)&from);
    dragLayer->GetViewRectRelativeToSelf(IView::Probe(d->mDragView), from);

    AutoPtr<IAnimatorUpdateListener> listener = new FlingAlongVectorAnimatorUpdateListener(
            dragLayer, vel, from, startTime, FLING_TO_DELETE_FRICTION);
    return listener;
}

ECode CDeleteDropTarget::OnFlingToDelete(
    /* [in] */ IDropTargetDragObject* d,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPointF* vel)
{
    DragObject* _d = (DragObject*)d;
    Boolean isAllApps = IAppsCustomizePagedView::Probe(_d->mDragSource) != NULL;

    // Don't highlight the icon as it's animating
    _d->mDragView->SetColor(0);
    _d->mDragView->UpdateInitialScaleToCurrentScale();
    // Don't highlight the target if we are flinging from AllApps
    if (isAllApps) {
        ResetHoverColor();
    }

    if (mFlingDeleteMode == MODE_FLING_DELETE_TO_TRASH) {
        // Defer animating out the drop target if we are animating to it
        mSearchDropTargetBar->DeferOnDragEnd();
        mSearchDropTargetBar->FinishAnimations();
    }

    AutoPtr<IViewConfigurationHelper> vcHelper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vcHelper);
    AutoPtr<IViewConfiguration> config;
    vcHelper->Get(IContext::Probe(mLauncher), (IViewConfiguration**)&config);
    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    const Int32 duration = FLING_DELETE_ANIMATION_DURATION;
    Int64 startTime;
    AnimationUtils::CurrentAnimationTimeMillis(&startTime);

    // NOTE: Because it takes time for the first frame of animation to actually be
    // called and we expect the animation to be a continuation of the fling, we have
    // to account for the time that has elapsed since the fling finished.  And since
    // we don't have a startDelay, we will always get call to update when we call
    // start() (which we want to ignore).
    AutoPtr<ITimeInterpolator> tInterpolator = new FlingToDeleteAnimatorInterpolator(startTime, duration);
    AutoPtr<IAnimatorUpdateListener> updateCb;
    if (mFlingDeleteMode == MODE_FLING_DELETE_TO_TRASH) {
        updateCb = CreateFlingToTrashAnimatorListener(dragLayer, _d, vel, config);
    }
    else if (mFlingDeleteMode == MODE_FLING_DELETE_ALONG_VECTOR) {
        updateCb = CreateFlingAlongVectorAnimatorListener(dragLayer, _d, vel, startTime,
                duration, config);
    }
    AutoPtr<IRunnable> onAnimationEndRunnable = new MyRunnable2(this, isAllApps, _d);
    return dragLayer->AnimateView(_d->mDragView, updateCb, duration, tInterpolator, onAnimationEndRunnable,
            IDragLayer::ANIMATION_END_DISAPPEAR, NULL);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos