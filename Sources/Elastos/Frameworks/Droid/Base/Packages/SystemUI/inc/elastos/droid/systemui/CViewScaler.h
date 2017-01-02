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

#ifndef  __ELASTOS_DROID_SYSTEMUI_CVIEWSCALER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CVIEWSCALER_H__

#include "_Elastos_Droid_SystemUI_CViewScaler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::SystemUI::StatusBar::IExpandableView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class CExpandHelper;

CarClass(CViewScaler)
    , public Object
    , public IViewScaler
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IExpandHelper* host);

    CARAPI SetView(
        /* [in] */ IExpandableView* v);

    CARAPI SetHeight(
        /* [in] */ Float h);

    CARAPI GetHeight(
        /* [out] */ Float* rst);

    CARAPI GetNaturalHeight(
        /* [in] */ Int32 maximum,
        /* [out] */ Int32* naturalHeight);

private:
    CExpandHelper* mHost;
    AutoPtr<IExpandableView> mView;
};

} // SystemUI
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CVIEWSCALER_H__
