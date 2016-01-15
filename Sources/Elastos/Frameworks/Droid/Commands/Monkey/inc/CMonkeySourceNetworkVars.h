/*
 * Copyright 2009, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CMONKEYSOURCENETWORKVARS_H__
#define __CMONKEYSOURCENETWORKVARS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_CMonkeySourceNetworkVars.h"
#include <elautoptr.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceNetworkVars)
{
class VarGetter;
public:
    class ListVarCommand : public ElRefBase, public IMonkeyCommand
    {
    public:
        CAR_INTERFACE_DECL();
        // listvar
        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    /**
     * Command to get the value of a var.
     */
     class GetVarCommand : public ElRefBase, public IMonkeyCommand
     {
     public:
        CAR_INTERFACE_DECL();
        // getvar varname
        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);

     };

     static AutoPtr<HashMap<String, AutoPtr<VarGetter> > > InitStatics();

private:
    class VarGetter : public ElRefBase {
         public:
         /**
         * Get the value of the var.
         * @returns the value of the var.
         */
            virtual String Get() = 0;
    };

    class StaticVarGetter : public VarGetter {
        public:
            StaticVarGetter(
                /* [in] */ const String& value);

            String Get();

        private:
            const String mValue;
    };

private:
    static AutoPtr<HashMap<String, AutoPtr<VarGetter> > > VAR_MAP;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCENETWORKVARS_H__
