#include "elastos/droid/internal/telephony/cdma/sms/CUserDataHelper.h"
#include "elastos/droid/internal/telephony/cdma/sms/UserData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CAR_INTERFACE_IMPL(CUserDataHelper, Singleton, IUserDataHelper)
CAR_SINGLETON_IMPL(CUserDataHelper)

ECode CUserDataHelper::StringToAscii(
    /* [in] */ const String& str,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = UserData::StringToAscii(str);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
