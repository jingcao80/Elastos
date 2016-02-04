
//#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/SystemClock.h"
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include "elastos/apps/packageinstaller/InstallFlowAnalytics.h"
#include "elastos/core/IntegralToString.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Security.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
//using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IntegralToString;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::Net::ISocket;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

//=====================================================================
//                 InstallFlowAnalytics::InnerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(InstallFlowAnalytics::InnerRunnable, Object, IRunnable)

InstallFlowAnalytics::InnerRunnable::InnerRunnable(
    /* [in] */ InstallFlowAnalytics* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(NULL != mOwner);
}

ECode InstallFlowAnalytics::InnerRunnable::Run()
{
    // ==================before translated======================
    // byte[] digest = null;
    // try {
    //     digest = getPackageContentsDigest();
    // } catch (IOException e) {
    //     Log.w(TAG, "Failed to hash APK contents", e);
    // } finally {
    //     String digestHex = (digest != null)
    //             ? IntegralToString.bytesToHexString(digest, false)
    //             : "";
    //     EventLogTags.writeInstallPackageAttempt(
    //             resultAndFlags,
    //             totalElapsedTime,
    //             elapsedTimeTillPackageInfoObtained,
    //             elapsedTimeTillInstallButtonClick,
    //             digestHex);
    // }

    AutoPtr< ArrayOf<Byte> > digest;
    //try {
        digest = mOwner->GetPackageContentsDigest();
    //} catch (IOException e) {
        Logger::W(TAG, "Failed to hash APK contents");
    //} finally {
         String digestHex = (digest != NULL) ? IntegralToString::ToHexString(*digest, FALSE) : String("");
        //--: has no this func in java; EventLogTags.writeInstallPackageAttempt(
                //resultAndFlags,
                //totalElapsedTime,
                //elapsedTimeTillPackageInfoObtained,
                //elapsedTimeTillInstallButtonClick,
                //digestHex);
    //}
    return NOERROR;
}

//=====================================================================
//                         InstallFlowAnalytics
//=====================================================================
const Byte InstallFlowAnalytics::RESULT_NOT_YET_AVAILABLE = -1;
const Byte InstallFlowAnalytics::RESULT_SUCCESS = 0;
const Byte InstallFlowAnalytics::RESULT_FAILED_UNSUPPORTED_SCHEME = 1;
const Byte InstallFlowAnalytics::RESULT_FAILED_TO_GET_PACKAGE_INFO = 2;
const Byte InstallFlowAnalytics::RESULT_FAILED_PACKAGE_MISSING = 3;
const Byte InstallFlowAnalytics::RESULT_BLOCKED_BY_UNKNOWN_SOURCES_SETTING = 4;
const Byte InstallFlowAnalytics::RESULT_CANCELLED_BY_USER = 5;
const Byte InstallFlowAnalytics::RESULT_PACKAGE_MANAGER_INSTALL_FAILED = 6;
const String InstallFlowAnalytics::TAG("InstallFlowAnalytics");
const Int32 InstallFlowAnalytics::FLAG_INSTALLS_FROM_UNKNOWN_SOURCES_PERMITTED;
const Int32 InstallFlowAnalytics::FLAG_INSTALL_REQUEST_FROM_UNKNOWN_SOURCE;
const Int32 InstallFlowAnalytics::FLAG_VERIFY_APPS_ENABLED;
const Int32 InstallFlowAnalytics::FLAG_APP_VERIFIER_INSTALLED;
const Int32 InstallFlowAnalytics::FLAG_FILE_URI;
const Int32 InstallFlowAnalytics::FLAG_REPLACE;
const Int32 InstallFlowAnalytics::FLAG_SYSTEM_APP;
const Int32 InstallFlowAnalytics::FLAG_PACKAGE_INFO_OBTAINED;
const Int32 InstallFlowAnalytics::FLAG_INSTALL_BUTTON_CLICKED;
const Int32 InstallFlowAnalytics::FLAG_NEW_PERMISSIONS_FOUND;
const Int32 InstallFlowAnalytics::FLAG_PERMISSIONS_DISPLAYED;
const Int32 InstallFlowAnalytics::FLAG_NEW_PERMISSIONS_DISPLAYED;
const Int32 InstallFlowAnalytics::FLAG_ALL_PERMISSIONS_DISPLAYED;

