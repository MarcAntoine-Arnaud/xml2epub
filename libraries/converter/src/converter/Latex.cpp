#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <fstream>

#include <libxml++/libxml++.h>

#include <tidy/tidy.h>
#include <tidy/buffio.h>

#include "latex.hpp"
#include "plot.hpp"

using namespace xmlpp;
using namespace std;

namespace xml2epub
{

class encaps_state : public LatexState
{
public:
	encaps_state( const string & encaps, LatexBuilder & root, LatexState & parent, ostream & outs );
	virtual ~encaps_state();
public:
	void finish();
};

class math_state : public LatexState
{
public:
	math_state( LatexBuilder & root, LatexState & parent, ostream & outs )
		: LatexState( root, parent, outs )
	{
		m_out << "$";
	}
	virtual ~math_state()
	{
	}

	OutputState * bold()
	{
		throw runtime_error( "can't use bold xml tag in latex math" );
	}
	OutputState * math()
	{
		throw runtime_error( "can't use math xml tag in latex math" );
	}

	OutputState * section( const std::string & section_name, unsigned int level )
	{
		throw runtime_error( "can't use section xml tag in latex math" );
	}

	OutputState * plot()
	{
		throw runtime_error( "can't use plot xml tag in latex math" );
	}
public:
	void finish()
	{
		m_out << "$";
	}
};


class latex_plot_state : public LatexState
{
private:
	stringstream m_data;
public:
	latex_plot_state( LatexBuilder & root, LatexState & parent, ostream & outs )
		: LatexState( root, parent, outs )
	{
	}

	virtual ~latex_plot_state()
	{
	}

	OutputState * bold()
	{
		throw runtime_error( "can't use bold xml tag in plot" );
	}

	OutputState * math()
	{
		throw runtime_error( "can't use math xml tag in plot" );
	}

	OutputState * section( const std::string & section_name, unsigned int level )
	{
		throw runtime_error( "can't use section xml tag in plot" );
	}

	OutputState * plot()
	{
		throw runtime_error( "can't use plot xml tag in plot" );
	}

	void put_text( const string & str )
	{
		m_data << str;
	}

	void finish()
	{
		string image_file_path;
		{
			stringstream ss;
			ss << "images/" << getpid() << "_" << random() << ".pdf";
			image_file_path = ss.str();
		}
		system( "mkdir -p images" );
		{
			ofstream pdf_file( image_file_path.c_str() );
			if ( !pdf_file ) {
				throw runtime_error( "Unable to create image file" );
			}
			parse_plot( m_data.str(), pdf_file, false );
		}
		m_out << "\\begin{figure}" << endl;
		m_out << "\\centering" << endl;
		m_out << "\\includegraphics[width=0.7\\textwidth]{" << image_file_path << "}" << endl;
		m_out << "\\end{figure}" << endl;
	}
};

LatexState::LatexState( LatexBuilder & root, LatexState & parent, ostream & outs )
	: m_root( root )
	, m_parent( parent )
	, m_out( outs )
{
}

LatexState::~LatexState()
{
	if ( &m_parent == this )
	{
		/* i am root */
		m_root.m_root = NULL;
	}
	else
	{
		vector<LatexState*>::iterator it;
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
		vector<LatexState*> copy( m_children );
		m_children.clear();
		for ( vector<LatexState*>::iterator it = copy.begin(); it != copy.end(); ++it )
		{
			if ( *it != NULL )
			{
				delete *it;
			}
		}
	}
}

void LatexState::put_text( const string & str )
{
	m_out << str;
}

OutputState * LatexState::section( const std::string & section_name, unsigned int level )
{
	if ( level > 2 )
	{
		throw runtime_error( "latex backend only supports subsubsection (level=2)" );
	}
	m_out << "\\";
	for ( unsigned int i=0; i<level; ++i )
	{
		m_out << "sub";
	}
	m_out << "section{" << section_name << "}" << endl;
	LatexState * retval = new LatexState( m_root, *this, m_out );
	m_children.push_back( retval );
	return retval;
}

OutputState * LatexState::bold()
{
	LatexState * retval = new encaps_state( "bf", m_root, * this, m_out );
	m_children.push_back( retval );
	return retval;
}

OutputState * LatexState::math()
{
	LatexState * retval = new math_state( m_root, * this, m_out );
	m_children.push_back( retval );
	return retval;
}

OutputState * LatexState::plot()
{
	LatexState * retval = new latex_plot_state( m_root, * this, m_out );
	m_children.push_back( retval );
	return retval;
}

void LatexState::finish() {
}

encaps_state::encaps_state( const string & encaps, LatexBuilder & root,
							LatexState & parent, std::ostream & outs )
	: LatexState( root, parent, outs )
{
	m_out << "{\\" << encaps << " ";
}

encaps_state::~encaps_state() {}

void encaps_state::finish()
{
	m_out << "}";
}

class root_state
	: public LatexState
{
public:
	root_state( LatexBuilder & root, std::ostream & outs, bool minimal ) :
		LatexState( root, *this, outs )
	{
		if ( minimal == true )
		{
			m_out << "\\documentclass{minimal}" << endl;
			m_out << "\\usepackage{fontspec}" << endl;
			m_out << "\\usepackage{unicode-math}" << endl;
			m_out << "\\setmathfont{STIXGeneral}" << endl;
			m_out << "\\begin{document}" << endl;
		}
		else
		{
			m_out << "\\documentclass[a4paper,12pt]{article}" << endl;
			m_out << "\\usepackage{fontspec}" << endl;
			m_out << "\\usepackage{unicode-math}" << endl;
			m_out << "\\usepackage{graphicx}" << endl;
			m_out << "\\usepackage{fullpage}" << endl;
			m_out << "\\setmathfont{STIXGeneral}" << endl;
			m_out << "\\begin{document}" << endl;
		}
	}
	virtual ~root_state()
	{
	}

	void finish()
	{
		m_out << "\\end{document}" << endl;
	}
};


LatexBuilder::LatexBuilder( ostream & output_stream, bool minimal )
	: m_out( output_stream ), m_root( NULL ), m_minimal( minimal )
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
	m_root = new root_state( *this, m_out, m_minimal );
	return m_root;
}

}
