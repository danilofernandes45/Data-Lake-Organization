#include <stdio.h>
#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <iostream>     // std::cout
#include <random>       // std::default_random_engine
#include <time.h>
#include <map>

using namespace std;

//PROJECT FILES
#include "Instance.hpp"
#include "Organization.hpp"

#define best_neighbor_func Organization* (*)(Organization*, int)

void print_organization(Organization *org)
{
    for(unsigned i = 0; i < org->all_states.size(); i++)
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

    for(unsigned i = 0; i < org->all_states.size(); i++)
    {
        for (State * state : org->all_states[i])
        {
            cout << "~ " << state->abs_column_id << " ~" << endl;
            for(int p = 0; p < org->instance->total_num_columns; p++)
                cout << state->reach_probs[p] << " ";
            cout << "\n" << state->overall_reach_prob << endl;
        }
        
    }
    cout << "\n";
}

void compare_orgs(Organization *org, Organization *new_org)
{

    set<State*, CompareID>::iterator iter_1, iter_2;
    set<State*, CompareLevel>::iterator state_1, state_2;

    assert( org->all_states.size() == new_org->all_states.size() );

    for(unsigned i = 0; i < org->all_states.size(); i++)
    {
        assert(org->all_states[i].size() == new_org->all_states[i].size());

        iter_1 = org->all_states[i].begin();
        iter_2 = new_org->all_states[i].begin();

        for(unsigned j = 0; j < org->all_states[i].size(); j++)
        {
            assert( (*iter_1)->abs_column_id == (*iter_2)->abs_column_id );
            assert( (*iter_1)->parents.size() == (*iter_2)->parents.size() );
            assert( (*iter_1)->children.size() == (*iter_2)->children.size() );

            // cout << "Parents" << endl;
            // state_1 = (*iter_1)->parents.begin();
            // state_2 = (*iter_2)->parents.begin();
            // for(unsigned s = 0; s < (*iter_1)->parents.size(); s++)
            // {
            //     cout << (*state_1)->level << " " << (*state_1)->abs_column_id << " " << (*state_2)->level << " " << (*state_2)->abs_column_id << endl;
            //     state_1++;
            //     state_2++;
            // }
            // cout << endl;

            state_1 = (*iter_1)->parents.begin();
            state_2 = (*iter_2)->parents.begin();
            for(unsigned s = 0; s < (*iter_1)->parents.size(); s++)
            {
                assert( (*state_1)->abs_column_id == (*state_2)->abs_column_id );
                state_1++;
                state_2++;
            }

            // cout << "Children" << endl;
            // state_1 = (*iter_1)->children.begin();
            // state_2 = (*iter_2)->children.begin();
            // for(unsigned s = 0; s < (*iter_1)->children.size(); s++)
            // {
            //     cout << (*state_1)->level << " " << (*state_1)->abs_column_id << " " << (*state_2)->level << " " << (*state_2)->abs_column_id << endl;
            //     state_1++;
            //     state_2++;
            // }

            // cout << endl;


            state_1 = (*iter_1)->children.begin();
            state_2 = (*iter_2)->children.begin();
            for(unsigned s = 0; s < (*iter_1)->children.size(); s++)
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

bool perturbation(Organization *org, int update_id)
{
    //GENERATOR OF RANDOM NUMBERS
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_real_distribution<double> distribution(0.0, 1.0);
    double x, delta, sum = 0;
    int level, level_id, i;
    bool perturbed = false;

    //Harmonic number
    double H_n = 0;
    for(double i = 2; i < org->all_states.size(); i++)
        H_n = H_n + (1 / i);

    //RANDOM SELECTION OF LEVEL (ROULLETE WHEEL)
    x = distribution(generator) * H_n;
    for(double i = 2; i < org->all_states.size(); i++) {
        sum = sum + (1 / i);
        if( x <= sum ){
            level = i;
            break;
        }
    }

    H_n = 0.0;
    for(State * state : org->all_states[level])
        H_n = H_n + ( 1 / state->overall_reach_prob );

    //RANDOM SELECTION OF LEVEL (ROULLETE WHEEL)
    sum = 0.0;
    i = 0;
    x = distribution(generator) * H_n;
    for(State * state : org->all_states[level]) {
        sum = sum + ( 1 / state->overall_reach_prob );
        if( x <= sum ){
            level_id = i;
            break;
        }
        i++;
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    if( rand() % 2 == 0 ) {
        org->add_parent(level, level_id, update_id);
        perturbed = true;
    } else {
        delta  = org->delete_early_eval(level, level_id);
        // cout << "Delta D: " << delta << endl;
        // if(delta >= 0) {
        //     org->delete_parent(level, level_id, update_id);
        //     perturbed = true;
        // }
        org->delete_parent(level, level_id, update_id);
        perturbed = true;
    }
    return perturbed;
}

Organization* simulated_annealing(Organization *org, int max_iter, float alpha, int K_max, double timeout, float target)
{
    int update_id = 1;
    Organization *best_org = org;
    Organization *new_org = org->copy();
    // compare_orgs(org, new_org);
    float T = org->effectiveness;
    // float T_min = pow(alpha, 10000);
    float delta;
    //GENERATOR OF RANDOM NUMBERS
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_real_distribution<float> distribution(0.0, 1.0);
    int count = 0;

    while( T > 0 && count < K_max ) {
        for(int i = 0; i < max_iter; i++) {
            count++;
            if( perturbation(new_org, update_id) ) {
                delta = org->effectiveness - new_org->effectiveness;
                // std::cout << "Delta SA: " << delta << endl;
                if( delta < 0 || distribution(generator) < exp(-delta/T) ) {
                    org = new_org->copy();
                    update_id = update_id + 2;
                    if( org->effectiveness > best_org->effectiveness ) {
                        best_org = org;
                        count = 0;
                    }
                } else {
                    delete new_org;
                    new_org = org->copy();
                }

                if( new_org->all_states.size() == 2 ) {
                    delete new_org;
                    new_org = best_org->copy();
                }
            }             
            time(&best_org->t_end);
            if( difftime(best_org->t_end, best_org->t_start) >= timeout || best_org->effectiveness >= target)
                return best_org;
        }
        T = alpha * T;
    }

    delete new_org;
    if(org != best_org)
        delete org;

    return best_org;
}

Organization* multistart_sa(Instance *instance, float gamma, int num_restarts, int max_iter, float alpha, int K_max, double timeout, float target) {
    
    time_t t_start;
    time(&t_start);

    Organization *best_org, *new_org;
    Organization *org = Organization::generate_organization_by_clustering(instance, gamma);
    org->t_start = t_start;
    time(&org->t_end);
    best_org = org;
    // best_org = new Organization;
    // best_org->effectiveness = 0;
    // best_org->t_end = t_start;

    // while ( i < num_restarts && difftime(best_org->t_end, t_start) < timeout )
    while ( difftime(best_org->t_end, t_start) < timeout && best_org->effectiveness < target )
    {
        // new_org = Organization::generate_organization_by_clustering(instance, gamma);
        new_org = simulated_annealing(org->copy(), max_iter, alpha, K_max, timeout, target);
        if( new_org->effectiveness > best_org->effectiveness ) {
            best_org = new_org;
            best_org->t_start = t_start;
        } else {
            delete new_org;
        }
        time(&best_org->t_end);

    }
    return best_org;    
}

map<string, string> parseCommandline(int argc, char* argv[]) {
    map<string, string> args;
    for(int i=1; i < argc; i = i + 2)
        args.insert(pair<string, string>(argv[i], argv[i+1]));
    return args;
}

int main(int argc, char* argv[]) {

    // Parses command line arguments.
    auto args = parseCommandline(argc, argv);

    Instance * instance = Instance::read_instance(args["-i"]);
    // instance->print_stats();
    // return 0;
    // instance->num_tags = 0;

    // float gamma = 25.0;
    float gamma = stof(args["--gamma"]);
    double timeout = stod(args["--time"]);
    // int num_restarts = stoi(args["--Kr"]);
    int max_iters = stoi(args["--Ki"]);
    int max_failures = stoi(args["--Kf"]);
    float alpha = stof(args["--alpha"]);

    float target = stof(args["--target"]);

    Organization *org;

    // org = Organization::generate_basic_organization(instance, gamma);
    // cout << org->effectiveness << ", " << org->all_states.size() << ", " << difftime(org->t_end, org->t_start) << "\n";
    // org->success_probabilities();
    // cout << '\n';

    instance->num_tags = 0;
    //org = Organization::generate_organization_by_clustering(instance, gamma);
    //cout << org->effectiveness << ", " << org->all_states.size() << ", " << difftime(org->t_end, org->t_start) << "\n";

    org = multistart_sa(instance, gamma, -1, max_iters, alpha, max_failures, timeout, target); // 500

    // cout << -org->effectiveness << endl; //FOR IRACE CALIBRATION
    // cout << difftime(org->t_end, org->t_start) << endl; // FOR TTPLOT
    cout << org->effectiveness << ", " << org->all_states.size() << ", " << difftime(org->t_end, org->t_start) << "\n";
    
    // org->success_probabilities();

    delete org;
    delete instance;

    return 0;
}