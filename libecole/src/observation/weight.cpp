#include <cstddef>
#include <optional>

#include <xtensor/xtensor.hpp>

#include "ecole/observation/weight.hpp"
#include "ecole/scip/model.hpp"
#include "ecole/scip/col.hpp"

namespace ecole::observation {

namespace {

auto get_weight(SCIP_COL* const col) {
  double weight = 0;
  auto const values = scip::get_vals(col);
  for (auto const val : values) {
    weight = (weight < val) ? val : weight;
  }
  return weight;
}

}  // namespace


std::optional<xt::xtensor<double, 1>> Weight::extract(scip::Model& model, bool /* done */) {
  if (model.stage() != SCIP_STAGE_SOLVING) {
		return {};
	}
  auto* const scip = model.get_scip_ptr();
  
  /* Store results in tensor */
  auto const nb_lp_columns = static_cast<std::size_t>(SCIPgetNLPCols(scip));
  xt::xtensor<double, 1> weights({nb_lp_columns}, std::nan(""));

  /* Extract item weight */
  auto const columns = model.lp_columns();
  auto const n_columns = columns.size();
  for (std::size_t col_idx = 0; col_idx < n_columns; ++col_idx) {
    auto* const col = columns[col_idx];
    auto const lp_index = static_cast<std::size_t>(SCIPcolGetLPPos(col));
    auto const weight = get_weight(col);
    weights[lp_index] = static_cast<double>(weight);
   }

  return weights;
  }


}  // namespace ecole::observation
