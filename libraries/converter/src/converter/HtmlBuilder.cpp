#include "HtmlBuilder.hpp"
#include "HtmlState.hpp"

#include <stdexcept>

namespace xml2epub {

HtmlBuilder::HtmlBuilder( std::ostream& output_stream )
	: out_root( out_doc.create_root_node( "html" ) )
	, m_out( output_stream )
	, m_root( NULL )
	, m_htmlState( *out_root )
{
	if( out_root == NULL )
	{
		throw std::runtime_error( "create_root_node failed" );
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
	m_root = new HtmlState( *out_root );
	return m_root;
}

}
