
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

