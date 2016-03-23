#include "elastos/droid/server/SystemConfig.h"
#include <elastos/droid/os/Process.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/droid/internal/utility/ArrayUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Content::Pm::CFeatureInfo;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IReader;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Server {

const String SystemConfig::TAG("SystemConfig");
Object SystemConfig::sLock;

AutoPtr<SystemConfig> SystemConfig::sInstance;


AutoPtr<SystemConfig> SystemConfig::GetInstance()
{
    synchronized(sLock) {
        if (sInstance == NULL) {
            sInstance = new SystemConfig();
        }
    }
    return sInstance;
}

AutoPtr<ArrayOf<Int32> > SystemConfig::GetGlobalGids()
{
    return mGlobalGids;
}

AutoPtr< HashMap<Int32, AutoPtr<HashSet<String> > > > SystemConfig::GetSystemPermissions()
{
    return mSystemPermissions;
}

AutoPtr<HashMap<String, String> > SystemConfig::GetSharedLibraries() {
    return mSharedLibraries;
}

AutoPtr<HashMap<String, AutoPtr<IFeatureInfo> > > SystemConfig::GetAvailableFeatures()
{
    return mAvailableFeatures;
}

AutoPtr<HashMap<String, AutoPtr<SystemConfig::PermissionEntry> > > SystemConfig::GetPermissions()
{
    return mPermissions;
}

AutoPtr<HashSet<String> > SystemConfig::GetAllowInPowerSave()
{
    return mAllowInPowerSave;
}

AutoPtr<HashSet<String> > SystemConfig::GetFixedImeApps()
{
    return mFixedImeApps;
}

AutoPtr< HashMap<AutoPtr<ISignature>, AutoPtr<HashSet<String> > > > SystemConfig::GetSignatureAllowances()
{
    assert(0);
    return NULL;
}

SystemConfig::SystemConfig()
{
    mSystemPermissions = new HashMap<Int32, AutoPtr<HashSet<String> > >();
    mSharedLibraries = new HashMap<String, String>();
    mAvailableFeatures = new HashMap<String, AutoPtr<IFeatureInfo> >();
    mPermissions = new HashMap<String, AutoPtr<PermissionEntry> >();
    mAllowInPowerSave = new HashSet<String>();
    mFixedImeApps = new HashSet<String>();

    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);

    AutoPtr<IFile> rootDir, oemDir;
    environment->GetRootDirectory((IFile**)&rootDir);
    environment->GetOemDirectory((IFile**)&oemDir);

    // Read configuration from system
    AutoPtr< ArrayOf<String> > segments = ArrayOf<String>::Alloc(2);
    segments->Set(0, String("etc"));
    segments->Set(1, String("sysconfig"));

    AutoPtr<IFile> path;
    environment->BuildPath(rootDir, segments, (IFile**)&path);
    ReadPermissions(path, FALSE);

    // Read configuration from the old permissions dir
    path = NULL;
    segments->Set(1, String("permissions"));
    environment->BuildPath(rootDir, segments, (IFile**)&path);
    ReadPermissions(path, FALSE);


    // Only read features from OEM config
    path = NULL;
    segments->Set(1, String("sysconfig"));
    environment->BuildPath(oemDir, segments, (IFile**)&path);
    ReadPermissions(path, TRUE);

    path = NULL;
    segments->Set(1, String("permissions"));
    environment->BuildPath(oemDir, segments, (IFile**)&path);
    ReadPermissions(path, TRUE);
}

ECode SystemConfig::ReadPermissions(
    /* [in] */ IFile* libraryDir,
    /* [in] */ Boolean onlyFeatures)
{
    Boolean exists, isDir;
    libraryDir->Exists(&exists);
    libraryDir->IsDirectory(&isDir);

    // Read permissions from given directory.
    if (!exists || !isDir) {
        if (!onlyFeatures) {
            Logger::W(TAG, "No directory %s, skipping", TO_CSTR(libraryDir));
        }
        return NOERROR;
    }

    Boolean canRead;
    libraryDir->CanRead(&canRead);
    if (!canRead) {
        Logger::W(TAG, "Directory %s cannot be read", TO_CSTR(libraryDir));
        return NOERROR;
    }

    // Iterate over the files in the directory and scan .xml files
    AutoPtr<ArrayOf<IFile*> > files;
    libraryDir->ListFiles((ArrayOf<IFile*>**)&files);

    if (files != NULL) {
        IFile* f;
        String path;
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            f = (*files)[i];
            f->GetPath(&path);
            // We'll read platform.xml last
            if (path.EndWith("etc/permissions/platform.xml")) {
                continue;
            }

            if (!path.EndWith(".xml")) {
                Logger::I(TAG, "Non-xml file %s in %s directory, ignoring",
                    TO_CSTR(f),
                    TO_CSTR(libraryDir));
                continue;
            }
            f->CanRead(&canRead);
            if (!canRead) {
                Logger::W(TAG, "Permissions library file %s cannot be read",
                    TO_CSTR(f));
                continue;
            }

            ReadPermissionsFromXml(f, onlyFeatures);
        }
    }

    // Read permissions from .../etc/permissions/platform.xml last so it will take precedence

    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    AutoPtr<IFile> rootDir;
    environment->GetRootDirectory((IFile**)&rootDir);

    AutoPtr<IFile> permFile;
    CFile::New(rootDir, String("etc/permissions/platform.xml"), (IFile**)&permFile);
    return ReadPermissionsFromXml(permFile, onlyFeatures);
}

