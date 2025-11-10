#include "sematype.hpp"

#include <utility>

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

ArrayType::ArrayType(SemaTypePtr elementType, std::optional<std::size_t> size)
	: SemaType(TypeKind::ARRAY),
	  elem_(std::move(elementType)),
	  size_(size) {}

const SemaTypePtr& ArrayType::elementType() const {
	return elem_;
}

std::optional<std::size_t> ArrayType::size() const {
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

FuncType::FuncType(SemaTypePtr returnType, std::vector<Param> params)
	: SemaType(TypeKind::FUNC),
	  ret_(std::move(returnType)),
	  params_(std::move(params)) {
	passes_.reserve(params_.size());
	for (const auto& param : params_) {
		passes_.push_back(param.second);
	}
}

const SemaTypePtr& FuncType::returnType() const {
	return ret_;
}

const std::vector<FuncType::ParamType>& FuncType::params() const {
	return params_;
}

bool FuncType::equals(const SemaType& other) const {
	if (other.getKind() != TypeKind::FUNC) {
		return false;
	}
	const auto& o = static_cast<const FuncType&>(other);

	if (!ret_ || !o.ret_) {
		if (ret_ != o.ret_) {
			return false;
		}
	} else if (!ret_->equals(*o.ret_)) {
		return false;
	}

	if (params_.size() != o.params_.size()) {
		return false;
	}

	for (std::size_t i = 0; i < params_.size(); ++i) {
		const auto& lhs = params_[i];
		const auto& rhs = o.params_[i];

		if (lhs.second != rhs.second) {
			return false;
		}

		if (!lhs.first || !rhs.first) {
			if (lhs.first != rhs.first) {
				return false;
			}
		} else if (!lhs.first->equals(*rhs.first)) {
			return false;
		}
	}

	return true;
}

// ===================== VoidType =====================

bool VoidType::equals(const SemaType& other) const {
	return other.getKind() == TypeKind::VOID;
}

