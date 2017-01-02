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

#ifndef __ELASTOS_DROID_DIALER_LIST_SHORTCUTCARDSADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_SHORTCUTCARDSADAPTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/dialer/calllog/CallLogAdapter.h"
#include "elastos/droid/dialer/calllog/CallLogQueryHandler.h"
#include "elastos/droid/dialer/list/SwipeHelper.h"
#include "elastos/droid/database/DataSetObserver.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Dialer::CallLog::CallLogAdapter;
using Elastos::Droid::Dialer::CallLog::CallLogQueryHandler;
using Elastos::Droid::Dialer::CallLog::ICallLogQueryHandlerListener;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

class ListsFragment;

/**
 * An adapter that displays call shortcuts from {@link com.android.dialer.calllog.CallLogAdapter}
 * in the form of cards.
 */
class ShortcutCardsAdapter
    : public BaseAdapter
    , public IShortcutCardsAdapter
{
private:
    class CustomDataSetObserver
        : public DataSetObserver
    {
    public:
        CustomDataSetObserver(
            /* [in] */ ShortcutCardsAdapter* host);

        // @Override
        CARAPI OnChanged();

        TO_STRING_IMPL("ShortcutCardsAdapter::CustomDataSetObserver")

    private:
        ShortcutCardsAdapter* mHost;
    };

    class CallLogOnItemSwipeListener
        : public Object
        , public ISwipeHelperOnItemGestureListener
    {
    public:
        CAR_INTERFACE_DECL()

        CallLogOnItemSwipeListener(
            /* [in] */ ShortcutCardsAdapter* host);

        // @Override
        CARAPI OnSwipe(
            /* [in] */ IView* view);

        // @Override
        CARAPI OnTouch();

        // @Override
        CARAPI IsSwipeEnabled(
            /* [out] */ Boolean* result);

    private:
        ShortcutCardsAdapter* mHost;
    };

    class CallLogQueryHandlerListener
        : public Object
        , public ICallLogQueryHandlerListener
    {
    public:
        CAR_INTERFACE_DECL()

        CallLogQueryHandlerListener(
            /* [in] */ ShortcutCardsAdapter* host);

        // @Override
        CARAPI OnVoicemailStatusFetched(
            /* [in] */ ICursor* statusCursor);

        // @Override
        CARAPI OnCallsFetched(
            /* [in] */ ICursor* combinedCursor,
            /* [out] */ Boolean* result);

    private:
        ShortcutCardsAdapter* mHost;
    };

    /**
     * The swipeable call log row.
     */
    class SwipeableShortcutCard
        : public FrameLayout
    {
    private:
        class InnerCallback
            : public Object
            , public ISwipeHelperCallback
        {
        public:
            CAR_INTERFACE_DECL()

            InnerCallback(
                /* [in] */ SwipeableShortcutCard* host)
                : mHost(host)
            {}

             // @Override
            CARAPI GetChildAtPosition(
                /* [in] */ IMotionEvent* ev,
                /* [out] */ IView** child);

            // @Override
            CARAPI GetChildContentView(
                /* [in] */ IView* v,
                /* [out] */ IView** child);

            // @Override
            CARAPI OnScroll();

            // @Override
            CARAPI CanChildBeDismissed(
                /* [in] */ IView* v,
                /* [out] */ Boolean* result);

            // @Override
            CARAPI OnBeginDrag(
                /* [in] */ IView* v);

            // @Override
            CARAPI OnChildDismissed(
                /* [in] */ IView* v);

            // @Override
            CARAPI OnDragCancelled(
                /* [in] */ IView* v);

        private:
            SwipeableShortcutCard* mHost;
        };

    public:
        SwipeableShortcutCard(
            /* [in] */ ShortcutCardsAdapter* host);

        CARAPI constructor(
            /* [in] */ IContext* context);

        // @Override
        CARAPI GetChildAtPosition(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ IView** child);

        // @Override
        CARAPI GetChildContentView(
            /* [in] */ IView* v,
            /* [out] */ IView** child);

        // @Override
        CARAPI OnScroll();

        // @Override
        CARAPI CanChildBeDismissed(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnBeginDrag(
            /* [in] */ IView* v);

        // @Override
        CARAPI OnChildDismissed(
            /* [in] */ IView* v);

        // @Override
        CARAPI OnDragCancelled(
            /* [in] */ IView* v);

        // @Override
        CARAPI OnInterceptTouchEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);

        CARAPI_(void) SetOnItemSwipeListener(
            /* [in] */ ISwipeHelperOnItemGestureListener* listener);

        /**
         * Clips the card by a specified amount.
         *
         * @param ratioHidden A float indicating how much of each edge of the card should be
         *         clipped. If 0, the entire card is displayed. If 0.5f, each edge is hidden
         *         entirely, thus obscuring the entire card.
         */
        CARAPI_(void) ClipCard(
            /* [in] */ Float ratioHidden);

    private:
        CARAPI_(void) PrepareChildView(
            /* [in] */ IView* view);

        CARAPI_(void) SetChildrenOpacity(
            /* [in] */ IViewGroup* viewGroup,
            /* [in] */ Float alpha);

    private:
        ShortcutCardsAdapter* mHost;
        AutoPtr<SwipeHelper> mSwipeHelper;
        AutoPtr<ISwipeHelperOnItemGestureListener> mOnItemSwipeListener;

        Float mPreviousTranslationZ;
        AutoPtr<IRect> mClipRect;

        friend class ShortcutCardsAdapter;
    };

public:
    CAR_INTERFACE_DECL()

    ShortcutCardsAdapter(
        /* [in] */ IContext* context,
        /* [in] */ ListsFragment* fragment,
        /* [in] */ CallLogAdapter* callLogAdapter);

    /**
     * Determines the number of items in the adapter.
     * mCallLogAdapter contains the item for the most recent caller.
     * mContactTileAdapter contains the starred contacts.
     * The +1 is to account for the presence of the favorites menu.
     *
     * @return Number of items in the adapter.
     */
    // @Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    // @Override
    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    // @Override
    CARAPI HasStableIds(
        /* [out] */ Boolean* result);

    /**
     * Determine the number of view types present.
     */
    // @Override
    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    // @Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    // @Override
    CARAPI AreAllItemsEnabled(
        /* [out] */ Boolean* enabled);

    // @Override
    CARAPI IsEnabled(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* enabled);

private:
    static const String TAG;
    static const Float CLIP_CARD_BARELY_HIDDEN_RATIO = 0.001f;
    static const Float CLIP_CARD_MOSTLY_HIDDEN_RATIO = 0.9f;
    // Fade out 5x faster than the hidden ratio.
    static const Float CLIP_CARD_OPACITY_RATIO = 5.0f;

    AutoPtr<CallLogAdapter> mCallLogAdapter;

    ListsFragment* mFragment;

    Int32 mCallLogMarginHorizontal;
    Int32 mCallLogMarginTop;
    Int32 mCallLogMarginBottom;
    Int32 mCallLogPaddingStart;
    Int32 mCallLogPaddingTop;
    Int32 mCallLogPaddingBottom;
    Int32 mCardMaxHorizontalClip;
    Int32 mShortCardBackgroundColor;

    AutoPtr<IContext> mContext;

    AutoPtr<IDataSetObserver> mObserver;

    AutoPtr<CallLogQueryHandler> mCallLogQueryHandler;

    AutoPtr<ISwipeHelperOnItemGestureListener> mCallLogOnItemSwipeListener;
    AutoPtr<ICallLogQueryHandlerListener> mCallLogQueryHandlerListener;

    friend class ListsFragment;
    friend class CallLogOnItemSwipeListener;
    friend class CallLogQueryHandlerListener;
    friend class SwipeableShortcutCard;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_SHORTCUTCARDSADAPTER_H__