ECode SystemConfig::ReadPermissionsFromXml(
    /* [in] */ IFile* permFile,
    /* [in] */ Boolean onlyFeatures)
{
    AutoPtr<IFileReader> permReader;
    ECode ec = CFileReader::New(permFile, (IFileReader**)&permReader);
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION){
        Logger::W(TAG, "Couldn't find or open permissions file %s", TO_CSTR(permFile));
        return NOERROR;
    }

    Int32 type, eventType;
    Int32 uid;
    String name, gidStr, perm, nullStr, uidStr, fname, pos, pkgname;
    HashMap<Int32, AutoPtr<HashSet<String> > >::Iterator it;
    AutoPtr<HashSet<String> > perms;

    const String sname("name");
    const String spackage("package");

    AutoPtr<IXmlPullParser> parser;
    ec = Xml::NewPullParser((IXmlPullParser**)&parser);
    FAIL_GOTO(ec, _Exit_)

    ec = parser->SetInput(IReader::Probe(permReader));
    FAIL_GOTO(ec, _Exit_)

    ec = parser->Next(&type);
    FAIL_GOTO(ec, _Exit_)
    while (type != IXmlPullParser::START_TAG
        && type != IXmlPullParser::END_DOCUMENT) {
        ec = parser->Next(&type);
        FAIL_GOTO(ec, _Exit_)
    }

    if (type != IXmlPullParser::START_TAG) {
        Logger::E(TAG, "No start tag found");
        ec =  E_XML_PULL_PARSER_EXCEPTION;
        FAIL_GOTO(ec, _Exit_)
    }

    parser->GetName(&name);
    if (!name.Equals("permissions") && !name.Equals("config")) {
        Logger::E(TAG, "Unexpected start tag: found %s, expected 'permissions' or 'config'", name.string());
        ec =  E_XML_PULL_PARSER_EXCEPTION;
        FAIL_GOTO(ec, _Exit_)
    }

    while (TRUE) {
        FAIL_GOTO(XmlUtils::NextElement(parser), _Exit_)
        FAIL_GOTO(parser->GetEventType(&eventType), _Exit_)
        if (eventType == IXmlPullParser::END_DOCUMENT) {
            break;
        }

        FAIL_GOTO(parser->GetName(&name), _Exit_)
        if (name.Equals("group") && !onlyFeatures) {
            FAIL_GOTO(parser->GetAttributeValue(nullStr, String("gid"), &gidStr), _Exit_)
            if (!gidStr.IsNull()) {
                Int32 gid = Process::GetGidForName(gidStr);
                AutoPtr<ArrayOf<Int32> > ids = mGlobalGids;
                mGlobalGids = ArrayUtils::AppendInt32(ids, gid);
            }
            else {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<group> without gid at %s", pos.string());
            }

            XmlUtils::SkipCurrentTag(parser);
            continue;
        }
        else if (name.Equals("permission") && !onlyFeatures) {
            FAIL_GOTO(parser->GetAttributeValue(nullStr, sname, &perm), _Exit_)
            if (perm.IsNull()) {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<permission> without name at %s", pos.string());
                XmlUtils::SkipCurrentTag(parser);
                continue;
            }

            ReadPermission(parser, perm);
        }
        else if (name.Equals("assign-permission") && !onlyFeatures) {
            FAIL_GOTO(parser->GetAttributeValue(nullStr, sname, &perm), _Exit_)
            if (perm == NULL) {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<assign-permission> without name at %s", pos.string());
                XmlUtils::SkipCurrentTag(parser);
                continue;
            }

            FAIL_GOTO(parser->GetAttributeValue(nullStr, String("uid"), &uidStr), _Exit_)
            if (uidStr == NULL) {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<assign-permission> without uid at %s", pos.string());
                XmlUtils::SkipCurrentTag(parser);
                continue;
            }
            uid = Process::GetUidForName(uidStr);
            if (uid < 0) {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<assign-permission> with unknown uid \"%s\" at ",
                    uidStr.string(), pos.string());
                XmlUtils::SkipCurrentTag(parser);
                continue;
            }

            perms = NULL;
            it = mSystemPermissions->Find(uid);
            if (it != mSystemPermissions->End()) {
                perms = it->mSecond;
            }

            if (perms == NULL) {
                perms = new HashSet<String>();
                (*mSystemPermissions)[uid] = perms;
            }
            perms->Insert(perm);
            XmlUtils::SkipCurrentTag(parser);

        }
        else if (name.Equals("library") && !onlyFeatures) {
            String lname, lfile;
            FAIL_GOTO(parser->GetAttributeValue(nullStr, sname, &lname), _Exit_)
            FAIL_GOTO(parser->GetAttributeValue(nullStr, String("file"), &lfile), _Exit_)
            if (lname == NULL) {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<library> without name at %s", pos.string());
            }
            else if (lfile == NULL) {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<library> without file at %s", pos.string());
            }
            else {
                //Log.i(TAG, "Got library " + lname + " in " + lfile);
                (*mSharedLibraries)[lname] = lfile;
            }
            XmlUtils::SkipCurrentTag(parser);
            continue;

        }
        else if (name.Equals("feature")) {
            FAIL_GOTO(parser->GetAttributeValue(nullStr, sname, &fname), _Exit_)
            if (fname.IsNull()) {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<feature> without name at %s", pos.string());
            }
            else {
                //Log.i(TAG, "Got feature " + fname);
                AutoPtr<IFeatureInfo> fi;
                CFeatureInfo::New((IFeatureInfo**)&fi);
                fi->SetName(fname);
                (*mAvailableFeatures)[fname] = fi;
            }
            XmlUtils::SkipCurrentTag(parser);
            continue;

        }
        else if (name.Equals("allow-in-power-save")) {
            FAIL_GOTO(parser->GetAttributeValue(nullStr, spackage, &pkgname), _Exit_)
            if (pkgname.IsNull()) {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<allow-in-power-save> without package at %s", pos.string());
            }
            else {
                mAllowInPowerSave->Insert(pkgname);
            }
            XmlUtils::SkipCurrentTag(parser);
            continue;

        }
        else if (name.Equals("fixed-ime-app")) {
            FAIL_GOTO(parser->GetAttributeValue(nullStr, spackage, &pkgname), _Exit_)
            if (pkgname.IsNull()) {
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<fixed-ime-app> without package at %s", pos.string());
            } else {
                mFixedImeApps->Insert(pkgname);
            }
            XmlUtils::SkipCurrentTag(parser);
            continue;

        }
        else {
            XmlUtils::SkipCurrentTag(parser);
            continue;
        }
    }

    ICloseable::Probe(permReader)->Close();

