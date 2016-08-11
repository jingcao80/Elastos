
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_CONTACTPRESENCEICONUTIL_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_CONTACTPRESENCEICONUTIL_H__

#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

/**
 * Define the contact present show policy in Contacts
 */
class ContactPresenceIconUtil
{
public:
    /**
     * Get the presence icon resource according the status.
     *
     * @return null means don't show the status icon.
     */
    static CARAPI_(AutoPtr<IDrawable>) GetPresenceIcon(
        /* [in] */ IContext* context,
        /* [in] */ Int32 status);
};

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_CONTACTPRESENCEICONUTIL_H__
