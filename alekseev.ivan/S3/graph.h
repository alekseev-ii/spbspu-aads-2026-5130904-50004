#ifndef GRAPH_H
#define GRAPH_H

#include "../common/hash_table.h"
#include "../common/vector.h"

namespace alekseev {
  using str = std::string;
  size_t str_hasher(const str & name);
  size_t hasher(const std::pair< str, str > & key);
  using hasher_ptr = size_t(*)(const std::pair< str, str > &);
  bool is_equal(const std::pair< str, str > & lhs, const std::pair< str, str > & rhs);
  using is_equal_ptr = bool(*)(const std::pair< str, str > &, const std::pair< str, str > &);

  struct Graph {
    ~Graph();
    Graph(const Graph & rhs);
    Graph & operator=(const Graph & rhs);
    Graph(Graph && rhs) noexcept;
    Graph & operator=(Graph && rhs) noexcept;
    Graph();

    void swap(Graph & rhs) noexcept;

    void ins_vertex(const str & vertex);
    void add_vertex(const str & vertex);
    void add_edge(const str & vertex1, const str & vertex2, size_t weight);
    void ins_edge(const str & vertex1, const str & vertex2, size_t weight);
    void add_edges(const str & vertex1, const str & vertex2, Vector< size_t > weights);
    bool has_vertex(const str & vertex) const;
    bool has_some_edge(const str & vertex1, const str & vertex2) const;
    bool has_edge(const str & vertex1, const str & vertex2, size_t weight) const;
    void remove_vertex(const str & vertex);
    void remove_edge(const str & vertex1, const str & vertex2, size_t weight);
    Vector< std::pair< str, Vector< size_t > > > outbounds(const str & vertex) const;
    Vector< std::pair< str, Vector< size_t > > > inbounds(const str & vertex) const;
    List< str > * vertexes() const;

    private:
      List< str > * vertexes_;
      HashTable< std::pair< str, str >, Vector< size_t >, hasher_ptr, is_equal_ptr > edges_;
  };

  Graph merge_graphs(const Graph & graph1, const Graph & graph2);
  Graph extract_graph(const Graph & source, const List< str > * vertexes);
}

#endif
