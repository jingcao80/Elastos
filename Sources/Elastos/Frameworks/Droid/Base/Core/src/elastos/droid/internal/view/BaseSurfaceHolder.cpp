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

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/internal/view/BaseSurfaceHolder.h"
#include "elastos/droid/view/CSurface.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::EIID_ISurfaceHolder;
using Elastos::Core::Thread;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

const Boolean BaseSurfaceHolder::DEBUG = FALSE;
const String BaseSurfaceHolder::TAG("BaseSurfaceHolder");

CAR_INTERFACE_IMPL(BaseSurfaceHolder, Object, ISurfaceHolder);

BaseSurfaceHolder::BaseSurfaceHolder() :
    mHaveGottenCallbacks(FALSE),
    mRequestedWidth(-1),
    mRequestedHeight(-1),
    mRequestedFormat(IPixelFormat::OPAQUE),
    mRequestedType(-1),
    mLastLockTime(0),
    mType(-1)
{
    ASSERT_SUCCEEDED(CArrayList::New((IArrayList**)&mCallbacks));
    ASSERT_SUCCEEDED(CReentrantLock::New((ILock**)&mSurfaceLock));
    ASSERT_SUCCEEDED(CSurface::New((ISurface**)&mSurface));
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mSurfaceFrame));
}

Int32 BaseSurfaceHolder::GetRequestedWidth()
{
    return mRequestedWidth;
}

Int32 BaseSurfaceHolder::GetRequestedHeight()
{
    return mRequestedHeight;
}

Int32 BaseSurfaceHolder::GetRequestedFormat()
{
    return mRequestedFormat;
}

Int32 BaseSurfaceHolder::GetRequestedType()
{
    return mRequestedType;
}

ECode BaseSurfaceHolder::AddCallback(
    /* [in] */ ISurfaceHolderCallback* cback)
{
    AutoLock lock(mCallbacks);

    // This is a linear search, but in practice we'll
    // have only a couple callbacks, so it doesn't matter.
    Boolean contains = FALSE;
    if (mCallbacks->Contains(cback, &contains), contains) {
        mCallbacks->Add(cback);
    }

    return NOERROR;
}

ECode BaseSurfaceHolder::RemoveCallback(
    /* [in] */ ISurfaceHolderCallback* cback)
{
    AutoLock lock(mCallbacks);
    return mCallbacks->Remove(cback);
}

AutoPtr<ArrayOf<ISurfaceHolderCallback*> > BaseSurfaceHolder::GetCallbacks()
{
    if (mHaveGottenCallbacks) {
        return mGottenCallbacks;
    }

    AutoLock lock(mCallbacks);

    Int32 N;
    mCallbacks->GetSize(&N);
    mGottenCallbacks = NULL;
    if (N > 0) {
        mCallbacks->ToArray((ArrayOf<IInterface*>**)&mGottenCallbacks);
    }
    mHaveGottenCallbacks = TRUE;

    return mGottenCallbacks;
}

void BaseSurfaceHolder::UngetCallbacks()
{
    mHaveGottenCallbacks = FALSE;
}

ECode BaseSurfaceHolder::SetFixedSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (mRequestedWidth != width || mRequestedHeight != height) {
        mRequestedWidth = width;
        mRequestedHeight = height;
        OnRelayoutContainer();
    }

    return NOERROR;
}

ECode BaseSurfaceHolder::SetSizeFromLayout()
{
    if (mRequestedWidth != -1 || mRequestedHeight != -1) {
        mRequestedWidth = mRequestedHeight = -1;
        OnRelayoutContainer();
    }

    return NOERROR;
}

ECode BaseSurfaceHolder::SetFormat(
    /* [in] */ Int32 format)
{
    if (mRequestedFormat != format) {
        mRequestedFormat = format;
        OnUpdateSurface();
    }

    return NOERROR;
}

ECode BaseSurfaceHolder::SetType(
    /* [in] */ Int32 type)
{
    switch (type) {
        case SURFACE_TYPE_HARDWARE:
        case SURFACE_TYPE_GPU:
            // these are deprecated, treat as "NORMAL"
            type = SURFACE_TYPE_NORMAL;
            break;
        default:
            break;
    }

    switch (type) {
        case SURFACE_TYPE_NORMAL:
        case SURFACE_TYPE_PUSH_BUFFERS:
            if (mRequestedType != type) {
                mRequestedType = type;
                OnUpdateSurface();
            }
            break;
        default:
            break;
    }

    return NOERROR;
}

ECode BaseSurfaceHolder::LockCanvas(
    /* [out] */ ICanvas** canvas)
{
    return InternalLockCanvas(NULL, canvas);
}

ECode BaseSurfaceHolder::LockCanvas(
    /* [in] */ IRect* dirty,
    /* [out] */ ICanvas** canvas)
{
    return InternalLockCanvas(dirty, canvas);
}

ECode BaseSurfaceHolder::InternalLockCanvas(
    /* [in] */ IRect* dirty,
    /* [out] */ ICanvas** canvas)
{
    VALIDATE_NOT_NULL(canvas);

    if (mType == SURFACE_TYPE_PUSH_BUFFERS) {
        //throw new BadSurfaceTypeException(
        //    "Surface type is SURFACE_TYPE_PUSH_BUFFERS");
        Logger::E(TAG, "Surface type is SURFACE_TYPE_PUSH_BUFFERS");
        return E_BAD_SURFACE_TYPE_EXCEPTION;
    }

    mSurfaceLock->Lock();

    if (DEBUG) {
        Logger::D(TAG, "Locking canvas..,");
    }

    if (OnAllowLockCanvas()) {
        if (dirty == NULL) {
            if (mTmpDirty == NULL) {
                CRect::New((IRect**)&mTmpDirty);
            }

            mTmpDirty->Set(mSurfaceFrame);
            dirty = mTmpDirty;
        }

        ECode ec = mSurface->LockCanvas(dirty, canvas);
        if (FAILED(ec)) {
            Logger::E(TAG, "Exception locking surface");
        }
    }

    if (DEBUG) {
        String str;
        IObject::Probe(*canvas)->ToString(&str);
        Logger::D(TAG, "Returned canvas: %s", str.string());
    }

    if (*canvas != NULL) {
        mLastLockTime = SystemClock::GetUptimeMillis();
        return NOERROR;
    }

    // If the Surface is not ready to be drawn, then return NULL,
    // but throttle calls to this function so it isn't called more
    // than every 100ms.
    Int64 now = SystemClock::GetUptimeMillis();
    Int64 nextTime = mLastLockTime + 100;
    if (nextTime > now) {
        Thread::Sleep(nextTime-now);
        now = SystemClock::GetUptimeMillis();
    }
    mLastLockTime = now;
    mSurfaceLock->UnLock();

    *canvas = NULL;
    return NOERROR;
}

ECode BaseSurfaceHolder::UnlockCanvasAndPost(
    /* [in] */ ICanvas* canvas)
{
    mSurface->UnlockCanvasAndPost(canvas);
    mSurfaceLock->UnLock();

    return NOERROR;
}

ECode BaseSurfaceHolder::GetSurface(
    /* [out] */ ISurface** surface)
{
    VALIDATE_NOT_NULL(surface);
    *surface = mSurface;
    REFCOUNT_ADD(*surface);

    return NOERROR;
}

ECode BaseSurfaceHolder::GetSurfaceFrame(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    *rect = mSurfaceFrame;
    REFCOUNT_ADD(*rect);

    return NOERROR;
}

ECode BaseSurfaceHolder::SetSurfaceFrameSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return mSurfaceFrame->Set(0, 0, width, height);
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
