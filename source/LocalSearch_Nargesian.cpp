#include <stdio.h>
#include <bits/stdc++.h>
#include <vector>
#include <iostream>     // std::cout
// #include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine
// #include <chrono>       // std::chrono::system_clock
using namespace std;

//PROJECT FILES
#include "Instance.hpp"
#include "Organization.hpp"

Organization* modify_organization(Organization *org, Instance *inst, int level, int level_id, int update_id)
{
    Organization *new_org_add = org->copy(inst->total_num_columns, inst->embedding_dim);
    Organization *new_org_del = org->copy(inst->total_num_columns, inst->embedding_dim);

    new_org_add->add_parent(level, level_id, inst, org->gamma, update_id);
    new_org_add->update_effectiveness(inst);

    new_org_del->delete_parent(level, level_id, inst->total_num_columns, update_id);
    new_org_del->update_effectiveness(inst);

    if(new_org_add->effectiveness > new_org_del->effectiveness)
        return new_org_add;
    return new_org_del;
}

Organization* local_search(Organization *org, Instance *inst, int plateau_iters, float eps)
{
    Organization *new_org;
    int level = 2, level_id = 0;
    int count = 0, update_id = 1;
    float prob_accept;

    default_random_engine generator();
    uniform_real_distribution<float> distribution(0.0, 1.0);

    while (count < plateau_iters && org->all_states.size() >= 3)
    {
        new_org = modify_organization(org, inst, level, level_id, update_id);
        if( new_org->effectiveness >= org->effectiveness ) {
            if( ( new_org->effectiveness - org->effectiveness ) < eps )
                count++;
            else
                count = 0;
            //UPDATE ORGANIZATION
            org = new_org;
            level = 2;
            level_id = 0;
            update_id++; 
        } else {
            prob_accept = new_org->effectiveness / org->effectiveness;
            if( distribution(generator) < prob_accept )
            {
                org = new_org;
                level = 2;
                level_id = 0;
                update_id++;
            } else {
                level_id = ( level_id + 1 ) % org->all_states[level].size();
                if( level_id == 0 )
                    level = ( level + 1 ) % org->all_states.size();
            }
        }
    }
    return org;
}

int main()
{
    Instance * instance = Instance::read_instance();
    float gamma = 1.0;

    Organization *org = Organization::generate_organization_by_clustering(instance, gamma);
    local_search(org, instance, 50, 0.001);
    return 0;
}