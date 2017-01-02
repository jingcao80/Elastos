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

#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/common/SurfaceWrapper.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Common {

SurfaceWrapper::SurfaceWrapper(
    /* [in] */ ISurface* surface)
    : mSurface(surface)
{
}

AutoPtr<ISurface> SurfaceWrapper::GetSurface()
{
    return mSurface;
}

// //@Override
ECode SurfaceWrapper::WriteToParcel(
    /* [in] */ IParcel* out)
{
    // Ignore flags so that the Surface won't call release()
    return IParcelable::Probe(mSurface)->WriteToParcel(out);
}

ECode SurfaceWrapper::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return IParcelable::Probe(mSurface)->ReadFromParcel(source);
}

} // namespace Common
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
