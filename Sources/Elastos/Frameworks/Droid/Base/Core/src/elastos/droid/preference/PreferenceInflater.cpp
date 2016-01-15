
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/preference/CPreferenceInflater.h"
#include "elastos/droid/preference/PreferenceInflater.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Intent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Preference {

const String PreferenceInflater::TAG("PreferenceInflater");
const String PreferenceInflater::INTENT_TAG_NAME("intent");
const String PreferenceInflater::EXTRA_TAG_NAME("extra");

CAR_INTERFACE_IMPL(PreferenceInflater, GenericInflater, IPreferenceInflater)

PreferenceInflater::PreferenceInflater()
{
}

ECode PreferenceInflater::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPreferenceManager* preferenceManager)
{
    mContext = context;
    SetFactorySet(FALSE);
    AutoPtr<ArrayOf<IInterface*> > ca = ArrayOf<IInterface*>::Alloc(2);
    SetConstructorArgs(ca);
    Init(preferenceManager);
    return NOERROR;
}

ECode PreferenceInflater::constructor(
    /* [in] */ IGenericInflater* original,
    /* [in] */ IPreferenceManager* preferenceManager,
    /* [in] */ IContext* newContext)
{
    mContext = newContext;
    SetFactorySet(FALSE);
    AutoPtr<ArrayOf<IInterface*> > ca = ArrayOf<IInterface*>::Alloc(2);
    SetConstructorArgs(ca);
    AutoPtr<IGenericInflaterFactory> factory;
    original->GetFactory((IGenericInflaterFactory**)&factory);
    original->SetFactory(factory);
    Init(preferenceManager);
    return NOERROR;
}

ECode PreferenceInflater::CloneInContext(
    /* [in] */ IContext* newContext,
    /* [out] */ IGenericInflater** ret)
{
    VALIDATE_NOT_NULL(ret)
    CPreferenceInflater::New(this, mPreferenceManager, newContext, ret);
    return NOERROR;
}

void PreferenceInflater::Init(
    /* [in] */ IPreferenceManager* preferenceManager)
{
    mPreferenceManager = preferenceManager;
    SetDefaultPackage(String("Elastos.Droid.Preference."));
}

ECode PreferenceInflater::OnCreateCustomFromTag(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IInterface* _parentPreference,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;

    AutoPtr<IPreference> parentPreference = IPreference::Probe(_parentPreference);
    assert(parentPreference != NULL);
    String tag;
    parser->GetName(&tag);
    if (tag.Equals(INTENT_TAG_NAME)) {
        AutoPtr<IIntent> intent;

        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IResources> resource;
        ctx->GetResources((IResources**)&resource);
        if (Intent::ParseIntent(resource, parser, attrs, (IIntent**)&intent) == (ECode)E_IO_EXCEPTION) {
            Slogger::D("PreferenceInflater", "Error parsing preference");
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        if (intent != NULL) {
            parentPreference->SetIntent(intent);
        }

        *res = TRUE;
        return NOERROR;
    }
    else if (tag.Equals(EXTRA_TAG_NAME)) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IResources> resource;
        ctx->GetResources((IResources**)&resource);
        AutoPtr<IBundle> data;
        parentPreference->GetExtras((IBundle**)&data);
        resource->ParseBundleExtra(EXTRA_TAG_NAME, attrs, data);
        if (XmlUtils::SkipCurrentTag(parser) == (ECode)E_IO_EXCEPTION) {
            Slogger::D("PreferenceInflater", "Error parsing preference");
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        *res = TRUE;
        return NOERROR;
    }

    return NOERROR;
}

ECode PreferenceInflater::OnMergeRoots(
    /* [in] */ IInterface* givenRoot,
    /* [in] */ Boolean attachToGivenRoot,
    /* [in] */ IInterface* xmlRoot,
    /* [out] */ IInterface** p)
{
    VALIDATE_NOT_NULL(p)
    // If we were given a Preferences, use it as the root (ignoring the root
    // Preferences from the XML file).
    if (givenRoot == NULL) {
        AutoPtr<IPreference> pg = IPreference::Probe(xmlRoot);
        if (pg != NULL) pg->OnAttachedToHierarchy(mPreferenceManager);
        *p = xmlRoot;
    }
    else {
        *p = givenRoot;
    }
    REFCOUNT_ADD(*p)
    return NOERROR;
}

} // Preference
} // Droid
} // Elastos
