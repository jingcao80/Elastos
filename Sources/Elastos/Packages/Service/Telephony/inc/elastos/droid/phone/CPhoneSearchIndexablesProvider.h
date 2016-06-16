#ifndef  __ELASTOS_DROID_PHONE_CPHONESEARCHINDEXABLESPROVIDER_H__
#define  __ELASTOS_DROID_PHONE_CPHONESEARCHINDEXABLESPROVIDER_H__

#include "_Elastos_Droid_Phone_CPhoneSearchIndexablesProvider.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CPhoneSearchIndexablesProvider)
    , public SearchIndexablesProvider
    , public IPhoneSearchIndexablesProvider
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

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
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CPHONESEARCHINDEXABLESPROVIDER_H__