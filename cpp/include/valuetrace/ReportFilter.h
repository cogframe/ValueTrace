#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>

namespace valuetrace {

// Verification states, not severity levels. Values are ordered only for the
// cumulative display presets. Producers must supply evidence before upgrading.
enum class ReportStatus {
    SourceConfirmed,
    BoundaryTriggerable,
    Unknown,
    Unverified,
    ScopedUnreachable,
};

inline constexpr std::array reportStatuses{
    ReportStatus::SourceConfirmed, ReportStatus::BoundaryTriggerable,
    ReportStatus::Unknown, ReportStatus::Unverified, ReportStatus::ScopedUnreachable};

const char* reportStatusName(ReportStatus status);

class ReportFilter {
public:
    ReportFilter() = default; // Level 2: confirmed and boundary-triggerable.
    static ReportFilter forLevel(unsigned level);
    static ReportFilter forStatuses(std::string_view names);
    bool includes(ReportStatus status) const;
    const std::string& description() const noexcept { return description_; }

private:
    ReportFilter(unsigned mask, std::string description);
    unsigned mask_ = 3;
    std::string description_ = "level:2";
};

struct ReportSummary {
    std::array<std::size_t, reportStatuses.size()> totals{};
    std::array<std::size_t, reportStatuses.size()> shown{};

    std::size_t totalCount() const;
    std::size_t shownCount() const;
    std::size_t hiddenCount() const;
};

ReportSummary summarizeReports(std::span<const ReportStatus> statuses, const ReportFilter& filter);

} // namespace valuetrace
