#include <string>
#include <vector>
#include <iostream>

#include <libxml++/libxml++.h>

#include "builder.hpp"

#pragma once

namespace xml2epub {

class LatexBuilder;

class LatexState : public OutputState
{
protected:
	friend class LatexBuilder;
	LatexBuilder & m_root;
	LatexState & m_parent;
	std::vector<LatexState*> m_children;
	std::ostream & m_out;
protected:
	LatexState( LatexBuilder & root, LatexState & parent, std::ostream & outs );
public:
	virtual ~LatexState();

	void put_text( const std::string & str );
	OutputState * bold();
	OutputState * math();
	OutputState * section( const std::string & section_name, unsigned int level );
	OutputState * plot();
	void finish();
};

class LatexBuilder : public OutputBuilder
{
private:
	friend class LatexState;
	std::ostream & m_out;
	LatexState * m_root;
	bool m_minimal;
public:
	LatexBuilder( std::ostream & output_stream, bool minimal = false );
	virtual ~LatexBuilder();
	OutputState * create_root();
};

}
