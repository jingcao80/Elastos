
#include <Elastos.CoreLibrary.h>
#include <ServiceManager.h>
#include <unistd.h>
#include <stdio.h>

#include <cutils/log.h>


using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;

int main(int argv, char* argc[])
{
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(String("HelloWorld"), (ICharSequence**)&cs);
    ServiceManager::GetInstance()->AddService(String("test"), cs);

    sleep(10000000000);

    return 0;
}
