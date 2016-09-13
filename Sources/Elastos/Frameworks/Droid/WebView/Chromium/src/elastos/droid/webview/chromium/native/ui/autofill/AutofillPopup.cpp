
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/autofill/AutofillPopup.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/DropdownAdapter.h"
#include "elastos/core/CoreUtils.h"

using Elastos::Droid::Webkit::Webview::Chromium::Ui::DropdownAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Autofill {

//=====================================================================
//                            AutofillPopup
//=====================================================================
const Int32 AutofillPopup::ITEM_ID_AUTOCOMPLETE_ENTRY;
const Int32 AutofillPopup::ITEM_ID_PASSWORD_ENTRY;
const Int32 AutofillPopup::ITEM_ID_SEPARATOR_ENTRY;
const Int32 AutofillPopup::ITEM_ID_DATA_LIST_ENTRY;

CAR_INTERFACE_IMPL(AutofillPopup::InnerListener, Object, IAdapterViewOnItemClickListener);

AutofillPopup::InnerListener::InnerListener(
  /* [in] */ AutofillPopup* host);

// @Override
ECode AutofillPopup::InnerListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}

AutofillPopup::AutofillPopup(
    /* [in] */ IContext* context,
    /* [in] */ ViewElastosDelegate* viewElastosDelegate,
    /* [in] */ AutofillPopupDelegate* autofillCallback)
    : DropdownPopupWindow(context, viewElastosDelegate)
    , mContext(context)
    , mAutofillCallback(autofillCallback)
{
    // ==================before translated======================
    // super(context, viewAndroidDelegate);
    // mContext = context;
    // mAutofillCallback = autofillCallback;
    //
    // setOnItemClickListener(this);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    SetOnItemClickListener(listener);
}

ECode AutofillPopup::FilterAndShow(
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    VALIDATE_NOT_NULL(suggestions);
    // ==================before translated======================
    // mSuggestions = new ArrayList<AutofillSuggestion>(Arrays.asList(suggestions));
    // // Remove the AutofillSuggestions with IDs that are not supported by Android
    // ArrayList<DropdownItem> cleanedData = new ArrayList<DropdownItem>();
    // HashSet<Integer> separators = new HashSet<Integer>();
    // for (int i = 0; i < suggestions.length; i++) {
    //     int itemId = suggestions[i].mUniqueId;
    //     if (itemId > 0 || itemId == ITEM_ID_AUTOCOMPLETE_ENTRY ||
    //             itemId == ITEM_ID_PASSWORD_ENTRY || itemId == ITEM_ID_DATA_LIST_ENTRY) {
    //         cleanedData.add(suggestions[i]);
    //     } else if (itemId == ITEM_ID_SEPARATOR_ENTRY) {
    //         separators.add(cleanedData.size());
    //     }
    // }
    // setAdapter(new DropdownAdapter(mContext, cleanedData, separators));
    // show();

    CArrayList::New((IList**)&mSuggestions);
    for (Int32 idx=0; idx<suggestions->GetLength(); ++idx) {
        AutoPtr<IInterface> interfaceTmp = (*suggestions)[idx];
        mSuggestions->Add(interfaceTmp);
    }

    // Remove the AutofillSuggestions with IDs that are not supported by Android
    AutoPtr<IList> cleanedData;
    CArrayList::New((IList**)&cleanedData);

    AutoPtr<ISet> separators;
    CHashSet::New((ISet**)&separators);

    Int32 size = 0;
    mSuggestions->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mSuggestions->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        AutofillSuggestion* suggestion = (AutofillSuggestion*)objTmp;
        assert(suggestion);
        Int32 itemId = suggestion->mUniqueId;
        if (itemId > 0 || itemId == ITEM_ID_AUTOCOMPLETE_ENTRY ||
                itemId == ITEM_ID_PASSWORD_ENTRY || itemId == ITEM_ID_DATA_LIST_ENTRY) {
            cleanedData->Add(interfaceTmp);
        }
        else if (itemId == ITEM_ID_SEPARATOR_ENTRY) {
            Int32 size = 0;
            cleanedData->GetSize(&size);
            separators->Add(TO_IINTERFACE(CoreUtils::Convert(size)));
        }
    }

    AutoPtr<DropdownAdapter> dropDownAdapter = new DropdownAdapter(mContext, cleanedData, separators);
    SetAdapter(dropDownAdapter);
    Show();
    return NOERROR;
}

ECode AutofillPopup::Dismiss()
{
    // ==================before translated======================
    // mAutofillCallback.requestHide();

    mAutofillCallback->RequestHide();
    return NOERROR;
}

ECode AutofillPopup::Hide()
{
    // ==================before translated======================
    // super.dismiss();

    Dismiss();
    return NOERROR;
}

ECode AutofillPopup::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    VALIDATE_NOT_NULL(parent);
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // DropdownAdapter adapter = (DropdownAdapter) parent.getAdapter();
    // int listIndex = mSuggestions.indexOf(adapter.getItem(position));
    // assert listIndex > -1;
    // mAutofillCallback.suggestionSelected(listIndex);

    AutoPtr<IAdapter> adapterTmp;
    parent->GetAdapter((IAdapter**)&adapterTmp);
    DropdownAdapter* adapter = (DropdownAdapter*)adapterTmp.Get();

    AutoPtr<IInterface> interfaceTmp;
    adapter->GetItem(position, (IInterface**)&interfaceTmp);

    Int32 listIndex = 0;
    mSuggestions->IndexOf(interfaceTmp, &listIndex);
    assert (listIndex > -1);
    mAutofillCallback->SuggestionSelected(listIndex);
    return NOERROR;
}

} // namespace Autofill
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


