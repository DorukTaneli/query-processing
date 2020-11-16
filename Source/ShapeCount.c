#include <stdlib.h>

typedef void* SortMergeJoinDatabase;

//edge struct to be kept in an array
struct edge {
   int fromNode;   
   int toNode;
   int edgeLabel;
};

//edge database struct to pass the length info around
struct edge_db {
    int length;
    struct edge db[];
};

//Our additional function to test if we successfully added an edge
int SortMergeJoinFindEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    //set to correct types from void pointer
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int totNoEdges = dbstruct->length;

    //iterate over db to check if given edge exists
    for (int i = 0; i<totNoEdges; i++) {
        if (db[i].edgeLabel == edgeLabel && db[i].fromNode == fromNodeID && db[i].toNode == toNodeID) {
            return db[i].edgeLabel;
        }
    }

    return -1; //edge not found
}

SortMergeJoinDatabase SortMergeJoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd) {
    //Allocate memory in heap for database
    struct edge_db *dbstruct = (struct edge_db *) malloc(sizeof(*dbstruct) + (sizeof(struct edge) * totalNumberOfEdgesInTheEnd));
    struct edge *db = dbstruct->db;
    dbstruct->length = totalNumberOfEdgesInTheEnd;

    //initialize contents to -1
    for (int i = 0; i < totalNumberOfEdgesInTheEnd; i++) {
        db[i].fromNode = -1;
        db[i].toNode = -1;
        db[i].edgeLabel = -1;
    }

    return (void*) dbstruct;
}

void SortMergeJoinInsertEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    //set to correct types from void pointer
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int totNoEdges = dbstruct->length;

    for (int i = 0; i<totNoEdges; i++) { //iterate over db
        if (db[i].edgeLabel == -1) { //find empty spot to insert edge
            db[i].fromNode = fromNodeID;
            db[i].toNode = toNodeID;
            db[i].edgeLabel = edgeLabel;
            break;    
        }
    }
}

//Comparator for qsort, gets edge, compares 'to' fields
int comparatorForTo (const void * a, const void * b) {
    return ((struct edge *)a)->toNode - ((struct edge *)b)->toNode;
}

//Comparator for qsort, gets edge, compares 'for' fields
int comparatorForFrom (const void * a, const void * b) {
    return ((struct edge *)a)->fromNode - ((struct edge *)b)->fromNode;
}

int SortMergeJoinRunQuery(SortMergeJoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3) {
    //set to correct types from void pointer
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int totNoEdges = dbstruct->length;

    //edgeXmatches contain edges with desired labels
    struct edge edge1matches[totNoEdges];
    struct edge edge2matches[totNoEdges];
    struct edge edge3matches[totNoEdges];

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


    //SortMergeJoin1:
    //edges1-toNode = edges2-fromNode
    qsort(edge1matches, (sizeof(edge1matches)/sizeof(edge1matches[0])), sizeof(edge1matches[0]), &comparatorForTo);
    qsort(edge2matches, (sizeof(edge2matches)/sizeof(edge2matches[0])), sizeof(edge2matches[0]), &comparatorForFrom);

    //valids contain the edges with desired labels that are connected to each other
    struct edge valids1[totNoEdges]; //for edges1 with label1
    struct edge valids2[totNoEdges]; //for edges2 with label2
    for(int i=0; i<totNoEdges; i++) { //initialize to -1
        valids1[i].fromNode = -1;
        valids1[i].toNode = -1;
        valids1[i].edgeLabel = -1;

        valids2[i].fromNode = -1;
        valids2[i].toNode = -1;
        valids2[i].edgeLabel = -1;
    }

    //SortMergeJoin Code from slides modified to handle non-uniqeness
    int validIter = 0;
    int leftIter = 0;
    int rightIter = 0;
    while(leftIter < totNoEdges && rightIter < totNoEdges) {
        struct edge leftInput = edge1matches[leftIter];
        struct edge rightInput = edge2matches[rightIter];
        if(leftInput.toNode == -1){
            leftIter++;
            }
        else if(rightInput.fromNode == -1)
            rightIter++;
        else if(leftInput.toNode < rightInput.fromNode)
            leftIter++;
        else if(rightInput.fromNode < leftInput.toNode)
            rightIter++;
        else {
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

    //SortMergeJoin2:
    //edges2-toNode = edges3-fromNode
    qsort(valids2, (sizeof(valids2)/sizeof(valids2[0])), sizeof(valids2[0]), &comparatorForTo);
    qsort(edge3matches, (sizeof(edge3matches)/sizeof(edge3matches[0])), sizeof(edge3matches[0]), &comparatorForFrom);

    //valids3 contains the third edge connected to edge2 with desired labels
    struct edge valids3[totNoEdges];
    for(int i=0; i<totNoEdges; i++) {
        valids3[i].fromNode = -1;
        valids3[i].toNode = -1;
        valids3[i].edgeLabel = -1;
    }

    //SortMergeJoin Code from slides modified to handle non-uniqeness
    leftIter = 0;
    rightIter = 0;
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

    //valids3_FINAL contains the last edge of the closed triangle with desired labels
    struct edge valids3_FINAL[totNoEdges];
    for(int i=0; i<totNoEdges; i++) {
        valids3_FINAL[i].fromNode = -1;
        valids3_FINAL[i].toNode = -1;
        valids3_FINAL[i].edgeLabel = -1;
    }

    qsort(valids3, (sizeof(valids3)/sizeof(valids3[0])), sizeof(valids3[0]), &comparatorForTo);
    qsort(valids1, (sizeof(valids1)/sizeof(valids1[0])), sizeof(valids1[0]), &comparatorForFrom);
    leftIter = 0;
    rightIter = 0;
    validIter = 0;

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
            valids3_FINAL[validIter].fromNode = valids3[rightIter].fromNode;
            valids3_FINAL[validIter].toNode = valids3[rightIter].toNode;
            valids3_FINAL[validIter].edgeLabel = valids3[rightIter].edgeLabel;
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

    
    /*
    printf("VALIDS 1: \n");
    for(int i=0; i< sizeof(valids1)/sizeof(valids1[0]); i++){
        if (valids1[i].edgeLabel != -1){
            printf("For: %d, To: %d, Label: %d \n", valids1[i].fromNode, valids1[i].toNode, valids1[i].edgeLabel);
        }
    }

    printf("VALIDS 2: \n");
    for(int i=0; i< sizeof(valids2)/sizeof(valids2[0]); i++){
        if (valids2[i].edgeLabel != -1){
            printf("For: %d, To: %d, Label: %d \n", valids2[i].fromNode, valids2[i].toNode, valids2[i].edgeLabel);
        }
    }

    printf("VALIDS 3: \n");
    for(int i=0; i< sizeof(valids3_FINAL)/sizeof(valids3_FINAL[0]); i++){
        if (valids3_FINAL[i].edgeLabel != -1){
            printf("For: %d, To: %d, Label: %d \n", valids3_FINAL[i].fromNode, valids3_FINAL[i].toNode, valids3_FINAL[i].edgeLabel);
        }
    }
    */
    


    int v1_counter = 0;
    int v2_counter = 0;
    int v3_counter = 0;

    //printf("valids3: \n ");
    for(int i=0; i< sizeof(valids1)/sizeof(valids1[0]); i++){
        if (valids1[i].edgeLabel != -1){
            v1_counter++;
        }
    }

    for(int i=0; i< sizeof(valids2)/sizeof(valids2[0]); i++){
        if (valids2[i].edgeLabel != -1){
            v2_counter++;
        }
    }

    for(int i=0; i< sizeof(valids3_FINAL)/sizeof(valids3_FINAL[0]); i++){
        if (valids3_FINAL[i].edgeLabel != -1){
            v3_counter++;
        }
    }

    int final_counts[3] = {v1_counter, v2_counter, v3_counter};
    int numTriangles = v1_counter;

    for (int i = 0; i<3;i++){
        if (final_counts[i]<numTriangles){
            numTriangles = final_counts[i];
        }
    }

    return numTriangles;

    
    //NOT NEEDED

    
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
    //set to correct types from void pointer
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int totNoEdges = dbstruct->length;

    for (int i = 0; i<(totNoEdges); i++) { //iterate over db
        if (db[i].fromNode == fromNodeID && db[i].toNode == toNodeID && db[i].edgeLabel == edgeLabel) { //find edge to delete
            db[i].fromNode = -1;
            db[i].toNode = -1;
            db[i].edgeLabel = -1;
            break;
        }
    }
}

void SortMergeJoinDeleteDatabase(SortMergeJoinDatabase database) {
    struct edge_db *dbstruct = (struct edge_db *) database;
    free(dbstruct);
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

    for (int i = 0; i < totalNumberOfEdgesInTheEnd*hashattribute; i++) {
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
    while (quad < hashTableSize) {
        if (db[(hashValue + quad)%hashTableSize].edgeLabel == -1) {
            db[(hashValue + quad)%hashTableSize] = buildInput;
            //printf("Inserted edge: %d \n", db[i].edgeLabel);
            break;    
        }
        quad++;
    }
}

int HashjoinRunQuery(HashjoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3){
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int hashTableSize = dbstruct->length;

    struct edge edge1matches[hashTableSize]; //To hash since we are interested in where edge1 is going at this time
    struct edge edge2matches[hashTableSize]; //FROM hash since we are interested in where edge2 is coming from at this time
    struct edge edge3matches[hashTableSize]; //FROM hash since we are interested in where edge3 is coming from at this time


    //Initializing the arrays
    //edgeXmatches will store all edges that have value X

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
            while (quad < hashTableSize) {
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
            while (quad < hashTableSize) {
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
            while (quad < hashTableSize) {
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

    //printf("edgeXmatches filled\n");

    // edges1 toNode = edges2 fromNode
    //valids1 means all edges that have the correct value for edge1 and are going to a node
    //that has a correct value for edge2
    //Same for the other two
    struct edge valids1[hashTableSize]; //FROM
    struct edge valids2[hashTableSize]; //TO
    struct edge valids3[hashTableSize]; 

    for(int i=0; i<hashTableSize; i++) {
        valids1[i].fromNode = -1;
        valids1[i].toNode = -1;
        valids1[i].edgeLabel = -1;

        valids2[i].fromNode = -1;
        valids2[i].toNode = -1;
        valids2[i].edgeLabel = -1;

        valids3[i].fromNode = -1;
        valids3[i].toNode = -1;
        valids3[i].edgeLabel = -1;      
    }

    //ITERATING OVER EDGE 1 MATCHES
    //We create valids1 by checking all edges that have the correct value for edge1 and if 
    //they are going to a node that has a correct valued edge coming out of it
    for (int iter = 0; iter<hashTableSize; iter++){

        int fromHashEdge2 = fromHash(edge1matches[iter].toNode, hashTableSize);
        int linear_proberE2M = 0;
        int already_inserted = 0;

        while (linear_proberE2M<hashTableSize){
            //Does this edge2 connect to an edge1, if so we include them is valids2 and valids1 respectively
            if(edge2matches[(fromHashEdge2 + linear_proberE2M)%hashTableSize].fromNode == edge1matches[iter].toNode) {
                //INSERT INTO VALIDS 1 AND VALIDS 2
                //valids 1 insert
                int valids1_toHash = toHash(edge1matches[iter].toNode, hashTableSize);
                int valids1_linear_prober = 0;
                while (valids1_linear_prober<hashTableSize){
                    //IF EMPTY SPOT FOUND INSERT matching edge1 into valids1
                    if (valids1[(valids1_toHash+valids1_linear_prober)%hashTableSize].edgeLabel == -1 && (already_inserted == 0)){
                        valids1[(valids1_toHash+valids1_linear_prober)%hashTableSize].fromNode = edge1matches[iter].fromNode;
                        valids1[(valids1_toHash+valids1_linear_prober)%hashTableSize].toNode = edge1matches[iter].toNode;
                        valids1[(valids1_toHash+valids1_linear_prober)%hashTableSize].edgeLabel = edge1matches[iter].edgeLabel;
                        already_inserted = 1;
                        break;
                    }

                    if (already_inserted == 1){
                        break;
                    }

                    valids1_linear_prober++;
                }

                //valids 2 insert
                int valids2_fromHash = fromHash(edge1matches[iter].fromNode, hashTableSize);
                int valids2_linear_prober = 0;

                while (valids2_linear_prober<hashTableSize){
                    //IF EMPTY SPOT FOUND INSERT matching edge2 into valids2
                    if (valids2[(valids2_fromHash+valids2_linear_prober)%hashTableSize].edgeLabel == -1){
                        valids2[(valids2_fromHash+valids2_linear_prober)%hashTableSize].fromNode = edge2matches[(fromHashEdge2 + linear_proberE2M)%hashTableSize].fromNode;
                        valids2[(valids2_fromHash+valids2_linear_prober)%hashTableSize].toNode = edge2matches[(fromHashEdge2 + linear_proberE2M)%hashTableSize].toNode;
                        valids2[(valids2_fromHash+valids2_linear_prober)%hashTableSize].edgeLabel = edge2matches[(fromHashEdge2 + linear_proberE2M)%hashTableSize].edgeLabel;
                        break;
                    }
                    valids2_linear_prober++;
                }

                //break;
            }
        linear_proberE2M++;
        }
    }

    //for loop through edge3matches then see if the edge corresponds to a valid in both valids1 and valids2
    //if yes then we van say that it belongs in valids3 since it completes the triangle
    // ie if edge3match.toNode = valids1.fromNode and edge3match.fromNode = valids2.toNode 


    for (int i = 0; i<hashTableSize;i++){
        int valids1_edge3toHash = toHash(edge3matches[i].toNode, hashTableSize);
        int valids1_edge3_lp = 0;
        int valids2_edge3fromHash = fromHash(edge3matches[i].fromNode, hashTableSize); 
        int valids2_edge3_lp = 0;
        int isMatch1 = 0;
        int isMatch2 = 0;

        while (valids1_edge3_lp<hashTableSize) {
            if(valids1[(valids1_edge3toHash+valids1_edge3_lp)%hashTableSize].fromNode == edge3matches[i].toNode){
                if (edge3matches[i].toNode != -1){
                    isMatch1 = 1;
                    break;
                }
            }
            valids1_edge3_lp++;
        }

        while (valids2_edge3_lp<hashTableSize) {
            if(valids2[(valids2_edge3fromHash+valids2_edge3_lp)%hashTableSize].toNode == edge3matches[i].fromNode){
                if (edge3matches[i].toNode != -1){
                    isMatch2 = 1;
                    break;
                }
            }
            valids2_edge3_lp++;
        }

        if ((isMatch1 == 1) && (isMatch2 == 1)){
            int valids3_toHash = toHash(edge3matches[i].toNode, hashTableSize);
            int valids3_lp = 0;

            while (valids3_lp < hashTableSize){
                if(valids3[(valids3_toHash+valids3_lp)%hashTableSize].fromNode == -1){
                    valids3[(valids3_toHash+valids3_lp)%hashTableSize].fromNode = edge3matches[i].fromNode;
                    valids3[(valids3_toHash+valids3_lp)%hashTableSize].toNode = edge3matches[i].toNode;
                    valids3[(valids3_toHash+valids3_lp)%hashTableSize].edgeLabel = edge3matches[i].edgeLabel;
                    break;
                }
                valids3_lp++;
            }
        }
    }
    

    //Run through valids3
    //If match on valids1 has a triangle with 

    //The largest valids table will correspond to the number of triangles
    //Triangles can share edges but if they have to differ in certain edges to be different
    //The validsX will mean that they differ on X and therefore the validsX.count() = number of triangles
    // If any of the valids are zero it means that the edges specified didn't form any triangles


    int valids1_counter = 0;
    int valids2_counter = 0;
    int valids3_counter = 0;

    for (int i = 0; i<hashTableSize; i++){
        if (valids3[i].edgeLabel != -1) {
            valids3_counter++;
        }
    }

    for (int i = 0; i<hashTableSize; i++){
        if (valids2[i].edgeLabel != -1) {
            valids2_counter++;
        }
    }
    for (int i = 0; i<hashTableSize; i++){
        if (valids1[i].edgeLabel != -1) {
            valids1_counter++;
        }
    }

    int final_counts[3] = {valids1_counter, valids2_counter, valids3_counter};
    int final_count = valids1_counter;

    for (int i = 0; i<3;i++){
        if (final_counts[i] >final_count){
            final_count = final_counts[i];
        }
        if (final_counts[i] == 0){
            return 0;
        }
    }

    return final_count;
    
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
    struct edge_db *dbstruct = (struct edge_db *) database;
    free(dbstruct);
}




typedef void* CompetitionDatabase;

int CompetitionFindEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
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

CompetitionDatabase CompetitionAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd) {

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

void CompetitionInsertEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int totNoEdges = dbstruct->length;

    for (int i = 0; i<totNoEdges; i++) {
        if (db[i].edgeLabel == -1) {
            db[i].fromNode = fromNodeID;
            db[i].toNode = toNodeID;
            db[i].edgeLabel = edgeLabel;
            break;    
        }
    }
}

int CompetitionRunQuery(SortMergeJoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3) {
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int totNoEdges = dbstruct->length;

    struct edge edge1matches[totNoEdges];
    struct edge edge2matches[totNoEdges];
    struct edge edge3matches[totNoEdges];

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

    // The sort merge joins we need to run
    // edges1 toNode = edges2 fromNode
    qsort(edge1matches, (sizeof(edge1matches)/sizeof(edge1matches[0])), sizeof(edge1matches[0]), &comparatorForTo);
    qsort(edge2matches, (sizeof(edge2matches)/sizeof(edge2matches[0])), sizeof(edge2matches[0]), &comparatorForFrom);

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

    struct edge valids3_FINAL[totNoEdges];
    for(int i=0; i<totNoEdges; i++) {
        valids3_FINAL[i].fromNode = -1;
        valids3_FINAL[i].toNode = -1;
        valids3_FINAL[i].edgeLabel = -1;
    }

    qsort(valids3, (sizeof(valids3)/sizeof(valids3[0])), sizeof(valids3[0]), &comparatorForTo);
    qsort(valids1, (sizeof(valids1)/sizeof(valids1[0])), sizeof(valids1[0]), &comparatorForFrom);
    leftIter = 0;
    rightIter = 0;
    validIter = 0;

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
            valids3_FINAL[validIter].fromNode = valids3[rightIter].fromNode;
            valids3_FINAL[validIter].toNode = valids3[rightIter].toNode;
            valids3_FINAL[validIter].edgeLabel = valids3[rightIter].edgeLabel;
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

    
    /*
    printf("VALIDS 1: \n");
    for(int i=0; i< sizeof(valids1)/sizeof(valids1[0]); i++){
        if (valids1[i].edgeLabel != -1){
            printf("For: %d, To: %d, Label: %d \n", valids1[i].fromNode, valids1[i].toNode, valids1[i].edgeLabel);
        }
    }

    printf("VALIDS 2: \n");
    for(int i=0; i< sizeof(valids2)/sizeof(valids2[0]); i++){
        if (valids2[i].edgeLabel != -1){
            printf("For: %d, To: %d, Label: %d \n", valids2[i].fromNode, valids2[i].toNode, valids2[i].edgeLabel);
        }
    }

    printf("VALIDS 3: \n");
    for(int i=0; i< sizeof(valids3_FINAL)/sizeof(valids3_FINAL[0]); i++){
        if (valids3_FINAL[i].edgeLabel != -1){
            printf("For: %d, To: %d, Label: %d \n", valids3_FINAL[i].fromNode, valids3_FINAL[i].toNode, valids3_FINAL[i].edgeLabel);
        }
    }
    */
    


    int v1_counter = 0;
    int v2_counter = 0;
    int v3_counter = 0;

    //printf("valids3: \n ");
    for(int i=0; i< sizeof(valids1)/sizeof(valids1[0]); i++){
        if (valids1[i].edgeLabel != -1){
            v1_counter++;
        }
    }

    for(int i=0; i< sizeof(valids2)/sizeof(valids2[0]); i++){
        if (valids2[i].edgeLabel != -1){
            v2_counter++;
        }
    }

    for(int i=0; i< sizeof(valids3_FINAL)/sizeof(valids3_FINAL[0]); i++){
        if (valids3_FINAL[i].edgeLabel != -1){
            v3_counter++;
        }
    }

    int final_counts[3] = {v1_counter, v2_counter, v3_counter};
    int numTriangles = v1_counter;

    for (int i = 0; i<3;i++){
        if (final_counts[i]<numTriangles){
            numTriangles = final_counts[i];
        }
    }

    return numTriangles;

    
    //NOT NEEDED

    
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

void CompetitionDeleteEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    struct edge_db *dbstruct = (struct edge_db *) database;
    struct edge *db = dbstruct->db;
    int totNoEdges = dbstruct->length;

    for (int i = 0; i<(totNoEdges); i++) {
        if (db[i].fromNode == fromNodeID && db[i].toNode == toNodeID && db[i].edgeLabel == edgeLabel) {
            db[i].fromNode = -1;
            db[i].toNode = -1;
            db[i].edgeLabel = -1;
            break;
        }
    }
}

void CompetitionDeleteDatabase(SortMergeJoinDatabase database) {
    struct edge_db *dbstruct = (struct edge_db *) database;

    free(dbstruct);
}

