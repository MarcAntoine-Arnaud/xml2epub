#include "HtmlState.hpp"
#include "Latex2util.hpp"

#include <libxml++/libxml++.h>

#include <string>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <fstream>

#pragma once

namespace xml2epub {

class HtmlMathState
	: public HtmlState
{
public:
	HtmlMathState( HtmlBuilder& root, HtmlState& parent, xmlpp::Element& xml_node )
		: HtmlState( root, parent, xml_node )
	{
		m_ss << "$";
	}

	virtual ~HtmlMathState()
	{
	}

	OutputState* bold()
	{
		throw std::runtime_error( "can't use bold xml tag in latex math" );
	}

	OutputState* math()
	{
		throw std::runtime_error( "can't use math xml tag in latex math" );
	}

	OutputState* section( const std::string& section_name, unsigned int level )
	{
		throw std::runtime_error( "can't use section xml tag in latex math" );
	}

	OutputState* plot()
	{
		throw std::runtime_error( "can't use plot xml tag in latex math" );
	}

	void put_text( const std::string & str )
	{
		m_ss << str;
	}

	void finish()
	{
		m_ss << "$";
		std::system( "mkdir -p images" );
		std::string file_name;
		{
			std::stringstream ss;
			ss << getpid() << "_" << random();
			file_name = ss.str();
		}
		xmlpp::Element * new_node = m_xml_node.add_child( "img" );
		std::string img_url;
		{
			std::stringstream ss;
			ss << "images/" << file_name << ".svg";
			img_url = ss.str();
		}
		{
			std::ofstream svg_file( img_url.c_str() );
			{
				std::string data( m_ss.str() );
				std::stringstream iss( data );
				latex2svg( iss, svg_file );
			}
		}
		new_node->set_attribute( std::string("src"), img_url );
	}

private:
	std::stringstream m_ss;
};

}
