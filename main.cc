#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <boost/program_options.hpp>
#include <libxml++/libxml++.h>
#include <tidy.h>
#include <buffio.h>
#include <glib-object.h>

#include "builder.hh"
#include "html.hh"
#include "latex.hh"

namespace po = boost::program_options;
using namespace std;
using namespace xmlpp;

namespace xml2epub {
  void parse_cmdline_args( int argc, char * argv[], bool keep_text, 
			   string & input_file,
			   bool & input_file_is_cin,
			   string & output_file,
			   bool & output_file_is_cout,
			   bool & output_html ) {
    /* defaults */
    keep_text = false;
    input_file = "";
    input_file_is_cin = true;
    output_file = "";
    output_file_is_cout = true;
    output_html = true;
    
    po::options_description desc("Allowed options");
    desc.add_options()
      ( "help,h", "produce help message" )
      ( "keep-text,t", po::value<bool>(), "When converting equations to svg images, keep text or convert to path" )
      ( "input-file,i", po::value< vector<string> >(), "input xml file path (default is standard input)" )
      ( "output-file,o", po::value< vector<string> >(), "output html file" )
      ( "latex,l", po::value<bool>(), "output latex file" );
    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, desc ), vm );

    if ( vm.count("help") ) {
      cout << desc << endl;
    }
    if ( vm.count("keep-text") ) {
      keep_text = vm["keep-text"].as<bool>();
    }
    if ( vm.count("latex") ) {
      output_html = ( vm["latex"].as<bool>() == false );
    }
    if ( vm.count("input-file") > 1 ) {
      throw runtime_error( "You may only specify one input file (or none for standard input)" );
    }
    if ( vm.count("input-file") == 1 ) {
      input_file = vm["input-file"].as< vector<string> >()[0];
      input_file_is_cin = false;
    }

    if ( vm.count("output-file") > 1 ) {
      throw runtime_error( "You may only specify one output file (or none for standard output)" );
    }
    if ( vm.count("output-file") == 1 ) {
      output_file = vm["output-file"].as< vector<string> >()[0];
      output_file_is_cout = false;
    }
  }

  void parse_node( const Node & in_node, output_state & state ) {
    if ( dynamic_cast<const ContentNode*>( &in_node ) != NULL ) {
      const ContentNode & content = dynamic_cast<const ContentNode &>( in_node );
      state.put_text( content.get_content() );
    } else {
      if ( dynamic_cast<const TextNode*>( &in_node ) != NULL ) {
	const TextNode & text = dynamic_cast<const TextNode &>( in_node );
	if ( text.is_white_space() ) {
	  return;
	}
      } else if ( dynamic_cast<const Element*>( &in_node ) != NULL ) {
	const Element & element = dynamic_cast<const Element&>( in_node );
	output_state * out = NULL;
	string name = element.get_name();
	if ( name == "b" ) {
	  out = state.bold();
	} else if ( name == "math" ) {
	  out = state.math();
	} else if ( name == "plot" ) {
	  out = state.plot();
	} else if ( ( name == "section" ) || ( name == "subsection" ) || ( name == "subsubsection" ) ) {
	  string section_name = element.get_attribute_value( "name" );
	  if ( section_name.length() == 0 ) {
	    throw runtime_error( "Sections must have a name attribute" );
	  }
	  unsigned int level;
	  if ( name == "subsection" ) {
	    level = 1;
	  } else if ( name == "subsubsection" ) {
	    level = 2;
	  } else {
	    level = 0;
	  }
	  out = state.section( section_name, level );
	} else {
	  stringstream ss;
	  ss << "Unknown element with name \"" << name << "\" found!" << endl;
	  throw runtime_error( ss.str().c_str() );
	}
	Node::NodeList list = in_node.get_children();
	for ( Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter ) {
	  if ( *iter != NULL ) {
	    parse_node( **iter, *out );
	  }
	}
	out->finish();
	delete out;
      }
    }
  }

  void parse_file( bool do_html, istream & input_stream, ostream & output_stream ) {
    DomParser parser;
    parser.set_substitute_entities( true );
    parser.parse_stream( input_stream );
    if ( parser ) {
      /* if succesfull create output */
      const Element * rootNode = parser.get_document()->get_root_node();
      if ( rootNode == NULL ) {
	throw runtime_error( "get_root_node() failed" );
      }
      
      output_builder * b;
      if ( do_html ) {
	b = new html_builder( output_stream );
      } else {
	b = new latex_builder( output_stream );
      }

      /* do stuff */
      {	
	const Element & root_in = dynamic_cast<const Element &>( *rootNode );
	if ( root_in.get_name() != "document" ) {
	  throw runtime_error( "root node must be document" );
	}
	output_state * s = b->create_root();
	Node::NodeList list = root_in.get_children();
	for ( Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter ) {
	  if ( *iter != NULL ) {
	    parse_node( **iter, * s );
	  }
	}
	s->finish();
	delete s;
      }
      delete b;
    }
  }
}

int main( int argc, char * argv[] ) {
  bool keep_text;
  string input_file_path;
  bool input_file_is_cin;
  string output_file_path;
  bool output_file_is_cout;
  bool output_html;

  g_type_init();

  xml2epub::parse_cmdline_args( argc, argv, keep_text, input_file_path, input_file_is_cin,
				output_file_path, output_file_is_cout, output_html );
  istream * in_stream = &cin;
  ostream * out_stream = &cout;

  if ( input_file_is_cin == false ) {
    in_stream = new ifstream( input_file_path.c_str() );
    if ( !(*in_stream) ) {
      cerr << "Unable to open file \"" << input_file_path << "\" for input!" << endl;
      return -1;
    }
  }
  if ( output_file_is_cout == false ) {
    out_stream = new ofstream( output_file_path.c_str() );
    if ( !(*out_stream) ) {
      cerr << "Unable to open file \"" << output_file_path << "\" for output!" << endl;
      return -1;
    }
  }

  xml2epub::parse_file( output_html, *in_stream, *out_stream );
  
  if ( input_file_is_cin == false ) {
    delete in_stream;
  }
  if ( output_file_is_cout == false ) {
    delete out_stream;
  }

  return 0;
}
