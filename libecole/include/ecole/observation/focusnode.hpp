#pragma once

#include <memory>
#include <optional>

#include "ecole/observation/abstract.hpp"

namespace ecole::observation {

class FocusNodeObs {
public:
	long long number;
	int depth;
	double lowerbound;
	double estimate;
	int n_added_conss;
	int n_vars;
	int nlpcands;
	int npseudocands;
	long long parent_number;
	double parent_lowerbound;
};

class FocusNode : public ObservationFunction<std::optional<FocusNodeObs>> {
public:
	using Observation = std::optional<FocusNodeObs>;
	Observation extract(scip::Model& model, bool done) override;
};

}  // namespace ecole::observation
