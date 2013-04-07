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
	std::string file_name;
	std::string plt_file;
	std::string shell_command;

	{
		std::stringstream ss;
		ss << getpid() << "_" << random();
		file_name = ss.str();
	}
	{
		std::stringstream ss;
		ss << "/tmp/" << file_name << ".plt";
		plt_file = ss.str();
	}
	{
		std::ofstream gnu_plot_file( plt_file.c_str() );
		if ( !gnu_plot_file ) {
			throw std::runtime_error( "Cannot creat tmp file" );
		}
		gnu_plot_file << "set terminal epslatex standalone color" << std::endl;
		gnu_plot_file << "set output \"/tmp/" << file_name << ".tex\"" << std::endl << std::endl;
		gnu_plot_file << data << std::endl;
		gnu_plot_file << "quit" << std::endl;
	}

	{
		std::stringstream ss;
		ss << "( ( cd /tmp; gnuplot " << file_name << ".plt; xelatex " << file_name << ".tex; cd -; ) 2>&1 ) > /dev/null";
		shell_command = ss.str();
	}
	system( shell_command.c_str() );
	{
		std::string pdf_file;
		{
			std::stringstream ss;
			ss << "/tmp/" << file_name << ".pdf";
			pdf_file = ss.str();
		}
		if ( out_svg == true )
		{
			pdf2svg( pdf_file, out );
		}
		else
		{
			std::ifstream pdf( pdf_file.c_str() );
			out << pdf.rdbuf();
		}
	}
	{
		std::stringstream ss;
		ss << "rm -f /tmp/" << file_name << ".*";
		shell_command = ss.str();
	}
	system( shell_command.c_str() );
}

}
