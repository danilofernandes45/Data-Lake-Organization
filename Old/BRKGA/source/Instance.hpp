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
        // EACH COLUMN IS REPRESENTED BY A SUM VECTOR. SUM VECTOR = nrows * TOPIC VECTOR
        // A TOPIC VECTOR IS THE MEAN OF WORD EMBEDDING VECTORS OF A COLUMN VALUES
        vector< vector<double> > sum_vectors; 
};

class Instance
{
    public:
        int num_tables; //NUMBER OF TABLES INTO DATA LAKE
        int total_num_columns; // TOTAL NUMBER OF COLUMNS
        int embedding_dim; // WORD VECTOR EMBEDDING DIMENSION
        Table **tables; // TABLES FROM DATA LAKE
        int **map; // MAP ABSOLUTE (IN DL) IN RELATIVE (IN ITS TABLE) COLUMN ID
        static Instance* read_instance();
};

#endif