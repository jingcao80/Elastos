
#include "ContactUpdateUtils.h"

using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Provider::IContactsContractData;
using Elastos::Droid::Provider::CContactsContractData;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Database {

const String ContactUpdateUtils::TAG("ContactUpdateUtils");

void ContactUpdateUtils::SetSuperPrimary(
    /* [in] */ IContext* context,
    /* [in] */ Int64 dataId)
{
    if (dataId == -1) {
        Logger::E(TAG, "Invalid arguments for setSuperPrimary request");
        return;
    }

    // Update the primary values in the data record.
    AutoPtr<IContentValues> values;
    CContentValues::New(2, (IContentValues**)&values);
    values->Put(IContactsContractDataColumns::IS_SUPER_PRIMARY, 1);
    values->Put(IContactsContractDataColumns::IS_PRIMARY, 1);


    AutoPtr<IContactsContractData> data;
    CContactsContractData::New((IContactsContractData**)&data);
    AutoPtr<IUri> uri;
    data->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    AutoPtr<IUri> newUri;
    contentUris->WithAppendedId(uri, dataId, (IUri**)&newUri);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->Update(newUri, values, NULL, NULL);
}

} // Database
} // Common
} // Contacts
} // Apps
} // Elastos
