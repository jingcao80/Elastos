
#include "elastos/droid/launcher2/PackageChangedReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(DeleteDropTarget::FlingAlongVectorAnimatorUpdateListener,
        Object, IAnimatorUpdateListener);

DeleteDropTarget::FlingAlongVectorAnimatorUpdateListener::FlingAlongVectorAnimatorUpdateListener(
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
    dragLayer->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    mFriction = 1f - (density * friction);

    CDecelerateInterpolator::New(0.75f, (ITimeInterpolator**)&mAlphaInterpolator);
}

ECode DeleteDropTarget::FlingAlongVectorAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IView> view;
    mDragLayer->GetAnimatedView((IView**)&view);
    AutoPtr<DragView> dragView = (DragView*)IObject::Probe(view);

    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> obj = IFloat::Probe(value);
    Float t;
    obj->GetValue(&t);

    Int64 curTime = AnimationUtils::GetCurrentAnimationTimeMillis();

    if (!mHasOffsetForScale) {
        mHasOffsetForScale = TRUE;
        Float scale;
        dragView->GetScaleX(&scale);

        Int32 width;
        dragView->GetMeasuredWidth(&width)
        Float xOffset = ((scale - 1f) * width) / 2f;
        Int32 height;
        dragView->GetMeasuredHeight(&height)
        Float yOffset = ((scale - 1f) * height) / 2f;

        Int32 l;
        mFrom->GetLeft(&l);
        mFrom->Setleft(l + xOffset);
        Int32 t;
        mFrom->GetTop(&t);
        mFrom->SetTop(t + yOffset);
    }

    Int32 x;
    mVelocity->GetX(&x);
    Int32 l;
    mFrom->GetLeft(&l);
    mFrom->SetLeft(l + (x * (curTime - mPrevTime) / 1000f));
    Int32 y;
    mVelocity->GetY(&y);
    Int32 t;
    mFrom->GetTop(&t);
    mFrom->SetTop(t + (y * (curTime - mPrevTime) / 1000f));

    Int32 left;
    mFrom->GetLeft(&left);
    dragView->SetTranslationX(left);
    Int32 top;
    mFrom->GetTop(&top);
    dragView->SetTranslationY(top);
    Float output;
    mAlphaInterpolator->GetInterpolation(t, &output);
    dragView->SetAlpha(1f - output);

    Int32 x2;
    mVelocity->GetX(&x2);
    mVelocity->SetX(x2 * mFriction);
    Int32 y2;
    mVelocity->GetY(&x2);
    mVelocity->SetY(y2 * mFriction);
    mPrevTime = curTime;
    return NOERROR;
}

DeleteDropTarget::MyRunnable::MyRunnable(
    /* [in] */ DeleteDropTarget* host)
    : mHost(host)
{
}

ECode DeleteDropTarget::MyRunnable::Run()
{
    mHost->mSearchDropTargetBar->OnDragEnd();
    mHost->mLauncher->ExitSpringLoadedDragMode();
    return mHost->CompleteDrop(d);
}

DeleteDropTarget::MyThread::MyThread(
    /* [in] */ ILauncherAppWidgetHost* appWidgetHost,
    /* [in] */ ILauncherAppWidgetInfo* launcherAppWidgetInfo)
    : mAppWidgetHost(appWidgetHost)
    , mLauncherAppWidgetInfo(launcherAppWidgetInfo)
{
}

ECode DeleteDropTarget::MyThread::Run()
{
    Int32 id;
    mLauncherAppWidgetInfo->GetAppWidgetId(&id);
    return mAppWidgetHost->DeleteAppWidgetId(id);
}

CAR_INTERFACE_IMPL(DeleteDropTarget::MyTimeInterpolator, Object, ITimeInterpolator);

ECode DeleteDropTarget::MyTimeInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    *result = t * t * t * t * t * t * t * t;
    return NOERROR;
}

CAR_INTERFACE_IMPL(DeleteDropTarget::MyIAnimatorUpdateListener,
        Object, IAnimatorUpdateListener);

DeleteDropTarget::MyIAnimatorUpdateListener::MyIAnimatorUpdateListener(
    /* [in] */ IDragLayer* dragLayer,
    /* [in] */ ITimeInterpolator* scaleAlphaInterpolator)
    : mDragLayer(dragLayer)
    , mScaleAlphaInterpolator(scaleAlphaInterpolator)
{
}

