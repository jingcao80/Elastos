
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/webkit/native/content/browser/ResourceExtractor.h"
#include "elastos/droid/webkit/native/ui/base/LocalizationUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
//TODO #include "preference/CPreferenceManagerHelper.h"

using Elastos::Core::StringUtils;
using Elastos::IO::EIID_IFilenameFilter;
//TODO using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Webkit::Ui::Base::LocalizationUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
    assert(0);
    // TODO
    // CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    AutoPtr<ISharedPreferences> prefs;
    helper->GetDefaultSharedPreferences(mOwner->mContext, (ISharedPreferences**)&prefs);
    assert(0);
    // TODO
    // HashSet<String> filenames = (HashSet<String>) prefs.getStringSet(
    //         PAK_FILENAMES, new HashSet<String>());
    String currentLocale = LocalizationUtils::GetDefaultLocale();
    String currentLanguage;// TODO = currentLocale.Split("-", 2)[0];

    assert(0);
    // TODO
    // String value;
    // prefs->GetString(LAST_LANGUAGE, String(""), &value);
    // if (value.Equals(currentLanguage)
    //         &&  filenames.size() >= sMandatoryPaks.length) {
    //     boolean filesPresent = true;
    //     for (String file : filenames) {
    //         if (!new File(outputDir, file).exists()) {
    //             filesPresent = false;
    //             break;
    //         }
    //     }
    //     if (filesPresent) return null;
    // } else {
    //     prefs.edit().putString(LAST_LANGUAGE, currentLanguage).apply();
    // }

    // StringBuilder p = new StringBuilder();
    // for (String mandatoryPak : sMandatoryPaks) {
    //     if (p.length() > 0) p.append('|');
    //     p.append("\\Q" + mandatoryPak + "\\E");
    // }

    // if (sExtractImplicitLocalePak) {
    //     if (p.length() > 0) p.append('|');
    //     // As well as the minimum required set of .paks above, we'll also add all .paks that
    //     // we have for the user's currently selected language.

    //     p.append(currentLanguage);
    //     p.append("(-\\w+)?\\.pak");
    // }

    // Pattern paksToInstall = Pattern.compile(p.toString());

    // AssetManager manager = mContext.getResources().getAssets();
    // try {
    //     // Loop through every asset file that we have in the APK, and look for the
    //     // ones that we need to extract by trying to match the Patterns that we
    //     // created above.
    //     byte[] buffer = null;
    //     String[] files = manager.list("");
    //     for (String file : files) {
    //         if (!paksToInstall.matcher(file).matches()) {
    //             continue;
    //         }
    //         boolean isICUData = file.equals(ICU_DATA_FILENAME);
    //         File output = new File(isICUData ? getAppDataDir() : outputDir, file);
    //         if (output.exists()) {
    //             continue;
    //         }

    //         InputStream is = null;
    //         OutputStream os = null;
    //         try {
    //             is = manager.open(file);
    //             os = new FileOutputStream(output);
    //             Log.i(LOGTAG, "Extracting resource " + file);
    //             if (buffer == null) {
    //                 buffer = new byte[BUFFER_SIZE];
    //             }

    //             int count = 0;
    //             while ((count = is.read(buffer, 0, BUFFER_SIZE)) != -1) {
    //                 os.write(buffer, 0, count);
    //             }
    //             os.flush();

    //             // Ensure something reasonable was written.
    //             if (output.length() == 0) {
    //                 throw new IOException(file + " extracted with 0 length!");
    //             }

    //             if (!isICUData) {
    //                 filenames.add(file);
    //             } else {
    //                 // icudata needs to be accessed by a renderer process.
    //                 output.setReadable(true, false);
    //             }
    //         } finally {
    //             try {
    //                 if (is != null) {
    //                     is.close();
    //                 }
    //             } finally {
    //                 if (os != null) {
    //                     os.close();
    //                 }
    //             }
    //         }
    //     }
    // } catch (IOException e) {
    //     // TODO(benm): See crbug/152413.
    //     // Try to recover here, can we try again after deleting files instead of
    //     // returning null? It might be useful to gather UMA here too to track if
    //     // this happens with regularity.
    //     Log.w(LOGTAG, "Exception unpacking required pak resources: " + e.getMessage());
    //     deleteFiles();
    //     return null;
    // }

    // // Finished, write out a timestamp file if we need to.

    // if (timestampFile != null) {
    //     try {
    //         new File(outputDir, timestampFile).createNewFile();
    //     } catch (IOException e) {
    //         // Worst case we don't write a timestamp, so we'll re-extract the resource
    //         // paks next start up.
    //         Log.w(LOGTAG, "Failed to write resource pak timestamp!");
    //     }
    // }
    // // TODO(yusufo): Figure out why remove is required here.
    // prefs.edit().remove(PAK_FILENAMES).apply();
    // prefs.edit().putStringSet(PAK_FILENAMES, filenames).apply();

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
        assert(0);
        // TODO
        // mExtractTask->Get();
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
    assert(0);
    // TODO
    // mExtractTask->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR);

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
    assert(0);
    // TODO
    // CFile::New(PathUtils::GetDataDirectory(mContext), (IFile**)&file);
    return file;
}

AutoPtr<IFile> ResourceExtractor::GetOutputDir()
{
    AutoPtr<IFile> file;
    assert(0);
    // TODO
    // CFile::New(GetAppDataDir(), String("paks"), (IFile**)&file);
    return file;
}

ECode ResourceExtractor::DeleteFiles()
{
    AutoPtr<IFile> icudata;
    assert(0);
    // TODO
    // CFile(GetAppDataDir(), ICU_DATA_FILENAME, (IFile**)&icudata);
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
