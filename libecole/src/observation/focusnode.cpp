#include <array>
#include <cstddef>
#include <limits>

#include "ecole/observation/focusnode.hpp"
#include "ecole/scip/model.hpp"
#include "ecole/scip/type.hpp"

namespace ecole::observation {

std::optional<FocusNodeObs> FocusNode::extract(scip::Model& model, bool /* done */) {

	if (model.stage() == SCIP_STAGE_SOLVING) {
		SCIP* scip = model.get_scip_ptr();
		SCIP_NODE* node = SCIPgetFocusNode(scip);

		long long number = SCIPnodeGetNumber(node) - 1;
		int depth = SCIPnodeGetDepth(node);
		double lowerbound = SCIPnodeGetLowerbound(node);
		double estimate = SCIPnodeGetEstimate(node);
		int n_added_conss = SCIPnodeGetNAddedConss(node);
		int n_vars = SCIPgetNVars(scip);

		// LP candidates
		int nlpcands;
		int npriolpcands;
		int nfracimplvars;
		SCIP_VAR** lpcands;
		SCIP_Real* lpcandssol;
		SCIP_Real* lpcandsfrac;
		SCIPgetLPBranchCands(scip, &lpcands, &lpcandssol, &lpcandsfrac,
															 &nlpcands, &npriolpcands, &nfracimplvars);

		// Pseudo candidates
		int npseudocands;
		int npriocands;
		SCIP_VAR** cands;
		SCIPgetPseudoBranchCands(scip, &cands, &npseudocands, &npriocands);

		// Parent node
		long long parent_number;
		double parent_lowerbound;
		if (number == 0) {
			// Root node
			parent_number = -1;
			parent_lowerbound = lowerbound;
		} else {
			SCIP_NODE* parent = SCIPnodeGetParent(node);
			parent_number = SCIPnodeGetNumber(parent) - 1;
			parent_lowerbound = SCIPnodeGetLowerbound(parent);
		}

		return FocusNodeObs{number, depth, lowerbound, estimate, n_added_conss, n_vars,
												nlpcands, npseudocands, parent_number, parent_lowerbound};
	}
	return {};
}

}  // namespace ecole::observation
