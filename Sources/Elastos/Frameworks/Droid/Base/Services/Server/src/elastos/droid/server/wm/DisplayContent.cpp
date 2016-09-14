
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/wm/StackTapPointerEventListener.h"
#include "elastos/droid/server/am/ActivityStackSupervisor.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Graphics::RegionOp_DIFFERENCE;
using Elastos::Droid::View::CDisplayInfo;
using Elastos::Droid::Server::Am::ActivityStackSupervisor;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

DisplayContent::DisplayContent(
    /* [in] */ IDisplay* display,
    /* [in] */ CWindowManagerService* service)
    : mInitialDisplayWidth(0)
    , mInitialDisplayHeight(0)
    , mInitialDisplayDensity(0)
    , mBaseDisplayWidth(0)
    , mBaseDisplayHeight(0)
    , mBaseDisplayDensity(0)
    , mLayoutNeeded(FALSE)
    , mPendingLayoutChanges(0)
    , mIsDefaultDisplay(FALSE)
    , mService(service)
    , mDeferredRemoval(FALSE)
    , mDisplay(display)
{
    CRect::New((IRect**)&mBaseDisplayRect);
    CRect::New((IRect**)&mContentRect);
    CArrayList::New((IArrayList**)&mExitingTokens);
    CRegion::New((IRegion**)&mTouchExcludeRegion);
    CRect::New((IRect**)&mTmpRect);

    CArrayList::New((WindowList**)&mWindows);
    CDisplayInfo::New((IDisplayInfo**)&mDisplayInfo);

    display->GetDisplayId(&mDisplayId);
    Boolean result;
    display->GetDisplayInfo(mDisplayInfo, &result);
    mIsDefaultDisplay = mDisplayId == IDisplay::DEFAULT_DISPLAY;

    CArrayList::New((IArrayList**)&mTmpTaskHistory);
    CArrayList::New((IArrayList**)&mStacks);
}

Int32 DisplayContent::GetDisplayId()
{
    return mDisplayId;
}

AutoPtr<WindowList> DisplayContent::GetWindowList()
{
    return mWindows;
}

AutoPtr<IDisplay> DisplayContent::GetDisplay()
{
    return mDisplay;
}

AutoPtr<IDisplayInfo> DisplayContent::GetDisplayInfo()
{
    return mDisplayInfo;
}

Boolean DisplayContent::HasAccess(
    /* [in] */ Int32 uid)
{
    Boolean result;
    mDisplay->HasAccess(uid, &result);
    return result;
}

Boolean DisplayContent::IsPrivate()
{
    Int32 flags;
    return (mDisplay->GetFlags(&flags), flags & IDisplay::FLAG_PRIVATE) != 0;
}

AutoPtr<IArrayList> DisplayContent::GetStacks()
{
    return mStacks;
}

AutoPtr<IArrayList> DisplayContent::GetTasks()
{
    mTmpTaskHistory->Clear();
    Int32 numStacks;
    mStacks->GetSize(&numStacks);
    for (Int32 stackNdx = 0; stackNdx < numStacks; ++stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        AutoPtr<IArrayList> tasks = To_TaskStack(obj)->GetTasks();
        mTmpTaskHistory->AddAll(Elastos::Utility::ICollection::Probe(tasks));
    }
    return mTmpTaskHistory;
}

AutoPtr<TaskStack> DisplayContent::GetHomeStack()
{
    if (mHomeStack == NULL && mDisplayId == IDisplay::DEFAULT_DISPLAY) {
        Slogger::E(CWindowManagerService::TAG, "getHomeStack: Returning null from this=%p", this);
    }
    return mHomeStack;
}

void DisplayContent::UpdateDisplayInfo()
{
    Boolean result;
    mDisplay->GetDisplayInfo(mDisplayInfo, &result);
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        To_TaskStack(obj)->UpdateDisplayInfo();
    }
}