ECode DeleteDropTarget::MyIAnimatorUpdateListener::OnAnimationUpdate(
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
    dragView->GetScaleX(&scale);
    Int32 width;
    dragView->GetMeasuredWidth(&width);
    Float x1o = ((1f - scale) * width) / 2f;
    Int32 height;
    dragView->GetMeasuredHeight(&height);
    Float y1o = ((1f - scale) * height) / 2f;
    Float x = (1f - t) * (1f - t) * (x1 - x1o) + 2 * (1f - t) * t * (x2 - x1o) +
            (t * t) * x3;
    Float y = (1f - t) * (1f - t) * (y1 - y1o) + 2 * (1f - t) * t * (y2 - x1o) +
            (t * t) * y3;

    dragView->SetTranslationX(x);
    dragView->SetTranslationY(y);
    dragView->SetScaleX(initialScale * (1f - tp));
    dragView->SetScaleY(initialScale * (1f - tp));
    return dragView->SetAlpha(finalAlpha + (1f - finalAlpha) * (1f - tp));
}

CAR_INTERFACE_IMPL(DeleteDropTarget::MyTimeInterpolator2, Object, ITimeInterpolator);

DeleteDropTarget::MyTimeInterpolator2::MyTimeInterpolator2()
    : mCount(-1)
    , mOffset(0f)
{
}

//@Override
CARAPI DeleteDropTarget::MyTimeInterpolator2::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    if (mCount < 0) {
        mCount++;
    }
    else if (mCount == 0) {
        mOffset = Math::Min(0.5f, (Float)(AnimationUtils::GetCurrentAnimationTimeMillis() -
                startTime) / duration);
        mCount++;
    }
    *result = Math::Min(1f, mOffset + t);
    return NOERROR;
}

DeleteDropTarget::MyRunnable2::MyRunnable2(
    /* [in] */ DeleteDropTarget* host,
    /* [in] */ Boolean isAllApps,
    /* [in] */ DragObject* d)
    : mHost(host)
    , mIsAllApps(isAllApps)
    , mD(d)
{
}

ECode DeleteDropTarget::MyRunnable2::Run()
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

Int32 DeleteDropTarget::DELETE_ANIMATION_DURATION = 285;
Int32 DeleteDropTarget::FLING_DELETE_ANIMATION_DURATION = 350;
Float DeleteDropTarget::FLING_TO_DELETE_FRICTION = 0.035f;
Int32 DeleteDropTarget::MODE_FLING_DELETE_TO_TRASH = 0;
Int32 DeleteDropTarget::MODE_FLING_DELETE_ALONG_VECTOR = 1;

CAR_INTERFACE_IMPL(DeleteDropTarget, ButtonDropTarget, IDeleteDropTarget);

DeleteDropTarget::DeleteDropTarget()
    : mFlingDeleteMode(MODE_FLING_DELETE_ALONG_VECTOR)
{
}

ECode DeleteDropTarget::construcor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return construcor(context, attrs, 0);
}

ECode DeleteDropTarget::construcor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return ButtonDropTarget::construcor(context, attrs, defStyle);
}

ECode DeleteDropTarget::OnFinishInflate()
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
    r->GetDrawable(Elastos::Droid::Launcher2::R::drawable::remove_target_selector);
    mRemoveDrawable = ITransitionDrawable::Probe(drawable2);

    mRemoveDrawable->SetCrossFadeEnabled(TRUE);
    mUninstallDrawable->SetCrossFadeEnabled(TRUE);

    // The current drawable is set to either the remove drawable or the uninstall drawable
    // and is initially set to the remove drawable, as set in the layout xml.
    AutoPtr<IDrawable> drawable3 = GetCurrentDrawable();
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
            SetText("");
        }
    }
    return NOERROR;
}

Boolean DeleteDropTarget::IsAllAppsApplication(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info)
{
    return (IAppsCustomizePagedView::Probe(source) != NULL) &&
           (IApplicationInfo::Probe(info) != NULL);
}

Boolean DeleteDropTarget::IsAllAppsWidget(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info)
{
    if (IAppsCustomizePagedView::Probe(source) != NULL) {
        if (IPendingAddItemInfo::Probe(info) != NULL) {
            AutoPtr<IPendingAddItemInfo> addInfo = IPendingAddItemInfo::Probe(info);
            Int32 itemType;
            addInfo->GetItemType(&itemType);
            switch (itemType) {
                case LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT:
                case LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET:
                    return TRUE;
            }
        }
    }
    return FALSE;
}

Boolean DeleteDropTarget::IsDragSourceWorkspaceOrFolder(
    /* [in] */ DragObject* d)
{
    return (IWorkspace::Probe(d->mDragSource) != NULL) ||
            (IFolder::Probe(d->mDragSource) != NULL);
}

Boolean DeleteDropTarget::IsWorkspaceOrFolderApplication(
    /* [in] */ DragObject* d)
{
    return IsDragSourceWorkspaceOrFolder(d) &&
            (IShortcutInfo::Probe(d->mDragInfo) != NULL);
}

Boolean DeleteDropTarget::IsWorkspaceOrFolderWidget(
    /* [in] */ DragObject* d)
{
    return IsDragSourceWorkspaceOrFolder(d) &&
            (ILauncherAppWidgetInfo::Probe(d->mDragInfo) != NULL);
}

Boolean DeleteDropTarget::IsWorkspaceFolder(
    /* [in] */ DragObject* d)
{
    return (IWorkspace::Probe(d->mDragSource) != NULL) &&
            (IFolderInfo::Probe(d->mDragInfo) != NULL);
}

void DeleteDropTarget::SetHoverColor()
{
    mCurrentDrawable->StartTransition(mTransitionDuration);
    SetTextColor(mHoverColor);
}

void DeleteDropTarget::ResetHoverColor()
{
    mCurrentDrawable->ResetTransition();
    SetTextColor(mOriginalTextColor);
}

ECode DeleteDropTarget::AcceptDrop(
    /* [in] */ DragObject* d,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    // We can remove everything including App shortcuts, folders, widgets, etc.
    *res = TRUE;
    return NOERROR;
}

ECode DeleteDropTarget::OnDragStart(
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
        AutoPtr<IApplicationInfo> appInfo = IApplicationInfo::Probe(info);
        ApplicationInfo* _appInfo = (ApplicationInfo*)appInfo;
        if ((_appInfo->mFlags & ApplicationInfo::DOWNLOADED_FLAG) != 0) {
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
        SetCompoundDrawablesRelativeWithIntrinsicBounds(mUninstallDrawable, NULL, NULL, NULL);
    }
    else {
        SetCompoundDrawablesRelativeWithIntrinsicBounds(mRemoveDrawable, NULL, NULL, NULL);
    }
    AutoPtr<IDrawable> drawable;
    GetCurrentDrawable((IDrawable**)&drawable);
    mCurrentDrawable = ITransitionDrawable::Probe(drawable);

    mActive = isVisible;
    ResetHoverColor();
    AutoPtr<IViewParent> parent;
    GetParent((IViewParent**)&parent);
    AutoPtr<IViewGroup> group = IViewGroup::probe(parent);
    group->SetVisibility(isVisible ? IView::VISIBLE : IView::GONE);
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

ECode DeleteDropTarget::OnDragEnd()
{
    ButtonDropTarget::OnDragEnd();
    mActive = FALSE;
    return NOERROR;
}

ECode DeleteDropTarget::OnDragEnter(
    /* [in] */ DragObject* d)
{
    ButtonDropTarget::OnDragEnter(d);

    SetHoverColor();
    return NOERROR;
}

ECode DeleteDropTarget::OnDragExit(
    /* [in] */ DragObject* d)
{
    ButtonDropTarget::OnDragExit(d);

    if (!d->mDragComplete) {
        ResetHoverColor();
    }
    else {
        // Restore the hover color if we are deleting
        return d->mDragView->SetColor(mHoverColor);
    }
    return NOERROR;
}

void DeleteDropTarget::AnimateToTrashAndCompleteDrop(
    /* [in] */ DragObject* d)
{
    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    AutoPtr<IRect> from;
    CRect::New((IRect**)&from);
    dragLayer->GetViewRectRelativeToSelf(d->mDragView, from);
    Int32 mWidth;
    d->mDragView->GetMeasuredWidth(&mWidth);
    Int32 mHeight;
    d->mDragView->GetMeasuredHeight(&mHeight);
    Int32 iWidth;
    mCurrentDrawable->GetIntrinsicWidth(&iWidth);
    Int32 iHeight;
    mCurrentDrawable->GetIntrinsicHeight(&iHeight);
    AutoPtr<IRect> to;
    GetIconRect(mWidth, mHeight, iWidth, iHeight, (IRect**)&to);

    Int32 width;
    to->GetWidth(&width);
    Int32 width2;
    from->GetWidth(&width2);
    Float scale = (Float)width / width2;

    mSearchDropTargetBar->DeferOnDragEnd();
    AutoPtr<IRunnable> onAnimationEndRunnable = new MyRunnable(this);
    AutoPtr<IInterpolator> polator;
    CDecelerateInterpolator::New(2, (IInterpolator**)&polator);
    AutoPtr<IInterpolator> polator2;
    CLinearInterpolator::New((IInterpolator**)&polator2);
    dragLayer->AnimateView(d->mDragView, from, to, scale, 1f, 1f, 0.1f, 0.1f,
            DELETE_ANIMATION_DURATION, polator,
            polator2, onAnimationEndRunnable,
            IDragLayer::ANIMATION_END_DISAPPEAR, NULL);
}

void DeleteDropTarget::CompleteDrop(
    /* [in] */ DragObject* d)
{
    AutoPtr<IItemInfo> item = IItemInfo::Probe(d->mDragInfo);

    if (IsAllAppsApplication(d->mDragSource, item)) {
        // Uninstall the application if it is being dragged from AppsCustomize
        AutoPtr<IUserHandle> user;
        item->Getuser((IUserHandle**)&user);
        mLauncher->StartApplicationUninstallActivity(IApplicationInfo::Probe(item), user);
    }
    else if (IsWorkspaceOrFolderApplication(d)) {
        LauncherModel::DeleteItemFromDatabase(mLauncher, item);
    }
    else if (IsWorkspaceFolder(d)) {
        // Remove the folder from the workspace and delete the contents from launcher model
        AutoPtr<IFolderInfo> folderInfo = IFolderInfo::Probe(item);
        mLauncher->RemoveFolder(folderInfo);
        LauncherModel::DeleteFolderContentsFromDatabase(mLauncher, folderInfo);
    }
    else if (IsWorkspaceOrFolderWidget(d)) {
        // Remove the widget from the workspace
        mLauncher->RemoveAppWidget(ILauncherAppWidgetInfo::Probe(item));
        LauncherModel::DeleteItemFromDatabase(mLauncher, item);

        AutoPtr<ILauncherAppWidgetInfo> launcherAppWidgetInfo = ILauncherAppWidgetInfo::Probe(item);
        AutoPtr<ILauncherAppWidgetHost> appWidgetHost;
        mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&appWidgetHost);
        if (appWidgetHost != NULL) {
            // Deleting an app widget ID is a void call but writes to disk before returning
            // to the caller...
            AutoPtr<MyThread> thread = new MyThread(appWidgetHost, launcherAppWidgetInfo);
            thread.Start();
        }
    }
}

ECode DeleteDropTarget::OnDrop(
    /* [in] */ DragObject* d)
{
    AnimateToTrashAndCompleteDrop(d);
    return NOERROR;
}

AutoPtr<IAnimatorUpdateListener> DeleteDropTarget::CreateFlingToTrashAnimatorListener(
    /* [in] */ IDragLayer* dragLayer,
    /* [in] */ DragObject* d,
    /* [in] */ IPointF* vel,
    /* [in] */ IViewConfiguration* config)
{
    Int32 mWidth;
    d->mDragView->GetMeasuredWidth(&mWidth);
    Int32 mHeight;
    d->mDragView->GetMeasuredHeight(&mHeight);
    Int32 iWidth;
    mCurrentDrawable->GetIntrinsicWidth(&iWidth);
    Int32 iHeight;
    mCurrentDrawable->GetIntrinsicHeight(&iHeight);
    AutoPtr<IRect> to;
    GetIconRect(mWidth, mHeight, iWidth, iHeight);
    AutoPtr<IRect> from;
    CRect::New((IRect**)&from);
    dragLayer->GetViewRectRelativeToSelf(d->mDragView, from);

    // Calculate how far along the velocity vector we should put the intermediate point on
    // the bezier curve
    Float l;
    vel->Length(&l);
    Float velocity = Math::Abs(l);
    Int32 maximumFlingVelocity;
    config->GetScaledMaximumFlingVelocity(&maximumFlingVelocity);
    Float vp = Math::Min(1f, velocity / (maximumFlingVelocity / 2f));
    Int32 offsetY = (Int32)(-from.top * vp);
    Int32 offsetX = (Int32)(offsetY / (vel.y / vel.x));

    Int32 top;
    from->GetTop(&top);
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

    AutoPtr<ITimeInterpolator> scaleAlphaInterpolator = new MyTimeInterpolator();
    AutoPtr<IAnimatorUpdateListener> listener = new MyIAnimatorUpdateListener(
            dragLayer, scaleAlphaInterpolator);
    return listener;
}

AutoPtr<IAnimatorUpdateListener> DeleteDropTarget::CreateFlingAlongVectorAnimatorListener(
    /* [in] */ IDragLayer* dragLayer,
    /* [in] */ DragObject* d,
    /* [in] */ IPointF* vel,
    /* [in] */ Int64 startTime,
    /* [in] */ Int32 duration,
    /* [in] */ IViewConfiguration* config)
{
    AutoPtr<IRect> from;
    CRect::New((IRect**)&from);
    dragLayer->GetViewRectRelativeToSelf(d->mDragView, from);

    AutoPtr<IAnimatorUpdateListener> listener = new FlingAlongVectorAnimatorUpdateListener(
            dragLayer, vel, from, startTime, FLING_TO_DELETE_FRICTION);
    return listener;
}

ECode DeleteDropTarget::OnFlingToDelete(
    /* [in] */ DragObject* d,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPointF* vel)
{
    Boolean isAllApps = IAppsCustomizePagedView::Probe(d->mDragSource) != NULL;

    // Don't highlight the icon as it's animating
    d->mDragView->SetColor(0);
    d->mDragView->UpdateInitialScaleToCurrentScale();
    // Don't highlight the target if we are flinging from AllApps
    if (isAllApps) {
        ResetHoverColor();
    }

    if (mFlingDeleteMode == MODE_FLING_DELETE_TO_TRASH) {
        // Defer animating out the drop target if we are animating to it
        mSearchDropTargetBar->DeferOnDragEnd();
        mSearchDropTargetBar->FinishAnimations();
    }

    AutoPtr<IViewConfiguration> config;
    ViewConfiguration::Get(mLauncher, (IViewConfiguration**)&config);
    AutoPtr<IDragLayer> dragLayer;
    mLauncher-GetDragLayer((IDragLayer**)&dragLayer);
    const Int32 duration = FLING_DELETE_ANIMATION_DURATION;
    const Int64 startTime = AnimationUtils::GetCurrentAnimationTimeMillis();

    // NOTE: Because it takes time for the first frame of animation to actually be
    // called and we expect the animation to be a continuation of the fling, we have
    // to account for the time that has elapsed since the fling finished.  And since
    // we don't have a startDelay, we will always get call to update when we call
    // start() (which we want to ignore).
    AutoPtr<ITimeInterpolator> tInterpolator = new MyTimeInterpolator2();
    AutoPtr<IAnimatorUpdateListener> updateCb;
    if (mFlingDeleteMode == MODE_FLING_DELETE_TO_TRASH) {
        updateCb = CountreateFlingToTrashAnimatorListener(dragLayer, d, vel, config);
    }
    else if (mFlingDeleteMode == MODE_FLING_DELETE_ALONG_VECTOR) {
        updateCb = CountreateFlingAlongVectorAnimatorListener(dragLayer, d, vel, startTime,
                duration, config);
    }
    AutoPtr<IRunnable> onAnimationEndRunnable = new MyRunnable2(this, isAllApps, d);
    return dragLayer->AnimateView(d->mDragView, updateCb, duration, tInterpolator, onAnimationEndRunnable,
            IDragLayer::ANIMATION_END_DISAPPEAR, NULL);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos