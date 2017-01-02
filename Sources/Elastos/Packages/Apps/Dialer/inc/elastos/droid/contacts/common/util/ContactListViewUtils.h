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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CONTACTLISTVIEWUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CONTACTLISTVIEWUTILS_H__

#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::Runnable;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

class ContactListViewUtils
{
private:
    class ApplyCardPaddingToViewRunnable : public Runnable
    {
    public:
        ApplyCardPaddingToViewRunnable(
            /* [in] */ IListView* listView,
            /* [in] */ IView* rootLayout,
            /* [in] */ Int32 listSpaceWeight,
            /* [in] */ Int32 listViewWeight)
            : mListView(listView)
            , mRootLayout(rootLayout)
            , mSpaceWeight(listViewWeight)
            , mWeight(listViewWeight)
        {}

        CARAPI Run();

    private:
        AutoPtr<IListView> mListView;
        AutoPtr<IView> mRootLayout;
        Int32 mSpaceWeight;
        Int32 mWeight;
    };

public:
    /**
     * Add padding to {@param listView} if this configuration has set both space weight and
     * view weight on the layout. Use this util method instead of defining the padding in the
     * layout file so that the {@param listView}'s padding can be set proportional to the card
     * padding.
     *
     * @param resources
     * @param listView ListView that we add padding to
     * @param rootLayout layout that contains ListView and R.id.list_card
     */
    static CARAPI ApplyCardPaddingToView(
        /* [in] */ IResources* resources,
        /* [in] */ IListView* listView,
        /* [in] */ IView* rootLayout);

private:
    static CARAPI_(void) AddPaddingToView(
        /* [in] */ IListView* listView,
        /* [in] */ Int32 parentWidth,
        /* [in] */ Int32 listSpaceWeight,
        /* [in] */ Int32 listViewWeight);

private:
    // These two constants will help add more padding for the text inside the card.
    static const Double TEXT_LEFT_PADDING_TO_CARD_PADDING_RATIO = 1.1;

    friend class ApplyCardPaddingToViewRunnable;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_CONTACTLISTVIEWUTILS_H__
