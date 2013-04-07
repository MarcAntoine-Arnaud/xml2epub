#include "OutputState.hpp"
#include "LatexState.hpp"

#include <stdexcept>
#include <iostream>

#pragma once

namespace xml2epub {

class LatexMathState : public LatexState
{
public:
	LatexMathState( LatexBuilder& root, LatexState& parent, std::ostream & outs )
		: LatexState( root, parent, outs )
	{
		m_out << "$";
	}
	virtual ~LatexMathState()
	{
	}

	OutputState* bold()
	{
		throw std::runtime_error( "can't use bold xml tag in latex math" );
	}
	OutputState* math()
	{
		throw std::runtime_error( "can't use math xml tag in latex math" );
	}

	OutputState* section( const std::string & section_name, unsigned int level )
	{
		throw std::runtime_error( "can't use section xml tag in latex math" );
	}

	OutputState* plot()
	{
		throw std::runtime_error( "can't use plot xml tag in latex math" );
	}

	void finish()
	{
		m_out << "$";
	}
};

}
