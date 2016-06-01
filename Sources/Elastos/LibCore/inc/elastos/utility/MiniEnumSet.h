#ifndef __ELASTOS_UTILITY_MINIENUMSET_H__
#define __ELASTOS_UTILITY_MINIENUMSET_H__

#include "EnumSet.h"

namespace Elastos {
namespace Utility {

class MiniEnumSet
    : public EnumSet
    , public IMiniEnumSet
{
private:
    class MiniEnumSetIterator
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        MiniEnumSetIterator(
            /* [in] */ IMiniEnumSet* host);

        CARAPI HasNext(
            /* [out] */ Boolean* has);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        IMiniEnumSet* mHost;
        Int64 mCurrentBits;
        Int64 mMask;
        AutoPtr<IInterface> mLast;
    };

protected:
    MiniEnumSet();

    MiniEnumSet(
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

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI Complement();

    CARAPI SetRange(
        /* [in] */ IInterface* start,
        /* [in] */ IInterface* end);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI GetEnums(
        /*[out, callee]*/ ArrayOf<IInterface*>** arrays);

    CARAPI GetBits(
        /*[out]*/ Int64* bits);

    CARAPI Remove(
        /* [in] */ IInterface* object);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

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
    CARAPI_(String) GetClassName() { return String("MiniEnumSet"); }

private:
    static const Int32 MAX_ELEMENTS;// = 64;

    AutoPtr< ArrayOf<IInterface*> > mEnums;

    Int32 mSize;

    Int64 mBits;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_MINIENUMSET_H__