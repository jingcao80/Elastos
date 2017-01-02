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

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/CUserDictionarySettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/inputmethod/UserDictionaryAddWordContents.h"
#include "elastos/droid/settings/inputmethod/UserDictionarySettingsUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Settings::Inputmethod::UserDictionaryAddWordContents;
using Elastos::Droid::Settings::Inputmethod::UserDictionarySettingsUtils;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Provider::CUserDictionaryWords;
using Elastos::Droid::Provider::IUserDictionaryWords;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::CAlphabetIndexer;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::EIID_ISectionIndexer;
using Elastos::Droid::Widget::EIID_ISimpleCursorAdapterViewBinder;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String CUserDictionarySettings::TAG("CUserDictionarySettings");

static AutoPtr<ArrayOf<String> > InitQUERY_PROJECTION()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(3);
    (*args)[0] = IUserDictionaryWords::_ID;
    (*args)[1] = IUserDictionaryWords::WORD;
    (*args)[2] = IUserDictionaryWords::SHORTCUT;

    return args;
}
const AutoPtr< ArrayOf<String> > CUserDictionarySettings::QUERY_PROJECTION = InitQUERY_PROJECTION();

const Int32 CUserDictionarySettings::INDEX_SHORTCUT = 2;

const String CUserDictionarySettings::QUERY_SELECTION =
            IUserDictionaryWords::LOCALE + String("=?");

const String CUserDictionarySettings::QUERY_SELECTION_ALL_LOCALES =
            IUserDictionaryWords::LOCALE + String(" is NULL");

static String InitDELETE_SELECTION_WITH_SHORTCUT()
{
    StringBuilder builder;
    builder += IUserDictionaryWords::WORD;
    builder += "=? AND ";
    builder += IUserDictionaryWords::SHORTCUT;
    builder += "=?";
    return builder.ToString();
}

const String CUserDictionarySettings::DELETE_SELECTION_WITH_SHORTCUT = InitDELETE_SELECTION_WITH_SHORTCUT();

static String InitDELETE_SELECTION_WITHOUT_SHORTCUT()
{
    StringBuilder builder;
    builder += IUserDictionaryWords::WORD;
    builder += "=? AND ";
    builder += IUserDictionaryWords::SHORTCUT;
    builder += " is NULL OR ";
    builder += IUserDictionaryWords::SHORTCUT;
    builder += "=''";
    return builder.ToString();
}

const String CUserDictionarySettings::DELETE_SELECTION_WITHOUT_SHORTCUT = InitDELETE_SELECTION_WITHOUT_SHORTCUT();

const Int32 CUserDictionarySettings::OPTIONS_MENU_ADD = IMenu::FIRST;

//===============================================================================
//                  CUserDictionarySettings::MyAdapter
//===============================================================================

CAR_INTERFACE_IMPL(CUserDictionarySettings::MyAdapter, SimpleCursorAdapter, ISectionIndexer)

ECode CUserDictionarySettings::MyAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layout,
    /* [in] */ ICursor* c,
    /* [in] */ ArrayOf<String>* from,
    /* [in] */ ArrayOf<Int32>* to,
    /* [in] */ CUserDictionarySettings* host)
{
    mHost = host;

    SimpleCursorAdapter::constructor(context, layout, c, from, to);

    mViewBinder = new ViewBinder();

    if (NULL != c) {
        String alphabet;
        context->GetString(
                Elastos::Droid::R::string::fast_scroll_alphabet, &alphabet);
        Int32 wordColIndex;
        c->GetColumnIndexOrThrow(IUserDictionaryWords::WORD, &wordColIndex);
        CAlphabetIndexer::New(c, wordColIndex, CoreUtils::Convert(alphabet), (IAlphabetIndexer**)&mIndexer);
    }
    return SetViewBinder(mViewBinder);
}

CUserDictionarySettings::MyAdapter::~MyAdapter()
{}

ECode CUserDictionarySettings::MyAdapter::GetPositionForSection(
    /* [in] */ Int32 section,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (NULL == mIndexer) return NOERROR;
    return ISectionIndexer::Probe(mIndexer)->GetPositionForSection(section, result);
}

ECode CUserDictionarySettings::MyAdapter::GetSectionForPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (NULL == mIndexer) return NOERROR;
    return ISectionIndexer::Probe(mIndexer)->GetSectionForPosition(position, result);
}

ECode CUserDictionarySettings::MyAdapter::GetSections(
    /* [out, callee] */ ArrayOf<IInterface*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (NULL == mIndexer) return NOERROR;
    return ISectionIndexer::Probe(mIndexer)->GetSections(result);
}

//===============================================================================
//                  CUserDictionarySettings::ViewBinder
//===============================================================================

CAR_INTERFACE_IMPL(CUserDictionarySettings::ViewBinder, Object, ISimpleCursorAdapterViewBinder)

CUserDictionarySettings::ViewBinder::ViewBinder()
{}

CUserDictionarySettings::ViewBinder::~ViewBinder()
{}

ECode CUserDictionarySettings::ViewBinder::SetViewValue(
    /* [in] */ IView* v,
    /* [in] */ ICursor* c,
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;

    if (columnIndex == INDEX_SHORTCUT) {
        String shortcut;
        c->GetString(INDEX_SHORTCUT, &shortcut);
        if (TextUtils::IsEmpty(shortcut)) {
            v->SetVisibility(IView::GONE);
        }
        else {
            ITextView::Probe(v)->SetText(CoreUtils::Convert(shortcut));
            v->SetVisibility(IView::VISIBLE);
        }
        v->Invalidate();
        *res = TRUE;
        return NOERROR;
    }

    return NOERROR;
}

//===============================================================================
//                  CUserDictionarySettings
//===============================================================================

CAR_OBJECT_IMPL(CUserDictionarySettings)

CUserDictionarySettings::CUserDictionarySettings()
{}

CUserDictionarySettings::~CUserDictionarySettings()
{}

ECode CUserDictionarySettings::constructor()
{
    return ListFragment::constructor();
}

ECode CUserDictionarySettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Slogger::I(TAG, " >> enter CUserDictionarySettings::OnCreate");

    ListFragment::OnCreate(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IActionBar> actionBar;
    activity->GetActionBar((IActionBar**)&actionBar);
    actionBar->SetTitle(R::string::user_dict_settings_title);

    Slogger::I(TAG, " << leave CUserDictionarySettings::OnCreate");
    return NOERROR;
}

ECode CUserDictionarySettings::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    return inflater->Inflate(
            Elastos::Droid::R::layout::preference_list_fragment, container, FALSE, result);
}

ECode CUserDictionarySettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    ListFragment::OnActivityCreated(savedInstanceState);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    String localeFromIntent;
    if (NULL != intent) {
        intent->GetStringExtra(String("locale"), &localeFromIntent);
    }

    AutoPtr<IBundle> arguments;
    GetArguments((IBundle**)&arguments);
    String localeFromArguments;
    if (NULL != arguments) {
        arguments->GetString(String("locale"), &localeFromArguments);
    }

    String locale;
    if (!localeFromArguments.IsNull()) {
        locale = localeFromArguments;
    }
    else if (!localeFromIntent.IsNull()) {
        locale = localeFromIntent;
    }
    else {
        locale = String(NULL);
    }

    mLocale = locale;
    mCursor = CreateCursor(locale);
    AutoPtr<IView> tmp;
    GetView((IView**)&tmp);
    AutoPtr<IView> emptyViewTmp;
    tmp->FindViewById(Elastos::Droid::R::id::empty, (IView**)&emptyViewTmp);
    ITextView* emptyView = ITextView::Probe(emptyViewTmp);
    emptyView->SetText(R::string::user_dict_settings_empty_text);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    IAdapterView* _listView = IAdapterView::Probe(listView);
    _listView->SetAdapter(IAdapter::Probe(CreateAdapter()));
    IAbsListView::Probe(listView)->SetFastScrollEnabled(TRUE);
    _listView->SetEmptyView(emptyViewTmp);

    SetHasOptionsMenu(TRUE);
    // Show the language as a subtitle of the action bar
    AutoPtr<IActionBar> actionBar;
    activity->GetActionBar((IActionBar**)&actionBar);
    actionBar->SetSubtitle(CoreUtils::Convert(
            UserDictionarySettingsUtils::GetLocaleDisplayName(IContext::Probe(activity), mLocale)));
    return NOERROR;
}

AutoPtr<ICursor> CUserDictionarySettings::CreateCursor(
    /* [in] */ const String& locale)
{
    // Locale can be any of:
    // - The string representation of a locale, as returned by Locale#ToString()
    // - The empty string. This means we want a cursor returning words valid for all locales.
    // - NULL. This means we want a cursor for the current locale, whatever this is.
    // Note that this contrasts with the data inside the database, where NULL means "all
    // locales" and there should never be an empty string. The confusion is called by the
    // historical use of NULL for "all locales".
    // TODO: it should be easy to make this more readable by making the special values
    // human-readable, like "all_locales" and "current_locales" strings, provided they
    // can be guaranteed not to match locales that may exist.
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    AutoPtr<IUserDictionaryWords> udw;
    CUserDictionaryWords::AcquireSingleton((IUserDictionaryWords**)&udw);
    AutoPtr<IUri> wordsUri;
    udw->GetCONTENT_URI((IUri**)&wordsUri);

    StringBuilder builder;
    builder += "UPPER(";
    builder += IUserDictionaryWords::WORD;
    builder += ")";

    AutoPtr<ICursor> cursor;
    if (String("").Equals(locale)) {
        // Case-insensitive sort
        activity->ManagedQuery(wordsUri, QUERY_PROJECTION,
                QUERY_SELECTION_ALL_LOCALES, NULL, builder.ToString(), (ICursor**)&cursor);
        return cursor;
    }
    else {
        String queryLocale;
        if (!locale.IsNull()) {
            queryLocale = locale;
        }
        else {
            AutoPtr<ILocaleHelper> helper;
            CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
            AutoPtr<ILocale> defaultLocale;
            helper->GetDefault((ILocale**)&defaultLocale);
            defaultLocale->ToString(&queryLocale);
        }

        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = queryLocale;

        activity->ManagedQuery(wordsUri, QUERY_PROJECTION,
                QUERY_SELECTION, args, builder.ToString(), (ICursor**)&cursor);
        return cursor;
    }
}

AutoPtr<IListAdapter> CUserDictionarySettings::CreateAdapter()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr< ArrayOf<String> > args1 = ArrayOf<String>::Alloc(2);
    (*args1)[0] = IUserDictionaryWords::WORD;
    (*args1)[1] = IUserDictionaryWords::SHORTCUT;

    AutoPtr< ArrayOf<Int32> > args2 = ArrayOf<Int32>::Alloc(2);
    (*args2)[0] = Elastos::Droid::R::id::text1;
    (*args2)[1] = Elastos::Droid::R::id::text2;

    AutoPtr<IListAdapter> adapter = new MyAdapter();
    ((MyAdapter*)adapter.Get())->constructor(IContext::Probe(activity),
            R::layout::user_dictionary_item, mCursor,
            args1, args2, this);
    return adapter;
}

ECode CUserDictionarySettings::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    const String word = GetWord(position);
    const String shortcut = GetShortcut(position);
    if (word != NULL) {
        ShowAddOrEditDialog(word, shortcut);
    }
    return NOERROR;
}

ECode CUserDictionarySettings::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    AutoPtr<IMenuItem> actionItem;
    menu->Add(0, OPTIONS_MENU_ADD, 0, R::string::user_dict_settings_add_menu_title, (IMenuItem**)&actionItem);
    actionItem->SetIcon(R::drawable::ic_menu_add_dark);
    actionItem->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM |
            IMenuItem::SHOW_AS_ACTION_WITH_TEXT);
    return NOERROR;
}

ECode CUserDictionarySettings::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 id;
    item->GetItemId(&id);
    if (id == OPTIONS_MENU_ADD) {
        ShowAddOrEditDialog(String(NULL), String(NULL));
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

void CUserDictionarySettings::ShowAddOrEditDialog(
    /* [in] */ const String& editingWord,
    /* [in] */ const String& editingShortcut)
{
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutInt32(UserDictionaryAddWordContents::EXTRA_MODE, editingWord.IsNull()
            ? UserDictionaryAddWordContents::MODE_INSERT
            : UserDictionaryAddWordContents::MODE_EDIT);
    args->PutString(UserDictionaryAddWordContents::EXTRA_WORD, editingWord);
    args->PutString(UserDictionaryAddWordContents::EXTRA_SHORTCUT, editingShortcut);
    args->PutString(UserDictionaryAddWordContents::EXTRA_LOCALE, mLocale);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CSettingsActivity* sa = (CSettingsActivity*)ISettingsActivity::Probe(activity);
    sa->StartPreferencePanel(String("Elastos.Droid.Settings.Inputmethod.CUserDictionaryAddWordFragment"),
            args, R::string::user_dict_settings_add_dialog_title, NULL, NULL, 0);
}

String CUserDictionarySettings::GetWord(
    /* [in] */ Int32 position)
{
    if (NULL == mCursor) return String(NULL);
    Boolean res;
    mCursor->MoveToPosition(position, &res);
    // Handle a possible race-condition
    mCursor->IsAfterLast(&res);
    if (res) return String(NULL);

    Int32 index;
    mCursor->GetColumnIndexOrThrow(IUserDictionaryWords::WORD, &index);

    String result;
    mCursor->GetString(index, &result);
    return result;
}

String CUserDictionarySettings::GetShortcut(
    /* [in] */ Int32 position)
{
    if (NULL == mCursor) return String(NULL);
    Boolean res;
    mCursor->MoveToPosition(position, &res);
    // Handle a possible race-condition
    mCursor->IsAfterLast(&res);
    if (res) return String(NULL);

    Int32 index;
    mCursor->GetColumnIndexOrThrow(IUserDictionaryWords::SHORTCUT, &index);
    String result;
    mCursor->GetString(index, &result);
    return result;
}

void CUserDictionarySettings::DeleteWord(
    /* [in] */ const String& word,
    /* [in] */ const String& shortcut,
    /* [in] */ IContentResolver* resolver)
{
    AutoPtr<IUserDictionaryWords> udw;
    CUserDictionaryWords::AcquireSingleton((IUserDictionaryWords**)&udw);
    AutoPtr<IUri> wordsUri;
    udw->GetCONTENT_URI((IUri**)&wordsUri);

    Int32 data;
    if (TextUtils::IsEmpty(shortcut)) {
        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = word;

        resolver->Delete(
                wordsUri, DELETE_SELECTION_WITHOUT_SHORTCUT, args, &data);
    }
    else {
        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
        (*args)[0] = word;
        (*args)[1] = shortcut;

        resolver->Delete(
                wordsUri, DELETE_SELECTION_WITH_SHORTCUT, args, &data);
    }
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos