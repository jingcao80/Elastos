
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/contacts/common/ContactStatusUtil.h"
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::IContactsContractStatusColumns;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

const String ContactStatusUtil::TAG("ContactStatusUtil");

String ContactStatusUtil::GetStatusString(
    /* [in] */ IContext* context,
    /* [in] */ Int32 presence)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    String str;
    switch (presence) {
        case IContactsContractStatusColumns::AVAILABLE:
            resources->GetString(Elastos::Droid::Dialer::R::string::status_available, &str);
            return str;
        case IContactsContractStatusColumns::IDLE:
        case IContactsContractStatusColumns::AWAY:
            resources->GetString(Elastos::Droid::Dialer::R::string::status_away, &str);
            return str;
        case IContactsContractStatusColumns::DO_NOT_DISTURB:
            resources->GetString(Elastos::Droid::Dialer::R::string::status_busy, &str);
            return str;
        case IContactsContractStatusColumns::OFFLINE:
        case IContactsContractStatusColumns::INVISIBLE:
        default:
            return String(NULL);
    }
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
