#include <cxxabi.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <utility>

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/dijkstra_shortest_paths.hpp"
#include "boost/graph/graph_traits.hpp"
#include "boost/graph/graphviz.hpp"
#include "boost/tuple/tuple.hpp"
#include "fmt/core.h"

int main() {
    using namespace boost;

    typedef property<vertex_name_t, std::string, property<vertex_color_t, float>> vertex_p;
    typedef property<edge_weight_t, double> edge_p;
    typedef property<graph_name_t, std::string> graph_p;

    typedef adjacency_list<vecS, vecS, directedS, vertex_p, edge_p, graph_p> graph_t;

    // Construct an empty graph and prepare the dynamic_property_maps.
    graph_t graph(0);
    dynamic_properties dp;

    property_map<graph_t, vertex_name_t>::type name = get(vertex_name, graph);
    dp.property("node_id", name);

    property_map<graph_t, vertex_color_t>::type mass = get(vertex_color, graph);
    dp.property("mass", mass);

    property_map<graph_t, edge_weight_t>::type weight = get(edge_weight, graph);
    dp.property("weight", weight);

    // Use ref_property_map to turn a graph property into a property map
    boost::ref_property_map<graph_t *, std::string> gname(get_property(graph, graph_name));
    dp.property("name", gname);

    // Sample graph as an std::istream;
    // std::istringstream gvgraph("digraph { [name=\"graphname\"]  a  c e [mass = 6.66] }");
    std::istringstream gvgraph(R"--(
digraph mygraph {
  node [];
  "//diagnose:spectrum_data_test (eb5b4fd)"
  "//diagnose:spectrum_data_test (eb5b4fd)" -> "//diagnose:spectrum_data (4fb1f77)"
  "//diagnose:read_example (4fb1f77)"
  "//diagnose:read_example (4fb1f77)" -> "//utility:proto_text_format (4fb1f77)"
  "//diagnose:diagnose (4fb1f77)"
  "//diagnose:diagnose (4fb1f77)" -> "//diagnose:entities (4fb1f77)"
  "//diagnose:diagnose (4fb1f77)" -> "//diagnose:irreps (4fb1f77)"
  "//diagnose:diagnose (4fb1f77)" -> "//diagnose:k_path (4fb1f77)"
  "//diagnose:diagnose (4fb1f77)" -> "//diagnose:latexify (4fb1f77)"
  "//diagnose:diagnose (4fb1f77)" -> "//diagnose:physics_and_chemistry (4fb1f77)"
  "//diagnose:diagnose (4fb1f77)" -> "//diagnose:spectrum_data (4fb1f77)"
  "//diagnose:diagnose (4fb1f77)" -> "//diagnose:visualize (4fb1f77)"
  "//diagnose:visualize (4fb1f77)"
  "//diagnose:visualize (4fb1f77)" -> "//diagnose:entities (4fb1f77)"
  "//diagnose:visualize (4fb1f77)" -> "//diagnose:latexify (4fb1f77)"
  "//diagnose:visualize (4fb1f77)" -> "//diagnose:spectrum_data (4fb1f77)"
  "//diagnose:visualize (4fb1f77)" -> "//utility:proto_text_format (4fb1f77)"
  "//utility:proto_text_format (4fb1f77)"
  "//diagnose:latexify (4fb1f77)"
  "//diagnose:latexify (4fb1f77)" -> "//diagnose:physics_and_chemistry (4fb1f77)"
  "//diagnose:latexify (4fb1f77)" -> "//diagnose:spectrum_data (4fb1f77)"
  "//diagnose:physics_and_chemistry (4fb1f77)"
  "//diagnose:physics_and_chemistry (4fb1f77)" -> "//diagnose:irreps (4fb1f77)"
  "//diagnose:physics_and_chemistry (4fb1f77)" -> "//diagnose:spectrum_data (4fb1f77)"
  "//diagnose:k_path (4fb1f77)"
  "//diagnose:k_path (4fb1f77)" -> "//diagnose:spectrum_data (4fb1f77)"
  "//diagnose:irreps (4fb1f77)"
  "//diagnose:irreps (4fb1f77)" -> "//utility:comparable (4fb1f77)"
  "//diagnose:entities (4fb1f77)"
  "//diagnose:entities (4fb1f77)" -> "//diagnose:spectrum_data (4fb1f77)"
  "//diagnose:spectrum_data (4fb1f77)"
  "//diagnose:spectrum_data (4fb1f77)" -> "//utility:comparable (4fb1f77)"
  "//utility:comparable (4fb1f77)"
}

    )--");

    bool status = read_graphviz(gvgraph, graph, dp, "node_id");
    fmt::print("{}\n", status);
    for (const auto &v : boost::make_iterator_range(boost::vertices(graph))) {
        fmt::print("{}\n", v);
        fmt::print("{}\n", name[v]);
    }
}
