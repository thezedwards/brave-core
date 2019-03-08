/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_page_graph/graph_item/edge/edge_storage_delete.h"
#include <string>
#include "brave/components/brave_page_graph/graph_item/edge/edge_storage.h"
#include "brave/components/brave_page_graph/graph_item/node.h"
#include "brave/components/brave_page_graph/types.h"

using ::std::string;

namespace brave_page_graph {

EdgeStorageDelete::EdgeStorageDelete(const PageGraphId id,
    const Node* in_node, const Node* out_node, const string& key) :
    EdgeStorage(id, in_node, out_node, key) {}

EdgeStorageDelete::~EdgeStorageDelete() {}

string EdgeStorageDelete::ItemName() const {
  return "EdgeStorageDelete#" + ::std::to_string(id_);
}

}  // brave_page_graph