CAR_INTERFACE_IMPL(InstallFlowAnalytics, Object, IParcelable)

InstallFlowAnalytics::InstallFlowAnalytics()
    : mFlags(0)
    , mResult(0)
    , mPackageManagerInstallResult(0)
    , mStartTimestampMillis(0)
    , mPackageInfoObtainedTimestampMillis(0)
    , mInstallButtonClickTimestampMillis(0)
    , mEndTimestampMillis(0)
    , mPackageUri("")
    , mLogged(FALSE)
{
}

InstallFlowAnalytics::InstallFlowAnalytics(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // mFlags = in.readInt();
    // mResult = in.readByte();
    // mPackageManagerInstallResult = in.readInt();
    // mStartTimestampMillis = in.readLong();
    // mPackageInfoObtainedTimestampMillis = in.readLong();
    // mInstallButtonClickTimestampMillis = in.readLong();
    // mEndTimestampMillis = in.readLong();
    // mPackageUri = in.readString();
    // mLogged = readBoolean(in);

    in->ReadInt32(&mFlags);
    in->ReadByte(&mResult);
    in->ReadInt32(&mPackageManagerInstallResult);
    in->ReadInt64(&mStartTimestampMillis);
    in->ReadInt64(&mPackageInfoObtainedTimestampMillis);
    in->ReadInt64(&mInstallButtonClickTimestampMillis);
    in->ReadInt64(&mEndTimestampMillis);
    in->ReadString(&mPackageUri);
    in->ReadBoolean(&mLogged);
}

ECode InstallFlowAnalytics::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    VALIDATE_NOT_NULL(dest);
    // ==================before translated======================
    // dest.writeInt(mFlags);
    // dest.writeByte(mResult);
    // dest.writeInt(mPackageManagerInstallResult);
    // dest.writeLong(mStartTimestampMillis);
    // dest.writeLong(mPackageInfoObtainedTimestampMillis);
    // dest.writeLong(mInstallButtonClickTimestampMillis);
    // dest.writeLong(mEndTimestampMillis);
    // dest.writeString(mPackageUri);
    // writeBoolean(dest, mLogged);

    dest->WriteInt32(mFlags);
    dest->WriteByte(mResult);
    dest->WriteInt32(mPackageManagerInstallResult);
    dest->WriteInt64(mStartTimestampMillis);
    dest->WriteInt64(mPackageInfoObtainedTimestampMillis);
    dest->WriteInt64(mInstallButtonClickTimestampMillis);
    dest->WriteInt64(mEndTimestampMillis);
    dest->WriteString(mPackageUri);
    dest->WriteBoolean(mLogged);
    return NOERROR;
}

ECode InstallFlowAnalytics::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

ECode InstallFlowAnalytics::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode InstallFlowAnalytics::DescribeContents(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return 0;

    *result = 0;
    return NOERROR;
}

ECode InstallFlowAnalytics::SetContext(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context);
    // ==================before translated======================
    // mContext = context;

    mContext = context;
    return NOERROR;
}

