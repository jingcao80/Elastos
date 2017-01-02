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

#ifndef __Elastos_DevSamples_SecurityDemo_CipherTest_H__
#define __Elastos_DevSamples_SecurityDemo_CipherTest_H__

#include "Elastos.DevSamples.SecurityDemo.h"

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

class CipherTest
{
public:
    static CARAPI Test();

private:
    static CARAPI_(void) AES_CBC_NoPaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) AES_CBC_PKCS5PaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) AES_ECB_NoPaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) AES_ECB_PKCS5PaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) AES_CFB_NoPaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) AES_CTR_NoPaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) AES_OFB_NoPaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) DESEDE_ECB_NoPaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) DESEDE_ECB_PKCS5PaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) DESEDE_CBC_NoPaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) DESEDE_CBC_PKCS5PaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) DESEDE_CFB_NoPaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) DESEDE_OFB_NoPaddingTest(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

    static CARAPI_(void) ARC4Test(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* key);

private:
     static const String IV16;
     static const String IV8;
};

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_SecurityDemo_CipherTest_H__
