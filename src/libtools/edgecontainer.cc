/*
 *            Copyright 2009-2018 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <set>
#include <vector>
#include <exception>
#include <votca/tools/edge.h>
#include <votca/tools/edgecontainer.h>
#include <algorithm>
#include <boost/lexical_cast.hpp>

using namespace votca::tools;
using namespace std;
using namespace boost;

EdgeContainer::EdgeContainer(Edge ed) { addEdge(ed); }

EdgeContainer::EdgeContainer(vector<Edge> eds) {
  for (auto ed : eds) {
    addEdge(ed);
  }
}

int EdgeContainer::getDegree(int vert){
  if(!adj_list_.count(vert)) throw invalid_argument("vertex is not defined");
  return lexical_cast<int>(adj_list_[vert].size());
}

bool EdgeContainer::edgeExist(Edge ed) {
  return (find(adj_list_[ed.getV1()].begin(), adj_list_[ed.getV1()].end(),
               ed.getV2()) != adj_list_[ed.getV1()].end());
}

bool EdgeContainer::vertexExist(int vert) { return adj_list_.count(vert); }

void EdgeContainer::addEdge(Edge ed) {
  adj_list_[ed.getV1()].insert(ed.getV2());
  adj_list_[ed.getV2()].insert(ed.getV1());
  return;
}

vector<int> EdgeContainer::getVertices() {
  vector<int> vertices;
  for (auto const& it : adj_list_) vertices.push_back(it.first);
  return vertices;
}

vector<int> EdgeContainer::getNeighVertices(int vert) {
  vector<int> neigh_verts;
  for (auto const& neigh_vert : adj_list_[vert]) {
    neigh_verts.push_back(neigh_vert);
  }
  return neigh_verts;
}

vector<Edge> EdgeContainer::getNeighEdges(int vert) {
  vector<Edge> neigh_edges;
  for (auto const& neigh_vert : adj_list_[vert]) {
    neigh_edges.push_back(Edge(vert, neigh_vert));
  }
  return neigh_edges;
}

vector<Edge> EdgeContainer::getEdges() {
  set<Edge> edgs;
  for (auto const& it : adj_list_) {
    for (auto const& vert : it.second) edgs.insert(Edge(it.first, vert));
  }
  vector<Edge> vec_edgs(edgs.begin(), edgs.end());
  return vec_edgs;
}
