#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ResourceExtractor.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/LocalizationUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/base/PathUtils.h"
#include <elastos/core/StringUtils.h>
#include "elastos/core/StringBuilder.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::LocalizationUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Base::PathUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;
using Elastos::IO::EIID_IFilenameFilter;
using Elastos::IO::CFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::CFileOutputStream;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                ResourceExtractor::InnerFilenameFilter
//=====================================================================

ResourceExtractor::InnerFilenameFilter::InnerFilenameFilter(
    /* [in] */ const String& timestamp)
    : TIMESTAMP_PREFIX(timestamp)
{
}

CAR_INTERFACE_IMPL(ResourceExtractor::InnerFilenameFilter, Object, IFilenameFilter);

ECode ResourceExtractor::InnerFilenameFilter::Accept(
    /* [in] */ IFile* dir,
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = name.StartWith(TIMESTAMP_PREFIX);
    return NOERROR;
}

//=====================================================================
//                    ResourceExtractor::ExtractTask
//=====================================================================
const Int32 ResourceExtractor::ExtractTask::BUFFER_SIZE;

ResourceExtractor::ExtractTask::ExtractTask(
    /* [in] */ ResourceExtractor* owner)
    : mOwner(owner)
{
}

ECode ResourceExtractor::ExtractTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IFile> outputDir = mOwner->GetOutputDir();
    Boolean bExists, bMkdirs;
    if (!(outputDir->Exists(&bExists), bExists) && !(outputDir->Mkdirs(&bMkdirs), bMkdirs)) {
        Slogger::E(LOGTAG, "Unable to create pak resources directory!");
        *result = NULL;
        return NOERROR;
    }

    String timestampFile = CheckPakTimestamp(outputDir);
    if (timestampFile != NULL) {
        mOwner->DeleteFiles();
    }

    AutoPtr<IPreferenceManagerHelper> helper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    AutoPtr<ISharedPreferences> prefs;
    helper->GetDefaultSharedPreferences(mOwner->mContext, (ISharedPreferences**)&prefs);
    // HashSet<String> filenames = (HashSet<String>) prefs.getStringSet(
    //         PAK_FILENAMES, new HashSet<String>());
    AutoPtr<ISet> defValue;
    CHashSet::New((ISet**)&defValue);
    AutoPtr<ISet> filenames;
    prefs->GetStringSet(PAK_FILENAMES, defValue, (ISet**)&filenames);
    String currentLocale = LocalizationUtils::GetDefaultLocale();
    AutoPtr<ArrayOf<String> > array;
    StringUtils::Split(currentLocale, "-", 2, (ArrayOf<String>**)&array);
    String currentLanguage = (*array)[0];

    String value;
    prefs->GetString(LAST_LANGUAGE, String(""), &value);
    Int32 filenamesSize;
    filenames->GetSize(&filenamesSize);
    if (value.Equals(currentLanguage)
            &&  filenamesSize >= sMandatoryPaks->GetLength()) {
        Boolean filesPresent = TRUE;
        AutoPtr<IIterator> keyIter;
        filenames->GetIterator((IIterator**)&keyIter);
        Boolean isflag = FALSE;
        while ((keyIter->HasNext(&isflag), isflag)) {
            AutoPtr<IInterface> obj;
            keyIter->GetNext((IInterface**)&obj);
            ICharSequence* cs = ICharSequence::Probe(obj);
            String file;
            cs->ToString(&file);
            AutoPtr<IFile> f;
            CFile::New(outputDir, file, (IFile**)&f);
            Boolean isExist;
            if (!(f->Exists(&isExist), isExist)) {
                filesPresent = FALSE;
                break;
            }
        }
        if (filesPresent) {
            *result = NULL;
            return NOERROR;
        }
    } else {
        AutoPtr<ISharedPreferencesEditor> spe;
        prefs->Edit((ISharedPreferencesEditor**)&spe);
        spe->PutString(LAST_LANGUAGE, currentLanguage);
        spe->Apply();
    }

    AutoPtr<StringBuilder> p = new StringBuilder();
    for(Int32 i = 0; i < sMandatoryPaks->GetLength(); ++i) {
        String mandatoryPak = (*sMandatoryPaks)[i];
        Int32 len;
        p->GetLength(&len);
        if (len > 0) p->AppendChar('|');
        p->Append("\\Q");
        p->Append(mandatoryPak);
        p->Append("\\E");
    }

    if (sExtractImplicitLocalePak) {
        Int32 len;
        p->GetLength(&len);
        if (len > 0) p->AppendChar('|');
        // As well as the minimum required set of .paks above, we'll also add all .paks that
        // we have for the user's currently selected language.

        p->Append(currentLanguage);
        p->Append("(-\\w+)?\\.pak");
    }

    AutoPtr<IPatternHelper> pHelper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&pHelper);
    AutoPtr<IPattern> paksToInstall;
    pHelper->Compile(p->ToString(), (IPattern**)&paksToInstall);

    AutoPtr<IResources> resources;
    mOwner->mContext->GetResources((IResources**)&resources);
    AutoPtr<IAssetManager> manager;
    resources->GetAssets((IAssetManager**)&manager);
    // try {
         // Loop through every asset file that we have in the APK, and look for the
         // ones that we need to extract by trying to match the Patterns that we
         // created above.
         //byte[] buffer = null;
         AutoPtr<ArrayOf<Byte> > buffer;
         AutoPtr<ArrayOf<String> > files;
         manager->List(String(""), (ArrayOf<String>**)&files);
         //for (String file : files)
         for (Int32 i = 0; i < files->GetLength(); ++i) {
             String file = (*files)[i];
             AutoPtr<IMatcher> matcher;
             paksToInstall->Matcher(file, (IMatcher**)&matcher);
             Boolean matched = FALSE;
             if (!matcher->Matches(&matched)) {
                 continue;
             }
             Boolean isICUData = file.Equals(ICU_DATA_FILENAME);
             AutoPtr<IFile> output;
             CFile::New(isICUData ? mOwner->GetAppDataDir() : outputDir, file, (IFile**)&output);
             Boolean exists;
             if (output->Exists(&exists), exists) {
                 continue;
             }

             AutoPtr<IInputStream> is;
             AutoPtr<IOutputStream> os;
             //try {
                 manager->Open(file, (IInputStream**)&is);
                 CFileOutputStream::New(output, (IOutputStream**)&os);
                 Logger::I(LOGTAG, "Extracting resource %s", file.string());
                 if (buffer == NULL) {
                     //buffer = new byte[BUFFER_SIZE];
                     buffer = ArrayOf<Byte>::Alloc(BUFFER_SIZE);
                 }

                 Int32 count = 0;
                 while ((is->Read(buffer, 0, BUFFER_SIZE, &count), count) != -1) {
                     os->Write(buffer, 0, count);
                 }
                 IFlushable::Probe(os)->Flush();

                 // Ensure something reasonable was written.
                 Int64 outputLength;
                 output->GetLength(&outputLength);
                 if (outputLength == 0) {
                     //throw new IOException(file + " extracted with 0 length!");
                     Logger::E(LOGTAG, "%s extracted with 0 length", file.string());
                 }


                 if (!isICUData) {

                     filenames->Add(TO_IINTERFACE(CoreUtils::Convert(file)));
                 } else {
                     // icudata needs to be accessed by a renderer process.
                     Boolean res;
                     output->SetReadable(TRUE, FALSE, &res);
                 }
             //} finally {
             //    try {
                     if (is != NULL) {
                         is->Close();
                     }
             //    } finally {
                     if (os != NULL) {
                         os->Close();
                     }
             //    }
             //}
         }
    //} catch (IOException e) {
    //     // TODO(benm): See crbug/152413.
    //     // Try to recover here, can we try again after deleting files instead of
    //     // returning null? It might be useful to gather UMA here too to track if
    //     // this happens with regularity.
    //     Log.w(LOGTAG, "Exception unpacking required pak resources: " + e.getMessage());
    //     deleteFiles();
    //     return null;
    // }

    // Finished, write out a timestamp file if we need to.

    if (timestampFile != NULL) {
        //try {
        AutoPtr<IFile> file;
        CFile::New(outputDir, timestampFile, (IFile**)&file);
        Boolean res;
        file->CreateNewFile(&res);
        //} catch (IOException e) {
            // Worst case we don't write a timestamp, so we'll re-extract the resource
            // paks next start up.
        //    Log.w(LOGTAG, "Failed to write resource pak timestamp!");
        //}
    }
    // TODO(yusufo): Figure out why remove is required here.
    AutoPtr<ISharedPreferencesEditor> spe;
    prefs->Edit((ISharedPreferencesEditor**)&spe);
    spe->Remove(PAK_FILENAMES);
    spe->Apply();
    spe->PutStringSet(PAK_FILENAMES, filenames);
    spe->Apply();

    *result = NULL;

    return NOERROR;
}

String ResourceExtractor::ExtractTask::CheckPakTimestamp(
    /* [in] */ IFile* outputDir)
{
    const String TIMESTAMP_PREFIX("pak_timestamp-");
    AutoPtr<IPackageManager> pm;
    mOwner->mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IPackageInfo> pi;

    // try {
        String packageName;
        mOwner->mContext->GetPackageName(&packageName);
        pm->GetPackageInfo(packageName, 0, (IPackageInfo**)&pi);
    // } catch (PackageManager.NameNotFoundException e) {
    //     return TIMESTAMP_PREFIX;
    // }

    if (pi == NULL) {
        return TIMESTAMP_PREFIX;
    }

    String expectedTimestamp(TIMESTAMP_PREFIX);
    Int32 versionCode;
    pi->GetVersionCode(&versionCode);
    String versionCodeStr = StringUtils::ToString(versionCode);
    Int64 lastUpdateTime;
    pi->GetLastUpdateTime(&lastUpdateTime);
    expectedTimestamp += StringUtils::ToString(versionCode);
    expectedTimestamp += "-";
    expectedTimestamp += StringUtils::ToString(lastUpdateTime);

    AutoPtr<IFilenameFilter> filenameFilter = new InnerFilenameFilter(TIMESTAMP_PREFIX);
    AutoPtr< ArrayOf<String> > timestamps;
    outputDir->List(filenameFilter, (ArrayOf<String>**)&timestamps);

    if (timestamps->GetLength() != 1) {
        // If there's no timestamp, nuke to be safe as we can't tell the age of the files.
        // If there's multiple timestamps, something's gone wrong so nuke.
        return expectedTimestamp;
    }

    if (!expectedTimestamp.Equals((*timestamps)[0])) {
        return expectedTimestamp;
    }

    // timestamp file is already up-to date.
    return String(NULL);
}

