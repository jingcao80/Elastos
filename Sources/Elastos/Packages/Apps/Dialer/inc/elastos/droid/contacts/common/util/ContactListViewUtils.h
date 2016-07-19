
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CONTACTLISTVIEWUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CONTACTLISTVIEWUTILS_H__

#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

class ContactListViewUtils
{
public:
    /**
     * Add padding to {@param listView} if this configuration has set both space weight and
     * view weight on the layout. Use this util method instead of defining the padding in the
     * layout file so that the {@param listView}'s padding can be set proportional to the card
     * padding.
     *
     * @param resources
     * @param listView ListView that we add padding to
     * @param rootLayout layout that contains ListView and R.id.list_card
     */
    static CARAPI ApplyCardPaddingToView(
        /* [in] */ IResources* resources,
        /* [in] */ IListView* listView,
        /* [in] */ IView* rootLayout);

private:
    // These two constants will help add more padding for the text inside the card.
    static const Double TEXT_LEFT_PADDING_TO_CARD_PADDING_RATIO = 1.1;

    static CARAPI_(void) AddPaddingToView(
        /* [in] */ IListView* listView,
        /* [in] */ Int32 parentWidth,
        /* [in] */ Int32 listSpaceWeight,
        /* [in] */ Int32 listViewWeight);
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_CONTACTLISTVIEWUTILS_H__
