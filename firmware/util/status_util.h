#pragma once

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "util/status_common.h"

#define DIE_IF_ERROR(...) VA_SELECT(DIE_IF_ERROR, __VA_ARGS__)

#define DIE_IF_ERROR_1(expr) DIE_IF_ERROR_2(expr, "Unrecoverable error")
#define DIE_IF_ERROR_2(expr, prefix)                       \
  do {                                                     \
    const absl::Status _status = (expr);                   \
    if (!_status.ok()) {                                   \
      std::cout << prefix << ": " << _status << std::endl; \
      exit(1);                                             \
    }                                                      \
  } while (0)

#define RETURN_IF_ERROR(expr)                                                \
  do {                                                                       \
    /* Using _status below to avoid capture problems if expr is "status". */ \
    const absl::Status _status = (expr);                                     \
    if (!_status.ok()) return _status;                                       \
  } while (0)

// Internal helper for concatenating macro values.
#define STATUS_MACROS_CONCAT_NAME_INNER(x, y) x##y
#define STATUS_MACROS_CONCAT_NAME(x, y) STATUS_MACROS_CONCAT_NAME_INNER(x, y)

#define ASSIGN_OR_RETURN_IMPL(status_or, lhs, rexpr) \
  auto status_or = (rexpr);                          \
  if (!status_or.ok()) {                             \
    return status_or.status();                       \
  }                                                  \
  lhs = std::move(status_or).value();

// Executes an expression that returns a util::StatusOr, extracting its value
// into the variable defined by lhs (or returning on error).
//
// Example: Assigning to a value
//   ASSIGN_OR_RETURN(ValueType value, MaybeGetValue(arg));
//
// WARNING: ASSIGN_OR_RETURN expands into multiple statements; it cannot be used
//  in a single statement (e.g. as the body of an if statement without {})!
#define ASSIGN_OR_RETURN(lhs, rexpr) \
  ASSIGN_OR_RETURN_IMPL(             \
      STATUS_MACROS_CONCAT_NAME(_status_or_value, __COUNTER__), lhs, rexpr);