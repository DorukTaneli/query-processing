#include "../Source/ShapeCount.hpp"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <functional>
using namespace std;

TEMPLATE_TEST_CASE("Dummy", "", SortMergeJoinImplementation, HashjoinImplementation, CompetitionImplementation) {
  // using Implementation = SortMergeImplementation;
  TestType const implementation;
  auto* db = implementation.allocateDatabase(32);

  //OUR TESTS:
  SECTION("Test insert edge") {
    implementation.insertEdge(db, 0, 1, 2);
    implementation.insertEdge(db, 3, 3, 3);
    implementation.insertEdge(db, 5, 6, 7);
    implementation.deleteEdge(db, 3, 3, 3);
    REQUIRE(implementation.findEdge(db, 0, 1, 2) == 2);
    REQUIRE(implementation.findEdge(db, 0, 2, 3) == -1);
    REQUIRE(implementation.findEdge(db, 3, 3, 3) == -1);
    REQUIRE(implementation.findEdge(db, 5, 6, 7) == 7);
    REQUIRE(implementation.findEdge(db, 5, 5, 7) == -1);

  }

  SECTION("Test valids") {
    implementation.insertEdge(db, 0, 1, 0);
    implementation.insertEdge(db, 1, 2, 1);
    implementation.insertEdge(db, 2, 0, 2);
  
    implementation.insertEdge(db, 0, 3, 0);
    implementation.insertEdge(db, 3, 2, 1);
    implementation.insertEdge(db, 2, 5, 2);
    REQUIRE(implementation.runQuery(db, 0, 1, 2) == 2);
  }

  SECTION("Test non ending triangle") {
    implementation.insertEdge(db,0,1,0);
    implementation.insertEdge(db,1,2,1);
    implementation.insertEdge(db,2,9,2);
    REQUIRE(implementation.runQuery(db, 0, 1, 2) == 0);
  }



  //END OF OUR TESTS

  SECTION("Single, self-isomorphic triangle") {
    implementation.insertEdge(db, 0, 1, 0);
    implementation.insertEdge(db, 1, 2, 0);
    implementation.insertEdge(db, 2, 0, 0);
    REQUIRE(implementation.runQuery(db, 0, 0, 0) == 3);
    REQUIRE(implementation.runQuery(db, 0, 1, 0) == 0);
  }
  SECTION("Single, non-self-isomorphic triangle") {
    implementation.insertEdge(db, 0, 1, 0);
    implementation.insertEdge(db, 1, 2, 1);
    implementation.insertEdge(db, 2, 0, 2);
    REQUIRE(implementation.runQuery(db, 0, 1, 2) == 1);
    REQUIRE(implementation.runQuery(db, 0, 0, 0) == 0);
  }
  SECTION("Multiple, non-self-isomorphic triangle") {
    implementation.insertEdge(db, 0, 1, 0);
    implementation.insertEdge(db, 0, 2, 0);
    implementation.insertEdge(db, 0, 3, 0);
    implementation.insertEdge(db, 1, 3, 1);
    implementation.insertEdge(db, 1, 4, 1);
    implementation.insertEdge(db, 1, 2, 1);
    implementation.insertEdge(db, 3, 4, 1);
    implementation.insertEdge(db, 3, 2, 1);
    implementation.insertEdge(db, 2, 0, 2);
    implementation.insertEdge(db, 4, 0, 2);
    implementation.insertEdge(db, 3, 0, 2);
    REQUIRE(implementation.runQuery(db, 0, 1, 2) == 5);
  }
  SECTION("Multiple, deleted edge") {
    implementation.insertEdge(db, 0, 1, 0);
    implementation.insertEdge(db, 0, 2, 0);
    implementation.insertEdge(db, 0, 3, 0);
    implementation.insertEdge(db, 1, 3, 1);
    implementation.insertEdge(db, 1, 4, 1);
    implementation.insertEdge(db, 1, 2, 1);
    implementation.insertEdge(db, 3, 4, 1);
    implementation.insertEdge(db, 3, 2, 1);
    implementation.insertEdge(db, 2, 0, 2);
    implementation.insertEdge(db, 4, 0, 2);
    implementation.insertEdge(db, 3, 0, 2);
    implementation.deleteEdge(db, 1, 3, 1);
    implementation.deleteEdge(db, 3, 4, 1);
    REQUIRE(implementation.runQuery(db, 0, 1, 2) == 3);
  }
  SECTION("Multiple, reinserted edge") {
    implementation.insertEdge(db, 0, 1, 0);
    implementation.insertEdge(db, 0, 2, 0);
    implementation.insertEdge(db, 0, 3, 0);
    implementation.insertEdge(db, 1, 3, 1);
    implementation.insertEdge(db, 1, 4, 1);
    implementation.insertEdge(db, 1, 2, 1);
    implementation.insertEdge(db, 3, 4, 1);
    implementation.insertEdge(db, 3, 2, 1);
    implementation.insertEdge(db, 2, 0, 2);
    implementation.deleteEdge(db, 1, 3, 1);
    implementation.deleteEdge(db, 3, 4, 1);
    implementation.insertEdge(db, 4, 0, 2);
    implementation.insertEdge(db, 3, 0, 2);
    implementation.insertEdge(db, 3, 4, 1);
    REQUIRE(implementation.runQuery(db, 0, 1, 2) == 4);
  }
  SECTION("Multiple, grouped edges") {
    implementation.insertEdge(db, 0, 1, 0);
    implementation.insertEdge(db, 0, 1, 1);
    implementation.insertEdge(db, 0, 2, 0);
    implementation.insertEdge(db, 0, 2, 1);
    implementation.insertEdge(db, 0, 3, 0);
    implementation.insertEdge(db, 0, 3, 1);
    implementation.insertEdge(db, 0, 3, 2);
    implementation.insertEdge(db, 1, 3, 1);
    implementation.insertEdge(db, 1, 3, 2);
    implementation.insertEdge(db, 1, 4, 1);
    implementation.insertEdge(db, 1, 4, 2);
    implementation.insertEdge(db, 1, 2, 1);
    implementation.insertEdge(db, 1, 2, 2);
    implementation.insertEdge(db, 3, 4, 1);
    implementation.insertEdge(db, 3, 4, 2);
    implementation.insertEdge(db, 3, 2, 1);
    implementation.insertEdge(db, 3, 2, 2);
    implementation.insertEdge(db, 2, 0, 2);
    implementation.insertEdge(db, 2, 0, 3);
    implementation.deleteEdge(db, 1, 3, 1);
    implementation.deleteEdge(db, 3, 4, 1);
    implementation.insertEdge(db, 4, 0, 2);
    implementation.insertEdge(db, 4, 0, 3);
    implementation.insertEdge(db, 3, 0, 2);
    implementation.insertEdge(db, 3, 0, 3);
    implementation.insertEdge(db, 3, 4, 1);
    implementation.insertEdge(db, 3, 4, 3);
    REQUIRE(implementation.runQuery(db, 0, 1, 2) == 4);
  }
  implementation.deleteDatabase(db);
}

TEST_CASE("Random Graph", "Sort-Merge Join and Hash Join"){
  SortMergeJoinImplementation SMJ;
  HashjoinImplementation HJ;
  CompetitionImplementation CJ;
  
  auto* dbSMJ = SMJ.allocateDatabase(32);
  auto* dbHJ = HJ.allocateDatabase(32);
  auto* dbCJ = CJ.allocateDatabase(32);


  for (auto i = 0; i < 32; i++){
    auto src = rand() % 10;
    auto dst = rand() % 10;
    auto label = rand() % 10;
    while(src == dst){
      dst = rand() % 10;
    }

    SMJ.insertEdge(dbSMJ, src, dst, label);
    HJ.insertEdge(dbHJ, src, dst, label);
    CJ.insertEdge(dbCJ, src, dst, label);
  }
  
  auto label1 = rand() % 10;
  auto label2 = rand() % 10;
  auto label3 = rand() % 10;
  REQUIRE(SMJ.runQuery(dbSMJ, label1, label2, label3) == HJ.runQuery(dbHJ, label1, label2, label3));
  REQUIRE(SMJ.runQuery(dbSMJ, label1, label2, label3) == CJ.runQuery(dbCJ, label1, label2, label3));
}
