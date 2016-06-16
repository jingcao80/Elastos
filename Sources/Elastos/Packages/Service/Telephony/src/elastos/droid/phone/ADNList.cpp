
#include "elastos/droid/phone/ADNList.h"
#include "elastos/droid/app/Application.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::Application;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;

namespace Elastos {
namespace Droid {
namespace Phone {

ADNList::QueryHandler::QueryHandler(
    /* [in] */ ADNList* host,
    /* [in] */ IContentResolver* cr)
    : mHost(host)
{
    AsyncQueryHandler::constructor(cr);
}

ECode ADNList::QueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    if (DBG) {
        Int32 count;
        c->GetCount(&count);
        StringBuilder sb;
        sb += "onQueryComplete: cursor.count=";
        sb += count;
        mHost->Log(sb.ToString());
    }
    mHost->mCursor = cursor;
    SetAdapter();
    DisplayProgress(FALSE);

    // Cursor is refreshed and inherited classes may have menu items depending on it.
    return InvalidateOptionsMenu();
}

ECode ADNList::QueryHandler::OnInsertComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri)
{
    if (DBG) mHost->Log(String("onInsertComplete: requery"));
    return ReQuery();
}

ECode ADNList::QueryHandler::OnUpdateComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int32 result)
{
    if (DBG) mHost->Log(String("onUpdateComplete: requery"));
    return ReQuery();
}

ECode ADNList::QueryHandler::OnDeleteComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int32 result)
{
    if (DBG) mHost->Log(String("onDeleteComplete: requery"));
    return ReQuery();
}

const String ADNList::TAG("ADNList");
const Boolean ADNList::DBG = FALSE;

const Int32 ADNList::NAME_COLUMN = 0;
const Int32 ADNList::NUMBER_COLUMN = 1;
const Int32 ADNList::EMAILS_COLUMN = 2;

const Int32 ADNList::QUERY_TOKEN = 0;
const Int32 ADNList::INSERT_TOKEN = 1;
const Int32 ADNList::UPDATE_TOKEN = 2;
const Int32 ADNList::DELETE_TOKEN = 3;

static AutoPtr<ArrayOf<String> > InitCOLUMN_NAMES()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    (*array)[0] = String("name");
    (*array)[1] = String("number");
    (*array)[2] = String("emails");
}
const AutoPtr<ArrayOf<String> > COLUMN_NAMES = InitCOLUMN_NAMES();

static AutoPtr<ArrayOf<Int32> > InitVIEW_NAMES()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
    (*array)[0] = android.R.id.text1;
    (*array)[1] = android.R.id.text2;
}
const AutoPtr<ArrayOf<Int32> > VIEW_NAMES = InitVIEW_NAMES();

CAR_INTERFACE_IMPL(ADNList, ListActivity, IADNList)

ECode ADNList::constructor()
{
    return ListActivity::constructor()
}

ECode ADNList::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    ListActivity::OnCreate(savedInstanceState);
    AutoPtr<IWindow> window = GetWindow();
    Boolean res;
    window->RequestFeature(IWindow::FEATURE_INDETERMINATE_PROGRESS, &res);
    SetContentView(R::layout::adn_list);
    AutoPtr<IView> view = FindViewById(android::R::id::empty);
    mEmptyText = ITextView::Probe(view);
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    mQueryHandler = new QueryHandler(this, resolver);
    return NOERROR;
}

ECode ADNList::OnResume()
{
    ListActivity::OnResume();
    Query();
    return NOERROR;
}

ECode ADNList::OnStop()
{
    ListActivity::OnStop();
    if (mCursor != NULL) {
        return mCursor->Deactivate();
    }
    return NOERROR;
}

ECode ADNList::ResolveIntent(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    if (data == NULL) {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> _data;
        helper->Parse(String("content://icc/adn"), (IUri**)&_data);
        intent->SetData(_data);
    }

    return intent->GetData(uri);
}

void ADNList::Query()
{
    AutoPtr<IUri> uri;
    ResolveIntent((IUri**)&uri);
    if (DBG) Log(String("query: starting an async query"));
    mQueryHandler->StartQuery(QUERY_TOKEN, NULL, uri, COLUMN_NAMES,
            String(NULL), NULL, String(NULL));
    DisplayProgress(TRUE);
}

void ADNList::ReQuery()
{
    Query();
}

void ADNList::SetAdapter()
{
    // NOTE:
    // As it it written, the positioning code below is NOT working.
    // However, this current non-working state is in compliance with
    // the UI paradigm, so we can't really do much to change it.

    // In the future, if we wish to get this "positioning" correct,
    // we'll need to do the following:
    //   1. Change the layout to in the cursor adapter to:
    //     android.R.layout.simple_list_item_checked
    //   2. replace the selection / focus code with:
    //     getListView().setChoiceMode(ListView.CHOICE_MODE_SINGLE);
    //     getListView().setItemChecked(mInitialSelection, true);

    // Since the positioning is really only useful for the dialer's
    // SpecialCharSequence case (dialing '2#' to get to the 2nd
    // contact for instance), it doesn't make sense to mess with
    // the usability of the activity just for this case.

    // These artifacts include:
    //  1. UI artifacts (checkbox and highlight at the same time)
    //  2. Allowing the user to edit / create new SIM contacts when
    //    the user is simply trying to retrieve a number into the d
    //    dialer.

    if (mCursorAdapter == NULL) {
        NewAdapter((ICursorAdapter**)&mCursorAdapter);

        SetListAdapter(mCursorAdapter);
    }
    else {
        mCursorAdapter->ChangeCursor(mCursor);
    }

    Int32 count = 0;
    if (mInitialSelection >= 0 && mInitialSelection <
            (mCursorAdapter->GetCount(&count),count)) {
        SetSelection(mInitialSelection);
        AutoPtr<IListView> listView;
        GetListView((IListView**)&listView);
        listView->SetFocusableInTouchMode(TRUE);
        Boolean gotfocus;
        listView->RequestFocus(&gotfocus);
    }
}

ECode ADNList::NewAdapter(
    /* [out] */ ICursorAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);
    *adapter = NULL;

    AutoPtr<SimpleCursorAdapter> _adapter;
    CSimpleCursorAdapter::New(this,
            android.R.layout.simple_list_item_2,
            mCursor, COLUMN_NAMES, VIEW_NAMES, (ISimpleCursorAdapter**)&_adapter);
    *adapter = _adapter;
    REFCOUNT_ADD(*adapter);
    return NOERROR;
}

void ADNList::DisplayProgress(
    /* [in] */ Boolean loading)
{
    if (DBG) {
        StringBuilder sb;
        sb += "displayProgress: ";
        sb += loading;
        Log(sb.ToString());
    }

    mEmptyText->SetText(loading ? R.string.simContacts_emptyLoading:
        (IsAirplaneModeOn(this) ? R.string.simContacts_airplaneMode :
            R.string.simContacts_empty));
    AutoPtr<IWindow> window = GetWindow();
    window->SetFeatureInt(
            IWindow::FEATURE_INDETERMINATE_PROGRESS,
            loading ? PROGRESS_VISIBILITY_ON : PROGRESS_VISIBILITY_OFF);
}

Boolean ADNList::IsAirplaneModeOn(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    Int32 value;
    helper->GetInt32(resolver, ISettingsSystem::AIRPLANE_MODE_ON, 0, &value);
    return value != 0;
}

void ADNList::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "[ADNList] ";
    sb += msg;
    Logger::D(TAG, sb.ToString());
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos