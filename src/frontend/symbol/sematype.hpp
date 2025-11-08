#pragma once

#include <cstddef>
#include <vector>
#include <utility>

class SemaType {
public:
	enum class TypeKind {
		INT,
		BYTE,
		ARRAY,
		FUNC,
		VOID
	};

	enum class ParamPass {
		BY_VAL,
		BY_REF
	};

	virtual ~SemaType() = default;
	TypeKind getKind() const;
	virtual bool equals(const SemaType &other) const = 0; // for type-checking

protected:
	explicit SemaType(TypeKind kind);

private:
	TypeKind kind_;
};

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
    ArrayType(const SemaType* elementType, std::size_t size);

    const SemaType* elementType() const;
    std::size_t size() const;

    bool equals(const SemaType& other) const override;

private:
    const SemaType* elem_;
    std::size_t size_;
};

// ---------- Function type ----------

class FuncType : public SemaType {
public:
    FuncType(const SemaType* returnType, std::vector<std::pair<const SemaType*, ParamPass>> paramTypes);
    const SemaType* returnType() const;
    const std::vector<const SemaType*>& paramTypes() const;
    const std::vector<ParamPass>& paramPasses() const;

    bool equals(const SemaType& other) const override;

private:
    const SemaType* ret_;
	std::vector<std::pair<const SemaType*, ParamPass>> paramTypes_;
};

// ---------- Void type (for procs / "no value") ----------

class VoidType : public SemaType {
public:
    VoidType() : SemaType(TypeKind::VOID) {}

    bool equals(const SemaType& other) const override;
};

