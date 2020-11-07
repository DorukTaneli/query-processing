typedef void* SortMergeJoinDatabase;

SortMergeJoinDatabase SortMergeJoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd) {
    
    int (*arr)[3] = malloc(totalNumberOfEdgesInTheEnd*3);

    //USE -1 to denote a free spot in the db
    for (int i = 0; i < totalNumberOfEdgesInTheEnd; i++) {
        for (int j = 0; j < 3; j++) {
            arr[i][j] = -1;
        }
    }

    return (void*) arr;
}

void SortMergeJoinInsertEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    int (*arr)[3] = (int (*)[3]) database;

    for (int i = 0; i<(sizeof(arr)/sizeof(arr[0])); i += sizeof(arr[0])) {
        if (arr[i][0] != -1) {
            arr[i][0] = fromNodeID;
            arr[i][1] = toNodeID;
            arr[i][2] = edgeLabel;
            break;    
        }
    }
}

int SortMergeJoinRunQuery(SortMergeJoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3) {
    int (*arr)[3] = (int (*)[3]) database;
    int edge1matches[(sizeof(arr)/sizeof(arr[0]))][3];
    int edge2matches[(sizeof(arr)/sizeof(arr[0]))][3];
    int edge3matches[(sizeof(arr)/sizeof(arr[0]))][3];

    for (int i = 0; i<(sizeof(arr)/sizeof(arr[0])); i += sizeof(arr[0])) {
        if(arr[i][2] == edgeLabel1) {
            edge1matches[i][0] = arr[i][0];
            edge1matches[i][1] = arr[i][1];
            edge1matches[i][2] = arr[i][2];
        }

        if(arr[i][2] == edgeLabel2) {
            edge2matches[i][0] = arr[i][0];
            edge2matches[i][1] = arr[i][1];
            edge2matches[i][2] = arr[i][2];
        }

        if(arr[i][2] == edgeLabel3) {
            edge3matches[i][0] = arr[i][0];
            edge3matches[i][1] = arr[i][1];
            edge3matches[i][2] = arr[i][2];
        }
    }

    // The sort merge joins we need to run
    // edges1 toNode = edges2 fromNode
    // edges2 toNode = edges3 fromNode
    // edges3 toNode = edges1 fromNode
    

}

void SortMergeJoinDeleteEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    int (*arr)[3] = (int (*)[3]) database;

    for (int i = 0; i<(sizeof(arr)/sizeof(arr[0])); i += sizeof(arr[0])) {
        if (arr[i][0] == fromNodeID && arr[i][1] == toNodeID && arr[i][2] == edgeLabel) {
            arr[i][0] = -1;
            arr[i][1] = -1;
            arr[i][2] = -1;
            break; //If we have duplicates remove this
        }
    }
}

void SortMergeJoinDeleteDatabase(SortMergeJoinDatabase database) {
    int (*arr)[3] = (int (*)[3]) database;
    free(arr);
}

typedef void* HashjoinDatabase;
HashjoinDatabase HashjoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd);
void HashjoinInsertEdge(HashjoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel);
int HashjoinRunQuery(HashjoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3);
void HashjoinDeleteEdge(HashjoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel);
void HashjoinDeleteDatabase(HashjoinDatabase database);

typedef void* CompetitionDatabase;
CompetitionDatabase CompetitionAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd);
void CompetitionInsertEdge(CompetitionDatabase database, int fromNodeID, int toNodeID,
                           int edgeLabel);
int CompetitionRunQuery(CompetitionDatabase database, int edgeLabel1, int edgeLabel2,
                        int edgeLabel3);
void CompetitionDeleteEdge(CompetitionDatabase database, int fromNodeID, int toNodeID,
                           int edgeLabel);
void CompetitionDeleteDatabase(CompetitionDatabase database);


//OUR HELPER CODE BELOW

