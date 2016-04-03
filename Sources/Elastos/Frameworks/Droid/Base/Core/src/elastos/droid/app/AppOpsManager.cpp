
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/AppOpsManager.h"
#include "elastos/droid/app/CAppOpsManagerAppOpsCallback.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Utility::CArrayMap;

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {


//========================================================================
// AppOpsManager::PackageOps
//========================================================================

CAR_INTERFACE_IMPL_2(AppOpsManager::PackageOps, Object, IAppOpsManagerPackageOps, IParcelable)

AppOpsManager::PackageOps::PackageOps()
    : mUid(0)
{}

AppOpsManager::PackageOps::~PackageOps()
{}

ECode AppOpsManager::PackageOps::constructor()
{
    return NOERROR;
}

ECode AppOpsManager::PackageOps::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ IList* entries)
{
    mPackageName = packageName;
    mUid = uid;
    mEntries = entries;
    return NOERROR;
}

ECode AppOpsManager::PackageOps::GetPackageName(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mPackageName;
    return NOERROR;
}

ECode AppOpsManager::PackageOps::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = mUid;
    return NOERROR;
}

ECode AppOpsManager::PackageOps::GetOps(
    /* [out] */ IList** ops)
{
    VALIDATE_NOT_NULL(ops)
    *ops = mEntries;
    REFCOUNT_ADD(*ops)
    return NOERROR;
}

ECode AppOpsManager::PackageOps::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mPackageName);
    dest->WriteInt32(mUid);
    Int32 size;
    mEntries->GetSize(&size);
    dest->WriteInt32(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mEntries->Get(i, (IInterface**)&obj);
        IParcelable::Probe(obj)->WriteToParcel(dest);
    }
    return NOERROR;
}

ECode AppOpsManager::PackageOps::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mPackageName);
    source->ReadInt32(&mUid);
    CArrayList::New((IArrayList**)&mEntries);
    Int32 N;
    source->ReadInt32(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<OpEntry> oe = new OpEntry();
        oe->ReadFromParcel(source);
        mEntries->Add(TO_IINTERFACE(oe));
    }
    return NOERROR;
}

ECode AppOpsManager::PackageOps::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("AppOpsManager::PackageOps");
    return NOERROR;
}

//========================================================================
// AppOpsManager::OpEntry
//========================================================================

CAR_INTERFACE_IMPL_2(AppOpsManager::OpEntry, Object, IAppOpsManagerOpEntry, IParcelable)

AppOpsManager::OpEntry::OpEntry()
    : mOp(0)
    , mMode(0)
    , mTime(0)
    , mRejectTime(0)
    , mDuration(0)
    , mAllowedCount(0)
    , mIgnoredCount(0)
{
}

AppOpsManager::OpEntry::~OpEntry()
{}

ECode AppOpsManager::OpEntry::constructor()
{
    return NOERROR;
}

ECode AppOpsManager::OpEntry::constructor(
    /* [in] */ Int32 op,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 time,
    /* [in] */ Int32 rejectTime,
    /* [in] */ Int32 duration,
    /* [in] */ Int32 allowedCount,
    /* [in] */ Int32 ignoredCount)
{
    mOp = op;
    mMode = mode;
    mTime = time;
    mRejectTime = rejectTime;
    mDuration = duration;
    mAllowedCount = allowedCount;
    mIgnoredCount = ignoredCount;
    return NOERROR;
}

ECode AppOpsManager::OpEntry::GetOp(
    /* [out] */ Int32* op)
{
    VALIDATE_NOT_NULL(op)
    *op = mOp;
    return NOERROR;
}

ECode AppOpsManager::OpEntry::GetMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = mMode;
    return NOERROR;
}

ECode AppOpsManager::OpEntry::GetTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mTime;
    return NOERROR;
}

ECode AppOpsManager::OpEntry::GetRejectTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mRejectTime;
    return NOERROR;
}

ECode AppOpsManager::OpEntry::IsRunning(
    /* [out] */ Boolean* running)
{
    VALIDATE_NOT_NULL(running)
    *running = mDuration == -1;
    return NOERROR;
}

ECode AppOpsManager::OpEntry::GetDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration)
    *duration = mDuration;
    if (mDuration == -1) {
        Int64 currentTimeMillis;
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&currentTimeMillis);
        *duration = (Int32)(currentTimeMillis - mTime);
    }
    return NOERROR;
}

ECode AppOpsManager::OpEntry::GetAllowedCount(
    /* [out] */ Int32* allowedCount)
{
    VALIDATE_NOT_NULL(allowedCount)
    *allowedCount = mAllowedCount;
    return NOERROR;
}

ECode AppOpsManager::OpEntry::GetIgnoredCount(
    /* [out] */ Int32* ignoredCount)
{
    VALIDATE_NOT_NULL(ignoredCount)
    *ignoredCount = mIgnoredCount;
    return NOERROR;
}

ECode AppOpsManager::OpEntry::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mOp);
    dest->WriteInt32(mMode);
    dest->WriteInt64(mTime);
    dest->WriteInt64(mRejectTime);
    dest->WriteInt32(mDuration);
    dest->WriteInt32(mAllowedCount);
    dest->WriteInt32(mIgnoredCount);
    return NOERROR;
}

ECode AppOpsManager::OpEntry::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mOp);
    source->ReadInt32(&mMode);
    source->ReadInt64(&mTime);
    source->ReadInt64(&mRejectTime);
    source->ReadInt32(&mDuration);
    source->ReadInt32(&mAllowedCount);
    source->ReadInt32(&mIgnoredCount);
    return NOERROR;
}

ECode AppOpsManager::OpEntry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("AppOpsManager::OpEntry");
    return NOERROR;
}

//========================================================================
// AppOpsManager::OnOpChangedInternalListener
//========================================================================
CAR_INTERFACE_IMPL_2(AppOpsManager::OnOpChangedInternalListener, Object, IAppOpsManagerOnOpChangedInternalListener, IAppOpsManagerOnOpChangedListener)

ECode AppOpsManager::OnOpChangedInternalListener::OnOpChanged(
    /* [in] */ const String& op,
    /* [in] */ const String& packageName)
{
    return NOERROR;
}

ECode AppOpsManager::OnOpChangedInternalListener::OnOpChanged(
    /* [in] */ Int32 op,
    /* [in] */ const String& packageName)
{
    return NOERROR;
}

ECode AppOpsManager::OnOpChangedInternalListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("AppOpsManager::OnOpChangedInternalListener");
    return NOERROR;
}

//========================================================================
// AppOpsManager
//========================================================================
const String AppOpsManager::TAG("AppOpsManager");
Object AppOpsManager::sClassLock;
AutoPtr<IBinder> AppOpsManager::sToken;
const Int32 AppOpsManager::sOpLength = 62;

String AppOpsManager::OPSTR_GPS("android:gps");
String AppOpsManager::OPSTR_VIBRATE("android:vibrate");
String AppOpsManager::OPSTR_READ_CONTACTS("android:read_contacts");
String AppOpsManager::OPSTR_WRITE_CONTACTS("android:write_contacts");
String AppOpsManager::OPSTR_READ_CALL_LOG("android:read_call_log");
String AppOpsManager::OPSTR_WRITE_CALL_LOG("android:write_call_log");
String AppOpsManager::OPSTR_READ_CALENDAR("android:read_calendar");
String AppOpsManager::OPSTR_WRITE_CALENDAR("android:write_calendar");
String AppOpsManager::OPSTR_WIFI_SCAN("android:wifi_scan");
String AppOpsManager::OPSTR_POST_NOTIFICATION("android:post_notification");
String AppOpsManager::OPSTR_NEIGHBORING_CELLS("android:neighboring_cells");
String AppOpsManager::OPSTR_CALL_PHONE("android:call_phone");
String AppOpsManager::OPSTR_READ_SMS("android:read_sms");
String AppOpsManager::OPSTR_WRITE_SMS("android:write_sms");
String AppOpsManager::OPSTR_RECEIVE_SMS("android:receive_sms");
String AppOpsManager::OPSTR_RECEIVE_EMERGECY_SMS("android:receive_emergecy_sms");
String AppOpsManager::OPSTR_RECEIVE_MMS("android:receive_mms");
String AppOpsManager::OPSTR_RECEIVE_WAP_PUSH("android:receive_wap_push");
String AppOpsManager::OPSTR_SEND_SMS("android:send_sms");
String AppOpsManager::OPSTR_READ_ICC_SMS("android:read_icc_sms");
String AppOpsManager::OPSTR_WRITE_ICC_SMS("android:write_icc_sms");
String AppOpsManager::OPSTR_WRITE_SETTINGS("android:write_settings");
String AppOpsManager::OPSTR_SYSTEM_ALERT_WINDOW("android:system_alert_window");
String AppOpsManager::OPSTR_ACCESS_NOTIFICATIONS("android:access_notifications");
String AppOpsManager::OPSTR_CAMERA("android:camera");
String AppOpsManager::OPSTR_RECORD_AUDIO("android:record_audio");
String AppOpsManager::OPSTR_PLAY_AUDIO("android:play_audio");
String AppOpsManager::OPSTR_READ_CLIPBOARD("android:read_clipboard");
String AppOpsManager::OPSTR_WRITE_CLIPBOARD("android:write_clipboard");
String AppOpsManager::OPSTR_TAKE_MEDIA_BUTTONS("android:take_media_buttons");
String AppOpsManager::OPSTR_TAKE_AUDIO_FOCUS("android:take_audio_focus");
String AppOpsManager::OPSTR_AUDIO_MASTER_VOLUME("android:audio_master_volume");
String AppOpsManager::OPSTR_AUDIO_VOICE_VOLUME("android:audio_voice_volume");
String AppOpsManager::OPSTR_AUDIO_RING_VOLUME("android:audio_ring_volume");
String AppOpsManager::OPSTR_AUDIO_MEDIA_VOLUME("android:audio_media_volume");
String AppOpsManager::OPSTR_AUDIO_ALARM_VOLUME("android:audio_alarm_volume");
String AppOpsManager::OPSTR_AUDIO_NOTIFICATION_VOLUME("android:audio_notification_volume");
String AppOpsManager::OPSTR_AUDIO_BLUETOOTH_VOLUME("android:audio_bluetooth_volume");
String AppOpsManager::OPSTR_WAKE_LOCK("android:wake_lock");
String AppOpsManager::OPSTR_MUTE_MICROPHONE("android:mute_microphone");
String AppOpsManager::OPSTR_TOAST_WINDOW("android:toast_window");
String AppOpsManager::OPSTR_PROJECT_MEDIA("android:project_media");
String AppOpsManager::OPSTR_WIFI_CHANGE("android:wifi_change");
String AppOpsManager::OPSTR_BLUETOOTH_CHANGE("android:bluetooth_change");
String AppOpsManager::OPSTR_SEND_MMS("android:send_mms");
String AppOpsManager::OPSTR_READ_MMS("android:read_mms");
String AppOpsManager::OPSTR_WRITE_MMS("android:write_mms");
String AppOpsManager::OPSTR_BOOT_COMPLETED("android:boot_completed");
String AppOpsManager::OPSTR_NFC_CHANGE("android:nfc_change");
String AppOpsManager::OPSTR_DELETE_SMS("android:delete_sms");
String AppOpsManager::OPSTR_DELETE_MMS("android:delete_mms");
String AppOpsManager::OPSTR_DELETE_CONTACTS("android:delete_contacts");
String AppOpsManager::OPSTR_DELETE_CALL_LOG("android:delete_call_log");
String AppOpsManager::OPSTR_DATA_CONNECT_CHANGE("android:data_connect_change");
String AppOpsManager::OPSTR_ALARM_WAKEUP("android:alarm_wakeup");
String AppOpsManager::OPSTR_SU("android:su");

Int32 AppOpsManager::sOpToSwitch[62] = {
        IAppOpsManager::OP_COARSE_LOCATION,
        IAppOpsManager::OP_COARSE_LOCATION,
        IAppOpsManager::OP_COARSE_LOCATION,
        IAppOpsManager::OP_VIBRATE,
        IAppOpsManager::OP_READ_CONTACTS,
        IAppOpsManager::OP_WRITE_CONTACTS,
        IAppOpsManager::OP_READ_CALL_LOG,
        IAppOpsManager::OP_WRITE_CALL_LOG,
        IAppOpsManager::OP_READ_CALENDAR,
        IAppOpsManager::OP_WRITE_CALENDAR,
        IAppOpsManager::OP_COARSE_LOCATION,
        IAppOpsManager::OP_POST_NOTIFICATION,
        IAppOpsManager::OP_COARSE_LOCATION,
        IAppOpsManager::OP_CALL_PHONE,
        IAppOpsManager::OP_READ_SMS,
        IAppOpsManager::OP_WRITE_SMS,
        IAppOpsManager::OP_RECEIVE_SMS,
        IAppOpsManager::OP_RECEIVE_SMS,
        IAppOpsManager::OP_RECEIVE_SMS,
        IAppOpsManager::OP_RECEIVE_SMS,
        IAppOpsManager::OP_SEND_SMS,
        IAppOpsManager::OP_READ_SMS,
        IAppOpsManager::OP_WRITE_SMS,
        IAppOpsManager::OP_WRITE_SETTINGS,
        IAppOpsManager::OP_SYSTEM_ALERT_WINDOW,
        IAppOpsManager::OP_ACCESS_NOTIFICATIONS,
        IAppOpsManager::OP_CAMERA,
        IAppOpsManager::OP_RECORD_AUDIO,
        IAppOpsManager::OP_PLAY_AUDIO,
        IAppOpsManager::OP_READ_CLIPBOARD,
        IAppOpsManager::OP_WRITE_CLIPBOARD,
        IAppOpsManager::OP_TAKE_MEDIA_BUTTONS,
        IAppOpsManager::OP_TAKE_AUDIO_FOCUS,
        IAppOpsManager::OP_AUDIO_MASTER_VOLUME,
        IAppOpsManager::OP_AUDIO_VOICE_VOLUME,
        IAppOpsManager::OP_AUDIO_RING_VOLUME,
        IAppOpsManager::OP_AUDIO_MEDIA_VOLUME,
        IAppOpsManager::OP_AUDIO_ALARM_VOLUME,
        IAppOpsManager::OP_AUDIO_NOTIFICATION_VOLUME,
        IAppOpsManager::OP_AUDIO_BLUETOOTH_VOLUME,
        IAppOpsManager::OP_WAKE_LOCK,
        IAppOpsManager::OP_COARSE_LOCATION,
        IAppOpsManager::OP_COARSE_LOCATION,
        IAppOpsManager::OP_GET_USAGE_STATS,
        IAppOpsManager::OP_MUTE_MICROPHONE,
        IAppOpsManager::OP_TOAST_WINDOW,
        IAppOpsManager::OP_PROJECT_MEDIA,
        IAppOpsManager::OP_ACTIVATE_VPN,
        IAppOpsManager::OP_WIFI_CHANGE,
        IAppOpsManager::OP_BLUETOOTH_CHANGE,
        IAppOpsManager::OP_SEND_MMS,
        IAppOpsManager::OP_READ_MMS,
        IAppOpsManager::OP_WRITE_MMS,
        IAppOpsManager::OP_BOOT_COMPLETED,
        IAppOpsManager::OP_NFC_CHANGE,
        IAppOpsManager::OP_DELETE_SMS,
        IAppOpsManager::OP_DELETE_MMS,
        IAppOpsManager::OP_DELETE_CONTACTS,
        IAppOpsManager::OP_DELETE_CALL_LOG,
        IAppOpsManager::OP_DATA_CONNECT_CHANGE,
        IAppOpsManager::OP_ALARM_WAKEUP,
        IAppOpsManager::OP_SU,
};

String AppOpsManager::sOpToString[62] = {
        IAppOpsManager::OPSTR_COARSE_LOCATION,
        IAppOpsManager::OPSTR_FINE_LOCATION,
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        IAppOpsManager::OPSTR_MONITOR_LOCATION,
        IAppOpsManager::OPSTR_MONITOR_HIGH_POWER_LOCATION,
        IAppOpsManager::OPSTR_GET_USAGE_STATS,
        String(NULL),
        String(NULL),
        String(NULL),
        IAppOpsManager::OPSTR_ACTIVATE_VPN,
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        String(NULL),
        OPSTR_SU,
};

String AppOpsManager::sOpToOpString[62] = {
        IAppOpsManager::OPSTR_COARSE_LOCATION,
        IAppOpsManager::OPSTR_FINE_LOCATION,
        OPSTR_GPS,
        OPSTR_VIBRATE,
        OPSTR_READ_CONTACTS,
        OPSTR_WRITE_CONTACTS,
        OPSTR_READ_CALL_LOG,
        OPSTR_WRITE_CALL_LOG,
        OPSTR_READ_CALENDAR,
        OPSTR_WRITE_CALENDAR,
        OPSTR_WIFI_SCAN,
        OPSTR_POST_NOTIFICATION,
        OPSTR_NEIGHBORING_CELLS,
        OPSTR_CALL_PHONE,
        OPSTR_READ_SMS,
        OPSTR_WRITE_SMS,
        OPSTR_RECEIVE_SMS,
        OPSTR_RECEIVE_EMERGECY_SMS,
        OPSTR_RECEIVE_MMS,
        OPSTR_RECEIVE_WAP_PUSH,
        OPSTR_SEND_SMS,
        OPSTR_READ_ICC_SMS,
        OPSTR_WRITE_ICC_SMS,
        OPSTR_WRITE_SETTINGS,
        OPSTR_SYSTEM_ALERT_WINDOW,
        OPSTR_ACCESS_NOTIFICATIONS,
        OPSTR_CAMERA,
        OPSTR_RECORD_AUDIO,
        OPSTR_PLAY_AUDIO,
        OPSTR_READ_CLIPBOARD,
        OPSTR_WRITE_CLIPBOARD,
        OPSTR_TAKE_MEDIA_BUTTONS,
        OPSTR_TAKE_AUDIO_FOCUS,
        OPSTR_AUDIO_MASTER_VOLUME,
        OPSTR_AUDIO_VOICE_VOLUME,
        OPSTR_AUDIO_RING_VOLUME,
        OPSTR_AUDIO_MEDIA_VOLUME,
        OPSTR_AUDIO_ALARM_VOLUME,
        OPSTR_AUDIO_NOTIFICATION_VOLUME,
        OPSTR_AUDIO_BLUETOOTH_VOLUME,
        OPSTR_WAKE_LOCK,
        OPSTR_MONITOR_LOCATION,
        OPSTR_MONITOR_HIGH_POWER_LOCATION,
        OPSTR_GET_USAGE_STATS,
        OPSTR_MUTE_MICROPHONE,
        OPSTR_TOAST_WINDOW,
        OPSTR_PROJECT_MEDIA,
        OPSTR_ACTIVATE_VPN,
        OPSTR_WIFI_CHANGE,
        OPSTR_BLUETOOTH_CHANGE,
        OPSTR_SEND_MMS,
        OPSTR_READ_MMS,
        OPSTR_WRITE_MMS,
        OPSTR_BOOT_COMPLETED,
        OPSTR_NFC_CHANGE,
        OPSTR_DELETE_SMS,
        OPSTR_DELETE_MMS,
        OPSTR_DELETE_CONTACTS,
        OPSTR_DELETE_CALL_LOG,
        OPSTR_DATA_CONNECT_CHANGE,
        OPSTR_ALARM_WAKEUP,
        OPSTR_SU,
};

String AppOpsManager::sOpNames[62] = {
        String("COARSE_LOCATION"),
        String("FINE_LOCATION"),
        String("GPS"),
        String("VIBRATE"),
        String("READ_CONTACTS"),
        String("WRITE_CONTACTS"),
        String("READ_CALL_LOG"),
        String("WRITE_CALL_LOG"),
        String("READ_CALENDAR"),
        String("WRITE_CALENDAR"),
        String("WIFI_SCAN"),
        String("POST_NOTIFICATION"),
        String("NEIGHBORING_CELLS"),
        String("CALL_PHONE"),
        String("READ_SMS"),
        String("WRITE_SMS"),
        String("RECEIVE_SMS"),
        String("RECEIVE_EMERGECY_SMS"),
        String("RECEIVE_MMS"),
        String("RECEIVE_WAP_PUSH"),
        String("SEND_SMS"),
        String("READ_ICC_SMS"),
        String("WRITE_ICC_SMS"),
        String("WRITE_SETTINGS"),
        String("SYSTEM_ALERT_WINDOW"),
        String("ACCESS_NOTIFICATIONS"),
        String("CAMERA"),
        String("RECORD_AUDIO"),
        String("PLAY_AUDIO"),
        String("READ_CLIPBOARD"),
        String("WRITE_CLIPBOARD"),
        String("TAKE_MEDIA_BUTTONS"),
        String("TAKE_AUDIO_FOCUS"),
        String("AUDIO_MASTER_VOLUME"),
        String("AUDIO_VOICE_VOLUME"),
        String("AUDIO_RING_VOLUME"),
        String("AUDIO_MEDIA_VOLUME"),
        String("AUDIO_ALARM_VOLUME"),
        String("AUDIO_NOTIFICATION_VOLUME"),
        String("AUDIO_BLUETOOTH_VOLUME"),
        String("WAKE_LOCK"),
        String("MONITOR_LOCATION"),
        String("MONITOR_HIGH_POWER_LOCATION"),
        String("GET_USAGE_STATS"),
        String("MUTE_MICROPHONE"),
        String("TOAST_WINDOW"),
        String("PROJECT_MEDIA"),
        String("ACTIVATE_VPN"),
        String("WIFI_CHANGE"),
        String("BLUETOOTH_CHANGE"),
        String("SEND_MMS"),
        String("READ_MMS"),
        String("WRITE_MMS"),
        String("BOOT_COMPLETED"),
        String("NFC_CHANGE"),
        String("DELETE_SMS"),
        String("DELETE_MMS"),
        String("DELETE_CONTACTS"),
        String("DELETE_CALL_LOG"),
        String("DATA_CONNECT_CHANGE"),
        String("ALARM_WAKEUP"),
        String("SU"),
};