ECode InstallFlowAnalytics::SetInstallsFromUnknownSourcesPermitted(
    /* [in] */ Boolean permitted)
{
    // ==================before translated======================
    // setFlagState(FLAG_INSTALLS_FROM_UNKNOWN_SOURCES_PERMITTED, permitted);

    SetFlagState(FLAG_INSTALLS_FROM_UNKNOWN_SOURCES_PERMITTED, permitted);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetInstallRequestFromUnknownSource(
    /* [in] */ Boolean unknownSource)
{
    // ==================before translated======================
    // setFlagState(FLAG_INSTALL_REQUEST_FROM_UNKNOWN_SOURCE, unknownSource);

    SetFlagState(FLAG_INSTALL_REQUEST_FROM_UNKNOWN_SOURCE, unknownSource);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetVerifyAppsEnabled(
    /* [in] */ Boolean enabled)
{
    // ==================before translated======================
    // setFlagState(FLAG_VERIFY_APPS_ENABLED, enabled);

    SetFlagState(FLAG_VERIFY_APPS_ENABLED, enabled);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetAppVerifierInstalled(
    /* [in] */ Boolean installed)
{
    // ==================before translated======================
    // setFlagState(FLAG_APP_VERIFIER_INSTALLED, installed);

    SetFlagState(FLAG_APP_VERIFIER_INSTALLED, installed);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetFileUri(
    /* [in] */ Boolean fileUri)
{
    // ==================before translated======================
    // setFlagState(FLAG_FILE_URI, fileUri);

    SetFlagState(FLAG_FILE_URI, fileUri);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetPackageUri(
    /* [in] */ String packageUri)
{
    // ==================before translated======================
    // mPackageUri = packageUri;

    mPackageUri = packageUri;
    return NOERROR;
}

ECode InstallFlowAnalytics::SetReplace(
    /* [in] */ Boolean replace)
{
    // ==================before translated======================
    // setFlagState(FLAG_REPLACE, replace);

    SetFlagState(FLAG_REPLACE, replace);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetSystemApp(
    /* [in] */ Boolean systemApp)
{
    // ==================before translated======================
    // setFlagState(FLAG_SYSTEM_APP, systemApp);

    SetFlagState(FLAG_SYSTEM_APP, systemApp);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetNewPermissionsFound(
    /* [in] */ Boolean found)
{
    // ==================before translated======================
    // setFlagState(FLAG_NEW_PERMISSIONS_FOUND, found);

    SetFlagState(FLAG_NEW_PERMISSIONS_FOUND, found);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetPermissionsDisplayed(
    /* [in] */ Boolean displayed)
{
    // ==================before translated======================
    // setFlagState(FLAG_PERMISSIONS_DISPLAYED, displayed);

    SetFlagState(FLAG_PERMISSIONS_DISPLAYED, displayed);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetNewPermissionsDisplayed(
    /* [in] */ Boolean displayed)
{
    // ==================before translated======================
    // setFlagState(FLAG_NEW_PERMISSIONS_DISPLAYED, displayed);

    SetFlagState(FLAG_NEW_PERMISSIONS_DISPLAYED, displayed);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetAllPermissionsDisplayed(
    /* [in] */ Boolean displayed)
{
    // ==================before translated======================
    // setFlagState(FLAG_ALL_PERMISSIONS_DISPLAYED, displayed);

    SetFlagState(FLAG_ALL_PERMISSIONS_DISPLAYED, displayed);
    return NOERROR;
}

ECode InstallFlowAnalytics::SetStartTimestampMillis(
    /* [in] */ Int64 timestampMillis)
{
    // ==================before translated======================
    // mStartTimestampMillis = timestampMillis;

    mStartTimestampMillis = timestampMillis;
    return NOERROR;
}

ECode InstallFlowAnalytics::SetPackageInfoObtained()
{
    // ==================before translated======================
    // setFlagState(FLAG_PACKAGE_INFO_OBTAINED, true);
    // mPackageInfoObtainedTimestampMillis = SystemClock.elapsedRealtime();

    SetFlagState(FLAG_PACKAGE_INFO_OBTAINED, TRUE);
    mPackageInfoObtainedTimestampMillis = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode InstallFlowAnalytics::SetInstallButtonClicked()
{
    // ==================before translated======================
    // setFlagState(FLAG_INSTALL_BUTTON_CLICKED, true);
    // mInstallButtonClickTimestampMillis = SystemClock.elapsedRealtime();

    SetFlagState(FLAG_INSTALL_BUTTON_CLICKED, TRUE);
    mInstallButtonClickTimestampMillis = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode InstallFlowAnalytics::SetFlowFinishedWithPackageManagerResult(
    /* [in] */ Int32 packageManagerResult)
{
    // ==================before translated======================
    // mPackageManagerInstallResult = packageManagerResult;
    // if (packageManagerResult == PackageManager.INSTALL_SUCCEEDED) {
    //     setFlowFinished(
    //             InstallFlowAnalytics.RESULT_SUCCESS);
    // } else {
    //     setFlowFinished(
    //             InstallFlowAnalytics.RESULT_PACKAGE_MANAGER_INSTALL_FAILED);
    // }

    mPackageManagerInstallResult = packageManagerResult;
    if (packageManagerResult == IPackageManager::INSTALL_SUCCEEDED) {
        SetFlowFinished(InstallFlowAnalytics::RESULT_SUCCESS);
    }
    else {
        SetFlowFinished(InstallFlowAnalytics::RESULT_PACKAGE_MANAGER_INSTALL_FAILED);
    }
    return NOERROR;
}

ECode InstallFlowAnalytics::SetFlowFinished(
    /* [in] */ Byte result)
{
    // ==================before translated======================
    // if (mLogged) {
    //     return;
    // }
    // mResult = result;
    // mEndTimestampMillis = SystemClock.elapsedRealtime();
    // writeToEventLog();

    if (mLogged) {
        return NOERROR;
    }
    mResult = result;
    mEndTimestampMillis = SystemClock::GetElapsedRealtime();
    WriteToEventLog();
    return NOERROR;
}

void InstallFlowAnalytics::WriteBoolean(
    /* [in] */ IParcel* dest,
    /* [in] */ Boolean value)
{
    // ==================before translated======================
    // dest.writeByte((byte) (value ? 1 : 0));

    dest->WriteByte((Byte)(value ? 1 : 0));
}

Boolean InstallFlowAnalytics::ReadBoolean(
    /* [in] */ IParcel* dest)
{
    // ==================before translated======================
    // return dest.readByte() != 0;

    Byte b;
    dest->ReadByte(&b);
    return b != 0;
}

Boolean InstallFlowAnalytics::IsInstallsFromUnknownSourcesPermitted()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_INSTALLS_FROM_UNKNOWN_SOURCES_PERMITTED);

    return IsFlagSet(FLAG_INSTALLS_FROM_UNKNOWN_SOURCES_PERMITTED);
}

Boolean InstallFlowAnalytics::IsInstallRequestFromUnknownSource()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_INSTALL_REQUEST_FROM_UNKNOWN_SOURCE);

    return IsFlagSet(FLAG_INSTALL_REQUEST_FROM_UNKNOWN_SOURCE);
}

Boolean InstallFlowAnalytics::IsVerifyAppsEnabled()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_VERIFY_APPS_ENABLED);

    return IsFlagSet(FLAG_VERIFY_APPS_ENABLED);
}

Boolean InstallFlowAnalytics::IsAppVerifierInstalled()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_APP_VERIFIER_INSTALLED);

    return IsFlagSet(FLAG_APP_VERIFIER_INSTALLED);
}

Boolean InstallFlowAnalytics::IsFileUri()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_FILE_URI);

    return IsFlagSet(FLAG_FILE_URI);
}

Boolean InstallFlowAnalytics::IsReplace()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_REPLACE);

    return IsFlagSet(FLAG_REPLACE);
}

Boolean InstallFlowAnalytics::IsSystemApp()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_SYSTEM_APP);

    return IsFlagSet(FLAG_SYSTEM_APP);
}

Boolean InstallFlowAnalytics::IsNewPermissionsFound()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_NEW_PERMISSIONS_FOUND);

    return IsFlagSet(FLAG_NEW_PERMISSIONS_FOUND);
}

Boolean InstallFlowAnalytics::IsPermissionsDisplayed()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_PERMISSIONS_DISPLAYED);

    return IsFlagSet(FLAG_PERMISSIONS_DISPLAYED);
}

Boolean InstallFlowAnalytics::IsNewPermissionsDisplayed()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_NEW_PERMISSIONS_DISPLAYED);

    return IsFlagSet(FLAG_NEW_PERMISSIONS_DISPLAYED);
}

Boolean InstallFlowAnalytics::IsAllPermissionsDisplayed()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_ALL_PERMISSIONS_DISPLAYED);

    return IsFlagSet(FLAG_ALL_PERMISSIONS_DISPLAYED);
}

Boolean InstallFlowAnalytics::IsPackageInfoObtained()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_PACKAGE_INFO_OBTAINED);

    return IsFlagSet(FLAG_PACKAGE_INFO_OBTAINED);
}

Boolean InstallFlowAnalytics::IsInstallButtonClicked()
{
    // ==================before translated======================
    // return isFlagSet(FLAG_INSTALL_BUTTON_CLICKED);

    return IsFlagSet(FLAG_INSTALL_BUTTON_CLICKED);
}

void InstallFlowAnalytics::WriteToEventLog()
{
    // ==================before translated======================
    // byte packageManagerInstallResultByte = 0;
    // if (mResult == RESULT_PACKAGE_MANAGER_INSTALL_FAILED) {
    //     // PackageManager install error codes are negative, starting from -1 and going to
    //     // -111 (at the moment). We thus store them in negated form.
    //     packageManagerInstallResultByte = clipUnsignedValueToUnsignedByte(
    //             -mPackageManagerInstallResult);
    // }
    //
    // final int resultAndFlags = (mResult & 0xff)
    //         | ((packageManagerInstallResultByte & 0xff) << 8)
    //         | ((mFlags & 0xffff) << 16);
    //
    // // Total elapsed time from start to end, in milliseconds.
    // final int totalElapsedTime =
    //         clipUnsignedLongToUnsignedInt(mEndTimestampMillis - mStartTimestampMillis);
    //
    // // Total elapsed time from start till information about the package being installed was
    // // obtained, in milliseconds.
    // final int elapsedTimeTillPackageInfoObtained = (isPackageInfoObtained())
    //         ? clipUnsignedLongToUnsignedInt(
    //                 mPackageInfoObtainedTimestampMillis - mStartTimestampMillis)
    //         : 0;
    //
    // // Total elapsed time from start till Install button clicked, in milliseconds
    // // milliseconds.
    // final int elapsedTimeTillInstallButtonClick = (isInstallButtonClicked())
    //         ? clipUnsignedLongToUnsignedInt(
    //                     mInstallButtonClickTimestampMillis - mStartTimestampMillis)
    //         : 0;
    //
    // // If this user has consented to app verification, augment the logged event with the hash of
    // // the contents of the APK.
    // if (((mFlags & FLAG_FILE_URI) != 0)
    //         && ((mFlags & FLAG_VERIFY_APPS_ENABLED) != 0)
    //         && (isUserConsentToVerifyAppsGranted())) {
    //     // Log the hash of the APK's contents.
    //     // Reading the APK may take a while -- perform in background.
    //     AsyncTask.THREAD_POOL_EXECUTOR.execute(new Runnable() {
    //         @Override
    //         public void run() {
    //             byte[] digest = null;
    //             try {
    //                 digest = getPackageContentsDigest();
    //             } catch (IOException e) {
    //                 Log.w(TAG, "Failed to hash APK contents", e);
    //             } finally {
    //                 String digestHex = (digest != null)
    //                         ? IntegralToString.bytesToHexString(digest, false)
    //                         : "";
    //                 EventLogTags.writeInstallPackageAttempt(
    //                         resultAndFlags,
    //                         totalElapsedTime,
    //                         elapsedTimeTillPackageInfoObtained,
    //                         elapsedTimeTillInstallButtonClick,
    //                         digestHex);
    //             }
    //         }
    //     });
    // } else {
    //     // Do not log the hash of the APK's contents
    //     EventLogTags.writeInstallPackageAttempt(
    //             resultAndFlags,
    //             totalElapsedTime,
    //             elapsedTimeTillPackageInfoObtained,
    //             elapsedTimeTillInstallButtonClick,
    //             "");
    // }
    // mLogged = true;
    //
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "Analytics:"
    //             + "\n\tinstallsFromUnknownSourcesPermitted: "
    //                 + isInstallsFromUnknownSourcesPermitted()
    //             + "\n\tinstallRequestFromUnknownSource: " + isInstallRequestFromUnknownSource()
    //             + "\n\tverifyAppsEnabled: " + isVerifyAppsEnabled()
    //             + "\n\tappVerifierInstalled: " + isAppVerifierInstalled()
    //             + "\n\tfileUri: " + isFileUri()
    //             + "\n\treplace: " + isReplace()
    //             + "\n\tsystemApp: " + isSystemApp()
    //             + "\n\tpackageInfoObtained: " + isPackageInfoObtained()
    //             + "\n\tinstallButtonClicked: " + isInstallButtonClicked()
    //             + "\n\tpermissionsDisplayed: " + isPermissionsDisplayed()
    //             + "\n\tnewPermissionsDisplayed: " + isNewPermissionsDisplayed()
    //             + "\n\tallPermissionsDisplayed: " + isAllPermissionsDisplayed()
    //             + "\n\tnewPermissionsFound: " + isNewPermissionsFound()
    //             + "\n\tresult: " + mResult
    //             + "\n\tpackageManagerInstallResult: " + mPackageManagerInstallResult
    //             + "\n\ttotalDuration: " + (mEndTimestampMillis - mStartTimestampMillis) + " ms"
    //             + "\n\ttimeTillPackageInfoObtained: "
    //                 + ((isPackageInfoObtained())
    //                     ? ((mPackageInfoObtainedTimestampMillis - mStartTimestampMillis)
    //                             + " ms")
    //                     : "n/a")
    //             + "\n\ttimeTillInstallButtonClick: "
    //                 + ((isInstallButtonClicked())
    //                     ? ((mInstallButtonClickTimestampMillis - mStartTimestampMillis) + " ms")
    //                     : "n/a"));
    //     Log.v(TAG, "Wrote to Event Log: 0x" + Long.toString(resultAndFlags & 0xffffffffL, 16)
    //             + ", " + totalElapsedTime
    //             + ", " + elapsedTimeTillPackageInfoObtained
    //             + ", " + elapsedTimeTillInstallButtonClick);
    // }

    assert(0);
    Byte packageManagerInstallResultByte = 0;
    if (mResult == RESULT_PACKAGE_MANAGER_INSTALL_FAILED) {
        // PackageManager install error codes are negative, starting from -1 and going to
        // -111 (at the moment). We thus store them in negated form.
        packageManagerInstallResultByte = ClipUnsignedValueToUnsignedByte(-mPackageManagerInstallResult);
    }

    const Int32 resultAndFlags = (mResult & 0xff) | ((packageManagerInstallResultByte & 0xff) << 8)
            | ((mFlags & 0xffff) << 16);

    // Total elapsed time from start to end, in milliseconds.
    const Int32 totalElapsedTime = ClipUnsignedLongToUnsignedInt(mEndTimestampMillis - mStartTimestampMillis);

    // Total elapsed time from start till information about the package being installed was
    // obtained, in milliseconds.
    const Int32 elapsedTimeTillPackageInfoObtained = (IsPackageInfoObtained())
            ? ClipUnsignedLongToUnsignedInt(mPackageInfoObtainedTimestampMillis - mStartTimestampMillis)
            : 0;

    // Total elapsed time from start till Install button clicked, in milliseconds
    // milliseconds.
    const Int32 elapsedTimeTillInstallButtonClick = (IsInstallButtonClicked())
            ? ClipUnsignedLongToUnsignedInt(mInstallButtonClickTimestampMillis - mStartTimestampMillis)
            : 0;

    // If this user has consented to app verification, augment the logged event with the hash of
    // the contents of the APK.
    if (((mFlags & FLAG_FILE_URI) != 0)
            && ((mFlags & FLAG_VERIFY_APPS_ENABLED) != 0)
            && (IsUserConsentToVerifyAppsGranted())) {
        // Log the hash of the APK's contents.
        // Reading the APK may take a while -- perform in background.
        AutoPtr<IRunnable> runnable = new InnerRunnable(this);
        //--: AsyncTask::THREAD_POOL_EXECUTOR->Execute(runnable);
    }
    else {
        // Do not log the hash of the APK's contents
        //--: has no this func in java; EventLogTags.writeInstallPackageAttempt(
                //resultAndFlags,
                //totalElapsedTime,
                //elapsedTimeTillPackageInfoObtained,
                //elapsedTimeTillInstallButtonClick,
                //"");
    }
    mLogged = TRUE;

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        String str = String("Analytics:")
                + String("\n\tinstallsFromUnknownSourcesPermitted: ")
                + StringUtils::BooleanToString(IsInstallsFromUnknownSourcesPermitted())
                + String("\n\tinstallRequestFromUnknownSource: ")
                + StringUtils::BooleanToString(IsInstallRequestFromUnknownSource())
                + String("\n\tverifyAppsEnabled: ") + StringUtils::BooleanToString(IsVerifyAppsEnabled())
                + String("\n\tappVerifierInstalled: ") + StringUtils::BooleanToString(IsAppVerifierInstalled())
                + String("\n\tfileUri: ") + StringUtils::BooleanToString(IsFileUri())
                + String("\n\treplace: ") + StringUtils::BooleanToString(IsReplace())
                + String("\n\tsystemApp: ") + StringUtils::BooleanToString(IsSystemApp())
                + String("\n\tpackageInfoObtained: ") + StringUtils::BooleanToString(IsPackageInfoObtained())
                + String("\n\tinstallButtonClicked: ") + StringUtils::BooleanToString(IsInstallButtonClicked())
                + String("\n\tpermissionsDisplayed: ") + StringUtils::BooleanToString(IsPermissionsDisplayed())
                + String("\n\tnewPermissionsDisplayed: ") + StringUtils::BooleanToString(IsNewPermissionsDisplayed())
                + String("\n\tallPermissionsDisplayed: ") + StringUtils::BooleanToString(IsAllPermissionsDisplayed())
                + String("\n\tnewPermissionsFound: ") + StringUtils::BooleanToString(IsNewPermissionsFound())
                + String("\n\tresult: ") + StringUtils::ToHexString(mResult)
                + String("\n\tpackageManagerInstallResult: ") + StringUtils::ToString(mPackageManagerInstallResult)
                + String("\n\ttotalDuration: ") + StringUtils::ToString(mEndTimestampMillis - mStartTimestampMillis) + String(" ms")
                + String("\n\ttimeTillPackageInfoObtained: ")
                + ((IsPackageInfoObtained()) ? (StringUtils::ToString(mPackageInfoObtainedTimestampMillis - mStartTimestampMillis) + String(" ms")) : String("n/a"))
                + String("\n\ttimeTillInstallButtonClick: ")
                + ((IsInstallButtonClicked()) ? (StringUtils::ToString(mInstallButtonClickTimestampMillis - mStartTimestampMillis) + String(" ms")) : String("n/a"));
        Logger::V(TAG, str);

        str = String("Wrote to Event Log: 0x") + StringUtils::ToString((Int32)(resultAndFlags & 0xffffffffL), 16)
                + String(", ") + StringUtils::ToString(totalElapsedTime)
                + String(", ") + StringUtils::ToString(elapsedTimeTillPackageInfoObtained)
                + String(", ") + StringUtils::ToString(elapsedTimeTillInstallButtonClick);
        Logger::V(TAG, str);
    }
}

