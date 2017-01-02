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

#ifndef __ELASTOS_DROID_INTERNAL_INPUTMETHOD_CINPUTMETHODUTILS_H__
#define __ELASTOS_DROID_INTERNAL_INPUTMETHOD_CINPUTMETHODUTILS_H__

#include "_Elastos_Droid_Internal_InputMethod_CInputMethodUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Core::ICharSequence;
using Elastos::Core::Singleton;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ILocale;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

CarClass(CInputMethodUtils)
    , public Singleton
    , public IInputMethodUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetStackTrace(
        /* [out] */ String* str);

    CARAPI GetApiCallStack(
        /* [out] */ String* str);

    CARAPI IsSystemIme(
        /* [in] */ IInputMethodInfo* inputMethod,
        /* [out] */ Boolean* result);

    CARAPI IsSystemImeThatHasEnglishKeyboardSubtype(
        /* [in] */ IInputMethodInfo* imi,
        /* [out] */ Boolean* result);

    CARAPI GetFallbackLocaleForDefaultIme(
        /* [in] */ IArrayList* imis,
        /* [in] */ IContext* context,
        /* [out] */ ILocale** locale);

    CARAPI GetSystemLocaleFromContext(
        /* [in] */ IContext* context,
        /* [out] */ ILocale** locale);

    CARAPI GetDefaultEnabledImes(
        /* [in] */ IContext* context,
        /* [in] */ Boolean isSystemReady,
        /* [in] */ IArrayList* imis,
        /* [out] */ IArrayList** arrayList);

    CARAPI IsImeThatHasSubtypeOf(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ ILocale* locale,
        /* [in] */ Boolean ignoreCountry,
        /* [in] */ const String& mode,
        /* [out] */ Boolean* result);

    CARAPI IsValidSystemDefaultIme(
        /* [in] */ Boolean isSystemReady,
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI ContainsSubtypeOf(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ ILocale* locale,
        /* [in] */ Boolean ignoreCountry,
        /* [in] */ const String& mode,
        /* [out] */ Boolean* result);

    CARAPI ContainsSubtypeOf(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ const String& language,
        /* [in] */ const String& mode,
        /* [out] */ Boolean* result);

    CARAPI GetSubtypes(
        /* [in] */ IInputMethodInfo* imi,
        /* [out] */ IArrayList** arrayList);

    CARAPI GetOverridingImplicitlyEnabledSubtypes(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ const String& mode,
        /* [out] */ IArrayList** arrayList);

    CARAPI GetMostApplicableDefaultIME(
        /* [in] */ IList* enabledImes,
        /* [out] */ IInputMethodInfo** imi);

    CARAPI IsValidSubtypeId(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Int32 subtypeHashCode,
        /* [out] */ Boolean* result);

    CARAPI GetSubtypeIdFromHashCode(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Int32 subtypeHashCode,
        /* [out] */ Int32* hashCode);

    CARAPI GetLanguageFromLocaleString(
        /* [in] */ const String& locale,
        /* [out] */ String* str);

    CARAPI FindLastResortApplicableSubtypeLocked(
        /* [in] */ IResources* res,
        /* [in] */ IList* subtypes,
        /* [in] */ const String& mode,
        /* [in] */ const String& locale,
        /* [in] */ Boolean canIgnoreLocaleAsLastResort,
        /* [out] */ IInputMethodSubtype** subtype);

    CARAPI CanAddToLastInputMethod(
        /* [in] */ IInputMethodSubtype* subtype,
        /* [out] */ Boolean* result);

    CARAPI SetNonSelectedSystemImesDisabledUntilUsed(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ IList* enabledImis);

    CARAPI GetImeAndSubtypeDisplayName(
        /* [in] */ IContext* context,
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype,
        /* [out] */ ICharSequence** displayName);

    CARAPI CheckIfPackageBelongsToUid(
        /* [in] */ IAppOpsManager* appOpsManager,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);
};

} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_INPUTMETHOD_CINPUTMETHODUTILS_H__
