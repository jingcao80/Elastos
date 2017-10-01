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

#include "elastos/droid/test/EcoFile.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::EIID_IComparable;
using Elastos::Utility::EIID_IEnumeration;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Test {

CAR_INTERFACE_IMPL(EcoFile::ClassInfo, Object, IComparable)

ECode EcoFile::ClassInfo::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<ClassInfo> res = (ClassInfo*)IObject::Probe(another);
    if (res == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = !(this == res);
    return NOERROR;
}


EcoFile::EFEnum::EFEnum(
    /* [in] */ EcoFile* ef)
    : mIndex(0)
{
    mClassInfoList = EcoFile::GetClassInfoList(ef->mEcoModule);
}

CAR_INTERFACE_IMPL(EcoFile::EFEnum, Object, IEnumeration)

ECode EcoFile::EFEnum::HasMoreElements(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIndex < mClassInfoList->GetLength();
    return NOERROR;
}

ECode EcoFile::EFEnum::GetNextElement(
    /* [out] */ IInterface** element)
{
    VALIDATE_NOT_NULL(element);
    *element = (IObject*)(*mClassInfoList)[mIndex++];
    REFCOUNT_ADD(*element);
    return NOERROR;
}


const String EcoFile::TAG("EcoFile");

EcoFile::EcoFile()
{}

EcoFile::~EcoFile()
{
    Close();
}

ECode EcoFile::NewEcoFile(
    /* [in] */ const String& fileName,
    /* [out] */ EcoFile** ecoFile)
{
    AutoPtr<EcoFile> eco = new EcoFile();
    ECode ec = eco->Init(fileName);
    if (FAILED(ec)) {
        *ecoFile = NULL;
        return ec;
    }
    *ecoFile = eco;
    REFCOUNT_ADD(*ecoFile);
    return NOERROR;
}

AutoPtr<IEnumeration> EcoFile::Entries()
{
    AutoPtr<IEnumeration> entries = new EFEnum(this);
    return entries;
}

ECode EcoFile::Close()
{
    //TODO:
    return NOERROR;
}

ECode EcoFile::Init(
    /* [in] */ const String& fileName)
{
    FAIL_RETURN(OpenEcoFile(fileName, (IModuleInfo**)&mEcoModule));
    mFileName = fileName;
    return NOERROR;
}

AutoPtr< ArrayOf<EcoFile::ClassInfo*> > EcoFile::GetClassInfoList(
    /* [in] */ IModuleInfo* ecoModule)
{
    AutoPtr< ArrayOf<ClassInfo*> > classInfoList;
    Int32 count;
    ecoModule->GetClassCount(&count);
    if (count <= 0) {
        classInfoList = ArrayOf<ClassInfo*>::Alloc(0);
        return classInfoList;
    }

    AutoPtr< ArrayOf<IClassInfo*> > classInfos = ArrayOf<IClassInfo*>::Alloc(count);
    ecoModule->GetAllClassInfos(classInfos);
    classInfoList = ArrayOf<ClassInfo*>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ClassInfo> classInfo = new ClassInfo((*classInfos)[i]);
        classInfoList->Set(i, classInfo.Get());
    }
    return classInfoList;
}

ECode EcoFile::OpenEcoFile(
    /* [in] */ const String& sourceName,
    /* [out] */ IModuleInfo** ecoModule)
{
    String fileName = sourceName;
    Int32 start = fileName.LastIndexOf("/");
    Int32 end = fileName.LastIndexOf("-");
    fileName = fileName.Substring(start >= 0 ? start + 1 : 0, end >= 0 ? end : fileName.GetLength() - 4)  + String(".eco");
    String path = String("/data/elastos/") + fileName;
    AutoPtr<IModuleInfo> moduleInfo;
    if (FAILED(CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo))) {
        *ecoModule = NULL;
        Slogger::E(TAG, "OpenEcoFile: Cann't Find the Instrumentation path is %s", path.string());
        return E_RUNTIME_EXCEPTION;
    }
    *ecoModule = moduleInfo;
    REFCOUNT_ADD(*ecoModule);
    return NOERROR;
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
