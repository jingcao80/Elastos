
#include "elastos/droid/content/CComponentNameHelper.h"
#include "elastos/droid/content/CComponentName.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CComponentNameHelper, Singleton, IComponentNameHelper)

CAR_SINGLETON_IMPL(CComponentNameHelper)

ECode CComponentNameHelper::UnflattenFromString(
    /* [in] */ const String& str,
    /* [out] */ IComponentName** componentName)
{
    return CComponentName::UnflattenFromString(str, componentName);
}

ECode CComponentNameHelper::WriteToParcel(
    /* [in] */ IComponentName* cn,
    /* [in] */ IParcel* dest)
{
    return CComponentName::WriteToParcel(cn, dest);
}

ECode CComponentNameHelper::ReadFromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ IComponentName** cn)
{
    return CComponentName::ReadFromParcel(source, cn);
}

ECode CComponentNameHelper::AppendShortClassName(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    return CComponentName::AppendShortClassName(sb, packageName, className);
}

ECode CComponentNameHelper::PrintShortClassName(
    /* [in] */ IPrintWriter* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    return CComponentName::PrintShortClassName(sb, packageName, className);
}

ECode CComponentNameHelper::AppendShortString(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    return CComponentName::AppendShortString(sb, packageName, className);
}

ECode CComponentNameHelper::PrintShortString(
    /* [in] */ IPrintWriter* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    return CComponentName::PrintShortString(sb, packageName, className);
}

} // namespace Content
} // namespace Droid
} // namespace Elastos
