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

#ifndef __ELASTOS_DROID_INTERNAL_INPUTMETHOD_INPUTMETHODUTILS_H__
#define __ELASTOS_DROID_INTERNAL_INPUTMETHOD_INPUTMETHODUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Text.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::Text::ISimpleStringSplitter;
using Elastos::Droid::Utility::IPair;
using Elastos::Core::ICharSequence;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ILocale;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

/**
 * InputMethodManagerUtils contains some static methods that provides IME informations.
 * This methods are supposed to be used in both the framework and the Settings application.
 */
class InputMethodUtils
{
public:
    /**
     * Utility class for putting and getting settings for InputMethod
     * TODO: Move all putters and getters of settings to this class.
     */
    class InputMethodSettings
        : public Object
        , public IInputMethodSettings
    {
    public:
        InputMethodSettings();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IResources* res,
            /* [in] */ IContentResolver* resolver,
            /* [in] */ IHashMap* methodMap,
            /* [in] */ IArrayList* methodList,
            /* [in] */ Int32 userId);

        CARAPI SetCurrentUserId(
            /* [in] */ Int32 userId);

        CARAPI SetCurrentProfileIds(
            /* [in] */ ArrayOf<Int32>* currentProfileIds);

        CARAPI IsCurrentProfile(
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

        CARAPI GetEnabledInputMethodListLocked(
            /* [out] */ IList** list);

        CARAPI GetEnabledInputMethodAndSubtypeHashCodeListLocked(
            /* [out] */ IList** list);

        CARAPI GetEnabledInputMethodSubtypeListLocked(
            /* [in] */ IContext* context,
            /* [in] */ IInputMethodInfo* imi,
            /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
            /* [out] */ IList** list);

        CARAPI GetEnabledInputMethodSubtypeListLocked(
            /* [in] */ IInputMethodInfo* imi,
            /* [out] */ IList** list);

        // At the initial boot, the settings for input methods are not set,
        // so we need to enable IME in that case.
        CARAPI EnableAllIMEsIfThereIsNoEnabledIME();

        CARAPI GetEnabledInputMethodsAndSubtypeListLocked(
            /* [out] */ IList** list);

        CARAPI AppendAndPutEnabledInputMethodLocked(
            /* [in] */ const String& id,
            /* [in] */ Boolean reloadInputMethodStr);

        /**
         * Build and put a string of EnabledInputMethods with removing specified Id.
         * @return the specified id was removed or not.
         */
        CARAPI BuildAndPutEnabledInputMethodsStrRemovingIdLocked(
            /* [in] */ IStringBuilder* builder,
            /* [in] */ IList* imsList,
            /* [in] */ const String& id,
            /* [out] */ Boolean* result);

        CARAPI GetEnabledInputMethodsStr(
            /* [out] */ String* str);

        CARAPI GetLastInputMethodAndSubtypeLocked(
            /* [out] */ IPair** pair);

        CARAPI GetLastSubtypeForInputMethodLocked(
            /* [in] */ const String& imeId,
            /* [out] */ String* str);

        CARAPI PutSelectedInputMethod(
            /* [in] */ const String& imeId);

        CARAPI PutSelectedSubtype(
            /* [in] */ Int32 subtypeId);

        CARAPI GetDisabledSystemInputMethods(
            /* [out] */ String* str);

        CARAPI GetSelectedInputMethod(
            /* [out] */ String* str);

        CARAPI IsSubtypeSelected(
            /* [out] */ Boolean* result);

        CARAPI IsShowImeWithHardKeyboardEnabled(
            /* [out] */ Boolean* result);

        CARAPI SetShowImeWithHardKeyboard(
            /* [in] */ Boolean show);

        CARAPI GetCurrentUserId(
            /* [out] */ Int32* id);

        CARAPI GetSelectedInputMethodSubtypeId(
            /* [in] */ const String& selectedImiId,
            /* [out] */ Int32* id);

        CARAPI SaveCurrentInputMethodAndSubtypeToHistory(
            /* [in] */ const String& curMethodId,
            /* [in] */ IInputMethodSubtype* currentSubtype);

        CARAPI GetExplicitlyOrImplicitlyEnabledInputMethodsAndSubtypeListLocked(
            /* [in] */ IContext* context,
            /* [out] */ IHashMap** enabledInputMethodAndSubtypes);

    private:
        static CARAPI_(void) BuildEnabledInputMethodsSettingString(
            /* [in] */ IStringBuilder* builder,
            /* [in] */ IPair* pair);

        CARAPI_(AutoPtr<IList>) CreateEnabledInputMethodListLocked(
            /* [in] */ IList* imsList);

        CARAPI_(AutoPtr<IList>) CreateEnabledInputMethodAndSubtypeHashCodeListLocked(
                /* [in] */ IList* imsList);

        CARAPI_(void) PutEnabledInputMethodsStr(
            /* [in] */ const String& str);

        CARAPI_(void) SaveSubtypeHistory(
            /* [in] */ IList* savedImes,
            /* [in] */ const String& newImeId,
            /* [in] */ const String& newSubtypeId);

        CARAPI_(void) AddSubtypeToHistory(
            /* [in] */ const String& imeId,
            /* [in] */ const String& subtypeId);

        CARAPI_(void) PutSubtypeHistoryStr(
            /* [in] */ const String& str);

        CARAPI_(AutoPtr<IPair>) GetLastSubtypeForInputMethodLockedInternal(
            /* [in] */ const String& imeId);

        CARAPI_(String) GetEnabledSubtypeHashCodeForInputMethodAndSubtypeLocked(
            /* [in] */ IList* enabledImes,
            /* [in] */ const String& imeId,
            /* [in] */ const String& subtypeHashCode);

        CARAPI_(AutoPtr<IList>) LoadInputMethodAndSubtypeHistoryLocked();

        CARAPI_(String) GetSubtypeHistoryStr();

        CARAPI_(Int32) GetSelectedInputMethodSubtypeHashCode();

    private:
        // The string for enabled input method is saved as follows:
        // example: ("ime0;subtype0;subtype1;subtype2:ime1:ime2;subtype0")
        static const Char32 INPUT_METHOD_SEPARATER;
        static const Char32 INPUT_METHOD_SUBTYPE_SEPARATER;

        AutoPtr<ISimpleStringSplitter> mInputMethodSplitter;
        AutoPtr<ISimpleStringSplitter> mSubtypeSplitter;

        AutoPtr<IResources> mRes;
        AutoPtr<IContentResolver> mResolver;
        AutoPtr<IHashMap> mMethodMap;
        AutoPtr<IArrayList> mMethodList;

        String mEnabledInputMethodsStrCache;
        Int32 mCurrentUserId;
        AutoPtr<ArrayOf<Int32> > mCurrentProfileIds;
    };

public:
    // ----------------------------------------------------------------------
    // Utilities for debug
    static CARAPI_(String) GetStackTrace();

    static CARAPI_(String) GetApiCallStack();
    // ----------------------------------------------------------------------

    static CARAPI_(Boolean) IsSystemIme(
        /* [in] */ IInputMethodInfo* inputMethod);

    /**
     * @deprecated Use {@link Locale} returned from
     * {@link #getFallbackLocaleForDefaultIme(ArrayList)} instead.
     */
    // @Deprecated
    static CARAPI_(Boolean) IsSystemImeThatHasEnglishKeyboardSubtype(
        /* [in] */ IInputMethodInfo* imi);

    static CARAPI_(AutoPtr<ILocale>) GetFallbackLocaleForDefaultIme(
            /* [in] */ IArrayList* imis,
            /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<ILocale>) GetSystemLocaleFromContext(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<IArrayList>) GetDefaultEnabledImes(
        /* [in] */ IContext* context,
        /* [in] */ Boolean isSystemReady,
        /* [in] */ IArrayList* imis);

    static CARAPI_(Boolean) IsImeThatHasSubtypeOf(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ ILocale* locale,
        /* [in] */ Boolean ignoreCountry,
        /* [in] */ const String& mode);

    /**
     * @deprecated Use {@link #IsSystemIme(InputMethodInfo)} and
     * {@link InputMethodInfo#isDefault(Context)} and
     * {@link #IsImeThatHasSubtypeOf(InputMethodInfo, Locale, Boolean, String))} instead.
     */
    //@Deprecated
    static CARAPI_(Boolean) IsValidSystemDefaultIme(
        /* [in] */ Boolean isSystemReady,
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) ContainsSubtypeOf(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ ILocale* locale,
        /* [in] */ Boolean ignoreCountry,
        /* [in] */ const String& mode);

    /**
     * @deprecated Use {@link #ContainsSubtypeOf(InputMethodInfo, Locale, Boolean, String)} instead.
     */
    //@Deprecated
    static CARAPI_(Boolean) ContainsSubtypeOf(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ const String& language,
        /* [in] */ const String& mode);

    static CARAPI_(AutoPtr<IArrayList>) GetSubtypes(
        /* [in] */ IInputMethodInfo* imi);

