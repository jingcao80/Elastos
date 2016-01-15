
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/CEnvironment.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/webkit/native/base/ContentUriUtils.h"
#include "elastos/droid/webkit/native/ui/base/SelectFileDialog.h"
#include "elastos/droid/webkit/native/ui/api/SelectFileDialog_dec.h"
#include "elastos/core/IntegralToString.h"
#include "elastos/utility/Arrays.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Webkit::Base::ContentUriUtils;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IntegralToString;
using Elastos::Core::ISystem;
using Elastos::IO::CFile;
using Elastos::IO::CFileHelper;
using Elastos::IO::IFile;
using Elastos::IO::IFileHelper;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Base {

//=====================================================================
//                 SelectFileDialog::GetDisplayNameTask
//=====================================================================
SelectFileDialog::GetDisplayNameTask::GetDisplayNameTask(
    /* [in] */ SelectFileDialog* owner,
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ Boolean isMultiple)
    : mOwner(owner)
    , mContentResolver(contentResolver)
    , mIsMultiple(isMultiple)
{
    // ==================before translated======================
    // mContentResolver = contentResolver;
    // mIsMultiple = isMultiple;
}

ECode SelectFileDialog::GetDisplayNameTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(params);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFilePaths = new String[uris.length];
    // String[] displayNames = new String[uris.length];
    // for (int i = 0; i < uris.length; i++) {
    //     mFilePaths[i] = uris[i].toString();
    //     displayNames[i] = ContentUriUtils.getDisplayName(
    //             uris[i], mContentResolver, MediaStore.MediaColumns.DISPLAY_NAME);
    // }
    // return displayNames;

    assert(0);
    *result = NULL;
    mFilePaths = ArrayOf<String>::Alloc(params->GetLength());
    AutoPtr<IList> dispalyNames;
    CArrayList::New(params->GetLength(), (IList**)&dispalyNames);

    String sTmp;
    for (Int32 i = 0; i < params->GetLength(); ++i) {
        ICharSequence* charSequecneTmp = ICharSequence::Probe((*params)[i]);
        charSequecneTmp->ToString(&sTmp);
        mFilePaths->Set(i, sTmp);

        IUri* uriTmp = IUri::Probe((*params)[i]);
        sTmp = ContentUriUtils::GetDisplayName(uriTmp, mContentResolver, IMediaStoreMediaColumns::DISPLAY_NAME);
        AutoPtr<ICharSequence> charSequenceTmp1;
        CString::New(sTmp, (ICharSequence**)&charSequenceTmp1);
        dispalyNames->Set(i, charSequenceTmp1);
    }

    *result = TO_IINTERFACE(dispalyNames);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SelectFileDialog::GetDisplayNameTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (!mIsMultiple) {
    //     nativeOnFileSelected(mNativeSelectFileDialog, mFilePaths[0], result[0]);
    // }

    assert(NULL != mOwner);
    if (!mIsMultiple) {
        IList* list = IList::Probe(result);
        AutoPtr<IInterface> interfaceTmp;
        list->Get(0, (IInterface**)&interfaceTmp);
        ICharSequence* charSequecneTmp = ICharSequence::Probe(interfaceTmp);
        String displayName;
        charSequecneTmp->ToString(&displayName);
        mOwner->NativeOnFileSelected(mOwner->mNativeSelectFileDialog, (*mFilePaths)[0], displayName);
    }
    return NOERROR;
}

//=====================================================================
//                           SelectFileDialog
//=====================================================================
const String SelectFileDialog::IMAGE_TYPE("image/");
const String SelectFileDialog::VIDEO_TYPE("video/");
const String SelectFileDialog::AUDIO_TYPE("audio/");
const String SelectFileDialog::ALL_IMAGE_TYPES(IMAGE_TYPE + "*");
const String SelectFileDialog::ALL_VIDEO_TYPES(VIDEO_TYPE + "*");
const String SelectFileDialog::ALL_AUDIO_TYPES(AUDIO_TYPE + "*");
const String SelectFileDialog::ANY_TYPES("*/*");
const String SelectFileDialog::CAPTURE_IMAGE_DIRECTORY("browser-photos");

ECode SelectFileDialog::OnIntentCompleted(
    /* [in] */ WindowElastos* window,
    /* [in] */ Int32 resultCode,
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ IIntent* results)
{
    VALIDATE_NOT_NULL(window);
    VALIDATE_NOT_NULL(contentResolver);
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // if (resultCode != Activity.RESULT_OK) {
    //     onFileNotSelected();
    //     return;
    // }
    //
    // if (results == null) {
    //     // If we have a successful return but no data, then assume this is the camera returning
    //     // the photo that we requested.
    //     nativeOnFileSelected(mNativeSelectFileDialog, mCameraOutputUri.getPath(), "");
    //
    //     // Broadcast to the media scanner that there's a new photo on the device so it will
    //     // show up right away in the gallery (rather than waiting until the next time the media
    //     // scanner runs).
    //     window.sendBroadcast(new Intent(
    //             Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, mCameraOutputUri));
    //     return;
    // }
    //
    // if (ContentResolver.SCHEME_FILE.equals(results.getData().getScheme())) {
    //     nativeOnFileSelected(mNativeSelectFileDialog,
    //             results.getData().getSchemeSpecificPart(), "");
    //     return;
    // }
    //
    // if (ContentResolver.SCHEME_CONTENT.equals(results.getScheme())) {
    //     GetDisplayNameTask task = new GetDisplayNameTask(contentResolver, false);
    //     task.execute(results.getData());
    //     return;
    // }
    //
    // onFileNotSelected();
    // window.showError(R.string.opening_file_error);

    assert(0);
    if (resultCode != IActivity::RESULT_OK) {
        OnFileNotSelected();
        return NOERROR;
    }

    if (NULL == results) {
        // If we have a successful return but no data, then assume this is the camera returning
        // the photo that we requested.
        String path;
        mCameraOutputUri->GetPath(&path);
        NativeOnFileSelected(mNativeSelectFileDialog, path, String(""));

        // Broadcast to the media scanner that there's a new photo on the device so it will
        // show up right away in the gallery (rather than waiting until the next time the media
        // scanner runs).
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MEDIA_SCANNER_SCAN_FILE, mCameraOutputUri, (IIntent**)&intent);
        window->SendBroadcast(intent);
        return NOERROR;
    }

    AutoPtr<IUri> url;
    results->GetData((IUri**)&url);
    String scheme;
    url->GetScheme(&scheme);
    if (IContentResolver::SCHEME_FILE == scheme) {
        String specifyPart;
        url->GetSchemeSpecificPart(&specifyPart);
        NativeOnFileSelected(mNativeSelectFileDialog, specifyPart, String(""));
        return NOERROR;
    }

    results->GetScheme(&scheme);
    if (IContentResolver::SCHEME_CONTENT == scheme) {
        AutoPtr<GetDisplayNameTask> task = new GetDisplayNameTask(this, contentResolver, FALSE);
        IRunnable* runnable = IRunnable::Probe(url);
        task->Execute(runnable);
        return NOERROR;
    }

    OnFileNotSelected();
    window->ShowError(String("")/*R::string::opening_file_error*/);
    return NOERROR;
}

