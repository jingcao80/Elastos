
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_DEFAULTCONTACTLISTADAPTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_DEFAULTCONTACTLISTADAPTER_H__

#include "elastos/droid/contacts/common/list/ContactListAdapter.h"

using Elastos::Droid::Content::ICursorLoader;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A cursor adapter for the {@link ContactsContract.Contacts#CONTENT_TYPE} content type.
 */
class DefaultContactListAdapter
    : public ContactListAdapter
{
public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI ConfigureLoader(
        /* [in] */ ICursorLoader* loader,
        /* [in] */ Int64 directoryId);

protected:
    CARAPI_(void) ConfigureUri(
        /* [in] */ ICursorLoader* loader,
        /* [in] */ Int64 directoryId,
        /* [in] */ IContactListFilter* filter);

    // @Override
    CARAPI_(void) BindView(
        /* [in] */ IView* v,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position);

private:
    CARAPI_(void) ConfigureSelection(
        /* [in] */ ICursorLoader* loader,
        /* [in] */ Int64 directoryId,
        /* [in] */ IContactListFilter* filter);

    CARAPI_(Boolean) IsCustomFilterForPhoneNumbersOnly();

public:
    static const Char32 SNIPPET_START_MATCH = '[';
    static const Char32 SNIPPET_END_MATCH = ']';
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_DEFAULTCONTACTLISTADAPTER_H__
