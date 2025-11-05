#include "types.hpp"

#include <utility>

const char* dataTypeName(TypeKind type) {
    switch (type) {
        case TypeKind::INT:
            return "int";
        case TypeKind::BYTE:
            return "byte";
        case TypeKind::VOID:
            return "void";
    }
    return "unknown";
}

SemaType::SemaType(TypeKind kind, std::vector<std::optional<int>> dimensionSizes)
    : kind_(kind), dimensionSizes_(std::move(dimensionSizes)) {}

TypeKind SemaType::kind() const noexcept {
    return kind_;
}

bool SemaType::isArray() const noexcept {
    return !dimensionSizes_.empty();
}

bool SemaType::isScalar() const noexcept {
    return dimensionSizes_.empty();
}

std::size_t SemaType::dimensions() const noexcept {
    return dimensionSizes_.size();
}

const std::vector<std::optional<int>>& SemaType::dimensionSizes() const noexcept {
    return dimensionSizes_;
}