SelectFileDialog::SelectFileDialog(
    /* [in] */ Int64 nativeSelectFileDialog)
    : mNativeSelectFileDialog(nativeSelectFileDialog)
{
    // ==================before translated======================
    // mNativeSelectFileDialog = nativeSelectFileDialog;
}

ECode SelectFileDialog::SelectFile(
    /* [in] */ ArrayOf<String>* fileTypes,
    /* [in] */ Boolean capture,
    /* [in] */ WindowElastos* window)
{
    VALIDATE_NOT_NULL(fileTypes);
    VALIDATE_NOT_NULL(window);
    // ==================before translated======================
    // mFileTypes = new ArrayList<String>(Arrays.asList(fileTypes));
    // mCapture = capture;
    //
    // Intent chooser = new Intent(Intent.ACTION_CHOOSER);
    // Intent camera = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
    // mCameraOutputUri = Uri.fromFile(getFileForImageCapture());
    // camera.putExtra(MediaStore.EXTRA_OUTPUT, mCameraOutputUri);
    // Intent camcorder = new Intent(MediaStore.ACTION_VIDEO_CAPTURE);
    // Intent soundRecorder = new Intent(
    //         MediaStore.Audio.Media.RECORD_SOUND_ACTION);
    //
    // // Quick check - if the |capture| parameter is set and |fileTypes| has the appropriate MIME
    // // type, we should just launch the appropriate intent. Otherwise build up a chooser based on
    // // the accept type and then display that to the user.
    // if (captureCamera()) {
    //     if (window.showIntent(camera, this, R.string.low_memory_error)) return;
    // } else if (captureCamcorder()) {
    //     if (window.showIntent(camcorder, this, R.string.low_memory_error)) return;
    // } else if (captureMicrophone()) {
    //     if (window.showIntent(soundRecorder, this, R.string.low_memory_error)) return;
    // }
    //
    // Intent getContentIntent = new Intent(Intent.ACTION_GET_CONTENT);
    // getContentIntent.addCategory(Intent.CATEGORY_OPENABLE);
    // ArrayList<Intent> extraIntents = new ArrayList<Intent>();
    // if (!noSpecificType()) {
    //     // Create a chooser based on the accept type that was specified in the webpage. Note
    //     // that if the web page specified multiple accept types, we will have built a generic
    //     // chooser above.
    //     if (shouldShowImageTypes()) {
    //         extraIntents.add(camera);
    //         getContentIntent.setType(ALL_IMAGE_TYPES);
    //     } else if (shouldShowVideoTypes()) {
    //         extraIntents.add(camcorder);
    //         getContentIntent.setType(ALL_VIDEO_TYPES);
    //     } else if (shouldShowAudioTypes()) {
    //         extraIntents.add(soundRecorder);
    //         getContentIntent.setType(ALL_AUDIO_TYPES);
    //     }
    // }
    //
    // if (extraIntents.isEmpty()) {
    //     // We couldn't resolve an accept type, so fallback to a generic chooser.
    //     getContentIntent.setType(ANY_TYPES);
    //     extraIntents.add(camera);
    //     extraIntents.add(camcorder);
    //     extraIntents.add(soundRecorder);
    // }
    //
    // chooser.putExtra(Intent.EXTRA_INITIAL_INTENTS,
    //         extraIntents.toArray(new Intent[] { }));
    //
    // chooser.putExtra(Intent.EXTRA_INTENT, getContentIntent);
    //
    // if (!window.showIntent(chooser, this, R.string.low_memory_error)) {
    //     onFileNotSelected();
    // }

    assert(0);
    Arrays::AsList(fileTypes, (IList**)&mFileTypes);
    mCapture = capture;

    AutoPtr<IIntent> chooser;
    AutoPtr<IIntent> camera;
    CIntent::New(IIntent::ACTION_CHOOSER, (IIntent**)&chooser);
    CIntent::New(IMediaStore::ACTION_IMAGE_CAPTURE, (IIntent**)&camera);

    Uri::FromFile(GetFileForImageCapture(), (IUri**)&mCameraOutputUri);
    String cameraOutputUriStr;
    mCameraOutputUri->ToSafeString(&cameraOutputUriStr);
    camera->PutExtra(IMediaStore::EXTRA_OUTPUT, cameraOutputUriStr);

    AutoPtr<IIntent> camcorder;
    AutoPtr<IIntent> soundRecorder;
    CIntent::New(IMediaStore::ACTION_VIDEO_CAPTURE, (IIntent**)&camcorder);
    CIntent::New(IMediaStoreAudioMedia::RECORD_SOUND_ACTION, (IIntent**)&soundRecorder);

    // Quick check - if the |capture| parameter is set and |fileTypes| has the appropriate MIME
    // type, we should just launch the appropriate intent. Otherwise build up a chooser based on
    // the accept type and then display that to the user.
    if (CaptureCamera()) {
        if (window->ShowIntent(camera, this, -1/*R::string::low_memory_error*/))
            return NOERROR;
    }
    else if (CaptureCamcorder()) {
        if (window->ShowIntent(camcorder, this, -1/*R::string::low_memory_error*/))
            return NOERROR;
    }
    else if (CaptureMicrophone()) {
        if (window->ShowIntent(soundRecorder, this, -1/*R::string::low_memory_error*/))
            return NOERROR;
    }

    AutoPtr<IIntent> getContentIntent;
    CIntent::New(IIntent::ACTION_GET_CONTENT, (IIntent**)&getContentIntent);
    getContentIntent->AddCategory(IIntent::CATEGORY_OPENABLE);

    AutoPtr<IArrayList> extraIntents;
    CArrayList::New((IArrayList**)&extraIntents);
    if (!NoSpecificType()) {
        // Create a chooser based on the accept type that was specified in the webpage. Note
        // that if the web page specified multiple accept types, we will have built a generic
        // chooser above.
        if (ShouldShowImageTypes()) {
            extraIntents->Add(camera);
            getContentIntent->SetType(ALL_IMAGE_TYPES);
        }
        else if (ShouldShowVideoTypes()) {
            extraIntents->Add(camcorder);
            getContentIntent->SetType(ALL_VIDEO_TYPES);
        }
        else if (ShouldShowAudioTypes()) {
            extraIntents->Add(soundRecorder);
            getContentIntent->SetType(ALL_AUDIO_TYPES);
        }
    }

    Int32 length = 0;
    extraIntents->GetSize(&length);
    if (0 == length) {
        // We couldn't resolve an accept type, so fallback to a generic chooser.
        getContentIntent->SetType(ANY_TYPES);
        extraIntents->Add(camera);
        extraIntents->Add(camcorder);
        extraIntents->Add(soundRecorder);
    }

    AutoPtr< ArrayOf<IInterface*> > toArray;
    extraIntents->ToArray((ArrayOf<IInterface*>**)&toArray);
    assert(0);
    AutoPtr< ArrayOf<IParcelable*> > parcelableArrayTmp = ArrayOf<IParcelable*>::Alloc(toArray->GetLength());
    for (Int32 idx=0; idx<toArray->GetLength(); ++idx) {
        AutoPtr<IInterface> interfaceTmp = (*toArray)[idx];
        IParcelable* parcelableTmp = IParcelable::Probe(interfaceTmp.Get());
        parcelableArrayTmp->Set(idx, parcelableTmp);
    }
    chooser->PutExtra(IIntent::EXTRA_INITIAL_INTENTS, parcelableArrayTmp);
    chooser->PutExtra(IIntent::EXTRA_INTENT, IParcelable::Probe(getContentIntent));

    if (!window->ShowIntent(chooser, this, -1/*R::string::low_memory_error*/)) {
        OnFileNotSelected();
    }
    return NOERROR;
}

AutoPtr<IFile> SelectFileDialog::GetFileForImageCapture()
{
    // ==================before translated======================
    // File externalDataDir = Environment.getExternalStoragePublicDirectory(
    //         Environment.DIRECTORY_DCIM);
    // File cameraDataDir = new File(externalDataDir.getAbsolutePath() +
    //         File.separator + CAPTURE_IMAGE_DIRECTORY);
    // if (!cameraDataDir.exists() && !cameraDataDir.mkdirs()) {
    //     cameraDataDir = externalDataDir;
    // }
    // File photoFile = new File(cameraDataDir.getAbsolutePath() +
    //         File.separator + System.currentTimeMillis() + ".jpg");
    // return photoFile;

    assert(0);
    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    AutoPtr<IFile> externalDataDir;
    environment->GetExternalStoragePublicDirectory(IEnvironment::DIRECTORY_DCIM, (IFile**)&externalDataDir);

    String absolutePath;
    externalDataDir->GetAbsolutePath(&absolutePath);

    AutoPtr<IFileHelper> fileHelper;
    CFileHelper::AcquireSingleton((IFileHelper**)&fileHelper);
    String separator;
    fileHelper->GetSeparator(&separator);

    AutoPtr<IFile> cameraDataDir;
    CFile::New(absolutePath + separator + CAPTURE_IMAGE_DIRECTORY, (IFile**)&cameraDataDir);

    Boolean exist;
    cameraDataDir->Exists(&exist);

    Boolean mkdir;
    cameraDataDir->Mkdirs(&mkdir);
    if (!exist && !mkdir) {
        cameraDataDir = externalDataDir;
    }

    cameraDataDir->GetAbsolutePath(&absolutePath);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 currTimeMill = 0;
    system->GetCurrentTimeMillis(&currTimeMill);

    String strCurrTimeMill = IntegralToString::ToString(currTimeMill);
    AutoPtr<IFile> photoFile;
    CFile::New(absolutePath + separator + strCurrTimeMill + String(".jpg"), (IFile**)&photoFile);
    return photoFile;
}

ECode SelectFileDialog::OnFileNotSelected()
{
    // ==================before translated======================
    // nativeOnFileNotSelected(mNativeSelectFileDialog);

    NativeOnFileNotSelected(mNativeSelectFileDialog);
    return NOERROR;
}

Boolean SelectFileDialog::NoSpecificType()
{
    // ==================before translated======================
    // // We use a single Intent to decide the type of the file chooser we display to the user,
    // // which means we can only give it a single type. If there are multiple accept types
    // // specified, we will fallback to a generic chooser (unless a capture parameter has been
    // // specified, in which case we'll try to satisfy that first.
    // return mFileTypes.size() != 1 || mFileTypes.contains(ANY_TYPES);

    assert(0);
    Int32 size = 0;
    mFileTypes->GetSize(&size);
    AutoPtr<ICharSequence> charSequence;
    CString::New(ANY_TYPES, (ICharSequence**)&charSequence);
    Boolean isContain = FALSE;
    mFileTypes->Contains(charSequence, &isContain);
    return (size != 1 || isContain);
}

Boolean SelectFileDialog::ShouldShowTypes(
    /* [in] */ const String& allTypes,
    /* [in] */ const String& specificType)
{
    // ==================before translated======================
    // if (noSpecificType() || mFileTypes.contains(allTypes)) return true;
    // return acceptSpecificType(specificType);

    AutoPtr<ICharSequence> charSequence;
    CString::New(allTypes, (ICharSequence**)&charSequence);
    Boolean isContain = FALSE;
    mFileTypes->Contains(charSequence, &isContain);
    if (NoSpecificType() || isContain)
        return TRUE;
    return AcceptSpecificType(specificType);
}

