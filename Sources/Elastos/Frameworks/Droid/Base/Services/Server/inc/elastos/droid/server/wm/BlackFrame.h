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

#ifndef __ELASTOS_DROID_SERVER_WM_BLACKFRAME_H__
#define __ELASTOS_DROID_SERVER_WM_BLACKFRAME_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::ISurfaceSession;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

/**
 * Four black surfaces put together to make a black frame.
 */
class BlackFrame : public Object
{
public:
    class BlackSurface : public Object
    {
    public:
        BlackSurface(
            /* [in] */ ISurfaceSession* session,
            /* [in] */ Int32 layer,
            /* [in] */ Int32 l,
            /* [in] */ Int32 t,
            /* [in] */ Int32 r,
            /* [in] */ Int32 b,
            /* [in] */ Int32 layerStack,
            /* [in] */ BlackFrame* host);

        CARAPI_(void) SetAlpha(
            /* [in] */ Float alpha);

        CARAPI_(void) SetMatrix(
            /* [in] */ IMatrix* matrix);

        CARAPI_(void) ClearMatrix();

    public:
        Int32 mLeft;
        Int32 mTop;
        Int32 mLayer;
        AutoPtr<ISurfaceControl> mSurface;

    private:
        BlackFrame* mHost;
    };

public:
    BlackFrame(
        /* [in] */ ISurfaceSession* session,
        /* [in] */ IRect* outer,
        /* [in] */ IRect* inner,
        /* [in] */ Int32 layer,
        /* [in] */ Int32 layerStack,
        /* [in] */ Boolean forceDefaultOrientation);

    // void printTo(String prefix, PrintWriter pw);

    CARAPI_(void) Kill();

    CARAPI_(void) Hide();

    CARAPI_(void) SetAlpha(
        /* [in] */ Float alpha);

    CARAPI_(void) SetMatrix(
        /* [in] */ IMatrix* matrix);

    CARAPI_(void) ClearMatrix();

public:
    AutoPtr<IRect> mOuterRect;
    AutoPtr<IRect> mInnerRect;
    AutoPtr<IMatrix> mTmpMatrix;
    AutoPtr< ArrayOf<Float> > mTmpFloats;
    AutoPtr< ArrayOf<BlackSurface*> > mBlackSurfaces;

    Boolean mForceDefaultOrientation;
};

} // Wm
} // Server
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Wm::BlackFrame::BlackSurface, IInterface)

#endif //__ELASTOS_DROID_SERVER_WM_BLACKFRAME_H__
