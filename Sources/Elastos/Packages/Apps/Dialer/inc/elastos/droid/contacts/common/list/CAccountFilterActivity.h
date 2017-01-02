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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CACCOUNTFILTERACTIVITY_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CACCOUNTFILTERACTIVITY_H__

#include "_Elastos_Droid_Contacts_Common_List_CAccountFilterActivity.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/app/Activity.h>
#include "elastos/droid/content/AsyncTaskLoader.h"
#include "elastos/droid/widget/BaseAdapter.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::ILoaderManagerLoaderCallbacks;
using Elastos::Droid::Content::AsyncTaskLoader;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Contacts::Common::Model::IAccountTypeManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

CarClass(CAccountFilterActivity)
    , public Activity
    , public IAccountFilterActivity
{
private:
    class InnerListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CAccountFilterActivity* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);
    private:
        CAccountFilterActivity* mHost;
    };

    class FilterLoader
        : public AsyncTaskLoader
    {
    public:
        FilterLoader(
            /* [in] */ IContext* context);

        // @Override
        CARAPI LoadInBackground(
            /* [out] */ IInterface** result);

    protected:
        // @Override
        CARAPI OnStartLoading();

        // @Override
        CARAPI OnStopLoading();

        // @Override
        CARAPI OnReset();

    private:
        AutoPtr<IContext> mContext;
    };

    class MyLoaderCallbacks
        : public Object
        , public ILoaderManagerLoaderCallbacks
    {
    public:
        MyLoaderCallbacks(
            /* [in] */ CAccountFilterActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnCreateLoader(
            /* [in] */ Int32 id,
            /* [in] */ IBundle* args,
            /* [out] */ ILoader** loader);

        // @Override
        CARAPI OnLoadFinished(
            /* [in] */ ILoader* loader,
            /* [in] */ IInterface* data);

        // @Override
        CARAPI OnLoaderReset(
            /* [in] */ ILoader* loader);

    private:
        CAccountFilterActivity* mHost;
    };

    class FilterListAdapter
        : public BaseAdapter
    {
    public:
        FilterListAdapter(
            /* [in] */ IContext* context,
            /* [in] */ IList* filters,
            /* [in] */ IContactListFilter* current);

        // @Override
        CARAPI GetCount(
            /* [out] */ Int32* count);

        // @Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        // @Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        // @Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    private:
        AutoPtr<IList> mFilters;
        AutoPtr<ILayoutInflater> mLayoutInflater;
        AutoPtr<IAccountTypeManager> mAccountTypes;
        AutoPtr<IContactListFilter> mCurrentFilter;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    // @Override
    virtual CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

private:
    static CARAPI_(AutoPtr<IList>) LoadAccountFilters(
        /* [in] */ IContext* context);

private:
    static const String TAG;

    static const Int32 SUBACTIVITY_CUSTOMIZE_FILTER = 0;

    static const Int32 FILTER_LOADER_ID = 0;

    AutoPtr<IListView> mListView;

    AutoPtr<IContactListFilter> mCurrentFilter;

    friend class FilterLoader;
    friend class MyLoaderCallbacks;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CACCOUNTFILTERACTIVITY_H__