void DisplayContent::GetLogicalDisplayRect(
    /* [in] */ IRect* out)
{
    // Uses same calculation as in LogicalDisplay#configureDisplayInTransactionLocked.
    Int32 orientation;
    mDisplayInfo->GetRotation(&orientation);
    Boolean rotated = (orientation == ISurface::ROTATION_90
            || orientation == ISurface::ROTATION_270);
    Int32 physWidth = rotated ? mBaseDisplayHeight : mBaseDisplayWidth;
    Int32 physHeight = rotated ? mBaseDisplayWidth : mBaseDisplayHeight;
    Int32 width;
    mDisplayInfo->GetLogicalWidth(&width);
    Int32 left = (physWidth - width) / 2;
    Int32 height;
    mDisplayInfo->GetLogicalHeight(&height);
    Int32 top = (physHeight - height) / 2;
    out->Set(left, top, left + width, top + height);
}

ECode DisplayContent::AttachStack(
    /* [in] */ TaskStack* stack)
{
    if (stack->mStackId == ActivityStackSupervisor::HOME_STACK_ID) {
        if (mHomeStack != NULL) {
            Slogger::E(CWindowManagerService::TAG, "attachStack: HOME_STACK_ID (0) not first.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mHomeStack = stack;
    }
    mStacks->Add((IObject*)stack);
    mLayoutNeeded = TRUE;
    return NOERROR;
}

void DisplayContent::MoveStack(
    /* [in] */ TaskStack* stack,
    /* [in] */ Boolean toTop)
{
    mStacks->Remove((IObject*)stack);
    Int32 size;
    mStacks->Add(toTop ? (mStacks->GetSize(&size), size) : 0, (IObject*)stack);
}

void DisplayContent::DetachStack(
    /* [in] */ TaskStack* stack)
{
    mStacks->Remove((IObject*)stack);
}

void DisplayContent::Resize(
    /* [in] */ IRect* contentRect)
{
    mContentRect->Set(contentRect);
}

Int32 DisplayContent::StackIdFromPoint(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        AutoPtr<TaskStack> stack = To_TaskStack(obj);
        stack->GetBounds(mTmpRect);
        Boolean contains;
        if (mTmpRect->Contains(x, y, &contains), contains) {
            return stack->mStackId;
        }
    }
    return -1;
}

void DisplayContent::SetTouchExcludeRegion(
    /* [in] */ TaskStack* focusedStack)
{
    //ActionsCode(authro:songzhining, comment: fix exit bug for enjoyphoto when screen rotation)
    //mTouchExcludeRegion.set(mBaseDisplayRect);
    Int32 width, height;
    mDisplayInfo->GetLogicalWidth(&width);
    mDisplayInfo->GetLogicalHeight(&height);
    Boolean result;
    mTouchExcludeRegion->Set(0, 0, width, height, &result);

    AutoPtr<WindowList> windows = GetWindowList();
    Int32 N;
    windows->GetSize(&N);
    for (Int32 stackNdx = N - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        windows->Get(stackNdx, (IInterface**)&obj);
        AutoPtr<WindowState> win = To_WindowState(obj);
        AutoPtr<TaskStack> stack = win->GetStack();
        Boolean isVisible;
        if ((win->IsVisibleLw(&isVisible), isVisible) && stack != NULL && stack.Get() != focusedStack) {
            mTmpRect->Set(win->mVisibleFrame);
            mTmpRect->Intersect(win->mVisibleInsets, &result);
            mTouchExcludeRegion->Op(mTmpRect, RegionOp_DIFFERENCE, &result);
        }
    }
    mTapDetector->SetTouchExcludeRegion(mTouchExcludeRegion);
}

void DisplayContent::SwitchUserStacks(
    /* [in] */ Int32 newUserId)
{
    AutoPtr<WindowList> windows = GetWindowList();
    Int32 N;
    windows->GetSize(&N);
    for (Int32 stackNdx = 0; stackNdx < N; stackNdx++) {
        AutoPtr<IInterface> obj;
        windows->Get(stackNdx, (IInterface**)&obj);
        AutoPtr<WindowState> win = To_WindowState(obj);
        if (win->IsHiddenFromUserLocked()) {
            if (CWindowManagerService::DEBUG_VISIBILITY) {
                Int32 type;
                win->mAttrs->GetType(&type);
                Slogger::W(CWindowManagerService::TAG, "user changing %d hiding %p, attrs=%d, belonging to %d"
                        , newUserId, win.Get(), type, win->mOwnerUid);
            }
            Boolean result;
            win->HideLw(FALSE, &result);
        }
    }

    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        To_TaskStack(obj)->SwitchUser(newUserId);
    }
}

void DisplayContent::ResetAnimationBackgroundAnimator()
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        To_TaskStack(obj)->ResetAnimationBackgroundAnimator();
    }
}

Boolean DisplayContent::AnimateDimLayers()
{
    Boolean result = FALSE;
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        result |= To_TaskStack(obj)->AnimateDimLayers();
    }
    return result;
}

void DisplayContent::ResetDimming()
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        To_TaskStack(obj)->ResetDimmingTag();
    }
}

Boolean DisplayContent::IsDimming()
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        if (To_TaskStack(obj)->IsDimming()) {
            return TRUE;
        }
    }
    return FALSE;
}

void DisplayContent::StopDimmingIfNeeded()
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        To_TaskStack(obj)->StopDimmingIfNeeded();
    }
}

Boolean DisplayContent::AnimateBlurLayers()
{
    Boolean result = FALSE;
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        result |= To_TaskStack(obj)->AnimateBlurLayers();
    }
    return result;
}

void DisplayContent::ResetBlurring()
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        To_TaskStack(obj)->ResetBlurringTag();
    }
}

Boolean DisplayContent::IsBlurring()
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        if (To_TaskStack(obj)->IsBlurring()) {
            return TRUE;
        }
    }
    return FALSE;
}

void DisplayContent::StopBlurringIfNeeded()
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        To_TaskStack(obj)->StopBlurringIfNeeded();
    }
}

void DisplayContent::Close()
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        To_TaskStack(obj)->Close();
    }
}

Boolean DisplayContent::IsAnimating()
{
    Int32 size;
    mStacks->GetSize(&size);
    for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        if (To_TaskStack(obj)->IsAnimating()) {
            return TRUE;
        }
    }
    return FALSE;
}

void DisplayContent::CheckForDeferredActions()
{
    Boolean animating = FALSE;
    Int32 SN;
    mStacks->GetSize(&SN);
    for (Int32 stackNdx = SN - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> obj;
        mStacks->Get(stackNdx, (IInterface**)&obj);
        AutoPtr<TaskStack> stack = To_TaskStack(obj);
        if (stack->IsAnimating()) {
            animating = TRUE;
        }
        else {
            if (stack->mDeferDetach) {
                mService->DetachStackLocked(this, stack);
            }
            AutoPtr<IArrayList> tasks = stack->GetTasks();
            Int32 numTasks;
            tasks->GetSize(&numTasks);
            for (Int32 taskNdx = numTasks - 1; taskNdx >= 0; --taskNdx) {
                AutoPtr<IInterface> t;
                tasks->Get(taskNdx, (IInterface**)&t);
                AutoPtr<Task> task = To_Task(t);
                AutoPtr<IArrayList> tokens = task->mAppTokens;
                Int32 size;
                tokens->GetSize(&size);
                for (Int32 tokenNdx = size - 1; tokenNdx >= 0; --tokenNdx) {
                    AutoPtr<IInterface> value;
                    tokens->Get(tokenNdx, (IInterface**)&value);
                    AutoPtr<AppWindowToken> wtoken = (AppWindowToken*)(IObject*)value.Get();
                    if (wtoken->mDeferRemoval) {
                        stack->mExitingAppTokens->Remove((IObject*)wtoken);
                        wtoken->mDeferRemoval = FALSE;
                        mService->RemoveAppFromTaskLocked(wtoken);
                    }
                }
                if (task->mDeferRemoval) {
                    task->mDeferRemoval = FALSE;
                    mService->RemoveTaskLocked(task);
                }
            }
        }
    }
    if (!animating && mDeferredRemoval) {
        mService->OnDisplayRemoved(mDisplayId);
    }
}

ECode DisplayContent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("DisplayContent{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " displayId=";
    sb += mDisplayId;
    sb += " info=";
    sb += Object::ToString(mDisplayInfo);
    sb += " stacks=";
    Int32 size;
    mStacks->GetSize(&size);
    sb += size;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
