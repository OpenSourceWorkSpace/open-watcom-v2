/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "wdeglbl.h"
#include <mbstring.h>
#include "statwnd.h"
#include "wdemsgbx.h"
#include "wdestat.h"
#include "rcstr.gh"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define STATUS_FONTNAME   "Helv"
#define STATUS_POINTSIZE  8
#define MAX_STATUS_TEXT   MAX_NAME
#define STATUS_LINE_PAD   4
#define STATUS1_WIDTH     160

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
bool WdeStatusHookProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool WdeDisplayStatusText( char * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static void      *WdeStatusBar = NULL;
static HWND      WdeStatusWindow = NULL;
static HFONT     WdeStatusFont = NULL;
static char      WdeStatusText[2 * MAX_STATUS_TEXT + 2] = { 0 };
static char      WdeClearStatusText[7] = {
    ' ', STATUS_ESC_CHAR, STATUS_NEXT_BLOCK,
    ' ', STATUS_ESC_CHAR, STATUS_NEXT_BLOCK,
    0
};
static int       WdeStatusDepth = 0;

int WdeGetStatusDepth( void )
{
    return( WdeStatusDepth );
}

void WdeDestroyStatusLine( void )
{
    if( WdeStatusWindow != NULL ) {
        DestroyWindow( WdeStatusWindow );
        StatusWndDestroy( WdeStatusBar );
        StatusWndFini();
    }

    if( WdeStatusFont != NULL ) {
        DeleteObject( WdeStatusFont );
    }
}

bool WdeCreateStatusLine( HWND main, HINSTANCE inst )
{
    RECT                rect;
    LOGFONT             lf;
    TEXTMETRIC          tm;
    HFONT               old_font;
    HDC                 dc;
    status_block_desc   sbd;
    char                *status_font;
    char                *cp;
    int                 point_size;
    bool                use_default;

    memset( &lf, 0, sizeof( LOGFONT ) );
    dc = GetDC( main );
    lf.lfWeight = FW_BOLD;
    use_default = true;

    status_font = WdeAllocRCString( WDE_STATUSFONT );
    if( status_font != NULL ) {
        cp = (char *)_mbschr( (unsigned char *)status_font, '.' );
        if( cp != NULL ) {
            *cp = '\0';
            strcpy( lf.lfFaceName, status_font );
            cp++;
            point_size = atoi( cp );
            use_default = false;
        }
        WdeFreeRCString( status_font );
    }

    if( use_default ) {
        strcpy( lf.lfFaceName, STATUS_FONTNAME );
        point_size = STATUS_POINTSIZE;
    }

    lf.lfHeight = -MulDiv( point_size, GetDeviceCaps( dc, LOGPIXELSY ), 72 );
    WdeStatusFont = CreateFontIndirect( &lf );
    old_font = SelectObject( dc, WdeStatusFont );
    GetTextMetrics( dc, &tm );
    SelectObject( dc, old_font );
    ReleaseDC( main, dc );

    GetClientRect( main, &rect );

    WdeStatusDepth = tm.tmHeight + STATUS_LINE_PAD + VERT_BORDER * 2;
    rect.top = rect.bottom - WdeStatusDepth;

    if( !StatusWndInit( inst, WdeStatusHookProc, 0, (HCURSOR)NULL ) ) {
        return( false );
    }
    WdeStatusBar = StatusWndStart();

    sbd.separator_width = STATUS_LINE_PAD;
    sbd.width = STATUS1_WIDTH;
    sbd.width_is_percent = false;
    sbd.width_is_pixels = true;

    StatusWndSetSeparators( WdeStatusBar, 1, &sbd );

    WdeStatusWindow = StatusWndCreate( WdeStatusBar, main, &rect, inst, NULL );

    if( WdeStatusWindow == NULL ) {
        WdeDisplayErrorMsg( WDE_NOCREATESTATUS );
        return( false );
    }

    /* set the text in the status window */
    WdeSetStatusReadyText();

    GetWindowRect( WdeStatusWindow, &rect );
    WdeStatusDepth = rect.bottom - rect.top;

    return( true );
}


void WdeResizeStatusWindows( RECT *rect )
{
    int ypos;

    if( WdeStatusWindow != NULL ) {
        ypos = ( rect->bottom - rect->top ) - WdeStatusDepth;
        if( ypos < 0 )
            ypos = 0;
        MoveWindow( WdeStatusWindow, 0, ypos, rect->right - rect->left, WdeStatusDepth, TRUE );
    }
}

bool WdeSetStatusReadyText( void )
{
    WdeSetStatusText( NULL, "", false );
    return( WdeSetStatusByID( WDE_READYMSG, 0 ) );
}

bool WdeSetStatusByID( msg_id id1, msg_id id2 )
{
    char        *str1;
    char        *str2;
    bool        ret;

    ret = false;
    str1 = NULL;
    str2 = NULL;

    if( id1 > 0 ) {
        str1 = WdeAllocRCString( id1 );
    }

    if( id2 > 0 ) {
        str2 = WdeAllocRCString( id2 );
    }

    ret = WdeSetStatusText( str1, str2, true );

    if( str1 != NULL ) {
        WdeFreeRCString( str1 );
    }

    if( str2 != NULL ) {
        WdeFreeRCString( str2 );
    }

    return( ret );
}

bool WdeSetStatusText( const char *status1, const char *status2, bool redisplay )
{
    int len;
    int pos;

    /* touch unused vars to get rid of warning */
    _wde_touch( redisplay );

    if( WdeStatusWindow == NULL ) {
        return( true );
    }

    if( status1 != NULL ) {
        len = strlen( status1 );
        if( len > MAX_STATUS_TEXT )
            len = MAX_STATUS_TEXT;
        if( len != 0 ) {
            memcpy( WdeStatusText, status1, len );
            pos = len;
        } else {
            WdeStatusText[0] = ' ';
            pos = 1;
        }
    } else {
        pos = 0;
    }

    if( status2 != NULL ) {
        WdeStatusText[pos++] = STATUS_ESC_CHAR;
        WdeStatusText[pos++] = STATUS_NEXT_BLOCK;
        len = strlen( status2 );
        if( len > MAX_STATUS_TEXT )
            len = MAX_STATUS_TEXT;
        if( len != 0 ) {
            memcpy( WdeStatusText + pos, status2, len );
            WdeStatusText[pos + len] = '\0';
        } else {
            WdeStatusText[pos++] = ' ';
            WdeStatusText[pos] = '\0';
        }
    } else {
        WdeStatusText[pos++] = '\0';
    }

    if( status1 != NULL || status2 != NULL ) {
        return( WdeDisplayStatusText( WdeStatusText ) );
    }

    return( true );
}

bool WdeDisplayStatusText( char *str )
{
    HDC hdc;

    if( WdeStatusWindow != NULL ) {
        hdc = GetDC( WdeStatusWindow );
        if( hdc != (HDC)NULL ) {
            if( str != NULL ) {
                StatusWndDrawLine( WdeStatusBar, hdc, WdeStatusFont, str, DT_ESC_CONTROLLED );
            } else {
                StatusWndDrawLine( WdeStatusBar, hdc, WdeStatusFont, WdeClearStatusText, DT_ESC_CONTROLLED );
            }
            ReleaseDC( WdeStatusWindow, hdc );
        }
    }

    return( true );
}

bool WdeStatusHookProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( hWnd );
    _wde_touch( wParam );
    _wde_touch( lParam );

    if( msg == WM_DESTROY ) {
        WdeStatusWindow = NULL;
    }

    return( false );
}
