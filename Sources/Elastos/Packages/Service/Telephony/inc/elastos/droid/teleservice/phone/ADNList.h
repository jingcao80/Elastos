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

#ifndef  __ELASTOS_DROID_PHONE_ADNLIST_H__
#define  __ELASTOS_DROID_PHONE_ADNLIST_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/content/AsyncQueryHandler.h"
#include "elastos/droid/app/ListActivity.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::ListActivity;
using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Widget::ICursorAdapter;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Abbreviated Dial Numbers (ADN) list activity for the Phone app. By default, this class will show
 * you all Service Dialing Numbers (SDN) that are supported by a service provider.  SDNs are a form
 * of speed dial for accessing service provider contacts like "#MIN" for getting user minutes.
 * To see this class in use, trigger the radio info screen by dialing *#*#INFO#*#* and open the
 * menu.
 * This class can also be used as a base class for simple contact lists that can be represented with
 * only labels and numbers.
 */
class ADNList
    : public ListActivity
{
private:
    class QueryHandler
        : public AsyncQueryHandler
    {
    public:
        QueryHandler(
            /* [in] */ ADNList* host,
            /* [in] */ IContentResolver* cr);

    protected:
        //@Override
        CARAPI OnQueryComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ ICursor* cursor);

        //@Override
        CARAPI OnInsertComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ IUri* uri);

        //@Override
        CARAPI OnUpdateComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ Int32 result);

        //@Override
        CARAPI OnDeleteComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ Int32 result);

    private:
        ADNList* mHost;
    };

public:
    ADNList()
        : mInitialSelection(-1)
    {}

    CARAPI constructor();

    CARAPI ResolveIntent(
        /* [out] */ IUri** uri);

    CARAPI NewAdapter(
        /* [out] */ ICursorAdapter** adapter);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnStop();

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    CARAPI_(void) Query();

    CARAPI_(void) ReQuery();

    CARAPI_(void) SetAdapter();

    CARAPI_(void) DisplayProgress(
        /* [in] */ Boolean loading);

    static CARAPI_(Boolean) IsAirplaneModeOn(
        /* [in] */ IContext* context);

protected:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 NAME_COLUMN;
    static const Int32 NUMBER_COLUMN;
    static const Int32 EMAILS_COLUMN;

    static const Int32 QUERY_TOKEN;
    static const Int32 INSERT_TOKEN;
    static const Int32 UPDATE_TOKEN;
    static const Int32 DELETE_TOKEN;

    AutoPtr<QueryHandler> mQueryHandler;
    AutoPtr<ICursorAdapter> mCursorAdapter;
    AutoPtr<ICursor> mCursor;

    Int32 mInitialSelection;

private:
    static const AutoPtr<ArrayOf<String> > COLUMN_NAMES;

    static const AutoPtr<ArrayOf<Int32> > VIEW_NAMES;

    AutoPtr<ITextView> mEmptyText;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_ADNLIST_H__