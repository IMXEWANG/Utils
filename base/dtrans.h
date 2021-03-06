/*****************************************************************************
 Nautilus base Copyright (c) 2015. All Rights Reserved.

 FileName: dtrans.h
 Version: 1.0
 Date: 2015.8.21

 History:
 jeffguo     2015.8.21   1.0     Create
 ******************************************************************************/

#ifndef __BASE_DTRANS_H__
#define __BASE_DTRANS_H__

#include "base.h"
#include "dictionary.h"

namespace base
{

class COMMON_API djson
{
public:
	static std::string dict2str(const base::dictionary* dict);
	static std::string dict2str(const base::dictionary& dict);
	static base::dictionary* str2dict(const std::string& json_str);
	static base::dictionary* str2dict(const char* json_str);
};

/*********************************************************************/

class COMMON_API dxml
{
public:
	static std::string dict2str(const base::dictionary* dict);
	static std::string dict2str(const base::dictionary& dict);
	static base::dictionary* str2dict(const std::string& xml_str);
	static base::dictionary* str2dict(const char* xml_str);
};

}

#endif
