
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_CONTACTSTATUSNUTIL_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_CONTACTSTATUSNUTIL_H__

#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

/**
 * Provides static function to get default contact status message.
 */
class ContactStatusUtil
{
public:
    /**
     * Get the presence icon resource according the status.
     *
     * @return null means don't show the status icon.
     */
    static CARAPI_(String) GetStatusString(
        /* [in] */ IContext* context,
        /* [in] */ Int32 presence);

private:
    static const String TAG;
};

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_CONTACTSTATUSNUTIL_H__
