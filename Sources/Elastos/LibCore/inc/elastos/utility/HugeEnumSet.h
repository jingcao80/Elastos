#ifndef __ELASTOS_UTILITY_HUGEENUMSET__H_
#define __ELASTOS_UTILITY_HUGEENUMSET__H_

#include "EnumSet.h"

namespace Elastos {
namespace Utility {

class HugeEnumSet
    : public EnumSet
    , public IHugeEnumSet
{
private:
    class HugeEnumSetIterator
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        HugeEnumSetIterator(
            /* [in] */ IHugeEnumSet* host);

        CARAPI ComputeNextElement();

        CARAPI HasNext(
            /* [out] */ Boolean* has);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        IHugeEnumSet* mHost;
        Int64 mCurrentBits;
        Int32 mIndex;
        Int64 mMask;
        AutoPtr<IInterface> mLast;
    };

protected:
    HugeEnumSet();

    HugeEnumSet(
        /* [in] */ const InterfaceID& cls,
        /* [in] */ ArrayOf<IInterface *> * enums);

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const InterfaceID& cls,
        /* [in] */ ArrayOf<IInterface *> * enums);

    CARAPI Add(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI Clear();

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI Complement();

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Remove(
        /* [in] */ IInterface* object);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

    CARAPI SetRange(
        /* [in] */ IInterface* start,
        /* [in] */ IInterface* end);

    CARAPI GetEnums(
        /*[out, callee]*/ ArrayOf<IInterface*>** arrays);

    CARAPI GetBits(
        /*[out, callee]*/ ArrayOf<Int64>** arrays);

    CARAPI Add(/* [in] */ IInterface* object);

    CARAPI AddAll(/* [in] */ ICollection* collection);

    CARAPI GetHashCode(/* [out] */ Int32* result);

    CARAPI IsEmpty(/* [out] */ Boolean* result);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inarray,
        /* [out, callee] */ ArrayOf<IInterface*>** outarray);

protected:
    CARAPI_(String) GetClassName() { return String("HugeEnumSet"); }

private:
    static const Int32 BIT_IN_LONG;// = 64;

    const AutoPtr<ArrayOf <IInterface*> > mEnums;

    AutoPtr<ArrayOf <Int64> > mBits;

    Int32 mSize;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_HUGEENUMSET__H_