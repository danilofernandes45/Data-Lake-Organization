#include <stdio.h>
#include <bits/stdc++.h>
#include <vector>
#include <iostream>     // std::cout
#include <random>       // std::default_random_engine
#include <time.h>
using namespace std;

//PROJECT FILES
#include "Instance.hpp"
#include "Organization.hpp"

void print_organization(Organization *org)
{
    for (int i = 0; i < org->all_states.size(); i++)
    {
        for (State * state : org->all_states[i])
        {
            cout << state->abs_column_id << " (" << state->level << ")\nParents => ";
            for(State * parent : state->parents)
                cout << parent->abs_column_id << " ";
            
            cout << "\nChildren => ";
            for(State * child : state->children)
                cout << child->abs_column_id << " ";
            cout << "\n";
        }
        
    }
    cout << "\n";

    // for (int i = 0; i < org->all_states.size(); i++)
    // {
    //     for (State * state : org->all_states[i])
    //     {
    //         cout << "~ " << state->abs_column_id << " ~" << endl;
    //         for(int p = 0; p < org->instance->total_num_columns; p++)
    //             cout << state->reach_probs[p] << " ";
    //         cout << "\n" << state->overall_reach_prob << endl;
    //     }
        
    // }
    // cout << "\n";
}

void compare_orgs(Organization *org, Organization *new_org)
{

    set<State*, CompareID>::iterator iter_1, iter_2;
    set<State*, CompareLevel>::iterator state_1, state_2;

    assert( org->all_states.size() == new_org->all_states.size() );

    for (int i = 0; i < org->all_states.size(); i++)
    {
        assert(org->all_states[i].size() == new_org->all_states[i].size());

        iter_1 = org->all_states[i].begin();
        iter_2 = new_org->all_states[i].begin();

        for(int j = 0; j < org->all_states[i].size(); j++)
        {
            assert( (*iter_1)->abs_column_id == (*iter_2)->abs_column_id );
            assert( (*iter_1)->parents.size() == (*iter_2)->parents.size() );
            assert( (*iter_1)->children.size() == (*iter_2)->children.size() );

            // cout << "Parents" << endl;
            // state_1 = (*iter_1)->parents.begin();
            // state_2 = (*iter_2)->parents.begin();
            // for (int s = 0; s < (*iter_1)->parents.size(); s++)
            // {
            //     cout << (*state_1)->level << " " << (*state_1)->abs_column_id << " " << (*state_2)->level << " " << (*state_2)->abs_column_id << endl;
            //     state_1++;
            //     state_2++;
            // }
            // cout << endl;

            state_1 = (*iter_1)->parents.begin();
            state_2 = (*iter_2)->parents.begin();
            for (int s = 0; s < (*iter_1)->parents.size(); s++)
            {
                assert( (*state_1)->abs_column_id == (*state_2)->abs_column_id );
                state_1++;
                state_2++;
            }

            // cout << "Children" << endl;
            // state_1 = (*iter_1)->children.begin();
            // state_2 = (*iter_2)->children.begin();
            // for (int s = 0; s < (*iter_1)->children.size(); s++)
            // {
            //     cout << (*state_1)->level << " " << (*state_1)->abs_column_id << " " << (*state_2)->level << " " << (*state_2)->abs_column_id << endl;
            //     state_1++;
            //     state_2++;
            // }

            // cout << endl;


            state_1 = (*iter_1)->children.begin();
            state_2 = (*iter_2)->children.begin();
            for (int s = 0; s < (*iter_1)->children.size(); s++)
            {
                assert( (*state_1)->abs_column_id == (*state_2)->abs_column_id );
                state_1++;
                state_2++;
            }
            
            iter_1++;
            iter_2++;
        }
    }
}

Organization* modify_organization(Organization *org, int level, int level_id, int update_id)
{
    Organization *new_org_add = org->copy();
    Organization *new_org_del = org->copy();

    #if DEBUG
    cout << "ORIGINAL: " << org->effectiveness << "\n\n";
    print_organization(org);
    #endif

    new_org_add->add_parent(level, level_id, update_id);

    #if DEBUG
    cout << "ADD: " << new_org_add->effectiveness << "\n\n";
    print_organization(new_org_add);
    #endif

    new_org_del->delete_parent(level, level_id, update_id);

    #if DEBUG
    cout << "REMOVE: " << new_org_del->effectiveness << "\n\n";
    print_organization(new_org_del);
    #endif

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

    while (count < plateau_iters && org->all_states.size() >= 3)
    {
        new_org = modify_organization(org, level, level_id, update_id);
        increse_perc = ( new_org->effectiveness - org->effectiveness ) / org->effectiveness;

        if( increse_perc >= 0 ) {
            if( increse_perc  < eps )
                count++;
            else
                count = 0;
            //UPDATE ORGANIZATION
            org = new_org->copy();
            level = 2;
            level_id = 0;
            update_id = update_id + 2; 
        } else {
            prob_accept = new_org->effectiveness / org->effectiveness;
            // cout << distribution(generator) << ", " << prob_accept << endl;
            if(  distribution(generator) < prob_accept )
            {
                org = new_org->copy();
                level = 2;
                level_id = 0;
                update_id = update_id + 2;
            } else {
                level_id = ( level_id + 1 ) % org->all_states[level].size();
                if( level_id == 0 ) {
                    level++; 
                    if( level == org->all_states.size() )
                        level = 2;
                }
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
    // int K_max = 10;
    int K_max = 1;

    Organization *org, *new_org, *best_org = NULL;

    //PERFORMANCE EVALUATION
    time_t start, end;
    // int num_trials = 2;
    // cout << "effectiveness,ellapsed_time\n";

    //TEST
    // org = Organization::generate_organization_by_clustering(instance, gamma);
    // print_organization(org);
    // //TEST

    time(&start);

    org = Organization::generate_organization_by_clustering(instance, gamma);
    for (int i = 0; i < K_max; i++)
    {
        // new_org = local_search(org, 40, 0.05);
        new_org = local_search(org, 5, 0.05);
        if( best_org == NULL || new_org->effectiveness > best_org->effectiveness )
            best_org = new_org;
    }

    time(&end);

    cout << best_org->effectiveness << ", " << best_org->all_states.size() << ", " << difftime(end, start) << "\n";

    // best_org->success_probabilities();


    return 0;
}