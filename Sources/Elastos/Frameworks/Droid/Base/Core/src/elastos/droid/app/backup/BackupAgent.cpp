
#include "elastos/droid/app/backup/BackupAgent.h"
#ifdef DROID_CORE
#include "elastos/droid/app/CBackupAgentBackupServiceBinder.h"
#include "elastos/droid/app/backup/CFullBackup.h"
#endif
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
//#include "elastos/droid/os/Binder.h"
//#include "utils/log.h"
//#include "elastos/droid/app/backup/CBackupDataOutput.h"
//#include "elastos/droid/app/backup/CBackupDataInput.h"

using Libcore::IO::IOsConstants;
using Libcore::IO::COsConstants;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IStructStat;
using Libcore::IO::IStructStatFs;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IContextWrapper;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

// fc63d29c-0276-4880-8a6c-e3239940f9f8
extern "C" const InterfaceID EIID_BackupAgent =
        { 0xfc63d29c, 0x0276, 0x4880, { 0x8a, 0x6c, 0xe3, 0x23, 0x99, 0x40, 0xf9, 0xf8 } };

const String BackupAgent::TAG("BackupAgent");
const Boolean BackupAgent::DEBUG = FALSE;

BackupAgent::BackupAgent()
{
    CBackupAgentBackupServiceBinder::New((Handle32)this, (IBinder**)&mBinder);
}

BackupAgent::~BackupAgent()
{

}

ECode BackupAgent::OnCreate()
{
    return NOERROR;
}

ECode BackupAgent::OnDestroy()
{
    return NOERROR;
}

ECode BackupAgent::OnFullBackup(
    /* [in] */ IFullBackupDataOutput* data)
{
    AutoPtr<IApplicationInfo> appInfo;
    GetApplicationInfo((IApplicationInfo**)&appInfo);

    String tempDataDir;
    appInfo->GetDataDir(&tempDataDir);
    AutoPtr<IFile> file;
    CFile::New(tempDataDir, (IFile**)&file);
    String rootDir;
    file->GetCanonicalPath(&rootDir);

    file = NULL;
    GetFilesDir((IFile**)&file);
    String filesDir;
    file->GetCanonicalPath(&filesDir);

    file = NULL;
    GetDatabasePath(String("foo"), (IFile**)&file);
    AutoPtr<IFile> parent;
    file->GetParentFile((IFile**)&parent);
    String databaseDir;
    parent->GetCanonicalPath(&databaseDir);

    file = NULL;
    parent = NULL;
    GetSharedPrefsFile(String("foo"), (IFile**)&file);
    file->GetParentFile((IFile**)&parent);
    String sharedPrefsDir;
    parent->GetCanonicalPath(&sharedPrefsDir);

    file = NULL;
    GetCacheDir((IFile**)&file);
    String cacheDir;
    file->GetCanonicalPath(&cacheDir);

    String nativeLibPath;
    String libDir;
    appInfo->GetNativeLibraryDir(&nativeLibPath);
    if(!nativeLibPath.IsNull()) {
        file = NULL;
        CFile::New(nativeLibPath, (IFile**)&file);
        file->GetCanonicalPath(&libDir);
    }

    // Filters, the scan queue, and the set of resulting entities
    AutoPtr< HashSet<String> > filterSet = new HashSet<String>;
    String packageName;
    GetPackageName(&packageName);

    // Okay, start with the app's root tree, but exclude all of the canonical subdirs
    if(!libDir.IsNull()) {
        filterSet->Insert(libDir);
    }
    filterSet->Insert(cacheDir);
    filterSet->Insert(databaseDir);
    filterSet->Insert(sharedPrefsDir);
    filterSet->Insert(filesDir);
    FullBackupFileTree(packageName, IFullBackup::ROOT_TREE_TOKEN, rootDir, filterSet, data);

    // Now do the same for the files dir, db dir, and shared prefs dir
    filterSet->Insert(rootDir);
    filterSet->Erase(filesDir);
    FullBackupFileTree(packageName, IFullBackup::DATA_TREE_TOKEN, filesDir, filterSet, data);

    filterSet->Insert(filesDir);
    filterSet->Erase(databaseDir);
    FullBackupFileTree(packageName, IFullBackup::DATABASE_TREE_TOKEN, databaseDir, filterSet, data);

    filterSet->Insert(databaseDir);
    filterSet->Erase(sharedPrefsDir);
    FullBackupFileTree(packageName, IFullBackup::SHAREDPREFS_TREE_TOKEN, sharedPrefsDir, filterSet, data);
    return NOERROR;
}

