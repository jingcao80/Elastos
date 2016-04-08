
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/test/ClassPathPackageInfoSource.h"
#include "elastos/droid/test/EcoFile.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::Utility::ISet;
using Elastos::Utility::INavigableSet;
using Elastos::Utility::CTreeSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Droid {
namespace Test {

AutoPtr< ArrayOf<String> > ClassPathPackageInfoSource::sEpkPaths;

ClassPathPackageInfoSource::ClassPathPackageInfoSource()
{
    mClassPath = GetClassPath();
}

void ClassPathPackageInfoSource::SetEpkPaths(
    /* [in] */ ArrayOf<String>* epkPaths)
{
    sEpkPaths = epkPaths;
}

AutoPtr<ClassPathPackageInfo> ClassPathPackageInfoSource::GetPackageInfo(
    /* [in] */ const String& pkgName)
{
    HashMap<String, AutoPtr<ClassPathPackageInfo> >::Iterator it =
            mCache.Find(pkgName);
    if (it != mCache.End()) {
        return it->mSecond;
    }
    AutoPtr<ClassPathPackageInfo> info = CreatePackageInfo(pkgName);
    mCache[pkgName] = info;
    return info;
}

AutoPtr<ClassPathPackageInfo> ClassPathPackageInfoSource::CreatePackageInfo(
    /* [in] */ const String& packageName)
{
    AutoPtr<ISet> subpackageNames, classInfos, topLevelClasses;
    CTreeSet::New((ISet**)&subpackageNames);
    CTreeSet::New((ISet**)&classInfos);
    CHashSet::New((ISet**)&topLevelClasses);
    FindClasses(packageName, classInfos, subpackageNames);
    AutoPtr<IIterator> it;
    classInfos->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> elem;
        it->GetNext((IInterface**)&elem);
        EcoFile::ClassInfo* classInfo = (EcoFile::ClassInfo*)IObject::Probe(elem);
        // if (className.endsWith(".R") || className.endsWith(".Manifest")) {
        //     // Don't try to load classes that are generated. They usually aren't in test apks.
        //     continue;
        // }

        // try {
        //     // We get errors in the emulator if we don't use the caller's class loader.
        //     topLevelClasses.add(Class.forName(className, false,
        //             (classLoader != null) ? classLoader : CLASS_LOADER));
        Boolean result;
        topLevelClasses->Add((IObject*)classInfo, &result);
        // } catch (ClassNotFoundException e) {
        //     // Should not happen unless there is a generated class that is not included in
        //     // the .apk.
        //     Log.w("ClassPathPackageInfoSource", "Cannot load class. "
        //             + "Make sure it is in your apk. Class name: '" + className
        //             + "'. Message: " + e.getMessage(), e);
        // }
    }
    return new ClassPathPackageInfo(this, packageName, subpackageNames, topLevelClasses);
}

ECode ClassPathPackageInfoSource::FindClasses(
    /* [in] */ const String& packageName,
    /* [in] */ ISet* classInfos,
    /* [in] */ ISet* subpackageNames)
{
    String packagePrefix = packageName + ".";

    for (Int32 i = 0; i < mClassPath->GetLength(); i++) {
        String entryName = (*mClassPath)[i];
        AutoPtr<IFile> classPathEntry;
        CFile::New(entryName, (IFile**)&classPathEntry);

        // Forge may not have brought over every item in the classpath. Be
        // polite and ignore missing entries.
        Boolean result;
        if (classPathEntry->Exists(&result), result) {
            // try {
            if (entryName.EndWith(".epk")) {
                ECode ec = FindClassesInEpk(entryName, packageName, classInfos, subpackageNames);
                if (FAILED(ec)) return E_ASSERTION_ERROR;
            }
            else {
                // scan the directories that contain apk files.
                for (Int32 j = 0; j < sEpkPaths->GetLength(); j++) {
                    String epkPath = (*sEpkPaths)[j];
                    AutoPtr<IFile> file;
                    CFile::New(epkPath, (IFile**)&file);
                    ECode ec = ScanForEpkFiles(file, packageName, classInfos, subpackageNames);
                    if (FAILED(ec)) return E_ASSERTION_ERROR;
                }
            }
            // } catch (IOException e) {
            //     throw new AssertionError("Can't read classpath entry " +
            //             entryName + ": " + e.getMessage());
            // }
        }
    }
    return NOERROR;
}

ECode ClassPathPackageInfoSource::ScanForEpkFiles(
    /* [in] */ IFile* source,
    /* [in] */ const String& packageName,
    /* [in] */ ISet* classInfos,
    /* [in] */ ISet* subpackageNames)
{
    String path;
    source->GetPath(&path);
    if (path.EndWith(".epk")) {
        return FindClassesInEpk(path, packageName, classInfos, subpackageNames);
    }
    else {
        AutoPtr< ArrayOf<IFile*> > files;
        source->ListFiles((ArrayOf<IFile*>**)&files);
        if (files != NULL) {
            for (Int32 i = 0; i < files->GetLength(); i++) {
                AutoPtr<IFile> file = (*files)[i];
                FAIL_RETURN(ScanForEpkFiles(file, packageName, classInfos, subpackageNames));
            }
        }
        return NOERROR;
    }
}

ECode ClassPathPackageInfoSource::FindClassesInEpk(
    /* [in] */ const String& epkPath,
    /* [in] */ const String& packageName,
    /* [in] */ ISet* classInfos,
    /* [in] */ ISet* subpackageNames)
{
    AutoPtr<EcoFile> ecoFile;
    // try {
    ECode ec = EcoFile::NewEcoFile(epkPath, (EcoFile**)&ecoFile);
    if (FAILED(ec)) return NOERROR;
    AutoPtr<IEnumeration> epkClassNames = ecoFile->Entries();
    Boolean result;
    while (epkClassNames->HasMoreElements(&result), result) {
        AutoPtr<IInterface> elem;
        epkClassNames->GetNextElement((IInterface**)&elem);
        EcoFile::ClassInfo* classInfo = (EcoFile::ClassInfo*)IObject::Probe(elem);
        String className;
        classInfo->mClassInfo->GetName(&className);
        if (className.StartWith(packageName)) {
            String subPackageName = packageName;
            Int32 lastPackageSeparator = className.LastIndexOf('.');
            if (lastPackageSeparator > 0) {
                subPackageName = className.Substring(0, lastPackageSeparator);
            }
            if (subPackageName.GetLength() > packageName.GetLength()) {
                AutoPtr<ICharSequence> csq;
                CString::New(subPackageName, (ICharSequence**)&csq);
                subpackageNames->Add(csq, &result);
            }
            else if (IsToplevelClass(className)) {
                classInfos->Add(elem, &result);
            }
        }
    }
    if (ecoFile != NULL) {
        // ecoFile->Close();
    }
//     } catch (IOException e) {
//         if (false) {
//             Log.w("ClassPathPackageInfoSource",
//                     "Error finding classes at apk path: " + apkPath, e);
//         }
//     } finally {
//         if (dexFile != null) {
//             // Todo: figure out why closing causes a dalvik error resulting in vm shutdown.
// //                dexFile.close();
//         }
//     }

    return NOERROR;
}

Boolean ClassPathPackageInfoSource::IsToplevelClass(
    /* [in] */ const String& fileName)
{
    // return fileName.IndexOf('$') < 0;
    if (fileName.Contains("ClassObject")) return FALSE;
    return TRUE;
}

AutoPtr< ArrayOf<String> > ClassPathPackageInfoSource::GetClassPath()
{
    // String classPath = System.getProperty("java.class.path");
    // String separator = System.getProperty("path.separator", ":");
    // return classPath.split(Pattern.quote(separator));
    String classPath(".");
    String separator(":");
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    String reg;
    helper->Quote(separator, &reg);
    AutoPtr< ArrayOf<String> > paths;
    StringUtils::Split(classPath, reg.string(), (ArrayOf<String>**)&paths);
    return paths;
}

void ClassPathPackageInfoSource::SetClassLoader(
    /* [in] */ IClassLoader* classLoader)
{
    mClassLoader = classLoader;
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
