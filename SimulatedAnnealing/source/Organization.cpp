#include "Organization.hpp"

// template<typename T>
// bool CompareProb::operator()(const T *state_1, const T *state_2) {
//     if( state_1->overall_reach_prob == state_2->overall_reach_prob )
//         return state_1 < state_2;
//     return state_1->overall_reach_prob < state_2->overall_reach_prob; 
// }

// template<typename T>
// bool CompareID::operator()(const T *state_1, const T *state_2) {
//     return state_1->abs_column_id < state_2->abs_column_id; 
// }

//TOPOLOGICAL SORT - APPROACH BASED ON DFS
void topological_sort(vector<State*> * ancestors, vector<State*> * stack, int update_id)
{
    State *current;
    vector<State*> path;
    path.push_back( ancestors->back() );
    ancestors->pop_back();

    while( !path.empty() ) {
        //GET THE TOP STATE FROM THE STACK
        current = path.back();
        //VERIFY IF THE CURRENT STATE HAS ALREADY BEEN VISITED
        if( current->update_id == update_id ) {
            path.pop_back();
            stack->push_back(current);
        } else {
            //ADD ITS CHILDREN TO THE STACK
            for( State * child : current->children ) {
                if( child->update_id != update_id )
                    path.push_back(child);
            }
            current->update_id = update_id;
        }
        //ADD AN UNVISITED STATE INTO THE PATH WHEN IT IS EMPTY
        while( path.empty() && !ancestors->empty() ) {
            current = ancestors->back();
            ancestors->pop_back();
            if( current->update_id != update_id ) 
                path.push_back( current );
        }
    }
}

void Organization::success_probabilities()
{
    State *leaf, *sim_leaf;
    int table_id;
    float column_success;
    vector<double> tables_success_probs;

    sort(this->leaves.begin(), this->leaves.end(), State::compare_id);

    for (int i = 0; i < this->instance->num_tables; i++)
        tables_success_probs.push_back(1.0);

    for (int i = 0; i < this->leaves.size(); i++)
    {   
        leaf = this->leaves[i];
        column_success = 1;

        for(int j = 0; j < this->leaves.size(); j++)
        {
            if(leaf->similarities[j] >= 0.9) {
                sim_leaf = this->leaves[j];
                column_success *= ( 1 - sim_leaf->reach_probs[j] );
            }
        }

        table_id = this->instance->map[leaf->abs_column_id][0];
        tables_success_probs[table_id] *= column_success;
    }

    for (int i = 0; i < this->instance->num_tables; i++){
        tables_success_probs[i] = 1 - tables_success_probs[i];
    }

    sort(tables_success_probs.begin(), tables_success_probs.end());

    for (int i = 0; i < this->instance->num_tables; i++){
        cout << tables_success_probs[i] << " ";
    }
    cout << endl;
    
}

void Organization::update_effectiveness()
{
    State *leaf;
    int table_id;
    float *tables_discover_probs = new float[this->instance->num_tables];

    for (int i = 0; i < this->instance->num_tables; i++)
        tables_discover_probs[i] = 1.0;

    #if DEBUG
    cout << "Columns Overall Reach Probs: ";
    for (int i = 0; i < this->leaves.size(); i++)
    {
        leaf = this->leaves[i];
        cout << leaf->overall_reach_prob << " (" << leaf->abs_column_id << ") ";
    }

    cout << "\nColumns Discover Probs: ";
    #endif

    for (int i = 0; i < this->leaves.size(); i++)
    {   
        leaf = this->leaves[i];
        table_id = this->instance->map[leaf->abs_column_id][0];
        tables_discover_probs[table_id] *= ( 1 - leaf->reach_probs[leaf->abs_column_id] );

        #if DEBUG
        cout << leaf->reach_probs[leaf->abs_column_id] << " (" << leaf->abs_column_id << ") ";
        #endif
    }
    #if DEBUG
    cout << "\nTables Discover Probs: ";
    #endif

    this->effectiveness = 0.0;
    for (int i = 0; i < this->instance->num_tables; i++){
        this->effectiveness += 1 - tables_discover_probs[i];

        #if DEBUG
        cout << 1 - tables_discover_probs[i] << " ";
        #endif
    }
    #if DEBUG
    cout << "\n\n";
    #endif

    this->effectiveness /= this->instance->num_tables; 
}

//INITIALIZE all_states, CONSIDERING THAT ALL STATES HAVE ONLY ONE PARENT
void Organization::init_all_states()
{
    State * current;
    //INITIALIZE SOURCE NODE PROPERTIES
    this->root->level = 0;
    this->root->overall_reach_prob = 1.0;
    this->root->reach_probs = new float[this->instance->total_num_columns];
    for (int i = 0; i < this->instance->total_num_columns; i++)
        this->root->reach_probs[i] = 1.0;

    vector<State*> ancestors;
    ancestors.push_back(this->root);
    vector<State*> stack;
    topological_sort(&ancestors, &stack, 0);
    
    //UPDATE THE STATES ORDERED BY TOPOLOGICAL SORT
    while ( !stack.empty() ) 
    {   
        current = stack.back();
        stack.pop_back();
        //COMPUTE ITS REACHABILITY PROBABILITIES AND UPDATE LEVEL OF CURRENT NODE. OBS.: ALL PARENTS ARE ALREADY UPDATED
        current->update_level();
        current->update_reach_probs(this->gamma, this->instance->total_num_columns);
        while( this->all_states.size() <= current->level )
            this->all_states.push_back(*(new set<State*, CompareID>));
        //UPDATING all_states WHEN current CHANGES ITS LEVEL OR ITS REACHABILITY, THIS ENSURES THE ORDER INTO BINARY TREE
        this->all_states[current->level].insert(current); //O(log N)

        if( current->children.empty() )
            this->leaves.push_back(current);
    }
}

Organization* Organization::copy()
{
    Organization *copy = new Organization;
    copy->instance = this->instance;
    copy->gamma = this->gamma;
    copy->effectiveness = this->effectiveness;
    copy->max_num_states = this->max_num_states;
    copy->t_start = this->t_start;

    set<State*>::iterator iter, iter_copy;
    State *copied_state;
    for(int i = 0; i < this->all_states.size(); i++)
    {
        set<State*, CompareID> states;
        for (State * state : this->all_states[i] ){
            copied_state = state->copy(this->instance->total_num_columns, this->max_num_states, this->instance->embedding_dim);
            states.insert( copied_state );
            if( state->children.empty() )
                copy->leaves.push_back(copied_state);
        }
        copy->all_states.push_back(states);
    }
    //ADD PARENT RELATIONSHIP
    for(int i = 1; i < this->all_states.size(); i++) {
        iter = this->all_states[i].begin();
        iter_copy = copy->all_states[i].begin();
        for(int j = 0; j < this->all_states[i].size(); j++) {
            for(State * parent : (*iter)->parents ) {
                for(State * state : copy->all_states[parent->level] ) {
                    if( parent->abs_column_id == state->abs_column_id ) {
                        (*iter_copy)->parents.insert(state);
                        state->children.insert(*iter_copy);
                        break;
                    }
                }
            }
            iter++;
            iter_copy++;
        }        
    }
    copy->root = *(copy->all_states[0].begin());

    return copy; 
}

void Organization::delete_parent(int level, int level_id, int update_id) 
{
    set<State*> deleted_states; // Binary tree
    vector<State*> grandpas;
    State * current = *next(this->all_states[level].begin(), level_id); //GET ELEMENT AT POSITION level_id
    State * deleted_parent = *(current->parents.begin());
    set<State*>::iterator iter;
    //FIND THE LEAST REACHABLE PARENT
    for( iter = next(current->parents.begin()); iter != current->parents.end(); iter++){
        if( (*iter)->overall_reach_prob < deleted_parent->overall_reach_prob )
            deleted_parent = *iter;
    }
    //FIND THE deleted_parent's SIBILINGS
    for( State * grandpa : deleted_parent->parents ) {
        for( State * sibiling : grandpa->children ) {
            if( sibiling == deleted_parent || !sibiling->is_tag && sibiling->children.size() > 0 )
                    deleted_states.insert(sibiling); // O(log N)
        }
    }
    //IF deleted_parent HAS A TAG, THEN ITS GRANDPAS WILL HAVE IT TOO
    if( deleted_parent->is_tag ) {
        for( State * grandpa : deleted_parent->parents )
            grandpa->is_tag = true;
    }

    //REMOVE THE PARENTSHIP FROM THE GRANDFATHER
    //ADD THE GRANDFATHERS INTO THE HEAP (THEIR DESCENT WILL BE UPDATED)
    for( State * parent : deleted_states ){
        for( State * grandpa : parent->parents ) {
            grandpa->children.erase(parent); // O(log N)
            grandpas.push_back(grandpa); // O(1)
        }
    }
    //TRANSFER THE PARENTSHIP TO THE GRANDFATHER
    for( State * parent : deleted_states ){
        for( State * child : parent->children ){
            child->parents.erase(parent); // O(log N)
            for( State * grandpa : parent->parents ){
                child->parents.insert(grandpa); // O(log N)
                grandpa->children.insert(child); // O(log N)
            }
        }
    }

    //REMOVE deleted_parents FROM THE ORGANIZATION
    for( State * parent : deleted_states )
        this->all_states[parent->level].erase(parent); // O(log N)

    //UPDATE DESCEDANTS
    this->update_descendants(&grandpas, update_id);
    this->update_effectiveness();
}

void Organization::delete_level(int level, int update_id) 
{
    vector<State*> grandpas;
    set<State*, CompareID> deleted_level = this->all_states[level];

    //IF deleted_parent HAS A TAG, THEN ITS GRANDPAS WILL HAVE IT TOO
    for( State * deleted_parent : this->all_states[level] ) {
        if( deleted_parent->children.size() > 0 ) {
            for( State * grandpa : deleted_parent->parents )
                grandpa->is_tag = deleted_parent->is_tag;
        }
    }

    //REMOVE THE PARENTSHIP FROM THE GRANDFATHER
    //ADD THE GRANDFATHERS INTO THE HEAP (THEIR DESCENT WILL BE UPDATED)
    for( State * parent : this->all_states[level] ){
        if( parent->children.size() > 0 ) {
            for( State * grandpa : parent->parents ) {
                grandpa->children.erase(parent); // O(log N)
                grandpas.push_back(grandpa); // O(1)
            }
        }
    }
    //TRANSFER THE PARENTSHIP TO THE GRANDFATHER
    for( State * parent : this->all_states[level] ){
        if( parent->children.size() > 0 ){
            for( State * child : parent->children ){
                child->parents.erase(parent); // O(log N)
                for( State * grandpa : parent->parents ){
                    child->parents.insert(grandpa); // O(log N)
                    grandpa->children.insert(child); // O(log N)
                }
            }
        }
    }
    //REMOVE deleted_parents FROM THE ORGANIZATION
    for( State * parent : deleted_level ) {
        if( parent->children.size() > 0 ) {
            this->all_states[parent->level].erase(parent); // O(log N)
        }
    }
    //UPDATE DESCEDANTS
    if( !grandpas.empty() ) {
        this->update_descendants(&grandpas, update_id);
        this->update_effectiveness();
    }
}

void Organization::add_parent(int level, int level_id, int update_id)
{
    State *current = *next(this->all_states[level].begin(), level_id);
    set<State*, CompareID> * candidates = &this->all_states[level-1];
    // set<State*>::iterator iter = candidates->end();
    State *best_candidate = NULL;
    int id;
    //FIND THE BEST CANDIDATE WHO IS NEITHER PARENT NOR DESCEDANT
    // for( int i = candidates->size()-1; i >= 0; i--) {
    //     iter--;
    //     id = abs( (*iter)->abs_column_id );
    //     if( (*iter)->children.size() > 0 && !current->reachable_states[id] && current->parents.find(*iter) == current->parents.end() ) {
    //         if(best_candidate == NULL || best_candidate->overall_reach_prob < (*iter)->overall_reach_prob)
    //             best_candidate = *iter;
    //     } 
    // }

    for( State * state : *candidates) {
        id = abs( state->abs_column_id );
        if( state->children.size() > 0 && !current->reachable_states[id] && current->parents.find(state) == current->parents.end() ) {
            if(best_candidate == NULL || best_candidate->overall_reach_prob < state->overall_reach_prob)
                best_candidate = state;
        } 
    }

    if( best_candidate != NULL ){
        current->parents.insert( best_candidate ); // O(log N)
        best_candidate->children.insert(current); // O(log N)
        //UPDATE DOMAINS AND PROBABILITIES
        this->update_reachable_states(current, best_candidate);
        best_candidate->compute_similarities(this->instance);
        this->update_ancestors(best_candidate, update_id);
        this->update_effectiveness();
    }
}

//IMPLEMENTATION CONSIDERING THE PARENTS OF A STATE ARE IN DIFFERENT LEVELS
void Organization::update_descendants(vector<State*> * ancestors, int update_id)
{
    int old_level;
    State *current;
    vector<State*> stack;
    topological_sort(ancestors, &stack, update_id);
    //UPDATE THE STATES ORDERED BY TOPOLOGICAL SORT
    while ( !stack.empty() ) 
    {   
        current = stack.back();
        stack.pop_back();
        old_level = current->level;
        //COMPUTE ITS REACHABILITY PROBABILITIES AND UPDATE LEVEL OF CURRENT NODE. OBS.: ALL PARENTS ARE ALREADY UPDATED
        current->update_level();
        current->update_reach_probs(this->gamma, this->instance->total_num_columns);
        if( old_level != current->level ){
            //UPDATING all_states WHEN current CHANGES ITS LEVEL OR ITS REACHABILITY, THIS ENSURES THE ORDER INTO BINARY TREE
            this->all_states[old_level].erase(current); //O(log N)
            //UPDATING all_states WHEN current CHANGES ITS LEVEL OR ITS REACHABILITY, THIS ENSURES THE ORDER INTO BINARY TREE
            this->all_states[current->level].insert(current); //O(log N)
        }
    }

    //REMOVE EMPTY LEVELS FROM all_states
    while( this->all_states.back().empty() ){
        this->all_states.pop_back(); // O(1)
    }
}

int Organization::update_reachable_states(State * descendant, State * current)
{
    bool has_changed = 0;
    int table_id, col_id, id;
    float *sum_vector_i;
    for(int i = 0; i < this->instance->total_num_columns; i++)
        {
            if( descendant->reachable_states[i] && !current->reachable_states[i] )
            {
                has_changed = 1;
                current->reachable_states[i] = 1;
                table_id = this->instance->map[i][0];
                col_id = this->instance->map[i][1];
                sum_vector_i = this->instance->tables[table_id]->sum_vectors[col_id];
                for(int d = 0; d < this->instance->embedding_dim; d++)
                    current->sum_vector[d] += sum_vector_i[d];
            }
        }
        //UPDATE THE REACHABILITY FOR THE INTERNAL STATES
        id = this->instance->total_num_columns;
        while( id < this->max_num_states ) {
            current->reachable_states[id] = current->reachable_states[id] | descendant->reachable_states[id];
            id++;
        }
    return has_changed;
}

