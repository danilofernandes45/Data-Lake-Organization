#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <stdio.h>
#include <bits/stdc++.h>
using namespace std;

class Table
{
    public:
        int nrows; //NUMBER OF ROWS
        int ncols; //NUMBER OF COLUMNS
        //EACH ROW OF topic_ids CORRESPONDS TO A TABLE COLUMN 
        // WHICH IS MAPPED INTO A SET OF TOPICS. topic_ids STORES ITS IDS IN topic_vectors
        int **topic_ids; 
};

class Instance
{
    public:
        int total_num_topics; //NUMBER OF TOPICS IN DL
        int embedding_dim; // WORD VECTOR EMBEDDING DIMENSION
        float **topic_vectors; //WORD EMBEDDING REPRESENTATION OF EACH TOPIC FOUND IN THE DL

        int num_tables; //NUMBER OF TABLES INTO DATA LAKE
        int num_topics_per_column; //NUMBER OF TOPICS EXTRACTED FROM EACH COLUMN
        // int total_num_columns; // TOTAL NUMBER OF COLUMNS
        Table **tables; // TABLES FROM DATA LAKE
        // int **map; // MAP ABSOLUTE (IN DL) IN RELATIVE (IN ITS TABLE) COLUMN ID

        static Instance* read_instance();
};

#endif