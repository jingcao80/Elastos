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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_INTERACTIONS_TOUCHPOINTMANAGER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_INTERACTIONS_TOUCHPOINTMANAGER_H__

#include <elastos/coredef.h>
#include "Elastos.Droid.Graphics.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IPoint;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Interactions {

/**
 * Singleton class to keep track of where the user last touched the screen.
 *
 * Used to pass on to the InCallUI for animation.
 */
class TouchPointManager
    : public Object
    , public ITouchPointManager
{
public:
    CAR_INTERFACE_DECL();

    static CARAPI_(AutoPtr<ITouchPointManager>) GetInstance();

    CARAPI GetPoint(
        /* [out] */ IPoint** point);

    CARAPI SetPoint(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI HasValidPoint(
        /* [out] */ Boolean* result);

private:
    TouchPointManager();

private:
    static AutoPtr<ITouchPointManager> sInstance;

    AutoPtr<IPoint> mPoint;
};

} // namespace Interactions
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_INTERACTIONS_TOUCHPOINTMANAGER_H__