void Organization::update_ancestors(State *descendant, int update_id)
{
    //PATRIARCHS WHOSE DESCENDANTS NEED TO UPDATE REACH_PROBS
    vector<State*> ancestors;  
    //ITERATORS
    State *current;
    int table_id, col_id, id;
    bool has_changed;
    float *sum_vector_i;
    //QUEUE USED IN THE BREADTH-FIRST SEARCH. THE DOMAIN UPDATING DON'T DEPENDS ON THE GRAPH NAVIGATION ORDER
    queue<State*> queue, reach_queue;
    for(State * parent : descendant->parents)
        queue.push(parent);

    //UPDATING ANCESTORS' DOMAIN (UPWARD)
    while( !queue.empty() ){
        //GET THE FIRST UNVISITED STATE FROM THE QUEUE 
        current = queue.front();
        queue.pop();
        //CHECK WHICH TOPICS NEED TO BE ADDED IN current's DOMAIN
        //UPDATE THE REACHABILITY FOR THE SINK STATES
        has_changed = this->update_reachable_states(descendant, current);
        current->update_id = update_id;

        if( has_changed ) {
            //UPDATE SIMILARITIES IF CHANGES current'S DOMAIN
            current->compute_similarities(this->instance);
            //ITS PARENTS NEED TO BE VERIFIED
            for(State * parent : current->parents) {
                if( parent->update_id != update_id )
                    queue.push(parent);
            }
        } else {
            //ITS DESCENDANTS NEED UPDATE THIER reach_probs
            ancestors.push_back(current);
            //ITS PARENTS MUST UPDATE THEIR reachable_states
            for(State * parent : current->parents) {
                if( parent->update_id != update_id ) {
                    reach_queue.push(parent);
                }
            }
        }  
    }
    //UPDATE REMAINING ANCESTORS'reachable_states (UPWARD)
    while( !reach_queue.empty() ) {
        current = reach_queue.front();
        reach_queue.pop();
        has_changed = 0;
        current->update_id = update_id;
        id = this->instance->total_num_columns;
        while( id < this->max_num_states ) {
            if(descendant->reachable_states[id] && !current->reachable_states[id] ) {
                has_changed = 1;
                current->reachable_states[id] = 1;
            }
            id++;
        }
        if( has_changed ) {
            for(State * parent : current->parents) {
                if( parent->update_id != update_id ) {
                    reach_queue.push(parent);
                }
            }
        }
    }
    //UPDATE PROBABILITIES ON AFFECTED SUBGRAPHS WITH outdated_states AS PATRIARCHS (DOWNWARD)
    this->update_descendants(&ancestors, update_id + 1);
}

//GENERATE THE BASELINE ORGANIZATION
Organization* Organization::generate_basic_organization(Instance * inst, float gamma)
{
    vector<State*> tags;
    int id = 0;
    int tag_id;
    State *state;
    
    //Organization setup
    Organization *org = new Organization;
    org->instance = inst;
    org->gamma = gamma;
    org->root = State::build(inst, -inst->total_num_columns - inst->num_tags, -1, -1);

    if( inst->num_tags > 0 )
        org->max_num_states = inst->total_num_columns + 2*inst->num_tags - 1;
    else
        org->max_num_states = 2*inst->total_num_columns - 1;

    for (int i = 0; i < org->max_num_states; i++)
        org->root->reachable_states[i] = 1;

    //Create non-leaf nodes with tags, when they exists
    for (int i = 0; i < inst->num_tags; i++)
    {
        state = State::build(inst, - inst->total_num_columns - i, -1, -1);
        state->is_tag = true;
        State::add_parenthood(org->root, state, org->instance->embedding_dim);
        tags.push_back(state);
    }
     
    //Leaf nodes (columns representation) creation
    for (int i = 0; i < inst->num_tables; i++)
    {
        for (int j = 0; j < inst->tables[i]->ncols; j++)
        {
            state = State::build(inst, id, i, j);
            state->compute_similarities(inst);
            if ( tags.empty() )
            {
                State::add_parenthood(org->root, state, org->instance->embedding_dim);
            } else {
                //IF THE TAGS ARE RELATE TO THE TABLE
                for (int k = 0; k < inst->tables[i]->tags_table.size(); k++)
                {
                    tag_id = inst->tables[i]->tags_table[k];
                    State::add_parenthood(tags[tag_id], state, inst->embedding_dim);
                }
                //IF THE TAGS ARE RELATED TO THE COLUMNS
                for (int k = 0; k < inst->tables[i]->tags_cols[j].size(); k++)
                {
                    tag_id = inst->tables[i]->tags_cols[j][k];
                    State::add_parenthood(tags[tag_id], state, inst->embedding_dim);
                }
            }
            id++;
        }   
    }
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

    if( inst->num_tags > 0 )
        org->max_num_states = inst->total_num_columns + 2*inst->num_tags - 1;
    else
        org->max_num_states = 2*inst->total_num_columns - 1;

    Cluster* active_clusters = Cluster::init_clusters(inst); // CHAINED LIST OF CLUSTERS AVAILABLE TO BE ADDED TO NN CHAIN
    int num_clusters = inst->num_tags > 0 ? inst->num_tags : inst->total_num_columns; // NUMBER OF INITIAL CLUSTERS
    float** dist_matrix = Cluster::init_dist_matrix(active_clusters, num_clusters, inst->embedding_dim); // DISTANCE BETWEEN ALL CLUSTERS

    Cluster *stack = active_clusters; //NEAREST NEIGHBORS CHAIN
    active_clusters = active_clusters->next; // REMOVE THE HEAD FROM CHAINED LIST AND ADD TO NN CHAIN

    Cluster *prev_nn, *nn; // PREVIOUS NN CLUSTER AND NN CLUSTER IN CHAINED LIST
    Cluster *previous, *current; // ITERATORS
    Cluster *new_cluster;

    int cluster_id = num_clusters; // MATRIX ID OF THE NEXT CLUSTER THAT WILL BE CREATED
    int state_id = - inst->total_num_columns - inst->num_tags;
    float diff_dists;

    //GENERATOR OF RANDOM NUMBERS
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_real_distribution<double> distribution(0.0, 1.0);

    //WHILE THERE ARE CLUSTERS TO MERGE 
    while(stack != NULL)
    {
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

        //WITHOUT STOCASTIC FACTOR
        if( stack->is_NN_of != NULL ){
            diff_dists = dist_matrix[stack->id][stack->is_NN_of->id] - dist_matrix[stack->id][nn->id];
        }
        //WITH STOCASTIC FACTOR
        // if( stack->is_NN_of != NULL ){
        //     if( distribution(generator) < pow( 1 - 0.5 * dist_matrix[stack->id][stack->is_NN_of->id], 10 ) )
        //         diff_dists = -1.0;
        //     else
        //         diff_dists = dist_matrix[stack->id][stack->is_NN_of->id] - dist_matrix[stack->id][nn->id];
        // }

        if( diff_dists < 0 || ( diff_dists == 0 && stack->is_NN_of->id < nn->id ) )
        {
            // printf("%d\n", stack->is_NN_of->id);
            //MERGE THE RNN INTO A NEW CLUSTER
            new_cluster = Cluster::merge_clusters(stack, dist_matrix, cluster_id, state_id, inst);
            //ADD THE NEW CLUSTER INTO UNMERGED CLUSTER LIST
            new_cluster->next = active_clusters;
            active_clusters = new_cluster;
            cluster_id++;
            state_id--;

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
            nn->is_NN_of = stack;
            stack = nn;
            if( active_clusters == NULL ) {
                active_clusters = Cluster::merge_clusters(stack, dist_matrix, cluster_id, state_id, inst);
                cluster_id++;
                state_id--;
                stack = stack->is_NN_of->is_NN_of;
            }
        }
        //Test!
        // current = stack;
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
    org->init_all_states();
    org->update_effectiveness();
    return org;
}
