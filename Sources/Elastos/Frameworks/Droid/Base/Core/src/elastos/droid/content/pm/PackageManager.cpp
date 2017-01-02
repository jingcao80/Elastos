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

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/pm/PackageManager.h"
#include "elastos/droid/content/pm/PackageParser.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CEnvironment.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

//==============================================================================
//                  PackageManager::LegacyPackageDeleteObserver
//==============================================================================

ECode PackageManager::LegacyPackageDeleteObserver::constructor(
    /* [in] */ IIPackageDeleteObserver* legacy)
{
    PackageDeleteObserver::constructor();
    mLegacy = legacy;
    return NOERROR;
}

ECode PackageManager::LegacyPackageDeleteObserver::OnPackageDeleted(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg)
{
    if (mLegacy == NULL) return NOERROR;
    // try {
    mLegacy->PackageDeleted(basePackageName, returnCode);
    Release();
    // } catch (RemoteException ignored) {
    // }
    return NOERROR;
}


//==============================================================================
//                  PackageManager
//==============================================================================

const String PackageManager::TAG("PackageManager");

CAR_INTERFACE_IMPL(PackageManager, Object, IPackageManager)

PackageManager::PackageManager()
{}

PackageManager::~PackageManager()
{}

ECode PackageManager::BuildPermissionRequestIntent(
    /* [in] */ ArrayOf<String>* permissions,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = NULL;
    if (permissions == NULL) {
        // throw new NullPointerException("permissions cannot be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    for (Int32 i = 0; i < permissions->GetLength(); ++i) {
        String permission = (*permissions)[i];
        if (permission.IsNull()) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    AutoPtr<IIntent> i;
    CIntent::New(ACTION_REQUEST_PERMISSION, (IIntent**)&i);
    i->PutExtra(EXTRA_REQUEST_PERMISSION_PERMISSION_LIST, permissions);
    i->SetPackage(String("com.android.packageinstaller"));
    *intent = i;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode PackageManager::GetPackageArchiveInfo(
    /* [in] */ const String& archiveFilePath,
    /* [in] */ Int32 flags,
    /* [out] */ IPackageInfo** pkgInfo)
{
    VALIDATE_NOT_NULL(pkgInfo)
    *pkgInfo = NULL;

    AutoPtr<PackageParser> parser = new PackageParser();
    AutoPtr<IFile> apkFile;
    CFile::New(archiveFilePath, (IFile**)&apkFile);

    // try {
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(PackageParser::CERTIFICATE_BUFFER_SIZE);
    AutoPtr<PackageParser::Package> pkg;
    ECode ec = parser->ParseMonolithicPackage(apkFile, 0, buffer, (PackageParser::Package**)&pkg);
    FAIL_RETURN(ec)

    if ((flags & GET_SIGNATURES) != 0) {
        FAIL_RETURN(parser->CollectCertificates(pkg, 0, buffer))
        FAIL_RETURN(parser->CollectManifestDigest(pkg))
    }

    AutoPtr<PackageUserState> state = new PackageUserState();
    AutoPtr<IPackageInfo> pi = PackageParser::GeneratePackageInfo(pkg, NULL, flags, 0, 0, NULL, state);
    *pkgInfo = pi;
    REFCOUNT_ADD(*pkgInfo)
    return NOERROR;
    // } catch (PackageParserException e) {
    //     return null;
    // }
}

ECode PackageManager::GetPackageSizeInfo(
    /* [in] */ const String&  packageName,
    /* [in] */ IIPackageStatsObserver* observer)
{
    AutoPtr<IUserHandleHelper> helper;
    Int32 id;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&id);
    return GetPackageSizeInfo(packageName, id, observer);
}

ECode PackageManager::AddPreferredActivity(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* activity,
    /* [in] */ Int32 userId)
{
    Slogger::E(TAG, "Not implemented. Must override in a subclass.");
    return E_RUNTIME_EXCEPTION;
}

String PackageManager::GetDataDirForUser(
    /* [in] */ Int32 userId,
    /* [in] */ const String& packageName)
{
    // TODO: This should be shared with Installer's knowledge of user directory
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> file;
    env->GetDataDirectory((IFile**)&file);
    String strFile;
    file->ToString(&strFile);
    strFile.AppendFormat("/user/%d/%s", userId, packageName.string());
    return strFile;
}

ECode PackageManager::ReplacePreferredActivityAsUser(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* activity,
    /* [in] */ Int32 userId)
{
    // throw new RuntimeException("Not implemented. Must override in a subclass.");
    return E_RUNTIME_EXCEPTION;
}

/** {@hide} */
String PackageManager::InstallStatusToString(
    /* [in] */ Int32 status,
    /* [in] */ const String& msg)
{
    String str = InstallStatusToString(status);
    if (!msg.IsNull()) {
        StringBuilder sb(str);
        sb += ": ";
        sb += msg;
        return sb.ToString();
    }
    else {
        return str;
    }
}

/** {@hide} */
String PackageManager::InstallStatusToString(
    /* [in] */ Int32 status)
{
    switch (status) {
        case IPackageManager::INSTALL_SUCCEEDED: return String("INSTALL_SUCCEEDED");
        case IPackageManager::INSTALL_FAILED_ALREADY_EXISTS: return String("INSTALL_FAILED_ALREADY_EXISTS");
        case IPackageManager::INSTALL_FAILED_INVALID_APK: return String("INSTALL_FAILED_INVALID_APK");
        case IPackageManager::INSTALL_FAILED_INVALID_URI: return String("INSTALL_FAILED_INVALID_URI");
        case IPackageManager::INSTALL_FAILED_INSUFFICIENT_STORAGE: return String("INSTALL_FAILED_INSUFFICIENT_STORAGE");
        case IPackageManager::INSTALL_FAILED_DUPLICATE_PACKAGE: return String("INSTALL_FAILED_DUPLICATE_PACKAGE");
        case IPackageManager::INSTALL_FAILED_NO_SHARED_USER: return String("INSTALL_FAILED_NO_SHARED_USER");
        case IPackageManager::INSTALL_FAILED_UPDATE_INCOMPATIBLE: return String("INSTALL_FAILED_UPDATE_INCOMPATIBLE");
        case IPackageManager::INSTALL_FAILED_SHARED_USER_INCOMPATIBLE: return String("INSTALL_FAILED_SHARED_USER_INCOMPATIBLE");
        case IPackageManager::INSTALL_FAILED_MISSING_SHARED_LIBRARY: return String("INSTALL_FAILED_MISSING_SHARED_LIBRARY");
        case IPackageManager::INSTALL_FAILED_REPLACE_COULDNT_DELETE: return String("INSTALL_FAILED_REPLACE_COULDNT_DELETE");
        case IPackageManager::INSTALL_FAILED_DEXOPT: return String("INSTALL_FAILED_DEXOPT");
        case IPackageManager::INSTALL_FAILED_OLDER_SDK: return String("INSTALL_FAILED_OLDER_SDK");
        case IPackageManager::INSTALL_FAILED_CONFLICTING_PROVIDER: return String("INSTALL_FAILED_CONFLICTING_PROVIDER");
        case IPackageManager::INSTALL_FAILED_NEWER_SDK: return String("INSTALL_FAILED_NEWER_SDK");
        case IPackageManager::INSTALL_FAILED_TEST_ONLY: return String("INSTALL_FAILED_TEST_ONLY");
        case IPackageManager::INSTALL_FAILED_CPU_ABI_INCOMPATIBLE: return String("INSTALL_FAILED_CPU_ABI_INCOMPATIBLE");
        case IPackageManager::INSTALL_FAILED_MISSING_FEATURE: return String("INSTALL_FAILED_MISSING_FEATURE");
        case IPackageManager::INSTALL_FAILED_CONTAINER_ERROR: return String("INSTALL_FAILED_CONTAINER_ERROR");
        case IPackageManager::INSTALL_FAILED_INVALID_INSTALL_LOCATION: return String("INSTALL_FAILED_INVALID_INSTALL_LOCATION");
        case IPackageManager::INSTALL_FAILED_MEDIA_UNAVAILABLE: return String("INSTALL_FAILED_MEDIA_UNAVAILABLE");
        case IPackageManager::INSTALL_FAILED_VERIFICATION_TIMEOUT: return String("INSTALL_FAILED_VERIFICATION_TIMEOUT");
        case IPackageManager::INSTALL_FAILED_VERIFICATION_FAILURE: return String("INSTALL_FAILED_VERIFICATION_FAILURE");
        case IPackageManager::INSTALL_FAILED_PACKAGE_CHANGED: return String("INSTALL_FAILED_PACKAGE_CHANGED");
        case IPackageManager::INSTALL_FAILED_UID_CHANGED: return String("INSTALL_FAILED_UID_CHANGED");
        case IPackageManager::INSTALL_FAILED_VERSION_DOWNGRADE: return String("INSTALL_FAILED_VERSION_DOWNGRADE");
        case IPackageManager::INSTALL_PARSE_FAILED_NOT_APK: return String("INSTALL_PARSE_FAILED_NOT_APK");
        case IPackageManager::INSTALL_PARSE_FAILED_BAD_MANIFEST: return String("INSTALL_PARSE_FAILED_BAD_MANIFEST");
        case IPackageManager::INSTALL_PARSE_FAILED_UNEXPECTED_EXCEPTION: return String("INSTALL_PARSE_FAILED_UNEXPECTED_EXCEPTION");
        case IPackageManager::INSTALL_PARSE_FAILED_NO_CERTIFICATES: return String("INSTALL_PARSE_FAILED_NO_CERTIFICATES");
        case IPackageManager::INSTALL_PARSE_FAILED_INCONSISTENT_CERTIFICATES: return String("INSTALL_PARSE_FAILED_INCONSISTENT_CERTIFICATES");
        case IPackageManager::INSTALL_PARSE_FAILED_CERTIFICATE_ENCODING: return String("INSTALL_PARSE_FAILED_CERTIFICATE_ENCODING");
        case IPackageManager::INSTALL_PARSE_FAILED_BAD_PACKAGE_NAME: return String("INSTALL_PARSE_FAILED_BAD_PACKAGE_NAME");
        case IPackageManager::INSTALL_PARSE_FAILED_BAD_SHARED_USER_ID: return String("INSTALL_PARSE_FAILED_BAD_SHARED_USER_ID");
        case IPackageManager::INSTALL_PARSE_FAILED_MANIFEST_MALFORMED: return String("INSTALL_PARSE_FAILED_MANIFEST_MALFORMED");
        case IPackageManager::INSTALL_PARSE_FAILED_MANIFEST_EMPTY: return String("INSTALL_PARSE_FAILED_MANIFEST_EMPTY");
        case IPackageManager::INSTALL_FAILED_INTERNAL_ERROR: return String("INSTALL_FAILED_INTERNAL_ERROR");
        case IPackageManager::INSTALL_FAILED_USER_RESTRICTED: return String("INSTALL_FAILED_USER_RESTRICTED");
        case IPackageManager::INSTALL_FAILED_DUPLICATE_PERMISSION: return String("INSTALL_FAILED_DUPLICATE_PERMISSION");
        case IPackageManager::INSTALL_FAILED_NO_MATCHING_ABIS: return String("INSTALL_FAILED_NO_MATCHING_ABIS");
        case IPackageManager::INSTALL_FAILED_ABORTED: return String("INSTALL_FAILED_ABORTED");
        default: return StringUtils::ToString(status);
    }
}

/** {@hide} */
Int32 PackageManager::InstallStatusToPublicStatus(
    /* [in] */ Int32 status)
{
    switch (status) {
        case IPackageManager::INSTALL_SUCCEEDED: return IPackageInstaller::STATUS_SUCCESS;
        case IPackageManager::INSTALL_FAILED_ALREADY_EXISTS: return IPackageInstaller::STATUS_FAILURE_CONFLICT;
        case IPackageManager::INSTALL_FAILED_INVALID_APK: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_FAILED_INVALID_URI: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_FAILED_INSUFFICIENT_STORAGE: return IPackageInstaller::STATUS_FAILURE_STORAGE;
        case IPackageManager::INSTALL_FAILED_DUPLICATE_PACKAGE: return IPackageInstaller::STATUS_FAILURE_CONFLICT;
        case IPackageManager::INSTALL_FAILED_NO_SHARED_USER: return IPackageInstaller::STATUS_FAILURE_CONFLICT;
        case IPackageManager::INSTALL_FAILED_UPDATE_INCOMPATIBLE: return IPackageInstaller::STATUS_FAILURE_CONFLICT;
        case IPackageManager::INSTALL_FAILED_SHARED_USER_INCOMPATIBLE: return IPackageInstaller::STATUS_FAILURE_CONFLICT;
        case IPackageManager::INSTALL_FAILED_MISSING_SHARED_LIBRARY: return IPackageInstaller::STATUS_FAILURE_INCOMPATIBLE;
        case IPackageManager::INSTALL_FAILED_REPLACE_COULDNT_DELETE: return IPackageInstaller::STATUS_FAILURE_CONFLICT;
        case IPackageManager::INSTALL_FAILED_DEXOPT: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_FAILED_OLDER_SDK: return IPackageInstaller::STATUS_FAILURE_INCOMPATIBLE;
        case IPackageManager::INSTALL_FAILED_CONFLICTING_PROVIDER: return IPackageInstaller::STATUS_FAILURE_CONFLICT;
        case IPackageManager::INSTALL_FAILED_NEWER_SDK: return IPackageInstaller::STATUS_FAILURE_INCOMPATIBLE;
        case IPackageManager::INSTALL_FAILED_TEST_ONLY: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_FAILED_CPU_ABI_INCOMPATIBLE: return IPackageInstaller::STATUS_FAILURE_INCOMPATIBLE;
        case IPackageManager::INSTALL_FAILED_MISSING_FEATURE: return IPackageInstaller::STATUS_FAILURE_INCOMPATIBLE;
        case IPackageManager::INSTALL_FAILED_CONTAINER_ERROR: return IPackageInstaller::STATUS_FAILURE_STORAGE;
        case IPackageManager::INSTALL_FAILED_INVALID_INSTALL_LOCATION: return IPackageInstaller::STATUS_FAILURE_STORAGE;
        case IPackageManager::INSTALL_FAILED_MEDIA_UNAVAILABLE: return IPackageInstaller::STATUS_FAILURE_STORAGE;
        case IPackageManager::INSTALL_FAILED_VERIFICATION_TIMEOUT: return IPackageInstaller::STATUS_FAILURE_ABORTED;
        case IPackageManager::INSTALL_FAILED_VERIFICATION_FAILURE: return IPackageInstaller::STATUS_FAILURE_ABORTED;
        case IPackageManager::INSTALL_FAILED_PACKAGE_CHANGED: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_FAILED_UID_CHANGED: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_FAILED_VERSION_DOWNGRADE: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_NOT_APK: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_BAD_MANIFEST: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_UNEXPECTED_EXCEPTION: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_NO_CERTIFICATES: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_INCONSISTENT_CERTIFICATES: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_CERTIFICATE_ENCODING: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_BAD_PACKAGE_NAME: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_BAD_SHARED_USER_ID: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_MANIFEST_MALFORMED: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_PARSE_FAILED_MANIFEST_EMPTY: return IPackageInstaller::STATUS_FAILURE_INVALID;
        case IPackageManager::INSTALL_FAILED_INTERNAL_ERROR: return IPackageInstaller::STATUS_FAILURE;
        case IPackageManager::INSTALL_FAILED_USER_RESTRICTED: return IPackageInstaller::STATUS_FAILURE_INCOMPATIBLE;
        case IPackageManager::INSTALL_FAILED_DUPLICATE_PERMISSION: return IPackageInstaller::STATUS_FAILURE_CONFLICT;
        case IPackageManager::INSTALL_FAILED_NO_MATCHING_ABIS: return IPackageInstaller::STATUS_FAILURE_INCOMPATIBLE;
        case IPackageManager::INSTALL_FAILED_ABORTED: return IPackageInstaller::STATUS_FAILURE_ABORTED;
        default: return IPackageInstaller::STATUS_FAILURE;
    }
}

/** {@hide} */
String PackageManager::DeleteStatusToString(
    /* [in] */ Int32 status,
    /* [in] */ const String& msg)
{
    String str = DeleteStatusToString(status);
    if (!msg.IsNull()) {
        StringBuilder sb(str);
        sb += ": ";
        sb += msg;
        return sb.ToString();
    }
    else {
        return str;
    }
}

/** {@hide} */
String PackageManager::DeleteStatusToString(
    /* [in] */ Int32 status)
{
    switch (status) {
        case IPackageManager::DELETE_SUCCEEDED: return String("DELETE_SUCCEEDED");
        case IPackageManager::DELETE_FAILED_INTERNAL_ERROR: return String("DELETE_FAILED_INTERNAL_ERROR");
        case IPackageManager::DELETE_FAILED_DEVICE_POLICY_MANAGER: return String("DELETE_FAILED_DEVICE_POLICY_MANAGER");
        case IPackageManager::DELETE_FAILED_USER_RESTRICTED: return String("DELETE_FAILED_USER_RESTRICTED");
        case IPackageManager::DELETE_FAILED_OWNER_BLOCKED: return String("DELETE_FAILED_OWNER_BLOCKED");
        case IPackageManager::DELETE_FAILED_ABORTED: return String("DELETE_FAILED_ABORTED");
        default: return StringUtils::ToString(status);
    }
}

/** {@hide} */
Int32 PackageManager::DeleteStatusToPublicStatus(
    /* [in] */ Int32 status)
{
    switch (status) {
        case IPackageManager::DELETE_SUCCEEDED: return IPackageInstaller::STATUS_SUCCESS;
        case IPackageManager::DELETE_FAILED_INTERNAL_ERROR: return IPackageInstaller::STATUS_FAILURE;
        case IPackageManager::DELETE_FAILED_DEVICE_POLICY_MANAGER: return IPackageInstaller::STATUS_FAILURE_BLOCKED;
        case IPackageManager::DELETE_FAILED_USER_RESTRICTED: return IPackageInstaller::STATUS_FAILURE_BLOCKED;
        case IPackageManager::DELETE_FAILED_OWNER_BLOCKED: return IPackageInstaller::STATUS_FAILURE_BLOCKED;
        case IPackageManager::DELETE_FAILED_ABORTED: return IPackageInstaller::STATUS_FAILURE_ABORTED;
        default: return IPackageInstaller::STATUS_FAILURE;
    }
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
