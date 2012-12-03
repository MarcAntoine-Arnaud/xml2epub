#include <sstream>
#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include "plot.hh"
#include "latex2util.hh"

using namespace std;

namespace xml2epub {
  void parse_plot( const string & data, ostream & out, bool out_svg ) {
    string file_name;
    {
      stringstream ss;
      ss << getpid() << "_" << random();
      file_name = ss.str();
    }
    string plt_file;
    {
      stringstream ss;
      ss << "/tmp/" << file_name << ".plt";
      plt_file = ss.str();
    }
    {
      ofstream gnu_plot_file( plt_file.c_str() );
      if ( !gnu_plot_file ) {
	throw runtime_error( "Cannot creat tmp file" );
      }
      gnu_plot_file << "set terminal epslatex standalone color" << endl;
      gnu_plot_file << "set output \"/tmp/" << file_name << ".tex\"" << endl << endl;
      gnu_plot_file << data << endl;
      gnu_plot_file << "quit" << endl;
    }

    string shell_command;
    {
      stringstream ss;
      ss << "( ( cd /tmp; gnuplot " << file_name << ".plt; xelatex " << file_name << ".tex; cd -; ) 2>&1 ) > /dev/null";
      shell_command = ss.str();
    }
    system( shell_command.c_str() );
    {
      string pdf_file;
      {
	stringstream ss;
	ss << "/tmp/" << file_name << ".pdf";
	pdf_file = ss.str();
      }
      if ( out_svg == true ) {
	pdf2svg( pdf_file, out );
      } else {
	ifstream pdf( pdf_file.c_str() );
	out << pdf.rdbuf();
      }
    }
    {
      stringstream ss;
      ss << "rm -f /tmp/" << file_name << ".*";
      shell_command = ss.str();
    }
    system( shell_command.c_str() );
  }
}
