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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/utility/CJournaledFile.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CJournaledFile, Object, IJournaledFile)

CAR_OBJECT_IMPL(CJournaledFile)

CJournaledFile::CJournaledFile()
    : mWriting(FALSE)
{
}

ECode CJournaledFile::constructor(
    /* [in] */ IFile* real,
    /* [in] */ IFile* temp)
{
    mReal = real;
    mTemp = temp;
    return NOERROR;
}

ECode CJournaledFile::ChooseForRead(
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    AutoPtr<IFile> result;
    Boolean rExists, tExists;
    mReal->Exists(&rExists);
    mTemp->Exists(&tExists);
    if (rExists) {
        result = mReal;
        if (tExists) {
            Boolean res;
            mTemp->Delete(&res);
        }
    }
    else if (tExists) {
        result = mTemp;
        Boolean res;
        mTemp->RenameTo(mReal, &res);
    }
    else {
        result = mReal;
    }
    *file = result;
    REFCOUNT_ADD(*file);
    return NOERROR;
}

ECode CJournaledFile::ChooseForWrite(
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    if (mWriting) {
        Logger::E("CJournaledFile", "uncommitted write already in progress");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Boolean rExists, tExists;
    mReal->Exists(&rExists);
    mTemp->Exists(&tExists);
    if (!rExists) {
        // If the real one doesn't exist, it's either because this is the first time
        // or because something went wrong while copying them.  In this case, we can't
        // trust anything that's in temp.  In order to have the chooseForRead code not
        // use the temporary one until it's fully written, create an empty file
        // for real, which will we'll shortly delete.
        Boolean res;
        mReal->CreateNewFile(&res);
    }

    if (tExists) {
        Boolean res;
        mTemp->Delete(&res);
    }
    mWriting = TRUE;
    *file = mTemp;
    REFCOUNT_ADD(*file);
    return NOERROR;
}

ECode CJournaledFile::Commit()
{
    if (!mWriting) {
        Logger::E("CJournaledFile", "no file to commit");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mWriting = FALSE;
    Boolean res;
    return mTemp->RenameTo(mReal, &res);
}

ECode CJournaledFile::Rollback()
{
    if (!mWriting) {
        Logger::E("CJournaledFile", "no file to roll back");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mWriting = FALSE;
    Boolean res;
    return mTemp->Delete(&res);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
