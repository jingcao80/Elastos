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

#include "NewInstanceHelper.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

AutoPtr<IInterface> NewInstanceHelper::NewInstance(
    /* [in] */ IClassLoader* classLoader,
    /* [in] */ const String& className)
{
    assert(0 && "TODO");
    // Class driverClass;
    // if (classLoader == null) {
    //     driverClass = Class.forName(className);
    // } else {
    //     driverClass = classLoader.loadClass(className);
    // }
    // return driverClass.newInstance();
    return NULL;
}

AutoPtr<IClassLoader> NewInstanceHelper::GetClassLoader ()
{
    assert(0 && "TODO");

    // Method m = null;

    // try {
    //     m = Thread.class.getMethod("getContextClassLoader");
    // } catch (NoSuchMethodException e) {
    //     // Assume that we are running JDK 1.1, use the current ClassLoader
    //     return NewInstance.class.getClassLoader();
    // }

    // try {
    //     return (ClassLoader) m.invoke(Thread.currentThread());
    // } catch (IllegalAccessException e) {
    //     // assert(false)
    //     throw new UnknownError(e.getMessage());
    // } catch (InvocationTargetException e) {
    //     // assert(e.getTargetException() instanceof SecurityException)
    //     throw new UnknownError(e.getMessage());
    // }
    return NULL;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
