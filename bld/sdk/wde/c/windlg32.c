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
#include "windlg.h"
#include "windlg32.h"
#include "wdedlgut.h"


/*
 * DialogTemplate - build a dialog template
 */
TEMPLATE_HANDLE DialogEXTemplate( DWORD style, DWORD exstyle, DWORD helpid,
                               int x, int y, int cx, int cy, const char *menuname,
                               const char *classname, const char *captiontext, WORD pointsize,
                               const char *typeface, WORD fontweight, BYTE fontitalic, BYTE fontcharset, size_t *templatelen )
{
    TEMPLATE_HANDLE     data;
    UINT                blocklen, menulen, classlen, captionlen, typefacelen;
    WPCHAR              databytes;
    WPCHAR              dlgtemp;
    WPDLGTEMPLATEEX     dt;
    WPFONTINFOEX        fi;

    *templatelen = 0;

    /*
     * get size of block and allocate memory
     */
    menulen = DlgStringLength( menuname );
    classlen = DlgStringLength( classname );
    captionlen = DlgStringLength( captiontext );

    blocklen = sizeof( WDLGTEMPLATEEX ) + menulen + classlen + captionlen;

    if( style & DS_SETFONT ) {
        typefacelen = DlgStringLength( typeface );
        blocklen += sizeof( WFONTINFOEX ) + typefacelen;
    } else {
        typefacelen = 0;
    }

    data = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, blocklen );
    if( data == NULL ) {
        return( NULL );
    }

    databytes = GetPtrGlobalLock( data );
    *templatelen = blocklen;

    /*
     * set up template
     */
    dt = (WPDLGTEMPLATEEX)databytes;

    dt->dtVer = 0x0001;                 // signature dword is 0xffff0001
    dt->dtSignature = 0xffff;
    dt->dtHelpID = helpid;
    dt->dtExtendedStyle = exstyle;
    dt->dtStyle = style;
    dt->dtItemCount = 0;

    dt->dtX = x;
    dt->dtY = y;
    dt->dtCX = cx;
    dt->dtCY = cy;

    dlgtemp = (WPCHAR)( dt + 1 );

    /*
     * add extra strings to block
     */
    dlgtemp = DlgCopyMBString( dlgtemp, menuname, menulen );
    dlgtemp = DlgCopyMBString( dlgtemp, classname, classlen );
    dlgtemp = DlgCopyMBString( dlgtemp, captiontext, captionlen );

    /*
     * add font data (if needed)
     */
    if( style & DS_SETFONT ) {
        fi = (WPFONTINFOEX)dlgtemp;
        fi->PointSize = pointsize;
        fi->weight = fontweight;
        fi->bItalic = fontitalic;
        fi->bCharset = fontcharset;
        dlgtemp = (WPCHAR)( fi + 1 );
        dlgtemp = DlgCopyMBString( dlgtemp, typeface, typefacelen );
    }

    GlobalUnlock( data );
    return( data );

} /* DialogEXTemplate */

/*
 * AddControlEX - add a control to a dialog
 */
TEMPLATE_HANDLE AddControlEX( TEMPLATE_HANDLE data, int x, int y, int cx, int cy, DWORD id, DWORD style,
                           DWORD exstyle, DWORD helpid, const char *class, const char *text,
                           BYTE infolen, const BYTE *infodata, size_t *templatelen )
{
    TEMPLATE_HANDLE     new;
    UINT                blocklen, classlen, textlen;
    WPCHAR              databytes;
    WPDLGTEMPLATEEX     dt;
    WPDLGITEMTEMPLATEEX dit;
    WPCHAR              ditstr;
    unsigned char       class_ordinal;
    size_t              item_start;

    /*
     * compute size of block, reallocate block to hold this stuff
     */

    class_ordinal = DlgGetClassOrdinal( class );
    if( class_ordinal > 0 ) {
#if defined( __WINDOWS__ )
        classlen = 1;
#else
        classlen = 4;
#endif
    } else {
        classlen = DlgStringLength( class );
    }
    textlen = DlgStringLength( text );

    item_start = *templatelen;
    ADJUST_DLGLEN( item_start );
#if defined( __WINDOWS__ )
    blocklen = item_start + sizeof( WDLGITEMTEMPLATEEX ) + classlen + textlen + sizeof( BYTE ) + infolen;
#else
    blocklen = item_start + sizeof( WDLGITEMTEMPLATEEX ) + classlen + textlen + sizeof( WORD ) + infolen;
#endif
    GlobalUnlock( data );
    new = GlobalReAlloc( data, blocklen, GMEM_MOVEABLE | GMEM_ZEROINIT );
    if( new == NULL ) {
        return( NULL );
    }

    databytes = GetPtrGlobalLock( new );

    /*
     * one more item...
     */
    dt = (WPDLGTEMPLATEEX)databytes;
    dt->dtItemCount++;

    /*
     * point to start of item template, and set up values
     */
    dit = (WPDLGITEMTEMPLATEEX)( databytes + item_start );
    dit->ditHelpID = helpid;
    dit->ditExtendedStyle = exstyle;
    dit->ditStyle = style;
    dit->ditX = x;
    dit->ditY = y;
    dit->ditCX = cx;
    dit->ditCY = cy;
    dit->ditID = id;

    ditstr = (WPCHAR)( dit + 1 );

    /*
     * append extra data
     */

    if( class_ordinal > 0 ) {
#if defined( __WINDOWS__ )
        *ditstr++ = class_ordinal;
#else
        ditstr = DlgCopyWord( ditstr, -1 );
        ditstr = DlgCopyWord( ditstr, class_ordinal );
#endif
    } else {
        ditstr = DlgCopyMBString( ditstr, class, classlen );
    }
    ditstr = DlgCopyMBString( ditstr, text, textlen );
#if defined( __WINDOWS__ )
    *ditstr++ = infolen;
#else
    ditstr = DlgCopyWord( ditstr, infolen );
#endif
    _FARmemcpy( ditstr, infodata, infolen );
    ditstr += infolen;

    *templatelen = (size_t)( ditstr - databytes );

    GlobalUnlock( new );
    return( new );

} /* AddControlEX */
