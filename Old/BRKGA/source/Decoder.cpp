#include "Decoder.hpp"

solution Decoder::decode_solution(vector< double >& chromosome) const {

	unsigned dim = this->states[0]->sum_vector.size();
	vector< vector<double> > centroids(this->num_clusters, vector<double>(dim));
	vector< vector<int> > clusters(this->num_clusters);
	vector< vector<int> > domain(this->num_clusters, vector<int>(this->leaves.size()));

	bool flag;
	double max_value;
	vector<double>::iterator iter;
	unsigned allele = 0;

	for(unsigned i = 0; i < states.size(); i++){
		flag = true;
		for(unsigned j = 0; j < this->num_clusters; j++){
			if( chromosome[allele] >= 0.5 ){
				flag = false;
				clusters[j].push_back(i);
				for(unsigned d = 0; d < this->leaves.size(); d++){
					if(domain[j][d] == 0 && states[i]->domain[d] == 1){
						domain[j][d] = 1;
						for(unsigned r = 0; r < dim; r++)
							centroids[j][r] += leaves[d]->sum_vector[r];
					}
				}
			}
			allele++;
		}
		//Correction: Every vector must belongs to at least one cluster
		if( flag ){
			allele -= this->num_clusters;
			iter = chromosome.begin() + allele;
			max_value = *max_element(iter, iter + this->num_clusters);
			for_each(iter, iter+this->num_clusters, [max_value](double &c){ c /= max_value; });
			i--;
		}
	}

	return {centroids, clusters, domain};
}

Decoder::Decoder(vector< State* > states, vector< State* > leaves, unsigned num_clusters) {
	 this->states = states; 
	 this->leaves = leaves;
	 this->num_clusters = num_clusters;
}

Decoder::~Decoder() { }

double Decoder::decode(vector< double >& chromosome) const {

	// return chromosome[0];

	double inner_cluster_sim;
	double inter_cluster_sim = 0.0;
	double fitness = 0.0;
	double alpha = 0.0;
	unsigned id_1, id_2;
	unsigned non_empty_clusters = 0;

	solution clustering = this->decode_solution(chromosome);
	vector< vector<double> > centroids = get<0>(clustering);
	vector< vector<int> > clusters = get<1>(clustering);

	for(unsigned k = 0; k < this->num_clusters; k++){
		inner_cluster_sim = 0.0;
		for(unsigned i = 0; i < clusters[k].size(); i++){
			id_1 = clusters[k][i];
			for(unsigned j = i+1; j < clusters[k].size(); j++){
				id_2 = clusters[k][j];
				inner_cluster_sim += cossine_similarity(states[id_1]->sum_vector, states[id_2]->sum_vector);
			}
		}
		if(clusters[k].size() > 1){
			inner_cluster_sim *= 2 / ( clusters[k].size() * ( clusters[k].size() - 1 ) );
			fitness += inner_cluster_sim / clusters[k].size();
			alpha += 1.0 / clusters[k].size();
		}
	}
	if( alpha > 0 )
		fitness /= alpha;

	// cout << fitness << " ";

	for(unsigned i=0; i < this->num_clusters; i++){
		if( clusters[i].size() > 0 ){
			for(unsigned j=i+1; j < this->num_clusters; j++){
				if( clusters[j].size() > 0 )
					inter_cluster_sim += cossine_similarity(centroids[i], centroids[j]);
			}
			non_empty_clusters++;
		}
	}

	if( non_empty_clusters > 1 ){
		inter_cluster_sim *= 2 / ( non_empty_clusters * (non_empty_clusters - 1) );
		fitness -= inter_cluster_sim;
	}

	// cout << inter_cluster_sim << " " << fitness << "\n";

	return -fitness; //API performs a minimization!
}
