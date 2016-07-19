
#include "elastos/droid/contacts/common/util/ContactListViewUtils.h"

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

const Double ContactListViewUtils::TEXT_LEFT_PADDING_TO_CARD_PADDING_RATIO;

ECode ContactListViewUtils::ApplyCardPaddingToView(
    /* [in] */ IResources* resources,
    /* [in] */ IListView* listView,
    /* [in] */ IView* rootLayout)
{
    assert(0);
    return NOERROR;
}

void ContactListViewUtils::AddPaddingToView(
    /* [in] */ IListView* listView,
    /* [in] */ Int32 parentWidth,
    /* [in] */ Int32 listSpaceWeight,
    /* [in] */ Int32 listViewWeight)
{}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
