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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/gesture/GestureLibraries.h"

using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Gesture {

AutoPtr<IGestureLibrary> GestureLibraries::FromFile(
    /* [in] */ const String& path)
{
    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    return FromFile(file);
}

AutoPtr<IGestureLibrary> GestureLibraries::FromFile(
    /* [in] */ IFile *path)
{
    AutoPtr<FileGestureLibrary> fgl = new FileGestureLibrary(path);
    REFCOUNT_ADD((IGestureLibrary*)fgl);
    return (IGestureLibrary*)fgl;
}

AutoPtr<IGestureLibrary> GestureLibraries::FromPrivateFile(
    /* [in] */ IContext *context,
    /* [in] */ const String& name)
{
    AutoPtr<IFile> file;
    context->GetFileStreamPath(name, (IFile**)&file);
    return FromFile(file);
}

AutoPtr<IGestureLibrary> GestureLibraries::FromRawResource(
    /* [in] */ IContext *context,
    /* [in] */ Int32 resourceId)
{
    AutoPtr<ResourceGestureLibrary> rgl = new ResourceGestureLibrary(context, resourceId);
    REFCOUNT_ADD((IGestureLibrary*)rgl);
    return (IGestureLibrary*)rgl;
}

GestureLibraries::FileGestureLibrary::FileGestureLibrary(
    /* [in] */ IFile *path)
    : mPath(path)
{}

GestureLibraries::FileGestureLibrary::FileGestureLibrary()
{}

GestureLibraries::FileGestureLibrary::~FileGestureLibrary()
{}

ECode GestureLibraries::FileGestureLibrary::IsReadOnly(
    /* [out] */ Boolean *isReadOnly)
{
    VALIDATE_NOT_NULL(isReadOnly);
    Boolean b;
    mPath->CanWrite(&b);
    *isReadOnly = !b;
    return NOERROR;
}

ECode GestureLibraries::FileGestureLibrary::Save(
    /* [out] */ Boolean *isSaved)
{
    VALIDATE_NOT_NULL(isSaved);
    Boolean b;
    mStore->HasChanged(&b);
    if (!b) {
        *isSaved = TRUE;
        return NOERROR;
    }

    AutoPtr<IFile> file = mPath;
    AutoPtr<IFile> parentFile;
    file->GetParentFile((IFile**)&parentFile);
    parentFile->Exists(&b);
    if (!b) {
        parentFile->Mkdirs(&b);
        if (!b) {
            *isSaved = FALSE;
            return NOERROR;
        }
    }

    Boolean result = FALSE;
    //try {
    //noinspection ResultOfMethodCallIgnored
    file->CreateNewFile(&b);
    AutoPtr<IFileOutputStream> fos;
    ECode ec = CFileOutputStream::New(file, (IFileOutputStream**)&fos);
    if (FAILED(ec)) {
        *isSaved = FALSE;
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    AutoPtr<IOutputStream> stream;
    stream = IOutputStream::Probe(fos);

    mStore->Save(stream, TRUE);
    result = TRUE;
    //} catch (FileNotFoundException e) {
    //    Log.d(myLOG_TAG, "Could not save the gesture library in " + mPath, e);
    //} catch (IOException e) {
    //    Log.d(myLOG_TAG, "Could not save the gesture library in " + mPath, e);
    //}
    *isSaved = result;
    return NOERROR;
}

ECode GestureLibraries::FileGestureLibrary::Load(
    /* [out] */ Boolean *isLoaded)
{
    VALIDATE_NOT_NULL(isLoaded);
    Boolean result = FALSE;
    const AutoPtr<IFile> file = mPath;
    Boolean a, b;
    file->Exists(&a);
    file->CanRead(&b);
    if (a && b) {
        //try {
        AutoPtr<IFileInputStream> fis;
        CFileInputStream::New(file, (IFileInputStream**)&fis);

        AutoPtr<IInputStream> stream;
        stream = IInputStream::Probe(fis);

        mStore->Load(stream, TRUE);
        result = TRUE;
        //} catch (FileNotFoundException e) {
        //    Log.d(myLOG_TAG, "Could not load the gesture library from " + mPath, e);
        //} catch (IOException e) {
        //    Log.d(myLOG_TAG, "Could not load the gesture library from " + mPath, e);
        //}
    }
    *isLoaded = result;
    return NOERROR;
}

ECode GestureLibraries::FileGestureLibrary::SetOrientationStyle(
    /* [in] */ Int32 style)
{
    return GestureLibrary::SetOrientationStyle(style);
}

ECode GestureLibraries::FileGestureLibrary::GetOrientationStyle(
    /* [out] */ Int32 *style)
{
    return GestureLibrary::GetOrientationStyle(style);
}

ECode GestureLibraries::FileGestureLibrary::SetSequenceType(
    /* [in] */ Int32 type)
{
    return GestureLibrary::SetSequenceType(type);
}

ECode GestureLibraries::FileGestureLibrary::GetSequenceType(
    /* [out] */ Int32 *type)
{
    return GestureLibrary::GetSequenceType(type);
}

ECode GestureLibraries::FileGestureLibrary::GetGestureEntries(
    /* [out] */ IList **entries)
{
    return GestureLibrary::GetGestureEntries(entries);
}

ECode GestureLibraries::FileGestureLibrary::Recognize(
    /* [in] */ IGesture *gesture,
    /* [out] */ IArrayList **predictions)
{
    return GestureLibrary::Recognize(gesture, predictions);
}

ECode GestureLibraries::FileGestureLibrary::AddGesture(
    /* [in] */  const String& entryName,
    /* [in] */  IGesture *gesture)
{
    return GestureLibrary::AddGesture(entryName, gesture);
}

ECode GestureLibraries::FileGestureLibrary::RemoveGesture(
    /* [in] */ const String& entryName,
    /* [in] */ IGesture *gesture)
{
    return GestureLibrary::RemoveGesture(entryName, gesture);
}

ECode GestureLibraries::FileGestureLibrary::RemoveEntry(
    /* [in] */ const String& entryName)
{
    return GestureLibrary::RemoveEntry(entryName);
}

ECode GestureLibraries::FileGestureLibrary::GetGestures(
    /* [in] */ const String& entryName,
    /* [out] */ IArrayList **gestures)
{
    return GestureLibrary::GetGestures(entryName, gestures);
}

CARAPI GestureLibraries::FileGestureLibrary::GetLearner(
    /* [out] */ ILearner** learner)
{
    return GestureLibrary::GetLearner(learner);
}

GestureLibraries::ResourceGestureLibrary::ResourceGestureLibrary(
    /* [in] */ IContext *context,
    /* [in] */ Int32 resourceId)
    : mResourceId(resourceId)
{
    AutoPtr<IWeakReference> iwp;

    IWeakReferenceSource::Probe(context)->GetWeakReference((IWeakReference**)&iwp);
    mContext = iwp;
}

GestureLibraries::ResourceGestureLibrary::~ResourceGestureLibrary()
{}

ECode GestureLibraries::ResourceGestureLibrary::constructor(
    /* [in] */ IContext *context,
    /* [in] */ Int32 resourceId)
{
//    GestureLibraries::ResourceGestureLibrary::ResourceGestureLibrary(context, resourceId);
    return NOERROR;
}

ECode GestureLibraries::ResourceGestureLibrary::IsReadOnly(
    /* [out] */ Boolean *isReadOnly)
{
    VALIDATE_NOT_NULL(isReadOnly);
    *isReadOnly = TRUE;
    return NOERROR;
}

ECode GestureLibraries::ResourceGestureLibrary::Save(
    /* [out] */ Boolean *isSaved)
{
    VALIDATE_NOT_NULL(isSaved);
    *isSaved = FALSE;
    return NOERROR;
}

ECode GestureLibraries::ResourceGestureLibrary::Load(
    /* [out] */ Boolean *isLoaded)
{
    VALIDATE_NOT_NULL(isLoaded);
    Boolean result = FALSE;
//    final Context context = mContext.get();
//    if (context != null) {
//        final InputStream in = context.getResources().openRawResource(mResourceId);
        //try {
//        mStore->Load(in, TRUE);
//        result = TRUE;
        //} catch (IOException e) {
        //    Log.d(myLOG_TAG, "Could not load the gesture library from raw resource " +
        //            context.getResources().getResourceName(mResourceId), e);
        //}
//    }
    *isLoaded = result;
    return NOERROR;
}

ECode GestureLibraries::ResourceGestureLibrary::SetOrientationStyle(
    /* [in] */ Int32 style)
{
    return GestureLibrary::SetOrientationStyle(style);
}

ECode GestureLibraries::ResourceGestureLibrary::GetOrientationStyle(
    /* [out] */ Int32 *style)
{
    return GestureLibrary::GetOrientationStyle(style);
}

ECode GestureLibraries::ResourceGestureLibrary::SetSequenceType(
    /* [in] */ Int32 type)
{
    return GestureLibrary::SetSequenceType(type);
}

ECode GestureLibraries::ResourceGestureLibrary::GetSequenceType(
    /* [out] */ Int32 *type)
{
    return GestureLibrary::GetSequenceType(type);
}

ECode GestureLibraries::ResourceGestureLibrary::GetGestureEntries(
    /* [out] */ IList **entries)
{
    return GestureLibrary::GetGestureEntries(entries);
}

ECode GestureLibraries::ResourceGestureLibrary::Recognize(
    /* [in] */ IGesture *gesture,
    /* [out] */ IArrayList **predictions)
{
    return GestureLibrary::Recognize(gesture, predictions);
}

ECode GestureLibraries::ResourceGestureLibrary::AddGesture(
    /* [in] */  const String& entryName,
    /* [in] */  IGesture *gesture)
{
    return GestureLibrary::AddGesture(entryName, gesture);
}

ECode GestureLibraries::ResourceGestureLibrary::RemoveGesture(
    /* [in] */ const String& entryName,
    /* [in] */ IGesture *gesture)
{
    return GestureLibrary::RemoveGesture(entryName, gesture);
}

ECode GestureLibraries::ResourceGestureLibrary::RemoveEntry(
    /* [in] */ const String& entryName)
{
    return GestureLibrary::RemoveEntry(entryName);
}

ECode GestureLibraries::ResourceGestureLibrary::GetGestures(
    /* [in] */ const String& entryName,
    /* [out] */ IArrayList **gestures)
{
    return GestureLibrary::GetGestures(entryName, gestures);
}

CARAPI GestureLibraries::ResourceGestureLibrary::GetLearner(
    /* [out] */ ILearner** learner)
{
    return GestureLibrary::GetLearner(learner);
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
