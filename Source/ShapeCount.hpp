#pragma once
#include <functional>
extern "C" {
typedef void* SortMergeJoinDatabase;
SortMergeJoinDatabase SortMergeJoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd);
int SortMergeJoinFindEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID,
                             int edgeLabel);
void SortMergeJoinInsertEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID,
                             int edgeLabel);
int SortMergeJoinRunQuery(SortMergeJoinDatabase database, int edgeLabel1, int edgeLabel2,
                          int edgeLabel3);
void SortMergeJoinDeleteEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID,
                             int edgeLabel);
void SortMergeJoinDeleteDatabase(SortMergeJoinDatabase database);

typedef void* HashjoinDatabase;
HashjoinDatabase HashjoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd);
int HashjoinFindEdge(HashjoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel);
void HashjoinInsertEdge(HashjoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel);
int HashjoinRunQuery(HashjoinDatabase database, int edgeLabel1, int edgeLabel2, int edgeLabel3);
void HashjoinDeleteEdge(HashjoinDatabase database, int fromNodeID, int toNodeID, int edgeLabel);
void HashjoinDeleteDatabase(HashjoinDatabase database);

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
}

struct SortMergeJoinImplementation {
  std::function<void(void* sVoid, int from, int to, int label)> const findEdge =
      ::SortMergeJoinFindEdge;
  std::function<void(void* sVoid, int from, int to, int label)> const insertEdge =
      ::SortMergeJoinInsertEdge;
  std::function<void(void* sVoid, int fromNode, int toNode, int label)> const deleteEdge =
      ::SortMergeJoinDeleteEdge;
  std::function<int(void* vstore, int label1, int label2, int label3)> const runQuery =
      ::SortMergeJoinRunQuery;
  std::function<void*(unsigned long size)> const allocateDatabase = ::SortMergeJoinAllocateDatabase;
  std::function<void(void* sVoid)> const deleteDatabase = ::SortMergeJoinDeleteDatabase;
};

struct HashjoinImplementation {
  std::function<void(void* sVoid, int from, int to, int label)> const findEdge =
      ::HashjoinFindEdge;
  std::function<void(void* sVoid, int from, int to, int label)> const insertEdge =
      ::HashjoinInsertEdge;
  std::function<void(void* sVoid, int fromNode, int toNode, int label)> const deleteEdge =
      ::HashjoinDeleteEdge;
  std::function<int(void* vstore, int label1, int label2, int label3)> const runQuery =
      ::HashjoinRunQuery;
  std::function<void*(unsigned long size)> const allocateDatabase = ::HashjoinAllocateDatabase;
  std::function<void(void* sVoid)> const deleteDatabase = ::HashjoinDeleteDatabase;
};

struct CompetitionImplementation {
  std::function<void(void* sVoid, int from, int to, int label)> const findEdge =
      ::HashjoinFindEdge;
  std::function<void(void* sVoid, int from, int to, int label)> const insertEdge =
      ::HashjoinInsertEdge;
  std::function<void(void* sVoid, int fromNode, int toNode, int label)> const deleteEdge =
      ::HashjoinDeleteEdge;
  std::function<int(void* vstore, int label1, int label2, int label3)> const runQuery =
      ::HashjoinRunQuery;
  std::function<void*(unsigned long size)> const allocateDatabase = ::HashjoinAllocateDatabase;
  std::function<void(void* sVoid)> const deleteDatabase = ::HashjoinDeleteDatabase;
};
