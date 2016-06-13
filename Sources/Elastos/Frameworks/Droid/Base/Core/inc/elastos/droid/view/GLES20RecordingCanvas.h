#ifndef __ELASTOS_DROID_VIEW_GLES20RECORDINGCANVAS_H__
#define __ELASTOS_DROID_VIEW_GLES20RECORDINGCANVAS_H__

#include "elastos/droid/view/GLES20Canvas.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::Utility::Pools;

namespace Elastos {
namespace Droid {
namespace View {

class GLES20RecordingCanvas
    : public GLES20Canvas
    , public IGLES20RecordingCanvas
{
public:
    TO_STRING_IMPL("GLES20RecordingCanvas")

    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IHardwareCanvas>) Obtain(
        /* [in] */ IRenderNode* node);

    CARAPI Recycle();

    CARAPI FinishRecording(
        /* [out] */ Int64* res);

    CARAPI IsRecordingFor(
        /* [in] */ IObject* o,
        /* [out] */ Boolean* res);

    AutoPtr<IRenderNode> mNode;

    static const Int32 POOL_LIMIT = 25;

private:
    GLES20RecordingCanvas();

    static Pools::SynchronizedPool<GLES20RecordingCanvas>* sPool;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::View::GLES20RecordingCanvas, IInterface)

#endif