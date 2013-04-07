#include "OutputState.hpp"
#include "LatexState.hpp"
#include "Plot.hpp"

#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <cstdlib>

#pragma once

namespace xml2epub {

class LatexPlotState : public LatexState
{
public:
	LatexPlotState( LatexBuilder& root, LatexState& parent, std::ostream& outs )
		: LatexState( root, parent, outs )
	{
	}

	virtual ~LatexPlotState()
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
			ss << "images/" << getpid() << "_" << random() << ".pdf";
			image_file_path = ss.str();
		}
		system( "mkdir -p images" );
		{
			std::ofstream pdf_file( image_file_path.c_str() );
			if ( !pdf_file )
			{
				throw std::runtime_error( "Unable to create image file" );
			}
			parse_plot( m_data.str(), pdf_file, false );
		}
		m_out << "\\begin{figure}" << std::endl;
		m_out << "\\centering" << std::endl;
		m_out << "\\includegraphics[width=0.7\\textwidth]{" << image_file_path << "}" << std::endl;
		m_out << "\\end{figure}" << std::endl;
	}

private:
	std::stringstream m_data;
};

}
