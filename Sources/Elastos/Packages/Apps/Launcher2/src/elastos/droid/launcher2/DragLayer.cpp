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

#include "elastos/droid/launcher2/DragLayer.h"
#include "elastos/droid/launcher2/CellLayout.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "elastos/droid/launcher2/CDragLayerLayoutParams.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Animation::CValueAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::EIID_IViewGroupOnHierarchyChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::IFloat;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(DragLayer::DragLayerLayoutParams, FrameLayout::FrameLayoutLayoutParams,
        IDragLayerLayoutParams);

DragLayer::DragLayerLayoutParams::DragLayerLayoutParams()
    : mX(0)
    , mY(0)
    , mCustomPosition(FALSE)
{
}

ECode DragLayer::DragLayerLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return FrameLayout::LayoutParams::constructor(width, height);
}

ECode DragLayer::DragLayerLayoutParams::SetWidth(
    /* [in] */ Int32 width)
{
    mWidth = width;
    return NOERROR;
}

ECode DragLayer::DragLayerLayoutParams::GetWidth(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mWidth;
    return NOERROR;
}

ECode DragLayer::DragLayerLayoutParams::SetHeight(
    /* [in] */ Int32 height)
{
    mHeight = height;
    return NOERROR;
}

ECode DragLayer::DragLayerLayoutParams::GetHeight(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mHeight;
    return NOERROR;
}

ECode DragLayer::DragLayerLayoutParams::SetX(
    /* [in] */ Int32 x)
{
    mX = x;
    return NOERROR;
}

ECode DragLayer::DragLayerLayoutParams::GetX(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mX;
    return NOERROR;
}

ECode DragLayer::DragLayerLayoutParams::SetY(
    /* [in] */ Int32 y)
{
    mY = y;
    return NOERROR;
}

ECode DragLayer::DragLayerLayoutParams::GetY(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mY;
    return NOERROR;
}

CAR_INTERFACE_IMPL(DragLayer::AnimateViewAnimatorUpdateListener, Object, IAnimatorUpdateListener);

DragLayer::AnimateViewAnimatorUpdateListener::AnimateViewAnimatorUpdateListener(
    /* [in] */ IDragView* view,
    /* [in] */ IInterpolator* alphaInterpolator,
    /* [in] */ IInterpolator* motionInterpolator,
    /* [in] */ Float initScaleX,
    /* [in] */ Float initScaleY,
    /* [in] */ Float dropViewScale,
    /* [in] */ Float finalScaleX,
    /* [in] */ Float finalScaleY,
    /* [in] */ Float finalAlpha,
    /* [in] */ Float initAlpha,
    /* [in] */ IRect* from,
    /* [in] */ IRect* to,
    /* [in] */ DragLayer* host)
    : mView(view)
    , mAlphaInterpolator(alphaInterpolator)
    , mMotionInterpolator(motionInterpolator)
    , mInitScaleX(initScaleX)
    , mInitScaleY(initScaleY)
    , mDropViewScale(dropViewScale)
    , mFinalScaleX(finalScaleX)
    , mFinalScaleY(finalScaleY)
    , mFinalAlpha(finalAlpha)
    , mInitAlpha(initAlpha)
    , mFrom(from)
    , mTo(to)
    , mHost(host)
{
}

ECode DragLayer::AnimateViewAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> fValue = IFloat::Probe(obj);
    Float percent;
    fValue->GetValue(&percent);
    Int32 width, height;
    IView::Probe(mView)->GetMeasuredWidth(&width);
    IView::Probe(mView)->GetMeasuredHeight(&height);

    Float alphaPercent;
    if (mAlphaInterpolator == NULL) {
        alphaPercent = percent;
    }
    else {
        ITimeInterpolator::Probe(mAlphaInterpolator)->GetInterpolation(percent, &alphaPercent);
    }

    Float motionPercent;
    if (mMotionInterpolator == NULL) {
        motionPercent = percent;
    }
    else {
        ITimeInterpolator::Probe(mMotionInterpolator)->GetInterpolation(percent, &motionPercent);
    }

    Float initialScaleX = mInitScaleX * mDropViewScale;
    Float initialScaleY = mInitScaleY * mDropViewScale;
    Float scaleX = mFinalScaleX * percent + initialScaleX * (1 - percent);
    Float scaleY = mFinalScaleY * percent + initialScaleY * (1 - percent);
    Float alpha = mFinalAlpha * alphaPercent + mInitAlpha * (1 - alphaPercent);

    Int32 fromleft, fromtop;
    mFrom->GetLeft(&fromleft);
    mFrom->GetTop(&fromtop);
    Float fromLeft = fromleft + (initialScaleX - 1.0f) * width / 2;
    Float fromTop = fromtop + (initialScaleY - 1.0f) * height / 2;

    Int32 toleft, totop;
    mTo->GetLeft(&toleft);
    mTo->GetTop(&totop);
    Int32 x = (Int32)(fromLeft + Elastos::Core::Math::Round(((toleft - fromLeft) *
            motionPercent)));
    Int32 y = (Int32)(fromTop + Elastos::Core::Math::Round(((totop - fromTop) * motionPercent)));

    IView* dropView = IView::Probe(mHost->mDropView);
    Int32 sx;
    dropView->GetScrollX(&sx);
    Int32 sx2;
    Int32 xPos = x - sx + (mHost->mAnchorView != NULL
            ? (mHost->mAnchorViewInitialScrollX - (mHost->mAnchorView->GetScrollX(&sx2), sx2)) : 0);
    Int32 sy;
    dropView->GetScrollY(&sy);
    Int32 yPos = y - sy;

    dropView->SetTranslationX(xPos);
    dropView->SetTranslationY(yPos);
    dropView->SetScaleX(scaleX);
    dropView->SetScaleY(scaleY);
    return dropView->SetAlpha(alpha);
}

DragLayer::AnimateViewAnimatorListenerAdapter::AnimateViewAnimatorListenerAdapter(
    /* [in] */ IRunnable* onCompleteRunnable,
    /* [in] */ Int32 animationEndStyle,
    /* [in] */ DragLayer* host)
    : mOnCompleteRunnable(onCompleteRunnable)
    , mAnimationEndStyle(animationEndStyle)
    , mHost(host)
{
}

ECode DragLayer::AnimateViewAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mOnCompleteRunnable != NULL) {
        mOnCompleteRunnable->Run();
    }
    switch (mAnimationEndStyle) {
        case ANIMATION_END_DISAPPEAR:
            mHost->ClearAnimatedView();
            break;
        case ANIMATION_END_FADE_OUT:
            mHost->FadeOutDragView();
            break;
        case ANIMATION_END_REMAIN_VISIBLE:
            break;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(DragLayer::FadeOutAnimatorUpdateListener, Object,
        IAnimatorUpdateListener);

DragLayer::FadeOutAnimatorUpdateListener::FadeOutAnimatorUpdateListener(
    /* [in] */ DragLayer* host)
    : mHost(host)
{
}

ECode DragLayer::FadeOutAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> fValue = IFloat::Probe(obj);
    Float percent;
    fValue->GetValue(&percent);

    Float alpha = 1 - percent;
    return IView::Probe(mHost->mDropView)->SetAlpha(alpha);
}

DragLayer::FadeOutAnimatorListenerAdapter::FadeOutAnimatorListenerAdapter(
    /* [in] */ DragLayer* host)
    : mHost(host)
{
}

ECode DragLayer::FadeOutAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mDropView != NULL) {
        mHost->mDragController->OnDeferredEndDrag(mHost->mDropView);
        mHost->mDropView = NULL;
    }
    return mHost->Invalidate();
}

DragLayer::MyRunnable::MyRunnable(
    /* [in] */ IView* child,
    /* [in] */ IRunnable* onFinishAnimationRunnable)
    : mChild(child)
    , mOnFinishAnimationRunnable(onFinishAnimationRunnable)
{
}

ECode DragLayer::MyRunnable::Run()
{
    mChild->SetVisibility(VISIBLE);
    if (mOnFinishAnimationRunnable != NULL) {
        mOnFinishAnimationRunnable->Run();
    }
    return NOERROR;
}


CAR_INTERFACE_IMPL(DragLayer::HierarchyChangeListener, Object, IViewGroupOnHierarchyChangeListener);

DragLayer::HierarchyChangeListener::HierarchyChangeListener(
    /* [in] */ DragLayer* host)
    : mHost(host)
{
}

ECode DragLayer::HierarchyChangeListener::OnChildViewAdded(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    return mHost->OnChildViewAdded(parent, child);
}

ECode DragLayer::HierarchyChangeListener::OnChildViewRemoved(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    return mHost->OnChildViewRemoved(parent, child);
}


CAR_INTERFACE_IMPL_2(DragLayer, FrameLayout, IDragLayer, IViewGroupOnHierarchyChangeListenerHolder);

DragLayer::DragLayer()
{
    mTmpXY = ArrayOf<Int32>::Alloc(2);
    mXDown = 0;
    mYDown = 0;
    CArrayList::New((IArrayList**)&mResizeFrames);
    CDecelerateInterpolator::New(1.5f, (ITimeInterpolator**)&mCubicEaseOutInterpolator);
    mAnchorViewInitialScrollX = 0;
    mHoverPointClosesFolder = FALSE;
    CRect::New((IRect**)&mHitRect);
    mWorkspaceIndex = -1;
    mQsbIndex = -1;
    mInScrollArea = FALSE;
}

ECode DragLayer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);

    mHierarchyChangeListener = new HierarchyChangeListener(this);

    // Disable multitouch across the workspace/all apps/customize tray
    SetMotionEventSplittingEnabled(FALSE);
    SetChildrenDrawingOrderEnabled(TRUE);
    SetOnHierarchyChangeListener(mHierarchyChangeListener);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetDrawable(
            Elastos::Droid::Launcher2::R::drawable::page_hover_left_holo,
            (IDrawable**)&mLeftHoverDrawable);

    return resources->GetDrawable(
            Elastos::Droid::Launcher2::R::drawable::page_hover_right_holo,
            (IDrawable**)&mRightHoverDrawable);
}

ECode DragLayer::GetViewGroupHierarchyChangeListener(
    /* [out] */ IViewGroupOnHierarchyChangeListener** listener)
{
    VALIDATE_NOT_NULL(listener)
    *listener = mHierarchyChangeListener;
    REFCOUNT_ADD(*listener)
    return NOERROR;
}

ECode DragLayer::Setup(
    /* [in] */ ILauncher* launcher,
    /* [in] */ IDragController* controller)
{
    mLauncher = launcher;
    mDragController = controller;
    return NOERROR;
}

