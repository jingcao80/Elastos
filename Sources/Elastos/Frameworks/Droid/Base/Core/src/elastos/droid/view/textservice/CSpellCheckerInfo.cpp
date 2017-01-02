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

#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/textservice/CSpellCheckerInfo.h"
#include "elastos/droid/view/textservice/CSpellCheckerSubtype.h"
#include "elastos/droid/R.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include "elastos/droid/content/pm/CServiceInfo.h"
#include "elastos/droid/utility/Xml.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::CServiceInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Xml;
using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              CSpellCheckerInfo::
//========================================================================================
CAR_INTERFACE_IMPL_2(CSpellCheckerInfo, Object, ISpellCheckerInfo, IParcelable)

CAR_OBJECT_IMPL(CSpellCheckerInfo)

CSpellCheckerInfo::CSpellCheckerInfo()
    : mId("")
    , mLabel(0)
    , mSettingsActivityName("")
{
    CArrayList::New((IArrayList**)&mSubtypes);
}

ECode CSpellCheckerInfo::constructor()
{
    return NOERROR;
}

ECode CSpellCheckerInfo::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service)
{
    mService = service;

    CResolveInfo* info = (CResolveInfo*)service;
    AutoPtr<IServiceInfo> si = info->mServiceInfo;
    AutoPtr<CServiceInfo> _si = (CServiceInfo*)si.Get();
    AutoPtr<IComponentName> name;
    CComponentName::New(_si->mPackageName, _si->mName, (IComponentName**)&name);
    name->FlattenToShortString(&mId);

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    Int32 label = 0;
    String settingsActivityComponent(NULL);

    AutoPtr<IXmlResourceParser> parser;
    IPackageItemInfo::Probe(si)->LoadXmlMetaData(pm, ISpellCheckerSession::SERVICE_META_DATA, (IXmlResourceParser**)&parser);
    if (parser == NULL) {
       // throw new XmlPullParserException("No "
       //         + SpellCheckerSession.SERVICE_META_DATA + " meta-data");
        Logger::E("CSpellCheckerInfo", "No %s meta-data", (const char*)ISpellCheckerSession::SERVICE_META_DATA);
    }

    AutoPtr<IResources> res;
    pm->GetResourcesForApplication(_si->mApplicationInfo, (IResources**)&res);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));
    Int32 type = 0;
    while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && type != IXmlPullParser::START_TAG) {
    }

    String nodeName;
    IXmlPullParser::Probe(parser)->GetName(&nodeName);
    if (!nodeName.Equals("spell-checker")) {
        Logger::E("CSpellCheckerInfo", "Meta-data does not start with spell-checker tag");
    }

    AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(2);
    (*arr)[0] = R::styleable::SpellChecker[0];
    (*arr)[1] = R::styleable::SpellChecker[1];
    AutoPtr<ITypedArray> sa;
    res->ObtainAttributes(attrs, arr, (ITypedArray**)&sa);
    sa->GetResourceId(R::styleable::SpellChecker_label, 0, &label);
    sa->GetString(R::styleable::SpellChecker_settingsActivity, &settingsActivityComponent);
    sa->Recycle();

    Int32 depth = 0, dp = 0;
    IXmlPullParser::Probe(parser)->GetDepth(&depth);
    // Parse all subtypes
    while (((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_TAG
            || (IXmlPullParser::Probe(parser)->GetDepth(&dp), dp) > depth)
            && type != IXmlPullParser::END_DOCUMENT) {
        if (type == IXmlPullParser::START_TAG) {
            String subtypeNodeName;
            IXmlPullParser::Probe(parser)->GetName(&subtypeNodeName);
            if (!subtypeNodeName.Equals("subtype")) {
                Logger::E("CSpellCheckerInfo", "Meta-data in spell-checker does not start with subtype tag");
            }
            AutoPtr<ArrayOf<Int32> > stArr = ArrayOf<Int32>::Alloc(3);
            (*stArr)[0] = R::styleable::SpellChecker_Subtype[0];
            (*stArr)[1] = R::styleable::SpellChecker_Subtype[1];
            (*stArr)[2] = R::styleable::SpellChecker_Subtype[2];
            AutoPtr<ITypedArray> a;
            res->ObtainAttributes(attrs, stArr, (ITypedArray**)&a);
            Int32 resID = 0;
            a->GetResourceId(R::styleable::SpellChecker_Subtype_label, 0, &resID);
            String strLoc, strVal;
            a->GetString(R::styleable::SpellChecker_Subtype_subtypeLocale, &strLoc);
            a->GetString(R::styleable::SpellChecker_Subtype_subtypeExtraValue, &strVal);
            AutoPtr<ISpellCheckerSubtype> subtype;
            CSpellCheckerSubtype::New(resID, strLoc, strVal, (ISpellCheckerSubtype**)&subtype);
            mSubtypes->Add(subtype);
        }
    }
    if (parser != NULL) parser->Close();
    mLabel = label;
    mSettingsActivityName = settingsActivityComponent;

    return NOERROR;
}

ECode CSpellCheckerInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&mLabel);
    parcel->ReadString(&mId);
    parcel->ReadString(&mSettingsActivityName);
    IParcelable::Probe(mService)->ReadFromParcel(parcel);
    parcel->ReadInterfacePtr((Handle32*)&mSubtypes);

    return NOERROR;
}

ECode CSpellCheckerInfo::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode CSpellCheckerInfo::GetComponent(
    /* [out] */ IComponentName** component)
{
    VALIDATE_NOT_NULL(component);
    AutoPtr<IServiceInfo> serviceInfo;
    String packageName, name;
    serviceInfo = ((CResolveInfo*)(mService.Get()))->mServiceInfo;
    packageName = ((CServiceInfo*)(serviceInfo.Get()))->mPackageName;
    name = ((CServiceInfo*)(serviceInfo.Get()))->mName;
    CComponentName::New(packageName, name, component);

    return NOERROR;
}

ECode CSpellCheckerInfo::GetPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    AutoPtr<IServiceInfo> serviceInfo;
    serviceInfo = ((CResolveInfo*)(mService.Get()))->mServiceInfo;
    *name = ((CServiceInfo*)(serviceInfo.Get()))->mPackageName;
    return NOERROR;
}

ECode CSpellCheckerInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mLabel);
    dest->WriteString(mId);
    dest->WriteString(mSettingsActivityName);
    IParcelable::Probe(mService)->WriteToParcel(dest);
    dest->WriteInterfacePtr(mSubtypes);

    return NOERROR;
}

ECode CSpellCheckerInfo::LoadLabel(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    if (mLabel == 0 || pm == NULL) {
        return CString::New(String(""), label);
    }

    AutoPtr<IServiceInfo> serviceInfo;
    serviceInfo = ((CResolveInfo*)(mService.Get()))->mServiceInfo;
    AutoPtr<IApplicationInfo> applicationInfo = ((CServiceInfo*)(serviceInfo.Get()))->mApplicationInfo;
    String packageName;
    GetPackageName(&packageName);

    return pm->GetText(packageName, mLabel, applicationInfo, label);
}

ECode CSpellCheckerInfo::LoadIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** drawable)
{
    return mService->LoadIcon(pm, drawable);
}

ECode CSpellCheckerInfo::GetServiceInfo(
    /* [out] */ IServiceInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = ((CResolveInfo*)(mService.Get()))->mServiceInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CSpellCheckerInfo::GetSettingsActivity(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mSettingsActivityName;
    return NOERROR;
}

ECode CSpellCheckerInfo::GetSubtypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mSubtypes->GetSize(count);
}

ECode CSpellCheckerInfo::GetSubtypeAt(
    /* [in] */ Int32 index,
    /* [out] */ ISpellCheckerSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);
    AutoPtr<IInterface> p;
    mSubtypes->Get(index, (IInterface**)&p);
    *subtype = ISpellCheckerSubtype::Probe(p);
    REFCOUNT_ADD(*subtype);
    return NOERROR;
}

} // namespace TextService
} // namespace View
} // namespace Droid
} // namespace Elastos
