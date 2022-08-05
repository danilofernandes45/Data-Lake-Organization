#include "Organization.hpp"

void Organization::destroy()
{
    this->instance = NULL;

    for(int i = 0; i < this->all_states.size(); i++)
    {
        for(int j = 0; j < this->all_states[i].size(); j++)
            this->all_states[i][j]->destroy();
        vector<State*>().swap(this->all_states[i]);
    } 
    vector< vector<State*> >().swap(this->all_states);  
    delete this;
}

void Organization::update_effectiveness()
{
    int top_id;
    float prob_table, prob_column;

    this->effectiveness = 0.0;

    for(int t = 0; t < this->instance->num_tables; t++)
    {
        prob_table = 1.0;
        for(int c = 0; c < this->instance->tables[t]->ncols; c++)
        {
            prob_column = 1.0;
            for(int i = 0; i < this->instance->num_topics_per_column; i++)
            {
                top_id = this->instance->tables[t]->topic_ids[c][i];
                prob_column *= ( 1 - this->leaves[top_id]->reach_probs[top_id] );
            }
            prob_table *= prob_column;
        }
        this->effectiveness += ( 1 - prob_table );
    }

    this->effectiveness /= this->instance->num_tables; 
}

void Organization::compute_all_reach_probs()
{
    this->root->level = 0;
    this->root->overall_reach_prob = 1.0;
    this->root->reach_probs = new double[this->instance->total_num_topics];
    for (int i = 0; i < this->instance->total_num_topics; i++)
        this->root->reach_probs[i] = 1.0;

    Organization::update_descendants(this->root, this->gamma, this->instance->total_num_topics, 0);
}

//INITIALIZE all_states, CONSIDERING THAT ALL STATES HAVE ONLY ONE PARENT
void Organization::init_all_states()
{
    this->leaves = vector<State*>(this->instance->total_num_topics);
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
            this->leaves[current->topic_id] = current;
        }
        //ADD states_level IN all_states, IF IT'S NECESSARY
        if( current_level < current->level ){

            // for (int i = 0; i < states_level->size(); i++)
            //     cout << (*states_level)[i]->topic_id << " ";
            // cout << "\n";

            //Unnecessary in a local search
            //sort( states_level->begin(), states_level->end(), State::compare );
            this->all_states.push_back(*states_level);
            states_level = new vector<State*>;
            current_level++;
        }
        //ADD CURRENT STATE IN states_level
        states_level->push_back(current);           
    }

    // for (int i = 0; i < states_level->size(); i++)
    //     cout << (*states_level)[i]->topic_id << " ";
    // cout << "\n";

    //Unnecessary in a local search
    // sort( states_level->begin(), states_level->end(), State::compare );
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
    copy->leaves = vector<State*>(this->instance->total_num_topics);

    State *copied_state;
    for(int i = 0; i < this->all_states.size(); i++)
    {
        vector<State*> states;
        for (int j = 0; j < this->all_states[i].size(); j++){
            copied_state = this->all_states[i][j]->copy(this->instance->total_num_topics, this->instance->embedding_dim);
            //ADD TRANSITIONS PARENT-CHILD
            //UNDER CONSIDERATION THAT ALL PARENTS ARE IN PREVIOUS LEVEL
            for (int k = 0; k < this->all_states[i][j]->parents.size(); k++)
            {
                for (int l = 0; l < copy->all_states[i-1].size(); l++)
                {
                    if( this->all_states[i][j]->parents[k]->topic_id == copy->all_states[i-1][l]->topic_id ) {
                        copied_state->parents.push_back( copy->all_states[i-1][l] );
                        copy->all_states[i-1][l]->children.push_back( copied_state );
                        break;
                    }
                }   
            }
            states.push_back( copied_state );
            if( this->all_states[i][j]->children.size() == 0 )
                copy->leaves[copied_state->topic_id] = copied_state;
        }
        copy->all_states.push_back(states);
    }
    copy->root = copy->all_states[0][0];

    return copy; 
}

vector<State*> Organization::delete_parent(int level, int level_id, int update_id)
{
    State *current, *parent;
    queue<State*> outdated_states;
    vector<State*> *new_parents, *grandpa_children, deleted_states;
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

                    // cout << "Parent " << parent->topic_id << "\n";
                    // for (int t = 0; t < grandpa_children->size(); t++)
                    //     cout << (*grandpa_children)[t]->topic_id << " ";
                    // cout << "\n";

                    iter = find(grandpa_children->begin(), grandpa_children->end(), parent);
                    if( iter != grandpa_children->end() )
                        grandpa_children->erase(iter);

                    // for (int t = 0; t < grandpa_children->size(); t++)
                    //     cout << (*grandpa_children)[t]->topic_id << " ";
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
        Organization::update_descendants(current, this->gamma, this->instance->total_num_topics, update_id);
    }
    //UPDATE all_states
    // AVOID LOST LEAF STATES
    for(int i=0; i < this->all_states[level-1].size(); i++)
    {
        if( this->all_states[level-1][i]->children.size() == 0 )
            this->all_states[level].push_back( this->all_states[level-1][i] );
    }

    deleted_states = this->all_states[level-1];
    this->all_states.erase(this->all_states.begin() + level - 1);
    this->update_effectiveness();

    // Unnecessary in a local search
    // for (int i = level-1; i < this->all_states.size(); i++)
    //     sort(this->all_states[i].begin(), this->all_states[i].end(), State::compare);
    
    this->last_modification = new Modification;
    this->last_modification->operation = 1;
    this->last_modification->level = level;
    this->last_modification->level_id = level_id;
    this->last_modification->deleted_states = deleted_states;

    return deleted_states;
}

//IT CONSIDERS THAT THE LAST PERFORMED OPERATION WAS A delete_parent IN THE NODES LOCATED IN level
void Organization::undo_delete_parent(Organization* prev_org, vector<State*> deleted_states, int level)
{
    State *parent, *child, *current, *previous;
    int *map, max_id;

    //REMOVE THE LEAF NODES INSERTED DURING delete_parent
    //THESE NODES ARE FROM THE REMOVED LEVEL
    this->all_states[level-1].resize( prev_org->all_states[level].size() );

    //REMOVE ALL PARENTHOOD RELATIONSHIP BETWEEN level-1 and level-2
    for(int i = 0; i < this->all_states[level-2].size(); i++)
        this->all_states[level-2][i]->children.clear();
    
    for(int i = 0; i < this->all_states[level-1].size(); i++)
        this->all_states[level-1][i]->parents.clear();

    //ADD BACK deleted_states IN ITS PREVIOUS POSITION IN all_states
    this->all_states.insert(this->all_states.begin() + level-1, deleted_states);
    //GENERATE MAP: topic_id -> id_{deleted_states}
    //THE GOAL IS TO ENSURE THE SAME ORDER IN BETWEEN this->all_states[i][j]->parents AND prev_org->all_states[i][j]->parents
    //THE ANALOGOUS IS CONSIDERED FOR all_states[i][j]->children
    max_id = 0;
    for (int i = 0; i < deleted_states.size(); i++)
        max_id = max( max_id, abs( deleted_states[i]->topic_id ) );

    map = new int[ 2 * (max_id+1) ];
    for (int i = 0; i < deleted_states.size(); i++) {
        max_id = deleted_states[i]->topic_id;
        if( max_id < 0 )
            max_id = 2 * abs(max_id);
        else
            max_id = 2 * abs(max_id) + 1;
        map[max_id] = i;
    }
    
    //ADD PARENTHOOD RELATIONSHIPS BETWEEN deleted_states AND ITS PREVIOUS PARENTS
    for (int i = 0; i < prev_org->all_states[level-2].size(); i++)
    {
        parent = prev_org->all_states[level-2][i];
        for (int j = 0; j < parent->children.size(); j++)
        {
            max_id = parent->children[j]->topic_id;
            if( max_id < 0 )
                max_id = 2 * abs(max_id);
            else
                max_id = 2 * abs(max_id) + 1;

            current = deleted_states[ map[max_id] ];
            this->all_states[level-2][i]->children.push_back(current);
        }
    }
    //ADD PARENTHOOD RELATIONSHIPS BETWEEN deleted_states AND ITS PREVIOUS CHILDREN
    for (int i = 0; i < prev_org->all_states[level].size(); i++)
    {
        child = prev_org->all_states[level][i];
        for (int j = 0; j < child->parents.size(); j++)
        {
            max_id = child->parents[j]->topic_id;
            if( max_id < 0 )
                max_id = 2 * abs(max_id);
            else
                max_id = 2 * abs(max_id) + 1;

            current = deleted_states[ map[max_id] ];
            this->all_states[level][i]->parents.push_back(current);
        }
    }

    //RECOVER PREVIOUS STATES' PROBABILITIES
    for(int i = level-1; i < this->all_states.size(); i++)
    {
        for(int j = 0; j < this->all_states[i].size(); j++)
        {
            previous = prev_org->all_states[i][j];
            current = this->all_states[i][j];
            if( previous->update_id != current->update_id )
            {
                current->update_id = previous->update_id;
                current->level = previous->level;
                current->overall_reach_prob = previous->overall_reach_prob;
                for(int r = 0; r < this->instance->total_num_topics; r++)
                    current->reach_probs[r] = previous->reach_probs[r];
            }
        }
    }
    this->effectiveness = prev_org->effectiveness;
}

