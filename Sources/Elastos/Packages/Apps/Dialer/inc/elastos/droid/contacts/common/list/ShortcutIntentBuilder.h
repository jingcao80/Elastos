
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_SHORTCUTINTENTBUILDER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_SHORTCUTINTENTBUILDER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * Constructs shortcut intents.
 */
class ShortcutIntentBuilder
    : public Object
{
public:
    ShortcutIntentBuilder(
        /* [in] */ IContext* context,
        /* [in] */ IOnShortcutIntentCreatedListener* listener);
};

} // List
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_SHORTCUTINTENTBUILDER_H__
