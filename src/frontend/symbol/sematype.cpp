#include "sematype.hpp"

// ===================== SemaType base =====================

SemaType::SemaType(TypeKind kind)
	: kind_(kind) {}

SemaType::TypeKind SemaType::getKind() const {
	return kind_;
}

// ===================== IntType =====================

bool IntType::equals(const SemaType& other) const {
	return other.getKind() == TypeKind::INT;
}

// ===================== ByteType =====================

bool ByteType::equals(const SemaType& other) const {
	return other.getKind() == TypeKind::BYTE;
}

// ===================== ArrayType =====================

ArrayType::ArrayType(const SemaType* elementType, std::size_t size)
	: SemaType(TypeKind::ARRAY),
	  elem_(elementType),
	  size_(size) {}

const SemaType* ArrayType::elementType() const {
	return elem_;
}

std::size_t ArrayType::size() const {
	return size_;
}

bool ArrayType::equals(const SemaType& other) const {
	if (other.getKind() != TypeKind::ARRAY) {
		return false;
	}
	const auto& o = static_cast<const ArrayType&>(other);

	if (size_ != o.size_) {
		return false;
	}
	if (!elem_ || !o.elem_) {
		return elem_ == o.elem_;
	}
	return elem_->equals(*o.elem_);
}

// ===================== FuncType =====================

FuncType::FuncType(const SemaType* returnType, std::vector<std::pair<const SemaType*, ParamPass>> paramTypes)
	: SemaType(TypeKind::FUNC),
	  ret_(returnType),
	  paramTypes_(std::move(paramTypes)) {}

const SemaType* FuncType::returnType() const {
	return ret_;
}

bool FuncType::equals(const SemaType& other) const {
	if (other.getKind() != TypeKind::FUNC) {
		return false;
	}
	const auto& o = static_cast<const FuncType&>(other);

	// Compare return types (nullptr = void)
	if (!ret_ || !o.ret_) {
		if (ret_ != o.ret_) {
			return false;
		}
	} else if (!ret_->equals(*o.ret_)) {
		return false;
	}

	// Compare parameter count
	if (paramTypes_.size() != o.paramTypes_.size()) {
		return false;
	}

	// Compare each parameter type and passing mode
	for (std::size_t i = 0; i < paramTypes_.size(); ++i) {
		const auto& a = paramTypes_[i];
		const auto& b = o.paramTypes_[i];

		if (a.second != b.second) {
			return false;
		}

		if (!a.first || !b.first) {
			if (a.first != b.first) {
				return false;
			}
		} else if (!a.first->equals(*b.first)) {
			return false;
		}
	}

	return true;
}

// ===================== VoidType =====================

bool VoidType::equals(const SemaType& other) const {
	return other.getKind() == TypeKind::VOID;
}

