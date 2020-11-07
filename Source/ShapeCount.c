typedef void* SortMergeJoinDatabase;
SortMergeJoinDatabase SortMergeJoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd) {
    int db[totalNumberOfEdgesInTheEnd][3];
    memset(db, -1, sizeof(db[0][0]) * totalNumberOfEdgesInTheEnd * 3);
    //USE -1 to denote a free spot in the db

}

void SortMergeJoinInsertEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel) {
    struct Database* db = ((struct Database*)database);

    int insert[3] = {fromNodeID, toNodeID, edgeLabel};

    for (int i = 0; i<(sizeof(db)/sizeof(db)); i++ ) {
        if (db[i][0] == -1) {

        }

    }


}

int SortMergeJoinRunQuery(SortMergeJoinDatabase database, int edgeLabel1, int edgeLabel2,
                          int edgeLabel3);
void SortMergeJoinDeleteEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID,
                             int edgeLabel);
void SortMergeJoinDeleteDatabase(SortMergeJoinDatabase database);

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

