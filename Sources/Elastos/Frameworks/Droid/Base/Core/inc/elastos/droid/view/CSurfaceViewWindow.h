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

#ifndef __ELASTOS_DROID_VIEW_CSURFACEVIEWWINDOW_H__
#define __ELASTOS_DROID_VIEW_CSURFACEVIEWWINDOW_H__

#include "_Elastos_Droid_View_CSurfaceViewWindow.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/view/BaseIWindow.h"

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::View::IBaseIWindow;
using Elastos::Droid::Internal::View::BaseIWindow;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CSurfaceViewWindow)
    , public BaseIWindow
{
public:
    CAR_OBJECT_DECL()

    CSurfaceViewWindow();

    CARAPI constructor(
        /* [in] */ ISurfaceView* surfaceView);

    CARAPI Resized(
        /* [in] */ IRect* frame,
        /* [in] */ IRect* overscanInsets,
        /* [in] */ IRect* coveredInsets,
        /* [in] */ IRect* visibleInsets,
        /* [in] */ IRect* stableInsets,
        /* [in] */ Boolean reportDraw,
        /* [in] */ IConfiguration* newConfig);


    CARAPI DispatchGetNewSurface();

    /**
     * Tell the window that it is either gaining or losing focus.  Keep it up
     * to date on the current state showing navigational focus (touch mode) too.
     */
    CARAPI WindowFocusChanged(
        /* [in] */ Boolean hasFocus,
        /* [in] */ Boolean touchEnabled);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    friend class SurfaceView;

    AutoPtr<IWeakReference> mSurfaceView;

    Int32 mCurWidth;// = -1;
    Int32 mCurHeight;// = -1;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CSURFACEVIEWWINDOW_H__
