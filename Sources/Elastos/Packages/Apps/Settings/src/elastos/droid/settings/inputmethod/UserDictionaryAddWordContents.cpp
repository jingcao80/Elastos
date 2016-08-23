
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/inputmethod/UserDictionaryAddWordContents.h"
#include "elastos/droid/settings/inputmethod/UserDictionaryList.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/settings/CUserDictionarySettings.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Provider::CUserDictionaryWords;
using Elastos::Droid::Provider::IUserDictionaryWords;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ITreeSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const String UserDictionaryAddWordContents::EXTRA_MODE("mode");
const String UserDictionaryAddWordContents::EXTRA_WORD("word");
const String UserDictionaryAddWordContents::EXTRA_SHORTCUT("shortcut");
const String UserDictionaryAddWordContents::EXTRA_LOCALE("locale");
const String UserDictionaryAddWordContents::EXTRA_ORIGINAL_WORD("originalWord");
const String UserDictionaryAddWordContents::EXTRA_ORIGINAL_SHORTCUT("originalShortcut");

const Int32 UserDictionaryAddWordContents::MODE_EDIT = 0;
const Int32 UserDictionaryAddWordContents::MODE_INSERT = 1;

const Int32 UserDictionaryAddWordContents::FREQUENCY_FOR_USER_DICTIONARY_ADDS = 250;

static AutoPtr< ArrayOf<String> > InitHAS_WORD_PROJECTION()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IUserDictionaryWords::WORD;
    return args;
}

const AutoPtr< ArrayOf<String> > UserDictionaryAddWordContents::HAS_WORD_PROJECTION = InitHAS_WORD_PROJECTION();

static String InitHAS_WORD_SELECTION_ONE_LOCALE()
{
    StringBuilder builder;
    builder += IUserDictionaryWords::WORD;
    builder += "=? AND ";
    builder += IUserDictionaryWords::LOCALE;
    builder += "=?";
    return builder.ToString();
}

const String UserDictionaryAddWordContents::HAS_WORD_SELECTION_ONE_LOCALE = InitHAS_WORD_SELECTION_ONE_LOCALE();
static String InitHAS_WORD_SELECTION_ALL_LOCALES()
{
    StringBuilder builder;
    builder += IUserDictionaryWords::WORD;
    builder += "=? AND ";
    builder += IUserDictionaryWords::LOCALE;
    builder += " is NULL";
    return builder.ToString();
}

const String UserDictionaryAddWordContents::HAS_WORD_SELECTION_ALL_LOCALES = InitHAS_WORD_SELECTION_ALL_LOCALES();

//===============================================================================
//                  UserDictionaryAddWordContents::LocaleRenderer
//===============================================================================

UserDictionaryAddWordContents::LocaleRenderer::LocaleRenderer(
    /* [in] */ IContext* context,
    /* [in] */ const String& localeString)
{
    mLocaleString = localeString;
    if (localeString.IsNull()) {
        context->GetString(R::string::user_dict_settings_more_languages, &mDescription);
    }
    else if (String("").Equals(localeString)) {
        context->GetString(R::string::user_dict_settings_all_languages, &mDescription);
    }
    else {
        Utils::CreateLocaleFromString(localeString)->GetDisplayName(&mDescription);
    }
}

ECode UserDictionaryAddWordContents::LocaleRenderer::ToString(
    /* [out] */ String* str)
{
    *str = mDescription;
    return NOERROR;
}

String UserDictionaryAddWordContents::LocaleRenderer::GetLocaleString()
{
    return mLocaleString;
}

Boolean UserDictionaryAddWordContents::LocaleRenderer::IsMoreLanguages()
{
    return mLocaleString.IsNull();
}

//===============================================================================
//                  UserDictionaryAddWordContents
//===============================================================================

UserDictionaryAddWordContents::UserDictionaryAddWordContents(
    /* [in] */ IView* view,
    /* [in] */ IBundle* args)
    : mMode(0)
{
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::user_dictionary_add_word_text, (IView**)&tmp);
    mWordEditText = IEditText::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::user_dictionary_add_shortcut, (IView**)&tmp);
    mShortcutEditText = IEditText::Probe(tmp);
    String word;
    args->GetString(EXTRA_WORD, &word);
    if (!word.IsNull()) {
        ITextView* wordEditText = ITextView::Probe(mWordEditText);
        wordEditText->SetText(CoreUtils::Convert(word));
        // Use getText in case the edit text modified the text we set. This happens when
        // it's too Int64 to be edited.
        AutoPtr<ICharSequence> cs;
        wordEditText->GetText((ICharSequence**)&cs);
        Int32 length;
        cs->GetLength(&length);
        mWordEditText->SetSelection(length);
    }
    String shortcut;
    args->GetString(EXTRA_SHORTCUT, &shortcut);
    if (!shortcut.IsNull() && NULL != mShortcutEditText) {
        ITextView::Probe(mShortcutEditText)->SetText(CoreUtils::Convert(shortcut));
    }
    args->GetInt32(EXTRA_MODE, &mMode); // default return value for #GetInt() is 0 = MODE_EDIT
    args->GetString(EXTRA_WORD, &mOldWord);
    args->GetString(EXTRA_SHORTCUT, &mOldShortcut);
    String str;
    args->GetString(EXTRA_LOCALE, &str);
    UpdateLocale(str);
}

