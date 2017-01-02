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

#ifndef __ELASTOS_DROID_SYSTEMUI_EGG_CLLandACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_EGG_CLLandACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Egg_CLLandHelper.h"
#include <elastos/core/Singleton.h>
#include "_Elastos.Droid.SystemUI.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Egg{

CarClass(CLLandHelper)
    , public Singleton
    , public ILLandHelper
{
public:
    CAR_SINGLETON_DECL()
    CAR_INTERFACE_DECL()

    CARAPI L(
        /* [in] */ const String& s,
        /* [in] */ ArrayOf<IInterface* >* objects);

    CARAPI Lerp(
        /* [in] */ Float x,
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [out] */ Float* f);

    CARAPI Rlerp(
        /* [in] */ Float v,
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [out] */ Float* f);

    CARAPI Clamp(
        /* [in] */ Float f,
        /* [out] */ Float* res);

    CARAPI Frand(
        /* [out] */ Float* f);

    CARAPI Frand(
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [out] */ Float* f);

    CARAPI Irand(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b,
        /* [out] */ Int32* f);
};

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_EGG_CLLandACTIVITY_H__