#include "options.hpp"
#include "common/global.hpp"

#include <tidy/tidy.h>
#include <tidy/buffio.h>
#include <glib-object.h>

#include <converter/builder.hpp>
#include <converter/html.hpp>
#include <converter/latex.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>


using namespace std;
namespace bpo = boost::program_options;

int main( int argc, char * argv[] )
{
	string input_file_path  = "";
	string output_file_path = "";

	//bool keep_text           = false;
	bool input_file_is_cin   = true;
	bool output_file_is_cout = true;
	bool output_html         = true;

	g_type_init();

	bpo::options_description desc("");
	desc.add_options()
			( "help,h", "produce help message" )
			//( "keep-text,t", bpo::value<bool>(), "When converting equations to svg images, keep text or convert to path" )
			( "input-file,i", bpo::value< vector<string> >(), "input xml file path (default is standard input)" )
			( "output-file,o", bpo::value< vector<string> >(), "output html file (default is standard ouput)" )
			( "latex,l", bpo::value<bool>(), "output latex file" );

	bpo::variables_map vm;
	try
	{
		bpo::store( bpo::parse_command_line( argc, argv, desc ), vm );
		bpo::notify( vm );
	}
	catch( const bpo::error& e)
	{
		COMMON_COUT( "command line error: " << e.what() );
		exit( -2 );
	}
	catch(...)
	{
		COMMON_COUT( "unknown error in command line." );
		exit( -2 );
	}

	if ( vm.count("help") )
	{
		COMMON_COUT( desc );
		return -1;
	}
	/*if ( vm.count("keep-text") )
	{
		keep_text = vm["keep-text"].as<bool>();
	}*/
	if ( vm.count("latex") )
	{
		output_html = ( vm["latex"].as<bool>() == false );
	}
	if ( vm.count("input-file") > 1 )
	{
		throw runtime_error( "You may only specify one input file (or none for standard input)" );
	}
	if ( vm.count("input-file") == 1 )
	{
		input_file_path = vm["input-file"].as< vector<string> >()[0];
		input_file_is_cin = false;
	}
	if ( vm.count("output-file") > 1 )
	{
		throw runtime_error( "You may only specify one output file (or none for standard output)" );
	}
	if ( vm.count("output-file") == 1 )
	{
		output_file_path = vm["output-file"].as< vector<string> >()[0];
		output_file_is_cout = false;
	}

	istream* in_stream = &cin;
	ostream* out_stream = &cout;

	if ( input_file_is_cin == false )
	{
		in_stream = new ifstream( input_file_path.c_str() );
		if ( !(*in_stream) )
		{
			COMMON_CERR( "Unable to open file \"" << input_file_path << "\" for input!" );
			return -1;
		}
	}
	if ( output_file_is_cout == false )
	{
		out_stream = new ofstream( output_file_path.c_str() );
		if ( !(*out_stream) )
		{
			COMMON_CERR( "Unable to open file \"" << output_file_path << "\" for output!" );
			return -1;
		}
	}

	xml2epub::parse_file( output_html, *in_stream, *out_stream );

	if ( input_file_is_cin == false )
	{
		delete in_stream;
	}
	if ( output_file_is_cout == false )
	{
		delete out_stream;
	}

	return 0;
}
