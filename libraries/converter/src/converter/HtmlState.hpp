#include "OutputState.hpp"
#include "HtmlBuilder.hpp"

#include <libxml++/libxml++.h>

#include <string>
#include <vector>
#include <iostream>

#pragma once

namespace xml2epub
{

class HtmlState : public OutputState
{
protected:
	HtmlState( HtmlBuilder& root, HtmlState& parent, xmlpp::Element& xml_node );
	HtmlState( HtmlBuilder& root, xmlpp::Element& xml_node );
	virtual ~HtmlState();

public:
	void put_text( const std::string& str );
	OutputState* bold();
	OutputState* math();
	OutputState* section( const std::string& section_name, unsigned int level );
	OutputState* plot();
	void finish();

protected:
	friend class HtmlBuilder;
	HtmlBuilder&            m_root;
	HtmlState&              m_parent;
	std::vector<HtmlState*> m_children;
	xmlpp::Element&         m_xml_node;
};

}