Boolean SelectFileDialog::ShouldShowImageTypes()
{
    // ==================before translated======================
    // return shouldShowTypes(ALL_IMAGE_TYPES, IMAGE_TYPE);

    return ShouldShowTypes(ALL_IMAGE_TYPES, IMAGE_TYPE);
}

Boolean SelectFileDialog::ShouldShowVideoTypes()
{
    // ==================before translated======================
    // return shouldShowTypes(ALL_VIDEO_TYPES, VIDEO_TYPE);

    return ShouldShowTypes(ALL_VIDEO_TYPES, VIDEO_TYPE);
}

Boolean SelectFileDialog::ShouldShowAudioTypes()
{
    // ==================before translated======================
    // return shouldShowTypes(ALL_AUDIO_TYPES, AUDIO_TYPE);

    return ShouldShowTypes(ALL_AUDIO_TYPES, AUDIO_TYPE);
}

Boolean SelectFileDialog::AcceptsSpecificType(
    /* [in] */ const String& type)
{
    // ==================before translated======================
    // return mFileTypes.size() == 1 && TextUtils.equals(mFileTypes.get(0), type);

    Int32 size = 0;
    mFileTypes->GetSize(&size);

    AutoPtr<IInterface> itemTmp;
    mFileTypes->Get(0, (IInterface**)&itemTmp);
    ICharSequence* charSequence = ICharSequence::Probe(itemTmp);

    String item;
    charSequence->ToString(&item);
    return (size == 1 && TextUtils::Equals(item, type));
}