    static CARAPI_(AutoPtr<IArrayList>) GetOverridingImplicitlyEnabledSubtypes(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ const String& mode);

    static CARAPI_(AutoPtr<IInputMethodInfo>) GetMostApplicableDefaultIME(
        /* [in] */ IList* enabledImes);

    static CARAPI_(Boolean) IsValidSubtypeId(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Int32 subtypeHashCode);

    static CARAPI_(Int32) GetSubtypeIdFromHashCode(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Int32 subtypeHashCode);

    /**
     * Returns the language component of a given locale string.
     * TODO: Use {@link Locale#toLanguageTag()} and {@link Locale#forLanguageTag(languageTag)}
     */
    static CARAPI_(String) GetLanguageFromLocaleString(
        /* [in] */ const String& locale);

    /**
     * If there are no selected subtypes, tries finding the most applicable one according to the
     * given locale.
     * @param subtypes this function will search the most applicable subtype in subtypes
     * @param mode subtypes will be filtered by mode
     * @param locale subtypes will be filtered by locale
     * @param canIgnoreLocaleAsLastResort if this function can't find the most applicable subtype,
     * it will return the first subtype matched with mode
     * @return the most applicable subtypeId
     */
    static CARAPI_(AutoPtr<IInputMethodSubtype>) FindLastResortApplicableSubtypeLocked(
        /* [in] */ IResources* res,
        /* [in] */ IList* subtypes,
        /* [in] */ const String& mode,
        /* [in] */ const String& _locale,
        /* [in] */ Boolean canIgnoreLocaleAsLastResort);

    static CARAPI_(Boolean) CanAddToLastInputMethod(
        /* [in] */ IInputMethodSubtype* subtype);

    static CARAPI_(void) SetNonSelectedSystemImesDisabledUntilUsed(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ IList* enabledImis);

    static CARAPI_(AutoPtr<ICharSequence>) GetImeAndSubtypeDisplayName(
        /* [in] */ IContext* context,
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IInputMethodSubtype* subtype);

    /**
     * Returns TRUE if a package name belongs to a UID.
     *
     * <p>This is a simple wrapper of {@link AppOpsManager#checkPackage(Int32, String)}.</p>
     * @param appOpsManager the {@link AppOpsManager} object to be used for the validation.
     * @param uid the UID to be validated.
     * @param packageName the package name.
     * @return {@code TRUE} if the package name belongs to the UID.
     */
    static CARAPI_(Boolean) CheckIfPackageBelongsToUid(
        /* [in] */ IAppOpsManager* appOpsManager,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

private:
    InputMethodUtils();

    static CARAPI_(Boolean) IsSystemAuxilialyImeThatHasAutomaticSubtype(
        /* [in] */ IInputMethodInfo* imi);

    static CARAPI_(AutoPtr<IArrayList>) GetImplicitlyApplicableSubtypesLocked(
        /* [in] */ IResources* res,
        /* [in] */ IInputMethodInfo* imi);

    static CARAPI_(AutoPtr<IList>) GetEnabledInputMethodSubtypeList(
        /* [in] */ IContext* context,
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IList* enabledSubtypes,
        /* [in] */ Boolean allowsImplicitlySelectedSubtypes);

    static CARAPI_(void) SetDisabledUntilUsed(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ const String& packageName);

public:
    static const Boolean DEBUG;
    static const Int32 NOT_A_SUBTYPE_ID;
    static const String SUBTYPE_MODE_ANY;
    static const String SUBTYPE_MODE_KEYBOARD;
    static const String SUBTYPE_MODE_VOICE;

private:
    static const String TAG;
    static const AutoPtr<ILocale> ENGLISH_LOCALE;
    static const String NOT_A_SUBTYPE_ID_STR;
    static const String TAG_ENABLED_WHEN_DEFAULT_IS_NOT_ASCII_CAPABLE;
    static const String TAG_ASCII_CAPABLE;
    /**
     * Used in {@link #getFallbackLocaleForDefaultIme(ArrayList, Context)} to find the fallback IMEs
     * that are mainly used until the system becomes ready. Note that {@link Locale} in this array
     * is checked with {@link Locale#equals(Object)}, which means that {@code Locale.ENGLISH}
     * doesn't automatically match {@code Locale("en", "IN")}.
     */
    static const AutoPtr<ArrayOf<ILocale*> > SEARCH_ORDER_OF_FALLBACK_LOCALES;
};

} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_INPUTMETHOD_INPUTMETHODUTILS_H__
