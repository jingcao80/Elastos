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

#ifndef __ELASTOS_DROID_LOCATION_GPSNAVIGATIONMESSAGE_H__
#define __ELASTOS_DROID_LOCATION_GPSNAVIGATIONMESSAGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * A class containing a GPS satellite Navigation Message.
 *
 * @hide
 */
class GpsNavigationMessage
    : public Object
    , public IGpsNavigationMessage
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GpsNavigationMessage();

    CARAPI constructor();

    /**
     * Sets all contents to the values stored in the provided object.
     */
    CARAPI Set(
        /* [in] */ IGpsNavigationMessage* navigationMessage);

    /**
     * Resets all the contents to its original state.
     */
    CARAPI Reset();

    /**
     * Gets the type of the navigation message contained in the object.
     */
    CARAPI GetType(
        /* [out] */ Byte* type);

    /**
     * Sets the type of the navigation message.
     */
    CARAPI SetType(
        /* [in] */ Byte value);

    /**
     * Gets the Pseudo-random number.
     * Range: [1, 32].
     */
    CARAPI GetPrn(
        /* [out] */ Byte* prn);

    /**
     * Sets the Pseud-random number.
     */
    CARAPI SetPrn(
        /* [in] */ Byte value);

    /**
     * Gets the Message Identifier.
     * It provides an index so the complete Navigation Message can be assembled. i.e. for L1 C/A
     * subframe 4 and 5, this value corresponds to the 'frame id' of the navigation message.
     * Subframe 1, 2, 3 does not contain a 'frame id' and this might be reported as -1.
     */
    CARAPI GetMessageId(
        /* [out] */ Int16* msgId);

    /**
     * Sets the Message Identifier.
     */
    CARAPI SetMessageId(
        /* [in] */ Int16 value);

    /**
     * Gets the Sub-message Identifier.
     * If required by {@link #getType()}, this value contains a sub-index within the current message
     * (or frame) that is being transmitted. i.e. for L1 C/A the sub-message identifier corresponds
     * to the sub-frame Id of the navigation message.
     */
    CARAPI GetSubmessageId(
        /* [out] */ Int16* id);

    /**
     * Sets the Sub-message identifier.
     */
    CARAPI SetSubmessageId(
        /* [in] */ Int16 value);

    /**
     * Gets the data associated with the Navigation Message.
     * The bytes (or words) specified using big endian format (MSB first).
     */
    // @NonNull
    CARAPI GetData(
        /* [out, callee] */ ArrayOf<Byte>** data);

    /**
     * Sets the data associated with the Navigation Message.
     */
    CARAPI SetData(
        /* [in] */ ArrayOf<Byte>* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI Initialize();

    /**
     * Gets a string representation of the 'type'.
     * For internal and logging use only.
     */
    CARAPI GetTypeString(
        /* [out] */ String* str);

private:
    const static String TAG;
    const static AutoPtr<ArrayOf<Byte> > EMPTY_ARRAY;
    Byte mType;
    Byte mPrn;
    Int16 mMessageId;
    Int16 mSubmessageId;
    AutoPtr<ArrayOf<Byte> > mData;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_GPSNAVIGATIONMESSAGE_H__