UserDictionaryAddWordContents::UserDictionaryAddWordContents(
    /* [in] */ IView* view,
    /* [in] */ UserDictionaryAddWordContents* oldInstanceToBeEdited)
    : mMode(0)
{
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::user_dictionary_add_word_text, (IView**)&tmp);
    mWordEditText = IEditText::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::user_dictionary_add_shortcut, (IView**)&tmp);
    mShortcutEditText = IEditText::Probe(tmp);
    mMode = MODE_EDIT;
    mOldWord = oldInstanceToBeEdited->mSavedWord;
    mOldShortcut = oldInstanceToBeEdited->mSavedShortcut;
    UpdateLocale(mLocale);
}

void UserDictionaryAddWordContents::UpdateLocale(
    /* [in] */ const String& locale)
{
    if (locale.IsNull()) {
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        AutoPtr<ILocale> defaultLocale;
        helper->GetDefault((ILocale**)&defaultLocale);
        defaultLocale->ToString(&mLocale);
    }
    else {
        mLocale = locale;
    }
}

void UserDictionaryAddWordContents::SaveStateIntoBundle(
    /* [in] */ IBundle* outState)
{
    AutoPtr<ICharSequence> cs;
    ITextView::Probe(mWordEditText)->GetText((ICharSequence**)&cs);
    String str;
    cs->ToString(&str);
    outState->PutString(EXTRA_WORD, str);
    outState->PutString(EXTRA_ORIGINAL_WORD, mOldWord);
    if (NULL != mShortcutEditText) {
        cs = NULL;
        ITextView::Probe(mShortcutEditText)->GetText((ICharSequence**)&cs);
        cs->ToString(&str);
        outState->PutString(EXTRA_SHORTCUT, str);
    }
    if (!mOldShortcut.IsNull()) {
        outState->PutString(EXTRA_ORIGINAL_SHORTCUT, mOldShortcut);
    }
    outState->PutString(EXTRA_LOCALE, mLocale);
}

void UserDictionaryAddWordContents::Delete(
    /* [in] */ IContext* context)
{
    if (MODE_EDIT == mMode && !TextUtils::IsEmpty(mOldWord)) {
        // Mode edit: remove the old entry.
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        CUserDictionarySettings::DeleteWord(mOldWord, mOldShortcut, resolver);
    }
    // If we are in add mode, nothing was added, so we don't need to do anything.
}

Int32 UserDictionaryAddWordContents::Apply(
    /* [in] */ IContext* context,
    /* [in] */ IBundle* outParameters)
{
    if (NULL != outParameters) SaveStateIntoBundle(outParameters);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    if (MODE_EDIT == mMode && !TextUtils::IsEmpty(mOldWord)) {
        // Mode edit: remove the old entry.
        CUserDictionarySettings::DeleteWord(mOldWord, mOldShortcut, resolver);
    }

    AutoPtr<ICharSequence> cs;
    ITextView::Probe(mWordEditText)->GetText((ICharSequence**)&cs);
    String newWord;
    cs->ToString(&newWord);
    String newShortcut;
    if (NULL == mShortcutEditText) {
        newShortcut = String(NULL);
    }
    else {
        cs = NULL;
        ITextView::Probe(mShortcutEditText)->GetText((ICharSequence**)&cs);
        String tmpShortcut;
        cs->ToString(&tmpShortcut);
        if (TextUtils::IsEmpty(tmpShortcut)) {
            newShortcut = String(NULL);
        }
        else {
            newShortcut = tmpShortcut;
        }
    }
    if (TextUtils::IsEmpty(newWord)) {
        // If the word is somehow empty, don't insert it.
        return 1;//UserDictionaryAddWordActivity.CODE_CANCEL;
    }
    mSavedWord = newWord;
    mSavedShortcut = newShortcut;
    // If there is no shortcut, and the word already exists in the database, then we
    // should not insert, because either A. the word exists with no shortcut, in which
    // case the exact same thing we want to insert is already there, or B. the word
    // exists with at least one shortcut, in which case it has priority on our word.
    if (TextUtils::IsEmpty(newShortcut) && HasWord(newWord, context)) {
        return 2;//UserDictionaryAddWordActivity.CODE_ALREADY_PRESENT;
    }

    // Disallow duplicates. If the same word with no shortcut is defined, remove it; if
    // the same word with the same shortcut is defined, remove it; but we don't mind if
    // there is the same word with a different, non-empty shortcut.
    CUserDictionarySettings::DeleteWord(newWord, String(NULL), resolver);
    if (!TextUtils::IsEmpty(newShortcut)) {
        // If newShortcut is empty we just deleted this, no need to do it again
        CUserDictionarySettings::DeleteWord(newWord, newShortcut, resolver);
    }

    // In this class we use the empty string to represent 'all locales' and mLocale cannot
    // be NULL. However the addWord method takes NULL to mean 'all locales'.
    AutoPtr<IUserDictionaryWords> udw;
    CUserDictionaryWords::AcquireSingleton((IUserDictionaryWords**)&udw);
    udw->AddWord(context, newWord,
            FREQUENCY_FOR_USER_DICTIONARY_ADDS, newShortcut,
            TextUtils::IsEmpty(mLocale) ? NULL : Utils::CreateLocaleFromString(mLocale));

    return 0;//UserDictionaryAddWordActivity.CODE_WORD_ADDED;
}

