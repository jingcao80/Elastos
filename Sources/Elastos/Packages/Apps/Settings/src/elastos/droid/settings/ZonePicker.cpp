
#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "elastos/droid/settings/ZonePicker.h"
#include "elastos/droid/settings/DateTimeSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Widget::CSimpleAdapter;
using Elastos::Core::CoreUtils;
using Elastos::Core::IComparable;
using Elastos::Core::EIID_IComparator;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::Logging::Slogger;
using Libcore::ICU::ITimeZoneNames;
using Libcore::ICU::CTimeZoneNames;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Settings {

const String ZonePicker::TAG("ZonePicker");

const String ZonePicker::KEY_ID("id");  // value: String
const String ZonePicker::KEY_DISPLAYNAME("name");  // value: String
const String ZonePicker::KEY_GMT("gmt");  // value: String
const String ZonePicker::KEY_OFFSET("offset");  // value: Int32 (Integer)
const String ZonePicker::XMLTAG_TIMEZONE("timezone");

const Int32 ZonePicker::HOURS_1 = 60 * 60000;

const Int32 ZonePicker::MENU_TIMEZONE;
const Int32 ZonePicker::MENU_ALPHABETICAL;

//===============================================================================
//                  ZonePicker::InitBroadcastReceiver
//===============================================================================

ZonePicker::ZoneGetter::ZoneGetter()
{
    CArrayList::New((IList**)&mZones);
    CHashSet::New((IHashSet**)&mLocalZones);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> calendar;
    helper->GetInstance((ICalendar**)&calendar);
    calendar->GetTime((IDate**)&mNow);
    CSimpleDateFormat::New(String("zzzz"), (ISimpleDateFormat**)&mZoneNameFormatter);
}

ZonePicker::ZoneGetter::~ZoneGetter()
{}

ECode ZonePicker::ZoneGetter::GetZones(
    /* [in] */ IContext* context,
    /* [out] */ IList** list) //  List<HashMap<String, Object>>
{
    VALIDATE_NOT_NULL(list)

    AutoPtr<ILocaleHelper> lHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lHelper);
    AutoPtr<ILocale> locale;
    lHelper->GetDefault((ILocale**)&locale);

    AutoPtr<ITimeZoneNames> tzn;
    CTimeZoneNames::AcquireSingleton((ITimeZoneNames**)&tzn);

    AutoPtr< ArrayOf<String> > array;
    tzn->ForLocale(locale, (ArrayOf<String>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        String olsonId = (*array)[i];
        mLocalZones->Add(CoreUtils::Convert(olsonId));
    }

    // try {
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IXmlResourceParser> xrp;
    resources->GetXml(R::xml::timezones, (IXmlResourceParser**)&xrp);
    Int32 next;
    IXmlPullParser* xpp = IXmlPullParser::Probe(xrp);
    while ((xpp->Next(&next), next) != IXmlPullParser::START_TAG) {
        continue;
    }
    xpp->Next(&next);
    Int32 eventType;
    while ((xpp->GetEventType(&eventType), eventType) != IXmlPullParser::END_TAG) {
        while ((xpp->GetEventType(&eventType), eventType) != IXmlPullParser::START_TAG) {
            if ((xpp->GetEventType(&eventType), eventType) == IXmlPullParser::END_DOCUMENT) {
                *list = mZones;
                REFCOUNT_ADD(*list)
                return NOERROR;
            }
            xpp->Next(&next);
        }
        String name;
        if ((xpp->GetName(&name), name).Equals(XMLTAG_TIMEZONE)) {
            String olsonId;
            xpp->GetAttributeValue(0, &olsonId);
            AddTimeZone(olsonId);
        }
        while ((xpp->GetEventType(&eventType), eventType) != IXmlPullParser::END_TAG) {
            xpp->Next(&next);
        }
        xpp->Next(&next);
    }
    xrp->Close();
    // } catch (XmlPullParserException xppe) {
    //     Logger::E(TAG, "Ill-formatted timezones.xml file");
    // } catch (java.io.IOException ioe) {
    //     Logger::E(TAG, "Unable to read timezones.xml file");
    // }
    *list = mZones;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

void ZonePicker::ZoneGetter::AddTimeZone(
    /* [in] */ const String& olsonId)
{
    // We always need the "GMT-07:00" string.
    AutoPtr<ITimeZoneHelper> helper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
    AutoPtr<ITimeZone> tz;
    helper->GetTimeZone(olsonId, (ITimeZone**)&tz);

    // For the display name, we treat time zones within the country differently
    // from other countries' time zones. So in en_US you'd get "Pacific Daylight Time"
    // but in de_DE you'd get "Los Angeles" for the same time zone.
    String displayName;
    Boolean res;
    mLocalZones->Contains(CoreUtils::Convert(olsonId), &res);
    if (res) {
        // Within a country, we just use the local name for the time zone.
        using Elastos::Text::IDateFormat;
        IDateFormat* zoneNameFormatter = IDateFormat::Probe(mZoneNameFormatter);
        zoneNameFormatter->SetTimeZone(tz);
        zoneNameFormatter->Format(mNow, &displayName);
    }
    else {
        // For other countries' time zones, we use the exemplar location.
        AutoPtr<ILocaleHelper> lHelper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lHelper);
        AutoPtr<ILocale> locale;
        lHelper->GetDefault((ILocale**)&locale);
        String localeName;
        locale->ToString(&localeName);
        AutoPtr<ITimeZoneNames> tzn;
        CTimeZoneNames::AcquireSingleton((ITimeZoneNames**)&tzn);
        tzn->GetExemplarLocation(localeName, olsonId, &displayName);
    }

    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    map->Put(CoreUtils::Convert(KEY_ID), CoreUtils::Convert(olsonId));
    map->Put(CoreUtils::Convert(KEY_DISPLAYNAME), CoreUtils::Convert(displayName));
    map->Put(CoreUtils::Convert(KEY_GMT), CoreUtils::Convert(DateTimeSettings::GetTimeZoneText(tz, FALSE)));
    Int64 nowTime;
    mNow->GetTime(&nowTime);
    Int32 offset;
    tz->GetOffset(nowTime, &offset);
    map->Put(CoreUtils::Convert(KEY_OFFSET), CoreUtils::Convert(offset));

    mZones->Add(map);
}

//===============================================================================
//                  ZonePicker::MyComparator
//===============================================================================

CAR_INTERFACE_IMPL(ZonePicker::MyComparator, Object, IComparator)

ZonePicker::MyComparator::MyComparator(
    /* [in] */ const String& sortingKey)
{
    mSortingKey = sortingKey;
}

ZonePicker::MyComparator::~MyComparator()
{}

ECode ZonePicker::MyComparator::SetSortingKey(
    /* [in] */ const String& sortingKey)
{
    mSortingKey = sortingKey;
    return NOERROR;
}

ECode ZonePicker::MyComparator::Compare(
    /* [in] */ IInterface* _map1,
    /* [in] */ IInterface* _map2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHashMap> map1 = IHashMap::Probe(_map1);
    AutoPtr<IHashMap> map2 = IHashMap::Probe(_map2);
    AutoPtr<IInterface> value1, value2;
    map1->Get(CoreUtils::Convert(mSortingKey), (IInterface**)&value1);
    map2->Get(CoreUtils::Convert(mSortingKey), (IInterface**)&value2);

    /*
     * This should never happen, but just in-case, put non-comparable
     * items at the end.
     */
    if (!IsComparable(value1)) {
        *result = IsComparable(value2) ? 1 : 0;
        return NOERROR;
    }
    else if (!IsComparable(value2)) {
        *result = -1;
        return NOERROR;
    }

    return IComparable::Probe(value1)->CompareTo(value2, result);
}

Boolean ZonePicker::MyComparator::IsComparable(
    /* [in] */ IInterface* value)
{
    return (value != NULL) && (IComparable::Probe(value) != NULL);
}

//===============================================================================
//                  ZonePicker
//===============================================================================

ZonePicker::ZonePicker()
    : mSortedByTimezone(FALSE)
{}

ZonePicker::~ZonePicker()
{}

ECode ZonePicker::constructor()
{
    return ListFragment::constructor();
}

AutoPtr<ISimpleAdapter> ZonePicker::ConstructTimezoneAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Boolean sortedByName)
{
    return ConstructTimezoneAdapter(context, sortedByName,
            R::layout::date_time_setup_custom_list_item_2);
}

AutoPtr<ISimpleAdapter> ZonePicker::ConstructTimezoneAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Boolean sortedByName,
    /* [in] */ Int32 layoutId)
{
    AutoPtr< ArrayOf<String> > from = ArrayOf<String>::Alloc(2);
    (*from)[0] = KEY_DISPLAYNAME;
    (*from)[1] = KEY_GMT;

    AutoPtr< ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(2);
    (*to)[0] = Elastos::Droid::R::id::text1;
    (*to)[1] = Elastos::Droid::R::id::text2;

    String sortKey = (sortedByName ? KEY_DISPLAYNAME : KEY_OFFSET);
    AutoPtr<MyComparator> comparator = new MyComparator(sortKey);
    AutoPtr<ZoneGetter> zoneGetter = new ZoneGetter();
    AutoPtr<IList> sortedList; //List<HashMap<String, Object>>
    zoneGetter->GetZones(context, (IList**)&sortedList);
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(sortedList, comparator);
    AutoPtr<ISimpleAdapter> adapter;
    CSimpleAdapter::New(context, sortedList, layoutId, from, to, (ISimpleAdapter**)&adapter);

    return adapter;
}

Int32 ZonePicker::GetTimeZoneIndex(
    /* [in] */ ISimpleAdapter* adapter,
    /* [in] */ ITimeZone* tz)
{
    String defaultId;
    tz->GetID(&defaultId);
    AutoPtr<IAdapter> _adapter = IAdapter::Probe(adapter);
    Int32 listSize;
    _adapter->GetCount(&listSize);
    for (Int32 i = 0; i < listSize; i++) {
        // Using HashMap<String, Object> induces unnecessary warning.
        AutoPtr<IInterface> obj;
        _adapter->GetItem(i, (IInterface**)&obj);
        AutoPtr<IHashMap> map = IHashMap::Probe(obj);
        obj = NULL;
        map->Get(CoreUtils::Convert(KEY_ID), (IInterface**)&obj);
        String id;
        ICharSequence::Probe(obj)->ToString(&id);
        if (defaultId.Equals(id)) {
            // If current timezone is in this list, move focus to it
            return i;
        }
    }
    return -1;
}

