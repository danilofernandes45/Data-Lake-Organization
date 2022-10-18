#ifndef ORGANIZATION_HPP
#define ORGANIZATION_HPP

#include "Cluster.hpp"
#include "State.hpp"
#include <algorithm>
#include <time.h>

#define DEBUG 0

// class CompareProb
// {
//     public:
//         template<typename T>
//         bool operator()(const T *state_1, const T *state_2);
// };

class Organization
{
    public:
        Instance *instance = NULL;
        State *root = NULL;
        float gamma; // HYPERPARAMETER USED IN PROBABILITY ESTIMATION
        vector< set<State*, CompareID> > all_states;
        vector<State*> leaves;
        float effectiveness;
        int max_num_states;
        time_t t_start; // TIME WHEN CREATE THE ORGANIZATION
        time_t t_end; // TIME WHEN FINISH THE OPTIMIZATION
        
        void init_all_states();
        void update_effectiveness();
        void success_probabilities();
        Organization* copy();
        void delete_parent(int level, int level_id, int update_id);
        void delete_level(int level, int update_id);
        void add_parent(int level, int level_id, int update_id);
        void update_ancestors(State *descendant, int update_id);
        void update_descendants(vector<State*> * ancestors, int update_id);
        int update_reachable_states(State * descendant, State * current);
        static Organization* generate_basic_organization(Instance * inst, float gamma);
        static Organization* generate_organization_by_clustering(Instance * inst, float gamma);
};

#endif