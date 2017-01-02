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

#ifndef __ELASTOS_DROID_SERVER_DEVICEPOLICY_DEVICEOWNER_H__
#define __ELASTOS_DROID_SERVER_DEVICEPOLICY_DEVICEOWNER_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace DevicePolicy {

/**
 * Stores and restores state for the Device and Profile owners. By definition there can be
 * only one device owner, but there may be a profile owner for each user.
 */
class DeviceOwner
    : public Object
{
public:
    class OwnerInfo
        : public Object
    {
    public:
        CARAPI constructor(
            /* [in] */ const String& name,
            /* [in] */ const String& packageName);

        CARAPI constructor(
            /* [in] */ const String& name,
            /* [in] */ IComponentName* admin);

        CARAPI GetName(
            /* [out] */ String* result);

        CARAPI SetName(
            /* [in] */ const String& name);

        CARAPI GetPackageName(
            /* [out] */ String* result);

        CARAPI SetPackageName(
            /* [in] */ const String& packageName);

        CARAPI GetAdmin(
            /* [out] */ IComponentName** result);

        CARAPI SetAdmin(
            /* [in] */ IComponentName* admin);

    private:
        String mName;

        String mPackageName;

        AutoPtr<IComponentName> mAdmin;
    };

public:
    // @VisibleForTesting
    CARAPI constructor(
        /* [in] */ IInputStream* in,
        /* [in] */ IOutputStream* out);

    /**
     * Loads the device owner state from disk.
     */
    static CARAPI Load(
        /* [out] */ DeviceOwner** result);

    /**
     * Creates an instance of the device owner object with the device owner set.
     */
    static CARAPI CreateWithDeviceOwner(
        /* [in] */ const String& packageName,
        /* [in] */ const String& ownerName,
        /* [out] */ DeviceOwner** result);

    /**
     * @deprecated Use a component name instead of package name
     * Creates an instance of the device owner object with the profile owner set.
     */
    static CARAPI CreateWithProfileOwner(
        /* [in] */ const String& packageName,
        /* [in] */ const String& ownerName,
        /* [in] */ Int32 userId,
        /* [out] */ DeviceOwner** result);

    /**
     * Creates an instance of the device owner object with the profile owner set.
     */
    static CARAPI CreateWithProfileOwner(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& ownerName,
        /* [in] */ Int32 userId,
        /* [out] */ DeviceOwner** result);

    CARAPI GetDeviceOwnerPackageName(
        /* [out] */ String* result);

    CARAPI GetDeviceOwnerName(
        /* [out] */ String* result);

    CARAPI SetDeviceOwner(
        /* [in] */ const String& packageName,
        /* [in] */ const String& ownerName);

    CARAPI ClearDeviceOwner();

    /**
     * @deprecated
     */
    CARAPI SetProfileOwner(
        /* [in] */ const String& packageName,
        /* [in] */ const String& ownerName,
        /* [in] */ Int32 userId);

    CARAPI SetProfileOwner(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& ownerName,
        /* [in] */ Int32 userId);

    CARAPI RemoveProfileOwner(
        /* [in] */ Int32 userId);

    /**
     * @deprecated Use getProfileOwnerComponent
     * @param userId
     * @return
     */
    CARAPI GetProfileOwnerPackageName(
        /* [in] */ Int32 userId,
        /* [out] */ String* result);

    CARAPI GetProfileOwnerComponent(
        /* [in] */ Int32 userId,
        /* [out] */ IComponentName** result);

    CARAPI GetProfileOwnerName(
        /* [in] */ Int32 userId,
        /* [out] */ String* result);

    CARAPI GetProfileOwnerKeys(
        /* [out] */ ISet** result);

    CARAPI HasDeviceOwner(
        /* [out] */ Boolean* result);

    static CARAPI IsInstalled(
        /* [in] */ const String& packageName,
        /* [in] */ IPackageManager* pm,
        /* [out] */ Boolean* result);

    static CARAPI IsInstalledForUser(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    CARAPI ReadOwnerFile();

    CARAPI WriteOwnerFile();

private:
    // Private default constructor.
    DeviceOwner();

    CARAPI WriteOwnerFileLocked();

    CARAPI OpenRead(
        /* [out] */ IInputStream** result);

    CARAPI StartWrite(
        /* [out] */ IOutputStream** result);

    CARAPI FinishWrite(
        /* [in] */ IOutputStream* stream);

private:
    static const String TAG;

    static const String DEVICE_OWNER_XML;

    static const String TAG_DEVICE_OWNER;

    static const String TAG_PROFILE_OWNER;

    static const String ATTR_NAME;

    static const String ATTR_PACKAGE;

    static const String ATTR_COMPONENT_NAME;

    static const String ATTR_USERID;

    AutoPtr<IAtomicFile> mFileForWriting;

    // Input/Output streams for testing.
    AutoPtr<IInputStream> mInputStreamForTest;

    AutoPtr<IOutputStream> mOutputStreamForTest;

    // Internal state for the device owner package.
    AutoPtr<OwnerInfo> mDeviceOwner;

    // Internal state for the profile owner packages.
    AutoPtr<IHashMap> mProfileOwners;
};

} // namespace DevicePolicy
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_DEVICEPOLICY_DEVICEOWNER_H__
