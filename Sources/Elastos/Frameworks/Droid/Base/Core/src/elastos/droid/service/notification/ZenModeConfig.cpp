#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/net/CUriBuilder.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/service/notification/CCondition.h"
#include "elastos/droid/service/notification/CZenModeConfig.h"
#include "elastos/droid/service/notification/ZenModeConfig.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::EIID_IComponentName;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Droid::Net::EIID_IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Service::Notification::CCondition;
using Elastos::Droid::Service::Notification::CZenModeConfig;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::R;
using Elastos::Core::CArrayOf;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::Math;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::IString;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

//==================================================
// ZenModeConfig
//==================================================

static AutoPtr<ArrayOf<Int32> > Init1()
{
    AutoPtr<ArrayOf<Int32> > alldays = ArrayOf<Int32>::Alloc(7);
    (*alldays)[0] = ICalendar::SUNDAY;
    (*alldays)[1] = ICalendar::MONDAY;
    (*alldays)[2] = ICalendar::TUESDAY;
    (*alldays)[3] = ICalendar::WEDNESDAY;
    (*alldays)[4] = ICalendar::THURSDAY;
    (*alldays)[5] = ICalendar::FRIDAY;
    (*alldays)[6] = ICalendar::SATURDAY;
    return alldays;
}

static AutoPtr<ArrayOf<Int32> > Init2()
{
    AutoPtr<ArrayOf<Int32> > alldays = ArrayOf<Int32>::Alloc(5);
    (*alldays)[0] = ICalendar::SUNDAY;
    (*alldays)[1] = ICalendar::MONDAY;
    (*alldays)[2] = ICalendar::TUESDAY;
    (*alldays)[3] = ICalendar::WEDNESDAY;
    (*alldays)[4] = ICalendar::THURSDAY;
    return alldays;
}

static AutoPtr<ArrayOf<Int32> > Init3()
{
    AutoPtr<ArrayOf<Int32> > alldays = ArrayOf<Int32>::Alloc(8);
    (*alldays)[0] = 15;
    (*alldays)[1] = 30;
    (*alldays)[2] = 45;
    (*alldays)[3] = 60;
    (*alldays)[4] = 120;
    (*alldays)[5] = 180;
    (*alldays)[6] = 240;
    (*alldays)[7] = 480;
    return alldays;
}

INIT_PROI_2 const AutoPtr<ArrayOf<Int32> > ZenModeConfig::ALL_DAYS = Init1();
INIT_PROI_2 const AutoPtr<ArrayOf<Int32> > ZenModeConfig::WEEKNIGHT_DAYS = Init2();
INIT_PROI_2 const AutoPtr<ArrayOf<Int32> > ZenModeConfig::MINUTE_BUCKETS = Init3();

const String ZenModeConfig::TAG("ZenModeConfig");

const Int32 ZenModeConfig::SECONDS_MS = 1000;
const Int32 ZenModeConfig::MINUTES_MS = 60 * SECONDS_MS;
const Int32 ZenModeConfig::ZERO_VALUE_MS = 20 * SECONDS_MS;

const Boolean ZenModeConfig::DEFAULT_ALLOW_EVENTS = TRUE;

const Int32 ZenModeConfig::XML_VERSION = 1;
const String ZenModeConfig::ZEN_TAG("zen");
const String ZenModeConfig::ZEN_ATT_VERSION("version");
const String ZenModeConfig::ALLOW_TAG("allow");
const String ZenModeConfig::ALLOW_ATT_CALLS("calls");
const String ZenModeConfig::ALLOW_ATT_MESSAGES("messages");
const String ZenModeConfig::ALLOW_ATT_FROM("from");
const String ZenModeConfig::ALLOW_ATT_EVENTS("events");
const String ZenModeConfig::SLEEP_TAG("sleep");
const String ZenModeConfig::SLEEP_ATT_MODE("mode");

const String ZenModeConfig::SLEEP_ATT_START_HR("startHour");
const String ZenModeConfig::SLEEP_ATT_START_MIN("startMin");
const String ZenModeConfig::SLEEP_ATT_END_HR("endHour");
const String ZenModeConfig::SLEEP_ATT_END_MIN("endMin");

const String ZenModeConfig::CONDITION_TAG("condition");
const String ZenModeConfig::CONDITION_ATT_COMPONENT("component");
const String ZenModeConfig::CONDITION_ATT_ID("id");
const String ZenModeConfig::CONDITION_ATT_SUMMARY("summary");
const String ZenModeConfig::CONDITION_ATT_LINE1("line1");
const String ZenModeConfig::CONDITION_ATT_LINE2("line2");
const String ZenModeConfig::CONDITION_ATT_ICON("icon");
const String ZenModeConfig::CONDITION_ATT_STATE("state");
const String ZenModeConfig::CONDITION_ATT_FLAGS("flags");

const String ZenModeConfig::EXIT_CONDITION_TAG("exitCondition");
const String ZenModeConfig::EXIT_CONDITION_ATT_COMPONENT("component");

const String ZenModeConfig::SYSTEM_AUTHORITY("android");

const String ZenModeConfig::COUNTDOWN_PATH("countdown");

const String ZenModeConfig::DOWNTIME_PATH("downtime");

CAR_INTERFACE_IMPL_2(ZenModeConfig, Object, IZenModeConfig, IParcelable)

ZenModeConfig::ZenModeConfig()
    : mAllowCalls(FALSE)
    , mAllowMessages(FALSE)
    , mAllowEvents(DEFAULT_ALLOW_EVENTS)
    , mAllowFrom(SOURCE_ANYONE)
    , mSleepStartHour(0)
    , mSleepStartMinute(0)
    , mSleepEndHour(0)
    , mSleepEndMinute(0)
{
}

ECode ZenModeConfig::constructor()
{
    return NOERROR;
}

ECode ZenModeConfig::constructor(
    /* [in] */ IParcelable* source)
{
    return NOERROR;
}

ECode ZenModeConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 i;
    source->ReadInt32(&i);
    mAllowCalls = i == 1;
    source->ReadInt32(&i);
    mAllowMessages = i == 1;
    source->ReadInt32(&i);
    mAllowEvents = i == 1;
    source->ReadInt32(&i);
    if (i == 1) {
        source->ReadString(&mSleepMode);
    }
    source->ReadInt32(&mSleepStartHour);
    source->ReadInt32(&mSleepStartMinute);
    source->ReadInt32(&mSleepEndHour);
    source->ReadInt32(&mSleepEndMinute);
    Int32 len;
    source->ReadInt32(&len);
    if (len > 0) {
        mConditionComponents = ArrayOf<IComponentName*>::Alloc(len);
        for (Int32 i = 0; i < len; ++len) {
            AutoPtr<IInterface> obj;
            source->ReadInterfacePtr((Handle32*)&obj);
            mConditionComponents->Set(i, IComponentName::Probe(obj));
        }
    }

    source->ReadInt32(&len);
    if (len > 0) {
        mConditionIds = ArrayOf<IUri*>::Alloc(len);
        for (Int32 i = 0; i < len; ++len) {
            AutoPtr<IInterface> obj;
            source->ReadInterfacePtr((Handle32*)&obj);
            mConditionIds->Set(i, IUri::Probe(obj));
        }
    }

    source->ReadInt32(&mAllowFrom);

    if (source->ReadInt32(&i), i != 0) {
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mExitCondition = ICondition::Probe(obj);
    }

    if (source->ReadInt32(&i), i != 0) {
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mExitConditionComponent = IComponentName::Probe(obj);
    }
    return NOERROR;
}

ECode ZenModeConfig::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mAllowCalls ? 1 : 0);
    dest->WriteInt32(mAllowMessages ? 1 : 0);
    dest->WriteInt32(mAllowEvents ? 1 : 0);
    if (!mSleepMode.IsNull()) {
        dest->WriteInt32(1);
        dest->WriteString(mSleepMode);
    } else {
        dest->WriteInt32(0);
    }
    dest->WriteInt32(mSleepStartHour);
    dest->WriteInt32(mSleepStartMinute);
    dest->WriteInt32(mSleepEndHour);
    dest->WriteInt32(mSleepEndMinute);

    if (mConditionComponents != NULL && mConditionComponents->GetLength() > 0) {
        Int32 len = mConditionComponents->GetLength();
        for (Int32 i = 0; i < len; ++i) {
            dest->WriteInterfacePtr((*mConditionComponents)[i]);
        }
    }
    else {
        dest->WriteInt32(0);
    }

    if (mConditionIds != NULL && mConditionIds->GetLength() > 0) {
        Int32 len = mConditionIds->GetLength();
        for (Int32 i = 0; i < len; ++i) {
            dest->WriteInterfacePtr((*mConditionIds)[i]);
        }
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteInt32(mAllowFrom);

    if (NULL != mExitCondition) {
        dest->WriteInt32(1);
        dest->WriteInterfacePtr(mExitCondition);
    }
    else {
        dest->WriteInt32(0);
    }

    if (NULL != mExitConditionComponent) {
        dest->WriteInt32(1);
        dest->WriteInterfacePtr(mExitConditionComponent);
    }
    else {
        dest->WriteInt32(0);
    }

    return NOERROR;
}

ECode ZenModeConfig::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICharSequence> cs;
    CString::New(String(","), (ICharSequence**)&cs);

    StringBuilder sb(/*ZenModeConfig.class.getSimpleName()*/"ZenModeConfig[");
    sb += "allowCalls=";
    sb += mAllowCalls;
    sb += ",allowMessages=";
    sb += mAllowMessages;
    sb += ",allowFrom=";
    sb += mAllowFrom;
    sb += ",allowEvents=";
    sb += mAllowEvents;
    sb += ",sleepMode=";
    sb += mSleepMode;
    sb += ",sleepStart=";
    sb += mSleepStartHour;
    sb += '.';
    sb += mSleepStartMinute;
    sb += ",sleepEnd=";
    sb += mSleepEndHour;
    sb += '.';
    sb += mSleepEndMinute;
    sb += ",conditionComponents=";

    AutoPtr<ArrayOf<IInterface*> > objs;
    if (mConditionComponents != NULL) {
        Int32 length = mConditionComponents->GetLength();
        objs = ArrayOf<IInterface*>::Alloc(length);
        for (Int32 i = 0; i < length; i++) {
            objs->Set(i, IInterface::Probe((*mConditionComponents)[i]));
        }
    }

    sb += mConditionComponents == NULL ? "NULL" : TextUtils::Join(cs.Get(), objs.Get());
    sb += ",conditionIds=";

    if (mConditionIds != NULL) {
        Int32 length = mConditionIds->GetLength();
        objs = ArrayOf<IInterface*>::Alloc(length);
        for (Int32 i = 0; i < length; i++) {
            objs->Set(i, IInterface::Probe((*mConditionIds)[i]));
        }
    }

    sb += mConditionIds == NULL ? "NULL" : TextUtils::Join(cs.Get(), objs.Get());
    sb += ",exitCondition=";
    sb += TO_CSTR(mExitCondition);
    sb += ",exitConditionComponent=";
    sb += TO_CSTR(mExitConditionComponent);
    sb += ']';
    *result = sb.ToString();
    return NOERROR;
}

String ZenModeConfig::SourceToString(
    /* [in] */ Int32 source)
{
    switch (source) {
        case IZenModeConfig::SOURCE_ANYONE:
            return String("anyone");
        case IZenModeConfig::SOURCE_CONTACT:
            return String("contacts");
        case IZenModeConfig::SOURCE_STAR:
            return String("stars");
        default:
            return String("UNKNOWN");
    }
}

