#include <string>
#include <vector>
#include <iostream>

#include <libxml++/libxml++.h>

#include "builder.hpp"
#pragma once

namespace xml2epub
{

class HtmlBuilder;

class HtmlState : public OutputState
{
protected:
	friend class HtmlBuilder;
	HtmlBuilder & m_root;
	HtmlState & m_parent;
	std::vector<HtmlState*> m_children;
	xmlpp::Element & m_xml_node;
protected:
	HtmlState( HtmlBuilder & root, HtmlState & parent, xmlpp::Element & xml_node );
	HtmlState( HtmlBuilder & root, xmlpp::Element & xml_node );
	virtual ~HtmlState();
public:
	void put_text( const std::string & str );
	OutputState * bold();
	OutputState * math();
	OutputState * section( const std::string & section_name, unsigned int level );
	OutputState * plot();
	void finish();
};

class HtmlBuilder : public OutputBuilder
{
private:
	friend class HtmlState;
	xmlpp::Document out_doc;
	xmlpp::Element * out_root;
	std::ostream & m_out;
	HtmlState * m_root;
public:
	HtmlBuilder( std::ostream & output_stream );
	virtual ~HtmlBuilder();
	OutputState * create_root();
};

}
