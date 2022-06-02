/*
 * SampleDecoder.h
 *
 * Any decoder must have the format below, i.e., implement the method decode(std::vector< double >&)
 * returning a double corresponding to the fitness of that vector. If parallel decoding is to be
 * used in the BRKGA framework, then the decode() method _must_ be thread-safe; the best way to
 * guarantee this is by adding 'const' to the end of decode() so that the property will be checked
 * at compile time.
 *
 * The chromosome inside the BRKGA framework can be changed if desired. To do so, just use the
 * first signature of decode() which allows for modification. Please use double values in the
 * interval [0,1) when updating, thus obeying the BRKGA guidelines.
 *
 *  Created on: Jan 14, 2011
 *      Author: rtoso
 */

#ifndef DECODER_HPP
#define DECODER_HPP

#include <list>
#include <vector>
#include <algorithm>

#include <bits/stdc++.h>

#include "utils.hpp"
#include "State.hpp"

#define solution tuple< vector< vector<double> >,  vector< vector<int> >, vector< vector<int> > >

using namespace std;

class Decoder {
public:
	Decoder(vector< State* > states, vector< State* > leaves, unsigned num_clusters);
	~Decoder();

	double decode(vector< double >& chromosome) const;
	solution decode_solution(vector< double >& chromosome) const;

private:
	unsigned num_clusters;
	vector< State* > states;
	vector< State* > leaves;
};

#endif
