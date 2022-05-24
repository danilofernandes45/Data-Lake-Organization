#include "Organization.hpp"

void Organization::update_effectiveness()
{
    State *leaf;
    int table_id;
    float *tables_discover_probs = new float[this->instance->num_tables];

    for (int i = 0; i < this->instance->num_tables; i++)
        tables_discover_probs[i] = 1.0;

    cout << "Columns Overall Reach Probs: ";
    for (int i = 0; i < this->leaves.size(); i++)
    {
        leaf = this->leaves[i];
        cout << leaf->overall_reach_prob << " (" << leaf->abs_column_id << ") ";
    }

    cout << "\nColumns Discover Probs: ";
    for (int i = 0; i < this->leaves.size(); i++)
    {   
        leaf = this->leaves[i];
        table_id = this->instance->map[leaf->abs_column_id][0];
        tables_discover_probs[table_id] *= ( 1 - leaf->reach_probs[leaf->abs_column_id] );
        cout << leaf->reach_probs[leaf->abs_column_id] << " (" << leaf->abs_column_id << ") ";
    }
    cout << "\nTables Discover Probs: ";
    this->effectiveness = 0.0;
    for (int i = 0; i < this->instance->num_tables; i++){
        this->effectiveness += 1 - tables_discover_probs[i];
        cout << 1 - tables_discover_probs[i] << " ";
    }
    cout << "\n\n";

    this->effectiveness /= this->instance->num_tables; 
}

void Organization::compute_all_reach_probs()
{
    this->root->level = 0;
    this->root->overall_reach_prob = 1.0;
    this->root->reach_probs = new float[this->instance->total_num_columns];
    for (int i = 0; i < this->instance->total_num_columns; i++)
        this->root->reach_probs[i] = 1.0;

    Organization::update_descendants(this->root, this->gamma, this->instance->total_num_columns, 0);
}

//INITIALIZE all_states, CONSIDERING THAT ALL STATES HAVE ONLY ONE PARENT
void Organization::init_all_states()
{
    queue<State*> queue;
    queue.push(this->root);
    State* current;
    int current_level = 0;
    vector<State*> *states_level = new vector<State*>; //STATES ADDED IN THE CURRENT LEVEL
    while( !queue.empty() ){
        //REMOVE FROM QUEUE
        current = queue.front();
        queue.pop();
        if( current->children.size() > 0) {
            //ADD ITS CHILDREN IN THE QUEUE
            for (int i = 0; i < current->children.size(); i++)
                queue.push(current->children[i]);
        } else {
            this->leaves.push_back(current);
        }
        //ADD states_level IN all_states, IF IT'S NECESSARY
        if( current_level < current->level ){

            // for (int i = 0; i < states_level->size(); i++)
            //     cout << (*states_level)[i]->abs_column_id << " ";
            // cout << "\n";

            sort( states_level->begin(), states_level->end(), State::compare );
            this->all_states.push_back(*states_level);
            states_level = new vector<State*>;
            current_level++;
        }
        //ADD CURRENT STATE IN states_level
        states_level->push_back(current);           
    }

    // for (int i = 0; i < states_level->size(); i++)
    //     cout << (*states_level)[i]->abs_column_id << " ";
    // cout << "\n";

    sort( states_level->begin(), states_level->end(), State::compare );
    this->all_states.push_back(*states_level);
      
}

void Organization::update_all_states(int level)
{
    for (int i = level; i < this->all_states.size(); i++)
        sort(this->all_states[i].begin(), this->all_states[i].end(), State::compare);
    
}

Organization* Organization::copy()
{
    Organization *copy = new Organization;
    copy->instance = this->instance;
    copy->gamma = this->gamma;
    copy->effectiveness = this->effectiveness;

    State *copied_state;
    for(int i = 0; i < this->all_states.size(); i++)
    {
        vector<State*> states;
        for (int j = 0; j < this->all_states[i].size(); j++){
            copied_state = this->all_states[i][j]->copy(this->instance->total_num_columns, this->instance->embedding_dim);
            //ADD TRANSITIONS PARENT-CHILD
            //UNDER CONSIDERATION THAT ALL PARENTS ARE IN PREVIOUS LEVEL
            for (int k = 0; k < this->all_states[i][j]->parents.size(); k++)
            {
                for (int l = 0; l < copy->all_states[i-1].size(); l++)
                {
                    if( this->all_states[i][j]->parents[k]->abs_column_id == copy->all_states[i-1][l]->abs_column_id ) {
                        copied_state->parents.push_back( copy->all_states[i-1][l] );
                        copy->all_states[i-1][l]->children.push_back( copied_state );
                        break;
                    }
                }   
            }
            states.push_back( copied_state );
            if( this->all_states[i][j]->children.size() == 0 )
                copy->leaves.push_back(copied_state);
        }
        copy->all_states.push_back(states);
    }
    copy->root = copy->all_states[0][0];

    return copy; 
}

void Organization::delete_parent(int level, int level_id, int update_id)
{
    State *current, *parent;
    queue<State*> outdated_states;
    vector<State*> *new_parents, *grandpa_children;
    vector<State*>::iterator iter;
    for (int i = 0; i < this->all_states[level].size(); i++)
    {
        current = this->all_states[level][i];
        new_parents = new vector<State*>;
        for (int j = 0; j < current->parents.size(); j++)
        {
            parent = current->parents[j];
            for (int k = 0; k < parent->parents.size(); k++)
            {
                if( find(new_parents->begin(), new_parents->end(), parent->parents[k]) == new_parents->end() )
                {
                    grandpa_children = &parent->parents[k]->children;

                    // cout << "Parent " << parent->abs_column_id << "\n";
                    // for (int t = 0; t < grandpa_children->size(); t++)
                    //     cout << (*grandpa_children)[t]->abs_column_id << " ";
                    // cout << "\n";

                    iter = find(grandpa_children->begin(), grandpa_children->end(), parent);
                    if( iter != grandpa_children->end() )
                        grandpa_children->erase(iter);

                    // for (int t = 0; t < grandpa_children->size(); t++)
                    //     cout << (*grandpa_children)[t]->abs_column_id << " ";
                    // cout << "\n";
                    
                    grandpa_children->push_back(current);
                    new_parents->push_back(parent->parents[k]);
                    outdated_states.push(parent->parents[k]);
                }
            }
        }
        current->parents = *new_parents;
    }
    while ( !outdated_states.empty() )
    {
        current = outdated_states.front();
        outdated_states.pop();
        Organization::update_descendants(current, this->gamma, this->instance->total_num_columns, update_id);
    }
    //UPDATE all_states
    // AVOID LOST LEAF STATES
    for(int i=0; i < this->all_states[level-1].size(); i++)
    {
        if( this->all_states[level-1][i]->children.size() == 0 )
            this->all_states[level].push_back( this->all_states[level-1][i] );
    }
    this->all_states.erase(all_states.begin() + level - 1);
    this->update_effectiveness();

    for (int i = level-1; i < this->all_states.size(); i++)
        sort(this->all_states[i].begin(), this->all_states[i].end(), State::compare);
}

void Organization::add_parent(int level, int level_id, int update_id)
{
    State *current = this->all_states[level][level_id];
    vector <State*> *candidates = &this->all_states[level-1];
    int min_level;

    for( int i = candidates->size()-1; i >= 0; i--)
    {
        if( find(current->parents.begin(), current->parents.end(), (*candidates)[i]) ==  current->parents.end() ){
            current->parents.push_back( (*candidates)[i] );
            (*candidates)[i]->children.push_back(current);
            break;
        } 
    }
    min_level = Organization::update_ancestors(current, this->instance, this->gamma, update_id);

    for (int i = min_level; i < this->all_states.size(); i++)
        sort(this->all_states[i].begin(), this->all_states[i].end(), State::compare);

    this->update_effectiveness();
}

//IMPLEMENTATION CONSIDERING THAT ALL PARENTS OF A STATE ARE IN THE SAME LEVEL
//UNDER THIS CONSIDERATION, A BREADTH-FIRST SEARCH CAN UPDATE ALL PARENTS BEFORE ITS CHILDREN
void Organization::update_descendants(State *patriarch, float gamma, int total_num_columns, int update_id)
{    
    queue<State*> queue; //QUEUE USED IN THE BREADTH-FIRST SEARCH
    queue.push(patriarch);
    State *current;

    while( !queue.empty() ){
        //GET THE FIRST STATE FROM THE QUEUE
        current = queue.front();
        queue.pop();
        //VERIFY IF THE CURRENT STATE HAS ALREADY BEEN UPDATED
        if( current->update_id != update_id )
        {
            //COMPUTE ITS REACHABILITY PROBABILITIES
            //UPDATE LEVEL OF CURRENT NODE. OBS.: ALL PARENTS ARE IN THE SAME LEVEL
            if( current != patriarch )
                current->update_reach_probs(gamma, total_num_columns);

            //<TEST!>
            // printf("\nLevel: %d\nID: %d\n", current->level, current->abs_column_id);
            // printf("Reach Probs\n");
            // for (int t = 0; t < total_num_columns; t++)
            //     printf("%.3f ", current->reach_probs[t]);
            
            // printf("\nOverall reach prob: %.3f\n", current->overall_reach_prob);
            // if(current->abs_column_id >= 0)
            //     printf("Discover probability: %.3f\n", current->reach_probs[current->abs_column_id]);
            //</TEST!>

            //ADD ITS CHILDREN TO THE QUEUE
            for(int i=0; i < current->children.size(); i++ )
                queue.push(current->children[i]);

            current->update_id = update_id;
        }
    }
}

int Organization::update_ancestors(State *descendant, Instance *inst, float gamma, int update_id)
{
    queue<State*> outdated_patriarchs; //PATRIARCHS WHOSE DESCENDANTS NEED TO UPDATE REACH_PROBS 
    queue<State*> queue; //QUEUE USED IN THE BREADTH-FIRST SEARCH
    queue.push(descendant);
    State *current;
    int table_id, col_id, min_level;
    int has_changed = 1;
    float *sum_vector_i;

    while( !queue.empty() ){
        //GET THE FIRST STATE FROM THE QUEUE 
        current = queue.front();
        queue.pop();

        if( current != descendant )
        {
            has_changed = 0;
            //CHECK WHICH TOPICS NEED TO BE ADDED IN current's DOMAIN
            for(int i = 0; i < inst->total_num_columns; i++)
            {
                if(descendant->domain[i] == 1 && current->domain[i] == 0)
                {
                    has_changed = 1;
                    current->domain[i] = 1;
                    table_id = inst->map[i][0];
                    col_id = inst->map[i][1];
                    for(int d = 0; d < inst->embedding_dim; d++)
                        current->sum_vector[d] += inst->tables[table_id]->sum_vectors[col_id][d];
                }
            }
        }

        if( has_changed == 1 )
        {
            //UPDATE SIMILARITIES
            for (int i = 0; i < inst->total_num_columns; i++)
            {
                table_id = inst->map[i][0];
                col_id = inst->map[i][1];
                sum_vector_i = inst->tables[table_id]->sum_vectors[col_id];
                current->similarities[i] = cossine_similarity(current->sum_vector, sum_vector_i, inst->embedding_dim);
            }
            
            //ITS PARENTS NEED TO BE VERIFIED
            for(int i = 0; i < current->parents.size(); i++)
                queue.push(current->parents[i]);
        } else {
            //ITS SIBILINGS NEED UPDATE THIER reach_probs
            outdated_patriarchs.push(current);
        }
        
    }
    min_level = outdated_patriarchs.front()->level;
    //I NEED MOVE THIS TO ANOTHER PLACE
    while( !outdated_patriarchs.empty() )
    {
        current = outdated_patriarchs.front();
        outdated_patriarchs.pop();
        min_level = min(min_level, current->level);
        Organization::update_descendants(current, gamma, inst->total_num_columns, update_id);
    }
    return min_level + 1;
}

//GENERATE THE BASELINE ORGANIZATION
Organization* Organization::generate_basic_organization(Instance * inst, float gamma)
{

    //Organizatio setup
    Organization *org = new Organization;
    org->instance = inst;
    org->gamma = gamma;
    //Root node creation
    org->root = new State;
    org->root->update_id = -1;
    org->root->abs_column_id = -inst->total_num_columns;
    org->root->sum_vector = new float[inst->embedding_dim]; // VECTOR INITIALIZED WITH ZEROS
    org->root->sample_size = 0;
    org->root->domain = new int[inst->total_num_columns];
    for (int i = 0; i < inst->total_num_columns; i++)
        org->root->domain[i] = 1;

    State *state;
    int count = 0;

    //Leaf nodes (columns representation) creation
    for (int i = 0; i < inst->num_tables; i++)
    {
        org->root->sample_size += inst->tables[i]->nrows * inst->tables[i]->ncols;

        for (int j = 0; j < inst->tables[i]->ncols; j++)
        {
            state = new State;

            state->abs_column_id = count;
            state->update_id = -1;
            state->domain = new int[inst->total_num_columns];
            state->domain[count] = 1;
            count++;

            state->parents.push_back( org->root );
            state->sum_vector = inst->tables[i]->sum_vectors[j];
            state->sample_size = inst->tables[i]->nrows;
            //root sum vector incrementation 
            for (int d = 0; d < inst->embedding_dim; d++)
                org->root->sum_vector[d] += state->sum_vector[d];
            //Adding the leaf to the organization as child of the root
            org->root->children.push_back(state);
        }   
    }
    org->compute_all_reach_probs();
    org->init_all_states();
    org->update_effectiveness();
    return org;
}


//GENERATE A ORGANIZATION BY HIERARQUICAL CLUSTERING
//ALGORITHM: NEAREST NEIGHBORS CHAIN -> O(N²)
//DISTANCE BETWEEN CLUSTERS: UNWEIGHTED PAIR-GROUP METHOD WITH ARITHMETIC MEAN (UPGMA)
Organization* Organization::generate_organization_by_clustering(Instance * inst, float gamma)
{
    //Organization setup
    Organization *org = new Organization;
    org->instance = inst;
    org->gamma = gamma;

    Cluster* active_clusters = Cluster::init_clusters(inst); // CHAINED LIST OF CLUSTERS AVAILABLE TO BE ADDED TO NN CHAIN
    float** dist_matrix = Cluster::init_dist_matrix(active_clusters, inst->total_num_columns, inst->embedding_dim); // DISTANCE BETWEEN ALL CLUSTERS

    Cluster *stack = active_clusters; //NEAREST NEIGHBORS CHAIN
    active_clusters = active_clusters->next; // REMOVE THE HEAD FROM CHAINED LIST AND ADD TO NN CHAIN

    Cluster *prev_nn, *nn; // PREVIOUS NN CLUSTER AND NN CLUSTER IN CHAINED LIST
    Cluster *previous, *current; // ITERATORS
    Cluster *new_cluster;

    int cluster_id = inst->total_num_columns; // MATRIX ID OF THE NEXT CLUSTER THAT WILL BE CREATED
    float diff_dists;

    //WHILE THERE ARE CLUSTERS TO MERGE 
    while(stack != NULL)
    {
        // printf("Iteration\n\n");
        // printf("%d ", stack->id);
        prev_nn = NULL;
        nn = active_clusters;
        previous = active_clusters;
        current = active_clusters->next;

        //FIND NEAREST NEIGHBOR TO TOP STACK CLUSTER
        while(current != NULL)
        {
            diff_dists = dist_matrix[stack->id][current->id] - dist_matrix[stack->id][nn->id];
            if( diff_dists < 0 || ( diff_dists == 0 && current->id < nn->id ) )
            {
                prev_nn = previous;
                nn = current;
            }
            previous = current;
            current = current->next;
        }

        //CHECK IF A PAIR OF RNN WERE FOUND
        diff_dists = 1.0;
        if( stack->is_NN_of != NULL ){
            diff_dists = dist_matrix[stack->id][stack->is_NN_of->id] - dist_matrix[stack->id][nn->id];
        }
        if( diff_dists < 0 || ( diff_dists == 0 && stack->is_NN_of->id < nn->id ) )
        {
            // printf("%d\n", stack->is_NN_of->id);
            //MERGE THE RNN INTO A NEW CLUSTER
            new_cluster = Cluster::merge_clusters(stack, dist_matrix, cluster_id, inst);
            //ADD THE NEW CLUSTER INTO UNMERGED CLUSTER LIST
            new_cluster->next = active_clusters;
            active_clusters = new_cluster;
            cluster_id++;

            //REMOVE THE RNN FROM NN CHAIN
            stack = stack->is_NN_of->is_NN_of;
            if(stack == NULL && active_clusters->next != NULL){
                stack = active_clusters;
                active_clusters = active_clusters->next;
            }
        } else {
            //REMOVE NN FROM THE CHAINED LIST
            if(prev_nn == NULL)
                active_clusters = nn->next;
            else
                prev_nn->next = nn->next;
            
            //ADD NN TO THE NN CHAIN
            // printf("%d\n", nn->id);
            nn->is_NN_of = stack;
            stack = nn;

            if( active_clusters == NULL ) {
                active_clusters = Cluster::merge_clusters(stack, dist_matrix, cluster_id, inst);
                cluster_id++;
                stack = stack->is_NN_of->is_NN_of;
            }
        }
        //Test!
        current = stack;
        // printf("Stack: ");
        // while(current != NULL){
        //     printf("%d ", current->id);
        //     current = current->is_NN_of;
        // }
        // printf("\n");

        // current = active_clusters;
        // printf("Active Clusters: ");
        // while(current != NULL){
        //     printf("%d ", current->id);
        //     current = current->next;
        // }vector<State*> states_level;
        // printf("\n\n");
    }

    org->root = active_clusters->state;
    org->compute_all_reach_probs();
    org->init_all_states();
    org->update_effectiveness();
    return org;
}
