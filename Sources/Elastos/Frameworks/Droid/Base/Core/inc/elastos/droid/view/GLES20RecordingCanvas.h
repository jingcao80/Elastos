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
        /* [in] */ IInterface* o,
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