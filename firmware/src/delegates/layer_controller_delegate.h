#pragma once

#include "src/layers/layer_id.h"

namespace threeboard {

class LayerControllerDelegate {
 public:
  virtual void SwitchToLayer(const LayerId &) = 0;

 protected:
  virtual ~LayerControllerDelegate() = default;
};
}  // namespace threeboard
