
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTPINNEDHEADERVIEW_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTPINNEDHEADERVIEW_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/widget/TextView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * A custom view for the pinned section header shown at the top of the contact list.
 */
class ContactListPinnedHeaderView
    : public TextView
{
public:
    ContactListPinnedHeaderView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IView* parent);

    /**
     * Sets section header or makes it invisible if the title is null.
     */
    CARAPI_(void) SetSectionHeaderTitle(
        /* [in] */ const String& title);
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTPINNEDHEADERVIEW_H__
