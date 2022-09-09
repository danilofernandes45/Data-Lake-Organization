#ifndef ORGANIZATION_HPP
#define ORGANIZATION_HPP

#include "Cluster.hpp"
#include "Modification.hpp"
#include <algorithm>

#define DEBUG 0

class Organization
{
    public:
        Instance *instance = NULL;
        State *root = NULL;
        float gamma; // HYPERPARAMETER USED IN PROBABILITY ESTIMATION
        vector< set<State*, CompareProb> > all_states;
        vector<State*> leaves;
        float effectiveness;
        
        void compute_all_reach_probs();
        void init_all_states();
        void update_all_states(int level);
        void update_effectiveness();
        void success_probabilities();
        Organization* copy();
        void delete_parent(int level, int level_id, int update_id);
        void add_parent(int level, int level_id, int update_id);

        void update_ancestors(State *descendant, int update_id);
        void update_descendants(vector<State*> * ancestors, int update_id);
        static Organization* generate_basic_organization(Instance * inst, float gamma);
        static Organization* generate_organization_by_clustering(Instance * inst, float gamma);
};

#endif