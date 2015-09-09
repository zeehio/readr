#ifndef FASTREAD_QI_PARSERS
#define FASTREAD_QI_PARSERS

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

struct DecimalCommaPolicy : public boost::spirit::qi::real_policies<double> {
  template <typename Iterator>
  static bool parse_dot(Iterator& first, Iterator const& last) {
    if (first == last || *first != ',')
      return false;
    ++first;
    return true;
  }
};

bool parseDouble(char decimalMark, std::string::const_iterator& first, std::string::const_iterator& last, const double& res);
bool parseInt(std::string::const_iterator& first, std::string::const_iterator& last, const int& res);

bool parseDouble(char decimalMark, const char*& first, const char*& last, const double& res);
bool parseInt(const char*& first, const char*& last, const int& res);

#endif