String AppOpsManager::sOpPerms[62] =  {
        Manifest::permission::ACCESS_COARSE_LOCATION,
        Manifest::permission::ACCESS_FINE_LOCATION,
        String(NULL),
        Manifest::permission::VIBRATE,
        Manifest::permission::READ_CONTACTS,
        Manifest::permission::WRITE_CONTACTS,
        Manifest::permission::READ_CALL_LOG,
        Manifest::permission::WRITE_CALL_LOG,
        Manifest::permission::READ_CALENDAR,
        Manifest::permission::WRITE_CALENDAR,
        Manifest::permission::ACCESS_WIFI_STATE,
        String(NULL), // no permission required for notifications
        String(NULL), // neighboring cells shares the coarse location perm
        Manifest::permission::CALL_PHONE,
        Manifest::permission::READ_SMS,
        Manifest::permission::WRITE_SMS,
        Manifest::permission::RECEIVE_SMS,
        Manifest::permission::RECEIVE_EMERGENCY_BROADCAST,
        Manifest::permission::RECEIVE_MMS,
        Manifest::permission::RECEIVE_WAP_PUSH,
        Manifest::permission::SEND_SMS,
        Manifest::permission::READ_SMS,
        Manifest::permission::WRITE_SMS,
        Manifest::permission::WRITE_SETTINGS,
        Manifest::permission::SYSTEM_ALERT_WINDOW,
        Manifest::permission::ACCESS_NOTIFICATIONS,
        Manifest::permission::CAMERA,
        Manifest::permission::RECORD_AUDIO,
        String(NULL), // no permission for playing audio
        String(NULL), // no permission for reading clipboard
        String(NULL), // no permission for writing clipboard
        String(NULL), // no permission for taking media buttons
        String(NULL), // no permission for taking audio focus
        String(NULL), // no permission for changing master volume
        String(NULL), // no permission for changing voice volume
        String(NULL), // no permission for changing ring volume
        String(NULL), // no permission for changing media volume
        String(NULL), // no permission for changing alarm volume
        String(NULL), // no permission for changing notification volume
        String(NULL), // no permission for changing bluetooth volume
        Manifest::permission::WAKE_LOCK,
        String(NULL), // no permission for generic location monitoring
        String(NULL), // no permission for high power location monitoring
        Manifest::permission::PACKAGE_USAGE_STATS,
        String(NULL), // no permission for muting/unmuting microphone
        String(NULL), // no permission for displaying toasts
        String(NULL), // no permission for projecting media
        String(NULL), // no permission for activating vpn
        Manifest::permission::CHANGE_WIFI_STATE,
        Manifest::permission::BLUETOOTH,
        Manifest::permission::SEND_SMS,
        Manifest::permission::READ_SMS,
        Manifest::permission::WRITE_SMS,
        Manifest::permission::RECEIVE_BOOT_COMPLETED,
        Manifest::permission::NFC,
        Manifest::permission::WRITE_SMS,
        Manifest::permission::WRITE_SMS,
        Manifest::permission::WRITE_CONTACTS,
        Manifest::permission::WRITE_CALL_LOG,
        Manifest::permission::MODIFY_PHONE_STATE,
        String(NULL), // OP_ALARM_WAKEUP
        String(NULL),
};

String AppOpsManager::sOpRestrictions[62] = {
        IUserManager::DISALLOW_SHARE_LOCATION, //COARSE_LOCATION
        IUserManager::DISALLOW_SHARE_LOCATION, //FINE_LOCATION
        IUserManager::DISALLOW_SHARE_LOCATION, //GPS
        String(NULL), //VIBRATE
        String(NULL), //READ_CONTACTS
        String(NULL), //WRITE_CONTACTS
        IUserManager::DISALLOW_OUTGOING_CALLS, //READ_CALL_LOG
        IUserManager::DISALLOW_OUTGOING_CALLS, //WRITE_CALL_LOG
        String(NULL), //READ_CALENDAR
        String(NULL), //WRITE_CALENDAR
        IUserManager::DISALLOW_SHARE_LOCATION, //WIFI_SCAN
        String(NULL), //POST_NOTIFICATION
        String(NULL), //NEIGHBORING_CELLS
        String(NULL), //CALL_PHONE
        IUserManager::DISALLOW_SMS, //READ_SMS
        IUserManager::DISALLOW_SMS, //WRITE_SMS
        IUserManager::DISALLOW_SMS, //RECEIVE_SMS
        String(NULL), //RECEIVE_EMERGENCY_SMS
        IUserManager::DISALLOW_SMS, //RECEIVE_MMS
        String(NULL), //RECEIVE_WAP_PUSH
        IUserManager::DISALLOW_SMS, //SEND_SMS
        IUserManager::DISALLOW_SMS, //READ_ICC_SMS
        IUserManager::DISALLOW_SMS, //WRITE_ICC_SMS
        String(NULL), //WRITE_SETTINGS
        IUserManager::DISALLOW_CREATE_WINDOWS, //SYSTEM_ALERT_WINDOW
        String(NULL), //ACCESS_NOTIFICATIONS
        String(NULL), //CAMERA
        String(NULL), //RECORD_AUDIO
        String(NULL), //PLAY_AUDIO
        String(NULL), //READ_CLIPBOARD
        String(NULL), //WRITE_CLIPBOARD
        String(NULL), //TAKE_MEDIA_BUTTONS
        String(NULL), //TAKE_AUDIO_FOCUS
        IUserManager::DISALLOW_ADJUST_VOLUME, //AUDIO_MASTER_VOLUME
        IUserManager::DISALLOW_ADJUST_VOLUME, //AUDIO_VOICE_VOLUME
        IUserManager::DISALLOW_ADJUST_VOLUME, //AUDIO_RING_VOLUME
        IUserManager::DISALLOW_ADJUST_VOLUME, //AUDIO_MEDIA_VOLUME
        IUserManager::DISALLOW_ADJUST_VOLUME, //AUDIO_ALARM_VOLUME
        IUserManager::DISALLOW_ADJUST_VOLUME, //AUDIO_NOTIFICATION_VOLUME
        IUserManager::DISALLOW_ADJUST_VOLUME, //AUDIO_BLUETOOTH_VOLUME
        String(NULL), //WAKE_LOCK
        IUserManager::DISALLOW_SHARE_LOCATION, //MONITOR_LOCATION
        IUserManager::DISALLOW_SHARE_LOCATION, //MONITOR_HIGH_POWER_LOCATION
        String(NULL), //GET_USAGE_STATS
        IUserManager::DISALLOW_UNMUTE_MICROPHONE, // MUTE_MICROPHONE
        IUserManager::DISALLOW_CREATE_WINDOWS, // TOAST_WINDOW
        String(NULL), //PROJECT_MEDIA
        IUserManager::DISALLOW_CONFIG_VPN, // ACTIVATE_VPN
        String(NULL), //WIFI_CHANGE
        String(NULL), //BLUETOOTH_CHANGE
        String(NULL), //SEND_MMS
        String(NULL), //READ_MMS
        String(NULL), //WRITE_MMS
        String(NULL), //BOOT_COMPLETED
        String(NULL), //NFC_CHANGE
        String(NULL), //DELETE_SMS
        String(NULL), //DELETE_MMS
        String(NULL), //DELETE_CONTACTS
        String(NULL), //DELETE_CALL_LOG
        String(NULL), //DATA_CONNECT_CHANGE
        String(NULL), //ALARM_WAKEUP
        IUserManager::DISALLOW_SU, //SU TODO: this should really be investigated.
};

Boolean AppOpsManager::sOpAllowSystemRestrictionBypass[62] = {
        FALSE, //COARSE_LOCATION
        FALSE, //FINE_LOCATION
        FALSE, //GPS
        FALSE, //VIBRATE
        FALSE, //READ_CONTACTS
        FALSE, //WRITE_CONTACTS
        FALSE, //READ_CALL_LOG
        FALSE, //WRITE_CALL_LOG
        FALSE, //READ_CALENDAR
        FALSE, //WRITE_CALENDAR
        TRUE, //WIFI_SCAN
        FALSE, //POST_NOTIFICATION
        FALSE, //NEIGHBORING_CELLS
        FALSE, //CALL_PHONE
        FALSE, //READ_SMS
        FALSE, //WRITE_SMS
        FALSE, //RECEIVE_SMS
        FALSE, //RECEIVE_EMERGECY_SMS
        FALSE, //RECEIVE_MMS
        FALSE, //RECEIVE_WAP_PUSH
        FALSE, //SEND_SMS
        FALSE, //READ_ICC_SMS
        FALSE, //WRITE_ICC_SMS
        FALSE, //WRITE_SETTINGS
        TRUE, //SYSTEM_ALERT_WINDOW
        FALSE, //ACCESS_NOTIFICATIONS
        FALSE, //CAMERA
        FALSE, //RECORD_AUDIO
        FALSE, //PLAY_AUDIO
        FALSE, //READ_CLIPBOARD
        FALSE, //WRITE_CLIPBOARD
        FALSE, //TAKE_MEDIA_BUTTONS
        FALSE, //TAKE_AUDIO_FOCUS
        FALSE, //AUDIO_MASTER_VOLUME
        FALSE, //AUDIO_VOICE_VOLUME
        FALSE, //AUDIO_RING_VOLUME
        FALSE, //AUDIO_MEDIA_VOLUME
        FALSE, //AUDIO_ALARM_VOLUME
        FALSE, //AUDIO_NOTIFICATION_VOLUME
        FALSE, //AUDIO_BLUETOOTH_VOLUME
        FALSE, //WAKE_LOCK
        FALSE, //MONITOR_LOCATION
        FALSE, //MONITOR_HIGH_POWER_LOCATION
        FALSE, //GET_USAGE_STATS
        FALSE, //MUTE_MICROPHONE
        TRUE, //TOAST_WINDOW
        FALSE, //PROJECT_MEDIA
        FALSE, //ACTIVATE_VPN
        FALSE, // WIFI_CHANGE
        FALSE, // BLUETOOTH_CHANGE
        FALSE, // SEND_MMS
        FALSE, // READ_MMS
        FALSE, // WRITE_MMS
        FALSE, // BOOT_COMPLETED
        FALSE, // NFC_CHANGE
        FALSE, //DELETE_SMS
        FALSE, //DELETE_MMS
        FALSE, //DELETE_CONTACTS
        FALSE, //DELETE_CALL_LOG
        FALSE, //DATA_CONNECT_CHANGE
        TRUE, //ALARM_WAKEUP
        FALSE, //SU
};

Int32 AppOpsManager::sOpDefaultMode[62] = {
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_IGNORED, // OP_WRITE_SMS
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_DEFAULT, // OP_GET_USAGE_STATS
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_IGNORED, // OP_PROJECT_MEDIA
        IAppOpsManager::MODE_IGNORED, // OP_ACTIVATE_VPN
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED,
        IAppOpsManager::MODE_ALLOWED, // OP_ALARM_WAKEUP
        IAppOpsManager::MODE_ASK, // OP_SU
};

Int32 AppOpsManager::sOpDefaultStrictMode[62] = {
        IAppOpsManager::MODE_ASK,     // OP_COARSE_LOCATION
        IAppOpsManager::MODE_ASK,     // OP_FINE_LOCATION
        IAppOpsManager::MODE_ASK,     // OP_GPS
        IAppOpsManager::MODE_ALLOWED, // OP_VIBRATE
        IAppOpsManager::MODE_ASK,     // OP_READ_CONTACTS
        IAppOpsManager::MODE_ASK,     // OP_WRITE_CONTACTS
        IAppOpsManager::MODE_ASK,     // OP_READ_CALL_LOG
        IAppOpsManager::MODE_ASK,     // OP_WRITE_CALL_LOG
        IAppOpsManager::MODE_ALLOWED, // OP_READ_CALENDAR
        IAppOpsManager::MODE_ALLOWED, // OP_WRITE_CALENDAR
        IAppOpsManager::MODE_ASK,     // OP_WIFI_SCAN
        IAppOpsManager::MODE_ALLOWED, // OP_POST_NOTIFICATION
        IAppOpsManager::MODE_ALLOWED, // OP_NEIGHBORING_CELLS
        IAppOpsManager::MODE_ASK,     // OP_CALL_PHONE
        IAppOpsManager::MODE_ASK,     // OP_READ_SMS
        IAppOpsManager::MODE_ASK,     // OP_WRITE_SMS
        IAppOpsManager::MODE_ALLOWED, // OP_RECEIVE_SMS
        IAppOpsManager::MODE_ALLOWED, // OP_RECEIVE_EMERGECY_SMS
        IAppOpsManager::MODE_ALLOWED, // OP_RECEIVE_MMS
        IAppOpsManager::MODE_ALLOWED, // OP_RECEIVE_WAP_PUSH
        IAppOpsManager::MODE_ASK,     // OP_SEND_SMS
        IAppOpsManager::MODE_ALLOWED, // OP_READ_ICC_SMS
        IAppOpsManager::MODE_ALLOWED, // OP_WRITE_ICC_SMS
        IAppOpsManager::MODE_ALLOWED, // OP_WRITE_SETTINGS
        IAppOpsManager::MODE_ALLOWED, // OP_SYSTEM_ALERT_WINDOW
        IAppOpsManager::MODE_ALLOWED, // OP_ACCESS_NOTIFICATIONS
        IAppOpsManager::MODE_ASK,     // OP_CAMERA
        IAppOpsManager::MODE_ASK,     // OP_RECORD_AUDIO
        IAppOpsManager::MODE_ALLOWED, // OP_PLAY_AUDIO
        IAppOpsManager::MODE_ALLOWED, // OP_READ_CLIPBOARD
        IAppOpsManager::MODE_ALLOWED, // OP_WRITE_CLIPBOARD
        IAppOpsManager::MODE_ALLOWED, // OP_TAKE_MEDIA_BUTTONS
        IAppOpsManager::MODE_ALLOWED, // OP_TAKE_AUDIO_FOCUS
        IAppOpsManager::MODE_ALLOWED, // OP_AUDIO_MASTER_VOLUME
        IAppOpsManager::MODE_ALLOWED, // OP_AUDIO_VOICE_VOLUME
        IAppOpsManager::MODE_ALLOWED, // OP_AUDIO_RING_VOLUME
        IAppOpsManager::MODE_ALLOWED, // OP_AUDIO_MEDIA_VOLUME
        IAppOpsManager::MODE_ALLOWED, // OP_AUDIO_ALARM_VOLUME
        IAppOpsManager::MODE_ALLOWED, // OP_AUDIO_NOTIFICATION_VOLUME
        IAppOpsManager::MODE_ALLOWED, // OP_AUDIO_BLUETOOTH_VOLUME
        IAppOpsManager::MODE_ALLOWED, // OP_WAKE_LOCK
        IAppOpsManager::MODE_ALLOWED, // OP_MONITOR_LOCATION
        IAppOpsManager::MODE_ASK,     // OP_MONITOR_HIGH_POWER_LOCATION
        IAppOpsManager::MODE_DEFAULT, // OP_GET_USAGE_STATS
        IAppOpsManager::MODE_ALLOWED, // OP_MUTE_MICROPHONE
        IAppOpsManager::MODE_ALLOWED, // OP_TOAST_WINDOW
        IAppOpsManager::MODE_IGNORED, // OP_PROJECT_MEDIA
        IAppOpsManager::MODE_IGNORED, // OP_ACTIVATE_VPN
        IAppOpsManager::MODE_ASK,     // OP_WIFI_CHANGE
        IAppOpsManager::MODE_ASK,     // OP_BLUETOOTH_CHANGE
        IAppOpsManager::MODE_ASK,     // OP_SEND_MMS
        IAppOpsManager::MODE_ASK,     // OP_READ_MMS
        IAppOpsManager::MODE_ASK,     // OP_WRITE_MMS
        IAppOpsManager::MODE_ALLOWED, // OP_BOOT_COMPLETED
        IAppOpsManager::MODE_ASK,     // OP_NFC_CHANGE
        IAppOpsManager::MODE_ASK,     // OP_DELETE_SMS
        IAppOpsManager::MODE_ASK,     // OP_DELETE_MMS
        IAppOpsManager::MODE_ASK,     // OP_DELETE_CONTACTS
        IAppOpsManager::MODE_ASK,     // OP_DELETE_CALL_LOG
        IAppOpsManager::MODE_ASK,     // OP_DATA_CONNECT_CHANGE
        IAppOpsManager::MODE_ALLOWED, // OP_ALARM_WAKEUP
        IAppOpsManager::MODE_ASK,     // OP_SU
};

