#pragma once

#include "valuetrace/ReportFilter.h"
#include "valuetrace/Slice.h"
#include <iosfwd>

namespace valuetrace {

// Display only. Does not modify the graph, verification states or exit status.
ReportSummary writeArithmeticReport(std::ostream& output, const DependencyGraph& graph,
                                   const SliceResult& slice, const ReportFilter& filter = {});

} // namespace valuetrace
