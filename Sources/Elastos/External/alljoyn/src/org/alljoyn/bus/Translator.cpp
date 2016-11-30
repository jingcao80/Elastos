
#include "org/alljoyn/bus/Translator.h"
#include "org/alljoyn/bus/NativeTranslator.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(Translator, Object, ITranslator);

Translator::Translator()
    : mHandle(0)
{
    Create();
}

Translator::~Translator()
{
    Destroy();
}

void Translator::Create()
{
    NativeTranslator* nbl = new NativeTranslator(this);
    assert(nbl != NULL);

    mHandle = reinterpret_cast<Int64>(nbl);
}

void Translator::Destroy()
{
    NativeTranslator* nbl = reinterpret_cast<NativeTranslator*>(mHandle);

    assert(nbl != NULL);
    delete nbl;

    mHandle = 0;
    return;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
