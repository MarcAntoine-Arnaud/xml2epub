#include "OutputBuilder.hpp"

#include <iostream>

#pragma once

namespace xml2epub {

class LatexState;

class LatexBuilder : public OutputBuilder
{
public:
	LatexBuilder( std::ostream & output_stream, bool minimal = false );
	virtual ~LatexBuilder();
	OutputState * create_root();

private:
	friend class LatexState;
	std::ostream& m_out;
	LatexState*   m_root;
	bool          m_minimal;
};

}
