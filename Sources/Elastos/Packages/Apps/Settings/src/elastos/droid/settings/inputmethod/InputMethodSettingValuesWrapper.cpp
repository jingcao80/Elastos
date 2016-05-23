
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/settings/inputmethod/InputMethodSettingValuesWrapper.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Internal::InputMethod::IInputMethodUtils;
using Elastos::Droid::Internal::InputMethod::CInputMethodUtils;
using Elastos::Droid::Internal::InputMethod::CInputMethodSettings;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const String InputMethodSettingValuesWrapper::TAG("InputMethodSettingValuesWrapper");

AutoPtr<InputMethodSettingValuesWrapper> InputMethodSettingValuesWrapper::sInstance;

InputMethodSettingValuesWrapper::InputMethodSettingValuesWrapper(
    /* [in] */ IContext* context)
{
    CArrayList::New((IArrayList**)&mMethodList);
    CHashMap::New((IHashMap**)&mMethodMap);
    CHashSet::New((IHashSet**)&mAsciiCapableEnabledImis);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    CInputMethodSettings::New(resources, resolver, mMethodMap, mMethodList,
            GetDefaultCurrentUserId(), (IInputMethodSettings**)&mSettings);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mImm = IInputMethodManager::Probe(obj);
    RefreshAllInputMethodAndSubtypes();
}

InputMethodSettingValuesWrapper::~InputMethodSettingValuesWrapper()
{}

AutoPtr<InputMethodSettingValuesWrapper> InputMethodSettingValuesWrapper::GetInstance(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        {    AutoLock syncLock(CoreUtils::Convert(TAG));
            if (sInstance == NULL) {
                sInstance = new InputMethodSettingValuesWrapper(context);
            }
        }
    }
    return sInstance;
}

Int32 InputMethodSettingValuesWrapper::GetDefaultCurrentUserId()
{
    // try {
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IUserInfo> userInfo;
    ECode ec = am->GetCurrentUser((IUserInfo**)&userInfo);
    if (SUCCEEDED(ec)) {
        Int32 id;
        userInfo->GetId(&id);
        return id;
    }
    // } catch (RemoteException e) {
    Slogger::W(TAG, "Couldn't get current user ID; guessing it's 0. 0x%08x", ec);
    // }
    return 0;
}

void InputMethodSettingValuesWrapper::RefreshAllInputMethodAndSubtypes()
{
    {    AutoLock syncLock(mMethodMap);
        mMethodList->Clear();
        mMethodMap->Clear();
        AutoPtr<IList> imms;
        mImm->GetInputMethodList((IList**)&imms);
        mMethodList->AddAll(ICollection::Probe(imms));
        Int32 size;
        imms->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            imms->Get(i, (IInterface**)&obj);
            AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);
            String strId;
            imi->GetId(&strId);
            mMethodMap->Put(CoreUtils::Convert(strId), imi);
        }
        UpdateAsciiCapableEnabledImis();
    }
}

void InputMethodSettingValuesWrapper::UpdateAsciiCapableEnabledImis()
{
    {    AutoLock syncLock(mMethodMap);
        mAsciiCapableEnabledImis->Clear();
        AutoPtr<IList> enabledImis;
        mSettings->GetEnabledInputMethodListLocked((IList**)&enabledImis);
        Int32 size;
        enabledImis->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            enabledImis->Get(i, (IInterface**)&obj);
            AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);
            Int32 subtypeCount;
            imi->GetSubtypeCount(&subtypeCount);
            for (Int32 j = 0; j < subtypeCount; ++j) {
                AutoPtr<IInputMethodSubtype> subtype;
                imi->GetSubtypeAt(j, (IInputMethodSubtype**)&subtype);
                String mode;
                subtype->GetMode(&mode);
                Boolean res;
                subtype->IsAsciiCapable(&res);
                if (IInputMethodUtils::SUBTYPE_MODE_KEYBOARD.EqualsIgnoreCase(mode)
                        && res) {
                    mAsciiCapableEnabledImis->Add(imi);
                    break;
                }
            }
        }
    }
}

AutoPtr<IList> InputMethodSettingValuesWrapper::GetInputMethodList()
{
    {    AutoLock syncLock(mMethodMap);
        return IList::Probe(mMethodList);
    }
    return NULL;
}

AutoPtr<ICharSequence> InputMethodSettingValuesWrapper::GetCurrentInputMethodName(
    /* [in] */ IContext* context)
{
    {    AutoLock syncLock(mMethodMap);
        String str;
        mSettings->GetSelectedInputMethod(&str);
        AutoPtr<IInterface> obj;
        mMethodMap->Get(CoreUtils::Convert(str), (IInterface**)&obj);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);
        if (imi == NULL) {
            Logger::W(TAG, "Invalid selected imi: %s", str.string());
            return CoreUtils::Convert("");
        }
        AutoPtr<IInputMethodSubtype> subtype;
        mImm->GetCurrentInputMethodSubtype((IInputMethodSubtype**)&subtype);
        AutoPtr<IInputMethodUtils> utils;
        CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&utils);
        AutoPtr<ICharSequence> seq;
        utils->GetImeAndSubtypeDisplayName(context, imi, subtype, (ICharSequence**)&seq);
        return seq;
    }
    return NULL;
}

Boolean InputMethodSettingValuesWrapper::IsAlwaysCheckedIme(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IContext* context)
{
    Boolean isEnabled = IsEnabledImi(imi);
    {    AutoLock syncLock(mMethodMap);
        AutoPtr<IList> list;
        mSettings->GetEnabledInputMethodListLocked((IList**)&list);
        Int32 size;
        list->GetSize(&size);
        if (size <= 1 && isEnabled) {
            return TRUE;
        }
    }

    Int32 enabledValidSystemNonAuxAsciiCapableImeCount =
            GetEnabledValidSystemNonAuxAsciiCapableImeCount(context);
    if (enabledValidSystemNonAuxAsciiCapableImeCount > 1) {
        return FALSE;
    }

    if (enabledValidSystemNonAuxAsciiCapableImeCount == 1 && !isEnabled) {
        return FALSE;
    }

    AutoPtr<IInputMethodUtils> utils;
    CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&utils);
    Boolean res;
    utils->IsSystemIme(imi, &res);

    if (!res) {
        return FALSE;
    }
    return IsValidSystemNonAuxAsciiCapableIme(imi, context);
}

Int32 InputMethodSettingValuesWrapper::GetEnabledValidSystemNonAuxAsciiCapableImeCount(
    /* [in] */ IContext* context)
{
    Int32 count = 0;
    AutoPtr<IList> enabledImis;
    {    AutoLock syncLock(mMethodMap);
        mSettings->GetEnabledInputMethodListLocked((IList**)&enabledImis);
    }

    Int32 size;
    enabledImis->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        enabledImis->Get(i, (IInterface**)&obj);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);
        if (IsValidSystemNonAuxAsciiCapableIme(imi, context)) {
            ++count;
        }
    }
    if (count == 0) {
        Logger::W(TAG, "No \"enabledValidSystemNonAuxAsciiCapableIme\"s found.");
    }
    return count;
}

Boolean InputMethodSettingValuesWrapper::IsEnabledImi(
    /* [in] */ IInputMethodInfo* imi)
{
    AutoPtr<IList> enabledImis;
    {    AutoLock syncLock(mMethodMap);
        mSettings->GetEnabledInputMethodListLocked((IList**)&enabledImis);
    }

    Int32 size;
    enabledImis->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        enabledImis->Get(i, (IInterface**)&obj);
        AutoPtr<IInputMethodInfo> tempImi = IInputMethodInfo::Probe(obj);
        String id1, id2;
        tempImi->GetId(&id1);
        imi->GetId(&id2);
        if (id1.Equals(id2)) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean InputMethodSettingValuesWrapper::IsValidSystemNonAuxAsciiCapableIme(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IContext* context)
{
    Boolean res;
    if (imi->IsAuxiliaryIme(&res), res) {
        return FALSE;
    }

    AutoPtr<IInputMethodUtils> utils;
    CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&utils);

    if (utils->IsValidSystemDefaultIme(TRUE /* isSystemReady */, imi, context, &res), res) {
        return TRUE;
    }
    if (mAsciiCapableEnabledImis->IsEmpty(&res), res) {
        Logger::W(TAG, "ascii capable subtype enabled imi not found. Fall back to English Keyboard subtype.");

        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        AutoPtr<ILocale> locale;
        helper->GetENGLISH((ILocale**)&locale);
        String language;
        locale->GetLanguage(&language);

        utils->ContainsSubtypeOf(imi, language,
                IInputMethodUtils::SUBTYPE_MODE_KEYBOARD, &res);
        return res;
    }
    mAsciiCapableEnabledImis->Contains(imi, &res);
    return res;
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos
