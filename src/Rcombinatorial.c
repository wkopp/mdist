#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <R_ext/Applic.h>

#include "overlap.h"
#include "combinatorial.h"
#include "score2d.h"
#include "markovchain.h"

extern int Rorder;
extern double *Rstation, *Rtrans;
extern DMatrix *Rcpwm, *Rpwm;
extern double Rgran, Rsiglevel;

#define DEBUG
#undef DEBUG
void RPosteriorProbability(double *alpha, double *beta, 
    double *beta3p, double *beta5p,
    double *hitdistribution, int *sseqlen,
    int *smaxhits, int *snos, int *singlestranded) {
    PosteriorCount prob;
    int seqlen;
    int i, maxhits, k, nos;
    int totalmaxhits; // total number of hits in the entire set of seq.
    double *prior, Zpartition;
    double *singlehitdistribution;
    double *delta, *deltap;
    double a0, aN;
    double abstol=1e-30, intol=1e-30;
    int trace=0, fail,fncount, type=2, gncount;
    double *extra=NULL;
    double sum, res;

    if (!Rpwm||!Rcpwm||!Rstation||!Rtrans) {
        error("load forground and background properly");
        return;
    }
    if (!beta||!beta3p||!beta5p||!hitdistribution||
                    !sseqlen||!smaxhits||!snos) {
        error("parameters are null");
        return;
    }

    seqlen=sseqlen[0] - Rpwm->nrow+1;
    nos=snos[0];
    maxhits=smaxhits[0];
    totalmaxhits=maxhits*nos;

    delta=Calloc(Rpwm->nrow,double);
    deltap=Calloc(Rpwm->nrow,double);
    if (delta==NULL||deltap==NULL) error("failed to allocate delta");

    computeDeltas(delta, deltap, beta, beta3p,beta5p,Rpwm->nrow);

    // correct bias of alpha by fitting 
    // markov model to the stationary distribution
    extra=Calloc(3*Rpwm->nrow+1, double);
    if (extra==NULL) {
        error("Memory-allocation in RPosteriorProbability failed");
    }
    extra[0]=alpha[0];
    a0=alpha[0];
    for (i=0; i<Rpwm->nrow; i++) {
        extra[1+i]=beta[i];
        extra[Rpwm->nrow+1+i]=beta3p[i];
        extra[2*Rpwm->nrow+1+i]=beta5p[i];
    }

    cgmin(1, &a0, &aN, &res, minmc, dmc, &fail, abstol, intol,
        (void*)extra, type, trace, &fncount, &gncount, 100);

    Free(extra);
    removeDist();

    allocPosteriorProbability(&prob, seqlen, Rpwm->nrow, maxhits);
    initPosteriorProbability(&prob, alpha[0], &beta, &beta3p, &beta5p, 
        &delta, &deltap);

    computePosteriorProbability(&prob);

    singlehitdistribution=Calloc(maxhits+1, double);
    if (singlehitdistribution==NULL) {
        error("Memory-allocation in RPosteriorProbability failed");
    }

#ifdef DEBUG
    Rprintf("omega=%e, alpha=%e\n", prob.omega, prob.alpha);
#endif


    // renormalize the distribution so that it sums to one
    singlehitdistribution[0]=prob.probzerohits;
    Zpartition=singlehitdistribution[0];

    for (k=1; k<=maxhits; k++) {
        finishPosteriorProbability(&prob, singlehitdistribution, k);
        Zpartition+=singlehitdistribution[k];
    }
    for (k=0; k<=maxhits; k++) {
        singlehitdistribution[k]/=Zpartition;
    }

    // compute the distribution of the number of hits
    // across multiple independent DNA sequences
    multipleShortSequenceProbability(singlehitdistribution, hitdistribution, 
                maxhits, totalmaxhits, nos);
    for (k=0,sum=0.0; k<=totalmaxhits; k++) {
        sum+=hitdistribution[k];
    }

    deletePosteriorProbability(&prob);
    Free(singlehitdistribution);
    Free(delta);
    Free(deltap);
    Free(prior);

    return;
}
