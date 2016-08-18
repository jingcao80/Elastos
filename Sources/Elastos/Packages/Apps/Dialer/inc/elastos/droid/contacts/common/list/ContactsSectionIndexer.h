
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTSSECTIONINDEXER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTSSECTIONINDEXER_H__

#include "Elastos.Droid.Widget.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Widget::ISectionIndexer;
using Elastos::Core::Object

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * A section indexer that is configured with precomputed section titles and
 * their respective counts.
 */
class ContactsSectionIndexer
    : public Object
    , public ISectionIndexer
{
public:
    /**
     * Constructor.
     *
     * @param sections a non-null array
     * @param counts a non-null array of the same size as <code>sections</code>
     */
    ContactsSectionIndexer(
        /* [in] */ ArrayOf<String>* sections,
        /* [in] */ ArrayOf<Int32>* counts);

    CAR_INTERFACE_DECL()

    CARAPI GetSections(
        /* [out, callee] */ ArrayOf<IInterface*>** sections);

    CARAPI GetPositionForSection(
        /* [in] */ Int32 sectionIndex,
        /* [out] */ Int32* position);

    CARAPI GetSectionForPosition(
        /* [in] */ Int32 position,
        /* [out] */ Int32* section);

    CARAPI_(void) SetProfileHeader(
        /* [in] */ const String& header);

private:
    static const String BLANK_HEADER_STRING;

    AutoPtr<ArrayOf<String> > mSections;
    AutoPtr<ArrayOf<Int32> > mPositions;
    Int32 mCount;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTSSECTIONINDEXER_H__
