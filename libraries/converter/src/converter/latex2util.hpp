#include <iostream>

namespace xml2epub {

void latex2pdf( std::istream & input, std::string & pdf_path );
void pdf2svg( const std::string & pdf_path, std::ostream & output );
void latex2svg( std::istream & input, std::ostream & output );

}
