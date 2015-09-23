#' Read NONMEM output files.
#'
#' @inheritParams read_delim
#' @export
#' @examples
#' read_nonmem("TABLE NO.
#'   NAME         THETA1       THETA2       THETA3
#'   THETA1        8.52824E-02 -7.41289E-02 -1.11339E-01
#'   THETA2       -7.41289E-02  2.90189E+00 -2.10314E-01
#'   THETA3       -1.11339E-01 -2.10314E-01  2.95712E+01
#'   THETA4       -5.79811E-01  2.42365E-01  5.31406E-02
#' ")
read_nonmem <- function(file, col_names = FALSE, col_types = NULL,
                        skip = 0, n_max = -1, progress = interactive()) {

  tokenizer <- tokenizer_nonmem()
  read_delimited(file, tokenizer, col_names = col_names, col_types = col_types,
    skip = skip, n_max = n_max, progress = progress)
}
