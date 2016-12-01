
#include "org/alljoyn/bus/Translator.h"
#include "org/alljoyn/bus/NativeTranslator.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(Translator, Object, ITranslator)

Translator::Translator()
    : mHandle(0)
{}

Translator::~Translator()
{
    Destroy();
}

ECode Translator::constructor()
{
    Create();
    return NOERROR;
}

void Translator::Create()
{
    mHandle = reinterpret_cast<Int64>(new NativeTranslator((ITranslator*)this));
}

void Translator::Destroy()
{
    if (mHandle != 0) {
        NativeTranslator* t = reinterpret_cast<NativeTranslator*>(mHandle);
        delete t;
        mHandle = 0;
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

