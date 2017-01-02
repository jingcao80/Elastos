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

#ifndef __PREFIX_H__
#define __PREFIX_H__

char* GenerateTypePrefix(const char* type);

char* GenerateCalleePrefix(const char* type);

char* GeneratePrefixalVarName(const char* prefix, const char* variable);

char* TrimOutTypeNameBar(const char* type);

char* GetTypeName(const char* type);

#endif /*__PREFIX_H__*/