int Organization::add_parent(int level, int level_id, int update_id)
{
    State *current = this->all_states[level][level_id];
    vector <State*> *candidates = &this->all_states[level-1];
    State *best_candidate = NULL;
    int min_level = -1;

    for( int i = candidates->size()-1; i >= 0; i--)
    {
        if( find(current->parents.begin(), current->parents.end(), (*candidates)[i]) ==  current->parents.end() ){
            if( best_candidate == NULL )
                best_candidate = (*candidates)[i];
            else if ( (*candidates)[i]->overall_reach_prob > best_candidate->overall_reach_prob )
                best_candidate = (*candidates)[i];
        } 
    }

    if( best_candidate != NULL ){
        current->parents.push_back( best_candidate );
        best_candidate->children.push_back(current);
        min_level = Organization::update_ancestors(current, this->instance, this->gamma, update_id);

        // Unnecessary in a local search
        // for (int i = min_level; i < this->all_states.size(); i++)
        //     sort(this->all_states[i].begin(), this->all_states[i].end(), State::compare);

        this->update_effectiveness();
    }

    this->last_modification = new Modification;
    this->last_modification->operation = 0;
    this->last_modification->level = level;
    this->last_modification->level_id = level_id;
    this->last_modification->min_level = min_level;

    return min_level;
}

//IT CONSIDERS THAT THE LAST PERFORMED OPERATION WAS A add_parent IN THE NODE LOCATED IN (level, level_id)
void Organization::undo_add_parent(Organization* prev_org, int level, int level_id, int min_level)
{

    //Remove the last parenthood relationship
    State *previous;
    State *current = this->all_states[level][level_id];
    State *parent = current->parents.back();
    parent->children.pop_back();
    current->parents.pop_back();

    for(int i = 0; i < this->all_states[min_level-1].size(); i++)
        this->all_states[min_level-1][i]->update_id = prev_org->all_states[min_level-1][i]->update_id;

    for(int l = min_level; l < this->all_states.size(); l++)
    {
        for(int l_id = 0; l_id < this->all_states[l].size(); l_id++)
        {
            current = this->all_states[l][l_id];
            previous = prev_org->all_states[l][l_id];
            if( current->update_id != previous->update_id )
            {
                current->update_id = previous->update_id;
                current->overall_reach_prob = previous->overall_reach_prob;

                for (int r = 0; r < this->instance->total_num_topics; r++)
                    current->reach_probs[r] = previous->reach_probs[r];
                
                if( l < level )
                {
                    current->sample_size = previous->sample_size;

                    for (int r = 0; r < this->instance->embedding_dim; r++)
                        current->sum_vector[r] = previous->sum_vector[r];

                    for (int r = 0; r < this->instance->total_num_topics; r++)
                        current->similarities[r] = previous->similarities[r];

                    for (int r = 0; r < this->instance->total_num_topics; r++)
                        current->domain[r] = current->domain[r];
                }
            }
        }
    }
    this->effectiveness = prev_org->effectiveness;
}

void Organization::undo_last_operation(Organization* prev_org)
{
    if( this->last_modification != NULL ) {
        if( this->last_modification->operation == 0 ) {
            this->undo_add_parent(prev_org, this->last_modification->level, this->last_modification->level_id, this->last_modification->min_level);
        } else {
            this->undo_delete_parent(prev_org, this->last_modification->deleted_states, this->last_modification->level);
        }
    }

    this->last_modification = NULL;
}

//IMPLEMENTATION CONSIDERING THAT ALL PARENTS OF A STATE ARE IN THE SAME LEVEL
//UNDER THIS CONSIDERATION, A BREADTH-FIRST SEARCH CAN UPDATE ALL PARENTS BEFORE ITS CHILDREN
void Organization::update_descendants(State *patriarch, float gamma, int total_num_topics, int update_id)
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
                current->update_reach_probs(gamma, total_num_topics);

            //<TEST!>
            // printf("\nLevel: %d\nID: %d\n", current->level, current->topic_id);
            // printf("Reach Probs\n");
            // for (int t = 0; t < total_num_columns; t++)
            //     printf("%.3f ", current->reach_probs[t]);
            
            // printf("\nOverall reach prob: %.3f\n", current->overall_reach_prob);
            // if(current->topic_id >= 0)
            //     printf("Discover probability: %.3f\n", current->reach_probs[current->topic_id]);
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
    queue<State*> queue; //QUEUE USED IN THE BREADTH-FIRST SEARCH
    queue.push(descendant);
    //PATRIARCHS WHOSE DESCENDANTS NEED TO UPDATE REACH_PROBS
    priority_queue<State*, vector<State*>, CompareLevel> outdated_states;  
    //ITERATORS
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
            for(int i = 0; i < inst->total_num_topics; i++)
            {
                if(descendant->domain[i] == 1 && current->domain[i] == 0)
                {
                    has_changed = 1;
                    current->domain[i] = 1;
                    for(int d = 0; d < inst->embedding_dim; d++)
                        current->sum_vector[d] += inst->topic_vectors[i][d];
                }
            }
        }

        if( has_changed == 1 )
        {
            //UPDATE SIMILARITIES
            for (int i = 0; i < inst->total_num_topics; i++)
                current->similarities[i] = cossine_similarity(current->sum_vector, inst->topic_vectors[i], inst->embedding_dim);
            
            //ITS PARENTS NEED TO BE VERIFIED
            for(int i = 0; i < current->parents.size(); i++)
                queue.push(current->parents[i]);
        } else {
            //ITS SIBILINGS NEED UPDATE THIER reach_probs
            outdated_states.push(current);
        }
        
    }
    //UPDATE AFFECTED SUBGRAPHS WITH outdated_states AS PATRIARCHS
    min_level = outdated_states.top()->level;
    while( !outdated_states.empty() )
    {
        current = outdated_states.top();
        outdated_states.pop();
        if( current->update_id != update_id )
        {
            current->update_reach_probs(gamma, inst->total_num_topics);
            current->update_id = update_id;
            for(int i = 0; i < current->children.size(); i++)
                outdated_states.push(current->children[i]);
        } 
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
    org->root->topic_id = -inst->total_num_topics;
    org->root->sum_vector = new float[inst->embedding_dim]; // VECTOR INITIALIZED WITH ZEROS
    org->root->sample_size = 0;
    org->root->domain = new int[inst->total_num_topics];
    for (int i = 0; i < inst->total_num_topics; i++)
        org->root->domain[i] = 1;

    State *state;
    //Leaf nodes (columns representation) creation
    for (int i = 0; i < inst->total_num_topics; i++)
    {
        state = new State;
        state->topic_id = i;
        state->update_id = -1;
        state->reach_probs = new double[inst->total_num_topics];
        state->domain = new int[inst->total_num_topics];
        state->domain[i] = 1;

        state->parents.push_back( org->root );
        state->sum_vector = inst->topic_vectors[i];
        //root sum vector incrementation 
        for (int d = 0; d < inst->embedding_dim; d++)
            org->root->sum_vector[d] += state->sum_vector[d];
        //Adding the leaf to the organization as child of the root
        org->root->children.push_back(state);

        state->similarities = new float[inst->total_num_topics];
        for(int s=0; s < inst->total_num_topics; s++)
            state->similarities[s] = cossine_similarity(state->sum_vector, inst->topic_vectors[s], inst->embedding_dim);   
    }
    org->compute_all_reach_probs();
    org->init_all_states();
    org->update_effectiveness();
    return org;
}


//GENERATE A ORGANIZATION BY HIERARQUICAL CLUSTERING
//ALGORITHM: NEAREST NEIGHBORS CHAIN -> O(N²)
//DISTANCE BETWEEN CLUSTERS: UNWEIGHTED PAIR-GROUP METHOD WITH ARITHMETIC MEAN (UPGMA)
Organization* Organization::generate_organization_by_clustering(Instance * inst, float gamma, bool compute_probs)
{
    //Organization setup
    Organization *org = new Organization;
    org->instance = inst;
    org->gamma = gamma;

    Cluster* active_clusters = Cluster::init_clusters(inst); // CHAINED LIST OF CLUSTERS AVAILABLE TO BE ADDED TO NN CHAIN
    float** dist_matrix = Cluster::init_dist_matrix(active_clusters, inst->total_num_topics, inst->embedding_dim); // DISTANCE BETWEEN ALL CLUSTERS

    Cluster *stack = active_clusters; //NEAREST NEIGHBORS CHAIN
    active_clusters = active_clusters->next; // REMOVE THE HEAD FROM CHAINED LIST AND ADD TO NN CHAIN

    Cluster *prev_nn, *nn; // PREVIOUS NN CLUSTER AND NN CLUSTER IN CHAINED LIST
    Cluster *previous, *current; // ITERATORS
    Cluster *new_cluster;

    int cluster_id = inst->total_num_topics; // MATRIX ID OF THE NEXT CLUSTER THAT WILL BE CREATED
    float diff_dists;

    //GENERATOR OF RANDOM NUMBERS
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_real_distribution<double> distribution(0.0, 1.0);

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
            if( 2 * distribution(generator) < ( 2 - dist_matrix[stack->id][stack->is_NN_of->id] ) )
                diff_dists = -1.0;
            else
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
    if( compute_probs ){
        org->compute_all_reach_probs();
        org->init_all_states();
        org->update_effectiveness();
    }
    return org;
}

Organization* Organization::generate_organization_by_heuristic(Instance * inst, float gamma)
{
    Organization *org = Organization::generate_organization_by_clustering(inst, gamma, false);
    int num_merges = ceil( 0.1 * ( abs(org->root->topic_id) - inst->total_num_topics ) );

    //GET NON-LEAF NODES IN THE ORGANIZATION
    State *current, *child;
    queue<State*> queue;
    vector<State*> internal_states;
    queue.push(org->root);
    internal_states.push_back(org->root);
    //O(N)
    while( !queue.empty() ){
        current = queue.front();
        queue.pop();
        for(int i=0; i < current->children.size(); i++) {
            child = current->children[i];
            if( child->children.size() > 0 ){
                if( child->children[0]->children.size() > 0 || child->children[1]->children.size() > 0 ) {
                    queue.push(child);
                    internal_states.push_back(child);
                }
            }
        }
    }


    int id;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    vector<State*> new_children;
    bool flag;

    for(int i = 0; i < num_merges; i++)
    {
        id = rand() % internal_states.size();
        current = internal_states[id];
        if( current->children.size() > 0 ){
            flag = false;
            for(int j = 0; j < current->children.size(); j++) {
                child = current->children[j];
                if( child->children.size() == 0 )
                    new_children.push_back(child);
                else {
                    flag = true;
                    // cout << child->topic_id << " " << child->children.size() <<  endl;
                    for(int k = 0; k < child->children.size(); k++) {
                        child->children[k]->parents[0] = current; //CHANGE THE PARENT
                        new_children.push_back(child->children[k]);
                    }
                    child->children.clear();
                }
            }
            if(flag)
                current->children = new_children;
            else {
                internal_states.erase(internal_states.begin() + id);
                i--;
            }
            new_children.clear();
        } else {
            internal_states.erase(internal_states.begin() + id);
            i--;
        }
    }

    org->compute_all_reach_probs();
    org->init_all_states();
    org->update_effectiveness();

    return org;
}