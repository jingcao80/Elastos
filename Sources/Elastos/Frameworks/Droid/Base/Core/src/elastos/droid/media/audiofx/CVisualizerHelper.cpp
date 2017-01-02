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

#include "elastos/droid/media/audiofx/CVisualizerHelper.h"
#include "elastos/droid/media/audiofx/CVisualizer.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CVisualizerHelper, Singleton, IVisualizerHelper)

CAR_SINGLETON_IMPL(CVisualizerHelper)

/**
 * Returns the capture size range.
 * @return the mininum capture size is returned in first array element and the maximum in second
 * array element.
 */
ECode CVisualizerHelper::GetCaptureSizeRange(
    /* [out, callee] */ ArrayOf<Int32>** captureSizeRange)
{
    return CVisualizer::GetCaptureSizeRange(captureSizeRange);
}

/**
 * Returns the maximum capture rate for the callback capture method. This is the maximum value
 * for the rate parameter of the
 * {@link #setDataCaptureListener(OnDataCaptureListener, int, boolean, boolean)} method.
 * @return the maximum capture rate expressed in milliHertz
 */
ECode CVisualizerHelper::GetMaxCaptureRate(
    /* [out] */ Int32* captureRate)
{
    return CVisualizer::GetMaxCaptureRate(captureRate);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
