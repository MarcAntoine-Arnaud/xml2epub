#include <sstream>
#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include "plot.hh"

using namespace std;

namespace xml2epub {
  void parse_plot( const string & data, ostream & out ) {
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
      ss << "( ( cd /tmp; gnuplot " << file_name << ".plt; xelatex " << file_name << ".tex; gs -sDEVICE=pngalpha -sOutputFile=" << file_name << ".png " << file_name << ".pdf; cd -; ) 2>&1 ) > /dev/null";
      shell_command = ss.str();
    }
    system( shell_command.c_str() );
    {
      string png_file;
      {
	stringstream ss;
	ss << "/tmp/" << file_name << ".png";
	png_file = ss.str();
      }
      ifstream png( png_file.c_str() );
      out << png.rdbuf();
    }
    {
      stringstream ss;
      ss << "rm -f /tmp/" << file_name << ".*";
      shell_command = ss.str();
    }
    system( shell_command.c_str() );
  }
}
