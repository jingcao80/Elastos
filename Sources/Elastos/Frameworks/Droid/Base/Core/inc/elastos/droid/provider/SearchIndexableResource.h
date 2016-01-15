#ifndef __ELASTOS_DROID_PROVIDER_SEARCHINDEXABLERESOURCE_H__
#define __ELASTOS_DROID_PROVIDER_SEARCHINDEXABLERESOURCE_H__

#include "elastos/droid/provider/SearchIndexableData.h"

namespace Elastos {
namespace Droid {
namespace Provider {

class SearchIndexableResource
    : public SearchIndexableData
    , public ISearchIndexableResource
{
public:
    SearchIndexableResource();

    virtual ~SearchIndexableResource();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ Int32 rank,
        /* [in] */ Int32 xmlResId,
        /* [in] */ const String& className,
        /* [in] */ Int32 iconResId);

    //@Override
    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI SetXmlResId(
        /* [in] */ Int32 xmlResId);

    CARAPI GetXmlResId(
        /* [out] */ Int32* xmlResId);

    //override
    CARAPI SetKey(
        /* [in] */ const String& key);

    //override
    CARAPI SetClassName(
        /* [in] */ const String& className);

    //override
    CARAPI SetIntentAction(
        /* [in] */ const String& intentAction);

    //override
    CARAPI SetIntentTargetPackage(
        /* [in] */ const String& intentTargetPackage);

    CARAPI GetIntentTargetClass(
        /* [out] */ String* intentTargetClass);

public:
    Int32 mXmlResId;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_SEARCHINDEXABLERESOURCE_H__