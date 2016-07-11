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
    virtual CARAPI_(ComprehensionTlvTag) GetTag() = 0;
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

    CARAPI constructor(
        /* [in] */ IParcel* in);

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
    Int32 sourceId;
    Int32 destinationId;
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
    Int32 recordNumber;
    Boolean selfExplanatory;
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
    AutoPtr<ArrayOf<Int32> > recordNumbers;
    Boolean selfExplanatory;
};

class ActivateDescriptor
    : public ValueObject
{
public:
    // @Override
    CARAPI GetTag(
        /* [out] */ ComprehensionTlvTag* result);

public:
    Int32 target;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDDETAILS_H__
