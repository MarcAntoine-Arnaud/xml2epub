#include "LatexState.hpp"

#include <string>
#include <iostream>

#pragma once

namespace xml2epub
{

class LatexEncapsState : public LatexState
{
public:
	LatexEncapsState( const std::string& encaps, LatexBuilder& root, LatexState& parent, std::ostream& outs );
	virtual ~LatexEncapsState();
public:
	void finish();
};

LatexEncapsState::LatexEncapsState( const std::string& encaps, LatexBuilder& root,
							LatexState& parent, std::ostream& outs )
	: LatexState( root, parent, outs )
{
	m_out << "{\\" << encaps << " ";
}

LatexEncapsState::~LatexEncapsState()
{
}

void LatexEncapsState::finish()
{
	m_out << "}";
}

}
