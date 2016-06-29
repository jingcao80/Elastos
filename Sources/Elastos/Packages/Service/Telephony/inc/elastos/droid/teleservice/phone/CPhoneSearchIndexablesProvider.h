#ifndef  __ELASTOS_DROID_PHONE_CPHONESEARCHINDEXABLESPROVIDER_H__
#define  __ELASTOS_DROID_PHONE_CPHONESEARCHINDEXABLESPROVIDER_H__

#include "_Elastos_Droid_TeleService_Phone_CPhoneSearchIndexablesProvider.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/provider/SearchIndexablesProvider.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::SearchIndexablesProvider;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CPhoneSearchIndexablesProvider)
    , public SearchIndexablesProvider
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI QueryXmlResources(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

    //@Override
    CARAPI QueryRawData(
       /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

    //@Override
    CARAPI QueryNonIndexableKeys(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

private:
    static const String TAG;

    static AutoPtr<ArrayOf<ISearchIndexableResource*> > INDEXABLE_RES;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CPHONESEARCHINDEXABLESPROVIDER_H__