
#include "elastos/droid/contacts/common/util/TelephonyManagerUtils.h"

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

const String TelephonyManagerUtils::TAG("TelephonyManagerUtils");

ECode TelephonyManagerUtils::GetVoiceMailAlphaTag(
    /* [in] */ IContext* context,
    /* [out] */ String* tag)
{
    assert(0);
    return NOERROR;
}

ECode TelephonyManagerUtils::GetCurrentCountryIso(
    /* [in] */ IContext* context,
    /* [in] */ ILocale* locale,
    /* [out] */ String* iso)
{
    assert(0);
    return NOERROR;
}

Boolean TelephonyManagerUtils::HasVideoCallSubscription(
    /* [in] */ IContext* context)
{
    assert(0);
    return FALSE;
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
