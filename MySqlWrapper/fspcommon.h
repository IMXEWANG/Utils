//
// Copyright Shanghai Flagship Prime Brokerage Technology LTD  2011. 
// All Rights Reserved.
// Room 502, Building 2, No.1690, 
// CaiLun Road, Zhangjiang High-tech Park, Shanghai.
//
// The copyright to the computer program(s) herein
// is the property of Shanghai Flagship Prime Brokerage Technology, china.
// The program(s) may be used and/or copied only with the
// written permission of Shanghai Flagship Prime Brokerage Technology 
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have
// been supplied. This copyright notice must not be
// removed.
//

#ifndef FUANDA_DBACCESS_SPCOMMON_H_
#define FUANDA_DBACCESS_SPCOMMON_H_

#include <string>

// Defined module namespace
#define BEGIN_MODULE_NAMESPACE namespace DBAccess {
#define END_MODULE_NAMESPACE }

namespace std {}

BEGIN_MODULE_NAMESPACE

using namespace std;

// Dll export or import
#if defined(__BCPLUSPLUS__) || (defined(_MSC_VER) && !defined(FUANDA_STATIC_LIBS))
#	define FUANDA_DECLSPEC_EXPORT __declspec(dllexport)
#	define FUANDA_DECLSPEC_IMPORT __declspec(dllimport)
#else
#	define FUANDA_DECLSPEC_EXPORT
#	define FUANDA_DECLSPEC_IMPORT
#endif

#ifdef _WIN32
#pragma warning(disable:4251)
#endif

#define FUANDA_DBACCESS_EXPORT FUANDA_DECLSPEC_EXPORT
#define FUANDA_DBACCESS_IMPORT FUANDA_DECLSPCE_IMPORT

// DBACCESS ERROR CODE LIST
#define SP_CONN_DB_ERRORCODE			 0x010001
#define SP_CLOSE_DB_ERRORCODE			 0x010002
#define SP_RELEASE_SP_ERRORCODE			 0x010003
#define SP_ORA_STMT_ERRORCODE			 0x010004
#define SP_PARAM_DATATYPE_ERRORCODE		 0x010005
#define SP_PARAM_DIRECTION_ERRORCODE	 0x010006
#define SP_ORA_UNKNOW_DATATYPE_ERRORCODE 0x010007
#define SP_ORA_CLOSE_RESULTSET_ERRORCODE 0x010008

#define SP_SYB_CALLINTERFACE_ERRORCODE   0x010009
#define SP_RESULT_ZERO_COLUMN_ERRORCODE	 0x01000A
#define SP_ALLOCATOR_MEMORY_ERRORCODE	 0x01000B
#define SP_SYB_CTBIND_ERRORCODE			 0x01000C
#define SP_SYB_UNKNOW_DATAFMT_ERRORCODE  0x01000D
#define SP_SYB_TYPE_MATCH_ERRORCODE		 0x01000E
#define SP_SYB_ROW_DATA_ERRORCODE		 0x01000F

#define SP_PARAM_INDEX_ERRORCODE		 0x010010
#define SP_PARAM_BOUND_ERRORCODE		 0x010011
#define SP_REG_RECEIVE_ERRORCODE		 0x010012

#if 0
FUANDA_DBACCESS_EXPORT 
string GetDBAccessLastError();

FUANDA_DBACCESS_EXPORT 
string SetDBAccessLastError(int code,...);
#endif
END_MODULE_NAMESPACE

#endif