ECode BackupAgent::FullBackupFile(
    /* [in] */ IFile* file,
    /* [in] */ IFullBackupDataOutput* output)
{
     // Look up where all of our various well-defined dir trees live on this device
     AutoPtr<IApplicationInfo> appInfo;
     GetApplicationInfo((IApplicationInfo**)&appInfo);

     String tempDataDir;
     appInfo->GetDataDir(&tempDataDir);
     AutoPtr<IFile> pFile;
     CFile::New(tempDataDir, (IFile**)&pFile);
     String mainDir;
     pFile->GetCanonicalPath(&mainDir);

     pFile = NULL;
     GetFilesDir((IFile**)&pFile);
     String filesDir;
     pFile->GetCanonicalPath(&filesDir);

     pFile = NULL;
     GetDatabasePath(String("foo"), (IFile**)&pFile);
     AutoPtr<IFile> parent;
     pFile->GetParentFile((IFile**)&parent);
     String dbDir;
     parent->GetCanonicalPath(&dbDir);

     pFile = NULL;
     parent = NULL;
     GetSharedPrefsFile(String("foo"), (IFile**)&pFile);
     pFile->GetParentFile((IFile**)&parent);
     String spDir;
     parent->GetCanonicalPath(&spDir);

     pFile = NULL;
     GetCacheDir((IFile**)&pFile);
     String cacheDir;
     pFile->GetCanonicalPath(&cacheDir);

     String nativeLibPath;
     appInfo->GetNativeLibraryDir(&nativeLibPath);
     String libDir(NULL);
     if (!nativeLibPath.IsNull()) {
         pFile = NULL;
         CFile::New(nativeLibPath, (IFile**)&pFile);
         pFile->GetCanonicalPath(&libDir);
     }

     String filePath;
     file->GetCanonicalPath(&filePath);

     if (filePath.StartWith(cacheDir) || filePath.StartWith(libDir)) {
         Logger::W(TAG, "lib and cache files are not backed up");
         return NOERROR;
     }

     String domain;
     String rootPath;

     if (filePath.StartWith(dbDir)) {
         domain = IFullBackup::DATABASE_TREE_TOKEN;
         rootPath = dbDir;
     }
     else if (filePath.StartWith(spDir)) {
         domain = IFullBackup::SHAREDPREFS_TREE_TOKEN;
         rootPath = spDir;
     }
     else if (filePath.StartWith(filesDir)) {
         domain = IFullBackup::DATA_TREE_TOKEN;
         rootPath = filesDir;
     }
     else if (filePath.StartWith(mainDir)) {
         domain = IFullBackup::ROOT_TREE_TOKEN;
         rootPath = mainDir;
     }
     else {
         Logger::W(TAG, "File %s is in an unsupported location; skipping" , filePath.string());
         return NOERROR;
     }

     // And now that we know where it lives, semantically, back it up appropriately
     Logger::I(TAG, "backupFile() of %s ==> domain= %s, rootPath=%s", filePath.string(), domain.string(), rootPath.string());

     String packageName;
     GetPackageName(&packageName);
     AutoPtr<IBackupDataOutput> backupData;
     output->GetData((IBackupDataOutput**)&backupData);
     AutoPtr<IFullBackup> fullBackup;
     CFullBackup::AcquireSingleton((IFullBackup**)&fullBackup);
     Int32 tempResult;
     return fullBackup->BackupToTar(packageName, domain, String(NULL), rootPath, filePath, backupData, &tempResult);
}

void BackupAgent::FullBackupFileTree(
    /* [in] */ const String& packageName,
    /* [in] */ const String& domain,
    /* [in] */ const String& rootPath,
    /* [in] */ HashSet<String>* excludes,
    /* [in] */ IFullBackupDataOutput* output)
{
    AutoPtr<IFile> rootFile;
    CFile::New(rootPath, (IFile**)&rootFile);
    Boolean isExits = FALSE;
    if (rootFile->Exists(&isExits), isExits) {
        AutoPtr<ILinkedList> scanQueue;
        CLinkedList::New((ILinkedList**)&scanQueue);
        Boolean flag = FALSE;
        scanQueue->Add(rootFile, &flag);

        Int32 size;
        while (scanQueue->GetSize(&size), size > 0) {
            AutoPtr<IInterface> obj;
            scanQueue->RemoveFirst((IInterface**)&obj);
            AutoPtr<IFile> file = IFile::Probe(obj);
            String filePath;
            // try {
            file->GetCanonicalPath(&filePath);

            //prune this subtree?
            if (excludes != NULL && excludes->Find(filePath) != excludes->End()) {
                continue;
            }

            // If it's a directory, enqueue its contents for scanning.
            AutoPtr<ILibcore> libcore;
            CLibcore::AcquireSingleton((ILibcore**)&libcore);
            AutoPtr<IOs> os;
            libcore->GetOs((IOs**)&os);
            AutoPtr<IStructStat> stat;
            os->Lstat(filePath, (IStructStat**)&stat);
            Int32 mode;
            stat->GetMode(&mode);
            AutoPtr<IOsConstants> osConstans;
            COsConstants::AcquireSingleton((IOsConstants**)&osConstans);
            Boolean isSymlink = FALSE;
            Boolean isDirectory = FALSE;
            if (osConstans->_S_ISLNK(mode, &isSymlink), isSymlink) {
                if (DEBUG) {
                    String path;
                    file->GetPath(&path);
                    Logger::I(TAG, "Symlink (skipping) !: %s", path.string());
                }
                continue;
            }
            else if (osConstans->_S_ISDIR(mode, &isDirectory), isDirectory) {
                AutoPtr< ArrayOf<IFile*> > contents;
                file->ListFiles((ArrayOf<IFile*>**)&contents);
                if (contents != NULL) {
                    for (Int32 i = 0; i < contents->GetLength(); ++i) {
                        AutoPtr<IFile> f = (*contents)[i];
                        Boolean flag = FALSE;
                        scanQueue->Add(f, &flag);
                    }
                }
            }
            // } catch (IOException e) {
            //     if (DEBUG) Log.w(TAG, "Error canonicalizing path of " + file);
            //     continue;
            // } catch (ErrnoException e) {
            //     if (DEBUG) Log.w(TAG, "Error scanning file " + file + " : " + e);
            //     continue;
            // }

            // Finally, back this file up before proceeding
            AutoPtr<IBackupDataOutput> backupData;
            output->GetData((IBackupDataOutput**)&backupData);
            AutoPtr<IFullBackup> fullBackup;
            CFullBackup::AcquireSingleton((IFullBackup**)&fullBackup);
            Int32 tempResult;
            fullBackup->BackupToTar(packageName, domain, String(NULL), rootPath, filePath, backupData, &tempResult);
        }
    }
}

ECode BackupAgent::OnRestoreFile(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int64 size,
    /* [in] */ IFile* destination,
    /* [in] */ Int32 type,
    /* [in] */ Int64 mode,
    /* [in] */ Int64 mtime)
{
    AutoPtr<IFullBackup> fullBackup;
    CFullBackup::AcquireSingleton((IFullBackup**)&fullBackup);
    return fullBackup->RestoreFile(data, size, type, mode, mtime, destination);
}

ECode BackupAgent::OnRestoreFile(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int64 size,
    /* [in] */ Int32 type,
    /* [in] */ const String& domain,
    /* [in] */ const String& path,
    /* [in] */ Int64 mode,
    /* [in] */ Int64 mtime)
{
    String basePath;

    if (DEBUG) {
        Logger::D(TAG, "onRestoreFile() size=%ld type=%d domain=%s relpath=%s mode=%ld mtime=%ld",size, type, domain.string(), path.string(), mode, mtime);
    }

    //Parse out the semantic domains into the correct physical location
    if (domain.Equals(IFullBackup::DATA_TREE_TOKEN)) {
        AutoPtr<IFile> file;
        GetFilesDir((IFile**)&file);
        file->GetCanonicalPath(&basePath);
    }
    else if (domain.Equals(IFullBackup::DATABASE_TREE_TOKEN)) {
        AutoPtr<IFile> file;
        GetDatabasePath(String("foo"), (IFile**)&file);
        AutoPtr<IFile> parentFile;
        file->GetParentFile((IFile**)&parentFile);
        parentFile->GetCanonicalPath(&basePath);
    }
    else if (domain.Equals(IFullBackup::ROOT_TREE_TOKEN)) {
        AutoPtr<IApplicationInfo> applicationInfo;
        GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        String dataDir;
        applicationInfo->GetDataDir(&dataDir);
        AutoPtr<IFile> file;
        CFile::New(dataDir, (IFile**)&file);
        file->GetCanonicalPath(&basePath);
    }
    else if (domain.Equals(IFullBackup::SHAREDPREFS_TREE_TOKEN)) {
        AutoPtr<IFile> file;
        GetSharedPrefsFile(String("foo"), (IFile**)&file);
        AutoPtr<IFile> parentFile;
        file->GetParentFile((IFile**)&parentFile);
        parentFile->GetCanonicalPath(&basePath);
    }
    else if (domain.Equals(IFullBackup::CACHE_TREE_TOKEN)) {
        AutoPtr<IFile> file;
        GetCacheDir((IFile**)&file);
        file->GetCanonicalPath(&basePath);
    }
    else {
        //Not a supported location
        Logger::I(TAG, "Data restored from non-app domain , %s ignoring", domain.string());
    }

    //Now that we've figured out where the data goes, send it on its way
    if (!basePath.IsNull()) {
        AutoPtr<IFile> outFile;
        CFile::New(basePath, path, (IFile**)&outFile);
        String outPath;
        outFile->GetPath(&outPath);
        if (DEBUG) Logger::I(TAG, "[%s:%s] mapped to %s", domain.string(), path.string(), outPath.string());
        return OnRestoreFile(data, size, outFile, type, mode, mtime);
    }
    else {
        //Not a supported output location?  We need to consume the data
        //anyway, so just use the default "copy the data out" implementation
        //with a null destination.
        if (DEBUG) Logger::I(TAG, "[ skipping data from unsupported domain  %s ]",domain.string());

        AutoPtr<IFullBackup> fullBackup;
        CFullBackup::AcquireSingleton((IFullBackup**)&fullBackup);
        return fullBackup->RestoreFile(data, size, type, mode, mtime, NULL);
    }
    return NOERROR;
}

ECode BackupAgent::OnBind(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    *binder = mBinder;
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode BackupAgent::Attach(
    /* [in] */ IContext* context)
{
    return AttachBaseContext(context);
}

ECode BackupAgent::Initialize()
{
    return NOERROR;
}

PInterface BackupAgent::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IBackupAgent*)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject*)this;
    }
    else if (riid == EIID_IBackupAgent) {
        return (IBackupAgent*)this;
    }
    else if (riid == EIID_IContextWrapper) {
        return (IContextWrapper*)this;
    }
    else if (riid == EIID_IContext) {
        return (IContext*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }
    else if (riid == EIID_BackupAgent) {
        return reinterpret_cast<PInterface>(this);
    }
    return NULL;

}

UInt32 BackupAgent::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BackupAgent::Release()
{
    return ElRefBase::Release();
}

ECode BackupAgent::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IBackupAgent *)this) {
        *pIID = EIID_IBackupAgent;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IObject *)this) {
        *pIID = EIID_IObject;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IWeakReferenceSource *)this) {
        *pIID = EIID_IWeakReferenceSource;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ECode BackupAgent::Aggregate(
    /* [in] */ AggrType aggrType,
    /* [in] */ PInterface pObject)
{
    return E_NOT_IMPLEMENTED;
}

ECode BackupAgent::GetDomain(
    /* [out] */ PInterface *ppObject)
{
    return E_NOT_IMPLEMENTED;
}

ECode BackupAgent::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode BackupAgent::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    IBackupAgent * o = IBackupAgent::Probe(other);
    if (o != NULL) {
        *result = (o == THIS_PROBE(IBackupAgent));
    }
    return NOERROR;
}

ECode BackupAgent::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (Int32)THIS_PROBE(IActivity);
    return NOERROR;
}

ECode BackupAgent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = ToString();
    return NOERROR;
}

String BackupAgent::ToString()
{
    StringBuilder sb("BackupAgent:(");
    sb += ")";
    return sb.ToString();
}

//
// IContext interfaces
//
ECode BackupAgent::AttachBaseContext(
    /* [in] */ IContext* newBase)
{
    if (mBase != NULL) {
        //throw new IllegalStateException("Base context already set");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mBase = newBase;
    return NOERROR;
}

ECode BackupAgent::GetBaseContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);

    *ctx = mBase;
    REFCOUNT_ADD(*ctx)
    return NOERROR;
}

ECode BackupAgent::GetAssets(
    /* [out] */ IAssetManager** assetManager)
{
    VALIDATE_NOT_NULL(assetManager);
    return mBase->GetAssets(assetManager);
}

ECode BackupAgent::GetResources(
    /* [out] */ IResources** resources)
{
    VALIDATE_NOT_NULL(resources);
    return mBase->GetResources(resources);
}

ECode BackupAgent::GetPackageManager(
    /* [out] */ IPackageManager** packageManager)
{
    VALIDATE_NOT_NULL(packageManager)
    return mBase->GetPackageManager(packageManager);
}

ECode BackupAgent::GetContentResolver(
    /* [out] */ IContentResolver** resolver)
{
    VALIDATE_NOT_NULL(resolver)
    return mBase->GetContentResolver(resolver);
}

ECode BackupAgent::GetMainLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper)
    return mBase->GetMainLooper(looper);
}

ECode BackupAgent::GetApplicationContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    return mBase->GetApplicationContext(ctx);
}

ECode BackupAgent::RegisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    return ctx->RegisterComponentCallbacks(componentCallback);
}

ECode BackupAgent::UnregisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    return ctx->UnregisterComponentCallbacks(componentCallback);
}

ECode BackupAgent::GetText(
    /* [in] */ Int32 resId,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text)
    *text = NULL;
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    return res->GetText(resId, text);
}

ECode BackupAgent::GetString(
    /* [in] */ Int32 resId,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    return res->GetString(resId, str);
}

ECode BackupAgent::GetString(
    /* [in] */ Int32 resId,
    /* [in] */ ArrayOf<IInterface*>* formatArgs,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    //return res->GetString(resId, formatArgs, str);
    return E_NOT_IMPLEMENTED;
}

ECode BackupAgent::SetTheme(
    /* [in] */ Int32 resid)
{
    return mBase->SetTheme(resid);
}

ECode BackupAgent::GetThemeResId(
    /* [out] */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId);
    return mBase->GetThemeResId(resId);
}

ECode BackupAgent::GetTheme(
    /* [out] */ IResourcesTheme** theme)
{
    VALIDATE_NOT_NULL(theme);
    return mBase->GetTheme(theme);
}

ECode BackupAgent::ObtainStyledAttributes(
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles)
    *styles = NULL;
    AutoPtr<IResourcesTheme> theme;
    GetTheme((IResourcesTheme**)&theme);
    return theme->ObtainStyledAttributes(attrs, styles);
}

ECode BackupAgent::ObtainStyledAttributes(
    /* [in] */ Int32 resid,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles)
    *styles = NULL;
    AutoPtr<IResourcesTheme> theme;
    GetTheme((IResourcesTheme**)&theme);
    return theme->ObtainStyledAttributes(resid, attrs, styles);
}

ECode BackupAgent::ObtainStyledAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles)
    *styles = NULL;
    AutoPtr<IResourcesTheme> theme;
    GetTheme((IResourcesTheme**)&theme);
    return theme->ObtainStyledAttributes(set, attrs, 0, 0, styles);
}

ECode BackupAgent::ObtainStyledAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles)
    *styles = NULL;
    AutoPtr<IResourcesTheme> theme;
    GetTheme((IResourcesTheme**)&theme);
    return theme->ObtainStyledAttributes(set, attrs,
            defStyleAttr, defStyleRes, styles);
}

ECode BackupAgent::GetClassLoader(
    /* [out] */ IClassLoader** loader)
{
    VALIDATE_NOT_NULL(loader)
    return mBase->GetClassLoader(loader);
}

ECode BackupAgent::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    return mBase->GetPackageName(packageName);
}

ECode BackupAgent::GetApplicationInfo(
    /* [out] */ IApplicationInfo** info)
{
    VALIDATE_NOT_NULL(info)
    return mBase->GetApplicationInfo(info);
}

ECode BackupAgent::GetPackageResourcePath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path)
    return mBase->GetPackageResourcePath(path);
}

ECode BackupAgent::GetPackageCodePath(
    /* [out] */ String* codePath)
{
    VALIDATE_NOT_NULL(codePath)
    return mBase->GetPackageCodePath(codePath);
}

ECode BackupAgent::GetSharedPrefsFile(
    /* [in] */ const String& name,
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    return mBase->GetSharedPrefsFile(name, file);
}

ECode BackupAgent::GetSharedPreferences(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ ISharedPreferences** prefs)
{
    VALIDATE_NOT_NULL(prefs)
    return mBase->GetSharedPreferences(name, mode, prefs);
}

ECode BackupAgent::OpenFileInput(
    /* [in] */ const String& name,
    /* [out] */ IFileInputStream** fileInputStream)
{
    VALIDATE_NOT_NULL(fileInputStream)
    return mBase->OpenFileInput(name, fileInputStream);
}

ECode BackupAgent::OpenFileOutput(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ IFileOutputStream** fileOutputStream)
{
    VALIDATE_NOT_NULL(fileOutputStream)
    return mBase->OpenFileOutput(name, mode, fileOutputStream);
}

ECode BackupAgent::DeleteFile(
    /* [in] */ const String& name,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mBase->DeleteFile(name, succeeded);
}

ECode BackupAgent::GetFileStreamPath(
    /* [in] */ const String& name,
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    return mBase->GetFileStreamPath(name, file);
}

ECode BackupAgent::GetFilesDir(
    /* [out] */ IFile** filesDir)
{
    VALIDATE_NOT_NULL(filesDir)
    return mBase->GetFilesDir(filesDir);
}

ECode BackupAgent::GetExternalFilesDir(
    /* [in] */ const String& type,
    /* [out] */ IFile** filesDir)
{
    VALIDATE_NOT_NULL(filesDir)
    return mBase->GetExternalFilesDir(type, filesDir);
}

ECode BackupAgent::GetObbDir(
    /* [out] */ IFile** obbDir)
{
    VALIDATE_NOT_NULL(obbDir)
    return mBase->GetObbDir(obbDir);
}

ECode BackupAgent::GetCacheDir(
    /* [out] */ IFile** cacheDir)
{
    VALIDATE_NOT_NULL(cacheDir)
    return mBase->GetCacheDir(cacheDir);
}

ECode BackupAgent::GetExternalCacheDir(
    /* [out] */ IFile** externalDir)
{
    VALIDATE_NOT_NULL(externalDir)
    return mBase->GetExternalCacheDir(externalDir);
}

ECode BackupAgent::GetFileList(
    /* [out, callee] */ ArrayOf<String>** fileList)
{
    VALIDATE_NOT_NULL(fileList)
    return mBase->GetFileList(fileList);
}

ECode BackupAgent::GetDir(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    return mBase->GetDir(name, mode, dir);
}

ECode BackupAgent::OpenOrCreateDatabase(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** sqliteDB)
{
    VALIDATE_NOT_NULL(sqliteDB)
    return mBase->OpenOrCreateDatabase(name, mode, factory, sqliteDB);
}

ECode BackupAgent::OpenOrCreateDatabase(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ IDatabaseErrorHandler* errorHandler,
    /* [out] */ ISQLiteDatabase** sqliteDB)
{
    VALIDATE_NOT_NULL(sqliteDB)
    return mBase->OpenOrCreateDatabase(name, mode, factory, errorHandler, sqliteDB);
}

ECode BackupAgent::DeleteDatabase(
    /* [in] */ const String& name,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mBase->DeleteDatabase(name, succeeded);
}

ECode BackupAgent::GetDatabasePath(
    /* [in] */ const String& name,
    /* [out] */ IFile** path)
{
    VALIDATE_NOT_NULL(path)
    return mBase->GetDatabasePath(name, path);
}

ECode BackupAgent::GetDatabaseList(
    /* [out, callee] */ ArrayOf<String>** databaseList)
{
    VALIDATE_NOT_NULL(databaseList)
    return mBase->GetDatabaseList(databaseList);
}

ECode BackupAgent::GetWallpaper(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    return mBase->GetWallpaper(drawable);
}

ECode BackupAgent::PeekWallpaper(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    return mBase->PeekWallpaper(drawable);
}

ECode BackupAgent::GetWallpaperDesiredMinimumWidth(
    /* [out] */ Int32* minWidth)
{
    VALIDATE_NOT_NULL(minWidth)
    return mBase->GetWallpaperDesiredMinimumWidth(minWidth);
}

ECode BackupAgent::GetWallpaperDesiredMinimumHeight(
    /* [out] */ Int32* minHeight)
{
    VALIDATE_NOT_NULL(minHeight)
    return mBase->GetWallpaperDesiredMinimumHeight(minHeight);
}

ECode BackupAgent::SetWallpaper(
    /* [in] */ IBitmap* bitmap)
{
    return mBase->SetWallpaper(bitmap);
}

ECode BackupAgent::SetWallpaper(
    /* [in] */ IInputStream* data)
{
    return mBase->SetWallpaper(data);
}

ECode BackupAgent::ClearWallpaper()
{
    return mBase->ClearWallpaper();
}

ECode BackupAgent::StartActivity(
    /* [in] */ IIntent *intent)
{
    return mBase->StartActivity(intent);
}

ECode BackupAgent::StartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options)
{
    return mBase->StartActivity(intent, options);
}

ECode BackupAgent::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return mBase->StartActivityAsUser(intent, user);
}

ECode BackupAgent::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* user)
{
    return mBase->StartActivityAsUser(intent, options, user);
}

ECode BackupAgent::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents)
{
    return mBase->StartActivities(intents);
}

ECode BackupAgent::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options)
{
    return mBase->StartActivities(intents, options);
}

ECode BackupAgent::StartActivitiesAsUser(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* userHandle)
{
    return mBase->StartActivitiesAsUser(intents, options, userHandle);
}

ECode BackupAgent::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags)
{
    return mBase->StartIntentSender(intent, fillInIntent, flagsMask, flagsValues, extraFlags);
}

ECode BackupAgent::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags,
    /* [in] */ IBundle* options)
{
    return mBase->StartIntentSender(intent, fillInIntent, flagsMask, flagsValues, extraFlags, options);
}

ECode BackupAgent::SendBroadcast(
    /* [in] */ IIntent* intent)
{
    return mBase->SendBroadcast(intent);
}

ECode BackupAgent::SendBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission)
{
    return mBase->SendBroadcast(intent, receiverPermission);
}

ECode BackupAgent::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission)
{
    return mBase->SendOrderedBroadcast(intent, receiverPermission);
}

ECode BackupAgent::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return mBase->SendOrderedBroadcast(intent, receiverPermission, resultReceiver, scheduler,
        initialCode, initialData, initialExtras);
}

ECode BackupAgent::SendBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return mBase->SendBroadcastAsUser(intent, user);
}

ECode BackupAgent::SendBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& receiverPermission)
{
    return mBase->SendBroadcastAsUser(intent, user, receiverPermission);
}

ECode BackupAgent::SendOrderedBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& receiverPermission,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return mBase->SendOrderedBroadcastAsUser(intent, user, receiverPermission, resultReceiver, scheduler,
        initialCode, initialData, initialExtras);
}

ECode BackupAgent::SendStickyBroadcast(
    /* [in] */ IIntent* intent)
{
    return mBase->SendStickyBroadcast(intent);
}

ECode BackupAgent::SendStickyOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return mBase->SendStickyOrderedBroadcast(intent, resultReceiver, scheduler, initialCode, initialData,
        initialExtras);
}

ECode BackupAgent::RemoveStickyBroadcast(
    /* [in] */ IIntent* intent)
{
    return mBase->RemoveStickyBroadcast(intent);
}

ECode BackupAgent::SendStickyBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return mBase->SendStickyBroadcastAsUser(intent, user);
}

ECode BackupAgent::SendStickyOrderedBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return mBase->SendStickyOrderedBroadcastAsUser(intent, user, resultReceiver, scheduler, initialCode,
        initialData, initialExtras);
}

ECode BackupAgent::RemoveStickyBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return mBase->RemoveStickyBroadcastAsUser(intent, user);
}

ECode BackupAgent::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [out] */ IIntent** stickyIntent)
{
    return mBase->RegisterReceiver(receiver, filter, stickyIntent);
}

ECode BackupAgent::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** stickyIntent)
{
    return mBase->RegisterReceiver(receiver, filter, broadcastPermission, scheduler, stickyIntent);
}

ECode BackupAgent::RegisterReceiverAsUser(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IUserHandle* user,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** stickyIntent)
{
    return mBase->RegisterReceiverAsUser(receiver, user, filter, broadcastPermission, scheduler, stickyIntent);
}

ECode BackupAgent::UnregisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver)
{
    return mBase->UnregisterReceiver(receiver);
}

ECode BackupAgent::StartService(
    /* [in] */ IIntent* service,
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name)
    return mBase->StartService(service, name);
}

ECode BackupAgent::StopService(
    /* [in] */ IIntent* service,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mBase->StopService(service, succeeded);
}

ECode BackupAgent::StartServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name)
    return mBase->StartServiceAsUser(service, user, name);
}

ECode BackupAgent::StopServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mBase->StopServiceAsUser(service, user, succeeded);
}

ECode BackupAgent::BindService(
    /* [in] */ IIntent* service,
    /* [in] */ Elastos::Droid::Content::IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mBase->BindService(service, conn, flags, succeeded);
}

ECode BackupAgent::BindService(
    /* [in] */ IIntent* service,
    /* [in] */ Elastos::Droid::Content::IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mBase->BindService(service, conn, flags, userHandle, succeeded);
}

ECode BackupAgent::UnbindService(
    /* [in] */ Elastos::Droid::Content::IServiceConnection* conn)
{
    return mBase->UnbindService(conn);
}

ECode BackupAgent::StartInstrumentation(
    /* [in] */ IComponentName* className,
    /* [in] */ const String& profileFile,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mBase->StartInstrumentation(className, profileFile, arguments, succeeded);
}

ECode BackupAgent::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    return mBase->GetSystemService(name, object);
}

ECode BackupAgent::CheckPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    return mBase->CheckPermission(permission, pid, uid, result);
}

ECode BackupAgent::CheckCallingPermission(
    /* [in] */ const String& permission,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mBase->CheckCallingPermission(permission, value);
}

ECode BackupAgent::CheckCallingOrSelfPermission(
    /* [in] */ const String& permission,
    /* [out] */ Int32* perm)
{
    VALIDATE_NOT_NULL(perm)
    return mBase->CheckCallingOrSelfPermission(permission, perm);
}

ECode BackupAgent::EnforcePermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& message)
{
    return mBase->EnforcePermission(permission, pid, uid, message);
}

ECode BackupAgent::EnforceCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& message)
{
    return mBase->EnforceCallingPermission(permission, message);
}

ECode BackupAgent::EnforceCallingOrSelfPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& message)
{
    return mBase->EnforceCallingOrSelfPermission(permission, message);
}

ECode BackupAgent::GrantUriPermission(
    /* [in] */ const String& toCapsule,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
    return mBase->GrantUriPermission(toCapsule, uri, modeFlags);
}

ECode BackupAgent::RevokeUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
    return mBase->RevokeUriPermission(uri, modeFlags);
}

ECode BackupAgent::CheckUriPermission(
    /* [in] */ IUri * uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    return mBase->CheckUriPermission(uri, pid, uid, modeFlags, result);
}

ECode BackupAgent::CheckCallingUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* permissionId)
{
    VALIDATE_NOT_NULL(permissionId)
    return mBase->CheckCallingUriPermission(uri, modeFlags, permissionId);
}

ECode BackupAgent::CheckCallingOrSelfUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* permissionId)
{
    return mBase->CheckCallingOrSelfUriPermission(uri, modeFlags, permissionId);
}

ECode BackupAgent::CheckUriPermission(
    /* [in] */ IUri * uri,
    /* [in] */ const String& readPermission,
    /* [in] */ const String& writePermission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32 * result)
{
    return mBase->CheckUriPermission(uri, readPermission, writePermission,
                    pid, uid, modeFlags, result);
}

ECode BackupAgent::EnforceUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return mBase->EnforceUriPermission(uri, pid, uid, modeFlags, message);
}

ECode BackupAgent::EnforceCallingUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return mBase->EnforceCallingUriPermission(uri, modeFlags, message);
}

ECode BackupAgent::EnforceCallingOrSelfUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return mBase->EnforceCallingOrSelfUriPermission(uri, modeFlags, message);
}

ECode BackupAgent::EnforceUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ const String& readPermission,
    /* [in] */ const String& writePermission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return mBase->EnforceUriPermission(uri, readPermission, writePermission,
            pid, uid, modeFlags, message);
}

ECode BackupAgent::CreatePackageContext(
    /* [in] */ const String& capsuleName,
    /* [in] */ Int32 flags,
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    return mBase->CreatePackageContext(capsuleName, flags, ctx);
}

ECode BackupAgent::CreatePackageContextAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* user,
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    return mBase->CreatePackageContextAsUser(packageName, flags, user, ctx);
}

ECode BackupAgent::CreateConfigurationContext(
    /* [in] */ IConfiguration* overrideConfiguration,
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    return mBase->CreateConfigurationContext(overrideConfiguration, ctx);
}

ECode BackupAgent::CreateDisplayContext(
    /* [in] */ IDisplay* display,
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    return mBase->CreateDisplayContext(display, ctx);
}

ECode BackupAgent::GetCompatibilityInfo(
    /* [in] */ Int32 displayId,
    /* [out] */ ICompatibilityInfoHolder** infoHolder)
{
    VALIDATE_NOT_NULL(infoHolder)
    return mBase->GetCompatibilityInfo(displayId, infoHolder);
}

ECode BackupAgent::IsRestricted(
    /* [out] */ Boolean* isRestricted)
{
    VALIDATE_NOT_NULL(isRestricted)
    return mBase->IsRestricted(isRestricted);
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
