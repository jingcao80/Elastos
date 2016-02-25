
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/inputmethod/CInputMethodSubtype.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Text::TextUtils;

using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Utility::Arrays;
using Elastos::Utility::CLocale;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CInputMethodSubtype::InputMethodSubtypeBuilder::
//========================================================================================
CAR_INTERFACE_IMPL(CInputMethodSubtype::InputMethodSubtypeBuilder, Object, IInputMethodSubtypeBuilder)

CInputMethodSubtype::InputMethodSubtypeBuilder::InputMethodSubtypeBuilder()
    : mIsAuxiliary(FALSE)
    , mOverridesImplicitlyEnabledSubtype(FALSE)
    , mIsAsciiCapable(FALSE)
    , mSubtypeIconResId(0)
    , mSubtypeNameResId(0)
    , mSubtypeId(0)
    , mSubtypeLocale("")
    , mSubtypeMode("")
    , mSubtypeExtraValue("")
{
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::constructor()
{
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::SetIsAuxiliary(
    /* [in] */ Boolean isAuxiliary)
{
    mIsAuxiliary = isAuxiliary;
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::SetOverridesImplicitlyEnabledSubtype(
    /* [in] */ Boolean overridesImplicitlyEnabledSubtype)
{
    mOverridesImplicitlyEnabledSubtype = overridesImplicitlyEnabledSubtype;
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::SetIsAsciiCapable(
    /* [in] */ Boolean isAsciiCapable)
{
    mIsAsciiCapable = isAsciiCapable;
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::SetSubtypeIconResId(
    /* [in] */ Int32 subtypeIconResId)
{
    mSubtypeIconResId = subtypeIconResId;
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::SetSubtypeNameResId(
    /* [in] */ Int32 subtypeNameResId)
{
    mSubtypeNameResId = subtypeNameResId;
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::SetSubtypeId(
    /* [in] */ Int32 subtypeId)
{
    mSubtypeId = subtypeId;
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::SetSubtypeLocale(
    /* [in] */ const String& subtypeLocale)
{
    mSubtypeLocale = subtypeLocale == NULL ? "" : subtypeLocale;
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::SetSubtypeMode(
    /* [in] */ const String& subtypeMode)
{
    mSubtypeMode = subtypeMode == NULL ? "" : subtypeMode;
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::SetSubtypeExtraValue(
    /* [in] */ const String& subtypeExtraValue)
{
    mSubtypeExtraValue = subtypeExtraValue == NULL ? "" : subtypeExtraValue;
    return NOERROR;
}

ECode CInputMethodSubtype::InputMethodSubtypeBuilder::Build(
    /* [out] */ IInputMethodSubtype** type)
{
    VALIDATE_NOT_NULL(type)
    AutoPtr<IInputMethodSubtype> res;
    CInputMethodSubtype::New(this, (IInputMethodSubtype**)&res);
    *type = res;
    REFCOUNT_ADD(*type)
    return NOERROR;
}

//========================================================================================
//              CInputMethodSubtype::
//========================================================================================
String CInputMethodSubtype::TAG("CInputMethodSubtype");
String CInputMethodSubtype::EXTRA_VALUE_PAIR_SEPARATOR(",");
String CInputMethodSubtype::EXTRA_VALUE_KEY_VALUE_SEPARATOR("=");
String CInputMethodSubtype::EXTRA_KEY_UNTRANSLATABLE_STRING_IN_SUBTYPE_NAME("UntranslatableReplacementStringInSubtypeName");

CAR_INTERFACE_IMPL_2(CInputMethodSubtype, Object, IInputMethodSubtype, IParcelable)

CAR_OBJECT_IMPL(CInputMethodSubtype)

CInputMethodSubtype::CInputMethodSubtype()
    : mIsAuxiliary(FALSE)
    , mOverridesImplicitlyEnabledSubtype(FALSE)
    , mIsAsciiCapable(FALSE)
    , mSubtypeHashCode(0)
    , mSubtypeIconResId(0)
    , mSubtypeNameResId(0)
    , mSubtypeId(0)
{
}

CInputMethodSubtype::~CInputMethodSubtype()
{
    mExtraValueHashMapCache = NULL;
}

AutoPtr<CInputMethodSubtype::InputMethodSubtypeBuilder> CInputMethodSubtype::GetBuilder(
    /* [in] */ Int32 nameId,
    /* [in] */ Int32 iconId,
    /* [in] */ const String& locale,
    /* [in] */ const String& mode,
    /* [in] */ const String& extraValue,
    /* [in] */ Boolean isAuxiliary,
    /* [in] */ Boolean overridesImplicitlyEnabledSubtype,
    /* [in] */ Int32 id,
    /* [in] */ Boolean isAsciiCapable)
{
    AutoPtr<InputMethodSubtypeBuilder> builder = new InputMethodSubtypeBuilder();
    builder->mSubtypeNameResId = nameId;
    builder->mSubtypeIconResId = iconId;
    builder->mSubtypeLocale = locale;
    builder->mSubtypeMode = mode;
    builder->mSubtypeExtraValue = extraValue;
    builder->mIsAuxiliary = isAuxiliary;
    builder->mOverridesImplicitlyEnabledSubtype = overridesImplicitlyEnabledSubtype;
    builder->mSubtypeId = id;
    builder->mIsAsciiCapable = isAsciiCapable;
    return builder;
 }

ECode CInputMethodSubtype::constructor()
{
    return NOERROR;
}

ECode CInputMethodSubtype::constructor(
    /* [in] */ Int32 nameId,
    /* [in] */ Int32 iconId,
    /* [in] */ const String& locale,
    /* [in] */ const String& mode,
    /* [in] */ const String& extraValue,
    /* [in] */ Boolean isAuxiliary,
    /* [in] */ Boolean overridesImplicitlyEnabledSubtype)
{
    return constructor(nameId, iconId, locale, mode, extraValue, isAuxiliary,
                overridesImplicitlyEnabledSubtype, 0);
}

ECode CInputMethodSubtype::constructor(
    /* [in] */ Int32 nameId,
    /* [in] */ Int32 iconId,
    /* [in] */ const String& locale,
    /* [in] */ const String& mode,
    /* [in] */ const String& extraValue,
    /* [in] */ Boolean isAuxiliary,
    /* [in] */ Boolean overridesImplicitlyEnabledSubtype,
    /* [in] */ Int32 id)
{
    return constructor(GetBuilder(nameId, iconId, locale, mode, extraValue, isAuxiliary,
                overridesImplicitlyEnabledSubtype, id, FALSE));
}

ECode CInputMethodSubtype::constructor(
    /* [in] */ IInputMethodSubtypeBuilder* builder)
{
    InputMethodSubtypeBuilder* cb = (InputMethodSubtypeBuilder*)builder;
    mSubtypeNameResId = cb->mSubtypeNameResId;
    mSubtypeIconResId = cb->mSubtypeIconResId;
    mSubtypeLocale = cb->mSubtypeLocale;
    mSubtypeMode = cb->mSubtypeMode;
    mSubtypeExtraValue = cb->mSubtypeExtraValue;
    mIsAuxiliary = cb->mIsAuxiliary;
    mOverridesImplicitlyEnabledSubtype = cb->mOverridesImplicitlyEnabledSubtype;
    mSubtypeId = cb->mSubtypeId;
    mIsAsciiCapable = cb->mIsAsciiCapable;
    // If hashCode() of this subtype is 0 and you want to specify it as an id of this subtype,
    // just specify 0 as this subtype's id. Then, this subtype's id is treated as 0.
    mSubtypeHashCode = mSubtypeId != 0 ? mSubtypeId : HashCodeInternal(mSubtypeLocale,
            mSubtypeMode, mSubtypeExtraValue, mIsAuxiliary, mOverridesImplicitlyEnabledSubtype,
            mIsAsciiCapable);
    return NOERROR;
}

ECode CInputMethodSubtype::GetNameResId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSubtypeNameResId;
    return NOERROR;
}

ECode CInputMethodSubtype::GetIconResId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSubtypeIconResId;
    return NOERROR;
}

ECode CInputMethodSubtype::GetLocale(
    /* [out] */ String* locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = mSubtypeLocale;
    return NOERROR;
}

ECode CInputMethodSubtype::GetMode(
    /* [out] */ String* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mSubtypeMode;
    return NOERROR;
}

ECode CInputMethodSubtype::GetExtraValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mSubtypeExtraValue;
    return NOERROR;
}

ECode CInputMethodSubtype::IsAuxiliary(
    /* [out] */ Boolean* auxi)
{
    VALIDATE_NOT_NULL(auxi);
    *auxi = mIsAuxiliary;
    return NOERROR;
}

ECode CInputMethodSubtype::OverridesImplicitlyEnabledSubtype(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mOverridesImplicitlyEnabledSubtype;
    return NOERROR;
}

ECode CInputMethodSubtype::IsAsciiCapable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsAsciiCapable;
    return NOERROR;
}

ECode CInputMethodSubtype::GetDisplayName(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [in] */ IApplicationInfo* appInfo,
    /* [out] */ ICharSequence** name)
{
    VALIDATE_NOT_NULL(name);

    AutoPtr<ILocale> locale = ConstructLocaleFromString(mSubtypeLocale);
    String localeStr = mSubtypeLocale;
    if (locale != NULL) {
        locale->GetDisplayName(&localeStr);
    }
    if (mSubtypeNameResId == 0) {
        return CString::New(localeStr, name);
    }
    AutoPtr<ICharSequence> subtypeName;
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    pm->GetText(packageName, mSubtypeNameResId, appInfo, (ICharSequence**)&subtypeName);
    if (!TextUtils::IsEmpty(subtypeName)) {
        Boolean result = FALSE;
        ContainsExtraValueKey(EXTRA_KEY_UNTRANSLATABLE_STRING_IN_SUBTYPE_NAME, &result);
        String replacementString = localeStr;
        if (result) {
            GetExtraValueOf(EXTRA_KEY_UNTRANSLATABLE_STRING_IN_SUBTYPE_NAME, &replacementString);
        }
        assert(0);
        // try {
        //     return String.format(
        //             subtypeName.toString(), replacementString != null ? replacementString : "");
        // } catch (IllegalFormatException e) {
        //     Slog.w(TAG, "Found illegal format in subtype name("+ subtypeName + "): " + e);
        //     return "";
        // }
    }
    return CString::New(localeStr, name);
}

AutoPtr< HashMap<String, String> > CInputMethodSubtype::GetExtraValueHashMap()
{
    if (mExtraValueHashMapCache == NULL) {
        synchronized(this) {
            if (mExtraValueHashMapCache == NULL) {
                mExtraValueHashMapCache = new HashMap<String, String>(10);
                assert(0);
                //TODO
                // final String[] pairs = mSubtypeExtraValue.split(EXTRA_VALUE_PAIR_SEPARATOR);
                AutoPtr< ArrayOf<String> > pairs;
                const Int32 N = pairs->GetLength();
                for (Int32 i = 0; i < N; ++i) {
                    assert(0);
                    //TODO
                    //final String[] pair = pairs[i].split(EXTRA_VALUE_KEY_VALUE_SEPARATOR);
                    AutoPtr< ArrayOf<String> > pair;
                    Int32 len = pair->GetLength();
                    if (len == 1) {
                        (*mExtraValueHashMapCache)[(*pair)[0]] = NULL;
                    }
                    else if (len > 1) {
                        if (len > 2) {
                            Slogger::W(TAG, "ExtraValue has two or more '='s");
                        }
                        (*mExtraValueHashMapCache)[(*pair)[0]] = (*pair)[1];
                    }
                }
            }
        }
    }
    return mExtraValueHashMapCache;
}

ECode CInputMethodSubtype::ContainsExtraValueKey(
    /* [in] */ const String& key,
    /* [out] */ Boolean* contains)
{
    VALIDATE_NOT_NULL(contains);

    AutoPtr< HashMap<String, String> > values = GetExtraValueHashMap();
    *contains = values->Find(key) != values->End();
    return NOERROR;
}

ECode CInputMethodSubtype::GetExtraValueOf(
    /* [in] */ const String& key,
    /* [out] */ String* extraValue)
{
    VALIDATE_NOT_NULL(extraValue);

    *extraValue = NULL;
    AutoPtr< HashMap<String, String> > values = GetExtraValueHashMap();
    HashMap<String, String>::Iterator it = values->Find(key);
    if (it != values->End()) {
        *extraValue = it->mSecond;
    }
    return NOERROR;
}

ECode CInputMethodSubtype::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mSubtypeHashCode;
    return NOERROR;
}

ECode CInputMethodSubtype::Equals(
    /* [in] */ IInputMethodSubtype* o,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals)

    CInputMethodSubtype* subtype = (CInputMethodSubtype*)o;
    if (subtype->mSubtypeId != 0 || mSubtypeId != 0) {
        *equals = subtype->mSubtypeHashCode == mSubtypeHashCode;
        return NOERROR;
    }
    *equals = (subtype->mSubtypeHashCode == mSubtypeHashCode)
            && (subtype->mSubtypeLocale.Equals(mSubtypeLocale))
            && (subtype->mSubtypeMode.Equals(mSubtypeMode))
            && (subtype->mSubtypeExtraValue.Equals(mSubtypeExtraValue))
            && (subtype->mIsAuxiliary == mIsAuxiliary)
            && (subtype->mOverridesImplicitlyEnabledSubtype == mOverridesImplicitlyEnabledSubtype)
            && (subtype->mIsAsciiCapable == mIsAsciiCapable);
    return NOERROR;
}

ECode CInputMethodSubtype::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    return Equals(IInputMethodSubtype::Probe(other), result);
}

ECode CInputMethodSubtype::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mSubtypeNameResId);
    dest->WriteInt32(mSubtypeIconResId);
    dest->WriteString(mSubtypeLocale);
    dest->WriteString(mSubtypeMode);
    dest->WriteString(mSubtypeExtraValue);
    dest->WriteBoolean(mIsAuxiliary);
    dest->WriteBoolean(mOverridesImplicitlyEnabledSubtype);
    dest->WriteInt32(mSubtypeHashCode);
    dest->WriteInt32(mSubtypeId);
    dest->WriteInt32(mIsAsciiCapable ? 1 : 0);
    return NOERROR;
}

ECode CInputMethodSubtype::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    String s;
    source->ReadInt32(&mSubtypeNameResId);
    source->ReadInt32(&mSubtypeIconResId);
    source->ReadString(&s);
    mSubtypeLocale = !s.IsNull() ? s : "";
    source->ReadString(&s);
    mSubtypeMode = !s.IsNull() ? s : "";
    source->ReadString(&s);
    mSubtypeExtraValue = !s.IsNull() ? s : "";
    source->ReadBoolean(&mIsAuxiliary);
    source->ReadBoolean(&mOverridesImplicitlyEnabledSubtype);
    source->ReadInt32(&mSubtypeHashCode);
    source->ReadInt32(&mSubtypeId);
    Int32 c = 0;
    source->ReadInt32(&c);
    mIsAsciiCapable = (c == 1);
    return NOERROR;
}

AutoPtr<ILocale> CInputMethodSubtype::ConstructLocaleFromString(
    /* [in] */ const String& localeStr)
{
    if (localeStr.IsNullOrEmpty()) return NULL;

    assert(0);
    //TODO
    //String[] localeParams = localeStr.Split("_", 3);
    AutoPtr<ArrayOf<String> > localeParams;

    AutoPtr<ILocale> locale;
    // The length of localeStr is guaranteed to always return a 1 <= value <= 3
    // because localeStr is not empty.
    Int32 len = localeParams->GetLength();
    if (len == 1) {
        CLocale::New((*localeParams)[0], (ILocale**)&locale);
        return locale;
    }
    else if (len == 2) {
        CLocale::New((*localeParams)[0], (*localeParams)[1], (ILocale**)&locale);
        return locale;
    }
    else if (len == 3) {
        CLocale::New((*localeParams)[0], (*localeParams)[1], (*localeParams)[2], (ILocale**)&locale);
        return locale;
    }
    return NULL;
}

Int32 CInputMethodSubtype::HashCodeInternal(
    /* [in] */ const String& locale,
    /* [in] */ const String& mode,
    /* [in] */ const String& extraValue,
    /* [in] */ Boolean isAuxiliary,
    /* [in] */ Boolean overridesImplicitlyEnabledSubtype,
    /* [in] */ Boolean isAsciiCapable)
{
    // CAVEAT: Must revisit how to compute needsToCalculateCompatibleHashCode when a new
    // attribute is added in order to avoid enabled subtypes being unexpectedly disabled.
    AutoPtr<ICharSequence> pLoc;
    CString::New(locale, (ICharSequence**)&pLoc);
    AutoPtr<ICharSequence> pMod;
    CString::New(mode, (ICharSequence**)&pMod);
    AutoPtr<ICharSequence> pExt;
    CString::New(extraValue, (ICharSequence**)&pExt);
    AutoPtr<IBoolean> pAux;
    CBoolean::New(isAuxiliary, (IBoolean**)&pAux);
    AutoPtr<IBoolean> pSub;
    CBoolean::New(overridesImplicitlyEnabledSubtype, (IBoolean**)&pSub);
    AutoPtr<IBoolean> pAsc;
    CBoolean::New(isAsciiCapable, (IBoolean**)&pAsc);

    Boolean needsToCalculateCompatibleHashCode = !isAsciiCapable;
    if (needsToCalculateCompatibleHashCode) {
        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(5);
        (*arr)[0] = pLoc;
        (*arr)[1] = pMod;
        (*arr)[2] = pExt;
        (*arr)[3] = pAux;
        (*arr)[4] = pSub;
        return Arrays::GetHashCode(arr);
    }

    AutoPtr<ArrayOf<IInterface*> > arr_1 = ArrayOf<IInterface*>::Alloc(6);
    (*arr_1)[0] = pLoc;
    (*arr_1)[1] = pMod;
    (*arr_1)[2] = pExt;
    (*arr_1)[3] = pAux;
    (*arr_1)[4] = pSub;
    (*arr_1)[5] = pAsc;
    return Arrays::GetHashCode(arr_1);
}

AutoPtr<IList> CInputMethodSubtype::Sort(
    /* [in] */ IContext* context,
    /* [in] */ Int32 flags,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IList* subtypeList)
{
    if (imi == NULL) return subtypeList;

    AutoPtr<IHashSet> inputSubtypesSet;
    CHashSet::New(ICollection::Probe(subtypeList), (IHashSet**)&inputSubtypesSet);
    AutoPtr<IArrayList> sortedList;
    CArrayList::New((IArrayList**)&sortedList);
    Int32 N = 0;
    imi->GetSubtypeCount(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInputMethodSubtype> subtype;
        imi->GetSubtypeAt(i, (IInputMethodSubtype**)&subtype);
        Boolean bContains = FALSE;
        if ((inputSubtypesSet->Contains(subtype, &bContains), bContains)) {
            sortedList->Add(subtype);
            inputSubtypesSet->Remove(subtype);
        }
    }
    // If subtypes in inputSubtypesSet remain, that means these subtypes are not
    // contained in imi, so the remaining subtypes will be appended.
    AutoPtr<IIterator> it;
    inputSubtypesSet->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        sortedList->Add(p);
    }
    return IList::Probe(sortedList);
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
