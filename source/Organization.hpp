#ifndef ORGANIZATION_HPP
#define ORGANIZATION_HPP

#include "Cluster.hpp"
#include <algorithm>

class Organization
{
    public:
        int embedding_dim;
        State *root = NULL;
        float gamma; // HYPERPARAMETER USED IN PROBABILITY ESTIMATION
        vector< vector<State*> > all_states;
        float effectiveness;
        
        void compute_all_reach_probs(Instance *inst);
        void init_all_states();
        void update_effectiveness(Instance *inst);
        Organization* copy(int total_num_columns, int embedding_dim);
        void delete_parent(int level, int level_id, int total_num_columns, int update_id);
        void add_parent(int level, int level_id, Instance *inst, int gamma, int update_id);

        static void update_ancestors(State *descendant, Instance *inst, float gamma, int update_id);
        static void update_descendants(State *patriarch, float gamma, int total_num_columns, int update_id);
        static Organization* generate_basic_organization(Instance * inst, float gamma);
        static Organization* generate_organization_by_clustering(Instance * inst, float gamma);
};

#endif