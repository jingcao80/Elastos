
#include "elastos/droid/server/devicepolicy/DeviceOwner.h"
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/app/AppGlobals.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace DevicePolicy {

//=============================================================================
// DeviceOwner::OwnerInfo
//=============================================================================
ECode DeviceOwner::OwnerInfo::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& packageName)
{
    mName = name;
    mPackageName = packageName;
    CComponentName::New(packageName, String(""), (IComponentName**)&mAdmin);
    return NOERROR;
}

ECode DeviceOwner::OwnerInfo::constructor(
    /* [in] */ const String& name,
    /* [in] */ IComponentName* admin)
{
    mName = name;
    mAdmin = admin;
    admin->GetPackageName(&mPackageName);
    return NOERROR;
}

ECode DeviceOwner::OwnerInfo::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mName;
    return NOERROR;
}

ECode DeviceOwner::OwnerInfo::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode DeviceOwner::OwnerInfo::GetPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPackageName;
    return NOERROR;
}

ECode DeviceOwner::OwnerInfo::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode DeviceOwner::OwnerInfo::GetAdmin(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mAdmin;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DeviceOwner::OwnerInfo::SetAdmin(
    /* [in] */ IComponentName* admin)
{
    mAdmin = admin;
    return NOERROR;
}

//=============================================================================
// DeviceOwner
//=============================================================================
const String DeviceOwner::TAG("DevicePolicyManagerService");
const String DeviceOwner::DEVICE_OWNER_XML("device_owner.xml");
const String DeviceOwner::TAG_DEVICE_OWNER("device-owner");
const String DeviceOwner::TAG_PROFILE_OWNER("profile-owner");
const String DeviceOwner::ATTR_NAME("name");
const String DeviceOwner::ATTR_PACKAGE("package");
const String DeviceOwner::ATTR_COMPONENT_NAME("component");
const String DeviceOwner::ATTR_USERID("userId");

DeviceOwner::DeviceOwner()
{
    CHashMap::New((IHashMap**)&mProfileOwners);
}

ECode DeviceOwner::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ IOutputStream* out)
{
    mInputStreamForTest = in;
    mOutputStreamForTest = out;
    return NOERROR;
}

