#include "LatexBuilder.hpp"

#include <vector>

#pragma once

namespace xml2epub {

class LatexBuilder;

class LatexState : public OutputState
{
protected:
	LatexState( LatexBuilder & root, LatexState & parent, std::ostream & outs );
	virtual ~LatexState();

public:
	void put_text( const std::string & str );
	OutputState* bold();
	OutputState* math();
	OutputState* section( const std::string & section_name, unsigned int level );
	OutputState* plot();
	void finish();

protected:
	friend class LatexBuilder;
	LatexBuilder& m_root;
	LatexState& m_parent;
	std::vector<LatexState*> m_children;
	std::ostream & m_out;
};

}
