
#include "elastos/droid/content/CIntentShortcutIconResource.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CIntentShortcutIconResource, Object, IIntentShortcutIconResource, IParcelable)

CAR_OBJECT_IMPL(CIntentShortcutIconResource)

ECode CIntentShortcutIconResource::constructor()
{
    return NOERROR;
}

ECode CIntentShortcutIconResource::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mPackageName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::GetResourceName(
    /* [out] */ String* resourceName)
{
    VALIDATE_NOT_NULL(resourceName)
    *resourceName = mResourceName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::SetResourceName(
    /* [in] */ const String& resourceName)
{
    mResourceName = resourceName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mResourceName;
    return NOERROR;
}

ECode CIntentShortcutIconResource::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mPackageName))
    FAIL_RETURN(source->ReadString(&mResourceName))
    return NOERROR;
}

ECode CIntentShortcutIconResource::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mPackageName))
    FAIL_RETURN(dest->WriteString(mResourceName))
    return NOERROR;
}

}
}
}

