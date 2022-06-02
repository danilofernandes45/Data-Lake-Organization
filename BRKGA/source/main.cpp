#include <iostream>
#include "Organization.hpp"

using namespace std;

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
    cout << "\n";
}

int main(int argc, char* argv[]) {

	// const unsigned n = 12;		// size of chromosomes
	const unsigned p = 10;		// size of population
	const double pe = 0.20;		// fraction of population to be the elite-set
	const double pm = 0.10;		// fraction of population to be replaced by mutants
	const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
	const unsigned K = 1;		// number of independent populations
	const unsigned MAXT = 1;	// number of threads for parallel decoding
	float gamma = 1.0;

    time_t start, end;

	Instance *instance = Instance::read_instance();

    time(&start);
	Organization *org = Organization::build_organization_BRKGA(instance, gamma, p, pe, pm, rhoe, K, MAXT);
    time(&end);

    cout << org->effectiveness << ", " << difftime(end, start) << "\n";

	// print_organization(org);

	return 0;
}