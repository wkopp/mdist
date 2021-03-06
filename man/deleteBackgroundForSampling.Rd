% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/background_wrapper.R
\name{deleteBackgroundForSampling}
\alias{deleteBackgroundForSampling}
\title{Delete the current background model for sampling}
\usage{
deleteBackgroundForSampling()
}
\value{
None
}
\description{
Similar to \code{link{deleteBackgroundForSampling}}, but
deletes parameters that where acquired using 
\code{link{readBackgroundForSampling}}.
}
\examples{

# Estimate first order Markov model based on the sequence provided
# in seq.fasta

seqfile=system.file("extdata","seq.fasta", package="mdist")
readBackgroundForSampling(file=seqfile,1)
deleteBackgroundForSampling()

}
\seealso{
\code{link{deleteBackgroundForSampling}}
}

