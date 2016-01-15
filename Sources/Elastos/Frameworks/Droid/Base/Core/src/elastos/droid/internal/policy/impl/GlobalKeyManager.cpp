
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/internal/policy/impl/GlobalKeyManager.h"
#include "elastos/droid/content/CComponentNameHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/view/CKeyEventHelper.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                           GlobalKeyManager
//=====================================================================
const String GlobalKeyManager::TAG("GlobalKeyManager");
const String GlobalKeyManager::TAG_GLOBAL_KEYS("global_keys");
const String GlobalKeyManager::ATTR_VERSION("version");
const String GlobalKeyManager::TAG_KEY("key");
const String GlobalKeyManager::ATTR_KEY_CODE("keyCode");
const String GlobalKeyManager::ATTR_COMPONENT("component");
const Int32 GlobalKeyManager::GLOBAL_KEY_FILE_VERSION;

GlobalKeyManager::GlobalKeyManager(
    /* [in] */ IContext* context)
{
    CSparseArray::New((ISparseArray**)&mKeyMapping);
    LoadGlobalKeys(context);
}

ECode GlobalKeyManager::HandleGlobalKey(
    /* [in] */ IContext* context,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mKeyMapping->GetSize(&size);
    if (size > 0)
    {
        AutoPtr<IInterface> iif;
        mKeyMapping->Get(keyCode, (IInterface**)&iif);
        IComponentName* component = IComponentName::Probe(iif);
        if (component != NULL) {
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_GLOBAL_BUTTON, (IIntent**)&intent);
            intent->SetComponent(component);
            IParcelable* eParcelable = IParcelable::Probe(event);
            intent->PutExtra(IIntent::EXTRA_KEY_EVENT, eParcelable);
            AutoPtr<IUserHandle> userHandle;
            AutoPtr<IUserHandleHelper> userHandleHelper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
            userHandleHelper->GetCURRENT((IUserHandle**)&userHandle);
            context->SendBroadcastAsUser(intent, userHandle, String(NULL));
            *result = TRUE;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode GlobalKeyManager::ShouldHandleGlobalKey(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> iif;
    mKeyMapping->Get(keyCode, (IInterface**)&iif);
    *result = (iif != NULL);
    return NOERROR;
}

ECode GlobalKeyManager::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
    VALIDATE_NOT_NULL(pw);
    Int32 numKeys;
    mKeyMapping->GetSize(&numKeys);
    if (numKeys == 0) {
        pw->Print(prefix);
        pw->Println(String("mKeyMapping.size=0"));
        return NOERROR;
    }
    pw->Print(prefix);
    pw->Println(String("mKeyMapping={"));
    for (Int32 i = 0; i < numKeys; ++i)
    {
        pw->Print(String("  "));
        pw->Print(prefix);
        Int32 keyValue;
        mKeyMapping->KeyAt(i, &keyValue);
        AutoPtr<IKeyEventHelper> keyEventHelper;
        CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&keyEventHelper);
        String keyCodeStr;
        keyEventHelper->KeyCodeToString(keyValue, &keyCodeStr);
        pw->Print(keyCodeStr);
        pw->Print(String("="));
        AutoPtr<IInterface> value;
        mKeyMapping->ValueAt(i, (IInterface**)&value);
        IComponentName* cn = IComponentName::Probe(value);
        String valueStr;
        cn->FlattenToString(&valueStr);
        pw->Println(valueStr);
    }
    pw->Print(prefix);
    pw->Println(String("}"));
    return NOERROR;
}

void GlobalKeyManager::LoadGlobalKeys(
    /* [in] */ IContext* context)
{
    AutoPtr<IXmlResourceParser> parser;
    //try {
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetXml(R::xml::global_keys, (IXmlResourceParser**)&parser);

    IXmlPullParser* xmlPullParser = IXmlPullParser::Probe(parser);
    XmlUtils::BeginDocument(xmlPullParser, TAG_GLOBAL_KEYS);
    Int32 version;
    IAttributeSet* attributeSet = IAttributeSet::Probe(parser);
    attributeSet->GetAttributeIntValue(String("NULL"), ATTR_VERSION, 0, &version);
    if (GLOBAL_KEY_FILE_VERSION == version) {
        AutoPtr<IKeyEventHelper> keyEventHelper;
        CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&keyEventHelper);
        AutoPtr<IComponentNameHelper> cnHelper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
        while (TRUE) {
            XmlUtils::NextElement(xmlPullParser);
            String element;
            xmlPullParser->GetName(&element);
            if (element.IsNullOrEmpty()) {
                break;
            }
            if (TAG_KEY.Equals(element)) {
                String keyCodeName;
                attributeSet->GetAttributeValue(String(NULL), ATTR_KEY_CODE, &keyCodeName);
                String componentName;
                attributeSet->GetAttributeValue(String(NULL), ATTR_COMPONENT, &componentName);
                Int32 keyCode;
                keyEventHelper->KeyCodeFromString(keyCodeName, &keyCode);
                if (keyCode != IKeyEvent::KEYCODE_UNKNOWN) {
                    AutoPtr<IComponentName> cn;
                    cnHelper->UnflattenFromString(componentName, (IComponentName**)&cn);
                    mKeyMapping->Put(keyCode, cn);
                }
            }
        }
    }
    //} catch (Resources.NotFoundException e) {
    //    Log.w(TAG, "global keys file not found", e);
    //} catch (XmlPullParserException e) {
    //    Log.w(TAG, "XML parser exception reading global keys file", e);
    //} catch (IOException e) {
    //    Log.w(TAG, "I/O exception reading global keys file", e);
    //} finally {
    if (parser != NULL) {
        ICloseable* c = ICloseable::Probe(parser);
        c->Close();
    }
    //}
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos


