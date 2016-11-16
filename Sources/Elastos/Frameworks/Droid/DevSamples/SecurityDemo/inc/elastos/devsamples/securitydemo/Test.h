
#ifndef __Elastos_DevSamples_SecurityDemo_Test_H__
#define __Elastos_DevSamples_SecurityDemo_Test_H__

#include "Elastos.DevSamples.SecurityDemo.h"

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

class SignatureTest
{
public:
    static CARAPI SignatureWithRSA();
    static CARAPI SignatureWithEC();
    static CARAPI SignatureWithDSA();
};

class CipherTest
{
public:
    static CARAPI AesECBNoPadding();
};

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_SecurityDemo_Test_H__
