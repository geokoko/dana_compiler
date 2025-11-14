#pragma once

#include <string>

enum class DataType {
    INT,
    BYTE
};

inline const char* dataTypeName(DataType type) {
    switch (type) {
        case DataType::INT:  return "int";
        case DataType::BYTE: return "byte";
    }
    return "<?>";
}
