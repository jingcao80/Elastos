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

#include "elastos/droid/view/GLES20RecordingCanvas.h"

namespace Elastos {
namespace Droid {
namespace View {


Pools::SynchronizedPool<GLES20RecordingCanvas>* GLES20RecordingCanvas::sPool =
        new Pools::SynchronizedPool<GLES20RecordingCanvas>(GLES20RecordingCanvas::POOL_LIMIT);

CAR_INTERFACE_IMPL(GLES20RecordingCanvas, GLES20Canvas, IGLES20RecordingCanvas)

AutoPtr<IHardwareCanvas> GLES20RecordingCanvas::Obtain(
    /* [in] */ IRenderNode* node,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    assert(node != NULL);
    AutoPtr<GLES20RecordingCanvas> canvas = sPool->AcquireItem();
    if (canvas == NULL) {
        canvas = new GLES20RecordingCanvas();
        canvas->constructor(node, width, height);
    }
    else {
        canvas->ResetDisplayListRenderer(node, width, height);
    }
    canvas->mNode = node;
    canvas->SetViewport(width, height);
    return canvas.Get();
}

ECode GLES20RecordingCanvas::constructor(
        /* [in] */ IRenderNode* node,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height)
{
    FAIL_RETURN(GLES20Canvas::constructor(node, width, height));
    mDensity = 0;
    return NOERROR;
}

ECode GLES20RecordingCanvas::Recycle()
{
    mNode = NULL;
    sPool->ReleaseItem(this);
    return NOERROR;
}

ECode GLES20RecordingCanvas::FinishRecording(
    /* [out] */ Int64* res)
{
    return nFinishRecording(mRenderer, res);
}

ECode GLES20RecordingCanvas::IsRecordingFor(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    AutoPtr<IRenderNode> n = IRenderNode::Probe(o);
    *res = n == mNode;
    return NOERROR;
}

GLES20RecordingCanvas::GLES20RecordingCanvas()
{}

} // namespace View
} // namespace Droid
} // namespace Elastos

