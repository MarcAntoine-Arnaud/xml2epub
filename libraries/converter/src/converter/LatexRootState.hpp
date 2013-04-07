#include "LatexState.hpp"

#pragma once

namespace xml2epub
{

class LatexRootState
	: public LatexState
{
public:
	LatexRootState( LatexBuilder & root, std::ostream & outs, bool minimal ) :
		LatexState( root, *this, outs )
	{
		if ( minimal == true )
		{
			m_out << "\\documentclass{minimal}" << std::endl;
			m_out << "\\usepackage{fontspec}" << std::endl;
			m_out << "\\usepackage{unicode-math}" << std::endl;
			m_out << "\\setmathfont{STIXGeneral}" << std::endl;
			m_out << "\\begin{document}" << std::endl;
		}
		else
		{
			m_out << "\\documentclass[a4paper,12pt]{article}" << std::endl;
			m_out << "\\usepackage{fontspec}" << std::endl;
			m_out << "\\usepackage{unicode-math}" << std::endl;
			m_out << "\\usepackage{graphicx}" << std::endl;
			m_out << "\\usepackage{fullpage}" << std::endl;
			m_out << "\\setmathfont{STIXGeneral}" << std::endl;
			m_out << "\\begin{document}" << std::endl;
		}
	}
	virtual ~LatexRootState()
	{
	}

	void finish()
	{
		m_out << "\\end{document}" << std::endl;
	}
};

}
