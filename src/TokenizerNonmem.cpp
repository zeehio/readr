#include <Rcpp.h>
using namespace Rcpp;

#include "TokenizerNonmem.h"
#include "boost.h"

void TokenizerNonmem::tokenize(SourceIterator begin, SourceIterator end) {
  cur_ = begin;
  begin_ = begin;
  end_ = end;

  row_ = 0;
  col_ = 0;
  state_ = NONMEM_NL;
  seenHeaders_ = false;
  moreTokens_ = true;
}

std::pair<double,size_t> TokenizerNonmem::progress() {
  size_t bytes = cur_ - begin_;
  return std::make_pair(bytes / (double) (end_ - begin_), bytes);
}

Token nonmemToken(SourceIterator begin, SourceIterator end, int row, int col) {
  Rcout << row << "," << col << ": " << std::string(begin, end) << "\n";
  Token t(begin, end, row, col, false);
  t.flagNA(std::vector<std::string>(1, "."));
  return t;
}

Token TokenizerNonmem::nextToken() {
  // Capture current position
  int row = row_, col = col_;

  if (!moreTokens_)
    return Token(TOKEN_EOF, row, col);

  SourceIterator tokenBegin = cur_;

  while (cur_ != end_) {
    Rcout << state_ << ": " << *cur_ << "\n";
    Advance advance(&cur_);

    if ((row_ + 1) % 100000 == 0 || (col_ + 1) % 100000 == 0)
      Rcpp::checkUserInterrupt();

    switch(state_) {
    case NONMEM_NL:
      {
        boost::iterator_range<const char*> haystack(cur_, end_);
        if (*cur_ == ' ' || *cur_ == '\t') {
          tokenBegin++; // skip leading whitespace
        } if (boost::starts_with(haystack, "TABLE")) {
          state_ = NONMEM_TABLE;
        } else {
          state_ = NONMEM_FIELD;
        }
      }
      break;

    case NONMEM_TABLE:
      if (*cur_ == '\n') {
        tokenBegin = cur_ + 1;
        state_ = NONMEM_NL_NAME;
      }

      break;

    case NONMEM_NL_NAME:
      switch(*cur_) {
      case ' ':
      case '\t':
        break;
      default:
        tokenBegin = cur_;
        state_ = NONMEM_NAME;
      }
      break;

    case NONMEM_NAME:
      switch(*cur_) {
      case ' ':
      case '\t':
        if (!seenHeaders_) {
          state_ = NONMEM_DELIM_N;
          col_++;
          return nonmemToken(tokenBegin, cur_ - 1, row, col);
        }
        break;
      case '\n':
      case '\r':
        state_ = NONMEM_FIELD;
        if (!seenHeaders_) {
          row_++;
          col_ = 0;
          seenHeaders_ = true;
          return nonmemToken(tokenBegin, advanceForLF(&cur_, end_) - 1, row, col);
        } else {
          advanceForLF(&cur_, end_);
          tokenBegin = cur_ + 1;
        }

        break;
      default:
        break;
      }
      break;

    case NONMEM_DELIM_N:
      switch(*cur_) {
      case ' ':
      case '\t':
        break;
      default:
        tokenBegin = cur_;
        state_ = NONMEM_NAME;
      }
      break;

    case NONMEM_FIELD:
      switch(*cur_) {
      case ' ':
      case '\t':
        state_ = NONMEM_DELIM_F;
        col_++;
        nonmemToken(tokenBegin, cur_ - 1, row, col);
        break;
      case '\n':
      case '\r':
        state_ = NONMEM_NL;
        col_ = 0;
        row_++;
        return nonmemToken(tokenBegin, advanceForLF(&cur_, end_) - 1, row, col);

      default:
        break;
      }
      break;

    case NONMEM_DELIM_F:
      switch(*cur_) {
      case ' ':
      case '\t':
        break;
      default:
        tokenBegin = cur_ + 1;
        state_ = NONMEM_FIELD;
        break;
      }
      break;
    }
  }

  // Reached end of Source: cur_ == end_
  moreTokens_ = false;

  // switch (state_) {
  // }

  return Token(TOKEN_EOF, row, col);
}
