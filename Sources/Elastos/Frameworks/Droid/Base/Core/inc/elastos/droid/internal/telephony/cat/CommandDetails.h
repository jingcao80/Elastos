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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDDETAILS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDDETAILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class ValueObject
    : public Object
{
public:
    virtual CARAPI GetTag(
        /* [out] */ ComprehensionTlvTag* result) = 0;
};

/**
  * Class for Command Details object of proactive commands from SIM.
  * {@hide}
  */
class CommandDetails
    : public ValueObject
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<CommandDetails>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ CommandDetails* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* in,
//            /* [out] */ CommandDetails** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ CommandDetails[]** result);
//
//    private:
//        CommandDetails* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    CommandDetails();

    CARAPI constructor();

    // @Override
    CARAPI GetTag(
        /* [out] */ ComprehensionTlvTag* result);

    virtual CARAPI CompareTo(
        /* [in] */ CommandDetails* other,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

public:
    Boolean mCompRequired;
    Int32 mCommandNumber;
    Int32 mTypeOfCommand;
    Int32 mCommandQualifier;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<CommandDetails> > CREATOR;
};

class DeviceIdentities
    : public ValueObject
{
public:
    // @Override
    CARAPI GetTag(
        /* [out] */ ComprehensionTlvTag* result);

public:
    Int32 mSourceId;
    Int32 mDestinationId;
};

// Container class to hold icon identifier value.
class IconId
    : public ValueObject
{
public:
    // @Override
    CARAPI GetTag(
        /* [out] */ ComprehensionTlvTag* result);

public:
    Int32 mRecordNumber;
    Boolean mSelfExplanatory;
};

// Container class to hold item icon identifier list value.
class ItemsIconId
    : public ValueObject
{
public:
    // @Override
    CARAPI GetTag(
        /* [out] */ ComprehensionTlvTag* result);

public:
    AutoPtr<ArrayOf<Int32> > mRecordNumbers;
    Boolean mSelfExplanatory;
};

class ActivateDescriptor
    : public ValueObject
{
public:
    // @Override
    CARAPI GetTag(
        /* [out] */ ComprehensionTlvTag* result);

public:
    Int32 mTarget;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDDETAILS_H__
