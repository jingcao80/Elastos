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

#include "elastos/droid/contacts/common/list/CContactListFilterView.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Contacts::Common::List::EIID_IContactListFilterView;
using Elastos::Droid::Contacts::Common::Model::Account::IAccountType;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

const String CContactListFilterView::TAG("CContactListFilterView");

CAR_INTERFACE_IMPL(CContactListFilterView, LinearLayout, IContactListFilterView)

CAR_OBJECT_IMPL(CContactListFilterView)

ECode CContactListFilterView::constructor(
    /* [in] */ IContext* context)
{
    return LinearLayout::constructor(context);
}

ECode CContactListFilterView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode CContactListFilterView::SetContactListFilter(
    /* [in] */ IContactListFilter* filter)
{
    mFilter = filter;
    return NOERROR;
}

ECode CContactListFilterView::GetContactListFilter(
    /* [out] */ IContactListFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    *filter = mFilter;
    REFCOUNT_ADD(*filter)
    return NOERROR;
}

ECode CContactListFilterView::SetSingleAccount(
    /* [in] */ Boolean flag)
{
    mSingleAccount = flag;
    return NOERROR;
}

ECode CContactListFilterView::SetActivated(
    /* [in] */ Boolean activated)
{
    FAIL_RETURN(LinearLayout::SetActivated(activated))
    if (mRadioButton != NULL) {
        ICheckable::Probe(mRadioButton)->SetChecked(activated);
    }
    else {
        // We're guarding against null-pointer exceptions,
        // but otherwise this code is not expected to work
        // properly if the button hasn't been initialized.
        Logger::E(TAG, "radio-button cannot be activated because it is null");
    }
    return NOERROR;
}

ECode CContactListFilterView::BindView(
    /* [in] */ IAccountTypeManager* accountTypes)
{
    if (mAccountType == NULL) {
        AutoPtr<IView> icon;
        FindViewById(Elastos::Droid::Dialer::R::id::icon, (IView**)&icon);
        mIcon = IImageView::Probe(icon);
        AutoPtr<IView> accountType;
        FindViewById(Elastos::Droid::Dialer::R::id::accountType, (IView**)&accountType);
        mAccountType = ITextView::Probe(accountType);
        AutoPtr<IView> accountUserName;
        FindViewById(Elastos::Droid::Dialer::R::id::accountUserName, (IView**)&accountUserName);
        mAccountUserName = ITextView::Probe(accountUserName);
        AutoPtr<IView> radioButton;
        FindViewById(Elastos::Droid::Dialer::R::id::radioButton, (IView**)&radioButton);
        mRadioButton = IRadioButton::Probe(radioButton);
        Boolean isActivated;
        IsActivated(&isActivated);
        ICheckable::Probe(mRadioButton)->SetChecked(isActivated);
    }

    if (mFilter == NULL) {
        mAccountType->SetText(Elastos::Droid::Dialer::R::string::contactsList);
        return NOERROR;
    }

    IView::Probe(mAccountUserName)->SetVisibility(IView::GONE);
    Int32 filterType;
    mFilter->GetFilterType(&filterType);
    switch (filterType) {
        case IContactListFilter::FILTER_TYPE_ALL_ACCOUNTS: {
            BindView(0, Elastos::Droid::Dialer::R::string::list_filter_all_accounts);
            break;
        }
        case IContactListFilter::FILTER_TYPE_STARRED: {
            BindView(Elastos::Droid::Dialer::R::drawable::ic_menu_star_holo_light,
                    Elastos::Droid::Dialer::R::string::list_filter_all_starred);
            break;
        }
        case IContactListFilter::FILTER_TYPE_CUSTOM: {
            BindView(Elastos::Droid::Dialer::R::drawable::ic_menu_settings_holo_light,
                    Elastos::Droid::Dialer::R::string::list_filter_customize);
            break;
        }
        case IContactListFilter::FILTER_TYPE_WITH_PHONE_NUMBERS_ONLY: {
            BindView(0, Elastos::Droid::Dialer::R::string::list_filter_phones);
            break;
        }
        case IContactListFilter::FILTER_TYPE_SINGLE_CONTACT: {
            BindView(0, Elastos::Droid::Dialer::R::string::list_filter_single);
            break;
        }
        case IContactListFilter::FILTER_TYPE_ACCOUNT: {
            IView::Probe(mAccountUserName)->SetVisibility(IView::VISIBLE);
            IView::Probe(mIcon)->SetVisibility(IView::VISIBLE);
            AutoPtr<IDrawable> icon;
            mFilter->GetIcon((IDrawable**)&icon);
            if (icon != NULL) {
                mIcon->SetImageDrawable(icon);
            }
            else {
                mIcon->SetImageResource(Elastos::Droid::Dialer::R::drawable::unknown_source);
            }
            AutoPtr<IAccountType> accountType;
            String at, dataSet;
            mFilter->GetAccountType(&at);
            mFilter->GetDataSet(&dataSet);
            accountTypes->GetAccountType(at, dataSet, (IAccountType**)&accountType);
            String accountName;
            mFilter->GetAccountName(&accountName);
            AutoPtr<ICharSequence> cs;
            CString::New(accountName, (ICharSequence**)&cs);
            mAccountUserName->SetText(cs);
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            AutoPtr<ICharSequence> label;
            accountType->GetDisplayLabel(ctx, (ICharSequence**)&label);
            mAccountType->SetText(label);
            break;
        }
    }
    return NOERROR;
}

void CContactListFilterView::BindView(
    /* [in] */ Int32 iconResource,
    /* [in] */ Int32 textResource)
{
    if (iconResource != 0) {
        IView::Probe(mIcon)->SetVisibility(IView::VISIBLE);
        mIcon->SetImageResource(iconResource);
    }
    else {
        IView::Probe(mIcon)->SetVisibility(IView::GONE);
    }

    mAccountType->SetText(textResource);
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
