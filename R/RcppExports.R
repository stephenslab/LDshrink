# Generated by using Rcpp::compileAttributes() -> do not edit by hand
# Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#' Calculate the constant theta, given `m`
#'
#' `calc_theta` is used to calculate the shrinkage coefficient	used by LDshrink
#'
#' @param m a number indicating the size of the panel used to create the
NULL

#' @export
calc_theta <- function(m) {
    .Call(`_ldshrink_calc_theta_exp`, m)
}

shrinkCov <- function(S, mapd, m, Ne, cutoff) {
    .Call(`_ldshrink_shrinkCov`, S, mapd, m, Ne, cutoff)
}

fastldshrink <- function(genotype_data, mapd, m, Ne, cutoff, isGeno = TRUE, cov_2_cor = TRUE) {
    .Call(`_ldshrink_fastldshrink`, genotype_data, mapd, m, Ne, cutoff, isGeno, cov_2_cor)
}

calcDist <- function(map) {
    .Call(`_ldshrink_calcDist`, map)
}

sparse_ldshrink <- function(data, mapd, indices, m, Ne, cutoff, total_size, progress = TRUE, useldshrink = TRUE) {
    .Call(`_ldshrink_sparse_ldshrink`, data, mapd, indices, m, Ne, cutoff, total_size, progress, useldshrink)
}

sparse_ldshrink_p <- function(data_a, data_b, mapd_a, mapd_b, indices_a, indices_b, m, Ne, cutoff, total_size, progress = FALSE, useldshrink = TRUE) {
    .Call(`_ldshrink_sparse_ldshrink_p`, data_a, data_b, mapd_a, mapd_b, indices_a, indices_b, m, Ne, cutoff, total_size, progress, useldshrink)
}

ld2df <- function(data, mapd, rsid, m, Ne, cutoff, r2cutoff = 0.01, progress = FALSE, useldshrink = TRUE) {
    .Call(`_ldshrink_ld2df`, data, mapd, rsid, m, Ne, cutoff, r2cutoff, progress, useldshrink)
}

ld2df_p <- function(data_a, data_b, mapd_a, mapd_b, rsid_a, rsid_b, m, Ne, cutoff, r2cutoff = 0.01, progress = FALSE, useldshrink = TRUE) {
    .Call(`_ldshrink_ld2df_p`, data_a, data_b, mapd_a, mapd_b, rsid_a, rsid_b, m, Ne, cutoff, r2cutoff, progress, useldshrink)
}

flip_allele <- function(gwas_ref, gwas_alt, ld_ref, ld_alt) {
    .Call(`_ldshrink_flip_allele`, gwas_ref, gwas_alt, ld_ref, ld_alt)
}

sorted_snp_df <- function(snp_info) {
    .Call(`_ldshrink_sorted_snp_df`, snp_info)
}

set_ld_region <- function(ld_regions, snp_info, assign_all = TRUE) {
    .Call(`_ldshrink_set_ld_region`, ld_regions, snp_info, assign_all)
}

interpolate_map <- function(map, map_pos, target_pos, progress = FALSE) {
    .Call(`_ldshrink_interpolate_map`, map, map_pos, target_pos, progress)
}

