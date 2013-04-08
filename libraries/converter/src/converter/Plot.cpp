#include <common/global.hpp>

#include "Plot.hpp"
#include "Latex2util.hpp"

#include <sstream>
#include <stdexcept>
#include <fstream>
#include <cstdlib>

namespace xml2epub
{

void parse_plot( const std::string& data, std::ostream& out, bool out_svg )
{
	std::string filename;
	std::string plotFile;
	std::string shellCommand;
	std::string pdfFile;

	COMMON_COUT( "start" );

	std::ostringstream oss;
	oss << getpid() << "_" << random();
	filename = oss.str();

	COMMON_COUT_VAR( filename );

	plotFile = "/tmp/" + filename + ".plt";

	pdfFile = "/tmp/" + filename + ".pdf";

	COMMON_COUT_VAR3( filename, plotFile, pdfFile );

	{
		std::ofstream gnu_plot_file( plotFile.c_str() );
		if ( !gnu_plot_file ) {
			throw std::runtime_error( "Cannot creat tmp file" );
		}
		gnu_plot_file << "set terminal epslatex standalone color" << std::endl;
		gnu_plot_file << "set output \"/tmp/" << filename << ".tex\"" << std::endl << std::endl;
		gnu_plot_file << data << std::endl;
		gnu_plot_file << "quit" << std::endl;
	}

	shellCommand = "( ( cd /tmp; gnuplot " + filename + ".plt; xelatex " + filename + ".tex; cd -; ) 2>&1 ) > /dev/null";

	COMMON_COUT( "run : " << shellCommand );

	system( shellCommand.c_str() );

	if ( out_svg == true )
	{
		pdf2svg( pdfFile, out );
	}
	else
	{
		std::ifstream pdf( pdfFile.c_str() );
		out << pdf.rdbuf();
	}

	shellCommand = "rm -f /tmp/" + filename + ".*";
	COMMON_COUT( "run : " << shellCommand );
	system( shellCommand.c_str() );
}

}
