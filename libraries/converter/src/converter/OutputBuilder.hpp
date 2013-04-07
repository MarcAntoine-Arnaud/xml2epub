#include "OutputState.hpp"

#pragma once

namespace xml2epub
{

class OutputBuilder
{
public:
	virtual ~OutputBuilder() {}
	virtual OutputState* create_root() = 0;
};

}
