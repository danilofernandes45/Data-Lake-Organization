#include <iostream>
#include "Decoder.h"
#include "MTRand.h"
#include "BRKGA.h"

int main(int argc, char* argv[]) {

	const unsigned n = 100;		// size of chromosomes
	const unsigned p = 1000;	// size of population
	const double pe = 0.20;		// fraction of population to be the elite-set
	const double pm = 0.10;		// fraction of population to be replaced by mutants
	const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
	const unsigned K = 1;		// number of independent populations
	const unsigned MAXT = 1;	// number of threads for parallel decoding
	
	std::vector< std::vector<float> > vectors;
	Decoder decoder(vectors);			// initialize the decoder
	
	const long unsigned rngSeed = 0;	// seed to the random number generator
	MTRand rng(rngSeed);				// initialize the random number generator
	
	// initialize the BRKGA-based heuristic
	BRKGA< Decoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);

	algorithm.evolve(100);	// evolve the population for one generation
	
	std::cout << "Best solution found has objective value = "
	 		<< algorithm.getBestFitness() << std::endl;
	
	return 0;
}