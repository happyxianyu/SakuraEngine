#pragma once
#include "pool.hpp"
#include "scheduler.hpp"
#include "type_registry.hpp"

#include "SkrRT/containers/string.hpp"

struct dual_context_t {
    dual_context_t();
    dual::pool_t normalPool;
    dual::pool_t largePool;
    dual::pool_t smallPool;
    dual::type_registry_t typeRegistry;
    dual::scheduler_t scheduler;
    skr::string error;
};