typedef void* SortMergeJoinDatabase;

struct edge {
   int fromNode;   
   int toNode;
   int edgeLabel;
};


SortMergeJoinDatabase SortMergeJoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd) {
    
    struct edge db[totalNumberOfEdgesInTheEnd];

    for (int i = 0; i < totalNumberOfEdgesInTheEnd; i++) {
        db[i].fromNode = -1;
        db[i].toNode = -1;
        db[i].edgeLabel = -1;
    }

    return (void*) db;
}

void SortMergeJoinInsertEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    struct edge db[] = (struct edge *)database;

    int totNoEdges = sizeof(db)/sizeof(struct edge);

    for (int i = 0; i<totNoEdges; i++) {
        if (db[i].edgeLabel != -1) {
            db[i].fromNode = fromNodeID;
            db[i].toNode = toNodeID;
            db[i].edgeLabel = edgeLabel;
            break;    
        }
    }
}

int comparatorForTo (const void * a, const void * b) {
    return ((struct edge *)a)->toNode - ((struct edge *)b)->toNode;
}

int comparatorForFrom (const void * a, const void * b) {
    return ((struct edge *)a)->fromNode - ((struct edge *)b)->fromNode;
}

int SortMergeJoinRunQuery(SortMergeJoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3) {
    struct edge db[] = (struct edge *)database;

    int totNoEdges = sizeof(db)/sizeof(struct edge);

    struct edge edge1matches[totNoEdges];
    struct edge edge2matches[totNoEdges];
    struct edge edge3matches[totNoEdges];

    for (int i = 0; i<totNoEdges; i++) {
        if(db[i].edgeLabel == edgeLabel1) {
            edge1matches[i].fromNode = db[i].fromNode;
            edge1matches[i].toNode = db[i].toNode;
            edge1matches[i].edgeLabel = db[i].edgeLabel;
        }

        if(db[i].edgeLabel == edgeLabel2) {
            edge2matches[i].fromNode = db[i].fromNode;
            edge2matches[i].toNode = db[i].toNode;
            edge2matches[i].edgeLabel = db[i].edgeLabel;
        }

        if(db[i].edgeLabel == edgeLabel3) {
            edge3matches[i].fromNode = db[i].fromNode;
            edge3matches[i].toNode = db[i].toNode;
            edge3matches[i].edgeLabel = db[i].edgeLabel;
        }
    }

    // The sort merge joins we need to run
    // edges1 toNode = edges2 fromNode
    qsort(edge1matches, sizeof(edge1matches), sizeof(struct edge), comparatorForTo);
    qsort(edge2matches, sizeof(edge2matches), sizeof(struct edge), comparatorForFrom);

    int leftIter = 0;
    int rightIter = 0;
    struct edge valids1[totNoEdges];
    struct edge valids2[totNoEdges];
    int validIter = 0;
    while(leftIter < totNoEdges && rightIter < totNoEdges) {
        struct edge leftInput = edge1matches[leftIter];
        struct edge rightInput = edge2matches[rightIter];
        if(leftInput.toNode < rightInput.fromNode)
            leftIter++;
        else if(rightInput.fromNode < leftInput.toNode)
            rightIter++;
        else {
            //Write to output
            valids1[validIter].fromNode = edge1matches[leftIter].fromNode;
            valids1[validIter].toNode = edge1matches[leftIter].toNode;
            valids1[validIter].edgeLabel = edge1matches[leftIter].edgeLabel;

            valids2[validIter].fromNode = edge2matches[leftIter].fromNode;
            valids2[validIter].toNode = edge2matches[leftIter].toNode;
            valids2[validIter].edgeLabel = edge2matches[leftIter].edgeLabel;
            validIter++;
            
            //Check for non-uniqueness
            if (leftInput.toNode == edge1matches[leftIter+1].toNode)
                leftIter++;
            else if (rightInput.fromNode == edge2matches[rightIter+1].fromNode)
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
HashjoinDatabase HashjoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd){

}
void HashjoinInsertEdge(HashjoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel){

}
int HashjoinRunQuery(HashjoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3){
    return 5;
}
void HashjoinDeleteEdge(HashjoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel){

}
void HashjoinDeleteDatabase(HashjoinDatabase database){

}

typedef void* CompetitionDatabase;
CompetitionDatabase CompetitionAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd);
void CompetitionInsertEdge(CompetitionDatabase database, int fromNodeID, int toNodeID,
                           int edgeLabel);
int CompetitionRunQuery(CompetitionDatabase database, int edgeLabel1, int edgeLabel2,
                        int edgeLabel3);
void CompetitionDeleteEdge(CompetitionDatabase database, int fromNodeID, int toNodeID,
                           int edgeLabel);
void CompetitionDeleteDatabase(CompetitionDatabase database);