//=====================================================================
//                          ResourceExtractor
//=====================================================================

const String ResourceExtractor::LOGTAG("ResourceExtractor");
const String ResourceExtractor::LAST_LANGUAGE("Last language");
const String ResourceExtractor::PAK_FILENAMES("Pak filenames");
const String ResourceExtractor::ICU_DATA_FILENAME("icudtl.dat");
AutoPtr< ArrayOf<String> > ResourceExtractor::sMandatoryPaks = NULL;
Boolean ResourceExtractor::sExtractImplicitLocalePak = TRUE;
AutoPtr<ResourceExtractor> ResourceExtractor::sInstance;

AutoPtr<ResourceExtractor> ResourceExtractor::Get(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        sInstance = new ResourceExtractor(context);
    }
    return sInstance;
}

ECode ResourceExtractor::SetMandatoryPaksToExtract(
    /* [in] */ ArrayOf<String>* mandatoryPaks)
{
    assert (sInstance == NULL || sInstance->mExtractTask == NULL);
    //        : "Must be called before startExtractingResources is called";
    sMandatoryPaks = mandatoryPaks;

    return NOERROR;
}

ECode ResourceExtractor::SetExtractImplicitLocaleForTesting(
    /* [in] */ Boolean extract)
{
    assert (sInstance == NULL || sInstance->mExtractTask == NULL);
    //        : "Must be called before startExtractingResources is called";
    sExtractImplicitLocalePak = extract;

    return NOERROR;
}

ECode ResourceExtractor::WaitForCompletion()
{
    if (ShouldSkipPakExtraction()) {
        return NOERROR;
    }

    assert(mExtractTask != NULL);

    // try {
        AutoPtr<IInterface> task;
        mExtractTask->Get((IInterface**)&task);
    // } catch (CancellationException e) {
    //     // Don't leave the files in an inconsistent state.
    //     deleteFiles();
    // } catch (ExecutionException e2) {
    //     deleteFiles();
    // } catch (InterruptedException e3) {
    //     deleteFiles();
    // }

    return NOERROR;
}

ECode ResourceExtractor::StartExtractingResources()
{
    if (mExtractTask != NULL) {
        return NOERROR;
    }

    if (ShouldSkipPakExtraction()) {
        return NOERROR;
    }

    mExtractTask = new ExtractTask(this);
    mExtractTask->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL);

    return NOERROR;
}

ResourceExtractor::ResourceExtractor(
    /* [in] */ IContext* context)
{
    context->GetApplicationContext((IContext**)&mContext);
}

AutoPtr<IFile> ResourceExtractor::GetAppDataDir()
{
    AutoPtr<IFile> file;
    CFile::New(PathUtils::GetDataDirectory(mContext), (IFile**)&file);
    return file;
}

AutoPtr<IFile> ResourceExtractor::GetOutputDir()
{
    AutoPtr<IFile> file;
    CFile::New(GetAppDataDir(), String("paks"), (IFile**)&file);
    return file;
}

ECode ResourceExtractor::DeleteFiles()
{
    AutoPtr<IFile> icudata;
    CFile::New(GetAppDataDir(), ICU_DATA_FILENAME, (IFile**)&icudata);
    Boolean bExists, bDelete;
    if ((icudata->Exists(&bExists), bExists) && !(icudata->Delete(&bDelete), bDelete)) {
        String log("Unable to remove the icudata ");
        String name;
        icudata->GetName(&name);
        log += name;
        Slogger::E(LOGTAG, name);
    }

    AutoPtr<IFile> dir = GetOutputDir();
    Boolean dirExists;
    dir->Exists(&dirExists);
    if (dirExists) {
        AutoPtr< ArrayOf<IFile*> > files;
        dir->ListFiles((ArrayOf<IFile*>**)&files);

        Int32 length = files->GetLength();
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IFile> file = (*files)[i];
            Boolean bFlag;
            file->Delete(&bFlag);
            if (!bFlag) {
                String log("Unable to remove existing resource ");
                String name;
                file->GetName(&name);
                log += name;
                Slogger::E(LOGTAG, log);
            }
        }
    }

    return NOERROR;
}

Boolean ResourceExtractor::ShouldSkipPakExtraction()
{
    // Must call setMandatoryPaksToExtract before beginning resource extraction.
    assert(sMandatoryPaks != NULL);
    return sMandatoryPaks->GetLength() == 1 && String("").Equals((*sMandatoryPaks)[0]);
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
