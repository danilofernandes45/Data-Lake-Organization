#include "Instance.hpp"

Instance* Instance::read_instance()
{
    Instance *instance = new Instance;
    scanf("%d %d", &instance->total_num_topics, &instance->embedding_dim);
    instance->topic_vectors = new float*[instance->total_num_topics];

    for (int i = 0; i < instance->total_num_topics; i++)
    {
        instance->topic_vectors[i] = new float[instance->embedding_dim];
        for (int d = 0; d < instance->embedding_dim; d++)
            scanf("%f", &instance->topic_vectors[i][d]);
    }
    
    scanf("%d %d", &instance->num_tables, &instance->num_topics_per_column);
    instance->tables = new Table*[instance->num_tables];

    Table *table;

    for (int i = 0; i < instance->num_tables; i++)
    {
        table = new Table;
        instance->tables[i] = table;

        scanf("%d %d", &table->nrows, &table->ncols);
        table->topic_ids = new int*[table->ncols];

        for (int j = 0; j < table->ncols; j++)
        {
            table->topic_ids[j] = new int[instance->num_topics_per_column];
            for (int d = 0; d < instance->num_topics_per_column; d++)
                scanf("%d", &table->topic_ids[j][d]);
        }
    }

    //GENERATE MAP ABSOLUTE ID -> RELATIVE ID
    // instance->map = new int*[instance->total_num_columns];
    // int count = 0;
    // for (int i = 0; i < instance->num_tables; i++)
    // {
    //     for (int j = 0; j < instance->tables[i]->ncols; j++){
    //         instance->map[count] = new int[2];
    //         instance->map[count][0] = i;
    //         instance->map[count][1] = j;
    //         count++;
    //     }
    // }

    return instance;
}