ECode DeviceOwner::Load(
    /* [out] */ DeviceOwner** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<DeviceOwner> owner = new DeviceOwner();
    AutoPtr<IFile> file;
    CFile::New(Environment::GetSystemSecureDirectory(), DEVICE_OWNER_XML, (IFile**)&file);
    Boolean isExists;
    file->Exists(&isExists);
    if (isExists) {
        owner->ReadOwnerFile();
        *result = owner;
        REFCOUNT_ADD(*result)
        return NOERROR;
    } else {
        *result = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode DeviceOwner::CreateWithDeviceOwner(
    /* [in] */ const String& packageName,
    /* [in] */ const String& ownerName,
    /* [out] */ DeviceOwner** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<DeviceOwner> owner = new DeviceOwner();
    owner->mDeviceOwner = new OwnerInfo();
    owner->mDeviceOwner->constructor(ownerName, packageName);
    *result = owner;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DeviceOwner::CreateWithProfileOwner(
    /* [in] */ const String& packageName,
    /* [in] */ const String& ownerName,
    /* [in] */ Int32 userId,
    /* [out] */ DeviceOwner** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<DeviceOwner> owner = new DeviceOwner();
    AutoPtr<OwnerInfo> ownerInfo = new OwnerInfo();
    ownerInfo->constructor(ownerName, packageName);
    owner->mProfileOwners->Put(CoreUtils::Convert(userId), TO_IINTERFACE(ownerInfo));
    *result = owner;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DeviceOwner::CreateWithProfileOwner(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& ownerName,
    /* [in] */ Int32 userId,
    /* [out] */ DeviceOwner** result)
{
    AutoPtr<DeviceOwner> owner = new DeviceOwner();
    AutoPtr<OwnerInfo> ownerInfo = new OwnerInfo();
    ownerInfo->constructor(ownerName, admin);
    owner->mProfileOwners->Put(CoreUtils::Convert(userId), TO_IINTERFACE(ownerInfo));
    *result = owner;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DeviceOwner::GetDeviceOwnerPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mDeviceOwner != NULL) {
        String packageName;
        mDeviceOwner->GetPackageName(&packageName);
        *result =  packageName;
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode DeviceOwner::GetDeviceOwnerName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mDeviceOwner != NULL) {
        String name;
        mDeviceOwner->GetName(&name);
        *result = name;
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode DeviceOwner::SetDeviceOwner(
    /* [in] */ const String& packageName,
    /* [in] */ const String& ownerName)
{
    mDeviceOwner = new OwnerInfo();
    mDeviceOwner->constructor(ownerName, packageName);
    return NOERROR;
}

ECode DeviceOwner::ClearDeviceOwner()
{
    mDeviceOwner = NULL;
    return NOERROR;
}

ECode DeviceOwner::SetProfileOwner(
    /* [in] */ const String& packageName,
    /* [in] */ const String& ownerName,
    /* [in] */ Int32 userId)
{
    AutoPtr<OwnerInfo> ownerInfo = new OwnerInfo();
    ownerInfo->constructor(ownerName, packageName);
    mProfileOwners->Put(CoreUtils::Convert(userId), TO_IINTERFACE(ownerInfo));
    return NOERROR;
}

ECode DeviceOwner::SetProfileOwner(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& ownerName,
    /* [in] */ Int32 userId)
{
    AutoPtr<OwnerInfo> ownerInfo = new OwnerInfo();
    ownerInfo->constructor(ownerName, admin);
    mProfileOwners->Put(CoreUtils::Convert(userId), TO_IINTERFACE(ownerInfo));
    return NOERROR;
}

ECode DeviceOwner::RemoveProfileOwner(
    /* [in] */ Int32 userId)
{
    mProfileOwners->Remove(CoreUtils::Convert(userId));
    return NOERROR;
}

ECode DeviceOwner::GetProfileOwnerPackageName(
    /* [in] */ Int32 userId,
    /* [out] */ String* result)
{
    AutoPtr<IInterface> obj;
    mProfileOwners->Get(CoreUtils::Convert(userId), (IInterface**)&obj);
    AutoPtr<OwnerInfo> profileOwner = (OwnerInfo*) IObject::Probe(obj);
    if (profileOwner != NULL) {
        String packageName;
        profileOwner->GetPackageName(&packageName);
        *result = packageName;
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode DeviceOwner::GetProfileOwnerComponent(
    /* [in] */ Int32 userId,
    /* [out] */ IComponentName** result)
{
    AutoPtr<IInterface> obj;
    mProfileOwners->Get(CoreUtils::Convert(userId), (IInterface**)&obj);
    AutoPtr<OwnerInfo> profileOwner = (OwnerInfo*) IObject::Probe(obj);
    AutoPtr<IComponentName> admin;
    profileOwner->GetAdmin((IComponentName**)&admin);
    *result = profileOwner != NULL ? admin : NULL;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DeviceOwner::GetProfileOwnerName(
    /* [in] */ Int32 userId,
    /* [out] */ String* result)
{
    AutoPtr<IInterface> obj;
    mProfileOwners->Get(CoreUtils::Convert(userId), (IInterface**)&obj);
    AutoPtr<OwnerInfo> profileOwner = (OwnerInfo*) IObject::Probe(obj);
    String name;
    profileOwner->GetName(&name);
    *result = profileOwner != NULL ? name : String(NULL);
    return NOERROR;
}

ECode DeviceOwner::GetProfileOwnerKeys(
    /* [out] */ ISet** result)
{
    return mProfileOwners->GetKeySet(result);
}

ECode DeviceOwner::HasDeviceOwner(
    /* [out] */ Boolean* result)
{
    *result = mDeviceOwner != NULL;
    return NOERROR;
}

ECode DeviceOwner::IsInstalled(
    /* [in] */ const String& packageName,
    /* [in] */ IPackageManager* pm,
    /* [out] */ Boolean* result)
{
    // try {
    ECode ec = NOERROR;
    do {
        AutoPtr<IPackageInfo> pi;
        ec = pm->GetPackageInfo(packageName, 0, (IPackageInfo**)&pi);
        if (FAILED(ec)) break;
        if (pi != NULL) {
            AutoPtr<IApplicationInfo> info;
            ec = pi->GetApplicationInfo((IApplicationInfo**)&info);
            if (FAILED(ec)) break;
            Int32 flags;
            ec = info->GetFlags(&flags);
            if (FAILED(ec)) break;
            if (flags != 0) {
                *result = TRUE;
                return NOERROR;
            }
        }
    } while(FALSE);
    // } catch (NameNotFoundException nnfe) {
    if (FAILED(ec)) {
        if ((ECode) E_NAME_NOT_FOUND_EXCEPTION == ec) {
            Slogger::W(TAG, "Device Owner package %s not installed.", packageName.string());
        }
        else
            return ec;
    }
    // }
    *result = FALSE;
    return NOERROR;
}

ECode DeviceOwner::IsInstalledForUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    // try {
    ECode ec = NOERROR;
    do {
        AutoPtr<IPackageInfo> pi;
        ec = AppGlobals::GetPackageManager()->GetPackageInfo(packageName, 0, userHandle, (IPackageInfo**)&pi);
        if (FAILED(ec)) break;
        AutoPtr<IApplicationInfo> appInfo;
        ec = pi->GetApplicationInfo((IApplicationInfo**)&appInfo);
        if (FAILED(ec)) break;
        Int32 flags;
        ec = appInfo->GetFlags(&flags);
        if (FAILED(ec)) break;
        if (pi != NULL && flags != 0) {
            *result = TRUE;
            return NOERROR;
        }
    } while(FALSE);
    // } catch (RemoteException re) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "Package manager has died %d", ec);
            return E_RUNTIME_EXCEPTION;
        }
        return ec;
    }
    // }
    *result = FALSE;
    return NOERROR;
}

ECode DeviceOwner::ReadOwnerFile()
{
    // try {
    ECode ec = NOERROR;
    do {
        AutoPtr<IInputStream> input;
        ec = OpenRead((IInputStream**)&input);
        if (FAILED(ec)) break;
        AutoPtr<IXmlPullParser> parser;
        ec = Xml::NewPullParser((IXmlPullParser**)&parser);
        if (FAILED(ec)) break;
        ec = parser->SetInput(input, String(NULL));
        if (FAILED(ec)) break;
        Int32 type;
        for (;;) {
            ec = parser->Next(&type);
            if (FAILED(ec)) break;
            if (type == IXmlPullParser::END_DOCUMENT) break;
            if (type != IXmlPullParser::START_TAG) {
                continue;
            }
            String tag;
            parser->GetName(&tag);
            if (FAILED(ec)) break;
            if (tag.Equals(TAG_DEVICE_OWNER)) {
                String name;
                parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
                String packageName;
                parser->GetAttributeValue(String(NULL), ATTR_PACKAGE, &packageName);
                mDeviceOwner = new OwnerInfo();
                mDeviceOwner->constructor(name, packageName);
            } else if (tag.Equals(TAG_PROFILE_OWNER)) {
                String profileOwnerPackageName;
                ec = parser->GetAttributeValue(String(NULL), ATTR_PACKAGE, &profileOwnerPackageName);
                if (FAILED(ec)) break;
                String profileOwnerName;
                ec = parser->GetAttributeValue(String(NULL), ATTR_NAME, &profileOwnerName);
                if (FAILED(ec)) break;
                String profileOwnerComponentStr;
                ec = parser->GetAttributeValue(String(NULL), ATTR_COMPONENT_NAME, &profileOwnerComponentStr);
                if (FAILED(ec)) break;
                String attributeValue;
                ec = parser->GetAttributeValue(String(NULL), ATTR_USERID, &attributeValue);
                if (FAILED(ec)) break;
                Int32 userId = StringUtils::ParseInt32(attributeValue);
                AutoPtr<OwnerInfo> profileOwnerInfo;
                if (profileOwnerComponentStr != NULL) {
                    AutoPtr<IComponentNameHelper> helper;
                    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
                    AutoPtr<IComponentName> admin;
                    ec = helper->UnflattenFromString(
                            profileOwnerComponentStr, (IComponentName**)&admin);
                    if (FAILED(ec)) break;
                    if (admin != NULL) {
                        profileOwnerInfo = new OwnerInfo();
                        profileOwnerInfo->constructor(profileOwnerName, admin);
                    } else {
                        // This shouldn't happen but switch from package name -> component name
                        // might have written bad device owner files. b/17652534
                        Slogger::E(TAG, "Error parsing device-owner file. Bad component name %s",
                                profileOwnerComponentStr.string());
                    }
                }
                if (profileOwnerInfo == NULL) {
                    profileOwnerInfo = new OwnerInfo();
                    profileOwnerInfo->constructor(profileOwnerName, profileOwnerPackageName);
                }
                ec = mProfileOwners->Put(CoreUtils::Convert(userId), TO_IINTERFACE(profileOwnerInfo));
                if (FAILED(ec)) break;
            } else {
                Slogger::E("Unexpected tag in device owner file: %s", tag.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }
        if (FAILED(ec)) break;
        ec = input->Close();
    } while(FALSE);
    if (FAILED(ec)) {
    // } catch (XmlPullParserException xppe) {
        if ((ECode) E_XML_PULL_PARSER_EXCEPTION == ec) {
            Slogger::E(TAG, "Error parsing device-owner file\n%d", ec);
        }
    // } catch (IOException ioe) {
        else if ((ECode) E_IO_EXCEPTION == ec) {
            Slogger::E(TAG, "IO Exception when reading device-owner file\n%d", ec);
        }
        else
            return ec;
    // }
    }
    return NOERROR;
}

ECode DeviceOwner::WriteOwnerFile()
{
    {    AutoLock syncLock(this);
        WriteOwnerFileLocked();
    }
    return NOERROR;
}

ECode DeviceOwner::WriteOwnerFileLocked()
{
    // try {
    ECode ec = NOERROR;
    do {
        AutoPtr<IOutputStream> outputStream;
        ec = StartWrite((IOutputStream**)&outputStream);
        if (FAILED(ec)) break;
        AutoPtr<IXmlSerializer> out;
        CFastXmlSerializer::New((IXmlSerializer**)&out);
        out->SetOutput(outputStream, String("utf-8"));
        out->StartDocument(String(NULL), TRUE);
        // Write device owner tag
        if (mDeviceOwner != NULL) {
            out->WriteStartTag(String(NULL), TAG_DEVICE_OWNER);
            String packageName;
            mDeviceOwner->GetPackageName(&packageName);
            out->WriteAttribute(String(NULL), ATTR_PACKAGE, packageName);
            String name;
            mDeviceOwner->GetName(&name);
            if (name != NULL) {
                out->WriteAttribute(String(NULL), ATTR_NAME, name);
            }
            out->WriteEndTag(String(NULL), TAG_DEVICE_OWNER);
        }
        // Write profile owner tags
        Int32 size;
        mProfileOwners->GetSize(&size);
        if (size > 0) {
            AutoPtr<ISet> entrySet;
            mProfileOwners->GetEntrySet((ISet**)&entrySet);
            AutoPtr<IIterator> it;
            ICollection::Probe(entrySet)->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IMapEntry> owner = IMapEntry::Probe(obj);
                out->WriteStartTag(String(NULL), TAG_PROFILE_OWNER);
                obj = NULL;
                owner->GetValue((IInterface**)&obj);
                AutoPtr<OwnerInfo> ownerInfo = (OwnerInfo*) IObject::Probe(obj);
                String packageName;
                ownerInfo->GetPackageName(&packageName);
                out->WriteAttribute(String(NULL), ATTR_PACKAGE, packageName);
                String name;
                ownerInfo->GetName(&name);
                out->WriteAttribute(String(NULL), ATTR_NAME, name);
                obj = NULL;
                owner->GetKey((IInterface**)&obj);
                out->WriteAttribute(String(NULL), ATTR_USERID, Object::ToString(obj));
                AutoPtr<IComponentName> admin;
                ownerInfo->GetAdmin((IComponentName**)&admin);
                if (admin != NULL) {
                    String flattenName;
                    admin->FlattenToString(&flattenName);
                    out->WriteAttribute(String(NULL), ATTR_COMPONENT_NAME, flattenName);
                }
                out->WriteEndTag(String(NULL), TAG_PROFILE_OWNER);
            }
        }
        out->EndDocument();
        out->Flush();
        FinishWrite(outputStream);
    } while(FALSE);
    // } catch (IOException ioe) {
    if (FAILED(ec)) {
        if ((ECode) E_IO_EXCEPTION == ec) {
            Slogger::E(TAG, "IO Exception when writing device-owner file\n%d", ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode DeviceOwner::OpenRead(
    /* [out] */ IInputStream** result)
{
    VALIDATE_NOT_NULL(result)

    if (mInputStreamForTest != NULL) {
        *result = mInputStreamForTest;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<IFile> file;
    CFile::New(Environment::GetSystemSecureDirectory(),
            DEVICE_OWNER_XML, (IFile**)&file);
    AutoPtr<IAtomicFile> atomicFile;
    CAtomicFile::New(file, (IAtomicFile**)&atomicFile);
    AutoPtr<IFileInputStream> fileInputStream;
    atomicFile->OpenRead((IFileInputStream**)&fileInputStream);
    *result = IInputStream::Probe(fileInputStream);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DeviceOwner::StartWrite(
    /* [out] */ IOutputStream** result)
{
    VALIDATE_NOT_NULL(result)

    if (mOutputStreamForTest != NULL) {
        *result = mOutputStreamForTest;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<IFile> file;
    CFile::New(Environment::GetSystemSecureDirectory(),
            DEVICE_OWNER_XML, (IFile**)&file);
    CAtomicFile::New(file, (IAtomicFile**)&mFileForWriting);
    AutoPtr<IFileOutputStream> fileOutputStream;
    mFileForWriting->StartWrite((IFileOutputStream**)&fileOutputStream);
    *result = IOutputStream::Probe(fileOutputStream);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DeviceOwner::FinishWrite(
    /* [in] */ IOutputStream* stream)
{
    if (mFileForWriting != NULL) {
        mFileForWriting->FinishWrite(IFileOutputStream::Probe(stream));
    }
    return NOERROR;
}

} // namespace DevicePolicy
} // namespace Server
} // namespace Droid
} // namespace Elastos
