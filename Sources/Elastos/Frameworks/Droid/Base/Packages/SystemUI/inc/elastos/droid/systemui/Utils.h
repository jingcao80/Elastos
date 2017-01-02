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

#ifndef  __ELASTOS_DROID_SYSTEMUI_UTILS_H__
#define  __ELASTOS_DROID_SYSTEMUI_UTILS_H__

#include <elastos/droid/ext/frameworkext.h>

using Elastos::Core::IClassLoader;

_ETL_NAMESPACE_BEGIN
template<> struct Hash<Elastos::InterfaceID>
{
    size_t operator()(const Elastos::InterfaceID& x) const { return size_t(&x); }
};
_ETL_NAMESPACE_END

namespace Elastos {
namespace Droid {
namespace SystemUI {

class Utils
{
public:
    static AutoPtr<IClassInfo> GetClassInfo(
        /* [in] */ const String& className);

private:
    static const String sModulePath;
    static AutoPtr<IClassLoader> sClassLoader;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_UTILS_H__
