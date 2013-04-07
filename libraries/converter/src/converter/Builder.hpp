#include <string>
#pragma once

namespace xml2epub
{

class OutputState
{
public:
	virtual ~OutputState() {}
	virtual void put_text( const std::string & str ) = 0;
	virtual OutputState * bold() = 0;
	virtual OutputState * math() = 0;
	virtual OutputState * section( const std::string & section_name, unsigned int level ) = 0;
	virtual OutputState * plot() = 0;
	virtual void finish() = 0;
};

class OutputBuilder
{
public:
	virtual ~OutputBuilder() {}
	virtual OutputState * create_root() = 0;
};

}
