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

#include "elastos/droid/media/CMediaScannerHelper.h"
#include "elastos/droid/media/CMediaScanner.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CMediaScannerHelper)

CAR_INTERFACE_IMPL(CMediaScannerHelper, Singleton, IMediaScannerHelper)

/*static*/
ECode CMediaScannerHelper::IsNoMediaPath(
    /* [in] */ const String& path,
    /* [out] */ Boolean* result)
{
    return CMediaScanner::IsNoMediaPath(path, result);
}

ECode CMediaScannerHelper::ClearMediaPathCache(
    /* [in] */ Boolean clearMediaPaths,
    /* [in] */ Boolean clearNoMediaPaths)
{
    return CMediaScanner::ClearMediaPathCache(clearMediaPaths, clearNoMediaPaths);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
