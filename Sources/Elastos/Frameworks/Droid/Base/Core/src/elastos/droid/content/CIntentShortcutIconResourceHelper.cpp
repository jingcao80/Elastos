
#include "elastos/droid/content/CIntentShortcutIconResourceHelper.h"
#include "elastos/droid/content/CIntentShortcutIconResource.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CIntentShortcutIconResourceHelper, Singleton, IIntentShortcutIconResourceHelper)

CAR_SINGLETON_IMPL(CIntentShortcutIconResourceHelper)

ECode CIntentShortcutIconResourceHelper::FromContext(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceId,
    /* [out] */ IIntentShortcutIconResource** shortcutIconRes)
{
    VALIDATE_NOT_NULL(shortcutIconRes)
    *shortcutIconRes = NULL;
    VALIDATE_NOT_NULL(context)

    AutoPtr<CIntentShortcutIconResource> icon;
    FAIL_RETURN(CIntentShortcutIconResource::NewByFriend((CIntentShortcutIconResource**)&icon))
    String packageName;
    FAIL_RETURN(context->GetPackageName(&packageName))
    FAIL_RETURN(icon->SetPackageName(packageName))
    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res))
    String resourceName;
    FAIL_RETURN(res->GetResourceName(resourceId, &resourceName))
    FAIL_RETURN(icon->SetResourceName(resourceName))
    *shortcutIconRes = (IIntentShortcutIconResource*)icon.Get();
    REFCOUNT_ADD(*shortcutIconRes)
    return NOERROR;
}

}
}
}

