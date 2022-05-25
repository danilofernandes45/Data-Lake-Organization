#include "Decoder.h"

float cossine_similarity(vector<float> vector_1, vector<float> vector_2){
    float inner_product = 0;
    float norm_1 = 0;
    float norm_2 = 0;

    for (int i = 0; i < vector_1.size(); i++)
    {
        inner_product += vector_1[i] * vector_2[i];
        norm_1 += vector_1[i] * vector_1[i];
        norm_2 += vector_2[i] * vector_2[i];
    }

    norm_1 = sqrt(norm_1);
    norm_2 = sqrt(norm_2);

    return inner_product / (norm_1 * norm_2);    

}

Decoder::Decoder(std::vector< std::vector<float> > vectors, unsigned max_num_clusters) {
	 this->vectors = vectors; 
	 this->num_clusters = num_clusters;
}

Decoder::~Decoder() { }

// Runs in \Theta(n \log n):
double Decoder::decode(const std::vector< double >& chromosome) const {

	unsigned dim = vectors[0].size();
	vector< vector<double> > centroids(this->num_clusters, vector<double>(dim));
	vector< vector<int> > clusters(this->num_clusters);

	vector<double> inner_cluster_sim(this->num_clusters);
	double inter_cluster_sim = 0.0;
	double alpha = 0.0;
	unsigned id_1, id_2, allele = 0;

	for(unsigned i = 0; i < vectors.size(); i++){
		for(unsigned j = 0; j < this->num_clusters; j++){
			if( chromosome[allele] >= 0.5 ){
				clusters[j].push_back(i);
				for(unsigned r = 0; r < dim; r++)
					centroids[j][r] += vectors[i][r];
			}
			allele++;
		}
	}

	for(unsigned k = 0; k < this->num_clusters; k++){
		for(unsigned i = 0; i < clusters[k].size(); i++){
			id_1 = clusters[k][i];
			for(unsigned j = i+1; j < clusters[k].size(); j++){
				id_2 = clusters[k][j];
				inner_cluster_sim[k] += cossine_similarity(vectors[id_1], vectors[id_2]);
			}
		}
	}

	// vector< pair< double, unsigned > > ranking(chromosome.size());

	// for(unsigned i = 0; i < chromosome.size(); ++i) {
	// 	ranking[i] = std::pair< double, unsigned >(chromosome[i], i);
	// }

	// // Here we sort 'permutation', which will then produce a permutation of [n] in pair::second:
	// std::sort(ranking.begin(), ranking.end());

	// // permutation[i].second is in {0, ..., n - 1}; a permutation can be obtained as follows
	// std::list< unsigned > permutation;
	// for(std::vector< std::pair< double, unsigned > >::const_iterator i = ranking.begin();
	// 		i != ranking.end(); ++i) {
	// 	permutation.push_back(i->second);
	// }

	// sample fitness is the first allele
	return 0;
}
