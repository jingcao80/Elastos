#include "InetUnixAddress.h"
#include "OsConstants.h"

using Elastos::Droid::System::OsConstants;

namespace Elastos {
namespace Net {


/**
 * Constructs an AF_UNIX InetAddress for the given path.
 */
InetUnixAddress::InetUnixAddress(
    /* [in] */ const String& path)
{
    InetAddress::constructor(OsConstants::_AF_UNIX, path.GetBytes(), String(NULL));
}

/**
 * Constructs an AF_UNIX InetAddress for the given path.
 */
InetUnixAddress::InetUnixAddress(
    /* [in] */ ArrayOf<Byte> * path)
{
    InetAddress::constructor(OsConstants::_AF_UNIX, path, String(NULL));
}

/**
 * Returns a string form of this InetAddress.
 */
ECode InetUnixAddress::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    return NOERROR;
}

} // namespace Net
} // namespace Elastos