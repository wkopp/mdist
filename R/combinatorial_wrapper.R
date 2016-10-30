
combinatorialDist=function(seqlen, overlap, singlestranded=TRUE, maxhits=100) {
  if (!all(seqlen==seqlen[1])) {
     stop("The sequences must be of equal length for dynamic programming!");
  }
  if (singlestranded==TRUE) {
      warning("This function must be checked. Originally, the method was intended for scanning both DNA strands only.")
  }
  dist=numeric(maxhits+1)
  ret=.C("RPosteriorProbability", 
  overlap$alpha, overlap$beta, overlap$beta3p, overlap$beta5p,
  as.numeric(dist), as.integer(seqlen[1]),
   as.integer(maxhits), as.integer(length(seqlen)))
  return(list(dist=ret[[5]]))
}

#markov.prob=function(overlap,N) {
	#ret=.Call("getMarkovProb",as.integer(N),overlap$alpha,overlap$beta,overlap$beta3p,overlap$beta5p);
	#return(ret)
#}

#sample.mc=function(alpha,beta,beta3p,beta5p,slen,nos,perm) {
	#ret=.Call("sample_mc",
						#as.numeric(alpha),beta,beta3p,beta5p,
						#as.integer(slen),as.integer(nos),as.integer(perm));
#}