Boolean AppOpsManager::sOpStrictMode[62] = {
        TRUE,     // OP_COARSE_LOCATION
        TRUE,     // OP_FINE_LOCATION
        TRUE,     // OP_GPS
        FALSE,    // OP_VIBRATE
        TRUE,     // OP_READ_CONTACTS
        TRUE,     // OP_WRITE_CONTACTS
        TRUE,     // OP_READ_CALL_LOG
        TRUE,     // OP_WRITE_CALL_LOG
        FALSE,    // OP_READ_CALENDAR
        FALSE,    // OP_WRITE_CALENDAR
        TRUE,     // OP_WIFI_SCAN
        FALSE,    // OP_POST_NOTIFICATION
        FALSE,    // OP_NEIGHBORING_CELLS
        TRUE,     // OP_CALL_PHONE
        TRUE,     // OP_READ_SMS
        TRUE,     // OP_WRITE_SMS
        FALSE,    // OP_RECEIVE_SMS
        FALSE,    // OP_RECEIVE_EMERGECY_SMS
        FALSE,    // OP_RECEIVE_MMS
        FALSE,    // OP_RECEIVE_WAP_PUSH
        TRUE,     // OP_SEND_SMS
        FALSE,    // OP_READ_ICC_SMS
        FALSE,    // OP_WRITE_ICC_SMS
        FALSE,    // OP_WRITE_SETTINGS
        FALSE,    // OP_SYSTEM_ALERT_WINDOW
        FALSE,    // OP_ACCESS_NOTIFICATIONS
        TRUE,     // OP_CAMERA
        TRUE,     // OP_RECORD_AUDIO
        FALSE,    // OP_PLAY_AUDIO
        FALSE,    // OP_READ_CLIPBOARD
        FALSE,    // OP_WRITE_CLIPBOARD
        FALSE,    // OP_TAKE_MEDIA_BUTTONS
        FALSE,    // OP_TAKE_AUDIO_FOCUS
        FALSE,    // OP_AUDIO_MASTER_VOLUME
        FALSE,    // OP_AUDIO_VOICE_VOLUME
        FALSE,    // OP_AUDIO_RING_VOLUME
        FALSE,    // OP_AUDIO_MEDIA_VOLUME
        FALSE,    // OP_AUDIO_ALARM_VOLUME
        FALSE,    // OP_AUDIO_NOTIFICATION_VOLUME
        FALSE,    // OP_AUDIO_BLUETOOTH_VOLUME
        FALSE,    // OP_WAKE_LOCK
        FALSE,    // OP_MONITOR_LOCATION
        TRUE,     // OP_MONITOR_HIGH_POWER_LOCATION
        FALSE,    // OP_GET_USAGE_STATS
        FALSE,    // OP_MUTE_MICROPHONE
        FALSE,    // OP_TOAST_WINDOW
        FALSE,    // OP_PROJECT_MEDIA
        FALSE,    // OP_ACTIVATE_VPN
        TRUE,     // OP_WIFI_CHANGE
        TRUE,     // OP_BLUETOOTH_CHANGE
        TRUE,     // OP_SEND_MMS
        TRUE,     // OP_READ_MMS
        TRUE,     // OP_WRITE_MMS
        FALSE,    // OP_BOOT_COMPLETED
        TRUE,     // OP_NFC_CHANGE
        TRUE,     // OP_DELETE_SMS
        TRUE,     // OP_DELETE_MMS
        TRUE,     // OP_DELETE_CONTACTS
        TRUE,     // OP_DELETE_CALL_LOG
        TRUE,     // OP_DATA_CONNECT_CHANGE
        FALSE,    // OP_ALARM_WAKEUP
        TRUE,     // OP_SU
};

Boolean AppOpsManager::sOpDisableReset[62] = {
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        TRUE,      // OP_WRITE_SMS
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,     // OP_WIFI_CHANGE
        FALSE,     // OP_BLUETOOTH_CHANGE
        FALSE,     // OP_SEND_MMS
        FALSE,     // OP_READ_MMS
        FALSE,     // OP_WRITE_MMS
        FALSE,     // OP_BOOT_COMPLETED
        FALSE,     // OP_NFC_CHANGE
        FALSE,     // OP_DELETE_SMS
        FALSE,     // OP_DELETE_MMS
        FALSE,     // OP_DELETE_CONTACTS
        FALSE,     // OP_DELETE_CALL_LOG
        FALSE,     // OP_DATA_CONNECT_CHANGE
        FALSE,     // OP_ALARM_WAKEUP
        FALSE,     // OP_SU
};

static AutoPtr<HashMap<String, Int32> > InitOpStrToOp(String strs[])
{
    AutoPtr<HashMap<String, Int32> > map = new HashMap<String, Int32>();
    for (Int32 i = 0; i < IAppOpsManager::_NUM_OP; i++) {
        if (!strs[i].IsNull()) {
            (*map)[strs[i]] = i;
        }
    }
    return map;
}

static AutoPtr<HashMap<String, Int32> > InitNameToOp()
{
    AutoPtr<HashMap<String, Int32> > map = new HashMap<String, Int32>();
    for (Int32 i = 0; i < IAppOpsManager::_NUM_OP; i++) {
        (*map)[AppOpsManager::sOpNames[i]] = i;
    }
    return map;
}

AutoPtr<HashMap<String, Int32> > AppOpsManager::sOpStrToOp = InitOpStrToOp(AppOpsManager::sOpToString);
AutoPtr<HashMap<String, Int32> > AppOpsManager::sOpStringToOp = InitOpStrToOp(AppOpsManager::sOpToOpString);
AutoPtr<HashMap<String, Int32> > AppOpsManager::sNameToOp = InitNameToOp();

CAR_INTERFACE_IMPL(AppOpsManager, Object, IAppOpsManager)

AppOpsManager::AppOpsManager()
{
}

AppOpsManager::~AppOpsManager()
{}

ECode AppOpsManager::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("AppOpsManager");
    return NOERROR;
}

Int32 AppOpsManager::OpToSwitch(
    /* [in] */ Int32 op)
{
    return sOpToSwitch[op];
}

String AppOpsManager::OpToName(
    /* [in] */ Int32 op)
{
    if (op == OP_NONE) return String("NONE");
    if (op < sOpLength) {
        return sOpNames[op];
    }
    StringBuilder sb("Unknown(");
    sb += op;
    sb += ")";
    return sb.ToString();
}

Int32 AppOpsManager::NameToOp(
    /* [in] */ const String& name)
{
    HashMap<String, Int32>::Iterator it = sNameToOp->Find(name);
    if (it == sNameToOp->End()) {
        return IAppOpsManager::OP_NONE;
    }

    return it->mSecond;
}

String AppOpsManager::OpToPermission(
    /* [in] */ Int32 op)
{
    return sOpPerms[op];
}

String AppOpsManager::OpToRestriction(
    /* [in] */ Int32 op)
{
    return sOpRestrictions[op];
}

Boolean AppOpsManager::OpAllowSystemBypassRestriction(
    /* [in] */ Int32 op)
{
    return sOpAllowSystemRestrictionBypass[op];
}

Int32 AppOpsManager::OpToDefaultMode(
    /* [in] */ Int32 op,
    /* [in] */ Boolean isStrict)
{
    if (isStrict) {
        return sOpDefaultStrictMode[op];
    }
    return sOpDefaultMode[op];
}

Boolean AppOpsManager::OpAllowsReset(
    /* [in] */ Int32 op)
{
    return !sOpDisableReset[op];
}

ECode AppOpsManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIAppOpsService* service)
{
    mContext = context;
    mService = service;
    return NOERROR;
}

ECode AppOpsManager::GetPackagesForOps(
    /* [in] */ ArrayOf<Int32>* ops,
    /* [out] */ IList** packages) //List<AppOpsManager::PackageOps>
{
    VALIDATE_NOT_NULL(packages)
    *packages = NULL;

    return mService->GetPackagesForOps(ops, packages);
}

ECode AppOpsManager::GetOpsForPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ ArrayOf<Int32>* ops,
    /* [out] */ IList** packages) //List<IAppOpsManager::PackageOps>
{
    VALIDATE_NOT_NULL(packages)
    *packages = NULL;
    return mService->GetOpsForPackage(uid, packageName, ops, packages);
}

ECode AppOpsManager::SetMode(
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 mode)
{
    return mService->SetMode(code, uid, packageName, mode);
}

ECode AppOpsManager::SetRestriction(
    /* [in] */ Int32 code,
    /* [in] */ Int32 usage,
    /* [in] */ Int32 mode,
    /* [in] */ ArrayOf<String>* exceptionPackages)
{
    Int32 uid = Binder::GetCallingUid();
    return mService->SetAudioRestriction(code, usage, uid, mode, exceptionPackages);
}

ECode AppOpsManager::ResetAllModes()
{
    return mService->ResetAllModes();
}

ECode AppOpsManager::StartWatchingMode(
    /* [in] */ const String& op,
    /* [in] */ const String& packageName,
    /* [in] */ IAppOpsManagerOnOpChangedListener* callback)
{
    Int32 ival;
    FAIL_RETURN(StrOpToOp(op, &ival))
    return StartWatchingMode(ival, packageName, callback);
}

ECode AppOpsManager::StartWatchingMode(
    /* [in] */ Int32 op,
    /* [in] */ const String& packageName,
    /* [in] */ IAppOpsManagerOnOpChangedListener* callback)
{
    synchronized(mModeWatchersLock) {
        HashMap<AutoPtr<IAppOpsManagerOnOpChangedListener>, AutoPtr<IIAppOpsCallback> >::Iterator it;
        AutoPtr<IAppOpsManagerOnOpChangedListener> listener;
        it = mModeWatchers.Find(listener);
        AutoPtr<IIAppOpsCallback> cb;
        if (it == mModeWatchers.End()) {
            CAppOpsManagerAppOpsCallback::New(listener, (IIAppOpsCallback**)&cb);
            mModeWatchers[listener] = cb;
        }
        else {
            cb = it->mSecond;
        }

        return mService->StartWatchingMode(op, packageName, cb);
    }
    return NOERROR;
}

ECode AppOpsManager::StopWatchingMode(
    /* [in] */ IAppOpsManagerOnOpChangedListener* callback)
{
    synchronized(mModeWatchersLock) {
        HashMap<AutoPtr<IAppOpsManagerOnOpChangedListener>, AutoPtr<IIAppOpsCallback> >::Iterator it;
        AutoPtr<IAppOpsManagerOnOpChangedListener> listener;
        it = mModeWatchers.Find(listener);
        if (it != mModeWatchers.End()) {
            return mService->StopWatchingMode(it->mSecond);
        }
    }
    return NOERROR;
}

String AppOpsManager::BuildSecurityExceptionMsg(
    /* [in] */ Int32 op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    StringBuilder sb(packageName);
    sb += " from uid ";
    sb += uid;
    sb += " not allowed to perform ";
    sb += sOpNames[op];
    return sb.ToString();
}

