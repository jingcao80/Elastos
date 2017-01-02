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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/view/textservice/CSpellCheckerSubtype.h"
#include "elastos/droid/text/CTextUtils.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::CTextUtils;

using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CLocale;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              CSpellCheckerSubtype::
//========================================================================================
const String CSpellCheckerSubtype::TAG("SpellCheckerSubtype");
const String CSpellCheckerSubtype::EXTRA_VALUE_PAIR_SEPARATOR(",");
const String CSpellCheckerSubtype::EXTRA_VALUE_KEY_VALUE_SEPARATOR("=");

CAR_INTERFACE_IMPL_2(CSpellCheckerSubtype, Object, ISpellCheckerSubtype, IParcelable)

CAR_OBJECT_IMPL(CSpellCheckerSubtype)

CSpellCheckerSubtype::CSpellCheckerSubtype()
    : mSubtypeHashCode(0)
    , mSubtypeNameResId(0)
{
}

ECode CSpellCheckerSubtype::constructor()
{
    return NOERROR;
}

ECode CSpellCheckerSubtype::constructor(
    /* [in] */ Int32 nameId,
    /* [in] */ const String& locale,
    /* [in] */ const String& extraValue)
{
    mSubtypeNameResId = nameId;
    mSubtypeLocale = locale != NULL ? locale : "";
    mSubtypeExtraValue = extraValue != NULL ? extraValue : "";
    mSubtypeHashCode = HashCodeInternal(mSubtypeLocale, mSubtypeExtraValue);

    return NOERROR;
}

ECode CSpellCheckerSubtype::GetNameResId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSubtypeNameResId;
    return NOERROR;
}

ECode CSpellCheckerSubtype::GetLocale(
    /* [out] */ String* locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = mSubtypeLocale;
    return NOERROR;
}

ECode CSpellCheckerSubtype::GetExtraValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mSubtypeExtraValue;
    return NOERROR;
}

AutoPtr<IHashMap> CSpellCheckerSubtype::GetExtraValueHashMap()
{
    if (mExtraValueHashMapCache == NULL) {
        CHashMap::New((IHashMap**)&mExtraValueHashMapCache);
        AutoPtr<ArrayOf<String> > pairs;
        StringUtils::Split(mSubtypeExtraValue, EXTRA_VALUE_PAIR_SEPARATOR, (ArrayOf<String>**)&pairs);
        Int32 N = pairs->GetLength();
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<ArrayOf<String> > pair;
            StringUtils::Split((*pairs)[i], EXTRA_VALUE_KEY_VALUE_SEPARATOR, (ArrayOf<String>**)&pair);
            if (pair->GetLength() == 1) {
                AutoPtr<ICharSequence> cs;
                CString::New((*pair)[0], (ICharSequence**)&cs);
                mExtraValueHashMapCache->Put(cs, NULL);
            }
            else if (pair->GetLength() > 1) {
                if (pair->GetLength() > 2) {
                    Slogger::W(TAG, "ExtraValue has two or more '='s");
                }
                AutoPtr<ICharSequence> cs_0, cs_1;
                CString::New((*pair)[0], (ICharSequence**)&cs_0);
                CString::New((*pair)[1], (ICharSequence**)&cs_1);
                mExtraValueHashMapCache->Put(cs_0, cs_1);
            }
        }
    }
    return mExtraValueHashMapCache;
}

ECode CSpellCheckerSubtype::ContainsExtraValueKey(
    /* [in] */ const String& key,
    /* [out] */ Boolean* bFlag)
{
    VALIDATE_NOT_NULL(bFlag);
    AutoPtr<IHashMap> hashMap = GetExtraValueHashMap();
    AutoPtr<ICharSequence> cs;
    CString::New(key, (ICharSequence**)&cs);
    return hashMap->ContainsKey(cs, bFlag);
}

ECode CSpellCheckerSubtype::GetExtraValueOf(
    /* [in] */ const String& key,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<IHashMap> hashMap = GetExtraValueHashMap();
    AutoPtr<ICharSequence> cs;
    CString::New(key, (ICharSequence**)&cs);
    AutoPtr<IInterface> val;
    hashMap->Get(cs, (IInterface**)&val);
    AutoPtr<ICharSequence> pVal = ICharSequence::Probe(val);
    return pVal->ToString(str);
}

ECode CSpellCheckerSubtype::GetHashCode(
    /* [out] */ Int32* hc)
{
    VALIDATE_NOT_NULL(hc)

    *hc = mSubtypeHashCode;
    return NOERROR;
}

ECode CSpellCheckerSubtype::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (ISpellCheckerSubtype::Probe(o) != NULL) {
        AutoPtr<ISpellCheckerSubtype> subtype = ISpellCheckerSubtype::Probe(o);
        Int32 shc = 0, hc = 0, snmID = 0, nmID = 0;
        *result = ((((CSpellCheckerSubtype*)(subtype.Get()))->GetHashCode(&shc), shc) == (GetHashCode(&hc), hc))
            && ((subtype->GetNameResId(&snmID), snmID) == (GetNameResId(&nmID), nmID));

        String sLoc, loc, sExt, ext;
        subtype->GetLocale(&sLoc);
        GetLocale(&loc);
        subtype->GetExtraValue(&sExt);
        GetExtraValue(&ext);
        *result = (*result) && sLoc.Equals(loc) && sExt.Equals(ext);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

AutoPtr<ILocale> CSpellCheckerSubtype::ConstructLocaleFromString(
    /* [in] */ const String& localeStr)
{
    AutoPtr<ITextUtils> tu;
    CTextUtils::AcquireSingleton((ITextUtils**)&tu);
    Boolean bEmp = FALSE;
    if ((tu->IsEmpty(localeStr, &bEmp), bEmp))
        return NULL;
    AutoPtr<ArrayOf<String> > localeParams;
    StringUtils::Split(localeStr, String("_"), 3, (ArrayOf<String>**)&localeParams);
    // The length of localeStr is guaranteed to always return a 1 <= value <= 3
    // because localeStr is not empty.
    if (localeParams->GetLength() == 1) {
        AutoPtr<ILocale> pLoc;
        CLocale::New((*localeParams)[0], (ILocale**)&pLoc);
        return pLoc;
    }
    else if (localeParams->GetLength() == 2) {
        AutoPtr<ILocale> pLoc;
        CLocale::New((*localeParams)[0], (*localeParams)[1], (ILocale**)&pLoc);
        return pLoc;
    }
    else if (localeParams->GetLength() == 3) {
        AutoPtr<ILocale> pLoc;
        CLocale::New((*localeParams)[0], (*localeParams)[1], (*localeParams)[2], (ILocale**)&pLoc);
        return pLoc;
    }
    return NULL;
}

ECode CSpellCheckerSubtype::GetDisplayName(
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
        AutoPtr<ICharSequence> cs;
        CString::New(localeStr, (ICharSequence**)&cs);
        *name = cs.Get();
        REFCOUNT_ADD(*name)
        return NOERROR;
    }
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<ICharSequence> subtypeName;
    pm->GetText(packageName, mSubtypeNameResId, appInfo, (ICharSequence**)&subtypeName);
    AutoPtr<ITextUtils> tu;
    CTextUtils::AcquireSingleton((ITextUtils**)&tu);
    Boolean bEmp = FALSE;
    if (!(tu->IsEmpty(subtypeName, &bEmp), bEmp)) {
        String str;
        subtypeName->ToString(&str);
        assert(0 && "TODO");
        String res;// = StringUtils::Format(str, localeStr);
        AutoPtr<ICharSequence> cs;
        CString::New(res, (ICharSequence**)&cs);
        *name = cs.Get();
        REFCOUNT_ADD(*name)
        return NOERROR;
    }
    else {
        AutoPtr<ICharSequence> cs;
        CString::New(localeStr, (ICharSequence**)&cs);
        *name = cs.Get();
        REFCOUNT_ADD(*name)
        return NOERROR;
    }
    return NOERROR;
}

ECode CSpellCheckerSubtype::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    String s;
    source->ReadInt32(&mSubtypeNameResId);
    source->ReadString(&mSubtypeLocale);
    source->ReadString(&mSubtypeExtraValue);
    mSubtypeHashCode = HashCodeInternal(mSubtypeLocale, mSubtypeExtraValue);
    return NOERROR;
}

ECode CSpellCheckerSubtype::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mSubtypeNameResId);
    dest->WriteString(mSubtypeLocale);
    dest->WriteString(mSubtypeExtraValue);

    return NOERROR;
}

Int32 CSpellCheckerSubtype::HashCodeInternal(
    /* [in] */ const String& locale,
    /* [in] */ const String& extraValue)
{
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> loc;
    CString::New(locale, (ICharSequence**)&loc);
    arr->Set(0, loc);
    AutoPtr<ICharSequence> ext;
    CString::New(extraValue, (ICharSequence**)&ext);
    arr->Set(1, ext);
    return Arrays::GetHashCode(arr);
}

AutoPtr<IList> CSpellCheckerSubtype::Sort(
    /* [in] */ IContext* context,
    /* [in] */ Int32 flags,
    /* [in] */ ISpellCheckerInfo* sci,
    /* [in] */ IList* subtypeList)
{
    if (sci == NULL) return subtypeList;
    AutoPtr<IHashSet> subtypesSet;
    CHashSet::New(ICollection::Probe(subtypeList), (IHashSet**)&subtypesSet);
    AutoPtr<IArrayList> sortedList;
    CArrayList::New((IArrayList**)&sortedList);
    Int32 N = 0;
    sci->GetSubtypeCount(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<ISpellCheckerSubtype> subtype;
        sci->GetSubtypeAt(i, (ISpellCheckerSubtype**)&subtype);
        Boolean bContains = FALSE;
        if ((subtypesSet->Contains(subtype, &bContains), bContains)) {
            sortedList->Add(subtype);
            subtypesSet->Remove(subtype);
        }
    }
    // If subtypes in subtypesSet remain, that means these subtypes are not
    // contained in sci, so the remaining subtypes will be appended.
    AutoPtr<IIterator> it;
    subtypesSet->GetIterator((IIterator**)&it);
    Boolean bNext = FALSE;
    while ((it->HasNext(&bNext), bNext)) {
        AutoPtr<IInterface> subtype;
        it->GetNext((IInterface**)&subtype);
        sortedList->Add(subtype);
    }
    return IList::Probe(sortedList);
}

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
