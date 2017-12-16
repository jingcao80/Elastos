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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYMANAGERCLIENT_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYMANAGERCLIENT_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityManagerClient.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include <binder/Binder.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityManagerClient)
    , public Object
    , public IIAccessibilityManagerClient
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityManagerClient();

    ~CAccessibilityManagerClient();

    CARAPI constructor(
        /* [in] */ IAccessibilityManager* host);

    CARAPI SetState(
        /* [in] */ Int32 stateFlags);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    android::IBinder* mBBinder;

private:
    CAccessibilityManager* mHost;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYMANAGERCLIENT_H__
