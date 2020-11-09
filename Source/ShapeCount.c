//#include <limits.h>

typedef void* SortMergeJoinDatabase;

struct edge {
   int fromNode;   
   int toNode;
   int edgeLabel;
};

struct edge_db {
    int length;
    struct edge db[];
};


int SortMergeJoinFindEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    struct edge_db *dbstruct = (struct edge_db *) database;

    struct edge *db = dbstruct->db;

    int totNoEdges = dbstruct->length;

    for (int i = 0; i<totNoEdges; i++) {
        if (db[i].edgeLabel == edgeLabel && db[i].fromNode == fromNodeID && db[i].toNode == toNodeID) {
            return db[i].edgeLabel;
        }
    }
    return -1;
}



SortMergeJoinDatabase SortMergeJoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd) {

    struct edge_db *dbstruct = (struct edge_db *) malloc(sizeof(*dbstruct) + (sizeof(struct edge) * totalNumberOfEdgesInTheEnd));
    
    struct edge *db = dbstruct->db;

    dbstruct->length = totalNumberOfEdgesInTheEnd;

    for (int i = 0; i < totalNumberOfEdgesInTheEnd; i++) {
        db[i].fromNode = -1;
        db[i].toNode = -1;
        db[i].edgeLabel = -1;
    }

    return (void*) dbstruct;
}

void SortMergeJoinInsertEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    //printf("InsertEdge called\n");
    struct edge_db *dbstruct = (struct edge_db *) database;

    struct edge *db = dbstruct->db;

    int totNoEdges = dbstruct->length;

    //printf("Total Edges: %d -- %d \n", sizeof(db), dbstruct->length);

    for (int i = 0; i<totNoEdges; i++) {
        //printf("%d\n", db[i].edgeLabel);
        if (db[i].edgeLabel == -1) {
            db[i].fromNode = fromNodeID;
            db[i].toNode = toNodeID;
            db[i].edgeLabel = edgeLabel;
            //printf("Inserted edge: %d \n", db[i].edgeLabel);
            break;    
        }
    }
}

int comparatorForTo (const void * a, const void * b) {
    //printf("Comparator called TO \n");
    return ((struct edge *)a)->toNode - ((struct edge *)b)->toNode;
}

int comparatorForFrom (const void * a, const void * b) {
    //printf("Comparator called FROM \n");
    return ((struct edge *)a)->fromNode - ((struct edge *)b)->fromNode;
}

int SortMergeJoinRunQuery(SortMergeJoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3) {
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int totNoEdges = dbstruct->length;
    //printf("Run query called \n");

    struct edge edge1matches[totNoEdges];
    struct edge edge2matches[totNoEdges];
    struct edge edge3matches[totNoEdges];

    //printf("Edge match arrays init complete\n");

    for (int i = 0; i<totNoEdges; i++) {
        if(db[i].edgeLabel == edgeLabel1) {
            edge1matches[i].fromNode = db[i].fromNode;
            edge1matches[i].toNode = db[i].toNode;
            edge1matches[i].edgeLabel = db[i].edgeLabel;
        } 
        if (db[i].edgeLabel != edgeLabel1) {
            edge1matches[i].fromNode = -1;
            edge1matches[i].toNode = -1;
            edge1matches[i].edgeLabel = -1;
        }

        if(db[i].edgeLabel == edgeLabel2) {
            edge2matches[i].fromNode = db[i].fromNode;
            edge2matches[i].toNode = db[i].toNode;
            edge2matches[i].edgeLabel = db[i].edgeLabel;
        } 
        if (db[i].edgeLabel != edgeLabel2) {
            edge2matches[i].fromNode = -1;
            edge2matches[i].toNode = -1;
            edge2matches[i].edgeLabel = -1;
        }

        if(db[i].edgeLabel == edgeLabel3) {
            edge3matches[i].fromNode = db[i].fromNode;
            edge3matches[i].toNode = db[i].toNode;
            edge3matches[i].edgeLabel = db[i].edgeLabel;
        } 
        if (db[i].edgeLabel != edgeLabel3) {
            edge3matches[i].fromNode = -1;
            edge3matches[i].toNode = -1;
            edge3matches[i].edgeLabel = -1;
        }
    }

    /*
    printf("Edge1matches: \n ");
    for(int i=0; i< sizeof(edge1matches)/sizeof(edge1matches[0]); i++){
        if (edge1matches[i].edgeLabel != -1)
            printf("For: %d, To: %d, Label: %d \n", edge1matches[i].fromNode, edge1matches[i].toNode, edge1matches[i].edgeLabel);
    }

    printf("Edge2matches: \n ");
    for(int i=0; i< sizeof(edge2matches)/sizeof(edge2matches[0]); i++){
        if (edge2matches[i].edgeLabel != -1)
            printf("For: %d, To: %d, Label: %d \n", edge2matches[i].fromNode, edge2matches[i].toNode, edge2matches[i].edgeLabel);
    }

    printf("Edge3matches: \n ");
    for(int i=0; i< sizeof(edge3matches)/sizeof(edge3matches[0]); i++){
        if (edge3matches[i].edgeLabel != -1)
            printf("For: %d, To: %d, Label: %d \n", edge3matches[i].fromNode, edge3matches[i].toNode, edge3matches[i].edgeLabel);
    }
    */

    //printf("Arrays filled \n");

    // The sort merge joins we need to run
    // edges1 toNode = edges2 fromNode
    qsort(edge1matches, (sizeof(edge1matches)/sizeof(edge1matches[0])), sizeof(edge1matches[0]), &comparatorForTo);
    qsort(edge2matches, (sizeof(edge2matches)/sizeof(edge2matches[0])), sizeof(edge2matches[0]), &comparatorForFrom);

    //printf("QSORT Completed \n");

    int leftIter = 0;
    int rightIter = 0;
    struct edge valids1[totNoEdges];
    struct edge valids2[totNoEdges];

    for(int i=0; i<totNoEdges; i++) {
        valids1[i].fromNode = -1;
        valids1[i].toNode = -1;
        valids1[i].edgeLabel = -1;

        valids2[i].fromNode = -1;
        valids2[i].toNode = -1;
        valids2[i].edgeLabel = -1;
    }

    int validIter = 0;
    while(leftIter < totNoEdges && rightIter < totNoEdges) {
        struct edge leftInput = edge1matches[leftIter];
        struct edge rightInput = edge2matches[rightIter];
        if(leftInput.toNode == -1){
            leftIter++;
            //printf("Skipped: %d, %d, %d \n", leftInput.fromNode, leftInput.toNode, leftInput.edgeLabel);
            }
        else if(rightInput.fromNode == -1)
            rightIter++;
        else if(leftInput.toNode < rightInput.fromNode)
            leftIter++;
        else if(rightInput.fromNode < leftInput.toNode)
            rightIter++;
        else {
            //Write to output
            //printf("Edge 1: %d, %d, %d \n", edge1matches[leftIter].fromNode, edge1matches[leftIter].toNode, edge1matches[leftIter].edgeLabel);
            //printf("Edge 2: %d, %d, %d \n",edge2matches[leftIter].fromNode, edge2matches[leftIter].toNode, edge2matches[leftIter].edgeLabel);
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

    printf("valids1: \n ");
    for(int i=0; i< sizeof(valids1)/sizeof(valids1[0]); i++){
        if (valids1[i].edgeLabel != -1)
            printf("For: %d, To: %d, Label: %d \n", valids1[i].fromNode, valids1[i].toNode, valids1[i].edgeLabel);
    }

    printf("valids2: \n ");
    for(int i=0; i< sizeof(valids2)/sizeof(valids2[0]); i++){
        if (valids2[i].edgeLabel != -1)
            printf("For: %d, To: %d, Label: %d \n", valids2[i].fromNode, valids2[i].toNode, valids2[i].edgeLabel);
    }

    //printf("First join completed \n");
    // edges2 toNode = edges3 fromNode
    qsort(valids2, (sizeof(valids2)/sizeof(valids2[0])), sizeof(valids2[0]), &comparatorForTo);
    qsort(edge3matches, (sizeof(edge3matches)/sizeof(edge3matches[0])), sizeof(edge3matches[0]), &comparatorForFrom);

    //printf("QSORT 2 Completed \n");

    leftIter = 0;
    rightIter = 0;
    struct edge valids3[totNoEdges];

    for(int i=0; i<totNoEdges; i++) {
        valids3[i].fromNode = -1;
        valids3[i].toNode = -1;
        valids3[i].edgeLabel = -1;
    }

    validIter = 0;
    while(leftIter < (totNoEdges) && rightIter < (totNoEdges)) {
        struct edge leftInput = valids2[leftIter];
        struct edge rightInput = edge3matches[rightIter];
        if(leftInput.toNode == -1)
            leftIter++;
        else if(rightInput.fromNode == -1)
            rightIter++;
        else if(leftInput.toNode < rightInput.fromNode)
            leftIter++;
        else if(rightInput.fromNode < leftInput.toNode)
            rightIter++;
        else {
            //Write to output
            valids3[validIter].fromNode = edge3matches[rightIter].fromNode;
            valids3[validIter].toNode = edge3matches[rightIter].toNode;
            valids3[validIter].edgeLabel = edge3matches[rightIter].edgeLabel;
            validIter++;
            
            //Check for non-uniqueness
            if (leftInput.toNode == valids2[leftIter+1].toNode)
                leftIter++;
            else if (rightInput.fromNode == edge3matches[rightIter+1].fromNode)
                rightIter++;
            else {
                leftIter++;
                rightIter++;
            }
        }
    }

    printf("valids3: \n ");
    for(int i=0; i< sizeof(valids3)/sizeof(valids3[0]); i++){
        if (valids3[i].edgeLabel != -1)
            printf("For: %d, To: %d, Label: %d \n", valids3[i].fromNode, valids3[i].toNode, valids3[i].edgeLabel);
    }

    //printf("Second join completed \n");

    // edges3 toNode = edges1 fromNode
    qsort(valids3, (sizeof(valids3)/sizeof(valids3[0])), sizeof(valids3[0]), &comparatorForTo);
    qsort(valids1, (sizeof(valids1)/sizeof(valids1[0])), sizeof(valids1[0]), &comparatorForFrom);

    //printf("QSORT 3 completed \n");

    leftIter = 0;
    rightIter = 0;
    int count = 0;
    while(leftIter < (totNoEdges) && rightIter < (totNoEdges)) {
        struct edge leftInput = valids3[leftIter];
        struct edge rightInput = valids1[rightIter];
        if(leftInput.toNode == -1)
            leftIter++;
        else if(rightInput.fromNode == -1)
            rightIter++;
        else if(leftInput.toNode < rightInput.fromNode)
            leftIter++;
        else if(rightInput.fromNode < leftInput.toNode)
            rightIter++;
        else {
            //Write to output
            count++;
            
            //Check for non-uniqueness
            if (leftInput.toNode == valids3[leftIter+1].toNode)
                leftIter++;
            else if (rightInput.fromNode == valids1[rightIter+1].fromNode)
                rightIter++;
            else {
                leftIter++;
                rightIter++;
            }
        }
    }

    if (count > 0 && count != 3) {
        printf("HERE count= %d",count);
        return (count + 1)/2;
    }

    return count;
    
}
void SortMergeJoinDeleteEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int totNoEdges = dbstruct->length;

    for (int i = 0; i<(totNoEdges); i++) {
        if (db[i].fromNode == fromNodeID && db[i].toNode == toNodeID && db[i].edgeLabel == edgeLabel) {
            //printf("Deleted edge: %d \n", db[i].edgeLabel);
            db[i].fromNode = -1;
            db[i].toNode = -1;
            db[i].edgeLabel = -1;
            break; //If we have duplicates remove this
        }
    }
}

/*
void SortMergeJoinDeleteEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    struct edge *db = (struct edge *)database;
    int totNoEdges = sizeof(db)/sizeof(struct edge);

    for (int i = 0; i<(totNoEdges); i++) {
        if (db[i].fromNode == fromNodeID && db[i].toNode == toNodeID && db[i].edgeLabel == edgeLabel) {
            db[i].fromNode = -1;
            db[i].toNode = -1;
            db[i].edgeLabel = -1;
            printf("Deleted edge: %d ", db[i].edgeLabel);
            break; //If we have duplicates remove this
        }
    }
}

*/

void SortMergeJoinDeleteDatabase(SortMergeJoinDatabase database) {
    struct edge *db = (struct edge *)database;
    free(db);
}

typedef void* HashjoinDatabase;
HashjoinDatabase HashjoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd){

}

int HashjoinFindEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    return 0;
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
int CompetitionFindEdge(CompetitionDatabase database, int fromNodeID, int toNodeID, int edgeLabel);
void CompetitionInsertEdge(CompetitionDatabase database, int fromNodeID, int toNodeID,
                           int edgeLabel);
int CompetitionRunQuery(CompetitionDatabase database, int edgeLabel1, int edgeLabel2,
                        int edgeLabel3);
void CompetitionDeleteEdge(CompetitionDatabase database, int fromNodeID, int toNodeID,
                           int edgeLabel);
void CompetitionDeleteDatabase(CompetitionDatabase database);


