#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <libxml++/libxml++.h>
#include <tidy.h>
#include <buffio.h>
#include <unistd.h>

#include "html.hh"
#include "latex.hh"
#include "plot.hh"

using namespace xmlpp;
using namespace std;

namespace xml2epub {
  
  class html_math_state : public html_state {
  private:
    stringstream m_ss;
    latex_builder m_latex;
    output_state * m_root_state;
    output_state * m_math_state;
  public:
    html_math_state( html_builder & root, html_state & parent, xmlpp::Element & xml_node ) 
      : html_state( root, parent, xml_node ), m_latex( m_ss, true ), m_root_state( NULL ), m_math_state( NULL ) {
      m_root_state = m_latex.create_root();
      m_math_state = m_root_state->math();
    }

    virtual ~html_math_state() {
      if ( m_math_state != NULL ) {
	delete m_math_state;
	m_math_state = NULL;
      }
      if ( m_root_state != NULL ) {
	delete m_root_state;
	m_root_state = NULL;
      }
    }

    output_state * bold() {
      throw runtime_error( "can't use bold xml tag in latex math" );
    }

    output_state * math() {
      throw runtime_error( "can't use math xml tag in latex math" );
    }

    output_state * section( const std::string & section_name ) {
      throw runtime_error( "can't use section xml tag in latex math" );
    }

    output_state * plot() {
      throw runtime_error( "can't use plot xml tag in latex math" );
    }

    void put_text( const string & str ) {
      m_math_state->put_text( str );
    }

    void finish() {
      m_math_state->finish();
      m_root_state->finish();
      string file_name;
      {
	stringstream ss;

	ss << getpid() << "_" << random();
	file_name = ss.str();
      }
      string tex_file("/tmp/");
      tex_file += file_name;
      tex_file += ".tex";
      ofstream file;
      file.open( tex_file.c_str(), ios_base::trunc | ios_base::out );
      if ( !file ) {
	throw runtime_error( "Unable to open tmp file" );
      }
      file << m_ss.str();
      file.close();

      string command;
      {
	stringstream ss;
	ss << "(( cd /tmp; xelatex " << file_name << ".tex; pdf2ps " << file_name << ".pdf; ps2eps " << file_name << ".ps; rm -f " << file_name << ".pdf; gs -dEPSCrop -sDEVICE=pngalpha -sOutputFile=" << file_name << ".png " << file_name << ".eps; cd -; mkdir -p images; cp /tmp/" << file_name << ".png images/; rm -f /tmp/" << file_name << ".*; ) 2>&1 ) > /dev/null";
	command = ss.str();
      }
      system( command.c_str() );
#if 0      
      string svgname;
      {
	stringstream ss;
	ss << "/tmp/" << file_name << ".svg";
	svgname = ss.str();
      }
      {
	ifstream svg( svgname.c_str() );
	if ( !svg ) {
	  throw runtime_error( "Unable to open svg file" );
	}
	DomParser parser;
	parser.set_substitute_entities( true );
	parser.parse_stream( svg );
	if ( !parser ) {
	  throw runtime_error( "Unable to parse svg" );
	}
	const Element * rootNode = parser.get_document()->get_root_node();
	m_xml_node.import_node( rootNode );
      }
#else
      Element * new_node = m_xml_node.add_child( "img" );
      string img_url;
      {
	stringstream ss;
	ss << "images/" << file_name << ".png";
	img_url = ss.str();
      }
      new_node->set_attribute( string("src"), img_url );
#endif
    }

  };

  class html_plot_state : public html_state {
  private:
    stringstream m_data;
  public:
    html_plot_state( html_builder & root, html_state & parent, xmlpp::Element & xml_node ) 
      : html_state( root, parent, xml_node ) {
    }
    
    virtual ~html_plot_state() {
    }

    output_state * bold() {
      throw runtime_error( "can't use bold xml tag in plot" );
    }

    output_state * math() {
      throw runtime_error( "can't use math xml tag in plot" );
    }

    output_state * section( const std::string & section_name ) {
      throw runtime_error( "can't use section xml tag in plot" );
    }

    output_state * plot() {
      throw runtime_error( "can't use plot xml tag in plot" );
    }    

    void put_text( const string & str ) {
      m_data << str;
    }

    void finish() {
      string image_file_path;
      {
	stringstream ss;
	ss << "images/" << getpid() << "_" << random() << ".png";
	image_file_path = ss.str();
      }
      system( "mkdir -p images" );
      {
	ofstream png_file( image_file_path.c_str() );
	if ( !png_file ) {
	  throw runtime_error( "Unable to create image file" );
	}
	parse_plot( m_data.str(), png_file );
      }
      Element * paragraph = m_xml_node.add_child( "p" );
      Element * new_node = paragraph->add_child( "img" );
      new_node->set_attribute( string("src"), image_file_path );
    }
  };

  html_state::html_state( html_builder & root, html_state & parent, Element & xml_node ) 
    : m_root( root ), m_parent( parent ), m_xml_node( xml_node ) {
  }

  html_state::html_state( html_builder & root, Element & xml_node ) :
    m_root( root ), m_parent( * this ), m_xml_node( xml_node ) {}

  html_state::~html_state() {
    if ( &m_parent == this ) {
      /* i am root */
      m_root.m_root = NULL;
    } else {
      vector<html_state*>::iterator it;
      if ( ( it = find( m_parent.m_children.begin(), m_parent.m_children.end(), this ) )
	   != m_parent.m_children.end() ) {
	m_parent.m_children.erase( it );
      }
    }
    /* delete all my children */
    if ( m_children.begin() != m_children.end() ) {
      cerr << "Warning: there are still children that have not been deleted" << endl;
      vector<html_state*> copy( m_children );
      m_children.clear();
      for ( vector<html_state*>::iterator it = copy.begin(); it != copy.end(); ++it ) {
	if ( *it != NULL ) {
	  delete *it;
	}
      }
    }
  }

  void html_state::put_text( const string & str ) {
    m_xml_node.add_child_text( str );
  }

  output_state * html_state::section( const std::string & section_name ) {
    Element * header_node = m_xml_node.add_child( "h1" );
    if ( header_node == NULL ) {
      throw runtime_error( "add_child() failed" );
    }
    header_node->add_child_text( section_name );
    Element * new_node = m_xml_node.add_child( "div" );
    if ( new_node == NULL ) {
      throw runtime_error( "add_child() failed" );
    }
    {
      stringstream ss;
      ss << "sec:" << section_name;
      new_node->set_attribute( string("id"), ss.str() );
    }
    html_state * retval = new html_state( m_root, * this, * new_node );
    m_children.push_back( retval );
    return retval;
  }

  output_state * html_state::bold() {
    Element * new_node = m_xml_node.add_child( "b" );
    if ( new_node == NULL ) {
      throw runtime_error( "add_child() failed" );
    }
    html_state * retval = new html_state( m_root, * this, * new_node );
    m_children.push_back( retval );
    return retval;
  }

  output_state * html_state::math() { 
    html_state * retval = new html_math_state( m_root, * this, m_xml_node );
    m_children.push_back( retval );
    return retval;
  }

  output_state * html_state::plot() { 
    html_state * retval = new html_plot_state( m_root, * this, m_xml_node );
    m_children.push_back( retval );
    return retval;
  }

  void html_state::finish() {
    if ( &m_parent == this ) {
      string data;
      data = m_root.out_doc.write_to_string(  );
#if 1
      TidyDoc tdoc = tidyCreate();
      if ( tidyOptSetBool( tdoc, TidyXhtmlOut, yes ) == false ) {
	throw runtime_error( "tidyOptSetBool failed" );
      }
      int rc=-1;
      TidyBuffer errbuf;
      tidyBufInit( &errbuf );
      
      rc = tidySetErrorBuffer( tdoc, &errbuf );
      if ( rc < 0 ) {
	throw runtime_error( "tidySetErrorBuffer failed" );
      }
      rc = tidyParseString( tdoc, data.c_str() );
      if ( rc < 0 ) {
	throw runtime_error( "tidyParseString failed" );
      }
      rc = tidyCleanAndRepair( tdoc );
      if ( rc < 0 ) {
	throw runtime_error( "tidyCleanAndRepair failed" );
      }
      TidyBuffer output_buffer;
      tidyBufInit( &output_buffer );
      rc = tidySaveBuffer( tdoc, &output_buffer );
      if ( rc < 0 ) {
	throw runtime_error( "tidySaveBuffer failed" );
      }
      string out_buffer( (const char *)output_buffer.bp );
      tidyBufFree( &output_buffer );
      tidyBufFree( &errbuf );
      tidyRelease( tdoc );
      m_root.m_out << out_buffer;
#else
      m_root.m_out << data;
#endif
    }
  }
    
  html_builder::html_builder( ostream & output_stream ) 
    : out_root( out_doc.create_root_node( "html" ) ), m_out( output_stream ),
      m_root( NULL ) {
    if ( out_root == NULL ) {
      throw runtime_error( "create_root_node failed" );
    }
  }
    
  html_builder::~html_builder() {
    if ( m_root != NULL ) {
      delete m_root;
    }
  }

  output_state * html_builder::create_root() {
    if ( m_root != NULL ) {
      delete m_root;
    }
    m_root = new html_state( *this, * out_root );
    return m_root;
  }

}
