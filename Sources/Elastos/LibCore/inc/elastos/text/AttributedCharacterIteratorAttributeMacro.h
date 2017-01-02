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

#ifndef __TEXTATTRIBUITEDCHARACTERITERATORATTRIBUTEMACRO_H__
#define __TEXTATTRIBUITEDCHARACTERITERATORATTRIBUTEMACRO_H__

#define TEXTATTRIBUITEDCHARACTERITERATORATTRIBUTE_METHODS_DECL()                                  \
                                                                                                  \
    CARAPI GetName(                                                                               \
        /* [out] */ String* name);                                                                \
                                                                                                  \
    CARAPI Equals(                                                                                \
        /* [in] */ IInterface * obj,                                                              \
        /* [out] */ Boolean * value);                                                             \
                                                                                                  \
    CARAPI GetHashCode(                                                                              \
        /* [out] */ Int32 * value);                                                               \
                                                                                                  \
    CARAPI ReadResolve(                                                                           \
        /* [out] */ IInterface** resolve);                                                        \
                                                                                                  \
    CARAPI ToString(                                                                              \
        /* [out] */ String * str);

#define TEXTATTRIBUITEDCHARACTERITERATORATTRIBUTE_METHODS_IMPL(className, superClass)             \
                                                                                                  \
    ECode className::GetName(                                                                     \
        /* [out] */ String* name)                                                                 \
    {                                                                                             \
        return superClass::GetName(name);                                                         \
    }                                                                                             \
                                                                                                  \
    ECode className::Equals(                                                                      \
            /* [in] */ IInterface * obj,                                                          \
            /* [out] */ Boolean * value)                                                          \
    {                                                                                             \
        return superClass::Equals(obj,value);                                                     \
    }                                                                                             \
                                                                                                  \
    ECode className::GetHashCode(                                                                    \
            /* [out] */ Int32 * value)                                                            \
    {                                                                                             \
        return superClass::GetHashCode(value);                                                       \
    }                                                                                             \
                                                                                                  \
    ECode className::ReadResolve(                                                                 \
            /* [out] */ IInterface** resolve)                                                     \
    {                                                                                             \
        return superClass::ReadResolve(resolve);                                                  \
    }                                                                                             \
                                                                                                  \
    ECode className::ToString(                                                                    \
            /* [out] */ String * str)                                                             \
    {                                                                                             \
        return superClass::ToString(str);                                                         \
    }                                                                                             \

#endif  //__TEXTATTRIBUITEDCHARACTERITERATORATTRIBUTEMACRO_H__
