#include "HtmlState.hpp"
#include "HtmlMathState.hpp"
#include "HtmlPlotState.hpp"
#include "Plot.hpp"
#include "Latex2util.hpp"


#include <libxml++/libxml++.h>
#include <tidy/tidy.h>
#include <tidy/buffio.h>

#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <unistd.h>

using namespace xmlpp;
using namespace std;

namespace xml2epub {

HtmlState::HtmlState( HtmlBuilder& root, HtmlState& parent, Element& xml_node )
	: m_root( root )
	, m_parent( parent )
	, m_xml_node( xml_node )
{
}

HtmlState::HtmlState( HtmlBuilder& root, Element& xml_node )
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

void HtmlState::put_text( const string& str )
{
	m_xml_node.add_child_text( str );
}

OutputState * HtmlState::section( const std::string& section_name, unsigned int level )
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
	HtmlState * retval = new HtmlMathState( m_root, * this, m_xml_node );
	m_children.push_back( retval );
	return retval;
}

OutputState * HtmlState::plot()
{
	HtmlState * retval = new HtmlPlotState( m_root, * this, m_xml_node );
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

}