ECode DragLayer::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    mDragController->DispatchKeyEvent(event, &res);
    Boolean res2;
    FrameLayout::DispatchKeyEvent(event, &res2);
    *result = res || res2;
    return NOERROR;
}

Boolean DragLayer::IsEventOverFolderTextRegion(
    /* [in] */ IFolder* folder,
    /* [in] */ IMotionEvent* ev)
{
    AutoPtr<IView> view;
    folder->GetEditTextRegion((IView**)&view);
    Float tmp;
    GetDescendantRectRelativeToSelf(view, mHitRect, &tmp);
    Float x;
    ev->GetX(&x);
    Float y;
    ev->GetY(&y);
    Boolean res;
    mHitRect->Contains((Int32)x, (Int32)y, &res);
    if (res) {
        return TRUE;
    }
    return FALSE;
}

Boolean DragLayer::IsEventOverFolder(
    /* [in] */ IFolder* folder,
    /* [in] */ IMotionEvent* ev)
{
    Float tmp;
    GetDescendantRectRelativeToSelf(IView::Probe(folder), mHitRect, &tmp);
    Float x;
    ev->GetX(&x);
    Float y;
    ev->GetY(&y);
    Boolean res;
    mHitRect->Contains((Int32)x, (Int32)y, &res);
    if (res) {
        return TRUE;
    }
    return FALSE;
}

Boolean DragLayer::HandleTouchDown(
    /* [in] */ IMotionEvent* ev,
    /* [in] */ Boolean intercept)
{
    AutoPtr<IRect> hitRect;
    CRect::New((IRect**)&hitRect);
    Float _x;
    ev->GetX(&_x);
    Int32 x = (Int32)_x;
    Float _y;
    ev->GetY(&_y);
    Int32 y = (Int32)_y;

    Int32 size;
    mResizeFrames->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mResizeFrames->Get(i, (IInterface**)&obj);
        AutoPtr<AppWidgetResizeFrame> child = (AppWidgetResizeFrame*)IObject::Probe(obj);

        child->GetHitRect(hitRect);
        Boolean res;
        hitRect->Contains(x, y, &res);
        if (res) {
            Int32 left;
            child->GetLeft(&left);
            Int32 top;
            child->GetTop(&top);
            Boolean tmp;
            if (child->BeginResizeIfPointInRegion(x - left, y - top, &tmp), tmp) {
                mCurrentResizeFrame = child;
                mXDown = x;
                mYDown = y;
                RequestDisallowInterceptTouchEvent(TRUE);
                return TRUE;
            }
        }
    }

    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    AutoPtr<IFolder> currentFolder;
    workspace->GetOpenFolder((IFolder**)&currentFolder);
    if (currentFolder != NULL) {
        Boolean res;
        mLauncher->IsFolderClingVisible(&res);
        if (!res  && intercept) {
            Boolean res2;
            currentFolder->IsEditingName(&res2);
            if (res2) {
                if (!IsEventOverFolderTextRegion(currentFolder, ev)) {
                    currentFolder->DismissEditingName();
                    return TRUE;
                }
            }
            Float tmp;
            GetDescendantRectRelativeToSelf(IView::Probe(currentFolder), hitRect, &tmp);
            if (!IsEventOverFolder(currentFolder, ev)) {
                mLauncher->CloseFolder();
                return TRUE;
            }
        }
    }
    return FALSE;
}

ECode DragLayer::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        if (HandleTouchDown(ev, TRUE)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    ClearAllResizeFrames();
    return mDragController->OnInterceptTouchEvent(ev, result);
}

