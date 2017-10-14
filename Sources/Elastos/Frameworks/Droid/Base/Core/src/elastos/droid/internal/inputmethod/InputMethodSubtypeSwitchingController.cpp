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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/internal/inputmethod/InputMethodSubtypeSwitchingController.h"
//#include "elastos/droid/internal/inputmethod/InputMethodUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CTreeMap;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Objects;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

CAR_INTERFACE_IMPL_2(InputMethodSubtypeSwitchingController::ImeSubtypeListItem, Object,
    IImeSubtypeListItem, IComparable)

InputMethodSubtypeSwitchingController::ImeSubtypeListItem::ImeSubtypeListItem(
    /* [in] */ ICharSequence* imeName,
    /* [in] */ ICharSequence* subtypeName,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeId,
    /* [in] */ const String& subtypeLocale,
    /* [in] */ const String& systemLocale)
{
    mImeName = imeName;
    mSubtypeName = subtypeName;
    mImi = imi;
    mSubtypeId = subtypeId;
    if (TextUtils::IsEmpty(subtypeLocale)) {
        mIsSystemLocale = FALSE;
        mIsSystemLanguage = FALSE;
    }
    else {
        mIsSystemLocale = subtypeLocale.Equals(systemLocale);
        if (mIsSystemLocale) {
            mIsSystemLanguage = TRUE;
        }
        else {
            // TODO: Use Locale#getLanguage or Locale#toLanguageTag
            String systemLanguage = ParseLanguageFromLocaleString(systemLocale);
            String subtypeLanguage = ParseLanguageFromLocaleString(subtypeLocale);
            mIsSystemLanguage = systemLanguage.GetLength() >= 2 &&
                    systemLanguage.Equals(subtypeLanguage);
        }
    }
}

String InputMethodSubtypeSwitchingController::ImeSubtypeListItem::ParseLanguageFromLocaleString(
    /* [in] */ const String& locale)
{
    Int32 idx = locale.IndexOf('_');
    if (idx < 0) {
        return locale;
    }
    else {
        return locale.Substring(0, idx);
    }
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    assert(IImeSubtypeListItem::Probe(_other) != NULL);
    ImeSubtypeListItem* other = (ImeSubtypeListItem*)IImeSubtypeListItem::Probe(_other);
    if (TextUtils::IsEmpty(mImeName)) {
        *result = 1;
        return NOERROR;
    }
    if (TextUtils::IsEmpty(other->mImeName)) {
        *result = -1;
        return NOERROR;
    }
    if (!TextUtils::Equals(mImeName, other->mImeName)) {
        String name, otherName;
        mImeName->ToString(&name);
        other->mImeName->ToString(&otherName);
        *result = name.Compare(otherName);
        return NOERROR;
    }
    if (TextUtils::Equals(mSubtypeName, other->mSubtypeName)) {
        *result = 0;
        return NOERROR;
    }
    if (mIsSystemLocale) {
        *result = -1;
        return NOERROR;
    }
    if (other->mIsSystemLocale) {
        *result = 1;
        return NOERROR;
    }
    if (mIsSystemLanguage) {
        *result = -1;
        return NOERROR;
    }
    if (other->mIsSystemLanguage) {
        *result = 1;
        return NOERROR;
    }
    if (TextUtils::IsEmpty(mSubtypeName)) {
        *result = 1;
        return NOERROR;
    }
    if (TextUtils::IsEmpty(other->mSubtypeName)) {
        *result = -1;
        return NOERROR;
    }
    String name, otherName;
    mSubtypeName->ToString(&name);
    other->mSubtypeName->ToString(&otherName);
    *result = name.Compare(otherName);
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    sb += "ImeSubtypeListItem{";
    sb += "mImeName=";
    sb += mImeName;
    sb += " mSubtypeName=";
    sb += mSubtypeName;
    sb += " mSubtypeId=";
    sb += mSubtypeId;
    sb += " mIsSystemLocale=";
    sb += mIsSystemLocale;
    sb += " mIsSystemLanguage=";
    sb += mIsSystemLanguage;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(o) == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }
    if (IImeSubtypeListItem::Probe(o)) {
        ImeSubtypeListItem* that = (ImeSubtypeListItem*)IImeSubtypeListItem::Probe(o);
        if (!Objects::Equals(mImi, that->mImi)) {
            *result = FALSE;
            return NOERROR;
        }
        if (mSubtypeId != that->mSubtypeId) {
            *result = FALSE;
            return NOERROR;
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}


ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::GetImeName(
    /* [out] */ ICharSequence** imeName)
{
    VALIDATE_NOT_NULL(imeName)
    *imeName = mImeName;
    REFCOUNT_ADD(*imeName)
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::SetImeName(
    /* [in] */ ICharSequence* imeName)
{
    mImeName = imeName;
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::GetSubtypeName(
    /* [out] */ ICharSequence** subtypeName)
{
    VALIDATE_NOT_NULL(subtypeName)
    *subtypeName = mSubtypeName;
    REFCOUNT_ADD(*subtypeName)
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::SetSubtypeName(
    /* [in] */ ICharSequence* subtypeName)
{
    mSubtypeName = subtypeName;
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::GetImi(
    /* [out] */ IInputMethodInfo** imi)
{
    VALIDATE_NOT_NULL(imi)
    *imi = mImi;
    REFCOUNT_ADD(*imi)
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::SetImi(
    /* [in] */ IInputMethodInfo* imi)
{
    mImi = imi;
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::GetSubtypeId(
    /* [out] */ Int32* subtypeId)
{
    VALIDATE_NOT_NULL(subtypeId)
    *subtypeId = mSubtypeId;
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::SetSubtypeId(
    /* [in] */ Int32 subtypeId)
{
    mSubtypeId = subtypeId;
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::GetIsSystemLocale(
    /* [out] */ Boolean* isSystemLocale)
{
    VALIDATE_NOT_NULL(isSystemLocale)
    *isSystemLocale = mIsSystemLocale;
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::SetIsSystemLocale(
    /* [in] */ Boolean isSystemLocale)
{
    mIsSystemLocale = isSystemLocale;
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::GetIsSystemLanguage(
    /* [out] */ Boolean* isSystemLanguage)
{
    VALIDATE_NOT_NULL(isSystemLanguage)
    *isSystemLanguage = mIsSystemLanguage;
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ImeSubtypeListItem::SetIsSystemLanguage(
    /* [in] */ Boolean isSystemLanguage)
{
    mIsSystemLanguage = isSystemLanguage;
    return NOERROR;
}

CAR_INTERFACE_IMPL(InputMethodSubtypeSwitchingController::InputMethodAndSubtypeList::Comparator,
            Object, IComparator)

InputMethodSubtypeSwitchingController::InputMethodAndSubtypeList::Comparator::Comparator(
    /* [in] */ IPackageManager* pm)
    : mPm(pm)
{
}

ECode InputMethodSubtypeSwitchingController::InputMethodAndSubtypeList::Comparator::Compare(
    /* [in] */ IInterface* _imi1,
    /* [in] */ IInterface* _imi2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInputMethodInfo> imi1 = IInputMethodInfo::Probe(_imi1);
    AutoPtr<IInputMethodInfo> imi2 = IInputMethodInfo::Probe(_imi2);
    if (imi2 == NULL) {
        *result = 0;
        return NOERROR;
    }
    if (imi1 == NULL) {
        *result = 1;
        return NOERROR;
    }

    String id1, id2;
    imi1->GetId(&id1);
    imi2->GetId(&id2);
    if (mPm == NULL) {
        *result = id1.Compare(id2);
        return NOERROR;
    }
    AutoPtr<ICharSequence> label1;
    imi1->LoadLabel(mPm, (ICharSequence**)&label1);
    StringBuilder sb1;
    sb1 += TO_IINTERFACE(label1);
    sb1 += "/";
    sb1 += id1;
    String imiId1 = sb1.ToString();
    AutoPtr<ICharSequence> label2;
    imi2->LoadLabel(mPm, (ICharSequence**)&label2);
    StringBuilder sb2;
    sb2 += TO_IINTERFACE(label2);
    sb2 += "/";
    sb2 += id2;
    String imiId2 = sb2.ToString();
    *result = imiId1.Compare(imiId2);
    return NOERROR;
}

InputMethodSubtypeSwitchingController::InputMethodAndSubtypeList::InputMethodAndSubtypeList(
    /* [in] */ IContext* context,
    /* [in] */ IInputMethodSettings* settings)
{
    mContext = context;
    mSettings = settings;
    context->GetPackageManager((IPackageManager**)&mPm);
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    AutoPtr<IConfiguration> c;
    r->GetConfiguration((IConfiguration**)&c);
    AutoPtr<ILocale> locale;
    c->GetLocale((ILocale**)&locale);
    if (locale != NULL)
        locale->ToString(&mSystemLocaleStr);
    else
        mSystemLocaleStr = "";
    AutoPtr<Comparator> comparator = new Comparator(mPm);
    CTreeMap::New(comparator, (ITreeMap**)&mSortedImmis);
}

AutoPtr<IList> InputMethodSubtypeSwitchingController::InputMethodAndSubtypeList::GetSortedInputMethodAndSubtypeList()
{
    return GetSortedInputMethodAndSubtypeList(TRUE, FALSE, FALSE);
}

AutoPtr<IList> InputMethodSubtypeSwitchingController::InputMethodAndSubtypeList::GetSortedInputMethodAndSubtypeList(
    /* [in] */ Boolean showSubtypes,
    /* [in] */ Boolean inputShown,
    /* [in] */ Boolean isScreenLocked)
{
    AutoPtr<IList> imList;
    CArrayList::New((IList**)&imList);
    AutoPtr<IHashMap> immis; //HashMap<InputMethodInfo, List<InputMethodSubtype>>
    mSettings->GetExplicitlyOrImplicitlyEnabledInputMethodsAndSubtypeListLocked(
        mContext, (IHashMap**)&immis);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    Int32 size;
    if (immis == NULL || (immis->GetSize(&size), size) == 0) {
        AutoPtr<IList> emptyList;
        collections->GetEmptyList((IList**)&emptyList);
        return emptyList;
    }

    mSortedImmis->Clear();
    mSortedImmis->PutAll(IMap::Probe(immis));
    AutoPtr<ISet> keySet;
    mSortedImmis->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> iter;
    keySet->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        iter->GetNext((IInterface**)&next);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(next);
        if (imi == NULL) {
            continue;
        }
        AutoPtr<IInterface> value;
        immis->Get(imi, (IInterface**)&value);
        AutoPtr<IList> explicitlyOrImplicitlyEnabledSubtypeList = IList::Probe(value);
        HashSet<String> enabledSubtypeSet;
        AutoPtr<ArrayOf<IInterface*> > array;
        explicitlyOrImplicitlyEnabledSubtypeList->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            Int32 hashCode;
            IObject::Probe((*array)[i])->GetHashCode(&hashCode);
            enabledSubtypeSet.Insert(StringUtils::ToString(hashCode));
        }
        AutoPtr<ICharSequence> imeLabel;
        imi->LoadLabel(mPm, (ICharSequence**)&imeLabel);
        if (showSubtypes && enabledSubtypeSet.GetSize() > 0) {
            Int32 subtypeCount;
            imi->GetSubtypeCount(&subtypeCount);
            if (DEBUG) {
                String id;
                imi->GetId(&id);
                Slogger::V(TAG, "Add subtypes: %d, %s", subtypeCount, id.string());
            }
            for (Int32 j = 0; j < subtypeCount; ++j) {
                AutoPtr<IInputMethodSubtype> subtype;
                imi->GetSubtypeAt(j, (IInputMethodSubtype**)&subtype);
                Int32 hashCode;
                IObject::Probe(subtype)->GetHashCode(&hashCode);
                String subtypeHashCode = StringUtils::ToString(hashCode);
                // We show all enabled IMEs and subtypes when an IME is shown.
                HashSet<String>::Iterator find = enabledSubtypeSet.Find(subtypeHashCode);
                Boolean isAuxiliary;
                if (find != enabledSubtypeSet.End() && ((inputShown && !isScreenLocked)
                    || !(subtype->IsAuxiliary(&isAuxiliary), isAuxiliary))) {
                    Boolean enabled;
                    subtype->OverridesImplicitlyEnabledSubtype(&enabled);
                    AutoPtr<ICharSequence> subtypeLabel;
                    if (enabled) {
                        String pkgName;
                        imi->GetPackageName(&pkgName);
                        AutoPtr<IServiceInfo> serviceInfo;
                        imi->GetServiceInfo((IServiceInfo**)&serviceInfo);
                        AutoPtr<IApplicationInfo> appInfo;
                        IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                        subtype->GetDisplayName(
                            mContext, pkgName, appInfo, (ICharSequence**)&subtypeLabel);
                    }
                    String locale;
                    subtype->GetLocale(&locale);
                    AutoPtr<IImeSubtypeListItem> item = new ImeSubtypeListItem(imeLabel,
                            subtypeLabel, imi, j, locale, mSystemLocaleStr);
                    imList->Add(item);

                    // Removing this subtype from enabledSubtypeSet because we no
                    // longer need to add an entry of this subtype to imList to avoid
                    // duplicated entries.
                    enabledSubtypeSet.Erase(subtypeHashCode);
                }
            }
        }
        else {
            AutoPtr<IImeSubtypeListItem> item = new ImeSubtypeListItem(
                imeLabel, NULL, imi, NOT_A_SUBTYPE_ID, String(NULL), mSystemLocaleStr);
            imList->Add(item);
        }
    }
    collections->Sort(imList);
    return imList;
}

InputMethodSubtypeSwitchingController::StaticRotationList::StaticRotationList(
    /* [in] */ IList* imeSubtypeList)
    : mImeSubtypeList(imeSubtypeList)
{
}

Int32 InputMethodSubtypeSwitchingController::StaticRotationList::GetIndex(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    Int32 currentSubtypeId = CalculateSubtypeId(imi, subtype);
    Int32 N;
    mImeSubtypeList->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> item;
        mImeSubtypeList->Get(i, (IInterface**)&item);
        AutoPtr<ImeSubtypeListItem> isli = (ImeSubtypeListItem*)IImeSubtypeListItem::Probe(item);
        // Skip until the current IME/subtype is found.
        Boolean equals;
        IObject::Probe(imi)->Equals(isli->mImi, &equals);
        if (equals && isli->mSubtypeId == currentSubtypeId) {
            return i;
        }
    }
    return -1;
}

AutoPtr<InputMethodSubtypeSwitchingController::ImeSubtypeListItem>
InputMethodSubtypeSwitchingController::StaticRotationList::GetNextInputMethodLocked(
    /* [in] */ Boolean onlyCurrentIme,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (imi == NULL) {
        return NULL;
    }
    Int32 N;
    mImeSubtypeList->GetSize(&N);
    if (N <= 1) {
        return NULL;
    }
    Int32 currentIndex = GetIndex(imi, subtype);
    if (currentIndex < 0) {
        return NULL;
    }
    for (Int32 offset = 1; offset < N; ++offset) {
        // Start searching the next IME/subtype from the next of the current index.
        Int32 candidateIndex = (currentIndex + offset) % N;
        AutoPtr<IInterface> item;
        mImeSubtypeList->Get(candidateIndex, (IInterface**)&item);
        AutoPtr<ImeSubtypeListItem> candidate = (ImeSubtypeListItem*)IImeSubtypeListItem::Probe(item);
        // Skip if searching inside the current IME only, but the candidate is not
        // the current IME.
        Boolean equals;
        IObject::Probe(imi)->Equals(candidate->mImi, &equals);
        if (onlyCurrentIme && !equals) {
            continue;
        }
        return candidate;
    }
    return NULL;
}

const String InputMethodSubtypeSwitchingController::DynamicRotationList::TAG("DynamicRotationList");

InputMethodSubtypeSwitchingController::DynamicRotationList::DynamicRotationList(
    /* [in] */ IList* imeSubtypeListItems)
{
    mImeSubtypeList = imeSubtypeListItems;
    Int32 N;
    mImeSubtypeList->GetSize(&N);
    mUsageHistoryOfSubtypeListItemIndex = ArrayOf<Int32>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        (*mUsageHistoryOfSubtypeListItemIndex)[i] = i;
    }
}

Int32 InputMethodSubtypeSwitchingController::DynamicRotationList::GetUsageRank(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    Int32 currentSubtypeId = CalculateSubtypeId(imi, subtype);
    Int32 N = mUsageHistoryOfSubtypeListItemIndex->GetLength();
    for (Int32 usageRank = 0; usageRank < N; usageRank++) {
        Int32 subtypeListItemIndex = (*mUsageHistoryOfSubtypeListItemIndex)[usageRank];
        AutoPtr<IInterface> item;
        mImeSubtypeList->Get(subtypeListItemIndex, (IInterface**)&item);
        AutoPtr<ImeSubtypeListItem> subtypeListItem = (ImeSubtypeListItem*)IImeSubtypeListItem::Probe(item);
        Boolean equals;
        IObject::Probe(subtypeListItem->mImi)->Equals(imi, &equals);
        if (equals && subtypeListItem->mSubtypeId == currentSubtypeId) {
            return usageRank;
        }
    }
    // Not found in the known IME/Subtype list.
    return -1;
}

void InputMethodSubtypeSwitchingController::DynamicRotationList::OnUserAction(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    Int32 currentUsageRank = GetUsageRank(imi, subtype);
    // Do nothing if currentUsageRank == -1 (not found), or currentUsageRank == 0
    if (currentUsageRank <= 0) {
        return;
    }
    Int32 currentItemIndex = (*mUsageHistoryOfSubtypeListItemIndex)[currentUsageRank];
    mUsageHistoryOfSubtypeListItemIndex->Copy(1, mUsageHistoryOfSubtypeListItemIndex, currentUsageRank);
    (*mUsageHistoryOfSubtypeListItemIndex)[0] = currentItemIndex;
}

AutoPtr<InputMethodSubtypeSwitchingController::ImeSubtypeListItem>
InputMethodSubtypeSwitchingController::DynamicRotationList::GetNextInputMethodLocked(
    /* [in] */ Boolean onlyCurrentIme,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    Int32 currentUsageRank = GetUsageRank(imi, subtype);
    if (currentUsageRank < 0) {
        if (DEBUG) {
            String id, strSubtype;
            imi->GetId(&id);
            IObject::Probe(subtype)->ToString(&strSubtype);
            Slogger::D(TAG, "IME/subtype is not found: %s, %s", id.string(), strSubtype.string());
        }
        return NULL;
    }
    Int32 N = mUsageHistoryOfSubtypeListItemIndex->GetLength();
    for (Int32 i = 1; i < N; i++) {
        Int32 subtypeListItemRank = (currentUsageRank + i) % N;
        Int32 subtypeListItemIndex = (*mUsageHistoryOfSubtypeListItemIndex)[subtypeListItemRank];
        AutoPtr<IInterface> item;
        mImeSubtypeList->Get(subtypeListItemIndex, (IInterface**)&item);
        AutoPtr<ImeSubtypeListItem> subtypeListItem = (ImeSubtypeListItem*)IImeSubtypeListItem::Probe(item);
        Boolean equals;
        IObject::Probe(imi)->Equals(subtypeListItem->mImi, &equals);
        if (onlyCurrentIme && !equals) {
            continue;
        }
        return subtypeListItem;
    }
    return NULL;
}

AutoPtr<InputMethodSubtypeSwitchingController::ControllerImpl>
InputMethodSubtypeSwitchingController::ControllerImpl::CreateFrom(
    /* [in] */ ControllerImpl* currentInstance,
    /* [in] */ IList* sortedEnabledItems)
{
    AutoPtr<DynamicRotationList> switchingAwareRotationList;
    {
        AutoPtr<IList> switchingAwareImeSubtypes = FilterImeSubtypeList(
            sortedEnabledItems, TRUE /* supportsSwitchingToNextInputMethod */);
        if (currentInstance != NULL &&
            currentInstance->mSwitchingAwareRotationList != NULL &&
            Objects::Equals(currentInstance->mSwitchingAwareRotationList->mImeSubtypeList,
                        switchingAwareImeSubtypes)) {
            // Can reuse the current instance.
            switchingAwareRotationList = currentInstance->mSwitchingAwareRotationList;
        }
        if (switchingAwareRotationList == NULL) {
            switchingAwareRotationList = new DynamicRotationList(switchingAwareImeSubtypes);
        }
    }

    AutoPtr<StaticRotationList> switchingUnawareRotationList;
    {
        AutoPtr<IList> switchingUnawareImeSubtypes = FilterImeSubtypeList(
                sortedEnabledItems, FALSE /* supportsSwitchingToNextInputMethod */);
        if (currentInstance != NULL &&
            currentInstance->mSwitchingUnawareRotationList != NULL &&
            Objects::Equals(currentInstance->mSwitchingUnawareRotationList->mImeSubtypeList,
                        switchingUnawareImeSubtypes)) {
            // Can reuse the current instance.
            switchingUnawareRotationList = currentInstance->mSwitchingUnawareRotationList;
        }
        if (switchingUnawareRotationList == NULL) {
            switchingUnawareRotationList = new StaticRotationList(switchingUnawareImeSubtypes);
        }
    }

    return new ControllerImpl(switchingAwareRotationList, switchingUnawareRotationList);
}

InputMethodSubtypeSwitchingController::ControllerImpl::ControllerImpl(
    /* [in] */ DynamicRotationList* switchingAwareRotationList,
    /* [in] */ StaticRotationList* switchingUnawareRotationList)
{
    mSwitchingAwareRotationList = switchingAwareRotationList;
    mSwitchingUnawareRotationList = switchingUnawareRotationList;
}

AutoPtr<InputMethodSubtypeSwitchingController::ImeSubtypeListItem>
InputMethodSubtypeSwitchingController::ControllerImpl::GetNextInputMethod(
    /* [in] */ Boolean onlyCurrentIme,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (imi == NULL) {
        return NULL;
    }
    Boolean supports;
    imi->SupportsSwitchingToNextInputMethod(&supports);
    if (supports) {
        return mSwitchingAwareRotationList->GetNextInputMethodLocked(
            onlyCurrentIme, imi, subtype);
    }
    else {
        return mSwitchingUnawareRotationList->GetNextInputMethodLocked(
            onlyCurrentIme, imi, subtype);
    }
}

void InputMethodSubtypeSwitchingController::ControllerImpl::OnUserActionLocked(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (imi == NULL) {
        return;
    }
    Boolean supports;
    imi->SupportsSwitchingToNextInputMethod(&supports);
    if (supports) {
        mSwitchingAwareRotationList->OnUserAction(imi, subtype);
    }
}

AutoPtr<IList> InputMethodSubtypeSwitchingController::ControllerImpl::FilterImeSubtypeList(
    /* [in] */ IList* items,
    /* [in] */ Boolean supportsSwitchingToNextInputMethod)
{
    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
    Int32 ALL_ITEMS_COUNT;
    items->GetSize(&ALL_ITEMS_COUNT);
    for (Int32 i = 0; i < ALL_ITEMS_COUNT; i++) {
        AutoPtr<IInterface> iitem;
        items->Get(i, (IInterface**)&iitem);
        AutoPtr<ImeSubtypeListItem> item = (ImeSubtypeListItem*)IImeSubtypeListItem::Probe(iitem);
        Boolean supports;
        item->mImi->SupportsSwitchingToNextInputMethod(&supports);
        if (supports == supportsSwitchingToNextInputMethod) {
            result->Add(TO_IINTERFACE(item));
        }
    }
    return result;
}

CAR_INTERFACE_IMPL(InputMethodSubtypeSwitchingController, Object, IInputMethodSubtypeSwitchingController)

const String InputMethodSubtypeSwitchingController::TAG("InputMethodSubtypeSwitchingController");
const Boolean InputMethodSubtypeSwitchingController::DEBUG = FALSE;
const Int32 InputMethodSubtypeSwitchingController::NOT_A_SUBTYPE_ID = IInputMethodUtils::NOT_A_SUBTYPE_ID;

InputMethodSubtypeSwitchingController::InputMethodSubtypeSwitchingController(
    /* [in] */ IInputMethodSettings* settings,
    /* [in] */ IContext* context)
{
    mSettings = settings;
    ResetCircularListLocked(context);
}

Int32 InputMethodSubtypeSwitchingController::CalculateSubtypeId(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (subtype != NULL) {
        Int32 hashCode;
        IObject::Probe(subtype)->GetHashCode(&hashCode);
        // return InputMethodUtils::GetSubtypeIdFromHashCode(imi, hashCode);
    }
    return NOT_A_SUBTYPE_ID;
}

AutoPtr<IInputMethodSubtypeSwitchingController> InputMethodSubtypeSwitchingController::CreateInstanceLocked(
    /* [in] */ IInputMethodSettings* settings,
    /* [in] */ IContext* context)
{
    return new InputMethodSubtypeSwitchingController(settings, context);
}

ECode InputMethodSubtypeSwitchingController::OnUserActionLocked(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (mController == NULL) {
        if (DEBUG) {
            Logger::E(TAG, "mController shouldn't be NULL.");
        }
        return NOERROR;
    }
    mController->OnUserActionLocked(imi, subtype);
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::ResetCircularListLocked(
    /* [in] */ IContext* context)
{
    mSubtypeList = new InputMethodAndSubtypeList(context, mSettings);
    mController = ControllerImpl::CreateFrom(mController,
        mSubtypeList->GetSortedInputMethodAndSubtypeList());
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::GetNextInputMethodLocked(
    /* [in] */ Boolean onlyCurrentIme,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype,
    /* [out] */ IImeSubtypeListItem** listItem)
{
    VALIDATE_NOT_NULL(listItem)
    if (mController == NULL) {
        if (DEBUG) {
            Logger::E(TAG, "mController shouldn't be NULL.");
        }
        *listItem = NULL;
        return NOERROR;
    }
    AutoPtr<IImeSubtypeListItem> temp = mController->GetNextInputMethod(onlyCurrentIme, imi, subtype);
    *listItem = temp;
    REFCOUNT_ADD(*listItem)
    return NOERROR;
}

ECode InputMethodSubtypeSwitchingController::GetSortedInputMethodAndSubtypeListLocked(
    /* [in] */ Boolean showSubtypes,
    /* [in] */ Boolean inputShown,
    /* [in] */ Boolean isScreenLocked,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    AutoPtr<IList> temp = mSubtypeList->GetSortedInputMethodAndSubtypeList(
            showSubtypes, inputShown, isScreenLocked);
    *list = temp;
    REFCOUNT_ADD(*list)
    return NOERROR;
}
} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos
