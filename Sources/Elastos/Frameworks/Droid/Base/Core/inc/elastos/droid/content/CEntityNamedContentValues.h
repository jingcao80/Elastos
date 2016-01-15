
#ifndef __ELASTOS_DROID_CONTENT_CENTITYNAMEDCONTENTVALUES_H__
#define __ELASTOS_DROID_CONTENT_CENTITYNAMEDCONTENTVALUES_H__

#include "_Elastos_Droid_Content_CEntityNamedContentValues.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContentValues;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CEntityNamedContentValues)
    , public Object
    , public IEntityNamedContentValues
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CEntityNamedContentValues();

    virtual ~CEntityNamedContentValues();

    CARAPI constructor(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values);

    CARAPI GetUri(
        /* [out] */ IUri** uri);

    CARAPI GetValues(
        /* [out] */ IContentValues** values);

private:
    AutoPtr<IUri> mUri;
    AutoPtr<IContentValues> mContentValues;

};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_CENTITYNAMEDCONTENTVALUES_H__
