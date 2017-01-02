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

#ifndef __ELASTOS_DROID_SETTINGS_ZONEPICKER_H__
#define __ELASTOS_DROID_SETTINGS_ZONEPICKER_H__

#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/app/ListFragment.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::ListFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISimpleAdapter;
using Elastos::Core::IComparator;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * The class displaying a list of time zones that match a filter string
 * such as "Africa", "Europe", etc. Choosing an item from the list will set
 * the time zone. Pressing Back without choosing from the list will not
 * result in a change in the time zone setting.
 */
class ZonePicker
    : public ListFragment
{
protected:
    class ZoneGetter
        : public Object
    {
        friend class ZonePicker;
    public:
        ZoneGetter();
        ~ZoneGetter();

    private:
        CARAPI GetZones(
            /* [in] */ IContext* context,
            /* [out] */ IList** list); //  List<HashMap<String, Object>>

        CARAPI_(void) AddTimeZone(
            /* [in] */ const String& olsonId);

    private:
        // final List<HashMap<String, Object>> mZones =
        //     new ArrayList<HashMap<String, Object>>();
        AutoPtr<IList> mZones;
        // final HashSet<String> mLocalZones = new HashSet<String>();
        AutoPtr<IHashSet> mLocalZones;
        AutoPtr<IDate> mNow; // = Calendar->GetInstance()->GetTime();
        AutoPtr<ISimpleDateFormat> mZoneNameFormatter;// = new SimpleDateFormat("zzzz");
    };

private:
    class MyComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        MyComparator(
            /* [in] */ const String& sortingKey);

        ~MyComparator();


        CARAPI SetSortingKey(
            /* [in] */ const String& sortingKey);

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* _map1,
            /* [in] */ IInterface* _map2,
            /* [out] */ Int32* result);

    private:
        Boolean IsComparable(
            /* [in] */ IInterface* value);

    private:
        String mSortingKey;
    };

public:
    ZonePicker();

    ~ZonePicker();

    CARAPI constructor();

    /**
     * Constructs an adapter with TimeZone list. Sorted by TimeZone in default.
     *
     * @param sortedByName use Name for sorting the list.
     */
    static CARAPI_(AutoPtr<ISimpleAdapter>) ConstructTimezoneAdapter(
        /* [in] */ IContext* context,
        /* [in] */ Boolean sortedByName);

    /**
     * Constructs an adapter with TimeZone list. Sorted by TimeZone in default.
     *
     * @param sortedByName use Name for sorting the list.
     */
    static CARAPI_(AutoPtr<ISimpleAdapter>) ConstructTimezoneAdapter(
        /* [in] */ IContext* context,
        /* [in] */ Boolean sortedByName,
        /* [in] */ Int32 layoutId);

    /**
     * Searches {@link TimeZone} from the given {@link SimpleAdapter} object, and returns
     * the index for the TimeZone.
     *
     * @param adapter SimpleAdapter constructed by
     * {@link #ConstructTimezoneAdapter(Context, Boolean)}.
     * @param tz TimeZone to be searched.
     * @return Index for the given TimeZone. -1 when there's no corresponding list item.
     * returned.
     */
    static CARAPI_(Int32) GetTimeZoneIndex(
        /* [in] */ ISimpleAdapter* adapter,
        /* [in] */ ITimeZone* tz);

    /**
     * @param item one of items in adapters. The adapter should be constructed by
     * {@link #ConstructTimezoneAdapter(Context, Boolean)}.
     * @return TimeZone object corresponding to the item.
     */
    static CARAPI_(AutoPtr<ITimeZone>) ObtainTimeZoneFromItem(
        /* [in] */ IInterface* item);

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    CARAPI SetZoneSelectionListener(
        /* [in] */ IZoneSelectionListener* listener);

    //@Override
    CARAPI OnListItemClick(
        /* [in] */ IListView* listView,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

private:
    CARAPI_(void) SetSorting(
        /* [in] */ Boolean sortByTimezone);

private:
    static const String TAG;

    static const String KEY_ID;  // value: String
    static const String KEY_DISPLAYNAME;  // value: String
    static const String KEY_GMT;  // value: String
    static const String KEY_OFFSET;  // value: Int32 (Integer)
    static const String XMLTAG_TIMEZONE;

    static const Int32 HOURS_1;

    static const Int32 MENU_TIMEZONE = IMenu::FIRST + 1;
    static const Int32 MENU_ALPHABETICAL = IMenu::FIRST;

    Boolean mSortedByTimezone;

    AutoPtr<ISimpleAdapter> mTimezoneSortedAdapter;
    AutoPtr<ISimpleAdapter> mAlphabeticalAdapter;

    AutoPtr<IZoneSelectionListener> mListener;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_ZONEPICKER_H__

