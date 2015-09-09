#include "QiParsers.h"
#include <boost/spirit/include/qi.hpp>
namespace qi = boost::spirit::qi;

template <typename Iterator>
bool parseDouble(char decimalMark, Iterator& first, Iterator& last,
                 const double& res) {
  if (decimalMark == '.') {
    return qi::parse(first, last, qi::double_, res);
  } else if (decimalMark == ',') {
    return qi::parse(first, last, qi::real_parser<double, DecimalCommaPolicy>(), res);
  } else {
    return false;
  }
}

template <typename Iterator>
bool parseInt(Iterator& first, Iterator& last, const int& res) {
  return qi::parse(first, last, qi::int_, res);
}
