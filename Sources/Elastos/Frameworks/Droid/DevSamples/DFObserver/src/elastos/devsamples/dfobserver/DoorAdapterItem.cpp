#include "DoorAdapterItem.h"

namespace Elastos {
namespace DevSamples {
namespace DFObserver {


CAR_INTERFACE_IMPL(DoorAdapterItem, Object, IDoorAdapterItem)

DoorAdapterItem::DoorAdapterItem()
    : mIsOpen(FALSE)
{}

DoorAdapterItem::DoorAdapterItem(
    /* [in] */ const String& name,
    /* [in] */ Boolean isOpen)
    : mName(name)
    , mIsOpen(isOpen)
{
}

ECode DoorAdapterItem::IsOpen(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mIsOpen;
    return NOERROR;
}

ECode DoorAdapterItem::SetIsOpen(
    /* [in] */ Boolean value)
{
    mIsOpen = value;
    return NOERROR;
}

ECode DoorAdapterItem::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos