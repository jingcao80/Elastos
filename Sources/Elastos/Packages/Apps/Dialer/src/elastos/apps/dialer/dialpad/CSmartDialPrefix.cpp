
#include "dialpad/CSmartDialPrefix.h"
#include "dialpad/SmartDialPrefix.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL(CSmartDialPrefix, Singleton, ISmartDialPrefix);

CAR_SINGLETON_IMPL(CSmartDialPrefix);

ECode CSmartDialPrefix::InitializeNanpSettings(
    /* [in] */ IContext* context)
{
    SmartDialPrefix::InitializeNanpSettings(context);
    return NOERROR;
}

ECode CSmartDialPrefix::SetUserInNanpRegion(
    /* [in] */ Boolean userInNanpRegion)
{
    SmartDialPrefix::SetUserInNanpRegion(userInNanpRegion);
    return NOERROR;
}

ECode CSmartDialPrefix::ParseToIndexTokens(
    /* [in] */ const String& contactName,
    /* [out] */ IArrayList** tokens)
{
    VALUE_NOT_NULL(tokens);

    AutoPtr<IArrayList> result = SmartDialPrefix::ParseToIndexTokens(contactName);
    *tokens = result;
    REFCOUNT_ADD(*tokens);
    return NOERROR;
}

ECode CSmartDialPrefix::GenerateNamePrefixes(
    /* [in] */ const String& index,
    /* [out] */ IArrayList** prefixes)
{
    VALUE_NOT_NULL(prefixes);

    AutoPtr<IArrayList> result = SmartDialPrefix::GenerateNamePrefixes(index);
    *prefixes = result;
    REFCOUNT_ADD(*prefixes);
    return NOERROR;
}

ECode CSmartDialPrefix::ParseToNumberTokens(
    /* [in] */ const String& number,
    /* [out] */ IArrayList** tokens)
{
    VALUE_NOT_NULL(tokens);

    AutoPtr<IArrayList> result = SmartDialPrefix::ParseToNumberTokens(number);
    *tokens = result;
    REFCOUNT_ADD(*tokens);
    return NOERROR;
}

ECode CSmartDialPrefix::ParsePhoneNumber(
    /* [in] */ const String& number,
    /* [out] */ ISmartDialPrefixPhoneNumberTokens** result)
{
    VALUE_NOT_NULL(result);

    AutoPtr<ISmartDialPrefixPhoneNumberTokens> tokens = SmartDialPrefix::ParsePhoneNumber(number);
    *result = tokens;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmartDialPrefix::GetMap(
    /* [out] */ ISmartDialMap** map)
{
    VALUE_NOT_NULL(map);

    AutoPtr<ISmartDialMap> result = SmartDialPrefix::GetMap();
    *map = result;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

ECode CSmartDialPrefix::IsCountryNanp(
    /* [in] */ const String& country,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    *result = SmartDialPrefix::IsCountryNanp(country);
    return NOERROR;
}

ECode CSmartDialPrefix::GetUserInNanpRegion(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    *result = SmartDialPrefix::GetUserInNanpRegion();
    return NOERROR;
}

} // Dialpad
} // Dialer
} // Apps
} // Elastos
