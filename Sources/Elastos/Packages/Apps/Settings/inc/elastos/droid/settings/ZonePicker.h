/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IListFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISimpleAdapter;


using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IDate;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::IMap;
using Elastos::Utility::ITimeZone;
using libcore::Icu::IICU;
using libcore::Icu::ITimeZoneNames;

/**
 * The class displaying a list of time zones that match a filter string
 * such as "Africa", "Europe", etc. Choosing an item from the list will set
 * the time zone. Pressing Back without choosing from the list will not
 * result in a change in the time zone setting.
 */
public class ZonePicker extends ListFragment {
    private static const String TAG = "ZonePicker";

    public static interface ZoneSelectionListener {
        // You can add any argument if you really need it...
        CARAPI OnZoneSelected(TimeZone tz);
    }

    private static const String KEY_ID = "id";  // value: String
    private static const String KEY_DISPLAYNAME = "name";  // value: String
    private static const String KEY_GMT = "gmt";  // value: String
    private static const String KEY_OFFSET = "offset";  // value: Int32 (Integer)
    private static const String XMLTAG_TIMEZONE = "timezone";

    private static const Int32 HOURS_1 = 60 * 60000;

    private static const Int32 MENU_TIMEZONE = Menu.FIRST+1;
    private static const Int32 MENU_ALPHABETICAL = Menu.FIRST;

    private Boolean mSortedByTimezone;

    private SimpleAdapter mTimezoneSortedAdapter;
    private SimpleAdapter mAlphabeticalAdapter;

    private ZoneSelectionListener mListener;

    /**
     * Constructs an adapter with TimeZone list. Sorted by TimeZone in default.
     *
     * @param sortedByName use Name for sorting the list.
     */
    public static SimpleAdapter ConstructTimezoneAdapter(Context context,
            Boolean sortedByName) {
        return ConstructTimezoneAdapter(context, sortedByName,
                R.layout.date_time_setup_custom_list_item_2);
    }

    /**
     * Constructs an adapter with TimeZone list. Sorted by TimeZone in default.
     *
     * @param sortedByName use Name for sorting the list.
     */
    public static SimpleAdapter ConstructTimezoneAdapter(Context context,
            Boolean sortedByName, Int32 layoutId) {
        final String[] from = new String[] {KEY_DISPLAYNAME, KEY_GMT};
        final Int32[] to = new Int32[] {android.R.id.text1, android.R.id.text2};

        final String sortKey = (sortedByName ? KEY_DISPLAYNAME : KEY_OFFSET);
        final MyComparator comparator = new MyComparator(sortKey);
        ZoneGetter zoneGetter = new ZoneGetter();
        final List<HashMap<String, Object>> sortedList = zoneGetter->GetZones(context);
        Collections->Sort(sortedList, comparator);
        final SimpleAdapter adapter = new SimpleAdapter(context,
                sortedList,
                layoutId,
                from,
                to);

        return adapter;
    }

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
    public static Int32 GetTimeZoneIndex(SimpleAdapter adapter, TimeZone tz) {
        final String defaultId = tz->GetID();
        final Int32 listSize = adapter->GetCount();
        for (Int32 i = 0; i < listSize; i++) {
            // Using HashMap<String, Object> induces unnecessary warning.
            final HashMap<?,?> map = (HashMap<?,?>)adapter->GetItem(i);
            final String id = (String)map->Get(KEY_ID);
            if (defaultId->Equals(id)) {
                // If current timezone is in this list, move focus to it
                return i;
            }
        }
        return -1;
    }

    /**
     * @param item one of items in adapters. The adapter should be constructed by
     * {@link #ConstructTimezoneAdapter(Context, Boolean)}.
     * @return TimeZone object corresponding to the item.
     */
    public static TimeZone ObtainTimeZoneFromItem(Object item) {
        return TimeZone->GetTimeZone((String)((Map<?, ?>)item).Get(KEY_ID));
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        final Activity activity = GetActivity();
        mTimezoneSortedAdapter = ConstructTimezoneAdapter(activity, FALSE);
        mAlphabeticalAdapter = ConstructTimezoneAdapter(activity, TRUE);

        // Sets the adapter
        SetSorting(TRUE);
        SetHasOptionsMenu(TRUE);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        final View view = super->OnCreateView(inflater, container, savedInstanceState);
        final ListView list = (ListView) view->FindViewById(android.R.id.list);
        Utils->ForcePrepareCustomPreferencesList(container, view, list, FALSE);
        return view;
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        menu->Add(0, MENU_ALPHABETICAL, 0, R::string::zone_list_menu_sort_alphabetically)
            .SetIcon(android.R.drawable.ic_menu_sort_alphabetically);
        menu->Add(0, MENU_TIMEZONE, 0, R::string::zone_list_menu_sort_by_timezone)
            .SetIcon(R.drawable.ic_menu_3d_globe);
        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        if (mSortedByTimezone) {
            menu->FindItem(MENU_TIMEZONE).SetVisible(FALSE);
            menu->FindItem(MENU_ALPHABETICAL).SetVisible(TRUE);
        } else {
            menu->FindItem(MENU_TIMEZONE).SetVisible(TRUE);
            menu->FindItem(MENU_ALPHABETICAL).SetVisible(FALSE);
        }
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {

            case MENU_TIMEZONE:
                SetSorting(TRUE);
                return TRUE;

            case MENU_ALPHABETICAL:
                SetSorting(FALSE);
                return TRUE;

            default:
                return FALSE;
        }
    }

