#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <libxml++/libxml++.h>
#include <tidy.h>
#include <buffio.h>

#include "latex.hh"
#include "plot.hh"

using namespace xmlpp;
using namespace std;

namespace xml2epub {
  class encaps_state : public latex_state {
  public:
    encaps_state( const string & encaps, latex_builder & root, latex_state & parent, ostream & outs );
    virtual ~encaps_state();
  public:
    void finish();
  };

  class math_state : public latex_state {
  public:
    math_state( latex_builder & root, latex_state & parent, ostream & outs ) :
      latex_state( root, parent, outs ) {
      m_out << "$";
    }
    virtual ~math_state() {
    }
    output_state * bold() {
      throw runtime_error( "can't use bold xml tag in latex math" );
    }
    output_state * math() {
      throw runtime_error( "can't use math xml tag in latex math" );
    }

    output_state * section( const std::string & section_name, unsigned int level ) {
      throw runtime_error( "can't use section xml tag in latex math" );
    }

    output_state * plot() {
      throw runtime_error( "can't use plot xml tag in latex math" );
    }    
  public:
    void finish() {
      m_out << "$";
    }
  };


class latex_plot_state : public latex_state {
  private:
    stringstream m_data;
  public:
    latex_plot_state( latex_builder & root, latex_state & parent, ostream & outs ) 
      : latex_state( root, parent, outs ) {
    }
    
    virtual ~latex_plot_state() {
    }

    output_state * bold() {
      throw runtime_error( "can't use bold xml tag in plot" );
    }

    output_state * math() {
      throw runtime_error( "can't use math xml tag in plot" );
    }

    output_state * section( const std::string & section_name, unsigned int level ) {
      throw runtime_error( "can't use section xml tag in plot" );
    }

    output_state * plot() {
      throw runtime_error( "can't use plot xml tag in plot" );
    }    

    void put_text( const string & str ) {
      m_data << str;
    }

    void finish() {
      string image_file_path;
      {
	stringstream ss;
	ss << "images/" << getpid() << "_" << random() << ".png";
	image_file_path = ss.str();
      }
      system( "mkdir -p images" );
      {
	ofstream png_file( image_file_path.c_str() );
	if ( !png_file ) {
	  throw runtime_error( "Unable to create image file" );
	}
	parse_plot( m_data.str(), png_file );
      }
      m_out << "\\begin{figure}" << endl;
      m_out << "\\centering" << endl;
      m_out << "\\includegraphics[width=0.7\\textwidth]{" << image_file_path << "}" << endl;
      m_out << "\\end{figure}" << endl;
    }
  };

  latex_state::latex_state( latex_builder & root, latex_state & parent, ostream & outs ) 
    : m_root( root ), m_parent( parent ), m_out( outs ) {
  }

  latex_state::~latex_state() {
    if ( &m_parent == this ) {
      /* i am root */
      m_root.m_root = NULL;
    } else {
      vector<latex_state*>::iterator it;
      if ( ( it = find( m_parent.m_children.begin(), m_parent.m_children.end(), this ) )
	   != m_parent.m_children.end() ) {
	m_parent.m_children.erase( it );
      }
    }
    /* delete all my children */
    if ( m_children.begin() != m_children.end() ) {
      cerr << "Warning: there are still children that have not been deleted" << endl;
      vector<latex_state*> copy( m_children );
      m_children.clear();
      for ( vector<latex_state*>::iterator it = copy.begin(); it != copy.end(); ++it ) {
	if ( *it != NULL ) {
	  delete *it;
	}
      }
    }
  }

  void latex_state::put_text( const string & str ) {
    m_out << str;
  }

  output_state * latex_state::section( const std::string & section_name, unsigned int level ) {
    if ( level > 2 ) {
      throw runtime_error( "latex backend only supports subsubsection (level=2)" );
    }
    m_out << "\\";
    for ( unsigned int i=0; i<level; ++i ) {
      m_out << "sub";
    }
    m_out << "section{" << section_name << "}" << endl;
    latex_state * retval = new latex_state( m_root, *this, m_out );
    m_children.push_back( retval );
    return retval;
  }

  output_state * latex_state::bold() {
    latex_state * retval = new encaps_state( "bf", m_root, * this, m_out );
    m_children.push_back( retval );
    return retval;
  }

  output_state * latex_state::math() {
    latex_state * retval = new math_state( m_root, * this, m_out );
    m_children.push_back( retval );
    return retval;
  }

  output_state * latex_state::plot() {
    latex_state * retval = new latex_plot_state( m_root, * this, m_out );
    m_children.push_back( retval );
    return retval;
  }

  void latex_state::finish() {
  }

  encaps_state::encaps_state( const string & encaps, latex_builder & root,
			      latex_state & parent, std::ostream & outs )
    : latex_state( root, parent, outs ) {
    m_out << "{\\" << encaps << " ";
  }

  encaps_state::~encaps_state() {}

  void encaps_state::finish() {
    m_out << "}";
  }

  class root_state : public latex_state {
  public:
    root_state( latex_builder & root, std::ostream & outs, bool minimal ) :
      latex_state( root, *this, outs ) {
      if ( minimal == true ) {
	m_out << "\\documentclass{minimal}" << endl;
	m_out << "\\usepackage{fontspec}" << endl;
	m_out << "\\usepackage{unicode-math}" << endl;
	m_out << "\\setmathfont{STIXGeneral}" << endl;
	m_out << "\\begin{document}" << endl;
      } else {
	m_out << "\\documentclass[a4paper,12pt]{article}" << endl;
	m_out << "\\usepackage{fontspec}" << endl;
	m_out << "\\usepackage{unicode-math}" << endl;
	m_out << "\\usepackage{graphicx}" << endl;
	m_out << "\\usepackage{fullpage}" << endl;
	m_out << "\\setmathfont{STIXGeneral}" << endl;
	m_out << "\\begin{document}" << endl;
      }
    }
    virtual ~root_state() {
      
    }
    void finish() {
      m_out << "\\end{document}" << endl;
    }
  };

    
  latex_builder::latex_builder( ostream & output_stream, bool minimal ) 
    : m_out( output_stream ), m_root( NULL ), m_minimal( minimal ) {
  }
    
  latex_builder::~latex_builder() {
    if ( m_root != NULL ) {
      delete m_root;
    }
  }

  output_state * latex_builder::create_root() {
    if ( m_root != NULL ) {
      delete m_root;
    }
    m_root = new root_state( *this, m_out, m_minimal );
    return m_root;
  }

}
