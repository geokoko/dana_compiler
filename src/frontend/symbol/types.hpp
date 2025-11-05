#pragma once

#include <cstddef>
#include <optional>
#include <vector>

enum class TypeKind {
    INT,
    BYTE,
    VOID
};

const char* dataTypeName(TypeKind type);

class SemaType {
public:
    explicit SemaType(TypeKind kind, std::vector<std::optional<int>> dimensionSizes = {});

    TypeKind kind() const noexcept;
    bool isArray() const noexcept;
    bool isScalar() const noexcept;
    std::size_t dimensions() const noexcept;
    const std::vector<std::optional<int>>& dimensionSizes() const noexcept;

private:
    TypeKind kind_;
    std::vector<std::optional<int>> dimensionSizes_;
};
