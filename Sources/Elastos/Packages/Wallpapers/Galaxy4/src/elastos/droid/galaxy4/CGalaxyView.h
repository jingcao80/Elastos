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

#ifndef __ELASTOS_DROID_GALAXY4_CGALAXYVIEW_H__
#define __ELASTOS_DROID_GALAXY4_CGALAXYVIEW_H__

#include "_Elastos_Droid_Galaxy4_CGalaxyView.h"
#include <elastos/droid/view/SurfaceView.h>
#include "GalaxyRS.h"
#include "RenderScript.h"

using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::SurfaceView;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

CarClass(CGalaxyView)
    , public SurfaceView
{
private:
    class SHCallback
        : public Object
        , public ISurfaceHolderCallback
    {
    public:
        CAR_INTERFACE_DECL()

        SHCallback(
            /* [in] */ CGalaxyView* host);

        CARAPI SurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        CARAPI SurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

    private:
        CGalaxyView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SurfaceChanged(
        /* [in] */ ISurfaceHolder* holder,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI SurfaceCreated(
        /* [in] */ ISurfaceHolder* holder);

    CARAPI SurfaceDestroyed(
        /* [in] */ ISurfaceHolder* holder);

    /**
     * @deprecated in API 16
     **/
    CARAPI CreateRenderScript(
        /* [in] */ RenderScript::SurfaceConfig* sc,
        /* [out] */ RenderScript** result);

    CARAPI DestroyRenderScript();

    CARAPI SetRenderScript(
        /* [in] */ RenderScript* rs);

protected:
    CARAPI OnDetachedFromWindow();

private:
    CARAPI_(void) Init();

private:
    AutoPtr<RenderScript> mRS;
    AutoPtr<GalaxyRS> mRender;
    AutoPtr<SHCallback> mCallback;
    AutoPtr<ISurfaceHolder> mSurfaceHolder;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_CGALAXYVIEW_H__
