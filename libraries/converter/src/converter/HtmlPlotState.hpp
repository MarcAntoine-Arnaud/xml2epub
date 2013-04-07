#include "HtmlState.hpp"
#include "Plot.hpp"

#include <sstream>
#include <fstream>

#pragma once

namespace xml2epub {

class HtmlPlotState
	: public HtmlState
{
public:
	HtmlPlotState( HtmlBuilder& root, HtmlState& parent, xmlpp::Element& xml_node )
		: HtmlState( root, parent, xml_node )
	{
	}

	virtual ~HtmlPlotState()
	{
	}

	OutputState* bold()
	{
		throw std::runtime_error( "can't use bold xml tag in plot" );
	}

	OutputState* math()
	{
		throw std::runtime_error( "can't use math xml tag in plot" );
	}

	OutputState* section( const std::string& section_name, unsigned int level )
	{
		throw std::runtime_error( "can't use section xml tag in plot" );
	}

	OutputState* plot()
	{
		throw std::runtime_error( "can't use plot xml tag in plot" );
	}

	void put_text( const std::string& str )
	{
		m_data << str;
	}

	void finish()
	{
		std::string image_file_path;
		{
			std::stringstream ss;
			ss << "images/" << getpid() << "_" << random() << ".svg";
			image_file_path = ss.str();
		}
		system( "mkdir -p images" );
		{
			std::ofstream svg_file( image_file_path.c_str() );
			if ( !svg_file )
			{
				throw std::runtime_error( "Unable to create image file" );
			}
			parse_plot( m_data.str(), svg_file, true );
		}
		xmlpp::Element * paragraph = m_xml_node.add_child( "p" );
		xmlpp::Element * new_node = paragraph->add_child( "img" );
		new_node->set_attribute( std::string("src"), image_file_path );
	}

private:
	std::stringstream m_data;
};

}
