#include "LatexBuilder.hpp"
#include "LatexState.hpp"
#include "LatexRootState.hpp"

namespace xml2epub {

LatexBuilder::LatexBuilder( std::ostream& output_stream, bool minimal )
	: m_out( output_stream )
	, m_root( NULL )
	, m_minimal( minimal )
{
}

LatexBuilder::~LatexBuilder()
{
	if ( m_root != NULL )
	{
		delete m_root;
	}
}

OutputState * LatexBuilder::create_root()
{
	if ( m_root != NULL )
	{
		delete m_root;
	}
	m_root = new LatexRootState( *this, m_out, m_minimal );
	return m_root;
}

}