ECode DragLayer::OnInterceptHoverEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mLauncher == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    if (workspace == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IFolder> currentFolder;
    workspace->GetOpenFolder((IFolder**)&currentFolder);
    if (currentFolder == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
        AutoPtr<IAccessibilityManager> accessibilityManager = IAccessibilityManager::Probe(obj);

        Boolean res;
        accessibilityManager->IsTouchExplorationEnabled(&res);
        if (res) {
            Int32 action;
            ev->GetAction(&action);
            Boolean isOverFolder;
            switch (action) {
                case IMotionEvent::ACTION_HOVER_ENTER:
                {
                    isOverFolder = IsEventOverFolder(currentFolder, ev);
                    if (!isOverFolder) {
                        Boolean tmp;
                        currentFolder->IsEditingName(&tmp);
                        SendTapOutsideFolderAccessibilityEvent(tmp);
                        mHoverPointClosesFolder = TRUE;
                        *result = TRUE;
                        return NOERROR;
                    }
                    else if (isOverFolder) {
                        mHoverPointClosesFolder = FALSE;
                    }
                    else {
                        *result = TRUE;
                        return NOERROR;
                    }
                }
                case IMotionEvent::ACTION_HOVER_MOVE:
                {
                    isOverFolder = IsEventOverFolder(currentFolder, ev);
                    if (!isOverFolder && !mHoverPointClosesFolder) {
                        Boolean tmp;
                        currentFolder->IsEditingName(&tmp);
                        SendTapOutsideFolderAccessibilityEvent(tmp);
                        mHoverPointClosesFolder = TRUE;
                        *result = TRUE;
                        return NOERROR;
                    }
                    else if (isOverFolder) {
                        mHoverPointClosesFolder = FALSE;
                    }
                    else {
                        *result = TRUE;
                        return NOERROR;
                    }
                }
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

void DragLayer::SendTapOutsideFolderAccessibilityEvent(
    /* [in] */ Boolean isEditingName)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IAccessibilityManager> accessibilityManager = IAccessibilityManager::Probe(obj);

    Boolean res;
    accessibilityManager->IsEnabled(&res);
    if (res) {
        Int32 stringId = isEditingName ?
                Elastos::Droid::Launcher2::R::string::folder_tap_to_rename :
                Elastos::Droid::Launcher2::R::string::folder_tap_to_close;


        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        AutoPtr<IAccessibilityEvent> event;
        helper->Obtain(IAccessibilityEvent::TYPE_VIEW_FOCUSED, (IAccessibilityEvent**)&event);
        OnInitializeAccessibilityEvent(event);

        AutoPtr<IContext> _context;
        GetContext((IContext**)&_context);
        String str;
        _context->GetString(stringId, &str);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        AutoPtr<IList> list;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
        list->Add(TO_IINTERFACE(cchar));
        accessibilityManager->SendAccessibilityEvent(event);
    }
}

ECode DragLayer::OnRequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    AutoPtr<IFolder> currentFolder;
    workspace->GetOpenFolder((IFolder**)&currentFolder);
    if (currentFolder != NULL) {
        if (TO_IINTERFACE(child) == TO_IINTERFACE(currentFolder)) {
            return FrameLayout::OnRequestSendAccessibilityEvent(child, event, result);
        }
        // Skip propagating onRequestSendAccessibilityEvent all for other children
        // when a folder is open
        *result = FALSE;
        return NOERROR;
    }
    return FrameLayout::OnRequestSendAccessibilityEvent(child, event, result);
}

ECode DragLayer::AddChildrenForAccessibility(
    /* [in] */ IArrayList* childrenForAccessibility)
{
    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    AutoPtr<IFolder> currentFolder;
    workspace->GetOpenFolder((IFolder**)&currentFolder);
    if (currentFolder != NULL) {
        // Only add the folder as a child for accessibility when it is open
        return childrenForAccessibility->Add(TO_IINTERFACE(currentFolder));
    }
    else {
        return FrameLayout::AddChildrenForAccessibility(childrenForAccessibility);
    }
    return NOERROR;
}

ECode DragLayer::OnHoverEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // If we've received this, we've already done the necessary handling
    // in onInterceptHoverEvent. Return true to consume the event.
    *result = FALSE;
    return NOERROR;
}

ECode DragLayer::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean handled = FALSE;
    Int32 action;
    ev->GetAction(&action);

    Float _x;
    ev->GetX(&_x);
    Int32 x = (Int32)_x;
    Float _y;
    ev->GetY(&_y);
    Int32 y = (Int32)_y;

    if (action == IMotionEvent::ACTION_DOWN) {
        if (action == IMotionEvent::ACTION_DOWN) {
            if (HandleTouchDown(ev, FALSE)) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }

    if (mCurrentResizeFrame != NULL) {
        handled = TRUE;
        switch (action) {
            case IMotionEvent::ACTION_MOVE:
                mCurrentResizeFrame->VisualizeResizeForDelta(x - mXDown, y - mYDown);
                break;
            case IMotionEvent::ACTION_CANCEL:
            case IMotionEvent::ACTION_UP:
                mCurrentResizeFrame->VisualizeResizeForDelta(x - mXDown, y - mYDown);
                mCurrentResizeFrame->OnTouchUp();
                mCurrentResizeFrame = NULL;
        }
    }
    if (handled) {
        *result = TRUE;
        return NOERROR;
    }
    return mDragController->OnTouchEvent(ev, result);
}

ECode DragLayer::GetDescendantRectRelativeToSelf(
    /* [in] */ IView* descendant,
    /* [in] */ IRect* r,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    (*mTmpXY)[0] = 0;
    (*mTmpXY)[1] = 0;
    Float scale;
    GetDescendantCoordRelativeToSelf(descendant, mTmpXY, &scale);

    Int32 width;
    descendant->GetWidth(&width);
    Int32 height;
    descendant->GetHeight(&height);
    r->Set((*mTmpXY)[0], (*mTmpXY)[1],
            (*mTmpXY)[0] + width, (*mTmpXY)[1] + height);
    *result = scale;
    return NOERROR;
}

ECode DragLayer::GetLocationInDragLayer(
    /* [in] */ IView* child,
    /* [in] */ ArrayOf<Int32>* loc,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    (*loc)[0] = 0;
    (*loc)[1] = 0;
    return GetDescendantCoordRelativeToSelf(child, loc, result);
}

ECode DragLayer::GetDescendantCoordRelativeToSelf(
    /* [in] */ IView* descendant,
    /* [in] */ ArrayOf<Int32>* coord,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    Float scale = 1.0f;
    AutoPtr<ArrayOf<Float> > pt = ArrayOf<Float>::Alloc(2);
    (*pt)[0] = (*coord)[0];
    (*pt)[1] = (*coord)[1];
    AutoPtr<IMatrix> matrix;
    descendant->GetMatrix((IMatrix**)&matrix);
    matrix->MapPoints(pt);

    Float x;
    descendant->GetScaleX(&x);
    scale *= x;
    Int32 left;
    descendant->GetLeft(&left);
    (*pt)[0] += left;
    Int32 top;
    descendant->GetTop(&top);
    (*pt)[1] += top;
    AutoPtr<IViewParent> viewParent;
    descendant->GetParent((IViewParent**)&viewParent);
    while (IView::Probe(viewParent) != NULL && TO_IINTERFACE(viewParent) != TO_IINTERFACE(this)) {
        AutoPtr<IView> view = IView::Probe(viewParent);
        AutoPtr<IMatrix> _matrix;
        view->GetMatrix((IMatrix**)&_matrix);
        _matrix->MapPoints(pt);

        Float _x;
        view->GetScaleX(&_x);
        scale *= _x;
        Int32 _left;
        view->GetLeft(&_left);
        Int32 sx;
        view->GetScrollX(&sx);
        (*pt)[0] += _left - sx;
        Int32 top;
        view->GetTop(&top);
        Int32 sy;
        view->GetScrollY(&sy);
        (*pt)[1] += top - sy;
        view->GetParent((IViewParent**)&viewParent);
    }
    (*coord)[0] = (Int32)Elastos::Core::Math::Round((*pt)[0]);
    (*coord)[1] = (Int32)Elastos::Core::Math::Round((*pt)[1]);
    *result = scale;
    return NOERROR;
}

ECode DragLayer::GetViewRectRelativeToSelf(
    /* [in] */ IView* v,
    /* [in] */ IRect* r)
{
    AutoPtr<ArrayOf<Int32> > loc = ArrayOf<Int32>::Alloc(2);
    GetLocationInWindow(loc);
    Int32 x = (*loc)[0];
    Int32 y = (*loc)[1];

    v->GetLocationInWindow(loc);
    Int32 vX = (*loc)[0];
    Int32 vY = (*loc)[1];

    Int32 left = vX - x;
    Int32 top = vY - y;
    Int32 width;
    v->GetMeasuredWidth(&width);
    Int32 height;
    v->GetMeasuredHeight(&height);
    return r->Set(left, top, left + width, top + height);
}

ECode DragLayer::DispatchUnhandledMove(
    /* [in] */ IView* focused,
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return mDragController->DispatchUnhandledMove(focused, direction, result);
}

ECode DragLayer::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    FrameLayout::OnLayout(changed, l, t, r, b);
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> param;
        child->GetLayoutParams((IViewGroupLayoutParams**)&param);
        if (IDragLayerLayoutParams::Probe(param) != NULL) {
            DragLayerLayoutParams* lp = (DragLayerLayoutParams*)IDragLayerLayoutParams::Probe(param);
            if (lp->mCustomPosition) {
                child->Layout(lp->mX, lp->mY, lp->mX + lp->mWidth, lp->mY + lp->mHeight);
            }
        }
    }
    return NOERROR;
}

ECode DragLayer::ClearAllResizeFrames()
{
    Int32 size;
    mResizeFrames->GetSize(&size);
    if (size > 0) {
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mResizeFrames->Get(i, (IInterface**)&obj);
            AutoPtr<IAppWidgetResizeFrame> frame = IAppWidgetResizeFrame::Probe(obj);

            frame->CommitResize();
            RemoveView(IView::Probe(frame));
        }
        mResizeFrames->Clear();
    }
    return NOERROR;
}

ECode DragLayer::HasResizeFrames(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 size;
    mResizeFrames->GetSize(&size);
    *result = size > 0;
    return NOERROR;
}

ECode DragLayer::IsWidgetBeingResized(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCurrentResizeFrame != NULL;
    return NOERROR;
}

ECode DragLayer::AddResizeFrame(
    /* [in] */ IItemInfo* itemInfo,
    /* [in] */ ILauncherAppWidgetHostView* widget,
    /* [in] */ ICellLayout* cellLayout)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<AppWidgetResizeFrame> resizeFrame = new AppWidgetResizeFrame();
    resizeFrame->constructor(context, widget, cellLayout, this);

    AutoPtr<CDragLayerLayoutParams> lp;
    CDragLayerLayoutParams::NewByFriend(-1, -1, (CDragLayerLayoutParams**)&lp);
    lp->mCustomPosition = TRUE;

    AddView(resizeFrame, lp);
    mResizeFrames->Add(TO_IINTERFACE(resizeFrame));

    return resizeFrame->SnapToWidget(FALSE);
}

ECode DragLayer::AnimateViewIntoPosition(
    /* [in] */ IDragView* dragView,
    /* [in] */ IView* child)
{
    return AnimateViewIntoPosition(dragView, child, NULL);
}

ECode DragLayer::AnimateViewIntoPosition(
    /* [in] */ IDragView* dragView,
    /* [in] */ ArrayOf<Int32>* pos,
    /* [in] */ Float alpha,
    /* [in] */ Float scaleX,
    /* [in] */ Float scaleY,
    /* [in] */ Int32 animationEndStyle,
    /* [in] */ IRunnable* onFinishRunnable,
    /* [in] */ Int32 duration)
{
    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    GetViewRectRelativeToSelf(IView::Probe(dragView), r);
    Int32 fromX;
    r->GetLeft(&fromX);
    Int32 fromY;
    r->GetTop(&fromY);

    return AnimateViewIntoPosition(dragView, fromX, fromY, (*pos)[0],
            (*pos)[1], alpha, 1, 1, scaleX, scaleY,
            onFinishRunnable, animationEndStyle, duration, NULL);
}

ECode DragLayer::AnimateViewIntoPosition(
    /* [in] */ IDragView* dragView,
    /* [in] */ IView* child,
    /* [in] */ IRunnable* onFinishAnimationRunnable)
{
    return AnimateViewIntoPosition(dragView, child, -1,
            onFinishAnimationRunnable, NULL);
}

