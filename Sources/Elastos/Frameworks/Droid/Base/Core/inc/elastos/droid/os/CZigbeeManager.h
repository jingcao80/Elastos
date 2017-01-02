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

#ifndef __ELASTOS_DROID_OS_CZIGBEEMANAGER_H__
#define __ELASTOS_DROID_OS_CZIGBEEMANAGER_H__

#include "_Elastos_Droid_Os_CZigbeeManager.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CZigbeeManager)
{
public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /*
     *  Check if type is defined
     *  Return value: true - type is defined, false - invalid type
     */
    CARAPI IsValidType(
        /* [in] */ Int32 checkType,
        /* [out] */ Boolean* res);

    // Get available ACTION for Zigbee service
    CARAPI GetAllActions(
        /* [out, callee] */ ArrayOf<String>** actions);

private:
    static const AutoPtr<ArrayOf<Int32> > sDeviceTypes;
    static const AutoPtr<ArrayOf<String> > sActions;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CZIGBEEMANAGER_H__
