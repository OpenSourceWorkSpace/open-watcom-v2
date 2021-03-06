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


#include <string.h>
#include <time.h>
#include "layer0.h"
#include "filefmt.h"
#include "write.h"
#include "resver.h"
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"

bool ResWriteVerBlockHeader( VerBlockHeader * head, bool use_unicode, uint_8 os, WResFileID fid )
/***********************************************************************************************/
/* Writes the header, correcting it for 32 bit alligning */
{
    bool        error;

    error = ResWriteUint16( head->Size, fid );
    if( !error ) {
        error = ResWriteUint16( head->ValSize, fid );
    }
    if( !error && os == WRES_OS_WIN32 ) {
        error = ResWriteUint16( head->Type, fid );
    }
    if( !error ) {
        error = ResWriteString( head->Key, use_unicode, fid );
    }
    if( !error ) {
        error = ResWritePadDWord( fid );
    }

    return( error );
}

size_t ResSizeVerBlockHeader( VerBlockHeader *head, bool use_unicode, uint_8 os )
/*******************************************************************************/
{
    size_t      key_size;
    size_t      padding;
    size_t      fixed_size;

    key_size = strlen( head->Key ) + 1;
    if( use_unicode )
        key_size *= 2;
    if( os == WRES_OS_WIN32 ) {
        /* the NT key field begins 2 bytes from a 32 bit boundary */
        padding = RES_PADDING( key_size + 2, sizeof( uint_32 ) );
        fixed_size = 3 * sizeof( uint_16 );
    } else {
        padding = RES_PADDING( key_size, sizeof( uint_32 ) );
        fixed_size = 2 * sizeof( uint_16 );
    }
    return( fixed_size + key_size + padding );
}

bool ResWriteVerValueItem( VerValueItem *item, bool use_unicode, WResFileID fid )
/*******************************************************************************/
{
    bool            error;

    error = false;
    if( item->IsNum ) {
        error = ResWriteUint16( item->Value.Num, fid );
    } else {
        if( item->strlen == VER_CALC_SIZE ) {
            error = ResWriteString( item->Value.String, use_unicode, fid );
        } else {
            error = ResWriteStringLen( item->Value.String, use_unicode, fid, item->strlen );
        }
    }
    return( error );
}

size_t ResSizeVerValueItem( VerValueItem * item, bool use_unicode )
/*****************************************************************/
{
    size_t  size;

    if( item->IsNum ) {
        size = sizeof( uint_16 );
    } else {
        if( item->strlen == VER_CALC_SIZE ) {
            size = strlen( item->Value.String ) + 1;
        } else {
            size = item->strlen + 1;
        }
        if( use_unicode ) {
            size = ConvToUnicode( size, item->Value.String, NULL );
        }
    }
    return( size );
}

bool ResWriteVerFixedInfo( VerFixedInfo *fixed, WResFileID fid )
/**************************************************************/
{
    fixed->Signature = VER_FIXED_SIGNATURE;
    fixed->StructVer = VER_FIXED_STRUCT_VER;
    fixed->FileDateLow = (uint_32)time( NULL );
    if( WRESWRITE( fid, fixed, sizeof( VerFixedInfo ) ) != sizeof( VerFixedInfo ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}
