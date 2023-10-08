#include "Instance.hpp"

Instance::~Instance() {
    Table *t;
    for (int i = 0; i < this->num_tables; i++) {
        t = this->tables[i];
        for (int j = 0; j < t->ncols; j++) {
            delete t->sum_vectors[j];
        }
        delete t;
    }
    for (int i = 0; i < this->total_num_columns; i++)
        delete this->map[i];
}

Instance* Instance::read_instance()
{
    Table *table;
    int num_tags_table, num_tags_col, aux;
    Instance *instance = new Instance;

    scanf("%d %d %d", &instance->num_tables, &instance->num_tags, &instance->embedding_dim);
    instance->total_num_columns = 0;
    instance->tables = new Table*[instance->num_tables];

    // instance->num_tables = min(10, instance->num_tables);

    for (int i = 0; i < instance->num_tables; i++)
    {
        table = new Table;
        instance->tables[i] = table;
        scanf("%d %d %d", &table->nrows, &table->ncols, &num_tags_table);

        //READ TABLE TAGS
        for (int j = 0; j < num_tags_table; j++) {
            scanf("%d", &aux);
            table->tags_table.push_back(aux);
        }

        instance->total_num_columns += table->ncols;
        table->sum_vectors = new float*[table->ncols];

        for (int j = 0; j < table->ncols; j++)
        {
            //READ COLUMN TAGS
            scanf("%d", &num_tags_col);
            table->tags_cols.push_back( vector<int>() );
            for (int k = 0; k < num_tags_col; k++) {
                scanf("%d", &aux);
                table->tags_cols[j].push_back(aux);
            }
            //READ COLUMN TOPIC VECTOR
            table->sum_vectors[j] = new float[instance->embedding_dim];
            for (int d = 0; d < instance->embedding_dim; d++)
                scanf("%f", &table->sum_vectors[j][d]);
        }
    }

    //GENERATE MAP ABSOLUTE ID -> RELATIVE ID
    instance->map = new int*[instance->total_num_columns];
    int count = 0;
    for (int i = 0; i < instance->num_tables; i++)
    {
        for (int j = 0; j < instance->tables[i]->ncols; j++){
            instance->map[count] = new int[2];
            instance->map[count][0] = i;
            instance->map[count][1] = j;
            count++;
        }
    }

    // printf("%d %d %d\n", instance.num_tables, instance.embedding_dim, instance.total_num_columns);
    
    // for (int i = 0; i < instance.num_tables; i++)
    // {
    //     Table *table = instance.tables[i];

    //     printf("%d %d\n", table->nrows, table->ncols);

    //     for (int j = 0; j < table->ncols; j++)
    //     {
    //         for (int d = 0; d < instance.embedding_dim; d++)
    //             printf("%f ", table->sum_vectors[j][d]); 
    //         printf("\n");
    //     }
    // }

    return instance;
}

void Instance::print_stats()
{
    double mean = 0;
    double sd = 0;
    double similarity;
    int table_id, col_id, num_dists;
    float *sum_vector_i, *sum_vector_j;
    cout << this->total_num_columns << endl;
    for(int i = 0; i < this->total_num_columns; i++)
    {
        table_id = this->map[i][0];
        col_id = this->map[i][1];
        sum_vector_i = this->tables[table_id]->sum_vectors[col_id];
        for(int j = i+1; j < this->total_num_columns; j++)
        {
            table_id = this->map[j][0];
            col_id = this->map[j][1];
            sum_vector_j = this->tables[table_id]->sum_vectors[col_id];
            similarity = cossine_similarity(sum_vector_i, sum_vector_j, this->embedding_dim);
            mean = mean + similarity;
            sd = sd + pow(similarity, 2);
        }
    }
    num_dists = ( pow(this->total_num_columns, 2) - this->total_num_columns ) / 2;
    mean = mean / num_dists;
    sd = ( sd - pow(mean, 2) ) / num_dists;

    cout << mean << " " << sd << endl;

}

Instance* Instance::read_instance(string filename)
{
    Table *table;
    int num_tags_table, num_tags_col, aux;
    Instance *instance = new Instance;

    ifstream file (filename);

    file >> instance->num_tables;
    file >> instance->num_tags;
    file >> instance->embedding_dim;

    // scanf("%d %d %d", &instance->num_tables, &instance->num_tags, &instance->embedding_dim);
    instance->total_num_columns = 0;
    instance->tables = new Table*[instance->num_tables];

    // instance->num_tables = min(10, instance->num_tables);

    for (int i = 0; i < instance->num_tables; i++)
    {
        table = new Table;
        instance->tables[i] = table;

        file >> table->nrows;
        file >> table->ncols;
        file >> num_tags_table;

        // scanf("%d %d %d", &table->nrows, &table->ncols, &num_tags_table);

        //READ TABLE TAGS
        for (int j = 0; j < num_tags_table; j++) {
            file >> aux;
            // scanf("%d", &aux);
            table->tags_table.push_back(aux);
        }

        instance->total_num_columns += table->ncols;
        table->sum_vectors = new float*[table->ncols];

        for (int j = 0; j < table->ncols; j++)
        {
            //READ COLUMN TAGS
            file >> num_tags_col;
            // scanf("%d", &num_tags_col);
            table->tags_cols.push_back( vector<int>() );
            for (int k = 0; k < num_tags_col; k++) {
                file >> aux;
                // scanf("%d", &aux);
                table->tags_cols[j].push_back(aux);
            }
            //READ COLUMN TOPIC VECTOR
            table->sum_vectors[j] = new float[instance->embedding_dim];
            for (int d = 0; d < instance->embedding_dim; d++)
                file >> table->sum_vectors[j][d];
                // scanf("%f", &table->sum_vectors[j][d]);
        }
    }

    //GENERATE MAP ABSOLUTE ID -> RELATIVE ID
    instance->map = new int*[instance->total_num_columns];
    int count = 0;
    for (int i = 0; i < instance->num_tables; i++)
    {
        for (int j = 0; j < instance->tables[i]->ncols; j++){
            instance->map[count] = new int[2];
            instance->map[count][0] = i;
            instance->map[count][1] = j;
            count++;
        }
    }

    // printf("%d %d %d\n", instance->num_tables, instance->num_tags, instance->embedding_dim, instance->total_num_columns);
    
    // for (int i = 0; i < instance->num_tables; i++)
    // {
    //     Table *table = instance->tables[i];

    //     printf("%d %d\n", table->nrows, table->ncols);

    //     for (int j = 0; j < table->ncols; j++)
    //     {
    //         for (int d = 0; d < instance->embedding_dim; d++)
    //             printf("%f ", table->sum_vectors[j][d]); 
    //         printf("\n");
    //     }
    // }

    return instance;
}