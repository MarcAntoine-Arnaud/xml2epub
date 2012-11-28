#include <string>
#include <vector>
#include <iostream>
#include <libxml++/libxml++.h>
#include "builder.hh"
#pragma once

namespace xml2epub {
  class html_builder;

  class html_state : public output_state {
  protected:
    friend class html_builder;
    html_builder & m_root;
    html_state & m_parent;
    std::vector<html_state*> m_children;
    xmlpp::Element & m_xml_node;
  protected:
    html_state( html_builder & root, html_state & parent, xmlpp::Element & xml_node );  
    html_state( html_builder & root, xmlpp::Element & xml_node );  
    virtual ~html_state();
  public:
    void put_text( const std::string & str );  
    output_state * bold();
    output_state * math();
    output_state * section( const std::string & section_name, unsigned int level );
    output_state * plot();
    void finish();
  };

  class html_builder : public output_builder {
  private:
    friend class html_state;
    xmlpp::Document out_doc;
    xmlpp::Element * out_root;
    std::ostream & m_out;
    html_state * m_root;
  public:
    html_builder( std::ostream & output_stream );
    virtual ~html_builder();
    output_state * create_root();
  };

}
