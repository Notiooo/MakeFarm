#pragma once

/**
 * @brief Determines the chunk rebuilding operation that can be performed
 */
enum class RebuildOperation
{
    None,
    Fast,
    Slow
};