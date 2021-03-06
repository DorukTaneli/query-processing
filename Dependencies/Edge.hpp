#ifndef EDGE_HPP
#define EDGE_HPP

#include <fstream>

#include "internal_config.hpp"


class Edge{

public:
	~Edge(){}

	EdgeIndexType src, dst;
	Edge(){}
	Edge(EdgeIndexType rec_src, EdgeIndexType rec_dst);
	Edge(const Edge &cSource);

	Edge& operator= (const Edge &cSource);

	// Returns true if the source and the destination vertices of the edge are the same.
	bool selfEdge();

	// For the comparison required by std::sort.
	friend bool operator< (const Edge& cR1, const Edge& cR2);

	friend bool operator== (const Edge& cR1, const Edge& cR2);

	// For printing edges in the output or into the file.
	friend std::ostream& operator<< (std::ostream &out, Edge &cEdge);

};

#endif	// EDGE_HPP
