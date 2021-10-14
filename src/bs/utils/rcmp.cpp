#include <rcmp/codegen.hpp>
#include <extern/beatsaber-hook/shared/inline-hook/And64InlineHook.hpp>

// Bridge between rcmp and And64InlineHook
rcmp::address_t rcmp::make_raw_hook(rcmp::address_t original_function, rcmp::address_t wrapper_function) {
    void* relocated_original = nullptr;
    ::A64HookFunction(original_function.as_ptr(), wrapper_function.as_ptr(), &relocated_original);
    if (!relocated_original) {
        throw rcmp::error("::A64HookFunction fails to hook at %" PRIXPTR, original_function.as_ptr());
    }

    return relocated_original;
}