Boolean SelectFileDialog::CaptureCamera()
{
    // ==================before translated======================
    // return mCapture && acceptsSpecificType(ALL_IMAGE_TYPES);

    return (mCapture && AcceptsSpecificType(ALL_IMAGE_TYPES));
}

Boolean SelectFileDialog::CaptureCamcorder()
{
    // ==================before translated======================
    // return mCapture && acceptsSpecificType(ALL_VIDEO_TYPES);

    return (mCapture && AcceptsSpecificType(ALL_VIDEO_TYPES));
}

Boolean SelectFileDialog::CaptureMicrophone()
{
    // ==================before translated======================
    // return mCapture && acceptsSpecificType(ALL_AUDIO_TYPES);

    return (mCapture && AcceptsSpecificType(ALL_AUDIO_TYPES));
}

Boolean SelectFileDialog::AcceptSpecificType(
    /* [in] */ const String& accept)
{
    // ==================before translated======================
    // for (String type : mFileTypes) {
    //     if (type.startsWith(accept)) {
    //         return true;
    //     }
    // }
    // return false;

    Int32 size = 0;
    mFileTypes->GetSize(&size);

    AutoPtr<IInterface> interfaceTmp;
    AutoPtr<ICharSequence> charSequenceTmp;
    String type;
    for (Int32 i=0; i<size; ++i) {
        mFileTypes->Get(i, (IInterface**)&interfaceTmp);
        charSequenceTmp = ICharSequence::Probe(interfaceTmp);
        charSequenceTmp->ToString(&type);
        if (type.StartWith(accept)) {
            return TRUE;
        }
    }

    return FALSE;
}

AutoPtr<IInterface> SelectFileDialog::Create(
    /* [in] */ Int64 nativeSelectFileDialog)
{
    // ==================before translated======================
    // return new SelectFileDialog(nativeSelectFileDialog);

    AutoPtr<SelectFileDialog> empty = new SelectFileDialog(nativeSelectFileDialog);
    return TO_IINTERFACE(empty);
}

ECode SelectFileDialog::NativeOnFileSelected(
    /* [in] */ Int64 nativeSelectFileDialogImpl,
    /* [in] */ const String& filePath,
    /* [in] */ const String& displayName)
{
    Elastos_SelectFileDialog_nativeOnFileSelected(THIS_PROBE(IInterface), (Handle32)nativeSelectFileDialogImpl, filePath, displayName);
    return NOERROR;
}

ECode SelectFileDialog::NativeOnFileNotSelected(
    /* [in] */ Int64 nativeSelectFileDialogImpl)
{
    Elastos_SelectFileDialog_nativeOnFileNotSelected(THIS_PROBE(IInterface), (Handle32)nativeSelectFileDialogImpl);
    return NOERROR;
}

void SelectFileDialog::SelectFile(
    /* [in] */ IInterface* obj,
    /* [in] */ ArrayOf<String>* fileTypes,
    /* [in] */ Boolean capture,
    /* [in] */ IInterface* window)
{
    SelectFileDialog* mObj = (SelectFileDialog*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("SelectFileDialog", "SelectFileDialog::SelectFile, mObj is NULL");
        return;
    }
    WindowElastos* w = (WindowElastos*)(IObject::Probe(window));
    mObj->SelectFile(fileTypes, capture, w);
}

} // namespace Base
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


