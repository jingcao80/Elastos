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

#ifndef __Elastos_DevSamples_DFObserver_DoorAdapterItem_H__
#define __Elastos_DevSamples_DFObserver_DoorAdapterItem_H__

#include "_Elastos.DevSamples.DFObserver.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

class DoorAdapterItem
    : public Object
    , public IDoorAdapterItem
{
public:
    CAR_INTERFACE_DECL()

    DoorAdapterItem();

    DoorAdapterItem(
        /* [in] */ const String& name,
        /* [in] */ Boolean isOpen);

    CARAPI GetIsOpen(
        /* [out] */ Boolean* value);

    CARAPI SetIsOpen(
        /* [in] */ Boolean value);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI ToString(
        /* [out] */ String* str);
private:
    String mName;
    Boolean mIsOpen;
};

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DFObserver_DoorAdapterItem_H__
