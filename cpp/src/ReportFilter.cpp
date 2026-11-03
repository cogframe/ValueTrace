#include "valuetrace/ReportFilter.h"

#include <numeric>
#include <stdexcept>
#include <utility>

namespace valuetrace {
namespace {

std::size_t indexOf(ReportStatus status) {
    const auto index = static_cast<std::size_t>(status);
    if (index >= reportStatuses.size()) throw std::invalid_argument("invalid report status");
    return index;
}

std::string_view trim(std::string_view text) {
    const auto begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string_view::npos) return {};
    return text.substr(begin, text.find_last_not_of(" \t\r\n") - begin + 1);
}

} // namespace

const char* reportStatusName(ReportStatus status) {
    switch (status) {
    case ReportStatus::SourceConfirmed: return "source-confirmed";
    case ReportStatus::BoundaryTriggerable: return "boundary-triggerable";
    case ReportStatus::Unknown: return "unknown";
    case ReportStatus::Unverified: return "unverified";
    case ReportStatus::ScopedUnreachable: return "scoped-unreachable";
    }
    throw std::invalid_argument("invalid report status");
}

ReportFilter::ReportFilter(unsigned mask, std::string description)
    : mask_(mask), description_(std::move(description)) {}

ReportFilter ReportFilter::forLevel(unsigned level) {
    if (level < 1 || level > 5) throw std::invalid_argument("--report-level must be an integer from 1 to 5");
    return {(1U << level) - 1, "level:" + std::to_string(level)};
}

ReportFilter ReportFilter::forStatuses(std::string_view names) {
    names = trim(names);
    if (names == "all") return {31, "statuses:all"};
    if (names == "none") return {0, "statuses:none"};
    unsigned mask = 0;
    while (true) {
        const auto comma = names.find(',');
        const auto token = trim(names.substr(0, comma));
        bool found = false;
        for (auto status : reportStatuses) {
            if (token == reportStatusName(status)) {
                mask |= 1U << indexOf(status);
                found = true;
                break;
            }
        }
        if (!found) throw std::invalid_argument("invalid --show-status entry '" + std::string(token) +
            "'; use source-confirmed,boundary-triggerable,unknown,unverified,scoped-unreachable, or all/none alone");
        if (comma == std::string_view::npos) break;
        names.remove_prefix(comma + 1);
    }
    std::string description = "statuses:";
    for (auto status : reportStatuses) {
        if ((mask & (1U << indexOf(status))) != 0) {
            if (description.back() != ':') description += ',';
            description += reportStatusName(status);
        }
    }
    return {mask, std::move(description)};
}

bool ReportFilter::includes(ReportStatus status) const { return (mask_ & (1U << indexOf(status))) != 0; }
std::size_t ReportSummary::totalCount() const { return std::accumulate(totals.begin(), totals.end(), std::size_t{0}); }
std::size_t ReportSummary::shownCount() const { return std::accumulate(shown.begin(), shown.end(), std::size_t{0}); }
std::size_t ReportSummary::hiddenCount() const { return totalCount() - shownCount(); }

ReportSummary summarizeReports(std::span<const ReportStatus> statuses, const ReportFilter& filter) {
    ReportSummary summary;
    for (auto status : statuses) {
        ++summary.totals[indexOf(status)];
        if (filter.includes(status)) ++summary.shown[indexOf(status)];
    }
    return summary;
}

} // namespace valuetrace
