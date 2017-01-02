//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/app/admin/CDeviceAdminInfo.h"
#include "elastos/droid/app/admin/CDeviceAdminInfoPolicyInfo.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::IAttributeSet;


namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

const String TAG("CDeviceAdminInfo");

CAR_INTERFACE_IMPL_2(CDeviceAdminInfo, Object, IDeviceAdminInfo, IParcelable)

CAR_OBJECT_IMPL(CDeviceAdminInfo)

AutoPtr<IDeviceAdminInfoPolicyInfo> CreatePolicyInfo(
    /* [in] */ Int32 identIn,
    /* [in] */ const char* tagIn,
    /* [in] */ Int32 labelIn,
    /* [in] */ Int32 descriptionIn)
{
    AutoPtr<IDeviceAdminInfoPolicyInfo> info;
    CDeviceAdminInfoPolicyInfo::New(identIn, String(tagIn), labelIn, descriptionIn,
            (IDeviceAdminInfoPolicyInfo**)&info);
    return info;
}

List<AutoPtr<IDeviceAdminInfoPolicyInfo> > CDeviceAdminInfo::sPoliciesDisplayOrder;
HashMap<String, AutoPtr<IInteger32> > CDeviceAdminInfo::sKnownPolicies;
AutoPtr<SparseArray> CDeviceAdminInfo::sRevKnownPolicies = new SparseArray(10);

Boolean CDeviceAdminInfo::sIsInited = CDeviceAdminInfo::Init();

Boolean CDeviceAdminInfo::Init()
{
    if (!sIsInited) {
        sIsInited = TRUE;

        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(USES_POLICY_WIPE_DATA, "wipe-data",
                Elastos::Droid::R::string::policylab_wipeData,
                Elastos::Droid::R::string::policydesc_wipeData));
        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(USES_POLICY_RESET_PASSWORD, "reset-password",
                Elastos::Droid::R::string::policylab_resetPassword,
                Elastos::Droid::R::string::policydesc_resetPassword));
        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(USES_POLICY_LIMIT_PASSWORD, "limit-password",
                Elastos::Droid::R::string::policylab_limitPassword,
                Elastos::Droid::R::string::policydesc_limitPassword));
        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(USES_POLICY_WATCH_LOGIN, "watch-login",
                Elastos::Droid::R::string::policylab_watchLogin,
                Elastos::Droid::R::string::policydesc_watchLogin));
        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(USES_POLICY_FORCE_LOCK, "force-lock",
                Elastos::Droid::R::string::policylab_forceLock,
                Elastos::Droid::R::string::policydesc_forceLock));
        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(USES_POLICY_SETS_GLOBAL_PROXY, "set-global-proxy",
                Elastos::Droid::R::string::policylab_setGlobalProxy,
                Elastos::Droid::R::string::policydesc_setGlobalProxy));
        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(USES_POLICY_EXPIRE_PASSWORD, "expire-password",
                Elastos::Droid::R::string::policylab_expirePassword,
                Elastos::Droid::R::string::policydesc_expirePassword));
        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(USES_ENCRYPTED_STORAGE, "encrypted-storage",
                Elastos::Droid::R::string::policylab_encryptedStorage,
                Elastos::Droid::R::string::policydesc_encryptedStorage));
        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(USES_POLICY_DISABLE_CAMERA, "disable-camera",
                Elastos::Droid::R::string::policylab_disableCamera,
                Elastos::Droid::R::string::policydesc_disableCamera));
        sPoliciesDisplayOrder.PushBack(CreatePolicyInfo(
                USES_POLICY_DISABLE_KEYGUARD_FEATURES, "disable-keyguard-features",
                Elastos::Droid::R::string::policylab_disableKeyguardFeatures,
                Elastos::Droid::R::string::policydesc_disableKeyguardFeatures));

        List<AutoPtr<IDeviceAdminInfoPolicyInfo> >::Iterator it;
        for (it = sPoliciesDisplayOrder.Begin(); it != sPoliciesDisplayOrder.End(); ++it) {
            AutoPtr<IDeviceAdminInfoPolicyInfo> pi = *it;
            Int32 ident = 0;
            pi->GetIdent(&ident);
            String tag;
            pi->GetTag(&tag);
            sRevKnownPolicies->Put(ident, pi);
            AutoPtr<IInteger32> identValue;
            CInteger32::New(ident, (IInteger32**)&identValue);
            sKnownPolicies[tag] = identValue;
        }
    }

    return TRUE;
}

CDeviceAdminInfo::CDeviceAdminInfo()
    : mVisible(FALSE)
    , mUsesPolicies(0)
{
}

ECode CDeviceAdminInfo::constructor(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode CDeviceAdminInfo::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* receiver)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(receiver);

    mReceiver = receiver;

    AutoPtr<IActivityInfo> ai;
    FAIL_RETURN(mReceiver->GetActivityInfo((IActivityInfo**)&ai));

    AutoPtr<IPackageManager> pm;
    FAIL_RETURN(context->GetPackageManager((IPackageManager**)&pm));

    StringBuilder sb;
    AutoPtr<IApplicationInfo> appInfo;
    AutoPtr<IResources> res;
    AutoPtr<IAttributeSet> attrs;
    Int32 type;
    String nodeName;
    Int32 szDeviceAdmin;
    AutoPtr<ArrayOf<Int32> > arDeviceAdmin;
    Int32 outerDepth, tempDepth, innerDepth;
    String tagName, policyName;
    AutoPtr<ITypedArray> sa;
    AutoPtr<IInteger32> val;
    AutoPtr<IComponentName> cn;
    String cnName;

    AutoPtr<IXmlResourceParser> parser;
    ECode ec = ai->LoadXmlMetaData(pm,
            IDeviceAdminReceiver::DEVICE_ADMIN_META_DATA,
            (IXmlResourceParser**)&parser);
    if (FAILED(ec))  goto EXIT;

    if (parser == NULL) {
        sb += "XmlPullParserException: No ";
        sb += IDeviceAdminReceiver::DEVICE_ADMIN_META_DATA;
        sb += " meta-data";
        Logger::E(TAG, sb.ToString());
        return E_XML_PULL_PARSER_EXCEPTION;
    }


    ec = ai->GetApplicationInfo((IApplicationInfo**)&appInfo);
    if (FAILED(ec)) goto EXIT;

    ec = pm->GetResourcesForApplication(appInfo, (IResources**)&res);
    if (FAILED(ec)) goto EXIT;

    attrs = Xml::AsAttributeSet(parser);

    while ((ec == parser->Next(&type) && !FAILED(ec))
            && type != IXmlPullParser::END_DOCUMENT
            && type != IXmlPullParser::START_TAG) {
        // nothing
    }
    if (FAILED(ec)) goto EXIT;

    ec = parser->GetName(&nodeName);
    if (FAILED(ec))  goto EXIT;

    if (nodeName != "device-admin") {
        Logger::E(TAG, "XmlPullParserException:Meta-data does not start with device-admin tag");
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    szDeviceAdmin = sizeof(Elastos::Droid::R::styleable::DeviceAdmin);
    arDeviceAdmin = ArrayOf<Int32>::Alloc(szDeviceAdmin);
    arDeviceAdmin->Copy(Elastos::Droid::R::styleable::DeviceAdmin, szDeviceAdmin);


    ec = res->ObtainAttributes(attrs, arDeviceAdmin, (ITypedArray**)&sa);
    if (FAILED(ec)) goto EXIT;

    sa->GetBoolean(Elastos::Droid::R::styleable::DeviceAdmin_visible, TRUE, &mVisible);
    sa->Recycle();

    ec = parser->GetDepth(&outerDepth);
    if (FAILED(ec)) goto EXIT;

    while ((ec == parser->Next(&type) && !FAILED(ec))
            && type != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG ||
                    ((ec == parser->GetDepth(&tempDepth)) && !FAILED(ec)
                            && tempDepth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        parser->GetName(&tagName);
        if (tagName.Equals("uses-policies")) {
             parser->GetDepth(&innerDepth);

             while ((ec == parser->Next(&type) && !FAILED(ec))
                     && type != IXmlPullParser::END_DOCUMENT
                     && (type != IXmlPullParser::END_TAG ||
                             ((ec == parser->GetDepth(&tempDepth)) && !FAILED(ec)
                                     && tempDepth > innerDepth))) {
                 if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                     continue;
                 }

                 ec = parser->GetName(&policyName);
                 if (FAILED(ec)) goto EXIT;

                 HashMap<String, AutoPtr<IInteger32> >::Iterator fit = sKnownPolicies.Find(policyName);
                 if (fit != sKnownPolicies.End()) val = fit->mSecond;
                 if (val != NULL) {
                     Int32 intVal = 0;
                     val->Int32Value(&intVal);
                     mUsesPolicies |= 1 << intVal;
                 }
                 else {
                     cn = NULL;
                     GetComponent((IComponentName**)&cn);
                     cn->ToString(&cnName);

                     sb.Reset();
                     sb += "Unknown tag under uses-policies of ";
                     sb += cnName;
                     sb += ": ";
                     sb += policyName;
                     Logger::W(TAG, sb.ToString());
                 }
             }
             if (FAILED(ec)) goto EXIT;
        }
    }

EXIT:
    if (parser) {
        parser->Close();
    }
    return ec;
}

ECode CDeviceAdminInfo::GetPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;

    AutoPtr<IActivityInfo> ai;
    FAIL_RETURN(mReceiver->GetActivityInfo((IActivityInfo**)&ai));
    return ai->GetPackageName(name);
}

ECode CDeviceAdminInfo::GetReceiverName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;

    AutoPtr<IActivityInfo> ai;
    FAIL_RETURN(mReceiver->GetActivityInfo((IActivityInfo**)&ai));
    return ai->GetName(name);
}

ECode CDeviceAdminInfo::GetActivityInfo(
    /* [out] */ IActivityInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return mReceiver->GetActivityInfo(info);
}

ECode CDeviceAdminInfo::GetComponent(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;

    AutoPtr<IActivityInfo> ai;
    FAIL_RETURN(mReceiver->GetActivityInfo((IActivityInfo**)&ai));

    String packageName, recvName;
    FAIL_RETURN(ai->GetPackageName(&packageName));
    FAIL_RETURN(ai->GetName(&recvName));
    return CComponentName::New(packageName, recvName, name);
}

ECode CDeviceAdminInfo::LoadLabel(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** label)
{
    return mReceiver->LoadLabel(pm, label);
}

ECode CDeviceAdminInfo::LoadDescription(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(pm);
    VALIDATE_NOT_NULL(description);
    *description = NULL;

    AutoPtr<IActivityInfo> ai;
    FAIL_RETURN(mReceiver->GetActivityInfo((IActivityInfo**)&ai));
    Int32 desRes = 0;
    ai->GetDescriptionRes(&desRes);
    if (desRes != 0) {
        String packageName;
        mReceiver->GetResolvePackageName(&packageName);
        AutoPtr<IApplicationInfo> applicationInfo;
        if (packageName.IsNull()) {
            ai->GetPackageName(&packageName);
            ai->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        }

        return pm->GetText(packageName, desRes, applicationInfo, description);
    }

    return E_NOT_FOUND_EXCEPTION;
}

ECode CDeviceAdminInfo::LoadIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    return mReceiver->LoadIcon(pm, icon);
}

ECode CDeviceAdminInfo::IsVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = mVisible;
    return NOERROR;
}

ECode CDeviceAdminInfo::UsesPolicy(
    /* [in] */ Int32 policyIdent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mUsesPolicies & (1<<policyIdent)) != 0;
    return NOERROR;
}

ECode CDeviceAdminInfo::GetTagForPolicy(
    /* [in] */ Int32 policyIdent,
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = NULL;

    AutoPtr<IInterface> item = sRevKnownPolicies->Get(policyIdent);
    AutoPtr<IDeviceAdminInfoPolicyInfo> policyInfo = IDeviceAdminInfoPolicyInfo::Probe(item);
    if (policyInfo != NULL) {
        policyInfo->GetTag(tag);
    }
    return NOERROR;
}

ECode CDeviceAdminInfo::GetUsedPolicies(
    /* [out] */ IObjectContainer** policies)
{
    VALIDATE_NOT_NULL(policies);
    *policies = NULL;
    CObjectContainer::New(policies);

    List<AutoPtr<IDeviceAdminInfoPolicyInfo> >::Iterator it;
    for (it = sPoliciesDisplayOrder.Begin(); it != sPoliciesDisplayOrder.End(); ++it) {
        AutoPtr<IDeviceAdminInfoPolicyInfo> pi = *it;
        Int32 ident = 0;
        pi->GetIdent(&ident);
        Boolean isUsesPolicy = FALSE;
        if (!FAILED(UsesPolicy(ident, &isUsesPolicy)) && isUsesPolicy) {
            (*policies)->Add(pi);
        }
    }
    return NOERROR;
}

ECode CDeviceAdminInfo::WritePoliciesToXml(
    /* [in] */ IXmlSerializer* out)
{
    VALIDATE_NOT_NULL(out);
    String value = StringUtils::ToString(mUsesPolicies);
    String nullStr;
    return out->WriteAttribute(nullStr, String("flags"), value);
}

ECode CDeviceAdminInfo::ReadPoliciesFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    VALIDATE_NOT_NULL(parser);

    String strValue;
    parser->GetAttributeValue(String(NULL), String("flags"), &strValue);
    mUsesPolicies = StringUtils::ParseInt32(strValue);
    return NOERROR;
}

ECode CDeviceAdminInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    VALIDATE_NOT_NULL(pw);
    StringBuilder sb(prefix);
    sb.Append("Receiver:");
    FAIL_RETURN(pw->Println(sb.ToString()));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("  ");
    return mReceiver->Dump(pw, sb.ToString());
}

ECode CDeviceAdminInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    mReceiver = NULL;
    CResolveInfo::New(IResolveInfo::Probe(parcel), (IResolveInfo**)&mReceiver);
    parcel->ReadInt32(&mUsesPolicies);
    return NOERROR;
}

ECode CDeviceAdminInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    FAIL_RETURN(IParcelable::Probe(mReceiver)->WriteToParcel(dest));
    FAIL_RETURN(dest->WriteInt32(mUsesPolicies));
    return NOERROR;
}

ECode CDeviceAdminInfo::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);
    *string = NULL;

    AutoPtr<IActivityInfo> ai;
    FAIL_RETURN(mReceiver->GetActivityInfo((IActivityInfo**)&ai));
    String name;
    FAIL_RETURN(ai->GetName(&name));

    StringBuilder sb("DeviceAdminInfo{");
    sb.Append(name);
    sb.Append("}");
    return sb.ToString(string);
}

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos
