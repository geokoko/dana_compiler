#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

class SemaType {
public:
	enum class TypeKind {
		INT,
		BYTE,
		ARRAY,
		FUNC,
		VOID
	};

	virtual ~SemaType() = default;
	TypeKind getKind() const;
	virtual bool equals(const SemaType &other) const = 0; // for type-checking

protected:
	explicit SemaType(TypeKind kind);

private:
	TypeKind kind_;
};

using SemaTypePtr = std::shared_ptr<const SemaType>;

// ---------- Primitive types ----------

class IntType : public SemaType {
public:
    IntType() : SemaType(TypeKind::INT) {}

    bool equals(const SemaType& other) const override;
};

class ByteType : public SemaType {
public:
    ByteType() : SemaType(TypeKind::BYTE) {}

    bool equals(const SemaType& other) const override;
};

// ---------- Array type ----------

class ArrayType : public SemaType {
public:
    ArrayType(SemaTypePtr elementType, std::optional<std::size_t> size);

    const SemaTypePtr& elementType() const;
    std::optional<std::size_t> size() const;

    bool equals(const SemaType& other) const override;

private:
    SemaTypePtr elem_;
    std::optional<std::size_t> size_;
};

// ---------- Function type ----------

class FuncType : public SemaType {
public:
    FuncType(SemaTypePtr returnType, std::vector<SemaTypePtr> params);

    const SemaTypePtr& returnType() const;
    const std::vector<SemaTypePtr>& params() const;

    bool equals(const SemaType& other) const override;

private:
    SemaTypePtr ret_;
	std::vector<SemaTypePtr> params_;
};

// ---------- Void type (for procs / "no value") ----------

class VoidType : public SemaType {
public:
    VoidType() : SemaType(TypeKind::VOID) {}

    bool equals(const SemaType& other) const override;
};

