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

#ifndef __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGERINPUTDEVICESCHANGEDLISTENER_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGERINPUTDEVICESCHANGEDLISTENER_H__

#include "_Elastos_Droid_Hardware_Input_CInputManagerInputDevicesChangedListener.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

class CInputManager;

CarClass(CInputManagerInputDevicesChangedListener)
    , public Object
    , public IInputDevicesChangedListener
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
            /* [in] */ Handle32 owner);

    CARAPI OnInputDevicesChanged(
        /* [in] */ ArrayOf<Int32>* deviceIdAndGeneration);

private:
    CInputManager* mOwner;
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_CINPUTMANAGERINPUTDEVICESCHANGEDLISTENER_H__
