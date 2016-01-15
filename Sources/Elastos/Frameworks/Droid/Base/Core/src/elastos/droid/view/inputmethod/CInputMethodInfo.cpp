
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/pm/CServiceInfo.h"
#include "elastos/droid/content/pm/CApplicationInfo.h"
#include "elastos/droid/view/inputmethod/CInputMethodInfo.h"
#include "elastos/droid/view/inputmethod/CInputMethodSubtype.h"
#include "elastos/droid/view/inputmethod/CInputMethodSubtypeArray.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"

#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::CServiceInfo;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::InputMethod::CInputMethodSubtype;
using Elastos::Droid::View::InputMethod::CInputMethodSubtypeArray;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Xml;

using Elastos::Core::CString;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {
//========================================================================================
//              CInputMethodInfo::
//========================================================================================
const String CInputMethodInfo::TAG("InputMethodInfo");

CAR_INTERFACE_IMPL_2(CInputMethodInfo, Object, IInputMethodInfo, IParcelable)

CAR_OBJECT_IMPL(CInputMethodInfo)

CInputMethodInfo::CInputMethodInfo()
    : mIsDefaultResId(0)
    , mIsAuxIme(FALSE)
{}

ECode CInputMethodInfo::constructor()
{
    return NOERROR;
}

ECode CInputMethodInfo::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service)
{
    return Init(context, service, NULL);
}

ECode CInputMethodInfo::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [in] */ IMap* additionalSubtypesMap)
{
    return Init(context, service, additionalSubtypesMap);
}

ECode CInputMethodInfo::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className,
    /* [in] */ ICharSequence* label,
    /* [in] */ const String& settingsActivity)
{
    return constructor(BuildDummyResolveInfo(packageName, className, label), FALSE, settingsActivity, NULL,
                0, FALSE /* forceDefault */, TRUE /* supportsSwitchingToNextInputMethod */);
}

ECode CInputMethodInfo::constructor(
    /* [in] */ IResolveInfo* ri,
    /* [in] */ Boolean isAuxIme,
    /* [in] */ const String& settingsActivity,
    /* [in] */ IList* subtypes,
    /* [in] */ Int32 isDefaultResId,
    /* [in] */ Boolean forceDefault)
{
    return constructor(ri, isAuxIme, settingsActivity, subtypes, isDefaultResId,
            forceDefault, TRUE /* supportsSwitchingToNextInputMethod */);
}

ECode CInputMethodInfo::constructor(
    /* [in] */ IResolveInfo* ri,
    /* [in] */ Boolean isAuxIme,
    /* [in] */ const String& settingsActivity,
    /* [in] */ IList* subtypes,
    /* [in] */ Int32 isDefaultResId,
    /* [in] */ Boolean forceDefault,
    /* [in] */ Boolean supportsSwitchingToNextInputMethod)
{
    AutoPtr<CResolveInfo> _ri = (CResolveInfo*) ri;
    AutoPtr<IServiceInfo> si = _ri->mServiceInfo;
    AutoPtr<CServiceInfo> _si = (CServiceInfo*) si.Get();
    mService = (CResolveInfo*)ri;
    AutoPtr<IComponentName> cn;
    CComponentName::New(_si->mPackageName, _si->mName, (IComponentName**)&cn);
    cn->FlattenToShortString(&mId);
    mSettingsActivityName = settingsActivity;
    mIsDefaultResId = isDefaultResId;
    mIsAuxIme = isAuxIme;
    CInputMethodSubtypeArray::New(subtypes, (IInputMethodSubtypeArray**)&mSubtypes);
    mForceDefault = forceDefault;
    mSupportsSwitchingToNextInputMethod = supportsSwitchingToNextInputMethod;
    return NOERROR;
}

AutoPtr<IResolveInfo> CInputMethodInfo::BuildDummyResolveInfo(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className,
    /* [in] */ ICharSequence* label)
{
    AutoPtr<IResolveInfo> ri;
    CResolveInfo::New((IResolveInfo**)&ri);
    AutoPtr<CResolveInfo> _ri = (CResolveInfo*) ri.Get();
    AutoPtr<IServiceInfo> si;
    CServiceInfo::New((IServiceInfo**)&si);
    AutoPtr<CServiceInfo> _si = (CServiceInfo*) si.Get();
    AutoPtr<IApplicationInfo> ai;
    CApplicationInfo::New((IApplicationInfo**)&ai);
    AutoPtr<CApplicationInfo> _ai = (CApplicationInfo*) ai.Get();
    _ai->mPackageName = packageName;
    _ai->mEnabled = TRUE;
    _si->mApplicationInfo = ai;
    _si->mEnabled = TRUE;
    _si->mPackageName = packageName;
    _si->mName = className;
    _si->mExported = TRUE;
    _si->mNonLocalizedLabel = label;
    _ri->mServiceInfo = si;
    return ri;
}

ECode CInputMethodInfo::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    source->ReadString(&mId);
    source->ReadString(&mSettingsActivityName);
    source->ReadInt32(&mIsDefaultResId);
    source->ReadBoolean(&mIsAuxIme);
    Int32 i = 0;
    source->ReadInt32(&i);
    mSupportsSwitchingToNextInputMethod = i == 1;
    source->ReadInterfacePtr((Handle32*)&mService);

    CInputMethodSubtypeArray::New((IInputMethodSubtypeArray**)&mSubtypes);
    IParcelable::Probe(mSubtypes)->ReadFromParcel(source);
    mForceDefault = FALSE;

    return NOERROR;
}

ECode CInputMethodInfo::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    dest->WriteString(mId);
    dest->WriteString(mSettingsActivityName);
    dest->WriteInt32(mIsDefaultResId);
    dest->WriteBoolean(mIsAuxIme);
    dest->WriteInt32(mSupportsSwitchingToNextInputMethod ? 1 : 0);
    dest->WriteInterfacePtr((IInterface*)(IResolveInfo*)mService.Get());

    IParcelable::Probe(mSubtypes)->WriteToParcel(dest);

    return NOERROR;
}

ECode CInputMethodInfo::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode CInputMethodInfo::GetPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = ((CServiceInfo*)mService->mServiceInfo.Get())->mPackageName;
    return NOERROR;
}

ECode CInputMethodInfo::GetServiceName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = ((CServiceInfo*)mService->mServiceInfo.Get())->mName;
    return NOERROR;
}

ECode CInputMethodInfo::GetServiceInfo(
    /* [out] */ IServiceInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mService->mServiceInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CInputMethodInfo::GetComponent(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    return CComponentName::New(((CServiceInfo*)mService->mServiceInfo.Get())->mPackageName,
            ((CServiceInfo*)mService->mServiceInfo.Get())->mName, name);
}

ECode CInputMethodInfo::LoadLabel(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;
    return mService->LoadLabel(pm, str);
}

ECode CInputMethodInfo::LoadIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return mService->LoadIcon(pm, drawable);
}

ECode CInputMethodInfo::GetSettingsActivity(
    /* [out] */ String* settings)
{
    VALIDATE_NOT_NULL(settings);
    *settings = mSettingsActivityName;
    return NOERROR;
}

ECode CInputMethodInfo::GetSubtypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mSubtypes->GetCount(count);
}

ECode CInputMethodInfo::GetSubtypeAt(
    /* [in] */ Int32 index,
    /* [out] */ IInputMethodSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);
    return mSubtypes->Get(index, subtype);
}

ECode CInputMethodInfo::GetIsDefaultResourceId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mIsDefaultResId;
    return NOERROR;
}

ECode CInputMethodInfo::IsDefault(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mForceDefault) {
        *result = TRUE;
        return NOERROR;
    }
//    try {
    *result = FALSE;
    Int32 id = 0;
    FAIL_RETURN(GetIsDefaultResourceId(&id));
    if (id == 0) {
        *result = FALSE;
        return NOERROR;
    }
    String name;
    FAIL_RETURN(GetPackageName(&name));
    AutoPtr<IContext> cxt;
    FAIL_RETURN(context->CreatePackageContext(name, 0, (IContext**)&cxt));
    AutoPtr<IResources> res;
    FAIL_RETURN(cxt->GetResources((IResources**)&res));
    return res->GetBoolean(id, result);
//    } catch (NameNotFoundException | NotFoundException e) {
//     return false;
//    }
}

ECode CInputMethodInfo::IsAuxiliaryIme(
    /* [in] */ Boolean* auxIme)
{
    assert(auxIme != NULL);
    *auxIme = mIsAuxIme;
    return NOERROR;
}

ECode CInputMethodInfo::SupportsSwitchingToNextInputMethod(
    /* [out] */ Boolean* supports)
{
    VALIDATE_NOT_NULL(supports)

    *supports = mSupportsSwitchingToNextInputMethod;
    return NOERROR;
}

ECode CInputMethodInfo::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    *code = mId.GetHashCode();
    return NOERROR;
}

ECode CInputMethodInfo::Equals(
    /* [in] */ IInputMethodInfo* o,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal);
    *equal = FALSE;

    if (o == this) {
        *equal = TRUE;
        return NOERROR;
    }

    if (o == NULL) {
        return NOERROR;
    }

    String id;
    o->GetId(&id);
    *equal = mId.Equals(id);
    return NOERROR;
}

ECode CInputMethodInfo::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    return Equals(IInputMethodInfo::Probe(other), result);
}

ECode CInputMethodInfo::Init(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [in] */ IMap* additionalSubtypesMap)
{
    mService = (CResolveInfo*)service;
    CServiceInfo* si = (CServiceInfo*)mService->mServiceInfo.Get();
    AutoPtr<CComponentName> componentName;
    CComponentName::NewByFriend(si->mPackageName, si->mName, (CComponentName**)&componentName);
    componentName->FlattenToShortString(&mId);
    Boolean isAuxIme = TRUE;
    Boolean supportsSwitchingToNextInputMethod = FALSE; // false as default
    mForceDefault = FALSE;

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    String settingsActivityComponent;
    Int32 isDefaultResId = 0;

    AutoPtr<IXmlResourceParser> parser;
    AutoPtr<IArrayList> subtypes;
    CArrayList::New((IArrayList**)&subtypes);
    //try {
    si->LoadXmlMetaData(pm, IInputMethod::SERVICE_META_DATA, (IXmlResourceParser**)&parser);
    if (parser == NULL) {
        Logger::E(TAG, "No %s meta-data", (const char*)IInputMethod::SERVICE_META_DATA);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    AutoPtr<IResources> res;
    pm->GetResourcesForApplication(si->mApplicationInfo, (IResources**)&res);

    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));

    Int32 type = 0;
    while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && type != IXmlPullParser::START_TAG) {
    }
    String nodeName;
    IXmlPullParser::Probe(parser)->GetName(&nodeName);
    if (!nodeName.Equals("input-method")) {
        Logger::E(TAG, "Meta-data does not start with input-method tag");
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    Int32 size = ArraySize(R::styleable::InputMethod);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::InputMethod, size);

    AutoPtr<ITypedArray> sa;
    res->ObtainAttributes(attrs, layout, (ITypedArray**)&sa);
    sa->GetString(R::styleable::InputMethod_settingsActivity, &settingsActivityComponent);
    sa->GetResourceId(R::styleable::InputMethod_isDefault, 0, &isDefaultResId);
    sa->GetBoolean(R::styleable::InputMethod_supportsSwitchingToNextInputMethod,
                    FALSE, &supportsSwitchingToNextInputMethod);
    sa->Recycle();

    Int32 depth = 0, tmpDepth = 0;
    IXmlPullParser::Probe(parser)->GetDepth(&depth);
    // Parse all subtypes
    while (((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_TAG || (IXmlPullParser::Probe(parser)->GetDepth(&tmpDepth), tmpDepth) > depth)
            && type != IXmlPullParser::END_DOCUMENT) {
        if (type == IXmlPullParser::START_TAG) {
            IXmlPullParser::Probe(parser)->GetName(&nodeName);
            if (!nodeName.Equals("subtype")) {
                parser->Close();
                Logger::E(TAG, "Meta-data in input-method does not start with subtype tag");
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            size = ArraySize(R::styleable::InputMethod_Subtype);
            layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::InputMethod_Subtype, size);

            AutoPtr<ITypedArray> a;
            res->ObtainAttributes(attrs, layout, (ITypedArray**)&a);

            AutoPtr<IInputMethodSubtypeBuilder> builder = new CInputMethodSubtype::InputMethodSubtypeBuilder();
            Int32 label = 0;
            a->GetResourceId(R::styleable::InputMethod_Subtype_label, 0, &label);
            builder->SetSubtypeNameResId(label);

            Int32 icon = 0;
            a->GetResourceId(R::styleable::InputMethod_Subtype_icon, 0, &icon);
            builder->SetSubtypeIconResId(icon);

            String imeSubtypeLocale;
            a->GetString(R::styleable::InputMethod_Subtype_imeSubtypeLocale, &imeSubtypeLocale);
            builder->SetSubtypeLocale(imeSubtypeLocale);

            String imeSubtypeMode;
            a->GetString(R::styleable::InputMethod_Subtype_imeSubtypeMode, &imeSubtypeMode);
            builder->SetSubtypeMode(imeSubtypeMode);

            String imeSubtypeExtraValue;
            a->GetString(R::styleable::InputMethod_Subtype_imeSubtypeExtraValue, &imeSubtypeExtraValue);
            builder->SetSubtypeExtraValue(imeSubtypeExtraValue);

            Boolean isAuxiliary = FALSE;
            a->GetBoolean(R::styleable::InputMethod_Subtype_isAuxiliary, FALSE, &isAuxiliary);
            builder->SetIsAuxiliary(isAuxiliary);

            Boolean overridesImplicitlyEnabledSubtype = FALSE;
            a->GetBoolean(R::styleable::InputMethod_Subtype_overridesImplicitlyEnabledSubtype, FALSE, &overridesImplicitlyEnabledSubtype);
            builder->SetOverridesImplicitlyEnabledSubtype(overridesImplicitlyEnabledSubtype);

            Int32 subtypeId = 0;
            a->GetInt32(R::styleable::InputMethod_Subtype_subtypeId, 0 /* use Arrays.hashCode */, &subtypeId);
            builder->SetSubtypeId(subtypeId);

            Boolean isAsciiCapable = FALSE;
            a->GetBoolean(R::styleable::InputMethod_Subtype_isAsciiCapable, FALSE, &isAsciiCapable);
            builder->SetIsAsciiCapable(isAsciiCapable);

            AutoPtr<IInputMethodSubtype> subtype;
            builder->Build((IInputMethodSubtype**)&subtype);

            Boolean auxiliary = FALSE;
            if (!(subtype->IsAuxiliary(&auxiliary), auxiliary)) {
                isAuxIme = FALSE;
            }
            subtypes->Add(subtype);
        }
    }
    // } catch (NameNotFoundException e) {
    //     throw new XmlPullParserException(
    //             "Unable to create context for: " + si.packageName);
    // } finally {
    //     if (parser != null) parser.close();
    // }
    if (parser != NULL) {
        parser->Close();
    }

    Boolean bIsEmp = FALSE;
    if ((subtypes->IsEmpty(&bIsEmp), bIsEmp)) {
        isAuxIme = FALSE;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(mId, (ICharSequence**)&cs);
    Boolean contains = FALSE;
    if (additionalSubtypesMap != NULL && (additionalSubtypesMap->ContainsKey(cs, &contains), contains)) {
        AutoPtr<IList> additionalSubtypes;
        additionalSubtypesMap->Get(cs, (IInterface**)&additionalSubtypes);
        Int32 N = 0;
        additionalSubtypes->GetSize(&N);
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> node;
            additionalSubtypes->Get(i, (IInterface**)&node);
            AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(node);
            if (!(subtypes->Contains(subtype, &contains), contains)) {
                subtypes->Add(subtype);
            }
            else {
                String loc, mod;
                subtype->GetLocale(&loc);
                subtype->GetMode(&mod);
                Slogger::W(TAG, "Duplicated subtype definition found: %s, %s", (const char*)loc, (const char*)mod);
            }
        }
    }

    CInputMethodSubtypeArray::New(IList::Probe(subtypes), (IInputMethodSubtypeArray**)&mSubtypes);
    mSettingsActivityName = settingsActivityComponent;
    mIsDefaultResId = isDefaultResId;
    mIsAuxIme = isAuxIme;
    mSupportsSwitchingToNextInputMethod = supportsSwitchingToNextInputMethod;
    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
