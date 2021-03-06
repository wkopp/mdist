#ifdef IN_R
#include <R.h>
#endif

#include "matrix.h"
#include "score2d.h"
#include "overlap.h"
#include "compoundpoisson.h"

#define EPSILON 1e-10
extern double *Rstation, *Rtrans;
extern int Rorder;
extern DMatrix *Rpwm, *Rcpwm;
extern double Rsiglevel, Rgran;

void RcompoundpoissonPape_useGamma(double *gamma, 
  double *hitdistribution, int *nseq, int *lseq, int * mhit, int *mclump) {
    int seqlen, i;
    int maxclumpsize, maxhits;
    double lambda;
    double *theta, extention[3];

    if (!Rpwm||!Rcpwm||!Rstation||!Rtrans) {
        error("load forground and background properly");
        return;
    }
    if (!gamma||!hitdistribution||!nseq||!lseq||!mhit||!mclump) {
        error("parameters are null");
        return;
    }
    if (Rgran==0.0 || Rsiglevel==0.0) {
        error("call mdist.option  first");
        return;
    }

    seqlen=0;
    for (i=0; i<*nseq; i++) {
        seqlen+=lseq[i]-Rpwm->nrow+1;
    }
    maxclumpsize=(double)mclump[0];
    maxhits=(double)mhit[0];

    memset(extention, 0, 3*sizeof(double));
            gamma[Rpwm->nrow]=(gamma[Rpwm->nrow]+EPSILON)/(1+2*EPSILON);
    computeExtentionFactorsPape(extention, gamma, Rpwm->nrow);
    theta=initTheta(maxclumpsize);
    if (theta==NULL) {
        error("Memory-allocation in initTheta failed");
    }

    computeInitialClump(theta, gamma,Rpwm->nrow);
    computeTheta(maxclumpsize, theta, extention, Rpwm->nrow);

    lambda=computePoissonParameter(seqlen, Rpwm->nrow, 
            maxclumpsize, gamma[0],theta);
    computeCompoundPoissonDistributionKemp(lambda, maxhits, 
            maxclumpsize, theta, hitdistribution);
    deleteTheta(theta);
}

void Rcompoundpoisson_useBeta(double *alpha, double *beta, 
                double *beta3p, double *beta5p,
                double *hitdistribution, int *nseq, 
                int *lseq, int * mhit, int *mclump, int *sstrand) {
    int seqlen, i;
    int maxclumpsize, maxhits, singlestranded;
    double lambda;
    double *theta, extention[3];
    double *delta, *deltap;

    if (!Rpwm||!Rcpwm||!Rstation||!Rtrans) {
        error("load forground and background properly");
        return;
    }
    if (!alpha||!beta||!beta3p||!beta5p||
            !hitdistribution||!nseq||!lseq||!mhit||!mclump) {
        error("parameters are null");
        return;
    }
    if (Rgran==0.0 || Rsiglevel==0.0) {
        error("call mdist.option  first");
        return;
    }

    //compute the total length of the sequence
    seqlen=0;
    for (i=0; i<*nseq; i++) {
        seqlen+=lseq[i]-Rpwm->nrow+1;
    }
    //init the maximal clump size and the max number of hits
    maxclumpsize=(double)mclump[0];
    maxhits=(double)mhit[0];
    singlestranded=*sstrand;

    delta=Calloc(Rpwm->nrow,double);
    deltap=Calloc(Rpwm->nrow,double);
    if (delta==NULL||deltap==NULL) {
        error("Memory-allocation in Rcompoundpoisson_useBeta failed");
    }

    // initialize the extention factors
    memset(extention, 0, 3*sizeof(double));

    if (singlestranded==1) {
        computeDeltasSingleStranded(delta, beta, Rpwm->nrow);

        computeExtentionFactorsKoppSingleStranded(extention, beta, Rpwm->nrow);
        theta=initThetaSingleStranded(maxclumpsize);
        if (theta==NULL) {
            error("Memory-allocation in initTheta failed");
        }

        computeInitialClumpKoppSingleStranded(theta, delta, Rpwm->nrow);
        computeThetaSingleStranded(maxclumpsize, theta, extention, Rpwm->nrow);

        lambda=computePoissonParameterSingleStranded(seqlen, Rpwm->nrow, 
                    maxclumpsize, alpha[0],theta);

        computeCompoundPoissonDistributionKempSingleStranded(lambda, maxhits, 
                    maxclumpsize, theta, hitdistribution);
    } else {
        beta3p[0]=(beta3p[0]+EPSILON)/(1+2*EPSILON);
        computeDeltas(delta, deltap, beta, beta3p,beta5p,Rpwm->nrow);


        computeExtentionFactorsKopp(extention, delta, deltap, beta, 
                    beta3p, beta5p, Rpwm->nrow);
        theta=initTheta(maxclumpsize);
        if (theta==NULL) {
            error("Memory-allocation in initTheta failed");
        }

        computeInitialClumpKopp(theta, beta3p,delta, deltap, Rpwm->nrow);
        computeTheta(maxclumpsize, theta, extention, Rpwm->nrow);

        lambda=computePoissonParameter(seqlen, Rpwm->nrow, maxclumpsize, 
                                alpha[0],theta);

        computeCompoundPoissonDistributionKemp(lambda, maxhits, maxclumpsize, 
                                theta, hitdistribution);
    }



    deleteTheta(theta);
    Free(delta);
    Free(deltap);
}