Boolean UserDictionaryAddWordContents::HasWord(
    /* [in] */ const String& word,
    /* [in] */ IContext* context)
{
    AutoPtr<ICursor> cursor;
    // mLocale == "" indicates this is an entry for all languages. Here, mLocale can't
    // be NULL at all (it's ensured by the updateLocale method).

    AutoPtr<IUserDictionaryWords> udw;
    CUserDictionaryWords::AcquireSingleton((IUserDictionaryWords**)&udw);
    AutoPtr<IUri> wordsUri;
    udw->GetCONTENT_URI((IUri**)&wordsUri);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    if (String("").Equals(mLocale)) {
        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = word;
        resolver->Query(wordsUri,
                HAS_WORD_PROJECTION, HAS_WORD_SELECTION_ALL_LOCALES,
                args, String(NULL) /* sort order */, (ICursor**)&cursor);
    }
    else {
        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
        (*args)[0] = word;
        (*args)[1] = mLocale;
        resolver->Query(wordsUri,
                HAS_WORD_PROJECTION, HAS_WORD_SELECTION_ONE_LOCALE,
                args, String(NULL) /* sort order */, (ICursor**)&cursor);
    }

    // try {
        if (NULL == cursor) return FALSE;
        Int32 count;
        cursor->GetCount(&count);
        ICloseable::Probe(cursor)->Close();
        return count > 0;
    // } finally {
        // if (NULL != cursor) cursor->Close();
    // }
}

void UserDictionaryAddWordContents::AddLocaleDisplayNameToList(
    /* [in] */ IContext* context,
    /* [in] */ IArrayList* list, //ArrayList<LocaleRenderer>
    /* [in] */ const String& locale)
{
    if (NULL != locale) {
        AutoPtr<LocaleRenderer> lr = new LocaleRenderer(context, locale);
        list->Add((IObject*)lr);
    }
}

AutoPtr<IArrayList> UserDictionaryAddWordContents::GetLocalesList(
    /* [in] */ IActivity* activity)
{
    AutoPtr<ITreeSet> locales = UserDictionaryList::GetUserDictionaryLocalesSet(IContext::Probe(activity));
    // Remove our locale if it's in, because we're always gonna put it at the top
    locales->Remove(CoreUtils::Convert(mLocale)); // mLocale may not be NULL

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> defaultLocale;
    helper->GetDefault((ILocale**)&defaultLocale);
    String systemLocale;
    defaultLocale->ToString(&systemLocale);
    // The system locale should be inside. We want it at the 2nd spot.
    locales->Remove(CoreUtils::Convert(systemLocale)); // system locale may not be NULL
    locales->Remove(CoreUtils::Convert("")); // Remove the empty string if it's there
    AutoPtr<IArrayList> localesList;
    CArrayList::New((IArrayList**)&localesList);
    // Add the passed locale, then the system locale at the top of the list. Add an
    // "all languages" entry at the bottom of the list.
    AddLocaleDisplayNameToList(IContext::Probe(activity), localesList, mLocale);
    if (!systemLocale.Equals(mLocale)) {
        AddLocaleDisplayNameToList(IContext::Probe(activity), localesList, systemLocale);
    }
    AutoPtr<IIterator> iterator;
    locales->GetIterator((IIterator**)&iterator);
    Boolean res;
    while (iterator->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        String l;
        ICharSequence::Probe(obj)->ToString(&l);
        // TODO: sort in unicode order
        AddLocaleDisplayNameToList(IContext::Probe(activity), localesList, l);
    }

    if (!String("").Equals(mLocale)) {
        // If mLocale is "", then we already inserted the "all languages" item, so don't do it
        AddLocaleDisplayNameToList(IContext::Probe(activity), localesList, String("")); // meaning: all languages
    }
    AutoPtr<LocaleRenderer> lr = new LocaleRenderer(IContext::Probe(activity), String(NULL));
    localesList->Add((IObject*)lr); // meaning: select another locale
    return localesList;
}

String UserDictionaryAddWordContents::GetCurrentUserDictionaryLocale()
{
    return mLocale;
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos