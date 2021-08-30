#pragma once

namespace vob::sta
{
    template <typename SourceT, typename TargetT>
    using same_const = std::conditional<std::is_const_v<SourceT>, TargetT const, std::remove_const_t<TargetT>>;

    template <typename SourceT, typename TargetT>
    using same_const_t = typename same_const<SourceT, TargetT>::type;
}