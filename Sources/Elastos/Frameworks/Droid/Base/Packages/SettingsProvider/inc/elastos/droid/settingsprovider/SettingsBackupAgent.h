#ifndef __ELASTOS_DROID_SETTINGSPROVIDER_SETTINGSBACKUPAGENT_H__
#define __ELASTOS_DROID_SETTINGSPROVIDER_SETTINGSBACKUPAGENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Wifi.h>
#include "elastos/droid/settingsprovider/SettingsHelper.h"
// #include "elastos/droid/app/backup/BackupAgentHelper.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>

// using Elastos::Droid::App::Backup::BackupAgentHelper;
using Elastos::Droid::App::Backup::IBackupDataInput;
using Elastos::Droid::App::Backup::IBackupDataOutput;
using Elastos::Droid::App::Backup::IFullBackupDataOutput;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::IO::IFile;
using Elastos::IO::IWriter;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IBufferedWriter;
using Elastos::Utility::IHashSet;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

/**
 * Performs backup and restore of the System and Secure settings.
 * List of settings that are backed up are stored in the Settings.java file
 */
class SettingsBackupAgent
    // : public BackupAgentHelper
    : public Object
{
public:
    // Class for capturing a network definition from the wifi supplicant config
    // file
    class NetWork
        : public Object
    {
    public:
        NetWork();

        ~NetWork();

        static CARAPI_(AutoPtr<NetWork>) ReadFromStream(
            /* [in] */ IBufferedReader* in);

        CARAPI_(void) RememberLine(
            /* [in] */ const String& line);

        CARAPI Write(
            /* [in] */ IWriter* w);

        CARAPI_(void) Dump();

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* res);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* code);

    public:
        String mSsid;
        String mKey_mgmt;
        Boolean mCertUsed;

    private:
        List<String> mRawLines;
    };

    // Ingest multiple wifi config file fragments, looking for network={} blocks
    // and eliminating duplicates
    class WifiNetworkSettings
        : public Object
    {
    private:
        struct HashNetWork
        {
            size_t operator()(NetWork* s) const
            {
                assert(s != NULL);
                Int32 code;
                s->GetHashCode(&code);
                return (size_t)code;
            }
        };
        // One for fast lookup, one for maintaining ordering
        HashSet<AutoPtr<NetWork>, HashNetWork > mKnownNetworks;

        List<AutoPtr<NetWork> > mNetWorks;

    public:
        CARAPI_(void) ReadNetworks(
            /* [in] */ IBufferedReader* in);

        CARAPI Write(
            /* [in] */ IWriter* w);

        CARAPI_(void) Dump();

    };

    class WifiRestoreRunnable
        : public Runnable
    {
    public:
        WifiRestoreRunnable(
            /* [in] */ SettingsBackupAgent* host);

        CARAPI Run();

    private:
        CARAPI_(void) IncorporateWifiSupplicant(
            /* [in] */ IBackupDataInput* data);

        CARAPI_(void) IncorporateWifiConfigFile(
            /* [in] */ IBackupDataInput* data);

    private:
        AutoPtr<ArrayOf<Byte> > mRestoredSupplicantData;
        AutoPtr<ArrayOf<Byte> > mRestoredWifiConfigFile;
        SettingsBackupAgent* mHost;
        friend class SettingsBackupAgent;
    };

public:
    SettingsBackupAgent();

    ~SettingsBackupAgent();

    CARAPI constructor();

    CARAPI OnCreate();

    CARAPI OnBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState);

    CARAPI OnRestore(
        /* [in] */ IBackupDataInput* data,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState);

    CARAPI OnFullBackup(
        /* [in] */ IFullBackupDataOutput* data);

    CARAPI OnRestoreFile(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int64 size,
        /* [in] */ IFile* destination,
        /* [in] */ Int32 type,
        /* [in] */ Int64 mode,
        /* [in] */ Int64 mtime);

private:
    // Instantiate the wifi-config restore runnable, scheduling it for execution
    // a minute hence
    CARAPI_(void) InitWifiRestoreIfNecessary();

    CARAPI_(AutoPtr<ArrayOf<Int64> >) ReadOldChecksums(
        /* [in] */ IParcelFileDescriptor* oldState);

    CARAPI_(void) WriteNewChecksums(
        /* [in] */ ArrayOf<Int64>* checksums,
        /* [in] */ IParcelFileDescriptor* newState);

    CARAPI_(Int64) WriteIfChanged(
        /* [in] */ Int64 oldChecksums,
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IBackupDataOutput* output);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetSystemSettings();

    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetSecureSettings();

    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetGlobalSettings();

    CARAPI_(void) RestoreSettings(
        /* [in] */ IBackupDataInput* data,
        /* [in] */ IUri* contentUri,
        /* [in] */ IHashSet* movedToGlobal);

    CARAPI RestoreSettings(
        /* [in] */ ArrayOf<Byte>* settings,
        /* [in] */ Int32 bytes,
        /* [in] */ IUri* contentUri,
        /* [in] */ IHashSet* movedToGlobal);

    /**
    * Given a cursor and a set of keys, extract the required keys and
    * values and write them to a byte array.
    *
    * @param cursor A cursor with settings data.
    * @param settings The settings to extract.
    * @return The byte array of extracted values.
    */
    CARAPI ExtractRelevantValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ ArrayOf<String>* settings,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetFileData(
        /* [in] */ const String& filename);

    CARAPI_(void) RestoreFileData(
        /* [in] */ const String& filename,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 size);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetWifiSupplicant(
        /* [in] */ const String& filename);

    CARAPI_(void) RestoreWifiSupplicant(
        /* [in] */ const String& filename,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 size);

    CARAPI_(void) CopyWifiSupplicantTemplate(
        /* [in] */ IBufferedWriter* bw);

    /**
    * Write an int in BigEndian into the byte array.
    * @param out byte array
    * @param pos current pos in array
    * @param value integer to write
    * @return the index after adding the size of an int (4) in bytes.
    */
    CARAPI_(Int32) WriteInt32(
        /* [in] */ ArrayOf<Byte>* out,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 value);

    CARAPI_(Int32) WriteBytes(
        /* [in] */ ArrayOf<Byte>* out,
        /* [in] */ Int32 pos,
        /* [in] */ ArrayOf<Byte>* value);

    CARAPI_(Int32) ReadInt32(
        /* [in] */ ArrayOf<Byte>* in,
        /* [in] */ Int32 pos);

    CARAPI_(Int32) EnableWifi(
        /* [in] */ Boolean enable);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean DEBUG_BACKUP;

    static const String KEY_SYSTEM;
    static const String KEY_SECURE;
    static const String KEY_GLOBAL;
    static const String KEY_LOCALE;

    // Versioning of the state file.  Increment this version
    // number any time the set of state items is altered.
    static const Int32 STATE_VERSION = 3;

    // Slots in the checksum array.  Never insert new items in the middle
    // of this array; new slots must be appended.
    static const Int32 STATE_SYSTEM = 0;
    static const Int32 STATE_SECURE = 1;
    static const Int32 STATE_LOCALE = 2;
    static const Int32 STATE_WIFI_SUPPLICANT = 3;
    static const Int32 STATE_WIFI_CONFIG = 4;
    static const Int32 STATE_GLOBAL = 5;
    static const Int32 STATE_SIZE = 6;//The current number of state items

    // Number of entries in the checksum array at various version numbers
    static const Int32 STATE_SIZES[];

    // Versioning of the 'full backup' format
    static const Int32 FULL_BACKUP_VERSION = 2;
    static const Int32 FULL_BACKUP_ADDED_GLOBAL = 2;
    static const Int32 INTEGER_BYTE_COUNT = 4; //Integer.SIZE / Byte.SIZE

    static const AutoPtr<ArrayOf<Byte> > EMPTY_DATA;

    static const Int32 COLUMN_NAME = 1;
    static const Int32 COLUMN_VALUE = 2;

    static const AutoPtr<ArrayOf<String> > PROJECTION;

    static const String FILE_WIFI_SUPPLICANT;
    static const String FILE_WIFI_SUPPLICANT_TEMPLATE;

    // the key to store the WIFI data under, should be sorted as last, so
    // restore happens last.
    // use very late unicode character to quasi-guarantee last sort position.
    static const String KEY_WIFI_SUPPLICANT;
    static const String KEY_WIFI_CONFIG;

    static const String STAGE_FILE;

    // Delay in milliseconds between the restore operation and when we will bounce
    // wifi in order to rewrite the supplicant config etc.
    static const Int64 WIFI_BOUNCE_DELAY_MILLIS = 60000; //60 * 1000

    AutoPtr<SettingsHelper> mSettingsHelper;
    AutoPtr<IWifiManager> mWfm;
    String mWifiConfigFile;
    AutoPtr<WifiRestoreRunnable> mWifiRestore;
};

} // namespace SettingsProvider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SETTINGSPROVIDER_SETTINGSBACKUPAGENT_H__
