#include <iostream>
#include <fstream>
#include <string>
#include "hash_table.h"
#include "graph.h"

namespace alekseev {
  using str = std::string;
  using Ht_Graphs = HashTable< str, Graph, size_t (*)(const str &), bool (*)(str, str) >;
  Ht_Graphs input_graphs(std::ifstream & input);
  bool str_less(const str & a, const str & b);
  Vector< str > split(const str & s, char delim = ' ');

  using command_type = void(*)(Ht_Graphs &, const Vector< str > &);
  void graphs(Ht_Graphs & graphs, const Vector< str > &);
  void vertexes(Ht_Graphs & graphs, const Vector< str > & args);
  void bounds(Ht_Graphs & graphs, const Vector< str > & args, bool out);
  void outbound(Ht_Graphs & graphs, const Vector< str > & args);
  void inbound(Ht_Graphs & graphs, const Vector< str > & args);

  void bind(Ht_Graphs & graphs, const Vector< str > & args);
  void cut(Ht_Graphs & graphs, const Vector< str > & args);

  void create(Ht_Graphs & graphs, const Vector< str > & args);
  void merge(Ht_Graphs & graphs, const Vector< str > & args);
  void extract(Ht_Graphs & graphs, const Vector< str > & args);

  struct Exec
  {
    Exec();
    HashTable< str, command_type, size_t (*)(const str &), bool (*)(const str &, const str &) >
    cmds;

    void operator()(Ht_Graphs & graphs, Vector< str > words);
  };
}

int main(int argc, char * argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }
  std::ifstream input(argv[1]);
  if (!input.is_open()) {
    std::cerr << "Error opening file " << argv[1] << "\n";
    return 1;
  }
  alekseev::Ht_Graphs graphs(alekseev::str_hasher, [](std::string s1, std::string s2) {
    return s1 == s2;
  }, 1);
  try {
    graphs = alekseev::input_graphs(input);
  } catch (...) {
    std::cerr << "Error while read file" << "\n";
    input.close();
    return 1;
  }
  input.close();
  std::string command;
  alekseev::Exec exec;
  while (std::getline(std::cin, command)) {
    try {
      alekseev::Vector< std::string > words = alekseev::split(command, ' ');
      exec(graphs, words);
    } catch (std::invalid_argument & e) {
      std::cout << "<INVALID COMMAND>" << "\n";
    } catch (std::exception & e) {
      std::cerr << e.what() << "\n";
      return 1;
    }
  }
  if (!std::cin.eof()) {
    std::cerr << "Input fail" << "\n";
    return 1;
  }
}

alekseev::Ht_Graphs alekseev::input_graphs(std::ifstream & input)
{
  Ht_Graphs ht(str_hasher, [](str s1, str s2) {
    return s1 == s2;
  }, 128);

  std::string line;
  size_t number = 0;
  std::string name;
  Graph current;
  bool has_graph = false;

  while (std::getline(input, line)) {
    if (line.empty()) {
      continue;
    }
    Vector< str > words = split(line, ' ');
    if (words.size() == 3 && number > 0) {
      --number;
      current.ins_vertex(words[0]);
      current.ins_vertex(words[1]);
      current.add_edge(words[0], words[1], std::stoull(words[2]));
      if (number == 0) {
        ht.insert(name, current);
        current = Graph();
        has_graph = false;
      }
    } else if (words.size() == 2 && number == 0 && !has_graph) {
      name = words[0];
      number = stoull(words[1]);
      has_graph = true;
      if (number == 0) {
        ht.insert(name, current);
        has_graph = false;
      }
    } else {
      throw std::invalid_argument("invalid input");
    }
  }
  return ht;
}

bool alekseev::str_less(const str & a, const str & b)
{
  return a < b;
}

alekseev::Vector< std::string > alekseev::split(const str & s, char delim)
{
  size_t start = 0;
  Vector< str > res;
  size_t i = 0;
  for (; i < s.size(); ++i) {
    if (s[i] == delim) {
      if (i > start) {
        res.pushBack(s.substr(start, i - start));
      }
      start = i + 1;
    }
  }
  res.pushBack(s.substr(start, i - start));
  return res;
}

void alekseev::graphs(Ht_Graphs & graphs, const Vector< str > &)
{
  Vector< str > names = graphs.keys();
  if (names.empty()) {
    std::cout << "\n";
    return;
  }
  names.bubbleSort(str_less);
  for (size_t i = 0; i < names.size(); ++i) {
    std::cout << names[i] << "\n";
  }
}

void alekseev::vertexes(Ht_Graphs & graphs, const Vector< str > & args)
{
  if (args.empty()) {
    throw std::invalid_argument("Invalid arguments");
  }
  if (!graphs.contains(args[0])) {
    throw std::invalid_argument("Invalid arguments");
  }
  Vector< str > vect_names;
  const List< str > & names = graphs.at(args[0]).vertexes();
  for (auto it = names.begin(); it != names.end(); ++it) {
    vect_names.pushBack(*it);
  }
  if (vect_names.empty()) {
    std::cout << "\n";
    return;
  }
  vect_names.bubbleSort(str_less);
  for (size_t i = 0; i < vect_names.size(); ++i) {
    std::cout << vect_names[i] << "\n";
  }
}

void alekseev::bounds(Ht_Graphs & graphs, const Vector< str > & args, bool out)
{
  if (args.size() != 2) {
    throw std::invalid_argument("Invalid arguments");
  }
  if (!graphs.contains(args[0])) {
    throw std::invalid_argument("Invalid arguments");
  }
  const Graph & graph = graphs.at(args[0]);
  if (!graph.has_vertex(args[1])) {
    throw std::invalid_argument("Invalid arguments");
  }
  Vector< std::pair< str, Vector< size_t > > > edges;
  if (out) {
    edges = graph.outbounds(args[1]);
  } else {
    edges = graph.inbounds(args[1]);
  }
  edges.bubbleSort(
      [](const std::pair< str, Vector< size_t > > & p1,
      const std::pair< str, Vector< size_t > > & p2) {
        return str_less(p1.first, p2.first);
      });
  for (size_t i = 0; i < edges.size(); ++i) {
    Vector< size_t > & weights = edges[i].second;
    std::cout << edges[i].first;

    weights.bubbleSort([](size_t a, size_t b) {
      return a < b;
    });
    for (size_t j = 0; j < weights.size(); ++j) {
      std::cout << " " << weights[j];
    }
    std::cout << "\n";
  }
  if (edges.empty()) {
    std::cout << "\n";
  }
}

void alekseev::outbound(Ht_Graphs & graphs, const Vector< str > & args)
{
  bounds(graphs, args, true);
}

void alekseev::inbound(Ht_Graphs & graphs, const Vector< str > & args)
{
  bounds(graphs, args, false);
}

void alekseev::bind(Ht_Graphs & graphs, const Vector< str > & args)
{
  if (args.size() != 4) {
    throw std::invalid_argument("Invalid arguments");
  }
  if (!graphs.contains(args[0])) {
    throw std::invalid_argument("Invalid arguments");
  }
  size_t weight = std::stoull(args[3]);

  Graph & graph = graphs.at(args[0]);
  bool added1 = false, added2 = false;
  try {
    if (!graph.has_vertex(args[1])) {
      graph.add_vertex(args[1]);
      added1 = true;
    }
    if (!graph.has_vertex(args[2])) {
      graph.add_vertex(args[2]);
      added2 = true;
    }
    graph.add_edge(args[1], args[2], weight);
  } catch (...) {
    if (added1) {
      graph.remove_vertex(args[1]);
    }
    if (added2) {
      graph.remove_vertex(args[2]);
    }
  }
}

void alekseev::cut(Ht_Graphs & graphs, const Vector< str > & args)
{
  if (args.size() != 4) {
    throw std::invalid_argument("Invalid arguments");
  }
  if (!graphs.contains(args[0])) {
    throw std::invalid_argument("Invalid arguments");
  }
  size_t weight = std::stoull(args[3]);
  Graph & graph = graphs.at(args[0]);
  if (!graph.has_vertex(args[1]) || !graph.has_vertex(args[2]) || !graph.has_edge(args[1], args[2],
      weight)) {
    throw std::invalid_argument("Invalid arguments");
  }
  graph.remove_edge(args[1], args[2], weight);
}

void alekseev::create(Ht_Graphs & graphs, const Vector< str > & args)
{
  if (args.empty()) {
    throw std::invalid_argument("Invalid arguments");
  }
  if (graphs.contains(args[0])) {
    throw std::invalid_argument("Invalid arguments");
  }
  Graph graph;
  if (args.size() > 1) {
    if (args.size() != std::stoull(args[1]) + 2) {
      throw std::invalid_argument("Invalid arguments");
    }
    for (size_t i = 2; i < args.size(); ++i) {
      graph.add_vertex(args[i]);
    }
  }
  graphs.insert(args[0], graph);
}

void alekseev::merge(Ht_Graphs & graphs, const Vector< str > & args)
{
  if (args.size() != 3) {
    throw std::invalid_argument("Invalid arguments");
  }
  if (graphs.contains(args[0]) || !graphs.contains(args[1]) || !graphs.contains(args[2])) {
    throw std::invalid_argument("Invalid arguments");
  }
  Graph new_graph = merge_graphs(graphs.at(args[1]), graphs.at(args[2]));
  graphs.insert(args[0], new_graph);
}

void alekseev::extract(Ht_Graphs & graphs, const Vector< str > & args)
{
  if (args.size() < 3) {
    throw std::invalid_argument("Invalid arguments");
  }
  if (graphs.contains(args[0]) || !graphs.contains(args[1])) {
    throw std::invalid_argument("Invalid arguments");
  }

  Graph graph;
  size_t number = std::stoull(args[2]);
  if (number > 0 && args.size() == number + 3) {
    List< str > vertexes;
    for (size_t i = 3; i < args.size(); ++i) {
      vertexes.push_front(args[i]);
    }
    graph = extract_graph(graphs.at(args[1]), vertexes);
  }
  graphs.insert(args[0], graph);
}

alekseev::Exec::Exec():
  cmds(str_hasher, [](const str & s1, const str & s2) {
    return s1 == s2;
  }, 16)
{
  cmds.insert("graphs", graphs);
  cmds.insert("vertexes", vertexes);
  cmds.insert("outbound", outbound);
  cmds.insert("inbound", inbound);
  cmds.insert("bind", bind);
  cmds.insert("cut", cut);
  cmds.insert("create", create);
  cmds.insert("merge", merge);
  cmds.insert("extract", extract);
}

void alekseev::Exec::operator()(Ht_Graphs & graphs, Vector< str > words)
{
  if (words.empty()) {
    throw std::invalid_argument("Invalid arguments");
  }
  if (!cmds.contains(words[0])) {
    throw std::invalid_argument("Invalid arguments");
  }
  Vector< str > args;
  args.insert(0, words, 1, words.size());
  cmds.at(words[0])(graphs, args);
}
