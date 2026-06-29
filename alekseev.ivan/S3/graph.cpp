#include "graph.h"
#include <cstring>
#include <boost/hash2/hash_append.hpp>
#include <boost/hash2/sha1.hpp>
#include <boost/hash2/flavor.hpp>
#include <boost/hash2/get_integral_result.hpp>

size_t alekseev::str_hasher(const str & name)
{
  boost::hash2::sha1_160 sha;

  boost::hash2::hash_append(sha, {}, name);
  return boost::hash2::get_integral_result< size_t >(sha);
}

size_t alekseev::hasher(const std::pair< str, str > & key)
{
  return str_hasher(key.first) ^ (str_hasher(key.second) << 1);
}

bool alekseev::is_equal(const std::pair< str, str > & lhs, const std::pair< str, str > & rhs)
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

alekseev::Graph::Graph():
  edges_(hasher, is_equal, 1024)
{ }

void alekseev::Graph::swap(Graph & rhs) noexcept
{
  vertexes_.swap(rhs.vertexes_);
  edges_.swap(rhs.edges_);
}

void alekseev::Graph::ins_vertex(const str & vertex)
{
  for (auto it = vertexes_.begin(); it != vertexes_.end(); ++it) {
    if (*it == vertex) {
      return;
    }
  }
  add_vertex(vertex);
}

void alekseev::Graph::add_vertex(const str & vertex)
{
  vertexes_.push_front(vertex);
}

void alekseev::Graph::add_edge(const str & vertex1, const str & vertex2, size_t weight)
{
  std::pair< str, str > key = std::make_pair(vertex1, vertex2);
  if (!has_some_edge(vertex1, vertex2)) {
    edges_.insert(key, Vector< size_t >());
  }
  edges_.at(key).pushBack(weight);
}

void alekseev::Graph::ins_edge(const str & vertex1, const str & vertex2, size_t weight)
{
  if (!has_vertex(vertex1) || !has_vertex(vertex2)) {
    throw std::invalid_argument("Invalid vertex");
  }
  add_edge(vertex1, vertex2, weight);
}

void alekseev::Graph::add_edges(const str & vertex1, const str & vertex2, Vector< size_t > weights)
{
  for (size_t i = 0; i < weights.size(); ++i) {
    add_edge(vertex1, vertex2, weights[i]);
  }
}

bool alekseev::Graph::has_vertex(const str & vertex) const
{
  bool res = false;
  for (auto it = vertexes_.begin(); it != vertexes_.end() && !res; ++it) {
    res = (*it == vertex);
  }
  return res;
}

bool alekseev::Graph::has_some_edge(const str & vertex1, const str & vertex2) const
{
  return edges_.contains(std::pair< str, str >(vertex1, vertex2));
}

bool alekseev::Graph::has_edge(const str & vertex1, const str & vertex2, size_t weight) const
{
  if (!has_some_edge(vertex1, vertex2)) {
    return false;
  }
  const Vector< size_t > & weights = edges_.at(std::pair< str, str >(vertex1, vertex2));
  bool res = false;
  for (size_t i = 0; i < weights.size() && !res; ++i) {
    res = (weights[i] == weight);
  }
  return res;
}

void alekseev::Graph::remove_vertex(const str & vertex)
{
  auto pre = vertexes_.before_begin();
  auto cur = vertexes_.begin();
  while (cur != vertexes_.end() && *cur != vertex) {
    ++cur;
    ++pre;
  }
  if (cur == vertexes_.end()) {
    return;
  }
  vertexes_.erase_after(pre);
  for (auto it = vertexes_.begin(); it != vertexes_.end();) {
    edges_.remove(std::pair< str, str >(vertex, *it));
    edges_.remove(std::pair< str, str >(*it, vertex));
  }
  edges_.remove(std::pair< str, str >(vertex, vertex));
}

void alekseev::Graph::remove_edge(const str & vertex1, const str & vertex2, size_t weight)
{
  Vector< size_t > & edges = edges_.at(std::make_pair(vertex1, vertex2));
  size_t id = 0;
  while (id < edges.size()) {
    if (edges[id] == weight) {
      break;
    }
    ++id;
  }
  if (id < edges.size()) {
    edges.erase(id);
    if (edges.empty()) {
      edges_.remove(std::pair< str, str >(vertex1, vertex2));
    }
  }
}

alekseev::Vector< std::pair< alekseev::str, alekseev::Vector< size_t > > >
alekseev::Graph::outbounds(const str & vertex) const
{
  Vector< std::pair< str, Vector< size_t > > > res;
  for (auto it = vertexes_.begin(); it != vertexes_.end(); ++it) {
    std::pair< str, str > edge(vertex, *it);
    if (edges_.contains(edge)) {
      res.pushBack(std::make_pair(*it, edges_.at(edge)));
    }
  }
  return res;
}

alekseev::Vector< std::pair< std::string, alekseev::Vector< size_t > > >
alekseev::Graph::inbounds(const str & vertex) const
{
  Vector< std::pair< str, Vector< size_t > > > res;
  for (auto it = vertexes_.begin(); it != vertexes_.end(); ++it) {
    std::pair< str, str > edge(*it, vertex);
    if (edges_.contains(edge)) {
      res.pushBack(std::make_pair(*it, edges_.at(edge)));
    }
  }
  return res;
}

const alekseev::List< std::string > & alekseev::Graph::vertexes() const
{
  return vertexes_;
}

alekseev::Graph alekseev::merge_graphs(const Graph & graph1, const Graph & graph2)
{
  Graph merged = graph1;
  const List< str > & vertexes2 = graph2.vertexes();
  auto current2 = vertexes2.begin();
  for (; current2 != vertexes2.end(); ++current2) {
    merged.ins_vertex(*current2);
  }

  current2 = vertexes2.begin();
  for (; current2 != vertexes2.end(); ++current2) {
    Vector< std::pair< str, Vector< size_t > > > inbounds = graph2.inbounds(*current2);
    for (size_t i = 0; i < inbounds.size(); ++i) {
      merged.add_edges(inbounds[i].first, *current2, inbounds[i].second);
    }
  }
  return merged;
}

alekseev::Graph alekseev::extract_graph(const Graph & source, const List< str > & vertexes)
{
  Graph extracted;
  for (auto current_vertex = vertexes.begin(); current_vertex != vertexes.end(); ++current_vertex) {
    if (source.has_vertex(*current_vertex) && !extracted.has_vertex(*current_vertex)) {
      extracted.add_vertex(*current_vertex);
    } else {
      throw std::invalid_argument("No such vertex or repeated vertex");
    }
  }
  for (auto current_vertex = vertexes.begin(); current_vertex != vertexes.end(); ++current_vertex) {
    Vector< std::pair< str, Vector< size_t > > > inbounds = source.inbounds(*current_vertex);
    for (size_t i = 0; i < inbounds.size(); ++i) {
      if (extracted.has_vertex(inbounds[i].first)) {
        extracted.add_edges(inbounds[i].first, *current_vertex, inbounds[i].second);
      }
    }
  }
  return extracted;
}