const Byte InstallFlowAnalytics::ClipUnsignedValueToUnsignedByte(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // if (value < 0) {
    //     return 0;
    // } else if (value > 0xff) {
    //     return (byte) 0xff;
    // } else {
    //     return (byte) value;
    // }

    if (value < 0) {
        return 0;
    }
    else if (value > 0xff) {
        return (Byte)0xff;
    }
    else {
        return (Byte)value;
    }
}

const Int32 InstallFlowAnalytics::ClipUnsignedLongToUnsignedInt(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // if (value < 0) {
    //     return 0;
    // } else if (value > 0xffffffffL) {
    //     return 0xffffffff;
    // } else {
    //     return (int) value;
    // }

    if (value < 0) {
        return 0;
    }
    else if (value > 0xffffffffL) {
        return 0xffffffff;
    }
    else {
        return (Int32)value;
    }
}

void InstallFlowAnalytics::SetFlagState(
    /* [in] */ Int32 flag,
    /* [in] */ Boolean set)
{
    // ==================before translated======================
    // if (set) {
    //     mFlags |= flag;
    // } else {
    //     mFlags &= ~flag;
    // }

    if (set) {
        mFlags |= flag;
    }
    else {
        mFlags &= ~flag;
    }
}

Boolean InstallFlowAnalytics::IsFlagSet(
    /* [in] */ Int32 flag)
{
    // ==================before translated======================
    // return (mFlags & flag) == flag;

    return (mFlags & flag) == flag;
}

Boolean InstallFlowAnalytics::IsUserConsentToVerifyAppsGranted()
{
    // ==================before translated======================
    // return Settings.Secure.getInt(
    //         mContext.getContentResolver(),
    //         Settings.Secure.PACKAGE_VERIFIER_USER_CONSENT, 0) != 0;

    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 resTmp = 0;
    settingsSecure->GetInt32(contentResolver, ISettingsSecure::PACKAGE_VERIFIER_USER_CONSENT, 0, &resTmp);
    return resTmp != 0;
}

AutoPtr< ArrayOf<Byte> > InstallFlowAnalytics::GetPackageContentsDigest()
{
    // ==================before translated======================
    // File file = new File(Uri.parse(mPackageUri).getPath());
    // return getSha256ContentsDigest(file);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(mPackageUri, (IUri**)&uri);
    String path;
    uri->GetPath(&path);
    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    return GetSha256ContentsDigest(file);
}

AutoPtr< ArrayOf<Byte> > InstallFlowAnalytics::GetSha256ContentsDigest(
    /* [in] */ IFile* file)
{
    // ==================before translated======================
    // MessageDigest digest;
    // try {
    //     digest = MessageDigest.getInstance("SHA-256");
    // } catch (NoSuchAlgorithmException e) {
    //     throw new RuntimeException("SHA-256 not available", e);
    // }
    //
    // byte[] buf = new byte[8192];
    // InputStream in = null;
    // try {
    //     in = new BufferedInputStream(new FileInputStream(file), buf.length);
    //     int chunkSize;
    //     while ((chunkSize = in.read(buf)) != -1) {
    //         digest.update(buf, 0, chunkSize);
    //     }
    // } finally {
    //     IoUtils.closeQuietly(in);
    // }
    // return digest.digest();

    AutoPtr<IMessageDigestHelper> helper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    AutoPtr<IMessageDigest> digest;
    //try {
        helper->GetInstance(String("SHA-256"), (IMessageDigest**)&digest);
    //}
    //catch (NoSuchAlgorithmException e) {
        //throw new RuntimeException("SHA-256 not available", e);
    //}

    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(8192);
    AutoPtr<IInputStream> in;
    //try {
        AutoPtr<IFileInputStream> inputStreamTmp;
        CFileInputStream::New(file, (IFileInputStream**)&inputStreamTmp);

        AutoPtr<IBufferedInputStream> bufferedInputStream;
        CBufferedInputStream::New(IInputStream::Probe(inputStreamTmp), buf->GetLength(), (IBufferedInputStream**)&bufferedInputStream);
        in = IInputStream::Probe(bufferedInputStream);
        Int32 chunkSize;
        Int32 number = 0;
        while ((chunkSize = in->Read(buf, &number), number) != -1) {
            digest->Update(buf, 0, chunkSize);
        }
    //} finally {
        //--: IoUtils::CloseQuietly(ISocket::Probe(in));
    //}

    AutoPtr< ArrayOf<Byte> > result;
    digest->Digest((ArrayOf<Byte>**)&result);
    return result;
}

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos


