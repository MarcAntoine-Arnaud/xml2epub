#include "common/global.hpp"

#include <converter/HtmlBuilder.hpp>
#include <converter/LatexBuilder.hpp>

#include <libxml++/libxml++.h>

#include <string>
#include <stdexcept>

using namespace std;
using namespace xmlpp;

namespace xml2epub
{

void parse_node( const Node& in_node, OutputState& state )
{
	if ( dynamic_cast<const ContentNode*>( &in_node ) != NULL )
	{
		const ContentNode & content = dynamic_cast<const ContentNode &>( in_node );
		state.put_text( content.get_content() );
	}
	else
	{
		if ( dynamic_cast<const TextNode*>( &in_node ) != NULL )
		{
			const TextNode & text = dynamic_cast<const TextNode &>( in_node );
			if ( text.is_white_space() ) {
				return;
			}
		} else if ( dynamic_cast<const Element*>( &in_node ) != NULL )
		{
			const Element & element = dynamic_cast<const Element&>( in_node );
			OutputState* out = NULL;
			string name = element.get_name();
			if ( name == "b" )
			{
				out = state.bold();
			} else if ( name == "math" )
			{
				out = state.math();
			} else if ( name == "plot" )
			{
				out = state.plot();
			} else if ( ( name == "section" ) || ( name == "subsection" ) || ( name == "subsubsection" ) )
			{
				string section_name = element.get_attribute_value( "name" );
				if ( section_name.length() == 0 )
				{
					throw runtime_error( "Sections must have a name attribute" );
				}
				unsigned int level;
				if ( name == "subsection" )
				{
					level = 1;
				} else if ( name == "subsubsection" )
				{
					level = 2;
				} else
				{
					level = 0;
				}
				out = state.section( section_name, level );
			}
			else
			{
				stringstream ss;
				ss << "Unknown element with name \"" << name << "\" found!" << endl;
				throw runtime_error( ss.str().c_str() );
			}
			Node::NodeList list = in_node.get_children();
			for ( Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter )
			{
				if ( *iter != NULL )
				{
					parse_node( **iter, *out );
				}
			}
			out->finish();
			delete out;
		}
	}
}

void parse_file( bool do_html, istream& input_stream, ostream& output_stream )
{
	DomParser parser;
	parser.set_substitute_entities( true );
	parser.parse_stream( input_stream );
	if ( parser )
	{
		/* if succesfull create output */
		const Element * rootNode = parser.get_document()->get_root_node();
		if ( rootNode == NULL )
		{
			throw runtime_error( "get_root_node() failed" );
		}

		OutputBuilder* b;
		if ( do_html )
		{
			b = new HtmlBuilder( output_stream );
		} else
		{
			b = new LatexBuilder( output_stream );
		}

		/* do stuff */
		{
			const Element & root_in = dynamic_cast<const Element &>( *rootNode );
			if ( root_in.get_name() != "document" )
			{
				throw runtime_error( "root node must be document" );
			}
			OutputState * s = b->create_root();
			Node::NodeList list = root_in.get_children();
			for ( Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter )
			{
				if ( *iter != NULL )
				{
					parse_node( **iter, * s );
				}
			}
			s->finish();
			delete s;
		}
		delete b;
	}
}

}
