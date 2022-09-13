#ifndef ORGANIZATION_HPP
#define ORGANIZATION_HPP

#include "Cluster.hpp"
#include "State.hpp"
#include <algorithm>

#define DEBUG 0

template<typename T>
bool CompareProb::operator()(const T *state_1, const T *state_2) {
    return state_1->overall_reach_prob > state_2->overall_reach_prob; 
}

class Organization
{
    public:
        Instance *instance = NULL;
        State *root = NULL;
        float gamma; // HYPERPARAMETER USED IN PROBABILITY ESTIMATION
        vector< set<State*, CompareProb> > all_states;
        vector<State*> leaves;
        float effectiveness;
        int max_num_states;
        
        void init_all_states();
        void update_effectiveness();
        void success_probabilities();
        Organization* copy();
        void delete_parent(int level, int level_id, int update_id);
        void add_parent(int level, int level_id, int update_id);
        void update_ancestors(State *descendant, int update_id);
        void update_descendants(vector<State*> * ancestors, int update_id);
        int update_reachable_states(State * descendant, State * current);
        static Organization* generate_basic_organization(Instance * inst, float gamma);
        static Organization* generate_organization_by_clustering(Instance * inst, float gamma);
};

#endif