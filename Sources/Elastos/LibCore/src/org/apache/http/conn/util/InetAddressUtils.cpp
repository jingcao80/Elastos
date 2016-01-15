
#include "org/apache/http/conn/util/InetAddressUtils.h"
#include "elastos/utility/regex/CPatternHelper.h"

using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Util {

static AutoPtr<IPattern> InitIPV4()
{
    AutoPtr<CPatternHelper> helper;
    CPatternHelper::AcquireSingletonByFriend((CPatternHelper**)&helper);
    AutoPtr<IPattern> p;
    helper->Compile(
            String("^(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)(\\.(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)){3}$"), (IPattern**)&p);
    return p;
}
const AutoPtr<IPattern> InetAddressUtils::IPV4_PATTERN = InitIPV4();

static AutoPtr<IPattern> InitIPV6Std()
{
    AutoPtr<CPatternHelper> helper;
    CPatternHelper::AcquireSingletonByFriend((CPatternHelper**)&helper);
    AutoPtr<IPattern> p;
    helper->Compile(String("^(?:[0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}$"), (IPattern**)&p);
    return p;
}
const AutoPtr<IPattern> InetAddressUtils::IPV6_STD_PATTERN = InitIPV6Std();

static AutoPtr<IPattern> InitIPV6Hex()
{
    AutoPtr<CPatternHelper> helper;
    CPatternHelper::AcquireSingletonByFriend((CPatternHelper**)&helper);
    AutoPtr<IPattern> p;
    helper->Compile(
            String("^((?:[0-9A-Fa-f]{1,4}(?::[0-9A-Fa-f]{1,4})*)?)::((?:[0-9A-Fa-f]{1,4}(?::[0-9A-Fa-f]{1,4})*)?)$")
            , (IPattern**)&p);
    return p;
}
const AutoPtr<IPattern> InetAddressUtils::IPV6_HEX_COMPRESSED_PATTERN = InitIPV6Hex();

ECode InetAddressUtils::IsIPv4Address(
    /* [in] */ const String& input,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatcher> matcher;
    IPV4_PATTERN->Matcher(input, (IMatcher**)&matcher);
    return matcher->Matches(result);
}

ECode InetAddressUtils::IsIPv6StdAddress(
    /* [in] */ const String& input,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatcher> matcher;
    IPV6_STD_PATTERN->Matcher(input, (IMatcher**)&matcher);
    return matcher->Matches(result);
}

ECode InetAddressUtils::IsIPv6HexCompressedAddress(
    /* [in] */ const String& input,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatcher> matcher;
    IPV6_HEX_COMPRESSED_PATTERN->Matcher(input, (IMatcher**)&matcher);
    return matcher->Matches(result);
}

ECode InetAddressUtils::IsIPv6Address(
    /* [in] */ const String& input,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean result1, result2;
    IsIPv6Address(input, &result1);
    IsIPv6HexCompressedAddress(input, &result2);
    *result = result1 || result2;
    return NOERROR;
}


} // namespace Util
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org