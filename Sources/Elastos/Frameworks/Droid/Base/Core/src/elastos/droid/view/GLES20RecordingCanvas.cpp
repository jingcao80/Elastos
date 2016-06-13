
#include "elastos/droid/view/GLES20RecordingCanvas.h"

namespace Elastos {
namespace Droid {
namespace View {


static Pools::SynchronizedPool<GLES20RecordingCanvas>* InitPoll()
{
    Pools::SynchronizedPool<GLES20RecordingCanvas>* pool;
    pool = new Pools::SynchronizedPool<GLES20RecordingCanvas>(GLES20RecordingCanvas::POOL_LIMIT);
    return pool;
}

Pools::SynchronizedPool<GLES20RecordingCanvas>* GLES20RecordingCanvas::sPool = InitPoll();

CAR_INTERFACE_IMPL(GLES20RecordingCanvas, GLES20Canvas, IGLES20RecordingCanvas)

AutoPtr<IHardwareCanvas> GLES20RecordingCanvas::Obtain(
    /* [in] */ IRenderNode* node)
{
    assert(node != NULL);
    AutoPtr<GLES20RecordingCanvas> canvas = sPool->AcquireItem();
    if (canvas == NULL) {
        canvas = new GLES20RecordingCanvas();
        canvas->constructor();
    }
    canvas->mNode = node;
    return canvas.Get();
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
    /* [in] */ IObject* o,
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

