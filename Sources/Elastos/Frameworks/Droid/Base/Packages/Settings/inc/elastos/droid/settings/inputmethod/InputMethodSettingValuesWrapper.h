#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_INPUTMETHODSETTINGVALUESWRAPPER_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_INPUTMETHODSETTINGVALUESWRAPPER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::InputMethod::IInputMethodSettings;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

/**
 * This class is a wrapper for InputMethodSettings. You need to refresh internal states
 * manually on some events when "InputMethodInfo"s and "InputMethodSubtype"s can be
 * changed.
 */
// TODO: Consolidate this with {@link InputMethodAndSubtypeUtil}.
class InputMethodSettingValuesWrapper
    : public Object
{
public:
    ~InputMethodSettingValuesWrapper();

    static CARAPI_(AutoPtr<InputMethodSettingValuesWrapper>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI_(void) RefreshAllInputMethodAndSubtypes();

    CARAPI_(AutoPtr<IList>)/*List<InputMethodInfo>*/ GetInputMethodList();

    CARAPI_(AutoPtr<ICharSequence>) GetCurrentInputMethodName(
        /* [in] */ IContext* context);

    CARAPI_(Boolean) IsAlwaysCheckedIme(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IContext* context);

    CARAPI_(Boolean) IsEnabledImi(
        /* [in] */ IInputMethodInfo* imi);

    CARAPI_(Boolean) IsValidSystemNonAuxAsciiCapableIme(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IContext* context);

private:
    // Ensure singleton
    InputMethodSettingValuesWrapper(
        /* [in] */ IContext* context);

    static CARAPI_(Int32) GetDefaultCurrentUserId();

    // TODO: Add a cts to ensure at least one AsciiCapableSubtypeEnabledImis exist
    CARAPI_(void) UpdateAsciiCapableEnabledImis();

    CARAPI_(Int32) GetEnabledValidSystemNonAuxAsciiCapableImeCount(
        /* [in] */ IContext* context);

private:
    static const String TAG;// = InputMethodSettingValuesWrapper.class->GetSimpleName();

    static /*volatile*/ AutoPtr<InputMethodSettingValuesWrapper> sInstance;
    // ArrayList<InputMethodInfo> mMethodList = new ArrayList<>();
    AutoPtr<IArrayList> mMethodList;
    // HashMap<String, InputMethodInfo> mMethodMap = new HashMap<>();
    AutoPtr<IHashMap> mMethodMap;
    AutoPtr<IInputMethodSettings> mSettings;
    AutoPtr<IInputMethodManager> mImm;
    // HashSet<InputMethodInfo> mAsciiCapableEnabledImis = new HashSet<>();
    AutoPtr<IHashSet> mAsciiCapableEnabledImis;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_INPUTMETHODSETTINGVALUESWRAPPER_H__
