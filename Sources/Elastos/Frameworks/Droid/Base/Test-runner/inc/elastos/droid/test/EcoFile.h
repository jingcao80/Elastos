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

#ifndef __ECOFILE__H__
#define __ECOFILE__H__

#include <elastos/droid/ext/frameworkdef.h>
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IComparable;
using Elastos::Utility::IEnumeration;

namespace Elastos {
namespace Droid {
namespace Test {

class EcoFile : public Object
{
public:
    class ClassInfo
        : public Object
        , public IComparable
    {
    public:
        ClassInfo(
            /* [in] */ IClassInfo* classInfo)
            : mClassInfo(classInfo)
        {}

        CAR_INTERFACE_DECL()

        CARAPI CompareTo(
            /* [in] */ IInterface* another,
            /* [out] */ Int32* result);

    public:
        AutoPtr<IClassInfo> mClassInfo;
    };

private:
    class EFEnum
        : public Object
        , public IEnumeration
    {
    public:
        EFEnum(
            /* [in] */ EcoFile* ef);

        CAR_INTERFACE_DECL()

        CARAPI HasMoreElements(
            /* [out] */ Boolean* result);

        CARAPI GetNextElement(
            /* [out] */ IInterface** element);

    private:
        Int32 mIndex;
        AutoPtr< ArrayOf<ClassInfo*> > mClassInfoList;
    };

public:
    ~EcoFile();

    static CARAPI NewEcoFile(
        /* [in] */ const String& fileName,
        /* [out] */ EcoFile** ecoFile);

    CARAPI_(AutoPtr<IEnumeration>) Entries();

    /**
     * Closes the DEX file.
     * <p>
     * This may not be able to release any resources. If classes from this
     * DEX file are still resident, the DEX file can't be unmapped.
     *
     * @throws IOException
     *             if an I/O error occurs during closing the file, which
     *             normally should not happen
     */
    CARAPI Close();

private:
    EcoFile();

    CARAPI Init(
        /* [in] */ const String& fileName);

    /* return a String array with class names */
    static CARAPI_(AutoPtr< ArrayOf<ClassInfo*> >) GetClassInfoList(
        /* [in] */ IModuleInfo* ecoModule);

    static CARAPI OpenEcoFile(
        /* [in] */ const String& sourceName,
        /* [out] */ IModuleInfo** ecoModule);

private:
    const static String TAG;
    AutoPtr<IModuleInfo> mEcoModule;
    String mFileName;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Test::EcoFile::ClassInfo, IInterface)

#endif //__ECOFILE__H__
