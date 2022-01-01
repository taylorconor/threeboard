#pragma once

#include "src/layers/layer_id.h"

namespace threeboard {

// An interface that allows Layer implementations to indicate to the
// LayerController that the current layer should be changed.
class LayerControllerDelegate {
 public:
  virtual bool SwitchToLayer(const LayerId &) = 0;

 protected:
  virtual ~LayerControllerDelegate() = default;
};
}  // namespace threeboard
