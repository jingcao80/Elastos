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

#include "elastos/droid/packageinstaller/CInstallFlowAnalytics.h"
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/core/IntegralToString.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::AsyncTask;
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
using Elastos::IO::ICloseable;
using Elastos::Net::ISocket;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

//=====================================================================
//                 CInstallFlowAnalytics::InnerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(CInstallFlowAnalytics::InnerRunnable, Object, IRunnable)

CInstallFlowAnalytics::InnerRunnable::InnerRunnable(
    /* [in] */ CInstallFlowAnalytics* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode CInstallFlowAnalytics::InnerRunnable::Run()
{
    AutoPtr< ArrayOf<Byte> > digest;
    //try {
        digest = mOwner->GetPackageContentsDigest();
    //} catch (IOException e) {
        // Logger::W(TAG, "Failed to hash APK contents");
    //} finally {
        // String digestHex = (digest != NULL) ? IntegralToString::ToHexString(*digest, FALSE) : String("");
        // EventLogTags.writeInstallPackageAttempt(
                //resultAndFlags,
                //totalElapsedTime,
                //elapsedTimeTillPackageInfoObtained,
                //elapsedTimeTillInstallButtonClick,
                //digestHex);
    //}
    return NOERROR;
}

//=====================================================================
//                         CInstallFlowAnalytics
//=====================================================================
const Byte CInstallFlowAnalytics::RESULT_NOT_YET_AVAILABLE = -1;
const Byte CInstallFlowAnalytics::RESULT_SUCCESS = 0;
const Byte CInstallFlowAnalytics::RESULT_FAILED_UNSUPPORTED_SCHEME = 1;
const Byte CInstallFlowAnalytics::RESULT_FAILED_TO_GET_PACKAGE_INFO = 2;
const Byte CInstallFlowAnalytics::RESULT_FAILED_PACKAGE_MISSING = 3;
const Byte CInstallFlowAnalytics::RESULT_BLOCKED_BY_UNKNOWN_SOURCES_SETTING = 4;
const Byte CInstallFlowAnalytics::RESULT_CANCELLED_BY_USER = 5;
const Byte CInstallFlowAnalytics::RESULT_PACKAGE_MANAGER_INSTALL_FAILED = 6;
const String CInstallFlowAnalytics::TAG("CInstallFlowAnalytics");
const Int32 CInstallFlowAnalytics::FLAG_INSTALLS_FROM_UNKNOWN_SOURCES_PERMITTED;
const Int32 CInstallFlowAnalytics::FLAG_INSTALL_REQUEST_FROM_UNKNOWN_SOURCE;
const Int32 CInstallFlowAnalytics::FLAG_VERIFY_APPS_ENABLED;
const Int32 CInstallFlowAnalytics::FLAG_APP_VERIFIER_INSTALLED;
const Int32 CInstallFlowAnalytics::FLAG_FILE_URI;
const Int32 CInstallFlowAnalytics::FLAG_REPLACE;
const Int32 CInstallFlowAnalytics::FLAG_SYSTEM_APP;
const Int32 CInstallFlowAnalytics::FLAG_PACKAGE_INFO_OBTAINED;
const Int32 CInstallFlowAnalytics::FLAG_INSTALL_BUTTON_CLICKED;
const Int32 CInstallFlowAnalytics::FLAG_NEW_PERMISSIONS_FOUND;
const Int32 CInstallFlowAnalytics::FLAG_PERMISSIONS_DISPLAYED;
const Int32 CInstallFlowAnalytics::FLAG_NEW_PERMISSIONS_DISPLAYED;
const Int32 CInstallFlowAnalytics::FLAG_ALL_PERMISSIONS_DISPLAYED;

CAR_INTERFACE_IMPL(CInstallFlowAnalytics, Object, IParcelable)

CAR_OBJECT_IMPL(CInstallFlowAnalytics)

CInstallFlowAnalytics::CInstallFlowAnalytics()
    : mFlags(0)
    , mResult(0)
    , mPackageManagerInstallResult(0)
    , mStartTimestampMillis(0)
    , mPackageInfoObtainedTimestampMillis(0)
    , mInstallButtonClickTimestampMillis(0)
    , mEndTimestampMillis(0)
    , mLogged(FALSE)
{
}

ECode CInstallFlowAnalytics::constructor()
{
    return NOERROR;
}

ECode CInstallFlowAnalytics::WriteToParcel(
    /* [in] */ IParcel* dest)
{
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

ECode CInstallFlowAnalytics::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mFlags);
    source->ReadByte(&mResult);
    source->ReadInt32(&mPackageManagerInstallResult);
    source->ReadInt64(&mStartTimestampMillis);
    source->ReadInt64(&mPackageInfoObtainedTimestampMillis);
    source->ReadInt64(&mInstallButtonClickTimestampMillis);
    source->ReadInt64(&mEndTimestampMillis);
    source->ReadString(&mPackageUri);
    source->ReadBoolean(&mLogged);

    return NOERROR;
}