AutoPtr<ITimeZone> ZonePicker::ObtainTimeZoneFromItem(
    /* [in] */ IInterface* item)
{
    AutoPtr<IInterface> obj;
    IMap::Probe(item)->Get(CoreUtils::Convert(KEY_ID), (IInterface**)&obj);
    String str;
    ICharSequence::Probe(obj)->ToString(&str);
    AutoPtr<ITimeZoneHelper> helper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
    AutoPtr<ITimeZone> tz;
    helper->GetTimeZone(str, (ITimeZone**)&tz);
    return tz;
}

ECode ZonePicker::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    Slogger::D("ZonePicker", " >> enter OnActivityCreated ");
    ListFragment::OnActivityCreated(savedInstanceState);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* _activity = IContext::Probe(activity);
    mTimezoneSortedAdapter = ConstructTimezoneAdapter(_activity, FALSE);
    mAlphabeticalAdapter = ConstructTimezoneAdapter(_activity, TRUE);

    // Sets the adapter
    SetSorting(TRUE);
    SetHasOptionsMenu(TRUE);
    Slogger::D("ZonePicker", " << leave OnActivityCreated ");
    return NOERROR;
}

ECode ZonePicker::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    Slogger::D("ZonePicker", " >> enter OnCreateView ");

    AutoPtr<IView> view;
    ListFragment::OnCreateView(inflater, container, savedInstanceState, (IView**)&view);
    AutoPtr<IView> tmp;
    view->FindViewById(Elastos::Droid::R::id::list, (IView**)&tmp);
    IListView* list = IListView::Probe(tmp);
    Utils::ForcePrepareCustomPreferencesList(container, view, list, FALSE);
    *result = view;
    REFCOUNT_ADD(*result)
    Slogger::D("ZonePicker", " << leave OnCreateView ");
    return NOERROR;
}

