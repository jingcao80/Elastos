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

#include "elastos/droid/internal/view/BaseIWindow.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::IIWindow;
using Elastos::Droid::View::EIID_IIWindow;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
CAR_INTERFACE_IMPL_3(BaseIWindow, Object, IBaseIWindow, IIWindow, IBinder)

ECode BaseIWindow::SetSession(
    /* [in] */ IWindowSession* session)
{
    mSession = session;
    return NOERROR;
}

ECode BaseIWindow::Resized(
    /* [in] */ IRect* frame,
    /* [in] */ IRect* overscanInsets,
    /* [in] */ IRect* contentInsets,
    /* [in] */ IRect* visibleInsets,
    /* [in] */ IRect* stableInsets,
    /* [in] */ Boolean reportDraw,
    /* [in] */ IConfiguration* newConfig)
{
    if (reportDraw) {
        mSession->FinishDrawing(this);
    }

    return NOERROR;
}

ECode BaseIWindow::Moved(
    /* [in] */ Int32 newX,
    /* [in] */ Int32 newY)
{
    return NOERROR;
}

ECode BaseIWindow::DispatchAppVisibility(
    /* [in] */ Boolean visible)
{
    return NOERROR;
}

ECode BaseIWindow::DispatchGetNewSurface()
{
    return NOERROR;
}

ECode BaseIWindow::WindowFocusChanged(
    /* [in] */ Boolean hasFocus,
    /* [in] */ Boolean touchEnabled)
{
    return NOERROR;
}

ECode BaseIWindow::ExecuteCommand(
    /* [in] */ const String& command,
    /* [in] */ const String& parameters,
    /* [in] */ IParcelFileDescriptor* out)
{
    return NOERROR;
}

ECode BaseIWindow::CloseSystemDialogs(
    /* [in] */ const String& reason)
{
    return NOERROR;
}

ECode BaseIWindow::DispatchWallpaperOffsets(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float xStep,
    /* [in] */ Float yStep,
    /* [in] */ Boolean sync)
{
    if (sync) {
        mSession->WallpaperOffsetsComplete(this);
    }
    return NOERROR;
}

ECode BaseIWindow::DispatchDragEvent(
    /* [in] */ IDragEvent* event)
{
    return NOERROR;
}

ECode BaseIWindow::DispatchSystemUiVisibilityChanged(
    /* [in] */ Int32 seq,
    /* [in] */ Int32 globalUi,
    /* [in] */ Int32 localValue,
    /* [in] */ Int32 localChanges)
{
    mSeq = seq;
    return NOERROR;
}

ECode BaseIWindow::DispatchWallpaperCommand(
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean sync)
{
    if (sync) {
        mSession->WallpaperCommandComplete(this, NULL);
    }
    return NOERROR;
}

ECode BaseIWindow::DoneAnimating()
{
    return NOERROR;
}

ECode BaseIWindow::SetSeq(
    /* [in] */ Int32 seq)
{
    mSeq = seq;
    return NOERROR;
}

ECode BaseIWindow::GetSeq(
    /* [out] */ Int32* seq)
{
    VALIDATE_NOT_NULL(seq)
    *seq = mSeq;
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