_Exit_:
    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Logger::W(TAG, "Got execption parsing permissions. E_XML_PULL_PARSER_EXCEPTION");
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "Got execption parsing permissions. E_IO_EXCEPTION");
    }

    return ec;
}

ECode SystemConfig::ReadPermission(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& name)
{
    AutoPtr<PermissionEntry>  perm;
    HashMap<String, AutoPtr<PermissionEntry> >::Iterator it = mPermissions->Find(name);
    if (it != mPermissions->End()) {
        perm = it->mSecond;
    }

    if (perm == NULL) {
        perm = new PermissionEntry(name);
        (*mPermissions)[name] = perm;
    }

    Int32 outerDepth, depth, type;
    String tagName, gidStr, nullStr;
    parser->GetDepth(&outerDepth);
    parser->Next(&type);
    while (type != IXmlPullParser::END_DOCUMENT
        && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth) > outerDepth)) {

        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            parser->Next(&type);
            continue;
        }

        parser->GetName(&tagName);
        if (tagName.Equals("group")) {
            parser->GetAttributeValue(nullStr, String("gid"), &gidStr);
            if (!gidStr.IsNull()) {
                Int32 gid = Process::GetGidForName(gidStr);
                AutoPtr<ArrayOf<Int32> > tmp = perm->mGids;
                perm->mGids = ArrayUtils::AppendInt32(tmp, gid);
            }
            else {
                String pos;
                parser->GetPositionDescription(&pos);
                Logger::W(TAG, "<group> without gid at %s", pos.string());
            }
        }
        XmlUtils::SkipCurrentTag(parser);
        parser->Next(&type);
    }
    return NOERROR;
}


} // namespace Server
} // namepsace Droid
} // namespace Elastos
