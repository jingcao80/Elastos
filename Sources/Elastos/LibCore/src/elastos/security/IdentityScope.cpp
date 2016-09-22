
#include "IdentityScope.h"
#include "CSecurity.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {

AutoPtr<IdentityScope> IdentityScope::systemScope;
CAR_INTERFACE_IMPL(IdentityScope, Identity, IIdentityScope)
IdentityScope::IdentityScope()
{}

ECode IdentityScope::constructor(
    /* [in] */ const String& name)
{
    return Identity::constructor(name);
}

ECode IdentityScope::constructor(
    /* [in] */ const String& name,
    /* [in] */ IdentityScope* scope) /*throws KeyManagementException*/
{
    return Identity::constructor(name, scope);
}

AutoPtr<IdentityScope> IdentityScope::GetSystemScope()
{
    /*
     * Test shows that the implementation class name is read from security property
     * "system.scope", and the class is only loaded from boot classpath. No default
     * implementation as fallback, i.e., return null if fails to init an instance.
     */
    if (systemScope == NULL) {
        AutoPtr<ISecurity> security;
        CSecurity::AcquireSingleton((ISecurity**)&security);
        String className;
        security->GetProperty(String("system.scope"), &className);
        if(className != NULL){
            assert(0 && "TODO");
            // try {
            //     systemScope = (IdentityScope) Class.ForName(className).newInstance();
            // } catch (Exception e) {
            //     e.printStackTrace();
            // }
        }
    }
    return systemScope;
}

void IdentityScope::SetSystemScope(
    /* [in] */ IdentityScope* scope)
{
    systemScope = scope;
}

ECode IdentityScope::GetIdentity(
    /* [in] */ IPrincipal* principal,
    /* [out] */ IIdentity** result)
{
    VALIDATE_NOT_NULL(result);
    String name;
    principal->GetName(&name);
    return GetIdentity(name, result);
}

ECode IdentityScope::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String s;
    Identity::ToString(&s);
    StringBuilder sb(s);
    sb.Append("[");
    Int32 size = 0;
    GetSize(&size);
    sb.Append(size);
    sb.Append("]");
    return sb.ToString(result);
}


} // namespace Security
} // namespace Elastos
