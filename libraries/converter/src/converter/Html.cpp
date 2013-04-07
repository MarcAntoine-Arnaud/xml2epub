#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <unistd.h>

#include <libxml++/libxml++.h>
#include <tidy/tidy.h>
#include <tidy/buffio.h>

#include "html.hpp"
#include "latex.hpp"
#include "plot.hpp"
#include "latex2util.hpp"

using namespace xmlpp;
using namespace std;

namespace xml2epub {

class html_math_state
	: public HtmlState
{

private:
	stringstream m_ss;

public:
	html_math_state( HtmlBuilder & root, HtmlState & parent, xmlpp::Element & xml_node )
		: HtmlState( root, parent, xml_node )
	{
		m_ss << "$";
	}

	virtual ~html_math_state()
	{
	}

	OutputState * bold()
	{
		throw runtime_error( "can't use bold xml tag in latex math" );
	}

	OutputState * math()
	{
		throw runtime_error( "can't use math xml tag in latex math" );
	}

	OutputState * section( const std::string & section_name, unsigned int level )
	{
		throw runtime_error( "can't use section xml tag in latex math" );
	}

	OutputState * plot()
	{
		throw runtime_error( "can't use plot xml tag in latex math" );
	}

	void put_text( const string & str )
	{
		m_ss << str;
	}

	void finish()
	{
		m_ss << "$";
		system( "mkdir -p images" );
		string file_name;
		{
			stringstream ss;
			ss << getpid() << "_" << random();
			file_name = ss.str();
		}
		Element * new_node = m_xml_node.add_child( "img" );
		string img_url;
		{
			stringstream ss;
			ss << "images/" << file_name << ".svg";
			img_url = ss.str();
		}
		{
			ofstream svg_file( img_url.c_str() );
			{
				string data( m_ss.str() );
				stringstream iss( data );
				latex2svg( iss, svg_file );
			}
		}
		new_node->set_attribute( string("src"), img_url );
	}

};

class html_plot_state
	: public HtmlState
{
private:
	stringstream m_data;
public:
	html_plot_state( HtmlBuilder & root, HtmlState & parent, xmlpp::Element & xml_node )
		: HtmlState( root, parent, xml_node ) {
	}

	virtual ~html_plot_state() {
	}

	OutputState * bold()
	{
		throw runtime_error( "can't use bold xml tag in plot" );
	}

	OutputState * math()
	{
		throw runtime_error( "can't use math xml tag in plot" );
	}

	OutputState * section( const std::string & section_name, unsigned int level )
	{
		throw runtime_error( "can't use section xml tag in plot" );
	}

	OutputState * plot()
	{
		throw runtime_error( "can't use plot xml tag in plot" );
	}

	void put_text( const string & str )
	{
		m_data << str;
	}

	void finish()
	{
		string image_file_path;
		{
			stringstream ss;
			ss << "images/" << getpid() << "_" << random() << ".svg";
			image_file_path = ss.str();
		}
		system( "mkdir -p images" );
		{
			ofstream svg_file( image_file_path.c_str() );
			if ( !svg_file )
			{
				throw runtime_error( "Unable to create image file" );
			}
			parse_plot( m_data.str(), svg_file, true );
		}
		Element * paragraph = m_xml_node.add_child( "p" );
		Element * new_node = paragraph->add_child( "img" );
		new_node->set_attribute( string("src"), image_file_path );
	}
};

HtmlState::HtmlState( HtmlBuilder & root, HtmlState & parent, Element & xml_node )
	: m_root( root )
	, m_parent( parent )
	, m_xml_node( xml_node )
{
}

HtmlState::HtmlState( HtmlBuilder & root, Element & xml_node )
	: m_root( root )
	, m_parent( * this )
	, m_xml_node( xml_node )
{
}

HtmlState::~HtmlState()
{
	if ( &m_parent == this )
	{
		/* i am root */
		m_root.m_root = NULL;
	}
	else
	{
		vector<HtmlState*>::iterator it;
		if ( ( it = find( m_parent.m_children.begin(), m_parent.m_children.end(), this ) )
			 != m_parent.m_children.end() )
		{
			m_parent.m_children.erase( it );
		}
	}
	/* delete all my children */
	if ( m_children.begin() != m_children.end() )
	{
		cerr << "Warning: there are still children that have not been deleted" << endl;
		vector<HtmlState*> copy( m_children );
		m_children.clear();
		for ( vector<HtmlState*>::iterator it = copy.begin(); it != copy.end(); ++it )
		{
			if ( *it != NULL )
			{
				delete *it;
			}
		}
	}
}

void HtmlState::put_text( const string & str )
{
	m_xml_node.add_child_text( str );
}

OutputState * HtmlState::section( const std::string & section_name, unsigned int level )
{
	string html_section_element_name;
	{
		stringstream ss;
		ss << "h" << ( level + 1 );
		html_section_element_name = ss.str();
	}
	Element * header_node = m_xml_node.add_child( html_section_element_name );
	if ( header_node == NULL )
	{
		throw runtime_error( "add_child() failed" );
	}
	header_node->add_child_text( section_name );
	Element * new_node = m_xml_node.add_child( "div" );
	if ( new_node == NULL )
	{
		throw runtime_error( "add_child() failed" );
	}
	{
		stringstream ss;
		ss << "sec:" << section_name;
		new_node->set_attribute( string("id"), ss.str() );
	}
	HtmlState * retval = new HtmlState( m_root, * this, * new_node );
	m_children.push_back( retval );
	return retval;
}

OutputState * HtmlState::bold()
{
	Element * new_node = m_xml_node.add_child( "b" );
	if ( new_node == NULL )
	{
		throw runtime_error( "add_child() failed" );
	}
	HtmlState * retval = new HtmlState( m_root, * this, * new_node );
	m_children.push_back( retval );
	return retval;
}

OutputState * HtmlState::math()
{
	HtmlState * retval = new html_math_state( m_root, * this, m_xml_node );
	m_children.push_back( retval );
	return retval;
}

OutputState * HtmlState::plot()
{
	HtmlState * retval = new html_plot_state( m_root, * this, m_xml_node );
	m_children.push_back( retval );
	return retval;
}

void HtmlState::finish()
{
	if ( &m_parent == this )
	{
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
		if ( rc < 0 )
		{
			throw runtime_error( "tidySetErrorBuffer failed" );
		}
		rc = tidyParseString( tdoc, data.c_str() );
		if ( rc < 0 )
		{
			throw runtime_error( "tidyParseString failed" );
		}
		rc = tidyCleanAndRepair( tdoc );
		if ( rc < 0 )
		{
			throw runtime_error( "tidyCleanAndRepair failed" );
		}
		TidyBuffer output_buffer;
		tidyBufInit( &output_buffer );
		rc = tidySaveBuffer( tdoc, &output_buffer );
		if ( rc < 0 )
		{
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

HtmlBuilder::HtmlBuilder( ostream & output_stream )
	: out_root( out_doc.create_root_node( "html" ) )
	, m_out( output_stream )
	, m_root( NULL )
{
	if ( out_root == NULL )
	{
		throw runtime_error( "create_root_node failed" );
	}
}

HtmlBuilder::~HtmlBuilder()
{
	if ( m_root != NULL )
	{
		delete m_root;
	}
}

OutputState * HtmlBuilder::create_root()
{
	if ( m_root != NULL )
	{
		delete m_root;
	}
	m_root = new HtmlState( *this, * out_root );
	return m_root;
}

}