ECode ZonePicker::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    Slogger::D("ZonePicker", " >> enter OnCreateOptionsMenu ");
    AutoPtr<IMenuItem> item;
    menu->Add(0, MENU_ALPHABETICAL, 0, R::string::zone_list_menu_sort_alphabetically, (IMenuItem**)&item);
    item->SetIcon(Elastos::Droid::R::drawable::ic_menu_sort_alphabetically);

    item = NULL;
    menu->Add(0, MENU_TIMEZONE, 0, R::string::zone_list_menu_sort_by_timezone, (IMenuItem**)&item);
    item->SetIcon(R::drawable::ic_menu_3d_globe);
    ListFragment::OnCreateOptionsMenu(menu, inflater);

    Slogger::D("ZonePicker", " << leave OnCreateOptionsMenu ");
    return NOERROR;
}

ECode ZonePicker::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu)
{
    Slogger::D("ZonePicker", " >> enter OnPrepareOptionsMenu ");
    if (mSortedByTimezone) {
        AutoPtr<IMenuItem> item;
        menu->FindItem(MENU_TIMEZONE, (IMenuItem**)&item);
        item->SetVisible(FALSE);
        item = NULL;
        menu->FindItem(MENU_ALPHABETICAL, (IMenuItem**)&item);
        item->SetVisible(TRUE);
    }
    else {
        AutoPtr<IMenuItem> item;
        menu->FindItem(MENU_TIMEZONE, (IMenuItem**)&item);
        item->SetVisible(TRUE);
        item = NULL;
        menu->FindItem(MENU_ALPHABETICAL, (IMenuItem**)&item);
        item->SetVisible(FALSE);
    }
    Slogger::D("ZonePicker", " << leave OnPrepareOptionsMenu ");
    return NOERROR;
}

ECode ZonePicker::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case MENU_TIMEZONE:
            SetSorting(TRUE);
            *result = TRUE;
            return NOERROR;

        case MENU_ALPHABETICAL:
            SetSorting(FALSE);
            *result = TRUE;
            return NOERROR;

        default:
            *result = FALSE;
            return NOERROR;
    }
    return NOERROR;
}

ECode ZonePicker::SetZoneSelectionListener(
    /* [in] */ IZoneSelectionListener* listener)
{
    mListener = listener;
    return NOERROR;
}

void ZonePicker::SetSorting(
    /* [in] */ Boolean sortByTimezone)
{
    AutoPtr<ISimpleAdapter> adapter =
            sortByTimezone ? mTimezoneSortedAdapter : mAlphabeticalAdapter;
    SetListAdapter(IListAdapter::Probe(adapter));
    mSortedByTimezone = sortByTimezone;
    AutoPtr<ITimeZoneHelper> helper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
    AutoPtr<ITimeZone> tz;
    helper->GetDefault((ITimeZone**)&tz);
    Int32 defaultIndex = GetTimeZoneIndex(adapter, tz);
    if (defaultIndex >= 0) {
        SetSelection(defaultIndex);
    }
}

ECode ZonePicker::OnListItemClick(
    /* [in] */ IListView* listView,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    // Ignore extra clicks
    Boolean res;
    if (IsResumed(&res), !res) return NOERROR;

    AutoPtr<IInterface> obj;
    IAdapterView::Probe(listView)->GetItemAtPosition(position, (IInterface**)&obj);
    AutoPtr<IMap> map = IMap::Probe(obj);
    obj = NULL;
    map->Get(CoreUtils::Convert(KEY_ID), (IInterface**)&obj);
    String tzId;
    ICharSequence::Probe(obj)->ToString(&tzId);

    // Update the system timezone value
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    obj = NULL;
    IContext::Probe(activity)->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    AutoPtr<IAlarmManager> alarm = IAlarmManager::Probe(obj);
    alarm->SetTimeZone(tzId);

    AutoPtr<ITimeZoneHelper> helper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
    AutoPtr<ITimeZone> tz;
    helper->GetTimeZone(tzId, (ITimeZone**)&tz);

    if (mListener != NULL) {
        mListener->OnZoneSelected(tz);
    }
    else {
        activity->OnBackPressed();
    }
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos