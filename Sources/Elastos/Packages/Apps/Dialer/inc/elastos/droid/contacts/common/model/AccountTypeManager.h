
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_MODEL_ACCOUNTTYPEMANAGER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_MODEL_ACCOUNTTYPEMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Model {

/**
 * Represents an IM data item, wrapping the columns in
 * {@link ContactsContract.CommonDataKinds.Im}.
 */
class AccountTypeManager
    : public Object
    , public IAccountTypeManager
{
public:
    /**
     * Requests the singleton instance of {@link AccountTypeManager} with data bound from
     * the available authenticators. This method can safely be called from the UI thread.
     */
    static CARAPI_(AutoPtr<IAccountTypeManager>) GetInstance(
        /* [in] */ IContext* context);

};

} // Model
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_MODEL_ACCOUNTTYPEMANAGER_H__
