#ifndef __ELASTOS_DROID_SEARCHINDEXABLEDATA_H__
#define __ELASTOS_DROID_SEARCHINDEXABLEDATA_H__

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Provider {

class SearchIndexableData
    : public Object
    , public ISearchIndexableData
{
public:
    SearchIndexableData();

    virtual ~SearchIndexableData();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI SetContext(
        /* [in] */ IContext* context);

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI SetLocale(
        /* [in] */ ILocale* locale);

    CARAPI GetLocale(
        /* [out] */ ILocale** locale);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetRank(
        /* [in] */ Int32 rank);

    CARAPI GetRank(
        /* [out] */ Int32* rank);

    CARAPI SetKey(
        /* [in] */ const String& key);

    CARAPI GetKey(
        /* [out] */ String* key);

    CARAPI SetUserId(
        /* [in] */ Int32 userId);

    CARAPI GetUserId(
        /* [out] */ Int32* userId);

    CARAPI SetClassName(
        /* [in] */ const String& className);

    CARAPI GetClassName(
        /* [out] */ String* className);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetIconResId(
        /* [in] */ Int32 iconResId);

    CARAPI GetIconResId(
        /* [out] */ Int32* iconResId);

    CARAPI SetIntentAction(
        /* [in] */ const String& intentAction);

    CARAPI GetIntentAction(
        /* [out] */ String* intentAction);

    CARAPI SetIntentTargetPackage(
        /* [in] */ const String& intentTargetPackage);

    CARAPI GetIntentTargetPackage(
        /* [out] */ String* intentTargetPackage);

    CARAPI SetIntentTargetClass(
        /* [in] */ const String& intentTargetClass);

    CARAPI GetIntentTargetClass(
        /* [out] */ String* intentTargetClass);

    //@Override
    CARAPI ToString(
        /* [out] */ String* info);

public:
    AutoPtr<IContext> mContext;
    AutoPtr<ILocale> mLocale;
    Boolean mEnabled;
    Int32 mRank;
    String mKey;
    Int32 mUserId;
    String mClassName;
    String mPackageName;
    Int32 mIConResId;
    String mIntentAction;
    String mIntentTargetPackage;
    String mIntentTargetClass;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SEARCHINDEXABLEDATA_H__
