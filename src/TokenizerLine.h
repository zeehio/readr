#ifndef FASTREAD_TOKENIZERLINE_H_
#define FASTREAD_TOKENIZERLINE_H_

#include "Token.h"
#include "Tokenizer.h"
#include "utils.h"
#include <Rcpp.h>

class TokenizerLine : public Tokenizer {
  SourceIterator begin_, cur_, end_;
  std::vector<std::string> NA_;
  bool moreTokens_;
  int line_;

public:
  TokenizerLine(std::vector<std::string> NA) : NA_(NA), moreTokens_(false) {}

  TokenizerLine() : moreTokens_(false) {}

  void tokenize(SourcePtr source) {
    begin_ = source->begin();
    cur_ = source->begin();
    end_ = source->end();
    line_ = 0;
    moreTokens_ = true;
  }

  std::pair<double, size_t> progress() {
    size_t bytes = cur_ - begin_;
    return std::make_pair(bytes / (double)(end_ - begin_), bytes);
  }

  Token nextToken() {
    SourceIterator token_begin = cur_;

    bool hasNull = false;

    if (!moreTokens_)
      return Token(TOKEN_EOF, line_, 0);

    while (cur_ != end_) {
      Advance advance(&cur_);

      if (*cur_ == '\0')
        hasNull = true;

      if ((line_ + 1) % 500000 == 0)
        Rcpp::checkUserInterrupt();

      switch (*cur_) {
      case '\r':
      case '\n': {
        Token t =
            Token(token_begin, advanceForLF(&cur_, end_), line_++, 0, hasNull);
        t.flagNA(NA_);
        return t;
      }
      default:
        break;
      }
    }

    // Reached end of Source: cur_ == end_
    moreTokens_ = false;
    if (token_begin == end_) {
      return Token(TOKEN_EOF, line_++, 0);
    } else {
      Token t = Token(token_begin, end_, line_++, 0, hasNull);
      t.flagNA(NA_);
      return t;
    }
  }
};

#endif
