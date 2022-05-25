#include <iostream>
#include "Decoder.h"
#include "MTRand.h"
#include "BRKGA.h"

int main(int argc, char* argv[]) {

	const unsigned n = 12;		// size of chromosomes
	const unsigned p = 100;		// size of population
	const double pe = 0.20;		// fraction of population to be the elite-set
	const double pm = 0.10;		// fraction of population to be replaced by mutants
	const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
	const unsigned K = 1;		// number of independent populations
	const unsigned MAXT = 1;	// number of threads for parallel decoding
	
	std::vector< std::vector<float> > vectors;
	std::vector<float> v1;
	v1.push_back(1);
	v1.push_back(0);
	std::vector<float> v2;
	v2.push_back(10);
	v2.push_back(0);
	std::vector<float> v3;
	v3.push_back(0);
	v3.push_back(1);
	std::vector<float> v4;
	v4.push_back(1);
	v4.push_back(1);

	vectors.push_back(v1);
	vectors.push_back(v2);
	vectors.push_back(v3);
	vectors.push_back(v4);

	Decoder decoder(vectors, 3);			// initialize the decoder
	
	const long unsigned rngSeed = 0;	// seed to the random number generator
	MTRand rng(rngSeed);				// initialize the random number generator
	
	// initialize the BRKGA-based heuristic
	BRKGA< Decoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);

	algorithm.evolve(100);	// evolve the population for one generation
	
	std::cout << "Best solution found has objective value = "
	 		<< algorithm.getBestFitness() << std::endl;
	
	return 0;
}