#+TITLE: Advanced Databases 2020 -- Coursework 1
#+DATE: 
#+OPTIONS: ':nil *:t -:t ::t <:t H:2 \n:nil ^:nil arch:headline author:nil c:nil creator:nil
#+OPTIONS: email:nil f:t inline:t num:nil p:nil pri:nil stat:t tags:t
#+OPTIONS: tasks:t tex:t timestamp:nil toc:nil todo:t |:t
#+LaTeX_HEADER: \usepackage{fullpage}


The objective of this coursework is to practice the interplay between storage
and processing in the context of a not-quite relational application: counting
triangles in a graph.

* Triangle counting
Triangle counting is one of the core challenges in structural graph
analytics. The idea is to find the number of sets of nodes that are connected in
a triangle. For this coursework, we make the task slightly more
interesting by considering a directed, labeled graph (see Figure).
#+attr_latex: :width .66\textwidth
[[./TrianglesVisualized.png]]

In this graph, we might be looking for triangles with the edge label sequence
~0, 1, 2~ and find the triangles ~1, 5, 6~ and ~4, 1, 3~.

Depending on the "connectedness" (the number of edges divided by the number of
vertices in the graph), this problem can be more or less challenging (we will
use 32 as a connectedness factor).

** The relational view
While this problem could be considered a graph problem (or sparse linear
algebra), it is a join problem as well (three joins, actually). Here is a
representation of the problem in SQL

Create the graph table:
 #+begin_src sql :exports code
create table Edges(from int, to int, label int);
 #+end_src

Insert some edges:
 #+begin_src sql :exports code
insert into Edges values(0,1,1);
insert into Edges values(4,1,0);
insert into Edges values(1,2,0);
insert into Edges values(1,3,1);
insert into Edges values(3,2,2);
insert into Edges values(1,5,0);
insert into Edges values(1,6,2);
insert into Edges values(5,6,1);
insert into Edges values(3,4,2);
insert into Edges values(2,7,1);
insert into Edges values(7,2,2);
insert into Edges values(3,7,1);
insert into Edges values(3,8,2);
insert into Edges values(8,4,1);
insert into Edges values(4,7,2);
 #+end_src

The triangle query would then be
 #+begin_src sql :exports code
   select from Edges as firstEdge, Edges as secondEdge, Edges as thirdEdge
   where const firstEdge.to = secondEdnge.from and firstEdge.label = 0
   and secondEdnge.to = thirdEdge.from and secondEdge.label = 1
   and thirdEdge.to = firstEdge.from and thirdEdge.label = 2;
 #+end_src

** Modifications
To make it even more interesting, you will have to deal with a graph that is
being modified between the counting of the triangles. Specifically, we insert
the data in batches of 1/8th of the overall dataset, count the triangles, delete
some edges and repeat.

* Your task

Your task is to implement the insertion, querying and deletion of the graph in C
using both, a sort-merge join as well as a hashjoin and compare the two. For
that purpose, you need to implement 10 functions in the ~Source/ShapeCount.c~
file (do not change any other file). Here are the functions for the sort-merge
join implementation (the hashjoin has the same signature but the functions have
a different prefix).
#+begin_src C++ :exports code :tangle yes :main no :cache no 
typedef void* SortMergeJoinDatabase;
SortMergeJoinDatabase SortMergeJoinAllocateDatabase(unsigned long totalNumberOfEdgesInTheEnd);
void SortMergeJoinInsertEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID,
                             int edgeLabel);
int SortMergeJoinRunQuery(SortMergeJoinDatabase database, int edgeLabel1, int edgeLabel2,
                          int edgeLabel3);
void SortMergeJoinDeleteEdge(SortMergeJoinDatabase database, int fromNodeID, int toNodeID,
                             int edgeLabel);
void SortMergeJoinDeleteDatabase(SortMergeJoinDatabase database);
#+end_src

The interface should be largely self-explanatory. The only peculiarity is the
way we handle the database: you are free to allocate any data structure in the
~...AllocateDatabase~-function. We pass it around as an untyped (i.e., ~void~
pointer). Each of the other functions receives the database pointer as its first
parameter. You probably want to cast it to whatever datastructure you use in
your code.

For the implementation, you are free to build the index structures
(hashtables/sorted arrays) at query or insert time.

Naturally, you will need to de-allocate the database (in the ~...DeleteDatabase~
function). Any data you store "on the side", i.e., do not deallocate in the
delete function will be counted as leaked.

The file ~ShapeCount.c~ contains stubs for the functions.

* Getting started

To get started log in to a lab machine and run the following sequence of commands:
#+BEGIN_SRC bash
git clone ${your repository URL}
cd ${your repository name}
#+END_SRC

You may want to set up two separate build directories for the code,
one for debugging and one for benchmarking. Here is how you could do
that:

#+begin_src bash :exports code
mkdir Debug
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cd ..
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
cd ..
#+end_src

You can compile each by (respectively) typing:
#+begin_src bash :exports code
cmake --build Debug
#+end_src

or 

#+begin_src bash :exports code
cmake --build Release
#+end_src

Note that the first time you build each of these will take a long time
since it also builds dependencies.

** Testing

To run the tests, simply run
 #+begin_src bash :exports code
 ./Debug/tests
 #+end_src

a successful run output should look like this (pass -? for more options)
#+begin_src bash :exports code
===============================================================================
All tests passed (30 assertions in 3 test cases)
#+end_src

** Benchmarking

 To run the benchmarks, simply run
 #+begin_src bash :exports code
 ./Release/Benchmarks
 #+end_src

if you want to restrict the benchmarks that are being run you can use, for example

 #+begin_src bash :exports code
 ./Release/Benchmarks --benchmark_filter='GraphQueryBenchmark<HashjoinImplementation>/64/32'
 #+end_src

(64 is the number of nodes in the graph, 32 the average number of edges)

 #+begin_src bash :exports code
./Benchmarks --benchmark_list_tests
 #+end_src

gives you a name of experiments (try ~./Benchmarks --help~ for more options).


For your submission, *you need to provide a brief (no more than 500 words)
interpretation/explanation of the performance results*. Which algorithm
performed better and why? Which scales better and why?

* Submission

The coursework will be submitted using LabTS. Important are two
files:

- ~Source/ShapeCount.c~, containing *all* code of your solution. No other files
  shall be modified!
- ~Explanation.txt~, explaining the performance of your solution.

* Marking
The marks are distributed as follows:

- Correct & leak-free implementation of the sort-merge-join implementation (note
  that passing tests are required but not necessarily sufficient for a correct
  implementation): 40%
- Correct & leak-free implementation of the hash-join implementation: 30%
- Reasonable performance (no silly performance bugs, unnecessary function
  pointers, etc.): 10%
- Clean code with appropriate documentation: 10%
- Interpretation/explanation of the performance of your implementation: 10%


* Competition -- Sponsored by Snowflake

In addition to, but completely unrelated to, the coursework, we are having a
competition. For that, we will run your implementation on a "big" graph: half a
million vertices, 16 million edges. We are running your implementation on a
~vertex~ machine that has been dedicated to this exercise (you can perform
optimizations on any other vertex machine if you like). Every time you trigger a
test on labts, your solution is tested, benchmarked and uploaded to the
leaderboard (assuming it finishes within a competitive time). The leaderboard
can be accessed at [[http://dbtitans.lsds.uk]].

The winner is the team with the fastest solution in the "Competition"-prefixed
implementation in the submission. However, the top three performing teams are
subject to "code-quality boosting": I will deduct up to 5% from their runtime
for having nice, readable code (my discretion applies).
