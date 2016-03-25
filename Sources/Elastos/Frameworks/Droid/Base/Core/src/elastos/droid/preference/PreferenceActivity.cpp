
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentHelper.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/preference/CPreferenceActivityHeader.h"
#include "elastos/droid/preference/CPreferenceManager.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::EIID_IArrayAdapter;
using Elastos::Droid::Widget::EIID_IBaseAdapter;
using Elastos::Droid::Widget::EIID_IListAdapter;
using Elastos::Droid::Widget::EIID_ISpinnerAdapter;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::R;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Preference {

const String PreferenceActivity::TAG("PreferenceActivity");
const String PreferenceActivity::HEADERS_TAG(":android:headers");
const String PreferenceActivity::CUR_HEADER_TAG(":android:cur_header");
const String PreferenceActivity::PREFERENCES_TAG(":android:preferences");
const String PreferenceActivity::BACK_STACK_PREFS(":android:prefs");
const String PreferenceActivity::EXTRA_PREFS_SHOW_BUTTON_BAR("extra_prefs_show_button_bar");
const String PreferenceActivity::EXTRA_PREFS_SHOW_SKIP("extra_prefs_show_skip");
const String PreferenceActivity::EXTRA_PREFS_SET_NEXT_TEXT("extra_prefs_set_next_text");
const String PreferenceActivity::EXTRA_PREFS_SET_BACK_TEXT("extra_prefs_set_back_text");
const Int32 PreferenceActivity::FIRST_REQUEST_CODE;
const Int32 PreferenceActivity::MSG_BIND_PREFERENCES;
const Int32 PreferenceActivity::MSG_BUILD_HEADERS;

//====================================================
// PreferenceActivity::MHandler
//====================================================

PreferenceActivity::MHandler::MHandler(
    /* [in] */ PreferenceActivity* host)
    : mHost(host)
{}

ECode PreferenceActivity::MHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case MSG_BIND_PREFERENCES: {
            mHost->BindPreferences();
            break;
        }
        case MSG_BUILD_HEADERS: {
            AutoPtr<IArrayList> oldHeaders;
            AutoPtr<ICollection> coll = ICollection::Probe(mHost->mHeaders);
            CArrayList::New((ICollection*)coll, (IArrayList**)&oldHeaders);
            mHost->mHeaders->Clear();
            mHost->OnBuildHeaders(mHost->mHeaders);
            AutoPtr<IBaseAdapter> baseAdapter = IBaseAdapter::Probe(mHost->mAdapter);
            if (baseAdapter != NULL) {
                baseAdapter->NotifyDataSetChanged();
            }
            AutoPtr<IPreferenceActivityHeader> header;
            mHost->OnGetNewHeader((IPreferenceActivityHeader**)&header);
            String fragment;
            if (header != NULL && (header->GetFragment(&fragment), !fragment.IsNull())) {
                AutoPtr<IPreferenceActivityHeader> mappedHeader;
                mHost->FindBestMatchingHeader((IPreferenceActivityHeader*)header, IList::Probe(oldHeaders), (IPreferenceActivityHeader**)&mappedHeader);
                if (mappedHeader == NULL || mHost->mCurHeader != mappedHeader) {
                    mHost->SwitchToHeader(header);
                }
            }
            else if (mHost->mCurHeader != NULL) {
                AutoPtr<IPreferenceActivityHeader> mappedHeader;
                mHost->FindBestMatchingHeader((IPreferenceActivityHeader*)(mHost->mCurHeader), (IList*)(mHost->mHeaders), (IPreferenceActivityHeader**)&mappedHeader);
                if (mappedHeader != NULL) {
                    mHost->SetSelectedHeader(mappedHeader);
                }
            }
            break;
        }
    }
    return NOERROR;
}

//====================================================
// PreferenceActivity::HeaderAdapter
//====================================================

CAR_INTERFACE_IMPL(PreferenceActivity::HeaderAdapter, ArrayAdapter, IPreferenceActivityHeaderAdapter)

PreferenceActivity::HeaderAdapter::HeaderAdapter(
    /* [in] */ IContext* context,
    /* [in] */ IList* objects,
    /* [in] */ Int32 layoutResId,
    /* [in] */ Boolean removeIconBehavior)
    : mLayoutResId(layoutResId)
    , mRemoveIconIfEmpty(removeIconBehavior)
{
    ArrayAdapter::constructor(context, 0, objects);
    AutoPtr<IInterface> object;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&object);
    mInflater = ILayoutInflater::Probe(object);
}

PreferenceActivity::HeaderAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<HeaderViewHolder> holder;
    AutoPtr<IView> v;

    if (convertView == NULL) {
        mInflater->Inflate(mLayoutResId, FALSE, (IView**)&v);
        holder = new HeaderViewHolder();
        AutoPtr<IView> tempView;
        v->FindViewById(R::id::icon, (IView**)&tempView);
        holder->mIcon = IImageView::Probe(tempView);
        tempView = NULL;
        v->FindViewById(R::id::title, (IView**)&tempView);
        holder->mTitle = ITextView::Probe(tempView);
        tempView = NULL;
        v->FindViewById(R::id::summary, (IView**)&tempView);
        holder->mSummary = ITextView::Probe(tempView);
        tempView = NULL;
        v->SetTag(holder->Probe(EIID_IInterface));
    }
    else {
        v = convertView;
        AutoPtr<IInterface> inface;
        v->GetTag((IInterface**)&inface);
        AutoPtr<IObject> h = IObject::Probe(inface.Get());
        holder = (HeaderViewHolder*)(h.Get());
    }

    // All view fields must be updated every time, because the view may be recycled
    AutoPtr<IInterface> obj;
    this->GetItem(position, (IInterface**)&obj);
    AutoPtr<IPreferenceActivityHeader> header = IPreferenceActivityHeader::Probe(obj);
    Int32 iconRes;
    header->GetIconRes(&iconRes);
    if (mRemoveIconIfEmpty) {
        Int32 iconRes = 0;
        header->GetIconRes(&iconRes);
        if (iconRes == 0) {
            IView::Probe(holder->mIcon)->SetVisibility(IView::GONE);
        } else {
             IView::Probe(holder->mIcon)->SetVisibility(IView::VISIBLE);
             holder->mIcon->SetImageResource(iconRes);
        }
    } else {
        holder->mIcon->SetImageResource(iconRes);
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resource;
    context->GetResources((IResources**)&resource);
    AutoPtr<ICharSequence> title;
    header->GetTitle(resource, (ICharSequence**)&title);
    holder->mTitle->SetText(title);
    AutoPtr<ICharSequence> summary;
    header->GetSummary(resource, (ICharSequence**)&summary);
    if (!TextUtils::IsEmpty(summary)) {
        IView::Probe(holder->mSummary)->SetVisibility(IView::VISIBLE);
        holder->mSummary->SetText(summary);
    }
    else {
        IView::Probe(holder->mSummary)->SetVisibility(IView::GONE);
    }

    *view = v;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

//====================================================
// PreferenceActivity::Header
//====================================================

CAR_INTERFACE_IMPL_2(PreferenceActivity::Header, Object, IPreferenceActivityHeader, IParcelable)

PreferenceActivity::Header::Header()
    : mId(IPreferenceActivity::HEADER_ID_UNDEFINED)
    , mTitleRes(0)
    , mSummaryRes(0)
    , mBreadCrumbTitleRes(0)
    , mBreadCrumbShortTitleRes(0)
    , mIconRes(0)
{
    // Empty
}

ECode PreferenceActivity::Header::constructor()
{
    return NOERROR;
}

ECode PreferenceActivity::Header::constructor(
    /* [in] */ IParcelable* source)
{
    return NOERROR;
}

ECode PreferenceActivity::Header::GetTitle(
    /* [in] */ IResources* res,
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    if (mTitleRes != 0) {
        return res->GetText(mTitleRes, title);
    }
    *title = mTitle;
    REFCOUNT_ADD(*title)
    return NOERROR;
}

ECode PreferenceActivity::Header::GetSummary(
    /* [in] */ IResources* res,
    /* [out] */ ICharSequence** summary)
{
    VALIDATE_NOT_NULL(summary)
    if (mSummaryRes != 0) {
        return res->GetText(mSummaryRes, summary);
    }
    *summary = mSummary;
    REFCOUNT_ADD(*summary)
    return NOERROR;
}

ECode PreferenceActivity::Header::GetBreadCrumbTitle(
    /* [in] */ IResources* res,
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    if (mBreadCrumbTitleRes != 0) {
        return res->GetText(mBreadCrumbTitleRes, title);
    }
    *title = mBreadCrumbTitle;
    REFCOUNT_ADD(*title)
    return NOERROR;
}

ECode PreferenceActivity::Header::GetBreadCrumbShortTitle(
    /* [in] */ IResources* res,
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    if (mBreadCrumbShortTitleRes != 0) {
        return res->GetText(mBreadCrumbShortTitleRes, title);
    }
    *title = mBreadCrumbShortTitle;
    REFCOUNT_ADD(*title);
    return NOERROR;
}

ECode PreferenceActivity::Header::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest)
    dest->WriteInt64(mId);
    dest->WriteInt32(mTitleRes);
    dest->WriteInterfacePtr(mTitle);
    dest->WriteInt32(mSummaryRes);
    dest->WriteInterfacePtr(mSummary);
    dest->WriteInt32(mBreadCrumbTitleRes);
    dest->WriteInterfacePtr(mBreadCrumbTitle);
    dest->WriteInt32(mBreadCrumbShortTitleRes);
    dest->WriteInterfacePtr(mBreadCrumbShortTitle);
    dest->WriteInt32(mIconRes);
    dest->WriteString(mFragment);
    dest->WriteInterfacePtr(mFragmentArguments);
    dest->WriteInterfacePtr(mIntent);
    dest->WriteInterfacePtr(mExtras);
    return NOERROR;
}

ECode PreferenceActivity::Header::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mId);
    source->ReadInt32(&mTitleRes);
    AutoPtr<IInterface> title;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&title);
    mTitle = ICharSequence::Probe(title);
    source->ReadInt32(&mSummaryRes);
    AutoPtr<IInterface> summary;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&summary);
    mSummary = ICharSequence::Probe(summary);
    source->ReadInt32(&mBreadCrumbTitleRes);
    AutoPtr<IInterface> breadCrumbTitle;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&breadCrumbTitle);
    mBreadCrumbTitle = ICharSequence::Probe(breadCrumbTitle);
    source->ReadInt32(&mBreadCrumbShortTitleRes);
    AutoPtr<IInterface> breadCrumbShortTitle;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&breadCrumbShortTitle);
    mBreadCrumbShortTitle = ICharSequence::Probe(breadCrumbShortTitle);
    source->ReadInt32(&mIconRes);
    source->ReadString(&mFragment);
    AutoPtr<IInterface> fragmentArguments;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&fragmentArguments);
    mFragmentArguments = IBundle::Probe(fragmentArguments);
    AutoPtr<IInterface> intent;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&intent);
    mIntent = IIntent::Probe(intent);
    AutoPtr<IInterface> extras;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&extras);
    mExtras = IBundle::Probe(extras);
    return NOERROR;
}

ECode PreferenceActivity::Header::GetId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode PreferenceActivity::Header::SetId(
    /* [in] */ Int64 id)
{
    mId = id;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetTitleRes(
    /* [out] */ Int32* titleRes)
{
    VALIDATE_NOT_NULL(titleRes)
    *titleRes = mTitleRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::SetTitleRes(
    /* [in] */ Int32 titleRes)
{
    mTitleRes = titleRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    *title = mTitle;
    REFCOUNT_ADD(*title)
    return NOERROR;
}

ECode PreferenceActivity::Header::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetSummaryRes(
    /* [out] */ Int32* summaryRes)
{
    VALIDATE_NOT_NULL(summaryRes)
    *summaryRes = mSummaryRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::SetSummaryRes(
    /* [in] */ Int32 summaryRes)
{
    mSummaryRes = summaryRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetSummary(
    /* [out] */ ICharSequence** summary)
{
    VALIDATE_NOT_NULL(summary)
    *summary = mSummary;
    REFCOUNT_ADD(*summary)
    return NOERROR;
}

ECode PreferenceActivity::Header::SetSummary(
    /* [in] */ ICharSequence* summary)
{
    mSummary = summary;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetBreadCrumbTitleRes(
    /* [out] */ Int32* breadCrumbTitleRes)
{
    VALIDATE_NOT_NULL(breadCrumbTitleRes)
    *breadCrumbTitleRes = mBreadCrumbTitleRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::SetBreadCrumbTitleRes(
    /* [in] */ Int32 breadCrumbTitleRes)
{
    mBreadCrumbTitleRes = breadCrumbTitleRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetBreadCrumbTitle(
    /* [out] */ ICharSequence** breadCrumbTitle)
{
    VALIDATE_NOT_NULL(breadCrumbTitle)
    *breadCrumbTitle = mBreadCrumbTitle;
    REFCOUNT_ADD(*breadCrumbTitle)
    return NOERROR;
}

ECode PreferenceActivity::Header::SetBreadCrumbTitle(
    /* [in] */ ICharSequence* breadCrumbTitle)
{
    mBreadCrumbTitle = breadCrumbTitle;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetBreadCrumbShortTitleRes(
    /* [out] */ Int32* breadCrumbShortTitleRes)
{
    VALIDATE_NOT_NULL(breadCrumbShortTitleRes);
    *breadCrumbShortTitleRes = mBreadCrumbShortTitleRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::SetBreadCrumbShortTitleRes(
    /* [in] */ Int32 breadCrumbShortTitleRes)
{
    mBreadCrumbShortTitleRes = breadCrumbShortTitleRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetBreadCrumbShortTitle(
    /* [out] */ ICharSequence** breadCrumbShortTitle)
{
    VALIDATE_NOT_NULL(breadCrumbShortTitle)
    *breadCrumbShortTitle = mBreadCrumbShortTitle;
    REFCOUNT_ADD(*breadCrumbShortTitle)
    return NOERROR;
}

ECode PreferenceActivity::Header::SetBreadCrumbShortTitle(
    /* [in] */ ICharSequence* breadCrumbShortTitle)
{
    mBreadCrumbShortTitle = breadCrumbShortTitle;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetIconRes(
    /* [out] */ Int32* iconRes)
{
    VALIDATE_NOT_NULL(iconRes)
    *iconRes = mIconRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::SetIconRes(
    /* [in] */ Int32 iconRes)
{
    mIconRes = iconRes;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment)
    *fragment = mFragment;
    return NOERROR;
}

ECode PreferenceActivity::Header::SetFragment(
    /* [in] */ const String& fragment)
{
    mFragment = fragment;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetFragmentArguments(
    /* [out] */ IBundle** fragmentArguments)
{
    VALIDATE_NOT_NULL(fragmentArguments)
    *fragmentArguments = mFragmentArguments;
    REFCOUNT_ADD(*fragmentArguments)
    return NOERROR;
}

ECode PreferenceActivity::Header::SetFragmentArguments(
    /* [in] */ IBundle* fragmentArguments)
{
    mFragmentArguments = fragmentArguments;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode PreferenceActivity::Header::SetIntent(
    /* [in] */ IIntent* intent)
{
    mIntent = intent;
    return NOERROR;
}

ECode PreferenceActivity::Header::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

ECode PreferenceActivity::Header::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

//====================================================
// PreferenceActivity::BackButtonListener
//====================================================

CAR_INTERFACE_IMPL(PreferenceActivity::BackButtonListener, Object, IViewOnClickListener);

PreferenceActivity::BackButtonListener::BackButtonListener(
    /* [in] */ PreferenceActivity* parent)
    : mParent(parent)
{}

ECode PreferenceActivity::BackButtonListener::OnClick(
    /* [in] */ IView* v)
{
    mParent->SetResult(IActivity::RESULT_CANCELED);
    return mParent->Finish();
}


//====================================================
// PreferenceActivity::SkipButtonListener
//====================================================

CAR_INTERFACE_IMPL(PreferenceActivity::SkipButtonListener, Object, IViewOnClickListener);

PreferenceActivity::SkipButtonListener::SkipButtonListener(
    /* [in] */ PreferenceActivity* parent)
    : mParent(parent)
{}

ECode PreferenceActivity::SkipButtonListener::OnClick(
    /* [in] */ IView* v)
{
    mParent->SetResult(IActivity::RESULT_OK);
    return mParent->Finish();
}


//====================================================
// PreferenceActivity::NextButtonListener
//====================================================

CAR_INTERFACE_IMPL(PreferenceActivity::NextButtonListener, Object, IViewOnClickListener)

PreferenceActivity::NextButtonListener::NextButtonListener(
    /* [in] */ PreferenceActivity* parent)
    : mParent(parent)
{}

ECode PreferenceActivity::NextButtonListener::OnClick(
    /* [in] */ IView* v)
{
    mParent->SetResult(IActivity::RESULT_OK);
    return mParent->Finish();
}


//====================================================
// PreferenceActivity
//====================================================

CAR_INTERFACE_IMPL_3(PreferenceActivity, ListActivity, IPreferenceActivity, IPreferenceManagerOnPreferenceTreeClickListener, IPreferenceFragmentOnPreferenceStartFragmentCallback)

PreferenceActivity::PreferenceActivity()
    : mSinglePane(FALSE)
    , mPreferenceHeaderItemResId(0)
    , mPreferenceHeaderRemoveEmptyIcon(FALSE)
{
    mHandler = new MHandler(this);
}

ECode PreferenceActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    ListActivity::OnCreate(savedInstanceState);

    // Theming for the PreferenceActivity layout and for the Preference Header(s) layout
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::PreferenceActivity),
            ArraySize(R::styleable::PreferenceActivity));
    AutoPtr<ITypedArray> sa;
    Activity::ObtainStyledAttributes(NULL, attrIds,
        R::attr::preferenceActivityStyle, 0, (ITypedArray**)&sa);

    Int32 layoutResId;
    sa->GetResourceId(
            R::styleable::PreferenceActivity_layout,
            R::layout::preference_list_content, &layoutResId);

    sa->GetResourceId(
            R::styleable::PreferenceActivity_headerLayout,
            R::layout::preference_header_item, &mPreferenceHeaderItemResId);
    sa->GetBoolean(
            R::styleable::PreferenceActivity_headerRemoveIconIfEmpty,
            FALSE, &mPreferenceHeaderRemoveEmptyIcon);

    sa->Recycle();

    Activity::SetContentView(layoutResId);

    AutoPtr<IView> view = Activity::FindViewById(R::id::list_footer);
    mListFooter = IFrameLayout::Probe(view);
    view = Activity::FindViewById(R::id::prefs_frame);
    mPrefsContainer = IViewGroup::Probe(view);
    Boolean hidingHeaders, isMultiPane;
    OnIsHidingHeaders(&hidingHeaders);
    OnIsMultiPane(&isMultiPane);
    mSinglePane = hidingHeaders || !isMultiPane;
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String initialFragment;
    intent->GetStringExtra(EXTRA_SHOW_FRAGMENT, &initialFragment);
    AutoPtr<IBundle> initialArguments;
    intent->GetBundleExtra(EXTRA_SHOW_FRAGMENT_ARGUMENTS, (IBundle**)&initialArguments);
    Int32 initialTitle;
    intent->GetInt32Extra(EXTRA_SHOW_FRAGMENT_TITLE, 0, &initialTitle);
    Int32 initialShortTitle;
    intent->GetInt32Extra(EXTRA_SHOW_FRAGMENT_SHORT_TITLE, 0, &initialShortTitle);

    if (savedInstanceState != NULL) {
        // We are restarting from a previous saved state; used that to
        // initialize, instead of starting fresh.
        AutoPtr<IArrayList> headers;
        savedInstanceState->GetParcelableArrayList(HEADERS_TAG, (IArrayList**)&headers);
        if (headers != NULL) {
            // We are restarting from a previous saved state; used that to
            // initialize, instead of starting fresh.
            AutoPtr<ICollection> collection = ICollection::Probe(headers);
            mHeaders->AddAll(collection.Get());
            Int32 curHeader;
            Int32 size;
            if ((savedInstanceState->GetInt32(CUR_HEADER_TAG, (Int32) HEADER_ID_UNDEFINED, &curHeader), curHeader >= 0)
                && (mHeaders->GetSize(&size), curHeader < size)) {
                AutoPtr<IInterface> obj;
                mHeaders->Get(curHeader, (IInterface**)&obj);
                AutoPtr<IPreferenceActivityHeader> element = IPreferenceActivityHeader::Probe(obj);
                SetSelectedHeader(element.Get());
            }

        }
    }
    else {
        if (!initialFragment.IsNull() && mSinglePane) {
            // If we are just showing a fragment, we want to run in
            // new fragment mode, but don't need to compute and show
            // the headers.
            SwitchToHeader(initialFragment, initialArguments);
            if (initialTitle != 0) {
                AutoPtr<ICharSequence> initialTitleStr;
                GetText(initialTitle, (ICharSequence**)&initialTitleStr);
                AutoPtr<ICharSequence> initialShortTitleStr;
                if(initialShortTitle != 0) {
                    GetText(initialShortTitle, (ICharSequence**)&initialShortTitleStr);
                }
                ShowBreadCrumbs(initialTitleStr, initialShortTitleStr);
            }

        }
        else {
            // We need to try to build the headers.
            OnBuildHeaders(mHeaders.Get());

            // If there are headers, then at this point we need to show
            // them and, depending on the screen, we may also show in-line
            // the currently selected preference fragment.
            Int32 size;
            if ((mHeaders->GetSize(&size), size > 0) && !mSinglePane) {
                 if (initialFragment.IsNull()) {
                    AutoPtr<IPreferenceActivityHeader> h;
                    OnGetInitialHeader((IPreferenceActivityHeader**)&h);
                    SwitchToHeader(h);
                }
                else {
                    SwitchToHeader(initialFragment, initialArguments);
                }
            }
        }
    }

    // The default configuration is to only show the list view.  Adjust
    // visibility for other configurations.
    Int32 size;
    if (!initialFragment.IsNull() && mSinglePane) {
        // Single pane, showing just a prefs fragment.
        Activity::FindViewById(R::id::headers)->SetVisibility(IView::GONE);
        IView::Probe(mPrefsContainer)->SetVisibility(IView::VISIBLE);
        if (initialTitle != 0) {
            AutoPtr<ICharSequence> initialTitleStr;
            GetText(initialTitle, (ICharSequence**)&initialTitleStr);
            AutoPtr<ICharSequence> initialShortTitleStr;
            if (initialShortTitle != 0) {
                GetText(initialShortTitle, (ICharSequence**)&initialShortTitleStr);
            }
            ShowBreadCrumbs(initialTitleStr, initialShortTitleStr);
        }
    }
    else if (mHeaders->GetSize(&size), size > 0) {
        AutoPtr<HeaderAdapter> ha = new HeaderAdapter(this, mHeaders, mPreferenceHeaderItemResId, mPreferenceHeaderRemoveEmptyIcon);
        ListActivity::SetListAdapter(IListAdapter::Probe(ha));
        if (!mSinglePane) {
            // Multi-pane.
            AutoPtr<IListView> listView;
            GetListView((IListView**)&listView);
            IAbsListView::Probe(listView)->SetChoiceMode(IAbsListView::CHOICE_MODE_SINGLE);
            if (mCurHeader != NULL) {
                SetSelectedHeader(mCurHeader);
            }
            IView::Probe(mPrefsContainer)->SetVisibility(IView::VISIBLE);
        }
    }
    else {
        // If there are no headers, we are in the old "just show a screen
        // of preferences" mode.
        SetContentView(R::layout::preference_list_content_single);
        view = Activity::FindViewById(R::id::list_footer);
        mListFooter = IFrameLayout::Probe(view);
        view = Activity::FindViewById(R::id::prefs);
        mPrefsContainer = IViewGroup::Probe(view);
        CPreferenceManager::New(this, FIRST_REQUEST_CODE, (IPreferenceManager**)&mPreferenceManager);
        AutoPtr<IPreferenceManagerOnPreferenceTreeClickListener> listener;
        listener = this;
        mPreferenceManager->SetOnPreferenceTreeClickListener(listener);
    }

    // see if we should show Back/Next buttons
    intent = NULL;
    GetIntent((IIntent**)&intent);
    Boolean isShow;
    if (intent->GetBooleanExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, FALSE, &isShow), isShow) {
        Activity::FindViewById(R::id::button_bar)->SetVisibility(IView::VISIBLE);
        view = Activity::FindViewById(R::id::back_button);
        AutoPtr<IButton> backButton = IButton::Probe(view);
        AutoPtr<IViewOnClickListener> backButtonListener = new BackButtonListener(this);
        IView::Probe(backButton)->SetOnClickListener(backButtonListener);

        view = Activity::FindViewById(R::id::skip_button);
        AutoPtr<IButton> skipButton = IButton::Probe(view);
        AutoPtr<IViewOnClickListener> skipButtonListener = new SkipButtonListener(this);
        IView::Probe(skipButton)->SetOnClickListener(skipButtonListener);

        view = Activity::FindViewById(R::id::next_button);
        mNextButton = IButton::Probe(view);
        AutoPtr<IViewOnClickListener> nextButtonListener = new NextButtonListener(this);
        IView::Probe(mNextButton)->SetOnClickListener(nextButtonListener);

        // set our various button parameters
        Boolean setNextText;
        if (intent->HasExtra(EXTRA_PREFS_SET_NEXT_TEXT, &setNextText), setNextText) {
            String buttonText;
            intent->GetStringExtra(EXTRA_PREFS_SET_NEXT_TEXT, &buttonText);
            if (TextUtils::IsEmpty(buttonText)) {
                IView::Probe(mNextButton)->SetVisibility(IView::GONE);
            }
            else {
                AutoPtr<ICharSequence> cButtonText;
                CString::New(buttonText, (ICharSequence**)&cButtonText);
                ITextView::Probe(mNextButton)->SetText(cButtonText);
            }
        }
        Boolean setBackText;
        if (intent->HasExtra(EXTRA_PREFS_SET_NEXT_TEXT, &setBackText), setBackText) {
            String buttonText;
            intent->GetStringExtra(EXTRA_PREFS_SET_BACK_TEXT, &buttonText);
            if (TextUtils::IsEmpty(buttonText)) {
                IView::Probe(backButton)->SetVisibility(IView::GONE);
            }
            else {
                AutoPtr<ICharSequence> cButtonText;
                CString::New(buttonText, (ICharSequence**)&cButtonText);
                ITextView::Probe(backButton)->SetText(cButtonText);
            }
        }
        Boolean showSkip;
        if (intent->GetBooleanExtra(EXTRA_PREFS_SHOW_SKIP, FALSE, &showSkip), showSkip) {
            IView::Probe(skipButton)->SetVisibility(IView::VISIBLE);
        }
    }
    return NOERROR;
}

ECode PreferenceActivity::HasHeaders(
    /* [out] */ Boolean* hasHeaders)
{
    VALIDATE_NOT_NULL(hasHeaders)
    AutoPtr<IListView> lw;
    GetListView((IListView**)&lw);
    Int32 visibility;
    IView::Probe(lw)->GetVisibility(&visibility);
    *hasHeaders = visibility == IView::VISIBLE && mPreferenceManager == NULL;
    return NOERROR;
}

ECode PreferenceActivity::GetHeaders(
    /* [out] */ IList** headers)
{
    VALIDATE_NOT_NULL(headers)
    *headers = mHeaders.Get();
    REFCOUNT_ADD(*headers);
    return NOERROR;
}

ECode PreferenceActivity::IsMultiPane(
    /* [out] */ Boolean* isMultiPane)
{
    VALIDATE_NOT_NULL(isMultiPane)
    Boolean hasHeaders;
    HasHeaders(&hasHeaders);
    if (!hasHeaders) {
        *isMultiPane = hasHeaders;
    }
    else {
        Int32 visiblity;
        IView::Probe(mPrefsContainer)->GetVisibility(&visiblity);
        *isMultiPane = visiblity == IView::VISIBLE;
    }
    return NOERROR;
}

ECode PreferenceActivity::OnIsMultiPane(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IResources> resource;
    GetResources((IResources**)&resource);
    return resource->GetBoolean(R::bool_::preferences_prefer_dual_pane, result);
}

ECode PreferenceActivity::OnIsHidingHeaders(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    return intent->GetBooleanExtra(EXTRA_NO_HEADERS, FALSE, result);
}

ECode PreferenceActivity::OnGetInitialHeader(
    /* [out] */ IPreferenceActivityHeader** header)
{
    VALIDATE_NOT_NULL(header)
    Int32 size;
    mHeaders->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mHeaders->Get(i, (IInterface**)&obj);
        AutoPtr<IPreferenceActivityHeader> h = IPreferenceActivityHeader::Probe(obj);
        String fragment;
        if (h->GetFragment(&fragment), !fragment.IsNull()) {
            *header = h.Get();
            REFCOUNT_ADD(*header)
            return NOERROR;
        }
    }
    Slogger::E(TAG, "Must have at least one header with a fragment");
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode PreferenceActivity::OnGetNewHeader(
    /* [out] */ IPreferenceActivityHeader** header)
{
    VALIDATE_NOT_NULL(header)
    *header = NULL;
    return NOERROR;
}

ECode PreferenceActivity::OnBuildHeaders(
    /* [in] */ IList* target)
{
    // Should be overloaded by subclasses
    return NOERROR;
}

ECode PreferenceActivity::InvalidateHeaders()
{
    Boolean result;
    if (mHandler->HasMessages(MSG_BUILD_HEADERS, &result), !result) {
        return mHandler->SendEmptyMessage(MSG_BUILD_HEADERS, &result);
    }
    return NOERROR;
}

ECode PreferenceActivity::LoadHeadersFromResource(
    /* [in] */ Int32 resid,
    /* [in] */ IList* target)
{
    AutoPtr<IXmlResourceParser> parser;
    AutoPtr<IResources> resource;
    GetResources((IResources**)&resource);
    resource->GetXml(resid, (IXmlResourceParser**)&parser);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));

    Int32 type;
    AutoPtr<IXmlPullParser> xp = IXmlPullParser::Probe(parser);
    while (xp->Next(&type),
            (type != IXmlPullParser::END_DOCUMENT && type != IXmlPullParser::START_TAG)) {
        // Parse next until start tag is found
    }

    String nodeName;
    xp->GetName(&nodeName);
    if (!nodeName.Equals("preference-headers")) {
        String description;
        xp->GetPositionDescription(&description);
        Logger::E("PreferenceActivity", "XML document must start with <preference-headers> tag; found %s at %s"
                , nodeName.string(), description.string());
        if (parser != NULL) parser->Close();
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IBundle> curBundle;

    Int32 outerDepth;
    xp->GetDepth(&outerDepth);
    Int32 depth;
    while ((xp->Next(&type), (type != IXmlPullParser::END_DOCUMENT
           && type != IXmlPullParser::END_TAG)) || (xp->GetDepth(&depth), depth > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        xp->GetName(&nodeName);
        if (nodeName.Equals("header")) {
            AutoPtr<IPreferenceActivityHeader> header;
            CPreferenceActivityHeader::New((IPreferenceActivityHeader**)&header);

            Int32 size = ArraySize(R::styleable::PreferenceHeader);
            AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::PreferenceHeader, size);
            AutoPtr<ITypedArray> sa;
            ObtainStyledAttributes(attrs, layout, (ITypedArray**)&sa);
            Int32 id;
            sa->GetResourceId(R::styleable::PreferenceHeader_id, HEADER_ID_UNDEFINED, &id);
            header->SetId(id);

            AutoPtr<ITypedValue> tv;
            sa->PeekValue(R::styleable::PreferenceHeader_title, (ITypedValue**)&tv);
            Int32 type;
            if (tv != NULL && (tv->GetType(&type), type == ITypedValue::TYPE_STRING)) {
                Int32 resourceId;
                tv->GetResourceId(&resourceId);
                if (resourceId != 0) {
                    header->SetTitleRes(resourceId);
                }
                else {
                    AutoPtr<ICharSequence> string;
                    tv->GetString((ICharSequence**)&string);
                    header->SetTitle(string);
                }
            }

            tv = NULL;
            sa->PeekValue(R::styleable::PreferenceHeader_summary, (ITypedValue**)&tv);
            if (tv != NULL && (tv->GetType(&type), type == ITypedValue::TYPE_STRING)) {
                Int32 resourceId;
                tv->GetResourceId(&resourceId);
                if (resourceId != 0) {
                    header->SetSummaryRes(resourceId);
                }
                else {
                    AutoPtr<ICharSequence> string;
                    tv->GetString((ICharSequence**)&string);
                    header->SetSummary(string);
                }
            }

            tv = NULL;
            sa->PeekValue(R::styleable::PreferenceHeader_breadCrumbTitle, (ITypedValue**)&tv);
            if (tv != NULL && (tv->GetType(&type), type == ITypedValue::TYPE_STRING)) {
                Int32 resourceId;
                tv->GetResourceId(&resourceId);
                if (resourceId != 0) {
                    header->SetBreadCrumbTitleRes(resourceId);
                }
                else {
                    AutoPtr<ICharSequence> string;
                    tv->GetString((ICharSequence**)&string);
                    header->SetBreadCrumbTitle(string);
                }
            }

            tv = NULL;
            sa->PeekValue(R::styleable::PreferenceHeader_breadCrumbShortTitle, (ITypedValue**)&tv);
            if (tv != NULL && (tv->GetType(&type), type == ITypedValue::TYPE_STRING)) {
                Int32 resourceId;
                tv->GetResourceId(&resourceId);
                if (resourceId != 0) {
                    header->SetBreadCrumbShortTitleRes(resourceId);
                }
                else {
                    AutoPtr<ICharSequence> string;
                    tv->GetString((ICharSequence**)&string);
                    header->SetBreadCrumbShortTitle(string);
                }
            }

            Int32 resourceId;
            sa->GetResourceId(R::styleable::PreferenceHeader_icon, 0, &resourceId);
            header->SetIconRes(resourceId);
            String fragment;
            sa->GetString(R::styleable::PreferenceHeader_fragment, &fragment);
            header->SetFragment(fragment);
            sa->Recycle();

            if (curBundle == NULL) {
                CBundle::New((IBundle**)&curBundle);
            }

            Int32 innerDepth;
            xp->GetDepth(&innerDepth);
            while ((xp->Next(&type), type != IXmlPullParser::END_DOCUMENT
                   && type != IXmlPullParser::END_TAG) || (xp->GetDepth(&depth), depth > innerDepth)) {
                if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                    continue;
                }

                String innerNodeName;
                xp->GetName(&innerNodeName);
                if (innerNodeName.Equals("extra")) {
                    resource->ParseBundleExtra(String("extra"), attrs, curBundle);
                    XmlUtils::SkipCurrentTag(IXmlPullParser::Probe(parser));

                }
                else if (innerNodeName.Equals("intent")) {
                    AutoPtr<IIntentHelper> helper;
                    CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
                    AutoPtr<IIntent> intent;
                    helper->ParseIntent(IResources::Probe(resource), IXmlPullParser::Probe(parser), IAttributeSet::Probe(attrs), (IIntent**)&intent);
                    header->SetIntent(intent);
                }
                else {
                    XmlUtils::SkipCurrentTag(IXmlPullParser::Probe(parser));
                }
            }

            if (curBundle->GetSize(&size), size > 0) {
                header->SetFragmentArguments(curBundle);
                curBundle = NULL;
            }

            target->Add(header);
        }
        else {
            XmlUtils::SkipCurrentTag(IXmlPullParser::Probe(parser));
        }
    }
    return NOERROR;
}

ECode PreferenceActivity::IsValidFragment(
    /* [in] */ const String& fragmentName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IApplicationInfo> info;
    Activity::GetApplicationInfo((IApplicationInfo**)&info);
    Int32 targetSdkVersion;
    info->GetTargetSdkVersion(&targetSdkVersion);
    if (targetSdkVersion  >= Elastos::Droid::Os::Build::VERSION_CODES::KITKAT) {
        Slogger::E(TAG,
            "Subclasses of PreferenceActivity must override isValidFragment(String) to verify that the Fragment class is valid! PreferenceActivity has not checked if fragment %s is valid.",
            fragmentName.string());
        return E_RUNTIME_EXCEPTION;
    } else {
        *result= TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode PreferenceActivity::SetListFooter(
    /* [in] */ IView* view)
{
    IViewGroup::Probe(mListFooter)->RemoveAllViews();
    AutoPtr<IViewGroupLayoutParams> layoutParams;
    CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT, (IFrameLayoutLayoutParams**)&layoutParams);
    return IViewGroup::Probe(mListFooter)->AddView(view, layoutParams);
}

ECode PreferenceActivity::OnStop()
{
    ListActivity::OnStop();
    if (mPreferenceManager != NULL) {
        mPreferenceManager->DispatchActivityStop();
    }
    return NOERROR;
}

ECode PreferenceActivity::OnDestroy()
{
    mHandler->RemoveMessages(MSG_BIND_PREFERENCES);
    mHandler->RemoveMessages(MSG_BUILD_HEADERS);
    ListActivity::OnDestroy();
    if (mPreferenceManager != NULL) {
        mPreferenceManager->DispatchActivityDestroy();
    }
    return NOERROR;
}

ECode PreferenceActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    ListActivity::OnSaveInstanceState(outState);

    Int32 size;
    if (mHeaders->GetSize(&size), size > 0) {
        AutoPtr<IArrayList> al = IArrayList::Probe(mHeaders);
        outState->PutParcelableArrayList(HEADERS_TAG, al);
        Int32 index;
        if (mCurHeader != NULL && (mHeaders->IndexOf(mCurHeader.Get(), &index), index >= 0)) {
            outState->PutInt32(CUR_HEADER_TAG, index);
        }
    }

    if (mPreferenceManager != NULL) {
        AutoPtr<IPreferenceScreen> preferenceScreen;
        GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
        if (preferenceScreen != NULL) {
            AutoPtr<IBundle> container;
            CBundle::New((IBundle**)&container);
            IPreference::Probe(preferenceScreen)->SaveHierarchyState(container);
            outState->PutBundle(PREFERENCES_TAG, container);
        }
    }
    return NOERROR;
}

ECode PreferenceActivity::OnRestoreInstanceState(
    /* [in] */ IBundle* outState)
{
    if (mPreferenceManager != NULL) {
        AutoPtr<IBundle> container;
        outState->GetBundle(PREFERENCES_TAG, (IBundle**)&container);
        if (container != NULL) {
            AutoPtr<IPreferenceScreen> preferenceScreen;
            GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
            if (preferenceScreen != NULL) {
                IPreference::Probe(preferenceScreen)->RestoreHierarchyState(container);
                mSavedInstanceState = outState;
                return NOERROR;
            }
        }
    }

    // Only call this if we didn't save the instance state for later.
    // If we did save it, it will be restored when we bind the adapter.
    return ListActivity::OnRestoreInstanceState(outState);
}

ECode PreferenceActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    ListActivity::OnActivityResult(requestCode, resultCode, data);
    if (mPreferenceManager != NULL) {
        mPreferenceManager->DispatchActivityResult(requestCode, resultCode, data);
    }
    return NOERROR;
}

ECode PreferenceActivity::OnContentChanged()
{
    ListActivity::OnContentChanged();
    if (mPreferenceManager != NULL) {
        PostBindPreferences();
    }
    return NOERROR;
}

ECode PreferenceActivity::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Boolean isResumed = FALSE;
    Activity::IsResumed(&isResumed);
    if (!isResumed) {
        return E_NULL_POINTER_EXCEPTION;
    }
    ListActivity::OnListItemClick(l, v, position, id);
    if (mAdapter != NULL) {
        AutoPtr<IInterface> item;
        IAdapter::Probe(mAdapter)->GetItem(position, (IInterface**)&item);
        AutoPtr<IPreferenceActivityHeader> header = IPreferenceActivityHeader::Probe(item);
        if (header) {
            OnHeaderClick(header, position);
        }
    }
    return NOERROR;
}

ECode PreferenceActivity::OnHeaderClick(
    /* [in] */ IPreferenceActivityHeader* header,
    /* [in] */ Int32 position)
{
    String fragment;
    header->GetFragment(&fragment);
    AutoPtr<IIntent> intent;
    if (!fragment.IsNull()) {
        if (mSinglePane) {
            Int32 titleRes;
            header->GetBreadCrumbTitleRes(&titleRes);
            Int32 shortTitleRes;
            header->GetBreadCrumbShortTitleRes(&shortTitleRes);
            if (titleRes == 0) {
                header->GetTitleRes(&titleRes);
                shortTitleRes = 0;
            }
            AutoPtr<IBundle> fragmentArguments;
            header->GetFragmentArguments((IBundle**)&fragmentArguments);
            StartWithFragment(fragment, fragmentArguments, NULL, 0, titleRes, shortTitleRes);
        }
        else {
            SwitchToHeader(header);
        }
    }
    else if (header->GetIntent((IIntent**)&intent), intent != NULL) {
        StartActivity(intent);
    }
    return NOERROR;
}

ECode PreferenceActivity::OnBuildStartFragmentIntent(
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args,
    /* [in] */ Int32 titleRes,
    /* [in] */ Int32 shortTitleRes,
    /* [out] */ IIntent** _intent)
{
    VALIDATE_NOT_NULL(_intent)

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);

    ClassID id;
    GetClassID(&id);
    intent->SetClass(this, id);
    intent->PutExtra(EXTRA_SHOW_FRAGMENT, fragmentName);
    intent->PutExtra(EXTRA_SHOW_FRAGMENT_ARGUMENTS, args);
    intent->PutExtra(EXTRA_SHOW_FRAGMENT_TITLE, titleRes);
    intent->PutExtra(EXTRA_SHOW_FRAGMENT_SHORT_TITLE, shortTitleRes);
    intent->PutBooleanExtra(EXTRA_NO_HEADERS, TRUE);

    *_intent = intent;
    REFCOUNT_ADD(*_intent)
    return NOERROR;
}

ECode PreferenceActivity::StartWithFragment(
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args,
    /* [in] */ IFragment* resultTo,
    /* [in] */ Int32 resultRequestCode)
{
    return StartWithFragment(fragmentName, args, resultTo, resultRequestCode, 0, 0);;
}

ECode PreferenceActivity::StartWithFragment(
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args,
    /* [in] */ IFragment* resultTo,
    /* [in] */ Int32 resultRequestCode,
    /* [in] */ Int32 titleRes,
    /* [in] */ Int32 shortTitleRes)
{
    AutoPtr<IIntent> intent;
    OnBuildStartFragmentIntent(fragmentName, args, titleRes, shortTitleRes, (IIntent**)&intent);
    if (resultTo == NULL) {
        return StartActivity(intent);
    }
    else {
        return resultTo->StartActivityForResult(intent, resultRequestCode);
    }
}

ECode PreferenceActivity::ShowBreadCrumbs(
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* shortTitle)
{
    if (mFragmentBreadCrumbs == NULL) {
        AutoPtr<IView> crumbs = Activity::FindViewById(R::id::title);
        // For screens with a different kind of title, don't create breadcrumbs.
        mFragmentBreadCrumbs = IFragmentBreadCrumbs::Probe(crumbs);
        if (mFragmentBreadCrumbs == NULL) {
            SetTitle(title);
            return E_NULL_POINTER_EXCEPTION;
        }
        if (mFragmentBreadCrumbs == NULL) {
            if (title != NULL) {
                SetTitle(title);
            }
            return E_NULL_POINTER_EXCEPTION;
        }
        if (mSinglePane) {
            IView::Probe(mFragmentBreadCrumbs)->SetVisibility(IView::GONE);
            // Hide the breadcrumb section completely for single-pane
            AutoPtr<IView> bcSection = Activity::FindViewById(R::id::breadcrumb_section);
            if (bcSection != NULL) {
                bcSection->SetVisibility(IView::GONE);
            }
            SetTitle(title);
        }
        mFragmentBreadCrumbs->SetMaxVisible(2);
        mFragmentBreadCrumbs->SetActivity(this);
    }
    Int32 res;
    IView::Probe(mFragmentBreadCrumbs)->GetVisibility(&res);
    if (res != IView::VISIBLE) {
        SetTitle(title);
    } else {
        //some peple forget to write setTitle in FragmentBreadCrumbs.car
        mFragmentBreadCrumbs->SetTitle(title, shortTitle);
        mFragmentBreadCrumbs->SetParentTitle(NULL, NULL, NULL);
    }
    return NOERROR;
}

ECode PreferenceActivity::SetParentTitle(
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* shortTitle,
    /* [in] */ IViewOnClickListener* listener)
{
    if (mFragmentBreadCrumbs != NULL) {
        return mFragmentBreadCrumbs->SetParentTitle(title, shortTitle, listener);
    }
    return NOERROR;
}

ECode PreferenceActivity::SetSelectedHeader(
    /* [in] */ IPreferenceActivityHeader* header)
{
    mCurHeader = header;
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    Int32 index;
    if (mHeaders->IndexOf(header, &index), index >= 0) {
        IAbsListView::Probe(listView)->SetItemChecked(index, TRUE);
    }
    else {
        IAbsListView::Probe(listView)->ClearChoices();
    }
    ShowBreadCrumbs(header);
    return NOERROR;
}

ECode PreferenceActivity::ShowBreadCrumbs(
    /* [in] */ IPreferenceActivityHeader* header)
{
    if (header != NULL) {
        AutoPtr<IResources> resource;
        GetResources((IResources**)&resource);
        AutoPtr<ICharSequence> title;
        header->GetBreadCrumbTitle(resource, (ICharSequence**)&title);
        if (title == NULL) {
            header->GetTitle(resource, (ICharSequence**)&title);
        }
        if (title == NULL) {
            GetTitle((ICharSequence**)&title);
        }
        AutoPtr<ICharSequence> breadCrumbShortTitle;
        header->GetBreadCrumbShortTitle(resource, (ICharSequence**)&breadCrumbShortTitle);
        ShowBreadCrumbs(title, breadCrumbShortTitle);
    }
    else {
        AutoPtr<ICharSequence> title;
        GetTitle((ICharSequence**)&title);
        ShowBreadCrumbs(title, NULL);
    }
    return NOERROR;
}

ECode PreferenceActivity::SwitchToHeaderInner(
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args)
{
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    manager->PopBackStack(BACK_STACK_PREFS, IFragmentManager::POP_BACK_STACK_INCLUSIVE);

    Boolean result = FALSE;
    IsValidFragment(fragmentName, &result);
    if (!result) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IFragment> f;
    Fragment::Instantiate(this, fragmentName, args, (IFragment**)&f);
    AutoPtr<IFragmentTransaction> transaction;
    manager->BeginTransaction((IFragmentTransaction**)&transaction);
    transaction->SetTransition(IFragmentTransaction::TRANSIT_FRAGMENT_FADE);
    transaction->Replace(R::id::prefs, f);
    Int32 res;
    transaction->CommitAllowingStateLoss(&res);
    return NOERROR;
}

ECode PreferenceActivity::SwitchToHeader(
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args)
{
    AutoPtr<IPreferenceActivityHeader> selectedHeader = NULL;
    Int32 size;
    mHeaders->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mHeaders->Get(i, (IInterface**)&obj);
        AutoPtr<IPreferenceActivityHeader> pah = IPreferenceActivityHeader::Probe(obj);
        String fragment;
        pah->GetFragment(&fragment);
        if (fragmentName.Equals(fragment)) {
            selectedHeader = pah;
            break;
        }
    }
    SetSelectedHeader(selectedHeader);
    SwitchToHeaderInner(fragmentName, args);
    return NOERROR;
}

ECode PreferenceActivity::SwitchToHeader(
    /* [in] */ IPreferenceActivityHeader* header)
{
    if (mCurHeader.Get() == header) {
        // This is the header we are currently displaying.  Just make sure
        // to pop the stack up to its root state.
        AutoPtr<IFragmentManager> manager;
        GetFragmentManager((IFragmentManager**)&manager);
        manager->PopBackStack(BACK_STACK_PREFS, IFragmentManager::POP_BACK_STACK_INCLUSIVE);
    }
    else {
        String fragment;
        header->GetFragment(&fragment);
        AutoPtr<IBundle> fragmentArguments;
        header->GetFragmentArguments((IBundle**)&fragmentArguments);
        if (fragment.IsNull()) {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        SwitchToHeaderInner(fragment, fragmentArguments);
        SetSelectedHeader(header);
    }

    return NOERROR;
}

ECode PreferenceActivity::FindBestMatchingHeader(
    /* [in] */ IPreferenceActivityHeader* cur,
    /* [in] */ IList* form,
    /* [out] */ IPreferenceActivityHeader** h)
{
    VALIDATE_NOT_NULL(h)
    AutoPtr<IArrayList> matches;
    Int32 size;
    form->GetSize(&size);
    for (Int32 j = 0; j < size; j++) {
        AutoPtr<IInterface> obj;
        form->Get(j, (IInterface**)&obj);
        AutoPtr<IPreferenceActivityHeader> oh = IPreferenceActivityHeader::Probe(obj.Get());
        Int64 curId;
        cur->GetId(&curId);
        Int64 ohId;
        oh->GetId(&ohId);
        if (cur == oh || (curId != HEADER_ID_UNDEFINED && curId == ohId)) {
            // Must be this one.
            IList::Probe(matches)->Clear();
            IList::Probe(matches)->Add(oh);
            break;
        }
        String curFragment, ohFragment;
        AutoPtr<IIntent> curIntent, ohIntent;
        AutoPtr<ICharSequence> curTitle, ohTitle;
        Boolean isEquals = FALSE;
        if ((cur->GetFragment(&curFragment), !curFragment.IsNull()) &&
            (oh->GetFragment(&ohFragment), curFragment.Equals(ohFragment))) {
            IList::Probe(matches)->Add(oh);
        }
        else if ((cur->GetIntent((IIntent**)&curIntent), curIntent != NULL) &&
            (oh->GetIntent((IIntent**)&ohIntent), IObject::Probe(curIntent)->Equals(ohIntent, &isEquals), isEquals)) {
            IList::Probe(matches)->Add(oh);
        }
        else if ((cur->GetTitle((ICharSequence**)&curTitle), curTitle != NULL) &&
            (oh->GetTitle((ICharSequence**)&ohTitle), IObject::Probe(curTitle)->Equals(ohTitle, &isEquals), isEquals)) {
            IList::Probe(matches)->Add(oh);
        }
    }
    Int32 NM;
    matches->GetSize(&NM);
    if (NM == 1) {
        AutoPtr<IInterface> obj;
        matches->Get(0, (IInterface**)&obj);
        *h = IPreferenceActivityHeader::Probe(obj);
        REFCOUNT_ADD(*h);
        return NOERROR;
    }
    else if (NM > 1) {
        for (Int32 j=0; j < NM; j++) {
            AutoPtr<IInterface> obj;
            matches->Get(j, (IInterface**)&obj);
            AutoPtr<IPreferenceActivityHeader> oh = IPreferenceActivityHeader::Probe(obj);
            AutoPtr<IBundle> curFragmentArguments, ohFragmentArguments;
            AutoPtr<IBundle> curExtras, ohExtras;
            AutoPtr<ICharSequence> curTitle, ohTitle;
            Boolean isEquals = FALSE;
            if ((cur->GetFragmentArguments((IBundle**)&curFragmentArguments), curFragmentArguments != NULL) &&
                (oh->GetFragmentArguments((IBundle**)&ohFragmentArguments), IObject::Probe(curFragmentArguments)->Equals(ohFragmentArguments, &isEquals), isEquals)) {
                *h = oh.Get();
                REFCOUNT_ADD(*h)
                return NOERROR;
            }
            if ((cur->GetExtras((IBundle**)&curExtras), curExtras != NULL) &&
                (oh->GetExtras((IBundle**)&ohExtras), IObject::Probe(curExtras)->Equals(ohExtras, &isEquals), isEquals)) {
                *h = oh.Get();
                REFCOUNT_ADD(*h)
                return NOERROR;
            }
            if ((cur->GetTitle((ICharSequence**)&curTitle), curTitle != NULL) &&
                (oh->GetTitle((ICharSequence**)&ohTitle), IObject::Probe(curTitle)->Equals(ohTitle, &isEquals), isEquals)) {
                *h = oh.Get();
                REFCOUNT_ADD(*h)
                return NOERROR;
            }
        }
    }
    *h = NULL;
    return NOERROR;
}

ECode PreferenceActivity::StartPreferenceFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ Boolean push)
{
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    AutoPtr<IFragmentTransaction> transaction;
    manager->BeginTransaction((IFragmentTransaction**)&transaction);
    transaction->Replace(R::id::prefs, fragment);
    if (push) {
        transaction->SetTransition(IFragmentTransaction::TRANSIT_FRAGMENT_OPEN);
        transaction->AddToBackStack(BACK_STACK_PREFS);
    }
    else {
        transaction->SetTransition(IFragmentTransaction::TRANSIT_FRAGMENT_FADE);
    }
    Int32 res;
    transaction->CommitAllowingStateLoss(&res);
    return NOERROR;
}

ECode PreferenceActivity::StartPreferencePanel(
    /* [in] */ const String& fragmentClass,
    /* [in] */ IBundle* args,
    /* [in] */ Int32 titleRes,
    /* [in] */ ICharSequence* titleText,
    /* [in] */ IFragment* resultTo,
    /* [in] */ Int32 resultRequestCode)
{
    if (mSinglePane) {
        StartWithFragment(fragmentClass, args, resultTo, resultRequestCode, titleRes, 0);
    }
    else {
        AutoPtr<IFragment> f;
        Fragment::Instantiate(this, fragmentClass, args, (IFragment**)&f);
        if (resultTo != NULL) {
            f->SetTargetFragment(resultTo, resultRequestCode);
        }
        AutoPtr<IFragmentManager> manager;
        GetFragmentManager((IFragmentManager**)&manager);
        AutoPtr<IFragmentTransaction> transaction;
        manager->BeginTransaction((IFragmentTransaction**)&transaction);
        transaction->Replace(R::id::prefs, f);
        if (titleRes != 0) {
            transaction->SetBreadCrumbTitle(titleRes);
        }
        else if (titleText != NULL) {
            transaction->SetBreadCrumbTitle(titleText);
        }
        transaction->SetTransition(IFragmentTransaction::TRANSIT_FRAGMENT_OPEN);
        transaction->AddToBackStack(BACK_STACK_PREFS);
        Int32 res;
        transaction->CommitAllowingStateLoss(&res);
    }
    return NOERROR;
}

ECode PreferenceActivity::FinishPreferencePanel(
    /* [in] */ IFragment* caller,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData)
{
    if (mSinglePane) {
        SetResult(resultCode, resultData);
        Finish();
    }
    else {
        // XXX be smarter about popping the stack.
        OnBackPressed();
        if (caller != NULL) {
            AutoPtr<IFragment> targetFragment;
            caller->GetTargetFragment((IFragment**)&targetFragment);
            if (targetFragment != NULL) {
                Int32 targetRequestCode;
                caller->GetTargetRequestCode(&targetRequestCode);
                targetFragment->OnActivityResult(targetRequestCode, resultCode, resultData);
            }
        }
    }
    return NOERROR;
}

ECode PreferenceActivity::OnPreferenceStartFragment(
    /* [in] */ IPreferenceFragment* caller,
    /* [in] */ IPreference* pref,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String fragment;
    pref->GetFragment(&fragment);
    AutoPtr<IBundle> extras;
    pref->GetExtras((IBundle**)&extras);
    Int32 titleRes;
    pref->GetTitleRes(&titleRes);
    AutoPtr<ICharSequence> title;
    pref->GetTitle((ICharSequence**)&title);
    StartPreferencePanel(fragment, extras, titleRes, title, NULL, 0);
    *result = TRUE;
    return NOERROR;
}

void PreferenceActivity::PostBindPreferences()
{
    Boolean hasMessages;
    if (mHandler->HasMessages(MSG_BIND_PREFERENCES, &hasMessages), !hasMessages) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_BIND_PREFERENCES, (IMessage**)&msg);
        msg->SendToTarget();
    }
}

void PreferenceActivity::BindPreferences()
{
    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    if (preferenceScreen != NULL) {
        AutoPtr<IListView> listView;
        GetListView((IListView**)&listView);
        preferenceScreen->Bind(listView);
        if (mSavedInstanceState != NULL) {
            ListActivity::OnRestoreInstanceState(mSavedInstanceState);
            mSavedInstanceState = NULL;
        }
    }
}

ECode PreferenceActivity::GetPreferenceManager(
    /* [out] */ IPreferenceManager** manager)
{
    VALIDATE_NOT_NULL(manager)
    *manager = mPreferenceManager;
    REFCOUNT_ADD(*manager)
    return NOERROR;
}

ECode PreferenceActivity::RequirePreferenceManager()
{
    if (mPreferenceManager == NULL) {
        if (mAdapter == NULL) {
            Logger::E("PreferenceActivity", "This should be called after super.onCreate.");
            return E_RUNTIME_EXCEPTION;
        }

        Logger::E("PreferenceActivity", "Modern two-pane PreferenceActivity requires use of a PreferenceFragment");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode PreferenceActivity::SetPreferenceScreen(
    /* [in] */ IPreferenceScreen* preferenceScreen)
{
    FAIL_RETURN(RequirePreferenceManager())

    Boolean result;
    if ((mPreferenceManager->SetPreferences(preferenceScreen, &result), result) && preferenceScreen != NULL) {
        PostBindPreferences();
        AutoPtr<IPreferenceScreen> preferenceScreen;
        GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
        AutoPtr<ICharSequence> title;
        IPreference::Probe(preferenceScreen)->GetTitle((ICharSequence**)&title);
        // Set the title of the activity
        if (title != NULL) {
            SetTitle(title);
        }
    }
    return NOERROR;
}

ECode PreferenceActivity::GetPreferenceScreen(
    /* [out] */ IPreferenceScreen** screen)
{
    VALIDATE_NOT_NULL(screen)
    if (mPreferenceManager != NULL) {
        AutoPtr<IPreferenceScreen> s;
        mPreferenceManager->GetPreferenceScreen((IPreferenceScreen**)&s);
        *screen = s;
        REFCOUNT_ADD(*screen)
        return NOERROR;
    }
    *screen = NULL;
    return NOERROR;
}

ECode PreferenceActivity::AddPreferencesFromIntent(
    /* [in] */ IIntent* intent)
{
    FAIL_RETURN(RequirePreferenceManager())

    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    AutoPtr<IPreferenceScreen> preScreen;
    mPreferenceManager->InflateFromIntent(intent, preferenceScreen, (IPreferenceScreen**)&preScreen);
    SetPreferenceScreen(preScreen);
    return NOERROR;
}

ECode PreferenceActivity::AddPreferencesFromResource(
    /* [in] */ Int32 preferencesResId)
{
    FAIL_RETURN(RequirePreferenceManager())

    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    AutoPtr<IPreferenceScreen> preScreen;
    mPreferenceManager->InflateFromResource(this, preferencesResId, preferenceScreen, (IPreferenceScreen**)&preScreen);
    SetPreferenceScreen(preScreen);
    return NOERROR;
}

ECode PreferenceActivity::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode PreferenceActivity::FindPreference(
    /* [in] */ const String& key,
    /* [out] */ IPreference** preference)
{
    VALIDATE_NOT_NULL(preference)
    if (mPreferenceManager == NULL) {
        *preference = NULL;
        return NOERROR;
    }

    AutoPtr<ICharSequence> cs;
    CString::New(key, (ICharSequence**)&cs);
    return mPreferenceManager->FindPreference(cs, preference);
}

ECode PreferenceActivity::OnNewIntent(
    /* [in] */ IIntent* intent)
{
    if (mPreferenceManager != NULL) {
        mPreferenceManager->DispatchNewIntent(intent);
    }

    return NOERROR;
}

Boolean PreferenceActivity::HasNextButton()
{
    return mNextButton != NULL;
}

AutoPtr<IButton> PreferenceActivity::GetNextButton()
{
    return mNextButton;
}

} // Preference
} // Droid
} // Elastos