ECode DragLayer::AnimateViewIntoPosition(
    /* [in] */ IDragView* dragView,
    /* [in] */ IView* child,
    /* [in] */ Int32 duration,
    /* [in] */ IRunnable* onFinishAnimationRunnable,
    /* [in] */ IView* anchorView)
{
    AutoPtr<IViewParent> p;
    child->GetParent((IViewParent**)&p);
    AutoPtr<IShortcutAndWidgetContainer> parentChildren = IShortcutAndWidgetContainer::Probe(p);

    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    AutoPtr<CellLayout::CellLayoutLayoutParams> lp =
            (CellLayout::CellLayoutLayoutParams*)ICellLayoutLayoutParams::Probe(params);
    parentChildren->MeasureChild(child);

    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    GetViewRectRelativeToSelf(IView::Probe(dragView), r);

    AutoPtr<ArrayOf<Int32> > coord = ArrayOf<Int32>::Alloc(2);
    Float childScale;
    child->GetScaleX(&childScale);
    Int32 x = lp->GetX();
    Int32 width;
    child->GetMeasuredWidth(&width);
    (*coord)[0] = x + (Int32)(width * (1 - childScale) / 2);
    Int32 y = lp->GetY();
    Int32 height;
    child->GetMeasuredHeight(&height);
    (*coord)[1] = y + (Int32)(height * (1 - childScale) / 2);

    // Since the child hasn't necessarily been laid out, we force the lp to be updated with
    // the correct coordinates (above) and use these to determine the final location
    AutoPtr<IViewParent> p2;
    child->GetParent((IViewParent**)&p2);
    Float scale;
    GetDescendantCoordRelativeToSelf(IView::Probe(p2), coord, &scale);
    // We need to account for the scale of the child itself, as the above only accounts for
    // for the scale in parents.
    scale *= childScale;
    Int32 toX = (*coord)[0];
    Int32 toY = (*coord)[1];
    if (ITextView::Probe(child) != NULL) {
        AutoPtr<ITextView> tv = ITextView::Probe(child);

        // The child may be scaled (always about the center of the view) so to account for it,
        // we have to offset the position by the scaled size.  Once we do that, we can center
        // the drag view about the scaled child view.
        Int32 ptop;
        IView::Probe(tv)->GetPaddingTop(&ptop);
        toY += Elastos::Core::Math::Round(scale * ptop);
        Int32 mheight;
        IView::Probe(dragView)->GetMeasuredHeight(&mheight);
        toY -= mheight * (1 - scale) / 2;
        Int32 mwidth;
        IView::Probe(dragView)->GetMeasuredWidth(&mwidth);
        Int32 mwidth2;
        child->GetMeasuredWidth(&mwidth2);
        toX -= (mwidth - Elastos::Core::Math::Round(scale * mwidth2)) / 2;
    }
    else if (IFolderIcon::Probe(child) != NULL) {
        // Account for holographic blur padding on the drag view
        toY -= scale * IWorkspace::DRAG_BITMAP_PADDING / 2;
        Int32 mheight;
        IView::Probe(dragView)->GetMeasuredHeight(&mheight);
        toY -= (1 - scale) * mheight / 2;
        // Center in the x coordinate about the target's drawable
        Int32 mwidth;
        IView::Probe(dragView)->GetMeasuredWidth(&mwidth);
        Int32 mwidth2;
        child->GetMeasuredWidth(&mwidth2);
        toX -= (mwidth - Elastos::Core::Math::Round(scale * mwidth2)) / 2;
    } else {
        Int32 height;
        IView::Probe(dragView)->GetHeight(&height);
        Int32 mheigth;
        child->GetMeasuredHeight(&mheigth);
        toY -= (Elastos::Core::Math::Round(scale * (height - mheigth))) / 2;
        Int32 mwidth;
        IView::Probe(dragView)->GetMeasuredWidth(&mwidth);
        Int32 mwidth2;
        child->GetMeasuredWidth(&mwidth2);
        toX -= (Elastos::Core::Math::Round(scale * (mwidth - mwidth2))) / 2;
    }

    Int32 fromX;
    r->GetLeft(&fromX);
    Int32 fromY;
    r->GetTop(&fromY);
    child->SetVisibility(INVISIBLE);
    AutoPtr<IRunnable> onCompleteRunnable = new MyRunnable(child, onFinishAnimationRunnable);
    return AnimateViewIntoPosition(dragView, fromX, fromY, toX, toY, 1, 1, 1, scale, scale,
            onCompleteRunnable, ANIMATION_END_DISAPPEAR, duration, anchorView);
}

ECode DragLayer::AnimateViewIntoPosition(
    /* [in] */ IDragView* view,
    /* [in] */ Int32 fromX,
    /* [in] */ Int32 fromY,
    /* [in] */ Int32 toX,
    /* [in] */ Int32 toY,
    /* [in] */ Float finalAlpha,
    /* [in] */ Float initScaleX,
    /* [in] */ Float initScaleY,
    /* [in] */ Float finalScaleX,
    /* [in] */ Float finalScaleY,
    /* [in] */ IRunnable* onCompleteRunnable,
    /* [in] */ Int32 animationEndStyle,
    /* [in] */ Int32 duration,
    /* [in] */ IView* anchorView)
{
    Int32 width;
    IView::Probe(view)->GetMeasuredWidth(&width);
    Int32 height;
    IView::Probe(view)->GetMeasuredHeight(&height);
    AutoPtr<IRect> from;
    CRect::New(fromX, fromY, fromX + width, fromY + height, (IRect**)&from);

    AutoPtr<IRect> to;
    CRect::New(toX, toY, toX + width, toY + height, (IRect**)&to);
    return AnimateView(view, from, to, finalAlpha, initScaleX, initScaleY,
            finalScaleX, finalScaleY, duration, NULL, NULL, onCompleteRunnable,
            animationEndStyle, anchorView);
}

ECode DragLayer::AnimateView(
    /* [in] */ IDragView* view,
    /* [in] */ IRect* from,
    /* [in] */ IRect* to,
    /* [in] */ Float finalAlpha,
    /* [in] */ Float initScaleX,
    /* [in] */ Float initScaleY,
    /* [in] */ Float finalScaleX,
    /* [in] */ Float finalScaleY,
    /* [in] */ Int32 duration,
    /* [in] */ IInterpolator* motionInterpolator,
    /* [in] */ IInterpolator* alphaInterpolator,
    /* [in] */ IRunnable* onCompleteRunnable,
    /* [in] */ Int32 animationEndStyle,
    /* [in] */ IView* anchorView)
{
    // Calculate the duration of the animation based on the object's distance
    Int32 toleft;
    to->GetLeft(&toleft);
    Int32 fromleft;
    from->GetLeft(&fromleft);
    Int32 totop;
    to->GetTop(&totop);
    Int32 fromtop;
    from->GetTop(&fromtop);
    Float dist = (Float)Elastos::Core::Math::Sqrt(Elastos::Core::Math::Pow(toleft - fromleft, 2) +
            Elastos::Core::Math::Pow(totop - fromtop, 2));
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 value;
    res->GetInteger(Elastos::Droid::Launcher2::R::integer::config_dropAnimMaxDist,
            &value);
    Float maxDist = (Float)value;

    // If duration < 0, this is a cue to compute the duration based on the distance
    if (duration < 0) {
        res->GetInteger(
                Elastos::Droid::Launcher2::R::integer::config_dropAnimMaxDuration,
                &duration);
        if (dist < maxDist) {
            Float polation;
            mCubicEaseOutInterpolator->GetInterpolation(dist / maxDist, &polation);
            duration *= polation;
        }
        Int32 _duration;
        res->GetInteger(Elastos::Droid::Launcher2::R::integer::config_dropAnimMinDuration,
                &_duration);
        duration = Elastos::Core::Math::Max(duration, _duration);
    }

    // Fall back to cubic ease out interpolator for the animation if none is specified
    AutoPtr<ITimeInterpolator> interpolator;
    if (alphaInterpolator == NULL || motionInterpolator == NULL) {
        interpolator = mCubicEaseOutInterpolator;
    }

    // Animate the view
    Float initAlpha;
    IView::Probe(view)->GetAlpha(&initAlpha);
    Float dropViewScale;
    IView::Probe(view)->GetScaleX(&dropViewScale);

    AutoPtr<IAnimatorUpdateListener> updateCb = new AnimateViewAnimatorUpdateListener(view,
            alphaInterpolator, motionInterpolator, initScaleX, initScaleY, dropViewScale,
            finalScaleX, finalScaleY, finalAlpha, initAlpha,
            from, to, this);
    return AnimateView(view, updateCb, duration, interpolator, onCompleteRunnable,
            animationEndStyle, anchorView);
}

ECode DragLayer::AnimateView(
    /* [in] */ IDragView* view,
    /* [in] */ IAnimatorUpdateListener* updateCb,
    /* [in] */ Int32 duration,
    /* [in] */ ITimeInterpolator* interpolator,
    /* [in] */ IRunnable* onCompleteRunnable,
    /* [in] */ Int32 animationEndStyle,
    /* [in] */ IView* anchorView)
{
    // Clean up the previous animations
    if (mDropAnim != NULL) IAnimator::Probe(mDropAnim)->Cancel();
    if (mFadeOutAnim != NULL) IAnimator::Probe(mFadeOutAnim)->Cancel();

    // Show the drop view if it was previously hidden
    mDropView = view;
    mDropView->CancelAnimation();
    mDropView->ResetLayoutParams();

    // Set the anchor view if the page is scrolling
    if (anchorView != NULL) {
        anchorView->GetScrollX(&mAnchorViewInitialScrollX);
    }
    mAnchorView = anchorView;

    // Create and start the animation
    mDropAnim = NULL;
    CValueAnimator::New((IValueAnimator**)&mDropAnim);
    IAnimator::Probe(mDropAnim)->SetInterpolator(interpolator);
    mDropAnim->SetDuration(duration);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 0.0f;
    (*array)[1] = 1.0f;
    mDropAnim->SetFloatValues(array);
    mDropAnim->AddUpdateListener(updateCb);
    AutoPtr<IAnimatorListener> lisener = new AnimateViewAnimatorListenerAdapter(
            onCompleteRunnable, animationEndStyle, this);
    IAnimator::Probe(mDropAnim)->AddListener(lisener);
    return IAnimator::Probe(mDropAnim)->Start();
}

ECode DragLayer::ClearAnimatedView()
{
    if (mDropAnim != NULL) {
        IAnimator::Probe(mDropAnim)->Cancel();
    }
    if (mDropView != NULL) {
        mDragController->OnDeferredEndDrag(mDropView);
        mDropView = NULL;
    }
    return Invalidate();
}

ECode DragLayer::GetAnimatedView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = IView::Probe(mDropView);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

void DragLayer::FadeOutDragView()
{
    mFadeOutAnim = NULL;
    CValueAnimator::New((IValueAnimator**)&mFadeOutAnim);
    mFadeOutAnim->SetDuration(150);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 0.0f;
    (*array)[1] = 1.0f;
    mFadeOutAnim->SetFloatValues(array);
    mFadeOutAnim->RemoveAllUpdateListeners();
    AutoPtr<IAnimatorUpdateListener> lisener = new FadeOutAnimatorUpdateListener(this);
    mFadeOutAnim->AddUpdateListener(lisener);
    AutoPtr<IAnimatorListener> adapter = new FadeOutAnimatorListenerAdapter(this);
    IAnimator::Probe(mFadeOutAnim)->AddListener(adapter);
    IAnimator::Probe(mFadeOutAnim)->Start();
}

ECode DragLayer::OnChildViewAdded(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    UpdateChildIndices();
    return NOERROR;
}

ECode DragLayer::OnChildViewRemoved(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    UpdateChildIndices();
    return NOERROR;
}

void DragLayer::UpdateChildIndices()
{
    if (mLauncher != NULL) {
        AutoPtr<IWorkspace> workspace;
        mLauncher->GetWorkspace((IWorkspace**)&workspace);
        IndexOfChild(IView::Probe(workspace), &mWorkspaceIndex);
        AutoPtr<ISearchDropTargetBar> bar;
        IndexOfChild(IView::Probe(bar), &mQsbIndex);
    }
}

ECode DragLayer::GetChildDrawingOrder(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 i,
    /* [out] */ Int32* order)
{
    VALIDATE_NOT_NULL(order);

    // TODO: We have turned off this custom drawing order because it now effects touch
    // dispatch order. We need to sort that issue out and then decide how to go about this.
    // if (TRUE || LauncherApplication.isScreenLandscape(getContext()) ||
    //         mWorkspaceIndex == -1 || mQsbIndex == -1 ||
    //         mLauncher.getWorkspace().isDrawingBackgroundGradient()) {
    //     return i;
    // }
    *order = i;
    return NOERROR;

    // This ensures that the workspace is drawn above the hotseat and qsb,
    // except when the workspace is drawing a background gradient, in which
    // case we want the workspace to stay behind these elements.
    if (i == mQsbIndex) {
        *order = mWorkspaceIndex;
        return NOERROR;
    }
    else if (i == mWorkspaceIndex) {
        *order = mQsbIndex;
        return NOERROR;
    }
    else {
        *order = i;
        return NOERROR;
    }
}

ECode DragLayer::OnEnterScrollArea(
    /* [in] */ Int32 direction)
{
    mInScrollArea = TRUE;
    return Invalidate();
}

ECode DragLayer::OnExitScrollArea()
{
    mInScrollArea = FALSE;
    return Invalidate();
}

Boolean DragLayer::IsLayoutDirectionRtl()
{
    Int32 direction;
    GetLayoutDirection(&direction);
    return (direction == LAYOUT_DIRECTION_RTL);
}

ECode DragLayer::DispatchDraw(
        /* [in] */ ICanvas* canvas)
{
    FrameLayout::DispatchDraw(canvas);

    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    if (mInScrollArea && !res) {
        AutoPtr<IWorkspace> workspace;
        mLauncher->GetWorkspace((IWorkspace**)&workspace);
        Int32 width;
        IView::Probe(workspace)->GetWidth(&width);
        AutoPtr<IRect> childRect;
        CRect::New((IRect**)&childRect);
        AutoPtr<IView> view;
        IViewGroup::Probe(workspace)->GetChildAt(0, (IView**)&view);
        Float value;
        GetDescendantRectRelativeToSelf(view, childRect, &value);

        Int32 page;
        IPagedView::Probe(workspace)->GetNextPage(&page);
        Boolean isRtl = IsLayoutDirectionRtl();
        AutoPtr<IView> tmp;
        IViewGroup::Probe(workspace)->GetChildAt(isRtl ? page + 1 : page - 1, (IView**)&tmp);
        AutoPtr<ICellLayout> leftPage = ICellLayout::Probe(tmp);
        AutoPtr<IView> tmp2;
        IViewGroup::Probe(workspace)->GetChildAt(isRtl ? page - 1 : page + 1, (IView**)&tmp2);
        AutoPtr<ICellLayout> rightPage = ICellLayout::Probe(tmp2);

        if (leftPage != NULL) {
            Boolean res;
            leftPage->GetIsDragOverlapping(&res);
            if (res) {
                Int32 top;
                childRect->GetTop(&top);
                Int32 width;
                mLeftHoverDrawable->GetIntrinsicWidth(&width);
                Int32 bottom;
                childRect->GetBottom(&bottom);
                mLeftHoverDrawable->SetBounds(0, top, width, bottom);
                mLeftHoverDrawable->Draw(canvas);
            }
        }
        else if (rightPage != NULL) {
            Boolean res;
            rightPage->GetIsDragOverlapping(&res);
            if (res) {
                Int32 _width;
                mRightHoverDrawable->GetIntrinsicWidth(&_width);
                Int32 top;
                childRect->GetTop(&top);
                Int32 bottom;
                childRect->GetBottom(&bottom);
                mRightHoverDrawable->SetBounds(width - _width, top, width, bottom);
                mRightHoverDrawable->Draw(canvas);
            }
        }
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos