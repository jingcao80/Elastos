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

#include "elastos/droid/webkit/webview/chromium/native/base/CpuFeatures.h"
#include "elastos/droid/webkit/webview/chromium/native/base/api/CpuFeatures_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * Return the number of CPU Cores on the device.
 */
Int32 CpuFeatures::GetCount()
{
    return NativeGetCoreCount();
}

/**
 * Return the CPU feature mask.
 * This is a 64-bit integer that corresponds to the CPU's features.
 * The value comes directly from android_getCpuFeatures().
 */
Int64 CpuFeatures::GetMask()
{
    return NativeGetCpuFeatures();
}

Int32 CpuFeatures::NativeGetCoreCount()
{
    return Elastos_CpuFeatures_nativeGetCoreCount();
}

Int64 CpuFeatures::NativeGetCpuFeatures()
{
    return Elastos_CpuFeatures_nativeGetCpuFeatures();
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
