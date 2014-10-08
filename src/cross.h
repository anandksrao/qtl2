// general qtlcross class
//
// see cross.cpp for info on how to add a new cross type

#ifndef CROSS_H
#define CROSS_H

#include <Rcpp.h>

using namespace Rcpp;

class QTLCross
{

public:
    String type;

    String phase_known_type;

    static QTLCross* Create(String type);

    virtual bool check_geno(int gen, bool is_observed_value,
                            bool is_X_chr, bool is_female,
                            IntegerVector cross_info)
    {
        if(is_observed_value && gen==0) return true;

        if(gen==1 || gen==2) return true;

        throw std::range_error("invalid genotype");
        return false; // can't get here
    }

    virtual double init(int true_gen,
                        bool is_X_chr, bool is_female,
                        IntegerVector cross_info)
    {
        check_geno(true_gen, false, is_X_chr, is_female, cross_info);

        return -log(2.0);
    }

    virtual double emit(int obs_gen, int true_gen, double error_prob,
                        bool is_X_chr, bool is_female,
                        IntegerVector cross_info)
    {

        check_geno(obs_gen, true, is_X_chr, is_female, cross_info);
        check_geno(true_gen, false, is_X_chr, is_female, cross_info);

        if(obs_gen==0) return 0.0; // missing

        if(obs_gen == true_gen) return log(1.0 - error_prob);
        else return log(error_prob);

    }

    virtual double step(int gen_left, int gen_right, double rec_frac,
                        bool is_X_chr, bool is_female,
                        IntegerVector cross_info)
    {

        check_geno(gen_left, false, is_X_chr, is_female, cross_info);
        check_geno(gen_right, false, is_X_chr, is_female, cross_info);

        if(gen_left == gen_right) return log(1.0-rec_frac);
        else return log(rec_frac);
    }

    virtual int ngen(bool is_X_chr)
    {
        return 2;
    }

    virtual IntegerVector possible_gen(bool is_X_chr, bool is_female,
                                       IntegerVector cross_info)
    {
        int ng = ngen(is_X_chr);
        IntegerVector x(ng);
        for(int i=0; i<ng; i++) x[i] = i+1;
        return x;
    }

    virtual double nrec(int gen_left, int gen_right,
                        bool is_X_chr, bool is_female,
                        IntegerVector cross_info)
    {
        check_geno(gen_left, false, is_X_chr, is_female, cross_info);
        check_geno(gen_right, false, is_X_chr, is_female, cross_info);

        if(gen_left == gen_right) return 0.0;
        else return 1.0;
    }

    virtual double est_rec_frac(NumericMatrix gamma, bool is_X_chr)
    {
        int n_gen = gamma.rows();
        int n_gen_sq = n_gen*n_gen;

        double denom = 0.0;
        for(int i=0; i<n_gen_sq; i++) denom += gamma[i];

        double diagsum = 0.0;
        for(int i=0; i<n_gen; i++) diagsum += gamma(i,i);

        return 1.0 - diagsum/denom;

    }

    // the following is for checking with a crosstype is supported from R
    // (some classes, like f2pk, aren't appropriate on the R side
    virtual bool crosstype_supported()
    {
        return true;
    }

};

#endif // CROSS.H
