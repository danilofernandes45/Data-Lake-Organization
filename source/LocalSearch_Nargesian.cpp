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

void print_organization(Organization *org)
{
    for (int i = 0; i < org->all_states.size(); i++)
    {
        for (int j = 0; j < org->all_states[i].size(); j++)
        {
            cout << org->all_states[i][j]->abs_column_id << " (" << org->all_states[i][j]->level << ")\nParents => ";
            for(int k = 0; k < org->all_states[i][j]->parents.size(); k++)
                cout << org->all_states[i][j]->parents[k]->abs_column_id << " ";
            
            cout << "\nChildren => ";
            for(int k = 0; k < org->all_states[i][j]->children.size(); k++)
                cout << org->all_states[i][j]->children[k]->abs_column_id << " ";
            cout << "\n";
        }
        
    }
    
}

Organization* modify_organization(Organization *org, int level, int level_id, int update_id)
{
    Organization *new_org_add = org->copy();
    Organization *new_org_del = org->copy();

    cout << "ORIGINAL: " << org->effectiveness << "\n\n";
    print_organization(org);

    new_org_add->add_parent(level, level_id, update_id);
    cout << "ADD: " << new_org_add->effectiveness << "\n\n";
    print_organization(new_org_add);

    new_org_del->delete_parent(level, level_id, update_id);
    cout << "REMOVE: " << new_org_del->effectiveness << "\n\n";
    print_organization(new_org_del);

    if(new_org_add->effectiveness > new_org_del->effectiveness)
        return new_org_add;
    return new_org_del;
}

Organization* local_search(Organization *org, int plateau_iters, float eps)
{
    Organization *new_org;
    int level = 2, level_id = 0;
    int count = 0, update_id = 1;
    float prob_accept, increse_perc;
    //GENERATOR OF RANDOM NUMBERS
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_real_distribution<float> distribution(0.0, 1.0);

    // print_organization(org);
    // cout << "First Effectiveness: " << org->effectiveness << "\n\n";

    while (count < plateau_iters && org->all_states.size() >= 3)
    {
        new_org = modify_organization(org, level, level_id, update_id);
        increse_perc = ( new_org->effectiveness - org->effectiveness ) / org->effectiveness;

        // cout << "\nEffectiveness: " << new_org->effectiveness << "\n";

        if( increse_perc >= 0 ) {
            if( increse_perc  < eps )
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
            if(  distribution(generator) < prob_accept )
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
            count++;
        }
    }
    return org;
}

int main()
{
    Instance * instance = Instance::read_instance();
    float gamma = 1.0;

    Organization *org = Organization::generate_organization_by_clustering(instance, gamma);
    local_search(org, 2, 0.01);
    return 0;
}