
#include "elastos/droid/teleservice/services/telephony/sip/SipProfileDb.h"
#include <elastos/core/AutoLock.h>

using Elastos::IO::CFile;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

const String SipProfileDb::PREFIX("[SipProfileDb] ");
const Boolean SipProfileDb::VERBOSE = FALSE; /* STOP SHIP if true */

const String SipProfileDb::PROFILES_DIR("/profiles/");
const String SipProfileDb::PROFILE_OBJ_FILE(".pobj");

Object SipProfileDb::sLock;

SipProfileDb::SipProfileDb(
    /* [in] */ IContext* context)
    : mProfilesCount(-1)
{
    AutoPtr<IFile> filesDir;
    context->GetFilesDir((IFile**)&filesDir);
    String path;
    filesDir->GetAbsolutePath(&path);
    mProfilesDirectory = path + PROFILES_DIR;
    assert(0);
    //mSipSharedPreferences = new SipSharedPreferences(context);
}

// ECode DeleteProfile(
//     /* [in] */ SipProfile p);

// ECode SaveProfile(
//     /* [in] */ SipProfile p);

ECode SipProfileDb::GetProfilesCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    assert(0);
    //return (mProfilesCount < 0) ?  mSipSharedPreferences.getProfilesCount() : mProfilesCount;
    return NOERROR;
}

ECode SipProfileDb::RetrieveSipProfileList(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)

    {
        AutoLock syncLock(sLock);
        *list = RetrieveSipProfileListInternal();
        REFCOUNT_ADD(*list);
    }
    return NOERROR;
}

void SipProfileDb::DeleteProfile(
    /* [in] */ IFile* file)
{
    Boolean isDirectory;
    if (file->IsDirectory(&isDirectory), isDirectory) {
        AutoPtr<ArrayOf<IFile*> > array;
        file->ListFiles((ArrayOf<IFile*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IFile> child = (*array)[i];

            DeleteProfile(child);
        }
    }
    file->Delete();
}

AutoPtr<IList> SipProfileDb::RetrieveSipProfileListInternal()
{
    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    AutoPtr<IList> sipProfileList;
    helper->SynchronizedList(list, (IList**)&sipProfileList);

    AutoPtr<IFile> root;
    CFile::New(mProfilesDirectory, (IFile**)&root);
    AutoPtr<ArrayOf<String> > dirs;
    root->List((ArrayOf<String>**)&dirs);
    if (dirs == NULL) return sipProfileList;

    for (Int32 i = 0; i < dirs->GetLength(); i++) {
        String dir = (*dirs)[i];
        AutoPtr<IFile> file;
        CFile::New(root, dir, (IFile**)&file);
        AutoPtr<IFile> f;
        CFile::New(file, PROFILE_OBJ_FILE, (IFile**)&f);
        Boolean res;
        if (f->Exists(&res), !res) continue;

        assert(0);
        // //try
        // {
        //     SipProfile p = deserialize(f);
        //     if (p == null) continue;
        //     if (!dir.equals(p.getProfileName())) continue;

        //     sipProfileList.add(p);
        // }
        // // catch (IOException e) {
        //     Log("retrieveSipProfileListInternal, exception: " + e);
        // //}
    }
    sipProfileList->GetSize(&mProfilesCount);
    assert(0);
    //mSipSharedPreferences.setProfilesCount(mProfilesCount);
    return sipProfileList;
}

// ECode Deserialize(
//     /* [in] */ IFile* profileObjectFile,
//     /* [out] */ SipProfile** profile);

void SipProfileDb::Log(
        /* [in] */ const String& msg)
{
    assert(0);
    //Logger::D(SipUtil::TAG, PREFIX + msg);
}

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos