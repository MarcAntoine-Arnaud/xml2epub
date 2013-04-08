#include "OutputBuilder.hpp"
#include "HtmlState.hpp"

#include <libxml++/libxml++.h>

#include <iostream>

#pragma once

namespace xml2epub
{

class HtmlState;

class HtmlBuilder : public OutputBuilder
{
public:
	HtmlBuilder( std::ostream& output_stream );
	virtual ~HtmlBuilder();
	OutputState* create_root();

	HtmlState getState()
	{
		return m_htmlState;
	}

private:
	friend class HtmlState;
	xmlpp::Document out_doc;
	xmlpp::Element* out_root;
	std::ostream&   m_out;
	HtmlState*      m_root;

	HtmlState       m_htmlState;
};

}
