
#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEINFLATER_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEINFLATER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/GenericInflater.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceInflater
    : public GenericInflater
    , public IPreferenceInflater
{
public:
    CAR_INTERFACE_DECL()

    PreferenceInflater();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPreferenceManager* preferenceManager);

    CARAPI constructor(
        /* [in] */ IGenericInflater* original,
        /* [in] */ IPreferenceManager* preferenceManager,
        /* [in] */ IContext* newContext);

    CARAPI CloneInContext(
        /* [in] */ IContext* newContext,
        /* [out] */ IGenericInflater** ret);

protected:
    CARAPI OnCreateCustomFromTag(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IInterface* parentPreference,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ Boolean* res);

    CARAPI OnMergeRoots(
        /* [in] */ IInterface* givenRoot,
        /* [in] */ Boolean attachToGivenRoot,
        /* [in] */ IInterface* xmlRoot,
        /* [out] */ IInterface** p);

private:
    CARAPI_(void) Init(
        /* [in] */ IPreferenceManager* preferenceManager);

private:
    static const String TAG;
    static const String INTENT_TAG_NAME;
    static const String EXTRA_TAG_NAME;

    AutoPtr<IPreferenceManager> mPreferenceManager;
};

} // Preference
} // Droid
} // Elastos

#endif // __CPREFERENCEINFLATER_H__
