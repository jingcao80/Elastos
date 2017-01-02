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
    static Object tagLock;
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
