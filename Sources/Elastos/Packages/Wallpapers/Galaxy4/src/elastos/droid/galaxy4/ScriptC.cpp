
#include "Elastos.CoreLibrary.IO.h"
#include "RenderScript.h"
#include "ScriptC.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::IO::IInputStream;
using Elastos::IO::IFile;
using Elastos::IO::CFile;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

const String ScriptC::CACHE_PATH("Elastos.Droid.Renderscript.Cache");
String ScriptC::sCachePath;
Object ScriptC::sLock;

ECode ScriptC::constructor(
    /* [in] */ RenderScript* rs,
    /* [in] */ IResources* resources,
    /* [in] */ Int32 resourceID)
{
    FAIL_RETURN(Script::constructor(0, rs));
    Int64 id;
    FAIL_RETURN(InternalCreate(rs, resources, resourceID, &id));
    if (id == 0) {
        // throw new RSRuntimeException("Loading of ScriptC script failed.");
        return E_RUNTIME_EXCEPTION;
    }
    SetID(id);
    return NOERROR;
}

ECode ScriptC::InternalCreate(
    /* [in] */ RenderScript* rs,
    /* [in] */ IResources* resources,
    /* [in] */ Int32 resourceID,
    /* [out] */ Int64* id)
{
    AutoLock lock(sLock);

    AutoPtr< ArrayOf<Byte> > pgm;
    Int32 pgmLength;
    AutoPtr<IInputStream> is;
    resources->OpenRawResource(resourceID, (IInputStream**)&is);
    // try {
    //     try {
    pgm =  ArrayOf<Byte>::Alloc(1024);
    pgmLength = 0;
    while(TRUE) {
        Int32 bytesLeft = pgm->GetLength() - pgmLength;
        if (bytesLeft == 0) {
            AutoPtr< ArrayOf<Byte> > buf2 = ArrayOf<Byte>::Alloc(pgm->GetLength() * 2);
            buf2->Copy(pgm, pgm->GetLength());
            pgm = buf2;
            bytesLeft = pgm->GetLength() - pgmLength;
        }
        Int32 bytesRead;
        ECode ec = is->Read(pgm, pgmLength, bytesLeft, &bytesRead);
        if (FAILED(ec)) {
            is->Close();
            return E_RESOURCES_NOT_FOUND_EXCEPTION;
        }
        if (bytesRead <= 0) {
            break;
        }
        pgmLength += bytesRead;
    }
    is->Close();
    //     } finally {
    //         is.close();
    //     }
    // } catch(IOException e) {
    //     throw new Resources.NotFoundException();
    // }

    String resName;
    resources->GetResourceEntryName(resourceID, &resName);

    // Create the RS cache path if we haven't done so already.
    if (sCachePath == NULL) {
        AutoPtr<IFile> f;
        CFile::New(rs->sCacheDir, CACHE_PATH, (IFile**)&f);
        f->GetAbsolutePath(&sCachePath);
        Boolean result;
        f->Mkdirs(&result);
    }
    //        Log.v(TAG, "Create script for resource = " + resName);
    return rs->nScriptCCreate(resName, sCachePath, pgm, pgmLength, id);
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
