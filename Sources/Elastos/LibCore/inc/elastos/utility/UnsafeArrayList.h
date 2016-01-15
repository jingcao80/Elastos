
#ifndef __ELASTOS_UTILITY_UNSAFEARRAYLIST_H__
#define __ELASTOS_UTILITY_UNSAFEARRAYLIST_H__

#include "AbstractList.h"

namespace Elastos {
namespace Utility {

class UnsafeArrayList
    : public AbstractList
    , public IUnsafeArrayList
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const InterfaceID& elementType,
        /* [in] */ Int32 initialCapacity);

    CARAPI Add(
        /* [in] */ IInterface* element,
        /* [out] */ Boolean* res);

    CARAPI GetArrays(
        /* [out, callee] */ ArrayOf<IInterface*> ** array);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI Get(
        /* [in] */ Int32 location,
        /* [out] */ IInterface** object);

    CARAPI Set(
        /* [in] */ Int32 location,
        /* [in] */ IInterface* object,
        /* [out] */ IInterface** prevObject);

    CARAPI Add(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* object);

    CARAPI Add(
        /* [in] */ IInterface* object);

    CARAPI AddAll(
        /* [in] */ Int32 index,
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI AddAll(
        /* [in] */ Int32 index,
        /* [in] */ ICollection* collection);

    CARAPI IndexOf(
        /* [in] */ IInterface* object,
        /* [out] */ Int32* index);

    CARAPI LastIndexOf(
        /* [in] */ IInterface* object,
        /* [out] */ Int32* index);

    CARAPI GetListIterator(
        /* [in] */ IListIterator** it);

    CARAPI GetListIterator(
        /* [in] */ Int32 index,
        /* [in] */ IListIterator** it);

    CARAPI Remove(
        /* [in] */ Int32 index,
        /* [in] */ IInterface** object);

    CARAPI Remove(
        /* [in] */ Int32 index);

    CARAPI Set(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* object);

    CARAPI GetSubList(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ IList** list);

    CARAPI GetIterator(
        /* [in] */ IIterator** it);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI AddAll(
        /* [in] */ ICollection* collection);

    CARAPI Clear();

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* index);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI Remove(
        /* [in] */ IInterface* object);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ Elastos::ArrayOf<IInterface*>* array,
        /* [out, callee] */ ArrayOf<IInterface*>** outarray);

protected:
    UnsafeArrayList();

    UnsafeArrayList(
        /* [in] */ const InterfaceID& elementType,
        /* [in] */ Int32 initialCapacity);

private:
    InterfaceID mElementType;

    AutoPtr< ArrayOf <IInterface*> >  mArray;

    Int32 mSize;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_UNSAFEARRAYLIST_H__