
#include "_Elastos.Droid.Commands.Pm.h"

using Elastos::Droid::Commands::Pm::CPm;
using Elastos::Droid::Commands::Pm::IPm;

int main(int argc, char* argv[])
{
    argc--;
    argv++;

    AutoPtr<ArrayOf<String> > argArray = ArrayOf<String>::Alloc(argc);
    for (Int32 i = 0; i < argc; i++) {
        (*argArray)[i] = String(argv[i]);
    }

    AutoPtr<IPm> pm;
    CPm::AcquireSingleton((IPm**)&pm);
    pm->Main(*argArray);

    return 0;
}
