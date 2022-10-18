#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "State.hpp"
#include "Instance.hpp"
#include "utils.hpp"

class Cluster
{
    public:
        State *state = NULL;
        int id; // DISTANCES MATRIX ID
        Cluster *is_NN_of = NULL; //IS NEAREST NEIGHBOR OF
        Cluster *next = NULL; // CHAINED LIST OF ACTIVE CLUSTERS
        int cardinality; //NUMBER OF ELEMENTS CONTAINED IN THIS CLUSTERS

        static Cluster* init_clusters(Instance * inst);
        static float** init_dist_matrix(Cluster* active_clusters, int total_num_columns, int embedding_dim);
        static Cluster* merge_clusters(Cluster *stack, float **dist_matrix, int cluster_id, int state_id, Instance *inst);

};

#endif