    CARAPI SetZoneSelectionListener(ZoneSelectionListener listener) {
        mListener = listener;
    }

    private void SetSorting(Boolean sortByTimezone) {
        final SimpleAdapter adapter =
                sortByTimezone ? mTimezoneSortedAdapter : mAlphabeticalAdapter;
        SetListAdapter(adapter);
        mSortedByTimezone = sortByTimezone;
        final Int32 defaultIndex = GetTimeZoneIndex(adapter, TimeZone->GetDefault());
        if (defaultIndex >= 0) {
            SetSelection(defaultIndex);
        }
    }

    static class ZoneGetter {
        private final List<HashMap<String, Object>> mZones =
                new ArrayList<HashMap<String, Object>>();
        private final HashSet<String> mLocalZones = new HashSet<String>();
        private final Date mNow = Calendar->GetInstance()->GetTime();
        private final SimpleDateFormat mZoneNameFormatter = new SimpleDateFormat("zzzz");

        private List<HashMap<String, Object>> GetZones(Context context) {
            for (String olsonId : TimeZoneNames->ForLocale(Locale->GetDefault())) {
                mLocalZones->Add(olsonId);
            }
            try {
                XmlResourceParser xrp = context->GetResources()->GetXml(R.xml.timezones);
                while (xrp->Next() != XmlResourceParser.START_TAG) {
                    continue;
                }
                xrp->Next();
                while (xrp->GetEventType() != XmlResourceParser.END_TAG) {
                    while (xrp->GetEventType() != XmlResourceParser.START_TAG) {
                        if (xrp->GetEventType() == XmlResourceParser.END_DOCUMENT) {
                            return mZones;
                        }
                        xrp->Next();
                    }
                    if (xrp->GetName()->Equals(XMLTAG_TIMEZONE)) {
                        String olsonId = xrp->GetAttributeValue(0);
                        AddTimeZone(olsonId);
                    }
                    while (xrp->GetEventType() != XmlResourceParser.END_TAG) {
                        xrp->Next();
                    }
                    xrp->Next();
                }
                xrp->Close();
            } catch (XmlPullParserException xppe) {
                Logger::E(TAG, "Ill-formatted timezones.xml file");
            } catch (java.io.IOException ioe) {
                Logger::E(TAG, "Unable to read timezones.xml file");
            }
            return mZones;
        }

        private void AddTimeZone(String olsonId) {
            // We always need the "GMT-07:00" string.
            final TimeZone tz = TimeZone->GetTimeZone(olsonId);

            // For the display name, we treat time zones within the country differently
            // from other countries' time zones. So in en_US you'd get "Pacific Daylight Time"
            // but in de_DE you'd get "Los Angeles" for the same time zone.
            String displayName;
            if (mLocalZones->Contains(olsonId)) {
                // Within a country, we just use the local name for the time zone.
                mZoneNameFormatter->SetTimeZone(tz);
                displayName = mZoneNameFormatter->Format(mNow);
            } else {
                // For other countries' time zones, we use the exemplar location.
                final String localeName = Locale->GetDefault()->ToString();
                displayName = TimeZoneNames->GetExemplarLocation(localeName, olsonId);
            }

            final HashMap<String, Object> map = new HashMap<String, Object>();
            map->Put(KEY_ID, olsonId);
            map->Put(KEY_DISPLAYNAME, displayName);
            map->Put(KEY_GMT, DateTimeSettings->GetTimeZoneText(tz, FALSE));
            map->Put(KEY_OFFSET, tz->GetOffset(mNow->GetTime()));

            mZones->Add(map);
        }
    }

    //@Override
    CARAPI OnListItemClick(ListView listView, View v, Int32 position, Int64 id) {
        // Ignore extra clicks
        if (!IsResumed()) return;
        final Map<?, ?> map = (Map<?, ?>)listView->GetItemAtPosition(position);
        final String tzId = (String) map->Get(KEY_ID);

        // Update the system timezone value
        final Activity activity = GetActivity();
        final AlarmManager alarm = (AlarmManager) activity->GetSystemService(Context.ALARM_SERVICE);
        alarm->SetTimeZone(tzId);
        final TimeZone tz = TimeZone->GetTimeZone(tzId);
        if (mListener != NULL) {
            mListener->OnZoneSelected(tz);
        } else {
            GetActivity()->OnBackPressed();
        }
    }

    private static class MyComparator implements Comparator<HashMap<?, ?>> {
        private String mSortingKey;

        public MyComparator(String sortingKey) {
            mSortingKey = sortingKey;
        }

        CARAPI SetSortingKey(String sortingKey) {
            mSortingKey = sortingKey;
        }

        public Int32 Compare(HashMap<?, ?> map1, HashMap<?, ?> map2) {
            Object value1 = map1->Get(mSortingKey);
            Object value2 = map2->Get(mSortingKey);

            /*
             * This should never happen, but just in-case, put non-comparable
             * items at the end.
             */
            if (!IsComparable(value1)) {
                return IsComparable(value2) ? 1 : 0;
            } else if (!IsComparable(value2)) {
                return -1;
            }

            return ((Comparable) value1).CompareTo(value2);
        }

        private Boolean IsComparable(Object value) {
            return (value != NULL) && (value instanceof Comparable);
        }
    }
}
