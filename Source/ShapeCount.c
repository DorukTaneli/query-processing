typedef void* SortMergeJoinDatabase;

SortMergeJoinDatabase SortMergeJoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd) {
    
    //int (*arr)[3] = malloc(sizeof(int)*totalNumberOfEdgesInTheEnd*3);
    int *arr[totalNumberOfEdgesInTheEnd];
    for (int i=0; i<totalNumberOfEdgesInTheEnd; i++) 
         arr[i] = (int *)malloc(3 * sizeof(int));

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
    qsort(edge1matches, sizeof(arr), sizeof(arr[0]), comparatorForTo);
    qsort(edge2matches, sizeof(arr), sizeof(arr[0]), comparatorForFrom);

    int leftIter = 0;
    int rightIter = 0;
    int valids1[(sizeof(arr)/sizeof(arr[0]))][3];
    int valids2[(sizeof(arr)/sizeof(arr[0]))][3];
    int validIter = 0;
    while(leftIter < (sizeof(arr)/sizeof(arr[0]) && rightIter < (sizeof(arr)/sizeof(arr[0])))) {
        auto leftInput = edge1matches[leftIter];
        auto rightInput = edge2matches[rightIter];
        if(leftInput[1] < rightInput[0])
            leftIter++;
        else if(rightInput[0] < leftInput[1])
            rightIter++;
        else {
            //Write to output
            valids1[validIter][0] = edge1matches[leftIter][0];
            valids1[validIter][1] = edge1matches[leftIter][1];
            valids1[validIter][2] = edge1matches[leftIter][2];

            valids2[validIter][0] = edge2matches[rightIter][0];
            valids2[validIter][1] = edge2matches[rightIter][1];
            valids2[validIter][1] = edge2matches[rightIter][2];
            validIter++;
            
            //Check for non-uniqueness
            if (leftInput[1] == edge1matches[leftIter+1][1])
                leftIter++;
            else if (rightInput[0] == edge2matches[rightIter+1][0])
                rightIter++;
            else {
                leftIter++;
                rightIter++;
            }
        }
    }
    // edges2 toNode = edges3 fromNode
    qsort(valids2, sizeof(arr), sizeof(arr[0]), comparatorForTo);
    qsort(edge3matches, sizeof(arr), sizeof(arr[0]), comparatorForFrom);

    leftIter = 0;
    rightIter = 0;
    int valids3[(sizeof(arr)/sizeof(arr[0]))][3];
    validIter = 0;
    while(leftIter < (sizeof(arr)/sizeof(arr[0]) && rightIter < (sizeof(arr)/sizeof(arr[0])))) {
        auto leftInput = valids2[leftIter];
        auto rightInput = edge3matches[rightIter];
        if(leftInput[1] < rightInput[0])
            leftIter++;
        else if(rightInput[0] < leftInput[1])
            rightIter++;
        else {
            //Write to output
            valids3[validIter][0] = edge3matches[rightIter][0];
            valids3[validIter][1] = edge3matches[rightIter][1];
            valids3[validIter][2] = edge3matches[rightIter][2];
            validIter++;
            
            //Check for non-uniqueness
            if (leftInput[1] == valids2[leftIter+1][1])
                leftIter++;
            else if (rightInput[0] == edge3matches[rightIter+1][0])
                rightIter++;
            else {
                leftIter++;
                rightIter++;
            }
        }
    }

    // edges3 toNode = edges1 fromNode
    qsort(valids3, sizeof(arr), sizeof(arr[0]), comparatorForTo);
    qsort(valids1, sizeof(arr), sizeof(arr[0]), comparatorForFrom);

    leftIter = 0;
    rightIter = 0;
    int count = 0;
    while(leftIter < (sizeof(arr)/sizeof(arr[0]) && rightIter < (sizeof(arr)/sizeof(arr[0])))) {
        auto leftInput = valids3[leftIter];
        auto rightInput = valids1[rightIter];
        if(leftInput[1] < rightInput[0])
            leftIter++;
        else if(rightInput[0] < leftInput[1])
            rightIter++;
        else {
            //Write to output
            count++;
            
            //Check for non-uniqueness
            if (leftInput[1] == valids3[leftIter+1][1])
                leftIter++;
            else if (rightInput[0] == valids1[rightIter+1][0])
                rightIter++;
            else {
                leftIter++;
                rightIter++;
            }
        }
    }

    return count;
    
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

int comparatorForTo (const void * a, const void * b) {
    return ((int *)a)[1] - ((int *)b)[1];
}

int comparatorForFrom (const void * a, const void * b) {
    return ((int *)a)[0] - ((int *)b)[0];
}

void

