% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/background_wrapper.R
\name{readBackgroundForSampling}
\alias{readBackgroundForSampling}
\title{Estimates the background model for sampling}
\usage{
readBackgroundForSampling(file, order = 1)
}
\arguments{
\item{file}{Fasta-filename.}

\item{order}{Order of the Markov models that shall be used as the
background model. Default: order=1.}
}
\value{
None
}
\description{
This function reads a DNA sequence from a given fasta file
and uses that sequence to estimate an order-m Markov model.
\strong{Note}: This function is only used for generating the random DNA
sequence that is used for computing the empirical
distribution. When using  \code{\link{compoundPoissonDist}},
\code{\link{combinatorialDist}} or \code{\link{motifEnrichmentTest}},
this function is not relevant. Instead, consult
\code{link{readBackground}}.
}
\examples{

# Estimate first order Markov model based on the sequence provided
# in seq.fasta

file=system.file("extdata","seq.fasta", package="mdist")
readBackgroundForSampling(file,1)


}
\seealso{
\code{link{readBackground}}
}