ECode AppOpsManager::AppOpsManager::StrOpToOp(
    /* [in] */ const String& op,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;

    HashMap<String, Int32>::Iterator it = sOpStrToOp->Find(op);
    if (it == sOpStrToOp->End()) {
        Logger::E(TAG, "Unknown operation string: %s", op.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = it->mSecond;
    return NOERROR;
}

ECode AppOpsManager::CheckOp(
    /* [in] */ const String& op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;
    Int32 ival;
    FAIL_RETURN(StrOpToOp(op, &ival))
    return CheckOp(ival, uid, packageName, result);
}

ECode AppOpsManager::CheckOpNoThrow(
    /* [in] */ const String& op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;
    Int32 ival;
    FAIL_RETURN(StrOpToOp(op, &ival))
    return CheckOpNoThrow(ival, uid, packageName, result);
}

ECode AppOpsManager::NoteOp(
    /* [in] */ const String& op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;
    Int32 ival;
    FAIL_RETURN(StrOpToOp(op, &ival))
    return NoteOp(ival, uid, packageName, result);
}

ECode AppOpsManager::NoteOpNoThrow(
    /* [in] */ const String& op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;
    Int32 ival;
    FAIL_RETURN(StrOpToOp(op, &ival))
    return NoteOpNoThrow(ival, uid, packageName, result);
}

ECode AppOpsManager::StartOp(
    /* [in] */ const String& op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;
    Int32 ival;
    FAIL_RETURN(StrOpToOp(op, &ival))
    return StartOp(ival, uid, packageName, result);
}

ECode AppOpsManager::StartOpNoThrow(
    /* [in] */ const String& op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;
    Int32 ival;
    FAIL_RETURN(StrOpToOp(op, &ival))
    return StartOpNoThrow(ival, uid, packageName, result);
}

ECode AppOpsManager::FinishOp(
    /* [in] */ const String& op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    Int32 ival;
    FAIL_RETURN(StrOpToOp(op, &ival))
    return FinishOp(ival, uid, packageName);
}

Boolean AppOpsManager::IsStrictEnable()
{
    Boolean value;
    SystemProperties::GetBoolean(String("persist.sys.strict_op_enable"), FALSE, &value);
    return value;
}

ECode AppOpsManager::CheckOp(
    /* [in] */ Int32 op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;

    Int32 mode;
    ECode ec = mService->CheckOperation(op, uid, packageName, &mode);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        return NOERROR;
    }
    if (mode == MODE_ERRORED) {
        String msg = BuildSecurityExceptionMsg(op, uid, packageName);
        Logger::E(TAG, "E_SECURITY_EXCEPTION: %s", msg.string());
        return E_SECURITY_EXCEPTION;
    }

    *result = mode;
    return NOERROR;
}

ECode AppOpsManager::CheckOpNoThrow(
    /* [in] */ Int32 op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;

    return mService->CheckOperation(op, uid, packageName, result);
}

ECode AppOpsManager::CheckPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    Int32 mode;
    ECode ec = mService->CheckPackage(uid, packageName, &mode);

    if (mode != MODE_ALLOWED) {
        Logger::E(TAG,  "Package %s does not belong to %d",
            packageName.string(), uid);
        return E_SECURITY_EXCEPTION;
    }

    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        return E_SECURITY_EXCEPTION;
    }

    return ec;
}

ECode AppOpsManager::CheckAudioOp(
    /* [in] */ Int32 op,
    /* [in] */ Int32 stream,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;

    Int32 mode;
    ECode ec = mService->CheckAudioOperation(op, stream, uid, packageName, &mode);
    if (mode == MODE_ERRORED) {
        String msg = BuildSecurityExceptionMsg(op, uid, packageName);
        Logger::E(TAG, "E_SECURITY_EXCEPTION: %s", msg.string());
        return E_SECURITY_EXCEPTION;
    }
    FAIL_RETURN(ec)

    *result = mode;
    return NOERROR;
}

ECode AppOpsManager::CheckAudioOpNoThrow(
    /* [in] */ Int32 op,
    /* [in] */ Int32 stream,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;

    ECode ec = mService->CheckAudioOperation(op, stream, uid, packageName, result);
    if (FAILED(ec)) {
        *result = MODE_IGNORED;
    }
    return NOERROR;
}

ECode AppOpsManager::NoteOp(
    /* [in] */ Int32 op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;

    Int32 mode;
    ECode ec = mService->NoteOperation(op, uid, packageName, &mode);
    if (FAILED(ec) || mode == MODE_ERRORED) {
        String msg = BuildSecurityExceptionMsg(op, uid, packageName);
        Logger::E(TAG, "E_SECURITY_EXCEPTION: %s", msg.string());
        return E_SECURITY_EXCEPTION;
    }

    return MODE_IGNORED;
}

ECode AppOpsManager::NoteOpNoThrow(
    /* [in] */ Int32 op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;
    return mService->NoteOperation(op, uid, packageName, result);
}

ECode AppOpsManager::NoteOp(
    /* [in] */ Int32 op,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    return NoteOp(op, Process::MyUid(), pkgName, result);
}

ECode AppOpsManager::AppOpsManager::GetToken(
    /* [in] */ IIAppOpsService* service,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = NULL;
    synchronized(sClassLock) {
        if (sToken == NULL) {
            AutoPtr<IBinder> binder;
            CBinder::New((IBinder**)&binder);
            service->GetToken(binder, (IBinder**)&sToken);
        }

        *binder = sToken;
        REFCOUNT_ADD(*binder)
    }
    return NOERROR;
}

ECode AppOpsManager::StartOp(
    /* [in] */ Int32 op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;

    Int32 mode;
    AutoPtr<IBinder> token;
    GetToken(mService, (IBinder**)&token);
    mService->StartOperation(token, op, uid, packageName, &mode);
    if (mode == MODE_ERRORED) {
        String msg = BuildSecurityExceptionMsg(op, uid, packageName);
        Logger::E(TAG, "E_SECURITY_EXCEPTION: %s", msg.string());
        return NOERROR;
    }

    *result = mode;
    return NOERROR;
}

ECode AppOpsManager::StartOpNoThrow(
    /* [in] */ Int32 op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MODE_IGNORED;

    AutoPtr<IBinder> token;
    GetToken(mService, (IBinder**)&token);
    return mService->StartOperation(token, op, uid, packageName, result);
}

ECode AppOpsManager::StartOp(
    /* [in] */ Int32 op,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    return StartOp(op, Process::MyUid(), pkgName, result);
}

ECode AppOpsManager::FinishOp(
    /* [in] */ Int32 op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    AutoPtr<IBinder> token;
    GetToken(mService, (IBinder**)&token);
    return mService->FinishOperation(token, op, uid, packageName);
}

ECode AppOpsManager::FinishOp(
    /* [in] */ Int32 op)
{
    String pkgName;
    return FinishOp(op, Process::MyUid(), pkgName);
}

ECode AppOpsManager::IsControlAllowed(
    /* [in] */ Int32 op,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isShow = TRUE;
    // try {
    mService->IsControlAllowed(op, packageName, &isShow);
    // } catch (RemoteException e) {
    // }
    *result = isShow;
    return NOERROR;
}

ECode AppOpsManager::GetPrivacyGuardSettingForPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state)
    Boolean value = FALSE;
    // try {
    ECode ec = mService->GetPrivacyGuardSettingForPackage(uid, packageName, &value);
    // } catch (RemoteException e) {
    // }
    if (FAILED(ec)) {
        *state = FALSE;
    }
    else {
        *state = value;
    }

    return NOERROR;
}

ECode AppOpsManager::SetPrivacyGuardSettingForPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean state)
{
    // try {
    return mService->SetPrivacyGuardSettingForPackage(uid, packageName, state);
    // } catch (RemoteException e) {
    // }
}

ECode AppOpsManager::ResetCounters()
{
    // try {
    return mService->ResetCounters();
    // } catch (RemoteException e) {
    // }
}

Boolean AppOpsManager::IsStrictOp(
    /* [in] */ Int32 code)
{
    return sOpStrictMode[code];
}

Int32 AppOpsManager::StringToMode(
    /* [in] */ const String& permission)
{
    if (permission.EqualsIgnoreCase("allowed")) {
        return IAppOpsManager::MODE_ALLOWED;
    }
    else if (permission.EqualsIgnoreCase("ignored")) {
        return IAppOpsManager::MODE_IGNORED;
    }
    else if (permission.EqualsIgnoreCase("ask")) {
        return IAppOpsManager::MODE_ASK;
    }
    return IAppOpsManager::MODE_ERRORED;
}

Int32 AppOpsManager::StringOpToOp(
    /* [in] */ const String& op)
{
    HashMap<String, Int32>::Iterator it = sOpStringToOp->Find(op);
    if (it == sOpStringToOp->End()) {
        return IAppOpsManager::OP_NONE;
    }

    return it->mSecond;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
