
#include "Elastos.Droid.Content.h"
#include "elastos/droid/utility/NativeTextHelper.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Utility {

ECode NativeTextHelper::GetLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    return GetLocalString(context, originalString, String("android"), originNamesId, localNamesId, result);
}

ECode NativeTextHelper::GetInternalLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    String packageName;
    context->GetPackageName(&packageName);
    return GetLocalString(context, originalString, packageName, originNamesId, localNamesId, result);
}

ECode NativeTextHelper::GetLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ const String& defPackage,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<String> > origNames;
    resources->GetStringArray(originNamesId, (ArrayOf<String>**)&origNames);

    AutoPtr< ArrayOf<String> > localNames;
    resources->GetStringArray(localNamesId, (ArrayOf<String>**)&localNames);
    for (Int32 i = 0; i < origNames->GetLength(); ++i) {
        if ((*origNames)[i].EqualsIgnoreCase(originalString)) {
            Int32 identify = 0;
            resources->GetIdentifier((*localNames)[i], String("string"), defPackage, &identify);
            return context->GetString(identify, result);
        }
    }
    *result = originalString;
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