ECode ZenModeConfig::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IZenModeConfig>_other = IZenModeConfig::Probe(o);
    if (_other == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (_other.Get() == (IZenModeConfig*)this) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<ZenModeConfig> other = (ZenModeConfig*)_other.Get();
    AutoPtr<IString> mSleepMode1, mSleepMode2;
    CString::New(other->mSleepMode, (IString**)&mSleepMode1);
    CString::New(mSleepMode, (IString**)&mSleepMode2);

    AutoPtr<IArrayOf> mConditionComponents1, mConditionComponents2;
    if (other->mConditionComponents) {
        CArrayOf::New(EIID_IComponentName, other->mConditionComponents->GetLength(), (IArrayOf**)&mConditionComponents1);
        for(Int32 i = 0; i < other->mConditionComponents->GetLength(); i++) {
            mConditionComponents1->Set(i, IInterface::Probe((*(other->mConditionComponents))[i]));
        }
    }

    if (mConditionComponents) {
        CArrayOf::New(EIID_IComponentName, mConditionComponents->GetLength(), (IArrayOf**)&mConditionComponents2);
        for(Int32 i = 0; i < mConditionComponents->GetLength(); i++) {
            mConditionComponents2->Set(i, IInterface::Probe((*mConditionComponents)[i]));
        }
    }

    AutoPtr<IArrayOf> mConditionIds1, mConditionIds2;
    if (other->mConditionIds) {
        CArrayOf::New(EIID_IUri, other->mConditionIds->GetLength(), (IArrayOf**)&mConditionIds1);
        for(Int32 i = 0; i < other->mConditionIds->GetLength(); i++) {
            mConditionIds1->Set(i, IInterface::Probe((*(other->mConditionComponents))[i]));
        }
    }

    if (mConditionIds) {
        CArrayOf::New(EIID_IUri, mConditionIds->GetLength(), (IArrayOf**)&mConditionIds2);
        for(Int32 i = 0; i < mConditionIds->GetLength(); i++) {
            mConditionIds2->Set(i, IInterface::Probe((*mConditionIds)[i]));
        }
    }

    *result = other->mAllowCalls == mAllowCalls
            && other->mAllowMessages == mAllowMessages
            && other->mAllowFrom == mAllowFrom
            && other->mAllowEvents == mAllowEvents
            && Objects::Equals(mSleepMode1, mSleepMode2)
            && other->mSleepStartHour == mSleepStartHour
            && other->mSleepStartMinute == mSleepStartMinute
            && other->mSleepEndHour == mSleepEndHour
            && other->mSleepEndMinute == mSleepEndMinute
            && Objects::DeepEquals(mConditionComponents1, mConditionComponents2)
            && Objects::DeepEquals(mConditionIds1, mConditionIds2)
            && Objects::Equals(other->mExitCondition, mExitCondition)
            && Objects::Equals(other->mExitConditionComponent, mExitConditionComponent);
    return NOERROR;
}

ECode ZenModeConfig::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    AutoPtr<ArrayOf<Int32> > hash = ArrayOf<Int32>::Alloc(13);
    hash->Set(0, mAllowCalls);
    hash->Set(1, mAllowMessages);
    hash->Set(2, mAllowFrom);
    hash->Set(3, mAllowEvents);
    hash->Set(4, mSleepMode.GetHashCode());
    hash->Set(5, mSleepStartHour);
    hash->Set(6, mSleepStartMinute);
    hash->Set(7, mSleepEndHour);
    hash->Set(8, mSleepEndMinute);
    hash->Set(9, Arrays::GetHashCode(mConditionComponents));
    hash->Set(10, Arrays::GetHashCode(mConditionIds));
    hash->Set(11, Object::GetHashCode(mExitCondition));
    hash->Set(12, Object::GetHashCode(mExitConditionComponent));

    *hashCode = Arrays::GetHashCode(hash);
    return NOERROR;
}

ECode ZenModeConfig::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsValidHour(mSleepStartHour) && IsValidMinute(mSleepStartMinute)
            && IsValidHour(mSleepEndHour) && IsValidMinute(mSleepEndMinute)
            && IsValidSleepMode(mSleepMode);
    return NOERROR;
}

Boolean ZenModeConfig::IsValidSleepMode(
    /* [in] */ const String& sleepMode)
{
    AutoPtr<ArrayOf<Int32> > days;
    TryParseDays(sleepMode, (ArrayOf<Int32>**)&days);
    return sleepMode.IsNull() || sleepMode.Equals(IZenModeConfig::SLEEP_MODE_NIGHTS)
            || sleepMode.Equals(IZenModeConfig::SLEEP_MODE_WEEKNIGHTS) || days != NULL;
}

ECode ZenModeConfig::TryParseDays(
    /* [in] */ const String& sleepMode,
    /* [out, callee] */ ArrayOf<Int32>** results)
{
    VALIDATE_NOT_NULL(results)
    if (sleepMode.IsNull()) {
        *results = NULL;
        return NOERROR;
    }
    String sm = sleepMode.Trim();
    if (IZenModeConfig::SLEEP_MODE_NIGHTS.Equals(sm)) {
        *results = ALL_DAYS;
        REFCOUNT_ADD(*results)
        return NOERROR;
    }
    if (IZenModeConfig::SLEEP_MODE_WEEKNIGHTS.Equals(sm)) {
        *results = WEEKNIGHT_DAYS;
        REFCOUNT_ADD(*results)
        return NOERROR;
    }
    if (!sm.StartWith(IZenModeConfig::SLEEP_MODE_DAYS_PREFIX)) {
        *results = NULL;
        return NOERROR;
    }
    if (sm.Equals(IZenModeConfig::SLEEP_MODE_DAYS_PREFIX)) {
        *results = NULL;
        return NOERROR;
    }
    String substring = sm.Substring(IZenModeConfig::SLEEP_MODE_DAYS_PREFIX.GetLength());
    const AutoPtr<ArrayOf<String> > tokens;
    StringUtils::Split(substring, String(","), (ArrayOf<String>**)&tokens);
    if (tokens->GetLength() == 0) {
        *results = NULL;
        return NOERROR;
    }
    const AutoPtr<ArrayOf<Int32> > rt = ArrayOf<Int32>::Alloc(tokens->GetLength());
    for (Int32 i = 0; i < tokens->GetLength(); i++) {
        const Int32 day = TryParseInt32((*tokens)[i], -1);
        if (day == -1) {
            *results = NULL;
            return NOERROR;
        }
        (*rt)[i] = day;
    }
    *results = rt.Get();
    REFCOUNT_ADD(*results)
    return NOERROR;
}

Int32 ZenModeConfig::TryParseInt32(
    /* [in] */ const String& value,
    /* [in] */ Int32 defValue)
{
    if (TextUtils::IsEmpty(value)) return defValue;
    // try {
    return StringUtils::ParseInt32(value);
    // } catch (NumberFormatException e) {
    //     return defValue;
    // }
}

ECode ZenModeConfig::ReadXml(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IZenModeConfig** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type;
    parser->GetEventType(&type);
    if (type != IXmlPullParser::START_TAG) {
        *result = NULL;
        return NOERROR;
    }
    String tag;
    parser->GetName(&tag);
    if (!ZEN_TAG.Equals(tag)) {
        *result = NULL;
        return NOERROR;
    }
    const AutoPtr<ZenModeConfig> rt = new ZenModeConfig();
    /*const Int32 version = */SafeInt(parser, ZEN_ATT_VERSION, XML_VERSION);
    const AutoPtr<IArrayList> conditionComponents, conditionIds;
    CArrayList::New((IArrayList**)&conditionComponents);
    CArrayList::New((IArrayList**)&conditionIds);
    Int32 next;
    parser->Next(&next);
    while ((type = next) != IXmlPullParser::END_DOCUMENT) {
        parser->GetName(&tag);
        if (type == IXmlPullParser::END_TAG && ZEN_TAG.Equals(tag)) {
            Boolean isEmpty;
            conditionComponents->IsEmpty(&isEmpty);
            if (!isEmpty) {
                Int32 length;
                conditionComponents->GetSize(&length);
                AutoPtr<ArrayOf<IComponentName*> > cns = ArrayOf<IComponentName*>::Alloc(length);

                AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(cns->GetLength());
                for(Int32 i = 0; i < cns->GetLength(); i++) {
                    inArray->Set(i, IInterface::Probe((*cns)[i]));
                }

                AutoPtr<ArrayOf<IInterface*> > objs1;
                conditionComponents->ToArray(inArray.Get(), (ArrayOf<IInterface*>**)&objs1);

                AutoPtr<ArrayOf<IComponentName*> > rtcns = ArrayOf<IComponentName*>::Alloc(objs1->GetLength());
                for (Int32 i = 0; i < rtcns->GetLength(); i++) {
                    rtcns->Set(i, IComponentName::Probe((*objs1)[i]));
                }
                rt->mConditionComponents = rtcns;

                conditionIds->GetSize(&length);
                AutoPtr<ArrayOf<IUri*> > uris = ArrayOf<IUri*>::Alloc(length);

                AutoPtr<ArrayOf<IInterface*> > inArray2 = ArrayOf<IInterface*>::Alloc(uris->GetLength());
                for(Int32 i = 0; i < uris->GetLength(); i++) {
                    inArray2->Set(i, IInterface::Probe((*uris)[i]));
                }

                AutoPtr<ArrayOf<IInterface*> > objs2;
                conditionIds->ToArray(inArray2.Get(), (ArrayOf<IInterface*>**)&objs2);

                AutoPtr<ArrayOf<IUri*> > rturis = ArrayOf<IUri*>::Alloc(objs2->GetLength());
                for (Int32 i = 0; i < rturis->GetLength(); i++) {
                    rturis->Set(i, IUri::Probe((*objs2)[i]));
                }
                rt->mConditionIds = rturis;
            }
            *result = rt.Get();
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        if (type == IXmlPullParser::START_TAG) {
            if (ALLOW_TAG.Equals(tag)) {
                rt->mAllowCalls = SafeBoolean(parser, ALLOW_ATT_CALLS, FALSE);
                rt->mAllowMessages = SafeBoolean(parser, ALLOW_ATT_MESSAGES, FALSE);
                rt->mAllowEvents = SafeBoolean(parser, ALLOW_ATT_EVENTS, DEFAULT_ALLOW_EVENTS);
                rt->mAllowFrom = SafeInt(parser, ALLOW_ATT_FROM, IZenModeConfig::SOURCE_ANYONE);
                if (rt->mAllowFrom < IZenModeConfig::SOURCE_ANYONE || rt->mAllowFrom > IZenModeConfig::MAX_SOURCE) {
                    Logger::E(TAG, "bad source in config:%d", rt->mAllowFrom);
                    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
                }
            }
            else if (SLEEP_TAG.Equals(tag)) {
                String _mode;
                parser->GetAttributeValue(String(NULL), SLEEP_ATT_MODE, &_mode);
                const String mode = _mode;
                rt->mSleepMode = IsValidSleepMode(mode)? mode : String(NULL);
                const Int32 startHour = SafeInt(parser, SLEEP_ATT_START_HR, 0);
                const Int32 startMinute = SafeInt(parser, SLEEP_ATT_START_MIN, 0);
                const Int32 endHour = SafeInt(parser, SLEEP_ATT_END_HR, 0);
                const Int32 endMinute = SafeInt(parser, SLEEP_ATT_END_MIN, 0);
                rt->mSleepStartHour = IsValidHour(startHour) ? startHour : 0;
                rt->mSleepStartMinute = IsValidMinute(startMinute) ? startMinute : 0;
                rt->mSleepEndHour = IsValidHour(endHour) ? endHour : 0;
                rt->mSleepEndMinute = IsValidMinute(endMinute) ? endMinute : 0;
            }
            else if (CONDITION_TAG.Equals(tag)) {
                const AutoPtr<IComponentName> component =
                        SafeComponentName(parser, CONDITION_ATT_COMPONENT);
                const AutoPtr<IUri> conditionId = SafeUri(parser, CONDITION_ATT_ID);
                if (component != NULL && conditionId != NULL) {
                    conditionComponents->Add(component);
                    conditionIds->Add(conditionId);
                }
            }
            else if (EXIT_CONDITION_TAG.Equals(tag)) {
                ReadConditionXml(parser, (ICondition**)(&(rt->mExitCondition)));
                if (rt->mExitCondition != NULL) {
                    rt->mExitConditionComponent =
                        SafeComponentName(parser, EXIT_CONDITION_ATT_COMPONENT);
                }
            }
        }
    }
    Logger::E(TAG, "Failed to reach END_DOCUMENT");
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode ZenModeConfig::WriteXml(
    /* [in] */ IXmlSerializer* out)
{
    out->WriteStartTag(String(NULL), ZEN_TAG);
    out->WriteAttribute(String(NULL), ZEN_ATT_VERSION, StringUtils::ToString(XML_VERSION));
    out->WriteStartTag(String(NULL), ALLOW_TAG);
    out->WriteAttribute(String(NULL), ALLOW_ATT_CALLS, StringUtils::BooleanToString(mAllowCalls));
    out->WriteAttribute(String(NULL), ALLOW_ATT_MESSAGES, StringUtils::BooleanToString(mAllowMessages));
    out->WriteAttribute(String(NULL), ALLOW_ATT_EVENTS, StringUtils::BooleanToString(mAllowEvents));
    out->WriteAttribute(String(NULL), ALLOW_ATT_FROM, StringUtils::ToString(mAllowFrom));
    out->WriteEndTag(String(NULL), ALLOW_TAG);
    out->WriteStartTag(String(NULL), SLEEP_TAG);
    if (!mSleepMode.IsNull()) {
        out->WriteAttribute(String(NULL), SLEEP_ATT_MODE, mSleepMode);
    }
    out->WriteAttribute(String(NULL), SLEEP_ATT_START_HR, StringUtils::ToString(mSleepStartHour));
    out->WriteAttribute(String(NULL), SLEEP_ATT_START_MIN, StringUtils::ToString(mSleepStartMinute));
    out->WriteAttribute(String(NULL), SLEEP_ATT_END_HR, StringUtils::ToString(mSleepEndHour));
    out->WriteAttribute(String(NULL), SLEEP_ATT_END_MIN, StringUtils::ToString(mSleepEndMinute));
    out->WriteEndTag(String(NULL), SLEEP_TAG);

    if (mConditionComponents != NULL && mConditionIds != NULL
            && mConditionComponents->GetLength() == mConditionIds->GetLength()) {
        for (Int32 i = 0; i < mConditionComponents->GetLength(); i++) {
            out->WriteStartTag(String(NULL), CONDITION_TAG);
            String flattern;
            (*mConditionComponents)[i]->FlattenToString(&flattern);
            out->WriteAttribute(String(NULL), CONDITION_ATT_COMPONENT, flattern);
            String str;
            IObject::Probe((*mConditionIds)[i])->ToString(&str);
            out->WriteAttribute(String(NULL), CONDITION_ATT_ID, str);
            out->WriteEndTag(String(NULL), CONDITION_TAG);
        }
    }
    if (mExitCondition != NULL && mExitConditionComponent != NULL) {
        out->WriteStartTag(String(NULL), EXIT_CONDITION_TAG);
        String flattern;
        mExitConditionComponent->FlattenToString(&flattern);
        out->WriteAttribute(String(NULL), EXIT_CONDITION_ATT_COMPONENT, flattern);
        WriteConditionXml(mExitCondition, out);
        out->WriteEndTag(String(NULL), EXIT_CONDITION_TAG);
    }
    out->WriteEndTag(String(NULL), ZEN_TAG);
    return NOERROR;
}

ECode ZenModeConfig::ReadConditionXml(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ ICondition** result)
{
    VALIDATE_NOT_NULL(result)
    const AutoPtr<IUri> id = SafeUri(parser, CONDITION_ATT_ID);
    String _summary;
    parser->GetAttributeValue(String(NULL), CONDITION_ATT_SUMMARY, &_summary);
    String _line1;
    parser->GetAttributeValue(String(NULL), CONDITION_ATT_LINE1, &_line1);
    String _line2;
    parser->GetAttributeValue(String(NULL), CONDITION_ATT_LINE2, &_line2);
    const String summary = _summary;
    const String line1 = _line1;
    const String line2 = _line2;
    const Int32 icon = SafeInt(parser, CONDITION_ATT_ICON, -1);
    const Int32 state = SafeInt(parser, CONDITION_ATT_STATE, -1);
    const Int32 flags = SafeInt(parser, CONDITION_ATT_FLAGS, -1);
    CCondition::New(id, summary, line1, line2, icon, state, flags, result);
    if (*result == NULL) {
        Slogger::W(TAG, "Unable to read condition xml");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ZenModeConfig::WriteConditionXml(
    /* [in] */ ICondition* c,
    /* [in] */ IXmlSerializer* out)
{
    AutoPtr<Condition> _c = (Condition*)c;
    String str;
    IObject::Probe(_c->mId)->ToString(&str);
    out->WriteAttribute(String(NULL), CONDITION_ATT_ID, str);
    out->WriteAttribute(String(NULL), CONDITION_ATT_SUMMARY, _c->mSummary);
    out->WriteAttribute(String(NULL), CONDITION_ATT_LINE1, _c->mLine1);
    out->WriteAttribute(String(NULL), CONDITION_ATT_LINE2, _c->mLine2);
    out->WriteAttribute(String(NULL), CONDITION_ATT_ICON, StringUtils::ToString(_c->mIcon));
    out->WriteAttribute(String(NULL), CONDITION_ATT_STATE, StringUtils::ToString(_c->mState));
    out->WriteAttribute(String(NULL), CONDITION_ATT_FLAGS, StringUtils::ToString(_c->mFlags));
    return NOERROR;
}

Boolean ZenModeConfig::IsValidHour(
    /* [in] */ Int32 val)
{
    return val >= 0 && val < 24;
}

Boolean ZenModeConfig::IsValidMinute(
    /* [in] */ Int32 val)
{
    return val >= 0 && val < 60;
}

Boolean ZenModeConfig::SafeBoolean(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& att,
    /* [in] */ Boolean defValue)
{
    String _val;
    parser->GetAttributeValue(String(NULL), att, &_val);
    const String val = _val;
    if (TextUtils::IsEmpty(val)) return defValue;
    Int32 i;
    StringUtils::Parse(val, &i);
    return i != 0 ? TRUE : FALSE;
}

Int32 ZenModeConfig::SafeInt(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& att,
    /* [in] */ Int32 defValue)
{
    String _val;
    parser->GetAttributeValue(String(NULL), att, &_val);
    const String val = _val;
    return TryParseInt32(val, defValue);
}

AutoPtr<IComponentName> ZenModeConfig::SafeComponentName(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& att)
{
    String _val;
    parser->GetAttributeValue(String(NULL), att, &_val);
    const String val = _val;
    if (TextUtils::IsEmpty(val)) return NULL;
    AutoPtr<IComponentName> cn;
    CComponentName::UnflattenFromString(val, (IComponentName**)&cn);
    return cn;
}

AutoPtr<IUri> ZenModeConfig::SafeUri(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& att)
{
    String _val;
    parser->GetAttributeValue(String(NULL), att, &_val);
    const String val = _val;
    if (TextUtils::IsEmpty(val)) return NULL;
    AutoPtr<IUri> uri;
    Uri::Parse(val, (IUri**)&uri);
    return uri;
}

ECode ZenModeConfig::Copy(
    /* [out] */ IZenModeConfig** copy)
{
    VALIDATE_NOT_NULL(copy)
    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);
    WriteToParcel(parcel);
    parcel->SetDataPosition(0);

    AutoPtr<IZenModeConfig> config;
    CZenModeConfig::New(copy);
    return IParcelable::Probe(*copy)->ReadFromParcel(parcel);
}

ECode ZenModeConfig::ToDowntimeInfo(
    /* [out] */ IZenModeConfigDowntimeInfo** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<DowntimeInfo> downtime = new DowntimeInfo();
    downtime->mStartHour = mSleepStartHour;
    downtime->mStartMinute = mSleepStartMinute;
    downtime->mEndHour = mSleepEndHour;
    downtime->mEndMinute = mSleepEndMinute;
    *result = downtime.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ZenModeConfig::ToTimeCondition(
    /* [in] */ Int32 minutesFromNow,
    /* [out] */ ICondition** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);
    const Int64 millis = minutesFromNow == 0 ? ZERO_VALUE_MS : minutesFromNow * MINUTES_MS;
    return ToTimeCondition(now + millis, minutesFromNow, result);
}

ECode ZenModeConfig::ToTimeCondition(
    /* [in] */ Int64 time,
    /* [in] */ Int32 minutes,
    /* [out] */ ICondition** result)
{
    VALIDATE_NOT_NULL(result)
    const Int32 num = minutes < 60 ? minutes : Elastos::Core::Math::Round(minutes / 60.0F);
    const Int32 resId = minutes < 60
        ? R::plurals::zen_mode_duration_minutes
        : R::plurals::zen_mode_duration_hours;

    AutoPtr<IInteger32> iv;
    CInteger32::New(num, (IInteger32**)&iv);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(1);
    inArray->Set(0, IInterface::Probe(iv));
    String caption;
    CResources::GetSystem()->GetQuantityString(resId, num, inArray, &caption);
    AutoPtr<IUri> id;
    ToCountdownConditionId(time, (IUri**)&id);
    CCondition::New(id, caption, String(""), String(""), 0, ICondition::STATE_TRUE,
        ICondition::FLAG_RELEVANT_NOW, result);
    return NOERROR;
}

ECode ZenModeConfig::ToCountdownConditionId(
    /* [in] */ Int64 time,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUriBuilder> builder;
    CUriBuilder::New((IUriBuilder**)&builder);
    builder->Scheme(ICondition::SCHEME);
    builder->Authority(SYSTEM_AUTHORITY);
    builder->AppendPath(COUNTDOWN_PATH);
    builder->AppendPath(StringUtils::ToString(time));
    builder->Build(result);
    return NOERROR;
}

ECode ZenModeConfig::TryParseCountdownConditionId(
    /* [in] */ IUri* conditionId,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Boolean isValidId;
    Condition::IsValidId(conditionId, SYSTEM_AUTHORITY, &isValidId);
    if (!isValidId) return NOERROR;

    AutoPtr<IList> list;
    conditionId->GetPathSegments((IList**)&list);
    Int32 size;
    list->GetSize(&size);
    if (size != 2) return NOERROR;

    AutoPtr<IInterface> obj;
    list->Get(0, (IInterface**)&obj);
    if (!COUNTDOWN_PATH.Equals(Object::ToString(obj))) return NOERROR;

    obj = NULL;
    list->Get(1, (IInterface**)&obj);
    ECode ec = StringUtils::Parse(Object::ToString(obj), result);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Error parsing countdown condition: %s", TO_CSTR(conditionId));
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode ZenModeConfig::IsValidCountdownConditionId(
    /* [in] */ IUri* conditionId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 value;
    TryParseCountdownConditionId(conditionId, &value);
    *result = value != 0;
    return NOERROR;
}

ECode ZenModeConfig::ToDowntimeConditionId(
    /* [in] */ IZenModeConfigDowntimeInfo* downtime,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUriBuilder> builder;
    CUriBuilder::New((IUriBuilder**)&builder);
    builder->Scheme(ICondition::SCHEME);
    builder->Authority(SYSTEM_AUTHORITY);
    builder->AppendPath(DOWNTIME_PATH);
    AutoPtr<DowntimeInfo> _downtime = (DowntimeInfo*)downtime;
    StringBuilder sb(_downtime->mStartHour);
    sb += ".";
    sb += _downtime->mStartMinute;
    builder->AppendQueryParameter(String("start"), sb.ToString());
    StringBuilder sb2(_downtime->mEndHour);
    sb2 += ".";
    sb2 += _downtime->mEndMinute;
    builder->AppendQueryParameter(String("end"), sb2.ToString());
    builder->Build(result);
    return NOERROR;
}

ECode ZenModeConfig::TryParseDowntimeConditionId(
    /* [in] */ IUri* conditionId,
    /* [out] */ IZenModeConfigDowntimeInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Boolean isValidId;
    Condition::IsValidId(conditionId, SYSTEM_AUTHORITY, &isValidId);
    if (!isValidId) return NOERROR;

    Int32 size = 0;
    AutoPtr<IList> list;
    conditionId->GetPathSegments((IList**)&list);
    list->GetSize(&size);
    if (size != 1) return NOERROR;

    AutoPtr<IInterface> obj;
    list->Get(0, (IInterface**)&obj);
    if (!DOWNTIME_PATH.Equals(Object::ToString(obj))) return NOERROR;

    const AutoPtr<ArrayOf<Int32> > start;
    String qp;
    conditionId->GetQueryParameter(String("start"), &qp);
    TryParseHourAndMinute(qp, (ArrayOf<Int32>**)&start);
    const AutoPtr<ArrayOf<Int32> > end;
    String qp2;
    conditionId->GetQueryParameter(String("end"), &qp2);
    TryParseHourAndMinute(qp2, (ArrayOf<Int32>**)&end);
    if (start == NULL || end == NULL) {
        *result = NULL;
        return NOERROR;
    }
    const AutoPtr<DowntimeInfo> downtime = new DowntimeInfo();
    downtime->mStartHour = (*start)[0];
    downtime->mStartMinute = (*start)[1];
    downtime->mEndHour = (*end)[0];
    downtime->mEndMinute = (*end)[1];
    *result = downtime.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ZenModeConfig::TryParseHourAndMinute(
    /* [in] */ const String& value,
    /* [out, callee] */ ArrayOf<Int32>** results)
{
    VALIDATE_NOT_NULL(results)
    *results = NULL;

    if (TextUtils::IsEmpty(value)) {
        return NOERROR;
    }
    const Int32 i = value.IndexOf('.');
    if (i < 1 || i >= value.GetLength() - 1) {
        return NOERROR;
    }
    const Int32 hour = TryParseInt32(value.Substring(0, i), -1);
    const Int32 minute = TryParseInt32(value.Substring(i + 1), -1);
    if (IsValidHour(hour) && IsValidMinute(minute)) {
        *results = ArrayOf<Int32>::Alloc(2);
        (**results)[0] = hour;
        (**results)[1] = minute;
        REFCOUNT_ADD(*results)
    }

    return NOERROR;
}

Boolean ZenModeConfig::IsValidDowntimeConditionId(
    /* [in] */ IUri* conditionId)
{
    AutoPtr<IZenModeConfigDowntimeInfo> info;
    TryParseDowntimeConditionId(conditionId, (IZenModeConfigDowntimeInfo**)&info);
    return info != NULL;
}

ECode ZenModeConfig::GetAllowCalls(
    /* [out] */ Boolean* allowCalls)
{
    VALIDATE_NOT_NULL(allowCalls);
    *allowCalls = mAllowCalls;
    return NOERROR;
}

ECode ZenModeConfig::SetAllowCalls(
    /* [in] */ Boolean allowCalls)
{
    mAllowCalls = allowCalls;
    return NOERROR;
}

ECode ZenModeConfig::GetAllowMessages(
    /* [out] */ Boolean* allowMessages)
{
    VALIDATE_NOT_NULL(allowMessages);
    *allowMessages = mAllowMessages;
    return NOERROR;
}

ECode ZenModeConfig::SetAllowMessages(
    /* [in] */ Boolean allowMessages)
{
    mAllowMessages = allowMessages;
    return NOERROR;
}

ECode ZenModeConfig::GetAllowEvents(
    /* [out] */ Boolean* allowEvents)
{
    VALIDATE_NOT_NULL(allowEvents);
    *allowEvents = mAllowEvents;
    return NOERROR;
}

ECode ZenModeConfig::SetAllowEvents(
    /* [in] */ Boolean allowEvents)
{
    mAllowEvents = allowEvents;
    return NOERROR;
}

ECode ZenModeConfig::GetAllowFrom(
    /* [out] */ Int32* allowFrom)
{
    VALIDATE_NOT_NULL(allowFrom);
    *allowFrom = mAllowFrom;
    return NOERROR;
}

ECode ZenModeConfig::SetAllowFrom(
    /* [in] */ Int32 allowFrom)
{
    mAllowFrom = allowFrom;
    return NOERROR;
}

ECode ZenModeConfig::GetSleepMode(
    /* [out] */ String* sleepMode)
{
    VALIDATE_NOT_NULL(sleepMode)
    *sleepMode = mSleepMode;
    return NOERROR;
}

ECode ZenModeConfig::SetSleepMode(
    /* [in] */ const String& sleepMode)
{
    mSleepMode = sleepMode;
    return NOERROR;
}

ECode ZenModeConfig::GetSleepStartHour(
    /* [out] */ Int32* sleepStartHour)
{
    VALIDATE_NOT_NULL(sleepStartHour);
    *sleepStartHour = mSleepStartHour;
    return NOERROR;
}

ECode ZenModeConfig::SetSleepStartHour(
    /* [in] */ Int32 sleepStartHour)
{
    mSleepStartHour = sleepStartHour;
    return NOERROR;
}

ECode ZenModeConfig::GetSleepStartMinute(
    /* [out] */ Int32* sleepStartMinute)
{
    VALIDATE_NOT_NULL(sleepStartMinute);
    *sleepStartMinute = mSleepStartMinute;
    return NOERROR;
}

ECode ZenModeConfig::SetSleepStartMinute(
    /* [in] */ Int32 sleepStartMinute)
{
    mSleepStartMinute = sleepStartMinute;
    return NOERROR;
}

ECode ZenModeConfig::GetSleepEndHour(
    /* [out] */ Int32* sleepEndHour)
{
    VALIDATE_NOT_NULL(sleepEndHour);
    *sleepEndHour = mSleepEndHour;
    return NOERROR;
}

ECode ZenModeConfig::SetSleepEndHour(
    /* [in] */ Int32 sleepEndHour)
{
    mSleepEndHour = sleepEndHour;
    return NOERROR;
}

ECode ZenModeConfig::GetSleepEndMinute(
    /* [out] */ Int32* sleepEndMinute)
{
    VALIDATE_NOT_NULL(sleepEndMinute);
    *sleepEndMinute = mSleepEndMinute;
    return NOERROR;
}

ECode ZenModeConfig::SetSleepEndMinute(
    /* [in] */ Int32 sleepEndMinute)
{
    mSleepEndMinute = sleepEndMinute;
    return NOERROR;
}

ECode ZenModeConfig::GetConditionComponents(
    /* [out, callee] */ ArrayOf<IComponentName*>** conditionComponents)
{
    VALIDATE_NOT_NULL(conditionComponents)
    *conditionComponents = mConditionComponents;
    REFCOUNT_ADD(*conditionComponents);
    return NOERROR;
}

ECode ZenModeConfig::SetConditionComponents(
    /* [in] */ ArrayOf<IComponentName*>* conditionComponents)
{
    mConditionComponents = conditionComponents;
    return NOERROR;
}

ECode ZenModeConfig::GetConditionIds(
    /* [out, callee] */ ArrayOf<IUri*>** conditionIds)
{
    VALIDATE_NOT_NULL(conditionIds);
    *conditionIds = mConditionIds;
    REFCOUNT_ADD(*conditionIds);
    return NOERROR;
}

ECode ZenModeConfig::SetConditionIds(
    /* [in] */ ArrayOf<IUri*>* conditionIds)
{
    mConditionIds = conditionIds;
    return NOERROR;
}

ECode ZenModeConfig::GetExitCondition(
    /* [out] */ ICondition** exitCondition)
{
    VALIDATE_NOT_NULL(exitCondition);
    *exitCondition = mExitCondition;
    REFCOUNT_ADD(*exitCondition);
    return NOERROR;
}

ECode ZenModeConfig::SetExitCondition(
    /* [in] */ ICondition* exitCondition)
{
    mExitCondition = exitCondition;
    return NOERROR;
}

ECode ZenModeConfig::GetExitConditionComponent(
    /* [out] */ IComponentName** exitConditionComponent)
{
    VALIDATE_NOT_NULL(exitConditionComponent);
    *exitConditionComponent = mExitConditionComponent;
    REFCOUNT_ADD(*exitConditionComponent);
    return NOERROR;
}

ECode ZenModeConfig::SetExitConditionComponent(
    /* [in] */ IComponentName* exitConditionComponent)
{
    mExitConditionComponent = exitConditionComponent;
    return NOERROR;
}

ECode ZenModeConfig::GetALL_DAYS(
    /* [out, callee] */ ArrayOf<Int32>** days)
{
    VALIDATE_NOT_NULL(days)
    *days = ALL_DAYS.Get();
    REFCOUNT_ADD(*days)
    return NOERROR;
}

ECode ZenModeConfig::GetWEEKNIGHT_DAYS(
    /* [out, callee] */ ArrayOf<Int32>** days)
{
    VALIDATE_NOT_NULL(days)
    *days = WEEKNIGHT_DAYS.Get();
    REFCOUNT_ADD(*days)
    return NOERROR;
}

ECode ZenModeConfig::GetMINUTE_BUCKETS(
    /* [out, callee] */ ArrayOf<Int32>** buckets)
{
    VALIDATE_NOT_NULL(buckets)
    *buckets = MINUTE_BUCKETS.Get();
    REFCOUNT_ADD(*buckets)
    return NOERROR;
}

//==================================================
// ZenModeConfig::DowntimeInfo
//==================================================

CAR_INTERFACE_IMPL(ZenModeConfig::DowntimeInfo, Object, IZenModeConfigDowntimeInfo)

ZenModeConfig::DowntimeInfo::DowntimeInfo()
    : mStartHour(0)
    , mStartMinute(0)
    , mEndHour(0)
    , mEndMinute(0)
{
}

ECode ZenModeConfig::DowntimeInfo::constructor()
{
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = 0;
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    const AutoPtr<IZenModeConfigDowntimeInfo> _other = IZenModeConfigDowntimeInfo::Probe(o);
    if (_other == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<DowntimeInfo> other = (DowntimeInfo*)_other.Get();
    *result = mStartHour == other->mStartHour
            && mStartMinute == other->mStartMinute
            && mEndHour == other->mEndHour
            && mEndMinute == other->mEndMinute;
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::GetStartHour(
    /* [out] */ Int32* startHour)
{
    VALIDATE_NOT_NULL(startHour);
    *startHour = mStartHour;
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::SetStartHour(
    /* [in] */ Int32 startHour)
{
    mStartHour = startHour;
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::GetStartMinute(
    /* [out] */ Int32* startMinute)
{
    VALIDATE_NOT_NULL(startMinute);
    *startMinute = mStartMinute;
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::SetStartMinute(
    /* [in] */ Int32 startMinute)
{
    mStartMinute = startMinute;
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::GetEndHour(
    /* [out] */ Int32* endHour)
{
    VALIDATE_NOT_NULL(endHour);
    *endHour = mEndHour;
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::SetEndHour(
    /* [in] */ Int32 endHour)
{
    mEndHour = endHour;
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::GetEndMinute(
    /* [out] */ Int32* endMinute)
{
    VALIDATE_NOT_NULL(endMinute);
    *endMinute = mEndMinute;
    return NOERROR;
}

ECode ZenModeConfig::DowntimeInfo::SetEndMinute(
    /* [in] */ Int32 endMinute)
{
    mEndMinute = endMinute;
    return NOERROR;
}

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos
