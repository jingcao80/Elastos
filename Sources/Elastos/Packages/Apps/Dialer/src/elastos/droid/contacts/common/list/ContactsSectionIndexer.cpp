
#include "elastos/droid/contacts/common/list/ContactsSectionIndexer.h"

using Elastos::Droid::Widget::EIID_ISectionIndexer;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

const String ContactsSectionIndexer::BLANK_HEADER_STRING(" ");

ContactsSectionIndexer::ContactsSectionIndexer(
    /* [in] */ ArrayOf<String>* sections,
    /* [in] */ ArrayOf<Int32>* counts);

CAR_INTERFACE_IMPL(ContactsSectionIndexer, Object, ISectionIndexer)

ECode ContactsSectionIndexer::GetSections(
    /* [out, callee] */ ArrayOf<IInterface*>** sections);

ECode ContactsSectionIndexer::GetPositionForSection(
    /* [in] */ Int32 sectionIndex,
    /* [out] */ Int32* position);

ECode ContactsSectionIndexer::GetSectionForPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* section);

void ContactsSectionIndexer::SetProfileHeader(
    /* [in] */ const String& header);

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
