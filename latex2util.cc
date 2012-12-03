#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <glib.h>
#include <poppler.h>
#include <poppler-document.h>
#include <poppler-page.h>
#include <cairo.h>
#include <cairo-svg.h>

#include "latex2util.hh"

using namespace std;

namespace xml2epub {
  void latex2pdf( istream & input, string & pdf_path ) {
    string file_name;
    {
      stringstream ss;
      
      ss << getpid() << "_" << random();
      file_name = ss.str();
    }
    string tex_file("/tmp/");
    tex_file += file_name;
    tex_file += ".tex";
    ofstream file;
    file.open( tex_file.c_str(), ios_base::trunc | ios_base::out );
    if ( !file ) {
      throw runtime_error( "Unable to open tmp file" );
    }
    file << "\\documentclass{minimal}" << endl;
    file << "\\usepackage{fontspec}" << endl;
    file << "\\usepackage{unicode-math}" << endl;
    file << "\\setmathfont{STIXGeneral}" << endl;
    file << "\\begin{document}" << endl;
    file << input.rdbuf();
    file << endl;
    file << "\\end{document}" << endl;
    file.close();
    
    string command;
    {
      stringstream ss;
      ss << "(( cd /tmp; xelatex " << file_name << ".tex ) 2>&1 ) > /dev/null";
      command = ss.str();
    }
    system( command.c_str() );
    
    {
      stringstream ss;
      ss << "/tmp/" << file_name << ".pdf";
      pdf_path = ss.str();
    }
  }

  cairo_status_t cairo_to_stream_write( void * closure, const unsigned char * data, unsigned int length ) {
    ostream * str = reinterpret_cast<ostream*>(closure);
    str->write( (const char *)data, length );
    return CAIRO_STATUS_SUCCESS;
  }
  
  void latex2svg( istream & input, ostream & output ) {
    string pdf_path;
    latex2pdf( input, pdf_path );
    pdf2svg( pdf_path, output );
    unlink( pdf_path.c_str() );    
  }

  void pdf2svg( const std::string & pdf_path, std::ostream & output ) {
    gchar * filename_uri = g_filename_to_uri( pdf_path.c_str(), NULL, NULL );
    PopplerDocument * doc = poppler_document_new_from_file( filename_uri, NULL, NULL );
    if ( doc == NULL ) {
      throw runtime_error( "poppler_document_new_from_file failed!" );
    }
    PopplerPage * page = poppler_document_get_page( doc, 0 );
    if ( page != NULL ) {
      double width, height;
      poppler_page_get_size( page, &width, &height );
      
      cairo_surface_t * bSurface = cairo_recording_surface_create( CAIRO_CONTENT_COLOR_ALPHA, NULL);
      cairo_t * drawcontext = cairo_create( bSurface );
      if ( bSurface == NULL ) {
	g_object_unref( page );
	g_object_unref( doc );
	throw runtime_error( "cairo_recording_surface_create failed" );
      }
      poppler_page_render( page, drawcontext );
      cairo_show_page( drawcontext );
      cairo_destroy( drawcontext );
      double bbox_x, bbox_y, bbox_width, bbox_height;
      cairo_recording_surface_ink_extents( bSurface, &bbox_x, &bbox_y, &bbox_width, &bbox_height );
      cairo_surface_destroy( bSurface );

      cairo_surface_t * surface = 
	cairo_svg_surface_create_for_stream( cairo_to_stream_write, &output,
					     bbox_width, bbox_height );
      if ( surface == NULL ) {
	g_object_unref( page );
	g_object_unref( doc );
	throw runtime_error( "cairo_svg_surface_create_for_stream failed" );
      }
      drawcontext = cairo_create( surface );
      cairo_translate( drawcontext, -1.*bbox_x, -1.*bbox_y );
      poppler_page_render( page, drawcontext );
      cairo_show_page( drawcontext );
      cairo_destroy( drawcontext );
      cairo_surface_destroy(surface);
      g_object_unref( page );
    }
    g_object_unref( doc );
  }
}
