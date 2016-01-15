
#include "elastos/droid/content/CIntentHelper.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CIntentHelper, Singleton, IIntentHelper)

CAR_SINGLETON_IMPL(CIntentHelper)

ECode CIntentHelper::CreateChooser(
    /* [in] */ IIntent* target,
    /* [in] */ ICharSequence* title,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> it = Intent::CreateChooser(target, title);
    *intent = it;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::MakeMainActivity(
    /* [in] */ IComponentName* mainActivity,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> it = Intent::MakeMainActivity(mainActivity);
    *intent = it;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::MakeMainSelectorActivity(
    /* [in] */ const String& selectorAction,
    /* [in] */ const String& selectorCategory,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> it = Intent::MakeMainSelectorActivity(selectorAction, selectorCategory);
    *intent = it;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::MakeRestartActivityTask(
    /* [in] */ IComponentName* mainActivity,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> it = Intent::MakeRestartActivityTask(mainActivity);
    *intent = it;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::GetIntent(
    /* [in] */ const String& uri,
    /* [out] */ IIntent** intent)
{
    return Intent::GetIntent(uri, intent);
}

ECode CIntentHelper::ParseUri(
    /* [in] */ const String& uri,
    /* [in] */ Int32 flags,
    /* [out] */ IIntent** intent)
{
    return Intent::ParseUri(uri, flags, intent);
}

ECode CIntentHelper::GetIntentOld(
    /* [in] */ const String& uri,
    /* [out] */ IIntent** intent)
{
    return Intent::GetIntentOld(uri, intent);
}

ECode CIntentHelper::ParseIntent(
    /* [in] */ IResources* resources,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IIntent** intent)
{
    return Intent::ParseIntent(resources, parser, attrs, intent);
}

ECode CIntentHelper::RestoreFromXml(
    /* [in] */ IXmlPullParser* in,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = Intent::RestoreFromXml(in);
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::NormalizeMimeType(
    /* [in] */ const String& type,
    /* [out] */ String* mimeType)
{
    VALIDATE_NOT_NULL(mimeType)
    *mimeType = Intent::NormalizeMimeType(type);
    return NOERROR;
}

}
}
}