ECode CInstallFlowAnalytics::SetContext(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetInstallsFromUnknownSourcesPermitted(
    /* [in] */ Boolean permitted)
{
    SetFlagState(FLAG_INSTALLS_FROM_UNKNOWN_SOURCES_PERMITTED, permitted);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetInstallRequestFromUnknownSource(
    /* [in] */ Boolean unknownSource)
{
    SetFlagState(FLAG_INSTALL_REQUEST_FROM_UNKNOWN_SOURCE, unknownSource);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetVerifyAppsEnabled(
    /* [in] */ Boolean enabled)
{
    SetFlagState(FLAG_VERIFY_APPS_ENABLED, enabled);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetAppVerifierInstalled(
    /* [in] */ Boolean installed)
{
    SetFlagState(FLAG_APP_VERIFIER_INSTALLED, installed);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetFileUri(
    /* [in] */ Boolean fileUri)
{
    SetFlagState(FLAG_FILE_URI, fileUri);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetPackageUri(
    /* [in] */ const String& packageUri)
{
    // ==================before translated======================
    // mPackageUri = packageUri;

    mPackageUri = packageUri;
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetReplace(
    /* [in] */ Boolean replace)
{
    SetFlagState(FLAG_REPLACE, replace);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetSystemApp(
    /* [in] */ Boolean systemApp)
{
    SetFlagState(FLAG_SYSTEM_APP, systemApp);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetNewPermissionsFound(
    /* [in] */ Boolean found)
{
    SetFlagState(FLAG_NEW_PERMISSIONS_FOUND, found);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetPermissionsDisplayed(
    /* [in] */ Boolean displayed)
{
    SetFlagState(FLAG_PERMISSIONS_DISPLAYED, displayed);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetNewPermissionsDisplayed(
    /* [in] */ Boolean displayed)
{
    SetFlagState(FLAG_NEW_PERMISSIONS_DISPLAYED, displayed);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetAllPermissionsDisplayed(
    /* [in] */ Boolean displayed)
{
    SetFlagState(FLAG_ALL_PERMISSIONS_DISPLAYED, displayed);
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetStartTimestampMillis(
    /* [in] */ Int64 timestampMillis)
{
    mStartTimestampMillis = timestampMillis;
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetPackageInfoObtained()
{
    SetFlagState(FLAG_PACKAGE_INFO_OBTAINED, TRUE);
    mPackageInfoObtainedTimestampMillis = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetInstallButtonClicked()
{
    SetFlagState(FLAG_INSTALL_BUTTON_CLICKED, TRUE);
    mInstallButtonClickTimestampMillis = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetFlowFinishedWithPackageManagerResult(
    /* [in] */ Int32 packageManagerResult)
{
    mPackageManagerInstallResult = packageManagerResult;
    if (packageManagerResult == IPackageManager::INSTALL_SUCCEEDED) {
        SetFlowFinished(CInstallFlowAnalytics::RESULT_SUCCESS);
    }
    else {
        SetFlowFinished(CInstallFlowAnalytics::RESULT_PACKAGE_MANAGER_INSTALL_FAILED);
    }
    return NOERROR;
}

ECode CInstallFlowAnalytics::SetFlowFinished(
    /* [in] */ Byte result)
{
    if (mLogged) {
        return NOERROR;
    }
    mResult = result;
    mEndTimestampMillis = SystemClock::GetElapsedRealtime();
    WriteToEventLog();
    return NOERROR;
}

void CInstallFlowAnalytics::WriteBoolean(
    /* [in] */ IParcel* dest,
    /* [in] */ Boolean value)
{
    dest->WriteByte((Byte)(value ? 1 : 0));
}

Boolean CInstallFlowAnalytics::ReadBoolean(
    /* [in] */ IParcel* dest)
{
    Byte b;
    dest->ReadByte(&b);
    return b != 0;
}

Boolean CInstallFlowAnalytics::IsInstallsFromUnknownSourcesPermitted()
{
    return IsFlagSet(FLAG_INSTALLS_FROM_UNKNOWN_SOURCES_PERMITTED);
}

Boolean CInstallFlowAnalytics::IsInstallRequestFromUnknownSource()
{
    return IsFlagSet(FLAG_INSTALL_REQUEST_FROM_UNKNOWN_SOURCE);
}

Boolean CInstallFlowAnalytics::IsVerifyAppsEnabled()
{
    return IsFlagSet(FLAG_VERIFY_APPS_ENABLED);
}

Boolean CInstallFlowAnalytics::IsAppVerifierInstalled()
{
    return IsFlagSet(FLAG_APP_VERIFIER_INSTALLED);
}

Boolean CInstallFlowAnalytics::IsFileUri()
{
    return IsFlagSet(FLAG_FILE_URI);
}

Boolean CInstallFlowAnalytics::IsReplace()
{
    return IsFlagSet(FLAG_REPLACE);
}

Boolean CInstallFlowAnalytics::IsSystemApp()
{
    return IsFlagSet(FLAG_SYSTEM_APP);
}

Boolean CInstallFlowAnalytics::IsNewPermissionsFound()
{
    return IsFlagSet(FLAG_NEW_PERMISSIONS_FOUND);
}

Boolean CInstallFlowAnalytics::IsPermissionsDisplayed()
{
    return IsFlagSet(FLAG_PERMISSIONS_DISPLAYED);
}

Boolean CInstallFlowAnalytics::IsNewPermissionsDisplayed()
{
    return IsFlagSet(FLAG_NEW_PERMISSIONS_DISPLAYED);
}

Boolean CInstallFlowAnalytics::IsAllPermissionsDisplayed()
{
    return IsFlagSet(FLAG_ALL_PERMISSIONS_DISPLAYED);
}

Boolean CInstallFlowAnalytics::IsPackageInfoObtained()
{
    return IsFlagSet(FLAG_PACKAGE_INFO_OBTAINED);
}

Boolean CInstallFlowAnalytics::IsInstallButtonClicked()
{
    return IsFlagSet(FLAG_INSTALL_BUTTON_CLICKED);
}

void CInstallFlowAnalytics::WriteToEventLog()
{
    // assert(0);
    // Byte packageManagerInstallResultByte = 0;
    // if (mResult == RESULT_PACKAGE_MANAGER_INSTALL_FAILED) {
    //     // PackageManager install error codes are negative, starting from -1 and going to
    //     // -111 (at the moment). We thus store them in negated form.
    //     packageManagerInstallResultByte = ClipUnsignedValueToUnsignedByte(-mPackageManagerInstallResult);
    // }

    // const Int32 resultAndFlags = (mResult & 0xff) | ((packageManagerInstallResultByte & 0xff) << 8)
    //         | ((mFlags & 0xffff) << 16);

    // // Total elapsed time from start to end, in milliseconds.
    // const Int32 totalElapsedTime = ClipUnsignedLongToUnsignedInt(mEndTimestampMillis - mStartTimestampMillis);

    // // Total elapsed time from start till information about the package being installed was
    // // obtained, in milliseconds.
    // const Int32 elapsedTimeTillPackageInfoObtained = (IsPackageInfoObtained())
    //         ? ClipUnsignedLongToUnsignedInt(mPackageInfoObtainedTimestampMillis - mStartTimestampMillis)
    //         : 0;

    // // Total elapsed time from start till Install button clicked, in milliseconds
    // // milliseconds.
    // const Int32 elapsedTimeTillInstallButtonClick = (IsInstallButtonClicked())
    //         ? ClipUnsignedLongToUnsignedInt(mInstallButtonClickTimestampMillis - mStartTimestampMillis)
    //         : 0;

    // // If this user has consented to app verification, augment the logged event with the hash of
    // // the contents of the APK.
    // if (((mFlags & FLAG_FILE_URI) != 0)
    //         && ((mFlags & FLAG_VERIFY_APPS_ENABLED) != 0)
    //         && (IsUserConsentToVerifyAppsGranted())) {
    //     // Log the hash of the APK's contents.
    //     // Reading the APK may take a while -- perform in background.
    //     AutoPtr<IRunnable> runnable = new InnerRunnable(this);
    //     //--: AsyncTask::THREAD_POOL_EXECUTOR->Execute(runnable);
    // }
    // else {
    //     // Do not log the hash of the APK's contents
    //     //--: has no this func in java; EventLogTags.writeInstallPackageAttempt(
    //             //resultAndFlags,
    //             //totalElapsedTime,
    //             //elapsedTimeTillPackageInfoObtained,
    //             //elapsedTimeTillInstallButtonClick,
    //             //"");
    // }
    // mLogged = TRUE;

    // if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
    //     String str = String("Analytics:")
    //             + String("\n\tinstallsFromUnknownSourcesPermitted: ")
    //             + StringUtils::BooleanToString(IsInstallsFromUnknownSourcesPermitted())
    //             + String("\n\tinstallRequestFromUnknownSource: ")
    //             + StringUtils::BooleanToString(IsInstallRequestFromUnknownSource())
    //             + String("\n\tverifyAppsEnabled: ") + StringUtils::BooleanToString(IsVerifyAppsEnabled())
    //             + String("\n\tappVerifierInstalled: ") + StringUtils::BooleanToString(IsAppVerifierInstalled())
    //             + String("\n\tfileUri: ") + StringUtils::BooleanToString(IsFileUri())
    //             + String("\n\treplace: ") + StringUtils::BooleanToString(IsReplace())
    //             + String("\n\tsystemApp: ") + StringUtils::BooleanToString(IsSystemApp())
    //             + String("\n\tpackageInfoObtained: ") + StringUtils::BooleanToString(IsPackageInfoObtained())
    //             + String("\n\tinstallButtonClicked: ") + StringUtils::BooleanToString(IsInstallButtonClicked())
    //             + String("\n\tpermissionsDisplayed: ") + StringUtils::BooleanToString(IsPermissionsDisplayed())
    //             + String("\n\tnewPermissionsDisplayed: ") + StringUtils::BooleanToString(IsNewPermissionsDisplayed())
    //             + String("\n\tallPermissionsDisplayed: ") + StringUtils::BooleanToString(IsAllPermissionsDisplayed())
    //             + String("\n\tnewPermissionsFound: ") + StringUtils::BooleanToString(IsNewPermissionsFound())
    //             + String("\n\tresult: ") + StringUtils::ToHexString(mResult)
    //             + String("\n\tpackageManagerInstallResult: ") + StringUtils::ToString(mPackageManagerInstallResult)
    //             + String("\n\ttotalDuration: ") + StringUtils::ToString(mEndTimestampMillis - mStartTimestampMillis) + String(" ms")
    //             + String("\n\ttimeTillPackageInfoObtained: ")
    //             + ((IsPackageInfoObtained()) ? (StringUtils::ToString(mPackageInfoObtainedTimestampMillis - mStartTimestampMillis) + String(" ms")) : String("n/a"))
    //             + String("\n\ttimeTillInstallButtonClick: ")
    //             + ((IsInstallButtonClicked()) ? (StringUtils::ToString(mInstallButtonClickTimestampMillis - mStartTimestampMillis) + String(" ms")) : String("n/a"));
    //     Logger::V(TAG, str);

    //     str = String("Wrote to Event Log: 0x") + StringUtils::ToString((Int32)(resultAndFlags & 0xffffffffL), 16)
    //             + String(", ") + StringUtils::ToString(totalElapsedTime)
    //             + String(", ") + StringUtils::ToString(elapsedTimeTillPackageInfoObtained)
    //             + String(", ") + StringUtils::ToString(elapsedTimeTillInstallButtonClick);
    //     Logger::V(TAG, str);
    // }
}

Byte CInstallFlowAnalytics::ClipUnsignedValueToUnsignedByte(
    /* [in] */ Int64 value)
{
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

Int32 CInstallFlowAnalytics::ClipUnsignedLongToUnsignedInt(
    /* [in] */ Int64 value)
{
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

void CInstallFlowAnalytics::SetFlagState(
    /* [in] */ Int32 flag,
    /* [in] */ Boolean set)
{
    if (set) {
        mFlags |= flag;
    }
    else {
        mFlags &= ~flag;
    }
}

Boolean CInstallFlowAnalytics::IsFlagSet(
    /* [in] */ Int32 flag)
{
    return (mFlags & flag) == flag;
}

Boolean CInstallFlowAnalytics::IsUserConsentToVerifyAppsGranted()
{
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 resTmp = 0;
    settingsSecure->GetInt32(contentResolver, ISettingsSecure::PACKAGE_VERIFIER_USER_CONSENT, 0, &resTmp);
    return resTmp != 0;
}

AutoPtr< ArrayOf<Byte> > CInstallFlowAnalytics::GetPackageContentsDigest()
{
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

AutoPtr< ArrayOf<Byte> > CInstallFlowAnalytics::GetSha256ContentsDigest(
    /* [in] */ IFile* file)
{
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
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        ioUtils->CloseQuietly(ICloseable::Probe(in));
    //}

    AutoPtr< ArrayOf<Byte> > result;
    digest->Digest((ArrayOf<Byte>**)&result);
    return result;
}

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos


