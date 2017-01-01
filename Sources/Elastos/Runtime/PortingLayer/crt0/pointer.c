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

/*
 * used to distinguish whether caller is EXE or different DLLs.
 */
static void * _onexitpointer;

void _crt_dll_setpointer()
{
    _onexitpointer = &_onexitpointer;
}

void _crt_exe_setpointer()
{
    _onexitpointer = (void *)-1;
}

void * _crt_getpointer()
{
    return _onexitpointer;
}

int _crt_is_in_exe()
{
    return _onexitpointer == (void *)-1;
}

