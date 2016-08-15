
#include "elastos/droid/contacts/common/ContactsUtils.h"

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

const String ContactsUtils::SCHEME_IMTO("imto");
const String ContactsUtils::SCHEME_MAILTO("mailto");
const String ContactsUtils::SCHEME_SMSTO("smsto");
const String ContactsUtils::TAG("ContactsUtils");
Int32 ContactsUtils::sThumbnailSize = -1;

String ContactsUtils::LookupProviderNameFromId(
    /* [in] */ Int32 protocol)
{
    assert(0);
    return String(NULL);
}

Boolean ContactsUtils::IsGraphic(
    /* [in] */ ICharSequence* str)
{
    assert(0);
    return FALSE;
}

Boolean ContactsUtils::AreObjectsEqual(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b)
{
    assert(0);
    return FALSE;
}

Boolean ContactsUtils::AreIntentActionEqual(
    /* [in] */ IIntent* a,
    /* [in] */ IIntent* b)
{
    assert(0);
    return FALSE;
}

Boolean ContactsUtils::AreContactWritableAccountsAvailable(
    /* [in] */ IContext* context)
{
    assert(0);
    return FALSE;
}

Boolean ContactsUtils::AreGroupWritableAccountsAvailable(
    /* [in] */ IContext* context)
{
    assert(0);
    return FALSE;
}

Int32 ContactsUtils::GetThumbnailSize(
    /* [in] */ IContext* context)
{
    assert(0);
    return 0;
}

AutoPtr<Pair<AutoPtr<IIntent>, AutoPtr<IIntent> > > ContactsUtils::BuildImIntent(
    /* [in] */ IContext* context,
    /* [in] */ ImDataItem* im)
{
    assert(0);
    return NULL;
}

AutoPtr<IIntent> ContactsUtils::GetCustomImIntent(
    /* [in] */ ImDataItem* im,
    /* [in] */ Int32 protocol)
{
    assert(0);
    return NULL;
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
