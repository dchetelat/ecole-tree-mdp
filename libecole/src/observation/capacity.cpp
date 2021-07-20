#include <cstddef>
#include <optional>

#include <xtensor/xtensor.hpp>

#include "ecole/observation/capacity.hpp"
#include "ecole/scip/model.hpp"
#include "ecole/scip/row.hpp"
#include "ecole/scip/col.hpp"

namespace ecole::observation {

namespace {

auto get_rhs(SCIP_COL* const col) {
  double rhs;
  double capacity = 0;
  auto const rows = scip::get_rows(col);
  for (auto* const row : rows) {
    rhs = SCIProwGetRhs(row);
    capacity = (capacity < rhs) ? rhs : capacity;
  }
  return capacity;
}

}  // namespace


std::optional<xt::xtensor<double, 1>> Capacity::extract(scip::Model& model, bool /* done */) {
  if (model.stage() != SCIP_STAGE_SOLVING) {
		return {};
	}
  auto* const scip = model.get_scip_ptr();

  /* Store results in tensor */
	auto const nb_lp_columns = static_cast<std::size_t>(SCIPgetNLPCols(scip));
	xt::xtensor<double, 1> capacities({nb_lp_columns}, std::nan(""));

  /* Extract knapsack capacity */
  auto const columns = model.lp_columns();
  auto const n_columns = columns.size();
  for (std::size_t col_idx = 0; col_idx < n_columns; ++col_idx) {
    auto* const col = columns[col_idx];
    auto const lp_index = static_cast<std::size_t>(SCIPcolGetLPPos(col));
    auto const rhs = get_rhs(col);
    capacities[lp_index] = static_cast<double>(rhs);
    }

  return capacities;
  }


}  // namespace ecole::observation
