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

#ifndef __ELASTOS_DROID_ACCOUNTS_CHOOSEACCOUNTACTIVITY_H__
#define __ELASTOS_DROID_ACCOUNTS_CHOOSEACCOUNTACTIVITY_H__

#include "_Elastos.Droid.Accounts.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include "elastos/droid/accounts/ChooseTypeAndAccountActivity.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Droid::View::ILayoutInflater;

namespace Elastos {
namespace Droid {
namespace Accounts {

class ChooseAccountActivity
    : public Activity
    , public IChooseAccountActivity
{
private:
    class AccountInfo
        : public Object
    {
    public:
        AccountInfo(
            /* [in] */ const String& name,
            /* [in] */ IDrawable* drawable)
            : mName(name)
            , mDrawable(drawable)
        {}

    public:
        String mName;
        AutoPtr<IDrawable> mDrawable;
    };

    class ViewHolder
        : public Object
    {
    public:
        AutoPtr<IImageView> mIcon;
        AutoPtr<ITextView> mText;
    };

    class AccountArrayAdapter
        : public ArrayAdapter
    {
    public:
        AccountArrayAdapter(
            /* [in] */ IContext* context,
            /* [in] */ Int32 textViewResourceId,
            /* [in] */ ArrayOf<AccountInfo*>* infos);

        virtual ~AccountArrayAdapter();

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    public:
        AutoPtr<ILayoutInflater> mLayoutInflater;
        AutoPtr<ArrayOf<AccountInfo*> > mInfos;
    };

    class AdapterViewOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        AdapterViewOnItemClickListener(
            /* [in] */ ChooseAccountActivity* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);
    private:
        ChooseAccountActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI Finish();

protected:
    CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

private:
    CARAPI_(void) GetAuthDescriptions();

    CARAPI_(AutoPtr<IDrawable>) GetDrawableForType(
        /* [in] */ const String& accountType);

private:
    static const String TAG;

    AutoPtr<ArrayOf<IParcelable*> > mAccounts;
    AutoPtr<IAccountManagerResponse> mAccountManagerResponse;
    AutoPtr<IBundle> mResult;

    AutoPtr<IHashMap> mTypeToAuthDescription;

    friend class AdapterViewOnItemClickListener;
};

} // Accounts
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Accounts::ChooseAccountActivity::AccountInfo, IInterface)

#endif //__ELASTOS_DROID_ACCOUNTS_CHOOSEACCOUNTACTIVITY_H__
