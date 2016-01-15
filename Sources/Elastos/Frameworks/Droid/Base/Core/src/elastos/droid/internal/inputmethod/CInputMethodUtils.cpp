
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.View.h"
#include "elastos/droid/internal/inputmethod/InputMethodUtils.h"
#include "elastos/droid/internal/inputmethod/CInputMethodUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

CAR_INTERFACE_IMPL(CInputMethodUtils, Singleton, IInputMethodUtils)

CAR_SINGLETON_IMPL(CInputMethodUtils)

ECode CInputMethodUtils::GetStackTrace(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = InputMethodUtils::GetStackTrace();
    return NOERROR;
}

ECode CInputMethodUtils::GetApiCallStack(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = InputMethodUtils::GetApiCallStack();
    return NOERROR;
}

ECode CInputMethodUtils::IsSystemIme(
    /* [in] */ IInputMethodInfo* inputMethod,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = InputMethodUtils::IsSystemIme(inputMethod);
    return NOERROR;
}

ECode CInputMethodUtils::IsSystemImeThatHasEnglishKeyboardSubtype(
    /* [in] */ IInputMethodInfo* imi,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = InputMethodUtils::IsSystemImeThatHasEnglishKeyboardSubtype(imi);
    return NOERROR;
}

ECode CInputMethodUtils::GetFallbackLocaleForDefaultIme(
    /* [in] */ IArrayList* imis,
    /* [in] */ IContext* context,
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    AutoPtr<ILocale> temp = InputMethodUtils::GetFallbackLocaleForDefaultIme(imis, context);
    *locale = temp;
    REFCOUNT_ADD(*locale)
    return NOERROR;
}

ECode CInputMethodUtils::GetSystemLocaleFromContext(
    /* [in] */ IContext* context,
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    AutoPtr<ILocale> temp = InputMethodUtils::GetSystemLocaleFromContext(context);
    *locale = temp;
    REFCOUNT_ADD(*locale)
    return NOERROR;
}

ECode CInputMethodUtils::GetDefaultEnabledImes(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isSystemReady,
    /* [in] */ IArrayList* imis,
    /* [out] */ IArrayList** arrayList)
{
    VALIDATE_NOT_NULL(arrayList)
    AutoPtr<IArrayList> temp = InputMethodUtils::GetDefaultEnabledImes(context, isSystemReady, imis);
    *arrayList = temp;
    REFCOUNT_ADD(*arrayList)
    return NOERROR;
}

ECode CInputMethodUtils::IsImeThatHasSubtypeOf(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ ILocale* locale,
    /* [in] */ Boolean ignoreCountry,
    /* [in] */ const String& mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    InputMethodUtils::IsImeThatHasSubtypeOf(imi, locale, ignoreCountry, mode);
    return NOERROR;
}

ECode CInputMethodUtils::IsValidSystemDefaultIme(
    /* [in] */ Boolean isSystemReady,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = InputMethodUtils::IsValidSystemDefaultIme(isSystemReady, imi, context);
    return NOERROR;
}

ECode CInputMethodUtils::ContainsSubtypeOf(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ ILocale* locale,
    /* [in] */ Boolean ignoreCountry,
    /* [in] */ const String& mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = InputMethodUtils::ContainsSubtypeOf(imi, locale, ignoreCountry, mode);
    return NOERROR;
}

ECode CInputMethodUtils::ContainsSubtypeOf(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ const String& language,
    /* [in] */ const String& mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = InputMethodUtils::ContainsSubtypeOf(imi, language, mode);
    return NOERROR;
}

ECode CInputMethodUtils::GetSubtypes(
    /* [in] */ IInputMethodInfo* imi,
    /* [out] */ IArrayList** arrayList)
{
    VALIDATE_NOT_NULL(arrayList)
    AutoPtr<IArrayList> temp = InputMethodUtils::GetSubtypes(imi);
    *arrayList = temp;
    REFCOUNT_ADD(*arrayList)
    return NOERROR;
}

ECode CInputMethodUtils::GetOverridingImplicitlyEnabledSubtypes(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ const String& mode,
    /* [out] */ IArrayList** arrayList)
{
    VALIDATE_NOT_NULL(arrayList)
    AutoPtr<IArrayList> temp = InputMethodUtils::GetOverridingImplicitlyEnabledSubtypes(imi, mode);
    *arrayList = temp;
    REFCOUNT_ADD(*arrayList)
    return NOERROR;
}

ECode CInputMethodUtils::GetMostApplicableDefaultIME(
    /* [in] */ IList* enabledImes,
    /* [out] */ IInputMethodInfo** imi)
{
    VALIDATE_NOT_NULL(imi)
    AutoPtr<IInputMethodInfo> temp = InputMethodUtils::GetMostApplicableDefaultIME(enabledImes);
    *imi = temp;
    REFCOUNT_ADD(*imi)
    return NOERROR;
}

ECode CInputMethodUtils::IsValidSubtypeId(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeHashCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = InputMethodUtils::IsValidSubtypeId(imi, subtypeHashCode);
    return NOERROR;
}

ECode CInputMethodUtils::GetSubtypeIdFromHashCode(
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Int32 subtypeHashCode,
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = InputMethodUtils::GetSubtypeIdFromHashCode(imi, subtypeHashCode);
    return NOERROR;
}

ECode CInputMethodUtils::GetLanguageFromLocaleString(
    /* [in] */ const String& locale,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = InputMethodUtils::GetLanguageFromLocaleString(locale);
    return NOERROR;
}

ECode CInputMethodUtils::FindLastResortApplicableSubtypeLocked(
    /* [in] */ IResources* res,
    /* [in] */ IList* subtypes,
    /* [in] */ const String& mode,
    /* [in] */ const String& locale,
    /* [in] */ Boolean canIgnoreLocaleAsLastResort,
    /* [out] */ IInputMethodSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype)
    AutoPtr<IInputMethodSubtype> temp = InputMethodUtils::FindLastResortApplicableSubtypeLocked(
        res, subtypes, mode, locale, canIgnoreLocaleAsLastResort);
    *subtype = temp;
    REFCOUNT_ADD(*subtype)
    return NOERROR;
}

ECode CInputMethodUtils::CanAddToLastInputMethod(
    /* [in] */ IInputMethodSubtype* subtype,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = InputMethodUtils::CanAddToLastInputMethod(subtype);
    return NOERROR;
}

ECode CInputMethodUtils::SetNonSelectedSystemImesDisabledUntilUsed(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IList* enabledImis)
{
    InputMethodUtils::SetNonSelectedSystemImesDisabledUntilUsed(packageManager, enabledImis);
    return NOERROR;
}

ECode CInputMethodUtils::GetImeAndSubtypeDisplayName(
    /* [in] */ IContext* context,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IInputMethodSubtype* subtype,
    /* [out] */ ICharSequence** displayName)
{
    VALIDATE_NOT_NULL(displayName)
    AutoPtr<ICharSequence> temp = InputMethodUtils::GetImeAndSubtypeDisplayName(context, imi, subtype);
    *displayName = temp;
    REFCOUNT_ADD(*displayName)
    return NOERROR;
}

ECode CInputMethodUtils::CheckIfPackageBelongsToUid(
    /* [in] */ IAppOpsManager* appOpsManager,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = InputMethodUtils::CheckIfPackageBelongsToUid(appOpsManager, uid, packageName);
    return NOERROR;
}

} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos
