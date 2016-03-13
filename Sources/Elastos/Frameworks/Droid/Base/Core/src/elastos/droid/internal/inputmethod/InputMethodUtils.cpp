
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/internal/inputmethod/InputMethodUtils.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/text/CSimpleStringSplitter.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/inputmethod/CInputMethodSubtype.h"
#include "elastos/droid/view/textservice/CTextServicesManager.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/utility/CPair.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Text::CSimpleStringSplitter;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::TextService::CTextServicesManager;
using Elastos::Droid::View::TextService::ISpellCheckerInfo;
using Elastos::Droid::View::InputMethod::CInputMethodSubtype;
using Elastos::Droid::Utility::CPair;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::IString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CLocale;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ICollection;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::ISet;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

const Char32 InputMethodUtils::InputMethodSettings::INPUT_METHOD_SEPARATER = ':';
const Char32 InputMethodUtils::InputMethodSettings::INPUT_METHOD_SUBTYPE_SEPARATER = ';';

CAR_INTERFACE_IMPL(InputMethodUtils::InputMethodSettings, Object, IInputMethodSettings)

InputMethodUtils::InputMethodSettings::InputMethodSettings()
    : mCurrentUserId(0)
{
    CSimpleStringSplitter::New(INPUT_METHOD_SEPARATER, (ISimpleStringSplitter**)&mInputMethodSplitter);
    CSimpleStringSplitter::New(INPUT_METHOD_SUBTYPE_SEPARATER, (ISimpleStringSplitter**)&mSubtypeSplitter);
    mCurrentProfileIds = ArrayOf<Int32>::Alloc(0);
}

void InputMethodUtils::InputMethodSettings::BuildEnabledInputMethodsSettingString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IPair* pair)
{
    AutoPtr<IInterface> first, second;
    pair->GetFirst((IInterface**)&first);
    pair->GetSecond((IInterface**)&second);
    String id;
    IObject::Probe(first)->ToString(&id);
    AutoPtr<IArrayList> subtypes = IArrayList::Probe(second);
    builder->Append(id);
    // Inputmethod and subtypes are saved in the settings as follows:
    // ime0;subtype0;subtype1:ime1;subtype0:ime2:ime3;subtype0;subtype1
    Int32 size;
    subtypes->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        subtypes->Get(i, (IInterface**)&item);
        String subtypeId;
        IObject::Probe(item)->ToString(&subtypeId);
        builder->AppendChar(INPUT_METHOD_SUBTYPE_SEPARATER);
        builder->Append(subtypeId);
    }
}

ECode InputMethodUtils::InputMethodSettings::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IHashMap* methodMap,
    /* [in] */ IArrayList* methodList,
    /* [in] */ Int32 userId)
{
    SetCurrentUserId(userId);
    mRes = res;
    mResolver = resolver;
    mMethodMap = methodMap;
    mMethodList = methodList;
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::SetCurrentUserId(
    /* [in] */ Int32 userId)
{
    if (DEBUG) {
        Slogger::D(TAG, "--- Swtich the current user from %d to %d", mCurrentUserId, userId);
    }
    // IMMS settings are kept per user, so keep track of current user
    mCurrentUserId = userId;
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::SetCurrentProfileIds(
    /* [in] */ ArrayOf<Int32>* currentProfileIds)
{
    AutoLock lock(this);
    mCurrentProfileIds = currentProfileIds;
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::IsCurrentProfile(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(this);
    if (userId == mCurrentUserId) {
        *result = TRUE;
        return NOERROR;
    }
    for (Int32 i = 0; i < mCurrentProfileIds->GetLength(); i++) {
        if (userId == (*mCurrentProfileIds)[i]) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::GetEnabledInputMethodListLocked(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    AutoPtr<IList> subtypes;
    GetEnabledInputMethodsAndSubtypeListLocked((IList**)&subtypes);
    AutoPtr<IList> ret = CreateEnabledInputMethodListLocked(subtypes);
    *list = ret;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::GetEnabledInputMethodAndSubtypeHashCodeListLocked(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    AutoPtr<IList> subtypes;
    GetEnabledInputMethodsAndSubtypeListLocked((IList**)&subtypes);
    AutoPtr<IList> ret = CreateEnabledInputMethodAndSubtypeHashCodeListLocked(subtypes);
    *list = ret;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::GetEnabledInputMethodSubtypeListLocked(
    /* [in] */ IContext* context,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    AutoPtr<IList> enabledSubtypes;
    GetEnabledInputMethodSubtypeListLocked(imi, (IList**)&enabledSubtypes);
    Boolean isEmpty;
    if (allowsImplicitlySelectedSubtypes && (enabledSubtypes->IsEmpty(&isEmpty), isEmpty)) {
        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r);
        enabledSubtypes = IList::Probe(GetImplicitlyApplicableSubtypesLocked(r, imi));
    }
    CInputMethodSubtype::Sort(context, 0, imi, enabledSubtypes);
    *list = enabledSubtypes;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::GetEnabledInputMethodSubtypeListLocked(
    /* [in] */ IInputMethodInfo* imi,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    AutoPtr<IList> imsList;
    GetEnabledInputMethodsAndSubtypeListLocked((IList**)&imsList);
    AutoPtr<IArrayList> enabledSubtypes;
    CArrayList::New((IArrayList**)&enabledSubtypes);
    if (imi != NULL) {
        Int32 size;
        imsList->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> item;
            imsList->Get(i, (IInterface**)&item);
            AutoPtr<IPair> pair = IPair::Probe(item);
            AutoPtr<IInterface> first, second;
            pair->GetFirst((IInterface**)&first);
            pair->GetSecond((IInterface**)&second);

            AutoPtr<IInterface> value;
            mMethodMap->Get(first, (IInterface**)&value);
            AutoPtr<IInputMethodInfo> info = IInputMethodInfo::Probe(value);
            String infoId, imiId;
            if (info != NULL && (info->GetId(&infoId),
                infoId.Equals((imi->GetId(&imiId), imiId)))) {
                Int32 subtypeCount;
                info->GetSubtypeCount(&subtypeCount);
                for (Int32 i = 0; i < subtypeCount; ++i) {
                    AutoPtr<IInputMethodSubtype> ims;
                    info->GetSubtypeAt(i, (IInputMethodSubtype**)&ims);
                    AutoPtr<IArrayList> array = IArrayList::Probe(second);
                    Int32 arraySize;
                    array->GetSize(&arraySize);
                    for (Int32 j = 0; j < arraySize; j++) {
                        AutoPtr<IInterface> element;
                        array->Get(j, (IInterface**)&element);
                        String s;
                        IObject::Probe(element)->ToString(&s);
                        Int32 hashCode;
                        IObject::Probe(ims)->GetHashCode(&hashCode);
                        if (StringUtils::ToString(hashCode).Equals(s)) {
                            enabledSubtypes->Add(ims);
                        }
                    }
                }
                break;
            }
        }
    }
    *list = IList::Probe(enabledSubtypes);
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::EnableAllIMEsIfThereIsNoEnabledIME()
{
    String enabledInputMethodsStr;
    GetEnabledInputMethodsStr(&enabledInputMethodsStr);
    if (TextUtils::IsEmpty(enabledInputMethodsStr)) {
        StringBuilder sb;
        Int32 N;
        mMethodList->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> item;
            mMethodList->Get(i, (IInterface**)&item);
            AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(item);
            String id;
            imi->GetId(&id);
            Slogger::I(TAG, "Adding: %s", id.string());
            if (i > 0)
                sb.AppendChar(':');
            sb.Append(id);
        }
        PutEnabledInputMethodsStr(sb.ToString());
    }
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::GetEnabledInputMethodsAndSubtypeListLocked(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    AutoPtr<IList> imsList;
    CArrayList::New((IList**)&imsList);
    String enabledInputMethodsStr;
    GetEnabledInputMethodsStr(&enabledInputMethodsStr);
    if (TextUtils::IsEmpty(enabledInputMethodsStr)) {
        *list = imsList;
        REFCOUNT_ADD(*list)
        return NOERROR;
    }
    mInputMethodSplitter->SetString(enabledInputMethodsStr);
    Boolean hasNext;
    while (mInputMethodSplitter->HasNext(&hasNext), hasNext) {
        String nextImsStr;
        mInputMethodSplitter->GetNext(&nextImsStr);
        mSubtypeSplitter->SetString(nextImsStr);
        if (mSubtypeSplitter->HasNext(&hasNext), hasNext) {
            AutoPtr<IArrayList> subtypeHashes;
            CArrayList::New((IArrayList**)&subtypeHashes);
            // The first element is ime id.
            AutoPtr<IInterface> imeId;
            mSubtypeSplitter->GetNext((IInterface**)&imeId);
            while (mSubtypeSplitter->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> next;
                mSubtypeSplitter->GetNext((IInterface**)&next);
                subtypeHashes->Add(next);
            }
            AutoPtr<IPair> pair;
            CPair::New(imeId, subtypeHashes, (IPair**)&pair);
            imsList->Add(pair);
        }
    }
    *list = imsList;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::AppendAndPutEnabledInputMethodLocked(
    /* [in] */ const String& id,
    /* [in] */ Boolean reloadInputMethodStr)
{
    if (reloadInputMethodStr) {
        String enabledInputMethodsStr;
        GetEnabledInputMethodsStr(&enabledInputMethodsStr);
    }
    if (TextUtils::IsEmpty(mEnabledInputMethodsStrCache)) {
        // Add in the newly enabled input method.
        PutEnabledInputMethodsStr(id);
    }
    else {
        StringBuilder sb(mEnabledInputMethodsStrCache);
        sb.AppendChar(INPUT_METHOD_SEPARATER);
        sb.Append(id);
        PutEnabledInputMethodsStr(sb.ToString());
    }
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::BuildAndPutEnabledInputMethodsStrRemovingIdLocked(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IList* imsList,
    /* [in] */ const String& id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isRemoved = FALSE;
    Boolean needsAppendSeparator = FALSE;
    Int32 size;
    imsList->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        imsList->Get(i, (IInterface**)&item);
        AutoPtr<IPair> ims = IPair::Probe(item);
        AutoPtr<IInterface> first, second;
        ims->GetFirst((IInterface**)&first);
        ims->GetSecond((IInterface**)&second);

        String curId;
        IObject::Probe(first)->ToString(&curId);
        if (curId.Equals(id)) {
            // We are disabling this input method, and it is
            // currently enabled.  Skip it to remove from the
            // new list.
            isRemoved = TRUE;
        }
        else {
            if (needsAppendSeparator) {
                builder->AppendChar(INPUT_METHOD_SEPARATER);
            }
            else {
                needsAppendSeparator = TRUE;
            }
            BuildEnabledInputMethodsSettingString(builder, ims);
        }
    }
    if (isRemoved) {
        // Update the setting with the new list of input methods.
        String str;
        IObject::Probe(builder)->ToString(&str);
        PutEnabledInputMethodsStr(str);
    }
    *result = isRemoved;
    return NOERROR;
}

AutoPtr<IList> InputMethodUtils::InputMethodSettings::CreateEnabledInputMethodListLocked(
    /* [in] */ IList* imsList)
{
    AutoPtr<IArrayList> res;
    CArrayList::New((IArrayList**)&res);
    Int32 size;
    imsList->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        imsList->Get(i, (IInterface**)&item);
        AutoPtr<IPair> pair = IPair::Probe(item);
        AutoPtr<IInterface> first;
        pair->GetFirst((IInterface**)&first);

        AutoPtr<IInterface> info;
        mMethodMap->Get(first, (IInterface**)&info);
        if (info != NULL) {
            res->Add(info);
        }
    }
    return IList::Probe(res);
}

AutoPtr<IList> InputMethodUtils::InputMethodSettings::CreateEnabledInputMethodAndSubtypeHashCodeListLocked(
        /* [in] */ IList* imsList)
{
    AutoPtr<IArrayList> res;
    CArrayList::New((IArrayList**)&res);
    Int32 size;
    imsList->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        imsList->Get(i, (IInterface**)&item);
        AutoPtr<IPair> pair = IPair::Probe(item);
        AutoPtr<IInterface> first, second;
        pair->GetFirst((IInterface**)&first);
        pair->GetSecond((IInterface**)&second);

        AutoPtr<IInterface> info;
        mMethodMap->Get(first, (IInterface**)&info);
        if (info != NULL) {
            AutoPtr<IPair> pair;
            CPair::New(info, second, (IPair**)&pair);
            res->Add(pair);
        }
    }
    return IList::Probe(res);
}

void InputMethodUtils::InputMethodSettings::PutEnabledInputMethodsStr(
    /* [in] */ const String& str)
{
    Boolean res;
    Settings::Secure::PutStringForUser(
            mResolver, ISettingsSecure::ENABLED_INPUT_METHODS, str, mCurrentUserId, &res);
    mEnabledInputMethodsStrCache = str;
    if (DEBUG) {
        Slogger::D(TAG, "putEnabledInputMethodStr: %s", str.string());
    }
}

ECode InputMethodUtils::InputMethodSettings::GetEnabledInputMethodsStr(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Settings::Secure::GetStringForUser(
        mResolver, ISettingsSecure::ENABLED_INPUT_METHODS, mCurrentUserId,
        &mEnabledInputMethodsStrCache);
    if (DEBUG) {
        Slogger::D(TAG, "GetEnabledInputMethodsStr: %s, %d",
            mEnabledInputMethodsStrCache.string(), mCurrentUserId);
    }
    *str = mEnabledInputMethodsStrCache;
    return NOERROR;
}

void InputMethodUtils::InputMethodSettings::SaveSubtypeHistory(
    /* [in] */ IList* savedImes,
    /* [in] */ const String& newImeId,
    /* [in] */ const String& newSubtypeId)
{
    StringBuilder builder;
    Boolean isImeAdded = FALSE;
    if (!TextUtils::IsEmpty(newImeId) && !TextUtils::IsEmpty(newSubtypeId)) {
        builder.Append(newImeId);
        builder.AppendChar(INPUT_METHOD_SUBTYPE_SEPARATER);
        builder.Append(newSubtypeId);
        isImeAdded = TRUE;
    }
    Int32 size;
    savedImes->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        savedImes->Get(i, (IInterface**)&item);
        AutoPtr<IPair> pair = IPair::Probe(item);
        AutoPtr<IInterface> first, second;
        pair->GetFirst((IInterface**)&first);
        pair->GetSecond((IInterface**)&second);

        String imeId, subtypeId;
        IObject::Probe(first)->ToString(&imeId);
        IObject::Probe(second)->ToString(&subtypeId);
        if (TextUtils::IsEmpty(subtypeId)) {
            subtypeId = NOT_A_SUBTYPE_ID_STR;
        }
        if (isImeAdded) {
            builder.AppendChar(INPUT_METHOD_SEPARATER);
        }
        else {
            isImeAdded = TRUE;
        }
        builder.Append(imeId);
        builder.AppendChar(INPUT_METHOD_SUBTYPE_SEPARATER);
        builder.Append(subtypeId);
    }
    // Remove the last INPUT_METHOD_SEPARATER
    PutSubtypeHistoryStr(builder.ToString());
}

void InputMethodUtils::InputMethodSettings::AddSubtypeToHistory(
    /* [in] */ const String& imeId,
    /* [in] */ const String& subtypeId)
{
    AutoPtr<IList> subtypeHistory = LoadInputMethodAndSubtypeHistoryLocked();
    Int32 size;
    subtypeHistory->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        subtypeHistory->Get(i, (IInterface**)&item);
        AutoPtr<IPair> ime = IPair::Probe(item);
        AutoPtr<IInterface> first, second;
        ime->GetFirst((IInterface**)&first);
        ime->GetSecond((IInterface**)&second);

        String id, subId;
        IObject::Probe(first)->ToString(&id);
        IObject::Probe(second)->ToString(&subId);

        if (id.Equals(imeId)) {
            if (DEBUG) {
                Slogger::V(TAG, "Subtype found in the history: %s, %s",
                    imeId.string(), subId.string());
            }
            // We should break here
            subtypeHistory->Remove(ime);
            break;
        }
    }
    if (DEBUG) {
        Slogger::V(TAG, "Add subtype to the history: %s, %s",
            imeId.string(), subtypeId.string());
    }
    SaveSubtypeHistory(subtypeHistory, imeId, subtypeId);
}

void InputMethodUtils::InputMethodSettings::PutSubtypeHistoryStr(
    /* [in] */ const String& str)
{
    if (DEBUG) {
        Slogger::D(TAG, "putSubtypeHistoryStr: %s", str.string());
    }
    Boolean res;
    Settings::Secure::PutStringForUser(
        mResolver, ISettingsSecure::INPUT_METHODS_SUBTYPE_HISTORY,
        str, mCurrentUserId, &res);
}

ECode InputMethodUtils::InputMethodSettings::GetLastInputMethodAndSubtypeLocked(
    /* [out] */ IPair** pair)
{
    VALIDATE_NOT_NULL(pair)
    // Gets the first one from the history
    AutoPtr<IPair> ret = GetLastSubtypeForInputMethodLockedInternal(String(NULL));
    *pair = ret;
    REFCOUNT_ADD(*pair)
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::GetLastSubtypeForInputMethodLocked(
    /* [in] */ const String& imeId,
    /* [out] */ String* str)
{
    AutoPtr<IPair> ime = GetLastSubtypeForInputMethodLockedInternal(imeId);
    if (ime != NULL) {
        AutoPtr<IInterface> second;
        ime->GetSecond((IInterface**)&second);
        return IObject::Probe(second)->ToString(str);
    }
    else {
        *str = NULL;
        return NOERROR;
    }
}

AutoPtr<IPair> InputMethodUtils::InputMethodSettings::GetLastSubtypeForInputMethodLockedInternal(
    /* [in] */ const String& imeId)
{
    AutoPtr<IList> enabledImes;
    GetEnabledInputMethodsAndSubtypeListLocked((IList**)&enabledImes);
    AutoPtr<IList> subtypeHistory = LoadInputMethodAndSubtypeHistoryLocked();
    Int32 size;
    subtypeHistory->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        subtypeHistory->Get(i, (IInterface**)&item);
        AutoPtr<IPair> imeAndSubtype = IPair::Probe(item);
        AutoPtr<IInterface> first, second;
        imeAndSubtype->GetFirst((IInterface**)&first);
        imeAndSubtype->GetSecond((IInterface**)&second);

        String imeInTheHistory;
        IObject::Probe(first)->ToString(&imeInTheHistory);
        // If imeId is empty, returns the first IME and subtype in the history
        if (TextUtils::IsEmpty(imeId) || imeInTheHistory.Equals(imeId)) {
            String subtypeInTheHistory ;
            IObject::Probe(second)->ToString(&subtypeInTheHistory);
            String subtypeHashCode = GetEnabledSubtypeHashCodeForInputMethodAndSubtypeLocked(
                    enabledImes, imeInTheHistory, subtypeInTheHistory);
            if (!TextUtils::IsEmpty(subtypeHashCode)) {
                if (DEBUG) {
                    Slogger::D(TAG, "Enabled subtype found in the history: %s", subtypeHashCode.string());
                }
                AutoPtr<IString> istr;
                CString::New(subtypeHashCode, (IString**)&istr);
                AutoPtr<IPair> pair;
                CPair::New(first, istr, (IPair**)&pair);
                return pair;
            }
        }
    }
    if (DEBUG) {
        Slogger::D(TAG, "No enabled IME found in the history");
    }
    return NULL;
}

String InputMethodUtils::InputMethodSettings::GetEnabledSubtypeHashCodeForInputMethodAndSubtypeLocked(
    /* [in] */ IList* enabledImes,
    /* [in] */ const String& imeId,
    /* [in] */ const String& subtypeHashCode)
{
    Int32 size;
    enabledImes->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        enabledImes->Get(i, (IInterface**)&item);
        AutoPtr<IPair> enabledIme = IPair::Probe(item);
        AutoPtr<IInterface> first, second;
        enabledIme->GetFirst((IInterface**)&first);
        enabledIme->GetSecond((IInterface**)&second);

        String strFirst;
        IObject::Probe(first)->ToString(&strFirst);
        if (strFirst.Equals(imeId)) {
            AutoPtr<IArrayList> explicitlyEnabledSubtypes = IArrayList::Probe(second);
            AutoPtr<IInterface> value;
            mMethodMap->Get(first, (IInterface**)&value);
            AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(value);
            Int32 subtypesSize;
            explicitlyEnabledSubtypes->GetSize(&subtypesSize);
            if (subtypesSize == 0) {
                // If there are no explicitly enabled subtypes, applicable subtypes are
                // enabled implicitly.
                // If IME is enabled and no subtypes are enabled, applicable subtypes
                // are enabled implicitly, so needs to treat them to be enabled.
                Int32 subtypeCount;
                if (imi != NULL && (imi->GetSubtypeCount(&subtypeCount), subtypeCount) > 0) {
                    AutoPtr<IArrayList> implicitlySelectedSubtypes =
                            GetImplicitlyApplicableSubtypesLocked(mRes, imi);
                    if (implicitlySelectedSubtypes != NULL) {
                        Int32 N;
                        implicitlySelectedSubtypes->GetSize(&N);
                        for (Int32 j = 0; j < N; ++j) {
                            AutoPtr<IInterface> item;
                            implicitlySelectedSubtypes->Get(j, (IInterface**)&item);
                            AutoPtr<IInputMethodSubtype> st = IInputMethodSubtype::Probe(item);
                            Int32 hashCode;
                            IObject::Probe(st)->GetHashCode(&hashCode);
                            if (StringUtils::ToString(hashCode).Equals(subtypeHashCode)) {
                                return subtypeHashCode;
                            }
                        }
                    }
                }
            }
            else {
                for (Int32 j = 0; j < subtypesSize; ++j) {
                    AutoPtr<IInterface> item;
                    explicitlyEnabledSubtypes->Get(j, (IInterface**)&item);
                    String s;
                    IObject::Probe(item)->ToString(&s);
                    if (s.Equals(subtypeHashCode)) {
                        // If both imeId and subtypeId are enabled, return subtypeId.
                        Int32 hashCode = StringUtils::ParseInt32(subtypeHashCode);
                        // Check whether the subtype id is valid or not
                        if (IsValidSubtypeId(imi, hashCode)) {
                            return s;
                        }
                        else {
                            return NOT_A_SUBTYPE_ID_STR;
                        }
                    }
                }
            }
            // If imeId was enabled but subtypeId was disabled.
            return NOT_A_SUBTYPE_ID_STR;
        }
    }
    // If both imeId and subtypeId are disabled, return NULL
    return String(NULL);
}

AutoPtr<IList> InputMethodUtils::InputMethodSettings::LoadInputMethodAndSubtypeHistoryLocked()
{
    AutoPtr<IList> imsList;
    CArrayList::New((IList**)&imsList);
    String subtypeHistoryStr = GetSubtypeHistoryStr();
    if (TextUtils::IsEmpty(subtypeHistoryStr)) {
        return imsList;
    }
    mInputMethodSplitter->SetString(subtypeHistoryStr);
    Boolean hasNext;
    while (mInputMethodSplitter->HasNext(&hasNext), hasNext) {
        String nextImsStr;
        mInputMethodSplitter->GetNext(&nextImsStr);
        mSubtypeSplitter->SetString(nextImsStr);
        if (mSubtypeSplitter->HasNext(&hasNext), hasNext) {
            String subtypeId = NOT_A_SUBTYPE_ID_STR;
            // The first element is ime id.
            String imeId;
            mSubtypeSplitter->GetNext(&imeId);
            while (mSubtypeSplitter->HasNext(&hasNext), hasNext) {
                mSubtypeSplitter->GetNext(&subtypeId);
                break;
            }
            AutoPtr<IString> first, second;
            CString::New(imeId, (IString**)&first);
            CString::New(subtypeId, (IString**)&second);
            AutoPtr<IPair> pair;
            CPair::New(first, second, (IPair**)&pair);
            imsList->Add(pair);
        }
    }
    return imsList;
}

String InputMethodUtils::InputMethodSettings::GetSubtypeHistoryStr()
{
    String str;
    Settings::Secure::GetStringForUser(
            mResolver, ISettingsSecure::INPUT_METHODS_SUBTYPE_HISTORY, mCurrentUserId, &str);
    if (DEBUG) {
        Slogger::D(TAG, "GetSubtypeHistoryStr: %s", str.string());
    }
    return str;
}

ECode InputMethodUtils::InputMethodSettings::PutSelectedInputMethod(
    /* [in] */ const String& imeId)
{
    if (DEBUG) {
        Slogger::D(TAG, "putSelectedInputMethodStr: %s, %d", imeId.string(), mCurrentUserId);
    }
    Boolean res;
    return Settings::Secure::PutStringForUser(
        mResolver, ISettingsSecure::DEFAULT_INPUT_METHOD, imeId, mCurrentUserId, &res);
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::PutSelectedSubtype(
    /* [in] */ Int32 subtypeId)
{
    if (DEBUG) {
        Slogger::D(TAG, "putSelectedInputMethodSubtypeStr: %d, %d", subtypeId, mCurrentUserId);
    }
    Boolean res;
    return Settings::Secure::PutInt32ForUser(mResolver, ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE,
            subtypeId, mCurrentUserId, &res);
}

ECode InputMethodUtils::InputMethodSettings::GetDisabledSystemInputMethods(
    /* [out] */ String* str)
{
    return Settings::Secure::GetStringForUser(
        mResolver, ISettingsSecure::DISABLED_SYSTEM_INPUT_METHODS, mCurrentUserId, str);
}

ECode InputMethodUtils::InputMethodSettings::GetSelectedInputMethod(
    /* [out] */ String* str)
{
    ECode ec = Settings::Secure::GetStringForUser(
        mResolver, ISettingsSecure::DEFAULT_INPUT_METHOD, mCurrentUserId, str);
    if (DEBUG) {
        Slogger::D(TAG, "GetSelectedInputMethodStr: %s, %d", str->string(), mCurrentUserId);
    }
    return ec;
}

ECode InputMethodUtils::InputMethodSettings::IsSubtypeSelected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetSelectedInputMethodSubtypeHashCode() != NOT_A_SUBTYPE_ID;
    return NOERROR;
}

Int32 InputMethodUtils::InputMethodSettings::GetSelectedInputMethodSubtypeHashCode()
{
    Int32 ret;
    if (FAILED(Settings::Secure::GetInt32ForUser(
        mResolver, ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE, mCurrentUserId, &ret))) {
        return NOT_A_SUBTYPE_ID;
    }
    return ret;
}

ECode InputMethodUtils::InputMethodSettings::IsShowImeWithHardKeyboardEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 ret;
    FAIL_RETURN(Settings::Secure::GetInt32ForUser(mResolver,
        ISettingsSecure::SHOW_IME_WITH_HARD_KEYBOARD, 0, mCurrentUserId, &ret))
    *result = ret == 1;
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::SetShowImeWithHardKeyboard(
    /* [in] */ Boolean show)
{
    Boolean res;
    return Settings::Secure::PutInt32ForUser(mResolver, ISettingsSecure::SHOW_IME_WITH_HARD_KEYBOARD,
            show ? 1 : 0, mCurrentUserId, &res);
}

ECode InputMethodUtils::InputMethodSettings::GetCurrentUserId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mCurrentUserId;
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::GetSelectedInputMethodSubtypeId(
    /* [in] */ const String& selectedImiId,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    AutoPtr<IString> key;
    CString::New(selectedImiId, (IString**)&key);
    AutoPtr<IInterface> value;
    mMethodMap->Get(key, (IInterface**)&value);
    AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(value);
    if (imi == NULL) {
        *id = NOT_A_SUBTYPE_ID;
        return NOERROR;
    }
    Int32 subtypeHashCode = GetSelectedInputMethodSubtypeHashCode();
    *id = GetSubtypeIdFromHashCode(imi, subtypeHashCode);
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::SaveCurrentInputMethodAndSubtypeToHistory(
    /* [in] */ const String& curMethodId,
    /* [in] */ IInputMethodSubtype* currentSubtype)
{
    String subtypeId = NOT_A_SUBTYPE_ID_STR;
    if (currentSubtype != NULL) {
        Int32 hashCode;
        IObject::Probe(currentSubtype)->GetHashCode(&hashCode);
        subtypeId = StringUtils::ToString(hashCode);
    }
    if (CanAddToLastInputMethod(currentSubtype)) {
        AddSubtypeToHistory(curMethodId, subtypeId);
    }
    return NOERROR;
}

ECode InputMethodUtils::InputMethodSettings::GetExplicitlyOrImplicitlyEnabledInputMethodsAndSubtypeListLocked(
    /* [in] */ IContext* context,
    /* [out] */ IHashMap** enabledInputMethodAndSubtypes)
{
    VALIDATE_NOT_NULL(enabledInputMethodAndSubtypes)
    CHashMap::New(enabledInputMethodAndSubtypes);
    AutoPtr<IList> list;
    GetEnabledInputMethodListLocked((IList**)&list);
    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> imi;
        list->Get(i, (IInterface**)&imi);
        AutoPtr<IList> value;
        GetEnabledInputMethodSubtypeListLocked(
            context, IInputMethodInfo::Probe(imi), TRUE, (IList**)&value);
        (*enabledInputMethodAndSubtypes)->Put(imi, value);
    }
    return NOERROR;
}

const Boolean InputMethodUtils::DEBUG = FALSE;
const Int32 InputMethodUtils::NOT_A_SUBTYPE_ID = -1;
const String InputMethodUtils::SUBTYPE_MODE_ANY;
const String InputMethodUtils::SUBTYPE_MODE_KEYBOARD("keyboard");
const String InputMethodUtils::SUBTYPE_MODE_VOICE("voice");

const String InputMethodUtils::TAG("InputMethodUtils");

static AutoPtr<ILocale> InitLocale(
    /* [in] */ const String& language)
{
    AutoPtr<ILocale> locale;
    CLocale::New(language, (ILocale**)&locale);
    return locale;
}
const AutoPtr<ILocale> InputMethodUtils::ENGLISH_LOCALE = InitLocale(String("en"));
const String InputMethodUtils::NOT_A_SUBTYPE_ID_STR = StringUtils::ToString(NOT_A_SUBTYPE_ID);
const String InputMethodUtils::TAG_ENABLED_WHEN_DEFAULT_IS_NOT_ASCII_CAPABLE("EnabledWhenDefaultIsNotAsciiCapable");
const String InputMethodUtils::TAG_ASCII_CAPABLE("AsciiCapable");

static AutoPtr<ArrayOf<ILocale*> > InitLocales()
{
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> en, us, uk;
    localeHelper->GetENGLISH((ILocale**)&en);
    localeHelper->GetUS((ILocale**)&us);
    localeHelper->GetUK((ILocale**)&uk);
    AutoPtr<ArrayOf<ILocale*> > locales = ArrayOf<ILocale*>::Alloc(3);
    locales->Set(0, en);
    locales->Set(1, us);
    locales->Set(2, uk);
    return locales;
}
const AutoPtr<ArrayOf<ILocale*> > InputMethodUtils::SEARCH_ORDER_OF_FALLBACK_LOCALES = InitLocales();

String InputMethodUtils::GetStackTrace()
{
    // StringBuilder sb = new StringBuilder();
    // try {
    //     throw new RuntimeException();
    // } catch (RuntimeException e) {
    //     final StackTraceElement[] frames = e.getStackTrace();
    //     // Start at 1 because the first frame is here and we don't care about it
    //     for (Int32 j = 1; j < frames.length; ++j) {
    //         sb.append(frames[j].toString() + "\n");
    //     }
    // }
    // return sb.toString();
    return String("GetStackTrace is not implemented");
}

String InputMethodUtils::GetApiCallStack()
{
    String apiCallStack("");
    // try {
    //     throw new RuntimeException();
    // } catch (RuntimeException e) {
    //     final StackTraceElement[] frames = e.getStackTrace();
    //     for (Int32 j = 1; j < frames.length; ++j) {
    //         final String tempCallStack = frames[j].toString();
    //         if (TextUtils::IsEmpty(apiCallStack)) {
    //             // Overwrite apiCallStack if it's empty
    //             apiCallStack = tempCallStack;
    //         }
    //         else if (tempCallStack.indexOf("Transact(") < 0) {
    //             // Overwrite apiCallStack if it's not a binder call
    //             apiCallStack = tempCallStack;
    //         }
    //         else {
    //             break;
    //         }
    //     }
    // }
    return apiCallStack;
}

Boolean InputMethodUtils::IsSystemIme(
    /* [in] */ IInputMethodInfo* inputMethod)
{
    AutoPtr<IServiceInfo> serviceInfo;
    inputMethod->GetServiceInfo((IServiceInfo**)&serviceInfo);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 flags;
    appInfo->GetFlags(&flags);
    return (flags & IApplicationInfo::FLAG_SYSTEM) != 0;
}

Boolean InputMethodUtils::IsSystemImeThatHasEnglishKeyboardSubtype(
    /* [in] */ IInputMethodInfo* imi)
{
    if (!IsSystemIme(imi)) {
        return FALSE;
    }
    String language;
    ENGLISH_LOCALE->GetLanguage(&language);
    return ContainsSubtypeOf(imi, language, SUBTYPE_MODE_KEYBOARD);
}

AutoPtr<ILocale> InputMethodUtils::GetFallbackLocaleForDefaultIme(
        /* [in] */ IArrayList* imis,
        /* [in] */ IContext* context)
{
    Int32 length = SEARCH_ORDER_OF_FALLBACK_LOCALES->GetLength();
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<ILocale> fallbackLocale = (*SEARCH_ORDER_OF_FALLBACK_LOCALES)[i];
        Int32 size;
        imis->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> item;
            imis->Get(i, (IInterface**)&item);
            AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(item);
            Boolean isDefault;
            if (IsSystemIme(imi) && (imi->IsDefault(context, &isDefault), isDefault) &&
                    ContainsSubtypeOf(imi, fallbackLocale, FALSE /* ignoreCountry */,
                            SUBTYPE_MODE_KEYBOARD)) {
                return fallbackLocale;
            }
        }
    }
    return NULL;
}

Boolean InputMethodUtils::IsSystemAuxilialyImeThatHasAutomaticSubtype(
    /* [in] */ IInputMethodInfo* imi)
{
    if (!IsSystemIme(imi)) {
        return FALSE;
    }
    Boolean isAuxiliaryIme;
    imi->IsAuxiliaryIme(&isAuxiliaryIme);
    if (!isAuxiliaryIme) {
        return FALSE;
    }
    Int32 subtypeCount;
    imi->GetSubtypeCount(&subtypeCount);
    for (Int32 i = 0; i < subtypeCount; ++i) {
        AutoPtr<IInputMethodSubtype> s;
        imi->GetSubtypeAt(i, (IInputMethodSubtype**)&s);
        Boolean enabled;
        s->OverridesImplicitlyEnabledSubtype(&enabled);
        if (enabled) {
            return TRUE;
        }
    }
    return FALSE;
}

AutoPtr<ILocale> InputMethodUtils::GetSystemLocaleFromContext(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    AutoPtr<IConfiguration> c;
    r->GetConfiguration((IConfiguration**)&c);
    AutoPtr<ILocale> locale;
    if (FAILED(c->GetLocale((ILocale**)&locale)))
        locale = NULL;
    return locale;
}

AutoPtr<IArrayList> InputMethodUtils::GetDefaultEnabledImes(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isSystemReady,
    /* [in] */ IArrayList* imis)
{
    // OK to store NULL in fallbackLocale because IsImeThatHasSubtypeOf() is NULL-tolerant.
    AutoPtr<ILocale> fallbackLocale = GetFallbackLocaleForDefaultIme(imis, context);

    if (!isSystemReady) {
        AutoPtr<IArrayList> retval;
        CArrayList::New((IArrayList**)&retval);
        Int32 size;
        imis->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> item;
            imis->Get(i, (IInterface**)&item);
            AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(item);
            Boolean isDefault;
            // TODO: We should check isAsciiCapable instead of relying on fallbackLocale.
            if (IsSystemIme(imi) && (imi->IsDefault(context, &isDefault), isDefault) &&
                    IsImeThatHasSubtypeOf(imi, fallbackLocale, FALSE /* ignoreCountry */,
                            SUBTYPE_MODE_KEYBOARD)) {
                retval->Add(imi);
            }
        }
        return retval;
    }

    // OK to store NULL in fallbackLocale because IsImeThatHasSubtypeOf() is NULL-tolerant.
    AutoPtr<ILocale> systemLocale = GetSystemLocaleFromContext(context);
    // TODO: Use LinkedHashSet to simplify the code.
    AutoPtr<IArrayList> retval;
    CArrayList::New((IArrayList**)&retval);
    Boolean systemLocaleKeyboardImeFound = FALSE;

    Int32 size;
    imis->GetSize(&size);
    // First, try to find IMEs with taking the system locale country into consideration.
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        imis->Get(i, (IInterface**)&item);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(item);
        Boolean isDefault;
        if (!IsSystemIme(imi) || !(imi->IsDefault(context, &isDefault), isDefault)) {
            continue;
        }
        Boolean isSystemLocaleKeyboardIme = IsImeThatHasSubtypeOf(imi, systemLocale,
                FALSE /* ignoreCountry */, SUBTYPE_MODE_KEYBOARD);
        // TODO: We should check isAsciiCapable instead of relying on fallbackLocale.
        // TODO: Use LinkedHashSet to simplify the code.
        if (isSystemLocaleKeyboardIme ||
                IsImeThatHasSubtypeOf(imi, fallbackLocale, FALSE /* ignoreCountry */,
                        SUBTYPE_MODE_ANY)) {
            retval->Add(imi);
        }
        systemLocaleKeyboardImeFound |= isSystemLocaleKeyboardIme;
    }

    // System locale country doesn't match any IMEs, try to find IMEs in a country-agnostic
    // way.
    if (!systemLocaleKeyboardImeFound) {
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> item;
            imis->Get(i, (IInterface**)&item);
            AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(item);
            Boolean isDefault;
            if (!IsSystemIme(imi) || !(imi->IsDefault(context, &isDefault), isDefault)) {
                continue;
            }
            if (IsImeThatHasSubtypeOf(imi, fallbackLocale, FALSE /* ignoreCountry */,
                    SUBTYPE_MODE_KEYBOARD)) {
                // IMEs that have fallback locale are already added in the previous loop. We
                // don't need to add them again here.
                // TODO: Use LinkedHashSet to simplify the code.
                continue;
            }
            if (IsImeThatHasSubtypeOf(imi, systemLocale, TRUE /* ignoreCountry */,
                    SUBTYPE_MODE_ANY)) {
                retval->Add(imi);
            }
        }
    }

    // If one or more auxiliary input methods are available, OK to stop populating the list.
    Int32 rSize;
    retval->GetSize(&rSize);
    for (Int32 i = 0; i < rSize; ++i) {
        AutoPtr<IInterface> item;
        retval->Get(i, (IInterface**)&item);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(item);
        Boolean isAuxiliaryIme;
        imi->IsAuxiliaryIme(&isAuxiliaryIme);
        if (isAuxiliaryIme) {
            return retval;
        }
    }
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> item;
        imis->Get(i, (IInterface**)&item);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(item);
        if (IsSystemAuxilialyImeThatHasAutomaticSubtype(imi)) {
            retval->Add(imi);
        }
    }
    return retval;
}

Boolean InputMethodUtils::IsImeThatHasSubtypeOf(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ ILocale* locale,
    /* [in] */ Boolean ignoreCountry,
    /* [in] */ const String& mode)
{
    if (locale == NULL) {
        return FALSE;
    }
    return ContainsSubtypeOf(imi, locale, ignoreCountry, mode);
}

Boolean InputMethodUtils::IsValidSystemDefaultIme(
    /* [in] */ Boolean isSystemReady,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IContext* context)
{
    if (!isSystemReady) {
        return FALSE;
    }
    if (!IsSystemIme(imi)) {
        return FALSE;
    }
    Int32 id;
    imi->GetIsDefaultResourceId(&id);
    if (id != 0) {
        do {
            Boolean isDefault;
            imi->IsDefault(context, &isDefault);
            AutoPtr<IResources> r;
            context->GetResources((IResources**)&r);
            AutoPtr<IConfiguration> c;
            r->GetConfiguration((IConfiguration**)&c);
            AutoPtr<ILocale> locale;
            if (FAILED(c->GetLocale((ILocale**)&locale)))
                break;

            String language;
            locale->GetLanguage(&language);
            if (isDefault && ContainsSubtypeOf(imi, language, SUBTYPE_MODE_ANY)) {
                return TRUE;
            }
        } while (0);
    }
    Int32 count;
    imi->GetSubtypeCount(&count);
    if (count == 0) {
        String packageName;
        imi->GetPackageName(&packageName);
        Slogger::W(TAG, "Found no subtypes in a system IME: %s", packageName.string());
    }
    return FALSE;
}

Boolean InputMethodUtils::ContainsSubtypeOf(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ ILocale* locale,
    /* [in] */ Boolean ignoreCountry,
    /* [in] */ const String& mode)
{
    Int32 N;
    imi->GetSubtypeCount(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInputMethodSubtype> subtype;
        imi->GetSubtypeAt(i, (IInputMethodSubtype**)&subtype);
        String strLocale;
        subtype->GetLocale(&strLocale);
        if (ignoreCountry) {
            AutoPtr<ILocale> subtypeLocale;
            CLocale::New(GetLanguageFromLocaleString(strLocale), (ILocale**)&subtypeLocale);
            String language1, language2;
            subtypeLocale->GetLanguage(&language1);
            locale->GetLanguage(&language2);
            if (!language1.Equals(language2)) {
                continue;
            }
        }
        else {
            // TODO: Use {@link Locale#toLanguageTag()} and
            // {@link Locale#forLanguageTag(languageTag)} instead.
            String str;
            IObject::Probe(locale)->ToString(&str);
            if (!TextUtils::Equals(strLocale, str)) {
                continue;
            }
        }
        String subtypeMode;
        if (mode == SUBTYPE_MODE_ANY || TextUtils::IsEmpty(mode) ||
            mode.EqualsIgnoreCase((subtype->GetMode(&subtypeMode), subtypeMode))) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean InputMethodUtils::ContainsSubtypeOf(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ const String& language,
    /* [in] */ const String& mode)
{
    Int32 N;
    imi->GetSubtypeCount(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInputMethodSubtype> subtype;
        imi->GetSubtypeAt(i, (IInputMethodSubtype**)&subtype);
        String locale;
        subtype->GetLocale(&locale);
        if (!locale.StartWith(language)) {
            continue;
        }
        String subtypeMode;
        if (mode == SUBTYPE_MODE_ANY || TextUtils::IsEmpty(mode) ||
            mode.EqualsIgnoreCase((subtype->GetMode(&subtypeMode), subtypeMode))) {
            return TRUE;
        }
    }
    return FALSE;
}

AutoPtr<IArrayList> InputMethodUtils::GetSubtypes(
    /* [in] */ IInputMethodInfo* imi)
{
    AutoPtr<IArrayList> subtypes;
    CArrayList::New((IArrayList**)&subtypes);
    Int32 subtypeCount;
    imi->GetSubtypeCount(&subtypeCount);
    for (Int32 i = 0; i < subtypeCount; ++i) {
        AutoPtr<IInputMethodSubtype> subtype;
        imi->GetSubtypeAt(i, (IInputMethodSubtype**)&subtype);
        subtypes->Add(subtype);
    }
    return subtypes;
}

AutoPtr<IArrayList> InputMethodUtils::GetOverridingImplicitlyEnabledSubtypes(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ const String& mode)
{
    AutoPtr<IArrayList> subtypes;
    CArrayList::New((IArrayList**)&subtypes);
    Int32 subtypeCount;
    imi->GetSubtypeCount(&subtypeCount);
    for (Int32 i = 0; i < subtypeCount; ++i) {
        AutoPtr<IInputMethodSubtype> subtype;
        imi->GetSubtypeAt(i, (IInputMethodSubtype**)&subtype);
        Boolean enabled;
        subtype->OverridesImplicitlyEnabledSubtype(&enabled);
        String subtypeMode;
        if (enabled && (subtype->GetMode(&subtypeMode), subtypeMode).Equals(mode)) {
            subtypes->Add(subtype);
        }
    }
    return subtypes;
}

AutoPtr<IInputMethodInfo> InputMethodUtils::GetMostApplicableDefaultIME(
    /* [in] */ IList* enabledImes)
{
    Boolean isEmpty;
    if (enabledImes == NULL || (enabledImes->IsEmpty(&isEmpty), isEmpty)) {
        return NULL;
    }
    // We'd prefer to fall back on a system IME, since that is safer.
    Int32 i;
    enabledImes->GetSize(&i);
    Int32 firstFoundSystemIme = -1;
    while (i > 0) {
        i--;
        AutoPtr<IInterface> item;
        enabledImes->Get(i, (IInterface**)&item);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(item);
        Boolean isAuxiliaryIme;
        imi->IsAuxiliaryIme(&isAuxiliaryIme);
        if (IsSystemImeThatHasEnglishKeyboardSubtype(imi) && !isAuxiliaryIme) {
            return imi;
        }
        if (firstFoundSystemIme < 0 && IsSystemIme(imi) && !isAuxiliaryIme) {
            firstFoundSystemIme = i;
        }
    }
    AutoPtr<IInterface> item;
    enabledImes->Get(Elastos::Core::Math::Max(firstFoundSystemIme, 0), (IInterface**)&item);
    return IInputMethodInfo::Probe(item);
}

Boolean InputMethodUtils::IsValidSubtypeId(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeHashCode)
{
    return GetSubtypeIdFromHashCode(imi, subtypeHashCode) != NOT_A_SUBTYPE_ID;
}

Int32 InputMethodUtils::GetSubtypeIdFromHashCode(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeHashCode)
{
    if (imi != NULL) {
        Int32 subtypeCount;
        imi->GetSubtypeCount(&subtypeCount);
        for (Int32 i = 0; i < subtypeCount; ++i) {
            AutoPtr<IInputMethodSubtype> ims;
            imi->GetSubtypeAt(i, (IInputMethodSubtype**)&ims);
            Int32 hashCode;
            IObject::Probe(ims)->GetHashCode(&hashCode);
            if (subtypeHashCode == hashCode) {
                return i;
            }
        }
    }
    return NOT_A_SUBTYPE_ID;
}

AutoPtr<IArrayList> InputMethodUtils::GetImplicitlyApplicableSubtypesLocked(
    /* [in] */ IResources* res,
    /* [in] */ IInputMethodInfo* imi)
{
    AutoPtr<IList> subtypes = IList::Probe(GetSubtypes(imi));
    AutoPtr<IConfiguration> c;
    res->GetConfiguration((IConfiguration**)&c);
    AutoPtr<ILocale> locale;
    c->GetLocale((ILocale**)&locale);
    String systemLocale;
    IObject::Probe(locale)->ToString(&systemLocale);
    AutoPtr<IArrayList> list;
    if (TextUtils::IsEmpty(systemLocale)) {
        CArrayList::New((IArrayList**)&list);
        return list;
    }
    String systemLanguage;
    locale->GetLanguage(&systemLanguage);
    AutoPtr<IHashMap> applicableModeAndSubtypesMap;
    CHashMap::New((IHashMap**)&applicableModeAndSubtypesMap);
    Int32 N;
    subtypes->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        // scan overriding implicitly enabled subtypes.
        AutoPtr<IInterface> item;
        subtypes->Get(i, (IInterface**)&item);
        AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(item);
        Boolean enabled;
        subtype->OverridesImplicitlyEnabledSubtype(&enabled);
        if (enabled) {
            String mode;
            subtype->GetMode(&mode);
            AutoPtr<IString> key;
            CString::New(mode, (IString**)&key);
            Boolean contains;
            applicableModeAndSubtypesMap->ContainsKey(key, &contains);
            if (!contains) {
                applicableModeAndSubtypesMap->Put(key, subtype);
            }
        }
    }
    Int32 size;
    applicableModeAndSubtypesMap->GetSize(&size);
    if (size > 0) {
        AutoPtr<ICollection> values;
        applicableModeAndSubtypesMap->GetValues((ICollection**)&values);
        CArrayList::New(values, (IArrayList**)&list);
        return list;
    }
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> item;
        subtypes->Get(i, (IInterface**)&item);
        AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(item);
        String locale;
        subtype->GetLocale(&locale);
        String mode;
        subtype->GetMode(&mode);
        String language = GetLanguageFromLocaleString(locale);
        // When system locale starts with subtype's locale, that subtype will be applicable
        // for system locale. We need to make sure the languages are the same, to prevent
        // locales like "fil" (Filipino) being matched by "fi" (Finnish).
        //
        // For instance, it's clearly applicable for cases like system locale = en_US and
        // subtype = en, but it is not necessarily considered applicable for cases like system
        // locale = en and subtype = en_US.
        //
        // We just call systemLocale.startsWith(locale) in this function because there is no
        // need to find applicable subtypes aggressively unlike
        // FindLastResortApplicableSubtypeLocked.
        //
        // TODO: This check is broken. It won't take scripts into account and doesn't
        // account for the mandatory conversions performed by Locale#toString.
        if (language.Equals(systemLanguage) && systemLocale.StartWith(locale)) {
            AutoPtr<IString> key;
            CString::New(mode, (IString**)&key);
            AutoPtr<IInterface> value;
            applicableModeAndSubtypesMap->Get(key, (IInterface**)&value);
            AutoPtr<IInputMethodSubtype> applicableSubtype = IInputMethodSubtype::Probe(value);
            // If more applicable subtypes are contained, skip.
            if (applicableSubtype != NULL) {
                String subtypeLocale;
                applicableSubtype->GetLocale(&subtypeLocale);
                if (systemLocale.Equals(subtypeLocale))
                    continue;
                if (!systemLocale.Equals(locale))
                    continue;
            }
            applicableModeAndSubtypesMap->Put(key, subtype);
        }
    }
    AutoPtr<IString> key;
    CString::New(SUBTYPE_MODE_KEYBOARD, (IString**)&key);
    AutoPtr<IInterface> value;
    applicableModeAndSubtypesMap->Get(key, (IInterface**)&value);
    AutoPtr<IInputMethodSubtype> keyboardSubtype = IInputMethodSubtype::Probe(value);

    AutoPtr<ICollection> values;
    applicableModeAndSubtypesMap->GetValues((ICollection**)&values);
    AutoPtr<IArrayList> applicableSubtypes;
    CArrayList::New(values, (IArrayList**)&applicableSubtypes);
    Boolean contains;
    if (keyboardSubtype != NULL && !(keyboardSubtype->ContainsExtraValueKey(
        TAG_ASCII_CAPABLE, &contains), contains)) {
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> item;
            subtypes->Get(i, (IInterface**)&item);
            AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(item);
            String mode;
            subtype->GetMode(&mode);

            if (SUBTYPE_MODE_KEYBOARD.Equals(mode) && (subtype->ContainsExtraValueKey(
                TAG_ENABLED_WHEN_DEFAULT_IS_NOT_ASCII_CAPABLE, &contains), contains)) {
                applicableSubtypes->Add(subtype);
            }
        }
    }
    if (keyboardSubtype == NULL) {
        AutoPtr<IInputMethodSubtype> lastResortKeyboardSubtype = FindLastResortApplicableSubtypeLocked(
                res, subtypes, SUBTYPE_MODE_KEYBOARD, systemLocale, TRUE);
        if (lastResortKeyboardSubtype != NULL) {
            applicableSubtypes->Add(lastResortKeyboardSubtype);
        }
    }
    return applicableSubtypes;
}

AutoPtr<IList> InputMethodUtils::GetEnabledInputMethodSubtypeList(
    /* [in] */ IContext* context,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IList* enabledSubtypes,
    /* [in] */ Boolean allowsImplicitlySelectedSubtypes)
{
    Boolean isEmpty;
    if (allowsImplicitlySelectedSubtypes && (enabledSubtypes->IsEmpty(&isEmpty), isEmpty)) {
        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r);
        enabledSubtypes = IList::Probe(GetImplicitlyApplicableSubtypesLocked(r, imi));
    }
    return CInputMethodSubtype::Sort(context, 0, imi, enabledSubtypes);
}

String InputMethodUtils::GetLanguageFromLocaleString(
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

AutoPtr<IInputMethodSubtype> InputMethodUtils::FindLastResortApplicableSubtypeLocked(
    /* [in] */ IResources* res,
    /* [in] */ IList* subtypes,
    /* [in] */ const String& mode,
    /* [in] */ const String& _locale,
    /* [in] */ Boolean canIgnoreLocaleAsLastResort)
{
    Int32 size;
    if (subtypes == NULL || (subtypes->GetSize(&size), size) == 0) {
        return NULL;
    }
    String locale = _locale;
    if (TextUtils::IsEmpty(locale)) {
        AutoPtr<IConfiguration> c;
        res->GetConfiguration((IConfiguration**)&c);
        AutoPtr<ILocale> l;
        c->GetLocale((ILocale**)&l);
        IObject::Probe(l)->ToString(&locale);
    }
    String language = GetLanguageFromLocaleString(locale);
    Boolean partialMatchFound = FALSE;
    AutoPtr<IInputMethodSubtype> applicableSubtype;
    AutoPtr<IInputMethodSubtype> firstMatchedModeSubtype;
    Int32 N;
    subtypes->GetSize(&N);
    for (Int32 i = 0; i < N; ++i) {
        AutoPtr<IInterface> item;
        subtypes->Get(i, (IInterface**)&item);
        AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(item);
        String subtypeLocale;
        subtype->GetLocale(&subtypeLocale);
        String subtypeLanguage = GetLanguageFromLocaleString(subtypeLocale);
        // An applicable subtype should match "mode". If mode is NULL, mode will be ignored,
        // and all subtypes with all modes can be candidates.
        String subtypeMode;
        subtype->GetMode(&subtypeMode);
        if (mode == NULL || subtypeMode.EqualsIgnoreCase(mode)) {
            if (firstMatchedModeSubtype == NULL) {
                firstMatchedModeSubtype = subtype;
            }
            if (locale.Equals(subtypeLocale)) {
                // Exact match (e.g. system locale is "en_US" and subtype locale is "en_US")
                applicableSubtype = subtype;
                break;
            }
            else if (!partialMatchFound && language.Equals(subtypeLanguage)) {
                // Partial match (e.g. system locale is "en_US" and subtype locale is "en")
                applicableSubtype = subtype;
                partialMatchFound = TRUE;
            }
        }
    }

    if (applicableSubtype == NULL && canIgnoreLocaleAsLastResort) {
        return firstMatchedModeSubtype;
    }

    // The first subtype applicable to the system locale will be defined as the most applicable
    // subtype.
    if (DEBUG) {
        if (applicableSubtype != NULL) {
            String mode, locale;
            applicableSubtype->GetMode(&mode);
            applicableSubtype->GetLocale(&locale);
            Slogger::D(TAG, "Applicable InputMethodSubtype was found: %s,%s",
                mode.string(), locale.string());
        }
    }
    return applicableSubtype;
}

Boolean InputMethodUtils::CanAddToLastInputMethod(
    /* [in] */ IInputMethodSubtype* subtype)
{
    if (subtype == NULL)
        return TRUE;
    Boolean isAuxiliary;
    subtype->IsAuxiliary(&isAuxiliary);
    return !isAuxiliary;
}

void InputMethodUtils::SetNonSelectedSystemImesDisabledUntilUsed(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IList* enabledImis)
{
    if (DEBUG) {
        Slogger::D(TAG, "setNonSelectedSystemImesDisabledUntilUsed");
    }
    AutoPtr<ArrayOf<String> > systemImesDisabledUntilUsed;
    CResources::GetSystem()->GetStringArray(
        R::array::config_disabledUntilUsedPreinstalledImes,
        (ArrayOf<String>**)&systemImesDisabledUntilUsed);
    if (systemImesDisabledUntilUsed == NULL || systemImesDisabledUntilUsed->GetLength() == 0) {
        return;
    }
    // Only the current spell checker should be treated as an enabled one.
    AutoPtr<ISpellCheckerInfo> currentSpellChecker;
    CTextServicesManager::GetInstance()->GetCurrentSpellChecker(
        (ISpellCheckerInfo**)&currentSpellChecker);
    for (Int32 i = 0; i < systemImesDisabledUntilUsed->GetLength(); i++ ) {
        String packageName = (*systemImesDisabledUntilUsed)[i];
        if (DEBUG) {
            Slogger::D(TAG, "check %s", packageName.string());
        }
        Boolean enabledIme = FALSE;
        Int32 size;
        enabledImis->GetSize(&size);
        for (Int32 j = 0; j < size; ++j) {
            AutoPtr<IInterface> item;
            enabledImis->Get(j, (IInterface**)&item);
            AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(item);
            String pkgName;
            imi->GetPackageName(&pkgName);
            if (packageName.Equals(pkgName)) {
                enabledIme = TRUE;
                break;
            }
        }
        if (enabledIme) {
            // enabled ime. skip
            continue;
        }
        String pkgName;
        if (currentSpellChecker != NULL &&
            packageName.Equals((currentSpellChecker->GetPackageName(&pkgName), pkgName))) {
            // enabled spell checker. skip
            if (DEBUG) {
                Slogger::D(TAG, "%s is the current spell checker. skip", packageName.string());
            }
            continue;
        }
        AutoPtr<IApplicationInfo> ai;
        if (FAILED(packageManager->GetApplicationInfo(packageName,
            IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS, (IApplicationInfo**)&ai))) {
            Slogger::W(TAG, "NameNotFoundException: %s", packageName.string());
        }

        if (ai == NULL) {
            // No app found for packageName
            continue;
        }
        Int32 flags;
        ai->GetFlags(&flags);
        Boolean isSystemPackage = (flags & IApplicationInfo::FLAG_SYSTEM) != 0;
        if (!isSystemPackage) {
            continue;
        }
        SetDisabledUntilUsed(packageManager, packageName);
    }
}

void InputMethodUtils::SetDisabledUntilUsed(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ const String& packageName)
{
    Int32 state;
    packageManager->GetApplicationEnabledSetting(packageName, &state);
    if (state == IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT
        || state == IPackageManager::COMPONENT_ENABLED_STATE_ENABLED) {
        if (DEBUG) {
            Slogger::D(TAG, "Update state(%s): DISABLED_UNTIL_USED", packageName.string());
        }
        packageManager->SetApplicationEnabledSetting(packageName,
                IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_UNTIL_USED, 0);
    }
    else {
        if (DEBUG) {
            Slogger::D(TAG, "%s is already DISABLED_UNTIL_USED", packageName.string());
        }
    }
}

AutoPtr<ICharSequence> InputMethodUtils::GetImeAndSubtypeDisplayName(
    /* [in] */ IContext* context,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype)
{
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<ICharSequence> imiLabel;
    imi->LoadLabel(packageManager, (ICharSequence**)&imiLabel);

    if (subtype != NULL) {
        String pkgName;
        imi->GetPackageName(&pkgName);
        AutoPtr<IServiceInfo> serviceInfo;
        imi->GetServiceInfo((IServiceInfo**)&serviceInfo);
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        AutoPtr<ICharSequence> subtypeLabel;
        subtype->GetDisplayName(
            context, pkgName, appInfo, (ICharSequence**)&subtypeLabel);

        String strImiLabel;
        imiLabel->ToString(&strImiLabel);
        String str = TextUtils::IsEmpty(imiLabel) ? String("") : (String(" - ") + strImiLabel);
        AutoPtr<ICharSequence> label;
        CString::New(str, (ICharSequence**)&label);
        AutoPtr<ArrayOf<ICharSequence*> > array = ArrayOf<ICharSequence*>::Alloc(2);
        array->Set(0, subtypeLabel);
        array->Set(1, label);
        return TextUtils::Concat(array);
    }
    return imiLabel;
}

Boolean InputMethodUtils::CheckIfPackageBelongsToUid(
    /* [in] */ IAppOpsManager* appOpsManager,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    if (appOpsManager->CheckPackage(uid, packageName) == (ECode)E_SECURITY_EXCEPTION)
        return FALSE;
    return TRUE;
}

} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos
