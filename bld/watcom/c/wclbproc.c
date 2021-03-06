/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WIN16/WIN386 MakeProcInstance.../FreeProcInstance...
*               for callback functions implementation
*
****************************************************************************/


#if defined( __WINDOWS__ )

#define INCLUDE_COMMDLG_H
#include <windows.h>
#include "wclbproc.h"

DLGPROC MakeProcInstance_DLG( DLGPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS_386__ )
    instance = instance;
    return( (DLGPROC)fn );
#else
    return( (DLGPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#endif
}
FONTENUMPROC MakeProcInstance_FONTENUM( FONTENUMPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS_386__ )
    instance = instance;
    return( (FONTENUMPROC)fn );
#else
    return( (FONTENUMPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#endif
}
HOOKPROC MakeProcInstance_HOOK( HOOKPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS_386__ )
    instance = instance;
    return( (HOOKPROC)fn );
#else
    return( (HOOKPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#endif
}
LPOFNHOOKPROC MakeProcInstance_OFNHOOK( LPOFNHOOKPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS_386__ )
    instance = instance;
    return( (LPOFNHOOKPROC)fn );
#else
    return( (LPOFNHOOKPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#endif
}
WNDENUMPROC MakeProcInstance_WNDENUM( WNDENUMPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS_386__ )
    instance = instance;
    return( (WNDENUMPROC)fn );
#else
    return( (WNDENUMPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#endif
}
WNDPROC MakeProcInstance_WND( WNDPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS_386__ )
    instance = instance;
    return( (WNDPROC)fn );
#else
    return( (WNDPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#endif
}
WNDPROC GetWndProc( WNDPROCx fn )
{
    return( (WNDPROC)fn );
}

void FreeProcInstance_DLG( DLGPROC fn )
{
#if defined( __WINDOWS_386__ )
    fn = fn;
#else
    FreeProcInstance( (FARPROC)fn );
#endif
}
void FreeProcInstance_FONTENUM( FONTENUMPROC fn )
{
#if defined( __WINDOWS_386__ )
    fn = fn;
#else
    FreeProcInstance( (FARPROC)fn );
#endif
}
void FreeProcInstance_HOOK( HOOKPROC fn )
{
#if defined( __WINDOWS_386__ )
    fn = fn;
#else
    FreeProcInstance( (FARPROC)fn );
#endif
}
void FreeProcInstance_OFNHOOK( LPOFNHOOKPROC fn )
{
#if defined( __WINDOWS_386__ )
    fn = fn;
#else
    FreeProcInstance( (FARPROC)fn );
#endif
}
void FreeProcInstance_WNDENUM( WNDENUMPROC fn )
{
#if defined( __WINDOWS_386__ )
    fn = fn;
#else
    FreeProcInstance( (FARPROC)fn );
#endif
}
void FreeProcInstance_WND( WNDPROC fn )
{
#if defined( __WINDOWS_386__ )
    fn = fn;
#else
    FreeProcInstance( (FARPROC)fn );
#endif
}

#endif
