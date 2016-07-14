
#include "Singleton.h"

namespace Elastos {
namespace Core {

Singleton::Singleton()
    : Object()
{
}

Singleton::~Singleton()
{
}

UInt32 Singleton::AddRef()
{
    return Object::AddRef();
}

UInt32 Singleton::Release()
{
    GetSelfSpinLock().Lock();
    Int32 ref = Object::Release();
    if (ref > 0) {
        GetSelfSpinLock().Unlock();
    }
    return (UInt32)ref;
}

} // namespace Core
} // namespace Elastos
