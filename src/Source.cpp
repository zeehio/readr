#include <Rcpp.h>
using namespace Rcpp;

#include "Iconv.h"
#include "Source.h"
#include "SourceFile.h"
#include "SourceRaw.h"
#include "SourceString.h"

SourcePtr Source::create(List spec) {
  std::string subclass(as<CharacterVector>(spec.attr("class"))[0]);

  int skip = as<int>(spec["skip"]);

  std::vector<std::string> comments;
  SEXP comment_sexp = spec["comment"];

  if (TYPEOF(comment_sexp) == CHARSXP) {
    std::string comment = Rf_translateCharUTF8(comment_sexp);
    if (comment.length() > 0) {
      comments.push_back(comment);
    }
  } else if (TYPEOF(comment_sexp) == STRSXP) {
    for (R_xlen_t i = 0; i < Rf_xlength(comment_sexp); ++i) {
      SEXP comm = STRING_ELT(comment_sexp, i);
      std::string comment = Rf_translateCharUTF8(comm);
      if (comment.length() > 0) {
        comments.push_back(comment);
      }
    }
  } else {
    stop("Wrong comment vector");
  }

  std::string encoding = as<std::string>(spec["encoding"]);

  if (subclass == "source_raw") {
    return SourcePtr(
        new SourceRaw(as<RawVector>(spec[0]), skip, comments, encoding));
  } else if (subclass == "source_string") {
    return SourcePtr(new SourceString(
        as<CharacterVector>(spec[0]), skip, comments, encoding));
  } else if (subclass == "source_file") {
    std::string path(as<CharacterVector>(spec[0])[0]);
    return SourcePtr(new SourceFile(path, skip, comments, encoding));
  }

  Rcpp::stop("Unknown source type");
  return SourcePtr();
}

// This function is used to test the encoding detection based on the BOM
// [[Rcpp::export]]
CharacterVector source_encoding(List spec) {
  SourcePtr source = Source::create(spec);
  return source->encoding();
}
