#pragma once

#include <string>

enum class DataType {
    Int,
    Byte
};

inline const char* dataTypeName(DataType type) {
    switch (type) {
        case DataType::Int:  return "int";
        case DataType::Byte: return "byte";
    }
    return "<?>";
}
