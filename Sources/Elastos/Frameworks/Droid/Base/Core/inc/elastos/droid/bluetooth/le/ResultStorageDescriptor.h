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

#ifndef __ELASTOS_DROID_BLUETOOTH_LE_RESULTSTORAGEDESCRIPTOR_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_RESULTSTORAGEDESCRIPTOR_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * Describes the way to store scan result.
  *
  * @hide
  */
// @SystemApi
class ResultStorageDescriptor
    : public Object
    , public IResultStorageDescriptor
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ResultStorageDescriptor();

    CARAPI constructor();

    /**
      * Constructor of {@link ResultStorageDescriptor}
      *
      * @param type Type of the data.
      * @param offset Offset from start of the advertise packet payload.
      * @param length Byte length of the data
      */
    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI GetType(
        /* [out] */ Int32* result);

    CARAPI GetOffset(
        /* [out] */ Int32* result);

    CARAPI GetLength(
        /* [out] */ Int32* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
    //    /* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI constructor(
        /* [in] */ IParcel* in);

private:
    Int32 mType;
    Int32 mOffset;
    Int32 mLength;
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_RESULTSTORAGEDESCRIPTOR_H__
