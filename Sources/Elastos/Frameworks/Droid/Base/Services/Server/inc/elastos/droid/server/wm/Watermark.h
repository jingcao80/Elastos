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

#ifndef __ELASTOS_DROID_SERVER_WM_WATERMARK_H__
#define __ELASTOS_DROID_SERVER_WM_WATERMARK_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::ISurfaceSession;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

/**
 * Displays a watermark on top of the window manager's windows.
 */
class Watermark : public Object
{
public:
    Watermark(
        /* [in] */ IDisplay* display,
        /* [in] */ IDisplayMetrics* dm,
        /* [in] */ ISurfaceSession* session,
        /* [in] */ ArrayOf<String>* tokens);

    CARAPI_(void) PositionSurface(
        /* [in] */ Int32 dw,
        /* [in] */ Int32 dh);

    CARAPI_(void) DrawIfNeeded();

private:
    AutoPtr<IDisplay> mDisplay;
    AutoPtr< ArrayOf<String> > mTokens;
    String mText;
    AutoPtr<IPaint> mTextPaint;
    Int32 mTextWidth;
    Int32 mTextHeight;
    Int32 mDeltaX;
    Int32 mDeltaY;

    AutoPtr<ISurfaceControl> mSurfaceControl;
    AutoPtr<ISurface> mSurface;
    Int32 mLastDW;
    Int32 mLastDH;
    Boolean mDrawNeeded;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_WATERMARK_H__
