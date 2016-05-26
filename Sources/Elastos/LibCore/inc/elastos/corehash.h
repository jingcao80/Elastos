
#ifndef __ELASTOS_CORE_HASH_H__
#define __ELASTOS_CORE_HASH_H__

#include "elastos/core/Object.h"

//
// Elastos.Core
//
#ifdef HASH_FOR_CORE
#include <Elastos.CoreLibrary.Core.h>
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Core::IInteger32)
#endif

//
// Elastos.Text
//
#ifdef HASH_FOR_TEXT
#include <Elastos.CoreLibrary.Text.h>
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Text::IAttributedCharacterIteratorAttribute)
#endif

//
// Elastos.Utility
//
#ifdef HASH_FOR_UTILITY
#include <Elastos.CoreLibrary.Utility.h>
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Utility::ILocale)
#endif

#endif // __ELASTOS_CORE_HASH_H__
