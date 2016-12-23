#include "DoorAdapterItem.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

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

ECode DoorAdapterItem::GetIsOpen(
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

ECode DoorAdapterItem::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("Door{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += ", name:";    sb += mName;
    sb += ", ";         sb += mIsOpen ? "Open" : "Closed";
    *str = sb.ToString();
    return NOERROR;
}

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos