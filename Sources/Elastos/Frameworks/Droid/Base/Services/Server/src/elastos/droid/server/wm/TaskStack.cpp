
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Utility.h>
#include "elastos/droid/server/wm/TaskStack.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringUtils;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const Int32 TaskStack::DEFAULT_DIM_DURATION;

TaskStack::TaskStack(
    /* [in] */ CWindowManagerService* service,
    /* [in] */ Int32 stackId)
    : mStackId(stackId)
    , mDimmingTag(FALSE)
    , mDeferDetach(FALSE)
    , mService(service)
    , mFullscreen(TRUE)
{
    CRect::New((IRect**)&mTmpRect);
    CRect::New((IRect**)&mBounds);
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

    List<AutoPtr<WindowState> > resizingWindows = mService->mResizingWindows;
    List<AutoPtr<Task> >::ReverseIterator rit = mTasks.RBegin();
    for (; rit != mTasks.REnd(); ++rit) {
        AppTokenList activities = (*rit)->mAppTokens;
        AppTokenList::ReverseIterator activityRit = activities.RBegin();
        for (; activityRit != activities.REnd(); ++activityRit) {
            List<AutoPtr<WindowState> > windows = (*activityRit)->mAllAppWindows;
            List<AutoPtr<WindowState> >::ReverseIterator winRit = windows.RBegin();
            for (; winRit != windows.REnd(); ++winRit) {
                AutoPtr<WindowState> win = *winRit;
                List<AutoPtr<WindowState> >::Iterator it = Find(resizingWindows.Begin(), resizingWindows.End(), win);
                if (it == resizingWindows.End()) {
                    if (CWindowManagerService::DEBUG_RESIZE)
                        Slogger::D(CWindowManagerService::TAG, "setBounds: Resizing %p", win.Get());
                    resizingWindows.PushBack(win);
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
    List<AutoPtr<Task> >::ReverseIterator taskRit = mTasks.RBegin();
    for (; taskRit != mTasks.REnd(); ++taskRit) {
        AppTokenList activities = (*taskRit)->mAppTokens;
        AppTokenList::ReverseIterator activityRit = activities.RBegin();
        for (; activityRit != activities.REnd(); ++activityRit) {
            List<AutoPtr<WindowState> > windows = (*activityRit)->mAllAppWindows;
            List<AutoPtr<WindowState> >::ReverseIterator winRit = windows.RBegin();
            for (; winRit != windows.REnd(); ++winRit) {
                AutoPtr<WindowStateAnimator> winAnimator = (*winRit)->mWinAnimator;
                if (winAnimator->IsAnimating() || winAnimator->mWin->mExiting) {
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
        List<AutoPtr<Task> >::ReverseIterator taskRit = mTasks.RBegin();
        if (!mService->IsCurrentProfileLocked(task->mUserId)) {
            // Place the task below all current user tasks.
            while (taskRit != mTasks.REnd()) {
                if (!mService->IsCurrentProfileLocked((*taskRit)->mUserId)) {
                    break;
                }
                ++taskRit;
            }
            // Put it above first non-current user task.
            --taskRit;
        }
        it = (--(taskRit.GetBase()));
    }
    if (CWindowManagerService::DEBUG_TASK_MOVEMENT)
        Slogger::D(CWindowManagerService::TAG, "addTask: task=%p toTop=%d pos=", task, toTop /*, stackNdx*/);
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
        if (mTasks.Begin() == mTasks.End()) {
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
    UpdateDisplayInfo();
    return NOERROR;
}

void TaskStack::DetachDisplay()
{
    // EventLog.writeEvent(EventLogTags.WM_STACK_REMOVED, mStackId);

    Boolean doAnotherLayoutPass = FALSE;
    List<AutoPtr<Task> >::ReverseIterator taskRit = mTasks.RBegin();
    for (; taskRit != mTasks.REnd(); ++taskRit) {
        AppTokenList appWindowTokens = (*taskRit)->mAppTokens;
        AppTokenList::ReverseIterator tokenRit = appWindowTokens.RBegin();
        for (; tokenRit != appWindowTokens.REnd(); ++tokenRit) {
            List<AutoPtr<WindowState> > appWindows = (*tokenRit)->mAllAppWindows;
            List<AutoPtr<WindowState> >::ReverseIterator winRit = appWindows.RBegin();
            for (; winRit != appWindows.REnd(); ++winRit) {
                mService->RemoveWindowInnerLocked(NULL, *winRit);
                doAnotherLayoutPass = TRUE;
            }
        }
    }
    if (doAnotherLayoutPass) {
        mService->RequestTraversalLocked();
    }

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
    Float dimAmount;
    if (mDimWinAnimator == NULL) {
        dimLayer = mDimLayer->GetLayer();
        dimAmount = 0;
    }
    else {
        dimLayer = mDimWinAnimator->mAnimLayer - CWindowManagerService::LAYER_OFFSET_DIM;
        mDimWinAnimator->mWin->mAttrs->GetDimAmount(&dimAmount);
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
    mDimLayer->mDimSurface->Destroy();
    mAnimationBackgroundSurface->mDimSurface->Destroy();
}


ECode TaskStack::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("{stackId=") + StringUtils::ToString(mStackId) + String("tasks=")/* + mTasks */;
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
