#pragma once

#include <extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp> // For using il2cpp_utils:: methods
#include <extern/beatsaber-hook/shared/utils/utils.h> // CRASH_UNLESS

#include <rcmp/codegen.hpp>

template <auto Method, class F>
void hookMethod(F&& hook) {
    const MethodInfo* methodInfo = ::il2cpp_utils::il2cpp_type_check::MetadataGetter<Method>::get();
    CRASH_UNLESS(methodInfo);

    auto address = reinterpret_cast<void*>(methodInfo->methodPointer);
    CRASH_UNLESS(address);

    using Signature = rcmp::flatten_pmf_t<decltype(Method), rcmp::cconv::native_x64>;
    rcmp::hook_function<Signature>(address, std::forward<F>(hook));
}
