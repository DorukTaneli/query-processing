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
    //printf("_____FUNCTION START_______ \n");
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

/*
    //printf("valids1: \n ");
    for(int i=0; i< sizeof(valids1)/sizeof(valids1[0]); i++){
        if (valids1[i].edgeLabel != -1)
            //printf("For: %d, To: %d, Label: %d \n", valids1[i].fromNode, valids1[i].toNode, valids1[i].edgeLabel);
    }
    */

    /* printf("valids2: \n ");
    for(int i=0; i< sizeof(valids2)/sizeof(valids2[0]); i++){
        if (valids2[i].edgeLabel != -1)
            //printf("For: %d, To: %d, Label: %d \n", valids2[i].fromNode, valids2[i].toNode, valids2[i].edgeLabel);
    }

    */

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

    int final_count = 0;

    //printf("valids3: \n ");
    for(int i=0; i< sizeof(valids3)/sizeof(valids3[0]); i++){
        if (valids3[i].edgeLabel != -1){
            //printf("For: %d, To: %d, Label: %d \n", valids3[i].fromNode, valids3[i].toNode, valids3[i].edgeLabel);
            final_count++;
        }
    }

    return final_count;

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
/*
    if (count > 0 && count != 3) {
        printf("HERE count= %d",count);
        return (count + 1)/2;
    }
*/
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

int hashattribute = 2;

int myHash(struct edge e, int hashTableSize) {
    return (e.fromNode*5 + e.toNode*7 + e.edgeLabel*11) % hashTableSize;
}
int toHash(int toNode, int hashTableSize) {
    return (toNode*7) % hashTableSize;
}
int fromHash(int fromNode, int hashTableSize) {
    return (fromNode*5) % hashTableSize;
}



HashjoinDatabase HashjoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd){
    struct edge_db *dbstruct = (struct edge_db *) malloc(sizeof(*dbstruct) + (sizeof(struct edge) * (totalNumberOfEdgesInTheEnd*hashattribute)));
    
    struct edge *db = dbstruct->db;

    dbstruct->length = totalNumberOfEdgesInTheEnd*hashattribute;

    for (int i = 0; i < totalNumberOfEdgesInTheEnd; i++) {
        db[i].fromNode = -1;
        db[i].toNode = -1;
        db[i].edgeLabel = -1;
    }

    return (void*) dbstruct;
}

int HashjoinFindEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int hashTableSize = dbstruct->length;

    struct edge probeInput = {fromNodeID, toNodeID, edgeLabel};
    int hashValue = myHash(probeInput, hashTableSize);

    int quad = 1;
    while (1) {
        if (db[(hashValue + quad)%hashTableSize].fromNode == probeInput.fromNode &&
            db[(hashValue + quad)%hashTableSize].toNode == probeInput.toNode &&
            db[(hashValue + quad)%hashTableSize].edgeLabel == probeInput.edgeLabel) {
                return probeInput.edgeLabel;  
        }

        if (quad > hashTableSize) {
            return -1;
        }

        quad++;
    }
}

void HashjoinInsertEdge(HashjoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel){
    //printf("InsertEdge called\n");
    struct edge_db *dbstruct = (struct edge_db *) database;

    struct edge *db = dbstruct->db;

    int hashTableSize = dbstruct->length;

    struct edge buildInput = {fromNodeID, toNodeID, edgeLabel};
    int hashValue = myHash(buildInput, hashTableSize);

    int quad = 1;
    while (1) {
        if (db[(hashValue + quad)%hashTableSize].edgeLabel == -1) {
            db[(hashValue + quad)%hashTableSize] = buildInput;
            //printf("Inserted edge: %d \n", db[i].edgeLabel);
            break;    
        }
        quad++;
    }
}


int HashjoinRunQuery(HashjoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3){
    //printf("_____FUNCTION START_______ \n");
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int hashTableSize = dbstruct->length;
    //printf("Run query called \n");

    struct edge edge1matches[hashTableSize]; //TO
    struct edge edge2matches[hashTableSize]; //FROM
    struct edge edge3matches[hashTableSize]; //FROM

    //printf("Edge match arrays init complete\n");

    for (int i = 0; i<hashTableSize; i++) {
        edge1matches[i].fromNode = -1;
        edge1matches[i].toNode = -1;
        edge1matches[i].edgeLabel = -1;

        edge2matches[i].fromNode = -1;
        edge2matches[i].toNode = -1;
        edge2matches[i].edgeLabel = -1;

        edge3matches[i].fromNode = -1;
        edge3matches[i].toNode = -1;
        edge3matches[i].edgeLabel = -1;
    }


    //edge1 - to
    for (int i = 0; i<hashTableSize; i++) {
        if (db[i].edgeLabel == edgeLabel1) {
            int hashValue = toHash(edgeLabel1, hashTableSize);

            int quad = 1;
            while (1) {
                if (edge1matches[(hashValue + quad)%hashTableSize].edgeLabel == -1) {
                    edge1matches[(hashValue + quad)%hashTableSize].edgeLabel = edgeLabel1;
                    edge1matches[(hashValue + quad)%hashTableSize].toNode = db[i].toNode;
                    edge1matches[(hashValue + quad)%hashTableSize].fromNode = db[i].fromNode;
                    //printf("Inserted edge: %d \n", db[i].edgeLabel);
                    break;    
                }
                quad++;
            }
        }
    }

    //edge2 - from
    for (int i = 0; i<hashTableSize; i++) {
        if (db[i].edgeLabel == edgeLabel2) {
            int hashValue = fromHash(edgeLabel2, hashTableSize);

            int quad = 1;
            while (1) {
                if (edge2matches[(hashValue + quad)%hashTableSize].edgeLabel == -1) {
                    edge2matches[(hashValue + quad)%hashTableSize].edgeLabel = edgeLabel2;
                    edge2matches[(hashValue + quad)%hashTableSize].toNode = db[i].toNode;
                    edge2matches[(hashValue + quad)%hashTableSize].fromNode = db[i].fromNode;
                    //printf("Inserted edge: %d \n", db[i].edgeLabel);
                    break;    
                }
                quad++;
            }
        }
    }

    //edge3 - from
    for (int i = 0; i<hashTableSize; i++) {
        if (db[i].edgeLabel == edgeLabel3) {
            int hashValue = fromHash(edgeLabel3, hashTableSize);

            int quad = 1;
            while (1) {
                if (edge3matches[(hashValue + quad)%hashTableSize].edgeLabel == -1) {
                    edge3matches[(hashValue + quad)%hashTableSize].edgeLabel = edgeLabel3;
                    edge3matches[(hashValue + quad)%hashTableSize].toNode = db[i].toNode;
                    edge3matches[(hashValue + quad)%hashTableSize].fromNode = db[i].fromNode;
                    //printf("Inserted edge: %d \n", db[i].edgeLabel);
                    break;    
                }
                quad++;
            }
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

    // edges1 toNode = edges2 fromNode
    struct edge valids1[hashTableSize]; //FROM
    struct edge valids2[hashTableSize]; //TO

    for(int i=0; i<hashTableSize; i++) {
        valids1[i].fromNode = -1;
        valids1[i].toNode = -1;
        valids1[i].edgeLabel = -1;

        valids2[i].fromNode = -1;
        valids2[i].toNode = -1;
        valids2[i].edgeLabel = -1;
    }


    int quad = 0;
    for (int iter = 0; iter < hashTableSize; iter++) {

        while (quad < hashTableSize) {
            int hashValue = fromHash(edge1matches[iter].fromNode, hashTableSize);
            int RightFrom;

            int hashValueR = toHash(edge1matches[iter].toNode, hashTableSize);

            int quadR = 1;
            while (quadR < hashTableSize) {
                if (edge2matches[(hashValueR + quadR)%hashTableSize].fromNode == edge1matches[iter].toNode) {
                        RightFrom = edge2matches[(hashValueR + quadR)%hashTableSize].fromNode;  
                }

                if (quadR > hashTableSize) {
                   break;
                }

                quadR++;
            }

            if (edge1matches[iter].toNode == RightFrom){
                //Insert to hashTable
                struct edge buildInputValids1 = {edge1matches[iter].fromNode, edge1matches[iter].toNode, edge1matches[iter].edgeLabel};
                int hashValueValids1 = fromHash(edge1matches[iter].fromNode, hashTableSize);

                int quadValids1 = 1;
                while (quadValids1 < hashTableSize) {
                    if (edge1matches[(hashValueValids1 + quadValids1)%hashTableSize].edgeLabel == -1) {
                        edge1matches[(hashValue + quadValids1)%hashTableSize] = buildInputValids1;
                        break;    
                    }
                    quadValids1++;
                }

                struct edge buildInputValids2 = {edge2matches[iter].fromNode, edge2matches[iter].toNode, edge2matches[iter].edgeLabel};
                int hashValueValids2 = toHash(edge2matches[iter].toNode, hashTableSize);

                int quadValids2 = 1;
                while (quadValids2 < hashTableSize) {
                    if (edge2matches[(hashValueValids2 + quadValids2)%hashTableSize].edgeLabel == -1) {
                        edge2matches[(hashValue + quadValids2)%hashTableSize] = buildInputValids2;
                        break;    
                    }
                    quadValids2++;
                }
            }

            if (quad > hashTableSize) {
               break;
            }
            quad++;

        }
    }

    // _________

    struct edge valids3[hashTableSize];

    for(int i=0; i<hashTableSize; i++) {
        valids3[i].fromNode = -1;
        valids3[i].toNode = -1;
        valids3[i].edgeLabel = -1;
    }


    int quad2 = 0;
    for (int iter = 0; iter < hashTableSize; iter++) {

        while (quad2 < hashTableSize) {
            int hashValue = fromHash(edge3matches[iter].fromNode, hashTableSize);
            int RightFrom;

            int hashValueR = toHash(valids2[iter].toNode, hashTableSize);

            int quadR = 1;
            while (quadR < hashTableSize) {
                if (edge3matches[(hashValueR + quadR)%hashTableSize].fromNode == valids2[iter].toNode) {
                        RightFrom = edge3matches[(hashValueR + quadR)%hashTableSize].fromNode;  
                }

                if (quadR > hashTableSize) {
                   break;
                }

                quadR++;
            }

            if (valids2[iter].toNode == RightFrom){
                //Insert to hashTable
                struct edge buildInputValids3 = {valids2[iter].fromNode, valids2[iter].toNode, valids2[iter].edgeLabel};
                int hashValueValids3 = fromHash(valids2[iter].fromNode, hashTableSize);

                int quadValids3 = 1;
                while (1) {
                    if (valids3[(hashValueValids3 + quadValids3)%hashTableSize].edgeLabel == -1) {
                        valids3[(hashValue + quadValids3)%hashTableSize] = buildInputValids3;
                        break;    
                    }
                    quadValids3++;
                }
            }

            if (quad2 > hashTableSize) {
               break;
            }
            quad2++;

        }
    }
    
    int final_counter = 0;
    printf("VALIDS 3: \n");
    for (int i = 0; i<hashTableSize; i++){
        if (valids3[i].edgeLabel != -1) {
            final_counter++;
            printf("fromNode: %d, toNode: %d, edgeLabel %d \n", valids3[i].fromNode,valids3[i].toNode, valids3[i].edgeLabel);
        }
    }

    return final_counter;

}
void HashjoinDeleteEdge(HashjoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel){
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int hashTableSize = dbstruct->length;

    struct edge probeInput = {fromNodeID, toNodeID, edgeLabel};
    int hashValue = myHash(probeInput, hashTableSize);

    int quad = 1;
    while (1) {
        if (db[(hashValue + quad)%hashTableSize].fromNode == probeInput.fromNode &&
            db[(hashValue + quad)%hashTableSize].toNode == probeInput.toNode &&
            db[(hashValue + quad)%hashTableSize].edgeLabel == probeInput.edgeLabel) {
                db[(hashValue + quad)%hashTableSize].fromNode = -1;
                db[(hashValue + quad)%hashTableSize].toNode = -1;
                db[(hashValue + quad)%hashTableSize].edgeLabel = -1;
                break;
        }

        //If edge is not in hashTable
        if (quad > hashTableSize) {
            break;
        }

        quad++;
    }
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


