#ifndef FASTREAD_TOKENIZER_NONMEM_H_
#define FASTREAD_TOKENIZER_NONMEM_H_

#include <Rcpp.h>
#include "Token.h"
#include "Tokenizer.h"
#include "utils.h"

enum NonmemState {
  NONMEM_NL,      // 0
  NONMEM_TABLE,
  NONMEM_NAME,    // 2
  NONMEM_NL_NAME,
  NONMEM_DELIM_N, // 4
  NONMEM_FIELD,
  NONMEM_DELIM_F  // 6
};

class TokenizerNonmem : public Tokenizer {
  SourceIterator begin_, cur_, end_;
  NonmemState state_;
  int row_, col_;
  bool moreTokens_;
  bool seenHeaders_;

public:

  TokenizerNonmem() {}

  void tokenize(SourceIterator begin, SourceIterator end);

  std::pair<double,size_t> progress();

  Token nextToken();
};

#endif
