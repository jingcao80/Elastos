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

#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_INPUTMETHODANDSUBTYPEUTIL_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_INPUTMETHODANDSUBTYPEUTIL_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Text::ISimpleStringSplitter;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

// TODO: Consolidate this with {@link InputMethodSettingValuesWrapper}.
class InputMethodAndSubtypeUtil
{
public:
    // InputMethods and subtypes are saved in the settings as follows:
    // ime0;subtype0;subtype1:ime1;subtype0:ime2:ime3;subtype0;subtype1
    static CARAPI_(String) BuildInputMethodsAndSubtypesString(
        /* [in] */ IHashMap* imeToSubtypesMap);//HashMap<String, HashSet<String>>

    static CARAPI_(AutoPtr<IHashMap>) /*HashMap<String, HashSet<String>>*/ ParseInputMethodsAndSubtypesString(
        /* [in] */ const String& inputMethodsAndSubtypesString);

    static CARAPI_(void) EnableInputMethodSubtypesOf(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& imiId,
        /* [in] */ IHashSet* enabledSubtypeIdSet); //HashSet<String>

    static CARAPI_(void) SaveInputMethodSubtypeList(
        /* [in] */ SettingsPreferenceFragment* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IList* inputMethodInfos,/*List<InputMethodInfo>*/
        /* [in] */ Boolean hasHardKeyboard);

    static CARAPI_(void) LoadInputMethodSubtypeList(
        /* [in] */ SettingsPreferenceFragment* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IList* inputMethodInfos,/*List<InputMethodInfo>*/
        /* [in] */ IMap* inputMethodPrefsMap);/*Map<String, List<Preference>>*/

    static CARAPI_(void) SetSubtypesPreferenceEnabled(
        /* [in] */ SettingsPreferenceFragment* context,
        /* [in] */ IList* inputMethodProperties, /*List<InputMethodInfo>*/
        /* [in] */ const String& id,
        /* [in] */ Boolean enabled);

    static CARAPI_(void) RemoveUnnecessaryNonPersistentPreference(
        /* [in] */ IPreference* pref);

private:
    static CARAPI_(String) BuildInputMethodsString(
        /* [in] */ IHashSet* imiList);//HashSet<String>

    static CARAPI_(Int32) GetInputMethodSubtypeSelected(
        /* [in] */ IContentResolver* resolver);

    static CARAPI_(Boolean) IsInputMethodSubtypeSelected(
        /* [in] */ IContentResolver* resolver);

    static CARAPI_(void) PutSelectedInputMethodSubtype(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int32 hashCode);

    // Needs to modify InputMethodManageService if you want to change the format of saved string.
    static CARAPI_(AutoPtr<IHashMap>) /*HashMap<String, HashSet<String>>*/ GetEnabledInputMethodsAndSubtypeList(
        /* [in] */ IContentResolver* resolver);

    static CARAPI_(AutoPtr<IHashSet>) /*HashSet<String>*/ GetDisabledSystemIMEs(
        /* [in] */ IContentResolver* resolver);

    static CARAPI_(void) UpdateSubtypesPreferenceChecked(
        /* [in] */ SettingsPreferenceFragment* context,
        /* [in] */ IList* inputMethodProperties,/*List<InputMethodInfo> */
        /* [in] */ IHashMap* enabledSubtypes);/*HashMap<String, HashSet<String>>*/

public:
    static const String TAG;

private:
    static const Boolean DEBUG;

    static const Char32 INPUT_METHOD_SEPARATER;
    static const Char32 INPUT_METHOD_SUBTYPE_SEPARATER;
    static const Int32 NOT_A_SUBTYPE_ID;

    static const AutoPtr<ISimpleStringSplitter> sStringInputMethodSplitter;

    static const AutoPtr<ISimpleStringSplitter> sStringInputMethodSubtypeSplitter;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_INPUTMETHODANDSUBTYPEUTIL_H__
