
#include <elastos/droid/ext/frameworkdef.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Utility.h>
#include "elastos/droid/server/wm/TaskStack.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/server/wm/BlurLayer.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const Int32 TaskStack::DEFAULT_DIM_DURATION;
const Int32 TaskStack::DEFAULT_BLUR_DURATION;
const Float TaskStack::MAX_BLUR_AMOUNT;

TaskStack::TaskStack(
    /* [in] */ CWindowManagerService* service,
    /* [in] */ Int32 stackId)
    : mStackId(stackId)
    , mDimmingTag(FALSE)
    , mBlurringTag(FALSE)
    , mDeferDetach(FALSE)
    , mService(service)
    , mFullscreen(TRUE)
{
    CRect::New((IRect**)&mTmpRect);
    CRect::New((IRect**)&mBounds);
    CArrayList::New((IArrayList**)&mExitingAppTokens);
}

AutoPtr<DisplayContent> TaskStack::GetDisplayContent()
{
    return mDisplayContent;
}

List< AutoPtr<Task> >& TaskStack::GetTasks()
{
    return mTasks;
}

void TaskStack::ResizeWindows()
{
    Int32 top;
    mBounds->GetTop(&top);
    Boolean underStatusBar = top == 0;

    AutoPtr<WindowList> resizingWindows = mService->mResizingWindows;
    List<AutoPtr<Task> >::ReverseIterator rit = mTasks.RBegin();
    for (; rit != mTasks.REnd(); ++rit) {
        AutoPtr<IArrayList> activities = (*rit)->mAppTokens;
        Int32 size;
        activities->GetSize(&size);
        for (Int32 activityNdx = size - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<IInterface> value;
            activities->Get(activityNdx, (IInterface**)&value);
            AutoPtr<WindowList> windows = ((AppWindowToken*)(IObject*)value.Get())->mAllAppWindows;
            Int32 N;
            windows->GetSize(&N);
            for (Int32 winNdx = N - 1; winNdx >= 0; --winNdx) {
                AutoPtr<IInterface> obj;
                windows->Get(winNdx, (IInterface**)&obj);
                AutoPtr<WindowState> win = To_WindowState(obj);
                Boolean result;
                if (resizingWindows->Contains((IWindowState*)win.Get(), &result), !result) {
                    if (CWindowManagerService::DEBUG_RESIZE)
                        Slogger::D(CWindowManagerService::TAG, "setBounds: Resizing %s", TO_CSTR(win));
                    resizingWindows->Add((IWindowState*)win.Get());
                }
                win->mUnderStatusBar = underStatusBar;
            }
        }
    }
}

Boolean TaskStack::SetBounds(
    /* [in] */ IRect* bounds)
{
    Boolean oldFullscreen = mFullscreen;
    if (mDisplayContent != NULL) {
        mDisplayContent->GetLogicalDisplayRect(mTmpRect);
        IObject::Probe(mTmpRect)->Equals(bounds, &mFullscreen);
    }

    Boolean equals;
    if ((IObject::Probe(mBounds)->Equals(bounds, &equals), equals) && oldFullscreen == mFullscreen) {
        return FALSE;
    }

    mDimLayer->SetBounds(bounds);
    mAnimationBackgroundSurface->SetBounds(bounds);
    mBlurLayer->SetBounds(bounds);
    mBounds->Set(bounds);

    return TRUE;
}

void TaskStack::GetBounds(
    /* [in] */ IRect* out)
{
    out->Set(mBounds);
}

void TaskStack::UpdateDisplayInfo()
{
    if (mFullscreen && mDisplayContent != NULL) {
        mDisplayContent->GetLogicalDisplayRect(mTmpRect);
        SetBounds(mTmpRect);
    }
}

Boolean TaskStack::IsFullscreen()
{
    return mFullscreen;
}

Boolean TaskStack::IsAnimating()
{
    AutoPtr<WindowState> windowState;
    List<AutoPtr<Task> >::ReverseIterator taskRit = mTasks.RBegin();
    for (; taskRit != mTasks.REnd(); ++taskRit) {
        AutoPtr<IArrayList> activities = (*taskRit)->mAppTokens;
        Int32 size;
        activities->GetSize(&size);
        for (Int32 activityNdx = size - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<IInterface> value;
            activities->Get(activityNdx, (IInterface**)&value);
            AutoPtr<WindowList> windows = ((AppWindowToken*)(IObject*)value.Get())->mAllAppWindows;
            Int32 N;
            windows->GetSize(&N);
            for (Int32 winNdx = N - 1; winNdx >= 0; --winNdx) {
                AutoPtr<IInterface> obj;
                windows->Get(winNdx, (IInterface**)&obj);
                AutoPtr<WindowStateAnimator> winAnimator = To_WindowState(obj)->mWinAnimator;
                if (winAnimator->IsAnimating() || (
                    (windowState = winAnimator->GetWindowState(), windowState != NULL)
                    && windowState->mExiting)) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

void TaskStack::AddTask(
    /* [in] */ Task* task,
    /* [in] */ Boolean toTop)
{
    List<AutoPtr<Task> >::Iterator it;
    if (!toTop) {
        it = mTasks.Begin();
    }
    else {
        it = mTasks.End();
        if (!mService->IsCurrentProfileLocked(task->mUserId)) {
            // Place the task below all current user tasks.
            while (--it != mTasks.End()) {
                if (!mService->IsCurrentProfileLocked((*it)->mUserId)) {
                    break;
                }
            }
            // Put it above first non-current user task.
            ++it;
        }
    }
    if (CWindowManagerService::DEBUG_TASK_MOVEMENT) {
        Slogger::D(CWindowManagerService::TAG, "addTask: task=%p toTop=%d pos=", task, toTop /*, stackNdx*/);
    }
    mTasks.Insert(it, task);

    task->mStack = this;
    mDisplayContent->MoveStack(this, TRUE);
    // EventLog.writeEvent(EventLogTags.WM_TASK_MOVED, task.taskId, toTop ? 1 : 0, stackNdx);
}

void TaskStack::MoveTaskToTop(
    /* [in] */ Task* task)
{
    if (CWindowManagerService::DEBUG_TASK_MOVEMENT)
        Slogger::D(CWindowManagerService::TAG, "moveTaskToTop: task=%p Callers=", task/*, Debug.getCallers(6)*/);
    mTasks.Remove(task);
    AddTask(task, TRUE);
}

void TaskStack::MoveTaskToBottom(
    /* [in] */ Task* task)
{
    if (CWindowManagerService::DEBUG_TASK_MOVEMENT) Slogger::D(CWindowManagerService::TAG, "moveTaskToBottom: task=%p", task);
    mTasks.Remove(task);
    AddTask(task, FALSE);
}

void TaskStack::RemoveTask(
    /* [in] */ Task* task)
{
    if (CWindowManagerService::DEBUG_TASK_MOVEMENT) Slogger::D(CWindowManagerService::TAG, "removeTask: task=%p", task);
    mTasks.Remove(task);
    if (mDisplayContent != NULL) {
        if (mTasks.IsEmpty()) {
            mDisplayContent->MoveStack(this, FALSE);
        }
        mDisplayContent->mLayoutNeeded = TRUE;
    }
}

ECode TaskStack::AttachDisplayContent(
    /* [in] */ DisplayContent* displayContent)
{
    if (mDisplayContent != NULL) {
        Slogger::E(CWindowManagerService::TAG, "attachDisplayContent: Already attached");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mDisplayContent = displayContent;
    mDimLayer = new DimLayer(mService, this, displayContent);
    mAnimationBackgroundSurface = new DimLayer(mService, this, displayContent);
    mBlurLayer = new BlurLayer(mService, this, displayContent);
    UpdateDisplayInfo();
    return NOERROR;
}

void TaskStack::DetachDisplay()
{
    // EventLog.writeEvent(EventLogTags.WM_STACK_REMOVED, mStackId);

    Boolean doAnotherLayoutPass = FALSE;
    List<AutoPtr<Task> >::ReverseIterator taskRit = mTasks.RBegin();
    for (; taskRit != mTasks.REnd(); ++taskRit) {
        AutoPtr<IArrayList> appWindowTokens = (*taskRit)->mAppTokens;
        Int32 size;
        appWindowTokens->GetSize(&size);
        for (Int32 appNdx = size - 1; appNdx >= 0; --appNdx) {
            AutoPtr<IInterface> value;
            appWindowTokens->Get(appNdx, (IInterface**)&value);
            AutoPtr<WindowList> appWindows = ((AppWindowToken*)(IObject*)value.Get())->mAllAppWindows;
            Int32 N;
            appWindows->GetSize(&N);
            for (Int32 winNdx = N -1; winNdx >= 0; --winNdx) {
                AutoPtr<IInterface> obj;
                appWindows->Get(winNdx, (IInterface**)&obj);
                mService->RemoveWindowInnerLocked(NULL, To_WindowState(obj));
                doAnotherLayoutPass = TRUE;
            }
        }
    }
    if (doAnotherLayoutPass) {
        mService->RequestTraversalLocked();
    }

    mBlurLayer->DestroySurface();
    mBlurLayer = NULL;
    mAnimationBackgroundSurface->DestroySurface();
    mAnimationBackgroundSurface = NULL;
    mDimLayer->DestroySurface();
    mDimLayer = NULL;
    mDisplayContent = NULL;
}

void TaskStack::ResetAnimationBackgroundAnimator()
{
    mAnimationBackgroundAnimator = NULL;
    mAnimationBackgroundSurface->Hide();
}

Int64 TaskStack::GetBlurBehindFadeDuration(
    /* [in] */ Int64 duration)
{
    return GetDimBehindFadeDuration(duration);
}

Int64 TaskStack::GetDimBehindFadeDuration(
    /* [in] */ Int64 duration)
{
    AutoPtr<ITypedValue> tv;
    CTypedValue::New((ITypedValue**)&tv);
    AutoPtr<IResources> res;
    mService->mContext->GetResources((IResources**)&res);
    res->GetValue(R::fraction::config_dimBehindFadeDuration, tv, TRUE);
    Int32 type;
    tv->GetType(&type);
    if (type == ITypedValue::TYPE_FRACTION) {
        Float fraction;
        tv->GetFraction(duration, duration, &fraction);
        duration = (Int64)fraction;
    }
    else if (type >= ITypedValue::TYPE_FIRST_INT && type <= ITypedValue::TYPE_LAST_INT) {
        Int32 data;
        tv->GetData(&data);
        duration = (Int64)data;
    }
    return duration;
}

Boolean TaskStack::AnimateDimLayers()
{
    Int32 dimLayer;
    Float dimAmount = 0;
    if (mDimWinAnimator == NULL) {
        dimLayer = mDimLayer->GetLayer();
        dimAmount = 0;
    }
    else {
        dimLayer = mDimWinAnimator->mAnimLayer - CWindowManagerService::LAYER_OFFSET_DIM;
        AutoPtr<WindowState> windowState = mDimWinAnimator->GetWindowState();
        if (windowState != NULL) {
            windowState->mAttrs->GetDimAmount(&dimAmount);
        }
    }
    Float targetAlpha = mDimLayer->GetTargetAlpha();
    if (targetAlpha != dimAmount) {
        if (mDimWinAnimator == NULL) {
            mDimLayer->Hide(DEFAULT_DIM_DURATION);
        }
        else {
            Int64 duration = DEFAULT_DIM_DURATION;
            if (mDimWinAnimator->mAnimating && mDimWinAnimator->mAnimation != NULL) {
                mDimWinAnimator->mAnimation->ComputeDurationHint(&duration);
            }
            if (targetAlpha > dimAmount) {
                duration = GetDimBehindFadeDuration(duration);
            }
            mDimLayer->Show(dimLayer, dimAmount, duration);
        }
    }
    else if (mDimLayer->GetLayer() != dimLayer) {
        mDimLayer->SetLayer(dimLayer);
    }
    if (mDimLayer->IsAnimating()) {
        if (!mService->OkToDisplay()) {
            // Jump to the end of the animation.
            mDimLayer->Show();
        }
        else {
            return mDimLayer->StepAnimation();
        }
    }
    return FALSE;
}

void TaskStack::ResetDimmingTag()
{
    mDimmingTag = FALSE;
}

void TaskStack::SetDimmingTag()
{
    mDimmingTag = TRUE;
}

Boolean TaskStack::TestDimmingTag()
{
    return mDimmingTag;
}

Boolean TaskStack::IsDimming()
{
    return mDimLayer->IsDimming();
}

Boolean TaskStack::IsDimming(
    /* [in] */ WindowStateAnimator* winAnimator)
{
    return mDimWinAnimator.Get() == winAnimator && mDimLayer->IsDimming();
}

void TaskStack::StartDimmingIfNeeded(
    /* [in] */ WindowStateAnimator* newWinAnimator)
{
    // Only set dim params on the highest dimmed layer.
    AutoPtr<WindowStateAnimator> existingDimWinAnimator = mDimWinAnimator;
    // Don't turn on for an unshown surface, or for any layer but the highest dimmed layer.
    if (newWinAnimator->mSurfaceShown && (existingDimWinAnimator == NULL
            || !existingDimWinAnimator->mSurfaceShown
            || existingDimWinAnimator->mAnimLayer < newWinAnimator->mAnimLayer)) {
        mDimWinAnimator = newWinAnimator;
    }
}

void TaskStack::StopDimmingIfNeeded()
{
    if (!mDimmingTag && IsDimming()) {
        mDimWinAnimator = NULL;
    }
}

void TaskStack::SetAnimationBackground(
    /* [in] */ WindowStateAnimator* winAnimator,
    /* [in] */ Int32 color)
{
    Int32 animLayer = winAnimator->mAnimLayer;
    if (mAnimationBackgroundAnimator == NULL
            || animLayer < mAnimationBackgroundAnimator->mAnimLayer) {
        mAnimationBackgroundAnimator = winAnimator;
        animLayer = mService->AdjustAnimationBackground(winAnimator);
        mAnimationBackgroundSurface->Show(animLayer - CWindowManagerService::LAYER_OFFSET_DIM,
                ((color >> 24) & 0xff) / 255, 0);
    }
}

Boolean TaskStack::AnimateBlurLayers()
{
    Boolean result = FALSE;
    Int32 blurLayer;
    Float blurAmount;
    if (mBlurWinAnimator == NULL) {
        blurLayer = mBlurLayer->GetLayer();
        blurAmount = 0;
    }
    else {
        blurLayer = mBlurWinAnimator->mAnimLayer - CWindowManagerService::LAYER_OFFSET_BLUR;
        blurAmount = MAX_BLUR_AMOUNT;
    }
    Float targetBlur = mBlurLayer->GetTargetBlur();
    if (targetBlur != blurAmount) {
        if (mBlurWinAnimator == NULL) {
            mBlurLayer->Hide(DEFAULT_BLUR_DURATION);
        }
        else {
            Int64 hint;
            Int64 duration = (mBlurWinAnimator->mAnimating && mBlurWinAnimator->mAnimation != NULL)
                    ? (mBlurWinAnimator->mAnimation->ComputeDurationHint(&hint), hint)
                    : DEFAULT_BLUR_DURATION;
            if (targetBlur > blurAmount) {
                duration = GetBlurBehindFadeDuration(duration);
            }
            if (duration > DEFAULT_BLUR_DURATION)
                duration = DEFAULT_BLUR_DURATION;
            mBlurLayer->Show(blurLayer, blurAmount, duration);
        }
    }
    else if (mBlurLayer->GetLayer() != blurLayer) {
        mBlurLayer->SetLayer(blurLayer);
    }
    if (mBlurLayer->IsAnimating()) {
        if (!mService->OkToDisplay()) {
            // Jump to the end of the animation.
            mBlurLayer->Show();
        }
        else {
            result = mBlurLayer->StepAnimation();
        }
    }
    return result;
}

void TaskStack::ResetBlurringTag()
{
    mBlurringTag = FALSE;
}

void TaskStack::SetBlurringTag()
{
    mBlurringTag = TRUE;
}

Boolean TaskStack::TestBlurringTag()
{
    return mBlurringTag;
}

Boolean TaskStack::IsBlurring()
{
    return mBlurLayer->IsBlurring();
}

Boolean TaskStack::IsBlurring(
    /* [in] */ WindowStateAnimator* winAnimator)
{
    return mBlurWinAnimator.Get() == winAnimator && mBlurLayer->IsBlurring();
}

void TaskStack::StopBlurringIfNeeded()
{
    if (!mBlurringTag && IsBlurring()) {
        mBlurWinAnimator = NULL;
    }
}

void TaskStack::StartBlurringIfNeeded(
    /* [in] */ WindowStateAnimator* newWinAnimator)
{
    AutoPtr<WindowStateAnimator> existingBlurWinAnimator = mBlurWinAnimator;
    // Don't turn on for an unshown surface, or for any layer but the highest blur layer.
    if (newWinAnimator->mSurfaceShown && (existingBlurWinAnimator == NULL
            || !existingBlurWinAnimator->mSurfaceShown
            || existingBlurWinAnimator->mAnimLayer < newWinAnimator->mAnimLayer)) {
        mBlurWinAnimator = newWinAnimator;
    }
}

void TaskStack::SwitchUser(
    /* [in] */ Int32 userId)
{
    Int32 top = mTasks.GetSize();
    List<AutoPtr<Task> >::Iterator it = mTasks.Begin();
    for (Int32 taskNdx = 0; taskNdx < top; ++taskNdx) {
        AutoPtr<Task> task = *it;
        if (mService->IsCurrentProfileLocked(task->mUserId)) {
            it = mTasks.Erase(it);
            mTasks.PushBack(task);
            --top;
            continue;
        }
        ++it;
    }
}

void TaskStack::Close()
{
    mBlurLayer->mBlurSurface->Destroy();
    mDimLayer->mDimSurface->Destroy();
    mAnimationBackgroundSurface->mDimSurface->Destroy();
}

ECode TaskStack::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("TaskStack{stackId=");
    sb += mStackId;
    sb += ", tasks size=";
    sb += mTasks.GetSize();
    *str = sb.ToString();
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
