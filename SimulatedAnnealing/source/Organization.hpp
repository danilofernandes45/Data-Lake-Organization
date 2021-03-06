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
        vector< vector<State*> > all_states;
        vector<State*> leaves;
        float effectiveness;
        Modification *last_modification = NULL; //LAST MODIFICATION ON THE ORGANIZATION
        
        void compute_all_reach_probs();
        void init_all_states();
        void update_all_states(int level);
        void update_effectiveness();
        void success_probabilities();
        Organization* copy();
        vector<State*> delete_parent(int level, int level_id, int update_id);
        int add_parent(int level, int level_id, int update_id);

        void undo_add_parent(Organization* prev_org, int level, int level_id, int min_level);
        void undo_delete_parent(Organization* prev_org, vector<State*> deleted_states, int level);
        void undo_last_operation(Organization* prev_org);
        void destroy();

        static int update_ancestors(State *descendant, Instance *inst, float gamma, int update_id);
        static void update_descendants(State *patriarch, float gamma, int total_num_columns, int update_id);
        static Organization* generate_basic_organization(Instance * inst, float gamma);
        static Organization* generate_organization_by_clustering(Instance * inst, float gamma, bool compute_probs = true);
        static Organization* generate_organization_by_heuristic(Instance * inst, float gamma);
};

#endif