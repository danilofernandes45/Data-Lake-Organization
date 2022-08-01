#include "Instance.hpp"

Instance* Instance::read_instance()
{
    Instance *instance = new Instance;

    scanf("%d %d", &instance->num_tables, &instance->embedding_dim);
    instance->total_num_columns = 0;
    instance->tables = new Table*[instance->num_tables];

    instance->num_tables = min(150, instance->num_tables);

    Table *table;

    for (int i = 0; i < instance->num_tables; i++)
    {
        table = new Table;
        instance->tables[i] = table;

        scanf("%d %d", &table->nrows, &table->ncols);
        instance->total_num_columns += table->ncols;
        table->sum_vectors = new float*[table->nrows];

        for (int j = 0; j < table->ncols; j++)
        {
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