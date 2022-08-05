#ifndef ORGANIZATION_HPP
#define ORGANIZATION_HPP

#include <algorithm>
#include <cmath>

#include "Instance.hpp"
#include "State.hpp"
#include "Decoder.hpp"
#include "MTRand.hpp"
#include "BRKGA.hpp"

#define DEBUG 0

using namespace std;

class Organization
{
    public:
        Instance *instance = NULL;
        State *root = NULL;
        float gamma; // HYPERPARAMETER USED IN PROBABILITY ESTIMATION
        vector< vector<State*> > all_states;
        vector<State*> leaves;
        float effectiveness;
        
        void compute_all_reach_probs();
        void init_all_states();
        void update_all_states(int level);
        void update_effectiveness();
        Organization* copy();
        void delete_parent(int level, int level_id, int update_id);
        void add_parent(int level, int level_id, int update_id);
        // void destroy();

        void build_leaves(); // NEW

        // static int update_ancestors(State *descendant, Instance *inst, float gamma, int update_id);
        static void update_descendants(State *patriarch, float gamma, int total_num_columns, int update_id);
        static Organization* generate_basic_organization(Instance * inst, float gamma);
        // static Organization* generate_organization_by_clustering(Instance * inst, float gamma);

        static Organization* build_organization_BRKGA(Instance * inst, float gamma, const unsigned p, const double pe, const double pm, const double rhoe, const unsigned K, const unsigned MAXT); // NEW
};

#endif