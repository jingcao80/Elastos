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

#ifndef __ELASTOS_DROID_PREFERENCE_EDITTEXTPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_EDITTEXTPREFERENCE_H__

#include <Elastos.Droid.Widget.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/preference/DialogPreference.h>

using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * A {@link Preference} that allows for string
* input.
* <p>
* It is a subclass of {@link DialogPreference} and shows the {@link EditText}
* in a dialog. This {@link EditText} can be modified either programmatically
* via {@link #getEditText()}, or through XML by setting any EditText
* attributes on the EditTextPreference.
* <p>
* This preference will store a string into the SharedPreferences.
* <p>
* See {@link android.R.styleable#EditText EditText Attributes}.
 */
class ECO_PUBLIC EditTextPreference
    : public DialogPreference
    , public IEditTextPreference
{
public:
    CAR_INTERFACE_DECL()

    EditTextPreference();

    virtual ~EditTextPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Saves the text to the {@link SharedPreferences}.
     *
     * @param text The text to save
     */
    CARAPI SetText(
        /* [in] */ const String& text);

    /**
     * Gets the text from the {@link SharedPreferences}.
     *
     * @return The current preference value.
     */
    CARAPI GetText(
        /* [out] */ String* text);

    /**
     * Returns the {@link EditText} widget that will be shown in the dialog.
     *
     * @return The {@link EditText} widget that will be shown in the dialog.
     */
    CARAPI GetEditText(
        /* [out] */ IEditText** editText);

    CARAPI ShouldDisableDependents(
        /* [out] */ Boolean* shouldDisableDependents);

protected:
    CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    /**
     * Adds the EditText widget of this preference to the dialog's view.
     *
     * @param dialogView The dialog view.
     */
    CARAPI OnAddEditTextToDialogView(
        /* [in] */ IView* dialogView,
        /* [in] */ IEditText* editText);

    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

    CARAPI OnGetDefaultValue(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    CARAPI OnSetInitialValue(
        /* [in] */ Boolean restoreValue,
        /* [in] */ IInterface* defaultValue);

    CARAPI NeedInputMethod(
        /* [out] */ Boolean* isNeed);

    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** state);

    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    /**
     * The edit text shown in the dialog.
     */
    AutoPtr<IEditText> mEditText;

    String mText;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_EDITTEXTPREFERENCE_H__
