
#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERRESULT_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERRESULT_H__

#include "_Elastos_Droid_Content_CContentProviderResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderResult)
    , public Object
    , public IContentProviderResult
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentProviderResult();

    virtual ~CContentProviderResult();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUri* uri);

    CARAPI constructor(
        /* [in] */ Int32 count);

    CARAPI constructor(
        /* [in] */ IContentProviderResult* cpr,
        /* [in] */ Int32 userId);

    CARAPI GetUri(
        /* [out] */ IUri** uri);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI SetUri(
        /* [in] */ IUri* uri);

    CARAPI SetCount(
        /* [in] */ Int32 count);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
private:
    AutoPtr<IUri> mUri;
    Int32 mCount;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERRESULT_H__
