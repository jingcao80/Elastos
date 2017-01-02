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

#ifndef __ELASTOS_DROID_VIEW_CSURFACECONTROLHELPER_H__
#define __ELASTOS_DROID_VIEW_CSURFACECONTROLHELPER_H__

#include "_Elastos_Droid_View_CSurfaceControlHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CSurfaceControlHelper)
    , public Singleton
    , public ISurfaceControlHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI OpenTransaction();

    CARAPI CloseTransaction();

    CARAPI SetAnimationTransaction();

    CARAPI ClearAnimationFrameStats(
        /* [out] */ Boolean* stats);

    CARAPI GetAnimationFrameStats(
        /* [in] */ IWindowAnimationFrameStats* outStats,
        /* [out] */ Boolean* stats);

    CARAPI SetDisplayPowerMode(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 mode);

    CARAPI GetDisplayConfigs(
        /* [in] */ IBinder* displayToken,
        /* [out, callee] */ ArrayOf<IPhysicalDisplayInfo*>** infos);

    CARAPI GetActiveConfig(
        /* [in] */ IBinder* displayToken,
        /* [out] */ Int32* config);

    CARAPI SetActiveConfig(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 id,
        /* [out] */ Boolean* result);

    CARAPI SetDisplayProjection(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 orientation,
        /* [in] */ IRect* layerStackRect,
        /* [in] */ IRect* displayRect);

    CARAPI SetDisplayLayerStack(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 layerStack);

    CARAPI SetDisplaySurface(
        /* [in] */ IBinder* displayToken,
        /* [in] */ ISurface* surface);

    CARAPI SetDisplaySize(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI CreateDisplay(
        /* [in] */ const String& name,
        /* [in] */ Boolean secure,
        /* [out] */ IBinder** display);

    CARAPI DestroyDisplay(
        /* [in] */ IBinder* displayToken);

    CARAPI GetBuiltInDisplay(
        /* [in] */ Int32 builtInDisplayId,
        /* [out] */ IBinder** display);

    CARAPI Screenshot(
        /* [in] */ IBinder* display,
        /* [in] */ ISurface* consumer,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 minLayer,
        /* [in] */ Int32 maxLayer,
        /* [in] */ Boolean useIdentityTransform);

    CARAPI Screenshot(
        /* [in] */ IBinder* display,
        /* [in] */ ISurface* consumer,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI Screenshot(
        /* [in] */ IBinder* display,
        /* [in] */ ISurface* consumer) ;

    CARAPI Screenshot(
        /* [in] */ IRect* sourceCrop,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 minLayer,
        /* [in] */ Int32 maxLayer,
        /* [in] */ Boolean useIdentityTransform,
        /* [in] */ Int32 rotation,
        /* [out] */ IBitmap** bm);

    CARAPI Screenshot(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IBitmap** bm);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CSURFACECONTROLHELPER_H__
