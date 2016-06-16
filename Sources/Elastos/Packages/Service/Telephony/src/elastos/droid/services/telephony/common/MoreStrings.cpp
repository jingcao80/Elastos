
#include "elastos/droid/services/telephony/common/MoreStrings.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {
namespace Common {

String MoreStrings::ToSafeString(
    /* [in] */ const String& value)
{
    if (value.IsNull()) {
        return String(NULL);
    }

    // Do exactly same thing as Uri#toSafeString() does, which will enable us to compare
    // sanitized phone numbers.
    StringBuilder builder;
    for (Int32 i = 0; i < value.GetLength(); i++) {
        Char32 c = value.GetChar(i);
        if (c == '-' || c == '@' || c == '.') {
            builder += c;
        }
        else {
            builder += 'x';
        }
    }
    return builder.ToString();
}

} // namespace Common
} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos