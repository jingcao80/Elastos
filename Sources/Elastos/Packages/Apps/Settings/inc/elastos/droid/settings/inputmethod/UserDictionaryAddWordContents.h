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

#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_USERDICTIONARYADDWORDCONTENTS_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_USERDICTIONARYADDWORDCONTENTS_H__

#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IEditText;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

/**
 * A container class to factor common code to UserDictionaryAddWordFragment
 * and UserDictionaryAddWordActivity.
 */
class UserDictionaryAddWordContents
    : public Object
{
    friend class CUserDictionaryAddWordFragment;
public:
    class LocaleRenderer
        : public Object
    {
    public:
        // LocaleString may NOT be NULL.
        LocaleRenderer(
            /* [in] */ IContext* context,
            /* [in] */ const String& localeString);

        //@Override
        CARAPI ToString(
        /* [out] */ String* str);

        virtual CARAPI_(String) GetLocaleString();

        // "More languages..." is NULL ; "All languages" is the empty string.
        virtual CARAPI_(Boolean) IsMoreLanguages();

    private:
        String mLocaleString;
        String mDescription;
    };

public:
    TO_STRING_IMPL("UserDictionaryAddWordContents")

    // Helper method to get the list of locales to display for this word
    virtual CARAPI_(AutoPtr<IArrayList>)/*ArrayList<LocaleRenderer>*/ GetLocalesList(
        /* [in] */ IActivity* activity);

    virtual CARAPI_(String) GetCurrentUserDictionaryLocale();

protected:
    /* package */ UserDictionaryAddWordContents(
        /* [in] */ IView* view,
        /* [in] */ IBundle* args);

    /* package */ UserDictionaryAddWordContents(
        /* [in] */ IView* view,
        /* [in] */ UserDictionaryAddWordContents* oldInstanceToBeEdited);

    // locale may be NULL, this means default locale
    // It may also be the empty string, which means "all locales"
    /* package */ CARAPI_(void) UpdateLocale(
        /* [in] */ const String& locale);

    /* package */ virtual CARAPI_(void) SaveStateIntoBundle(
        /* [in] */ IBundle* outState);

    /* package */ virtual CARAPI_(void) Delete(
        /* [in] */ IContext* context);

    /* package */ virtual CARAPI_(Int32) Apply(
        /* [in] */ IContext* context,
        /* [in] */ IBundle* outParameters);

private:
    CARAPI_(Boolean) HasWord(
        /* [in] */ const String& word,
        /* [in] */ IContext* context);

    static CARAPI_(void) AddLocaleDisplayNameToList(
        /* [in] */ IContext* context,
        /* [in] */ IArrayList* list, //ArrayList<LocaleRenderer>
        /* [in] */ const String& locale);

public:
    static const String EXTRA_MODE;
    static const String EXTRA_WORD;
    static const String EXTRA_SHORTCUT;
    static const String EXTRA_LOCALE;
    static const String EXTRA_ORIGINAL_WORD;
    static const String EXTRA_ORIGINAL_SHORTCUT;

    static const Int32 MODE_EDIT;
    static const Int32 MODE_INSERT;

private:
    static const Int32 FREQUENCY_FOR_USER_DICTIONARY_ADDS;
    static const AutoPtr< ArrayOf<String> > HAS_WORD_PROJECTION;
    static const String HAS_WORD_SELECTION_ONE_LOCALE;
    static const String HAS_WORD_SELECTION_ALL_LOCALES;

    Int32 mMode; // Either MODE_EDIT or MODE_INSERT
    AutoPtr<IEditText> mWordEditText;
    AutoPtr<IEditText> mShortcutEditText;
    String mLocale;
    String mOldWord;
    String mOldShortcut;
    String mSavedWord;
    String mSavedShortcut;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_USERDICTIONARYADDWORDCONTENTS_H__