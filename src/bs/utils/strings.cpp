#include "strings.h"

#include <System/Text/UTF8Encoding.hpp>

std::string utils::to_utf8(Il2CppString* str) {
    const auto utf8_encoding = System::Text::Encoding::get_UTF8();
    const auto bytes = utf8_encoding->GetBytes(str);
    return { reinterpret_cast<char*>(bytes->values), static_cast<std::size_t>(bytes->get_Length()) };
}

Il2CppString* utils::to_il2cpp(std::string_view str) {
    const auto utf8_encoding = System::Text::Encoding::get_UTF8();
    return System::String::CreateStringFromEncoding(
            reinterpret_cast<unsigned char*>(const_cast<char*>(str.data())),
            static_cast<int>(str.length()),
            utf8_encoding
    );
}