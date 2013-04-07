#include <common/global.hpp>

#include "LatexState.hpp"
#include "LatexEncapsState.hpp"
#include "LatexMathState.hpp"
#include "LatexPlotState.hpp"

#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace xml2epub {

LatexState::LatexState( LatexBuilder& root, LatexState& parent, std::ostream& outs )
	: m_root( root )
	, m_parent( parent )
	, m_out( outs )
{
}

LatexState::~LatexState()
{
	if( &m_parent == this )
	{
		/* i am root */
		m_root.m_root = NULL;
	}
	else
	{
		std::vector<LatexState*>::iterator it;
		if ( ( it = find( m_parent.m_children.begin(), m_parent.m_children.end(), this ) )
			 != m_parent.m_children.end() )
		{
			m_parent.m_children.erase( it );
		}
	}
	/* delete all my children */
	if ( m_children.begin() != m_children.end() )
	{
		COMMON_CERR( "Warning: there are still children that have not been deleted" );
		std::vector<LatexState*> copy( m_children );
		m_children.clear();
		for ( std::vector<LatexState*>::iterator it = copy.begin(); it != copy.end(); ++it )
		{
			if ( *it != NULL )
			{
				delete *it;
			}
		}
	}
}

void LatexState::put_text( const std::string& str )
{
	m_out << str;
}

OutputState* LatexState::section( const std::string& section_name, unsigned int level )
{
	if ( level > 2 )
	{
		throw std::runtime_error( "latex backend only supports subsubsection (level=2)" );
	}
	m_out << "\\";
	for ( unsigned int i=0; i<level; ++i )
	{
		m_out << "sub";
	}
	m_out << "section{" << section_name << "}" << std::endl;
	LatexState* retval = new LatexState( m_root, *this, m_out );
	m_children.push_back( retval );
	return retval;
}

OutputState* LatexState::bold()
{
	LatexState* retval = new LatexEncapsState( "bf", m_root, *this, m_out );
	m_children.push_back( retval );
	return retval;
}

OutputState* LatexState::math()
{
	LatexState* retval = new LatexMathState( m_root, *this, m_out );
	m_children.push_back( retval );
	return retval;
}

OutputState* LatexState::plot()
{
	LatexState* retval = new LatexPlotState( m_root, *this, m_out );
	m_children.push_back( retval );
	return retval;
}

void LatexState::finish()
{
}

}


