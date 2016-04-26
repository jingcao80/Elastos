#include "elastos/droid/utility/CParcelableList.h"

using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_OBJECT_IMPL(CParcelableList)

CAR_INTERFACE_IMPL(CParcelableList, ArrayList, IParcelable)

ECode CParcelableList::constructor(
    /* [in] */ IList* list)
{
    return ArrayList::constructor(ICollection::Probe(list));
}

ECode CParcelableList::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 size;
    GetSize(&size);
    dest->WriteInt32(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        Get(i, (IInterface**)&item);
        dest->WriteInterfacePtr(item);
    }

    return NOERROR;
}

ECode CParcelableList::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 size;
    source->ReadInt32(&size);
    EnsureCapacity(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        source->ReadInterfacePtr((Handle32*)&item);
        Add(item);
    }

    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos

