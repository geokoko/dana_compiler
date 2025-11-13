#include "sematype.hpp"

#include <utility>

SemaType::SemaType(TypeKind kind)
    : kind_(kind) {}

SemaType::TypeKind SemaType::getKind() const {
    return kind_;
}

bool IntType::equals(const SemaType& other) const {
    return other.getKind() == TypeKind::INT;
}

bool ByteType::equals(const SemaType& other) const {
    return other.getKind() == TypeKind::BYTE;
}

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

    const auto& rhs = static_cast<const ArrayType&>(other);
    if (size_ != rhs.size_) {
        return false;
    }

    if (elem_ == rhs.elem_) {
        return true;
    }

    if (!elem_ || !rhs.elem_) {
        return false;
    }

    return elem_->equals(*rhs.elem_);
}

FuncType::FuncType(SemaTypePtr returnType, std::vector<SemaTypePtr> params)
    : SemaType(TypeKind::FUNC),
      ret_(std::move(returnType)),
      params_(std::move(params)) {}

const SemaTypePtr& FuncType::returnType() const {
    return ret_;
}

const std::vector<SemaTypePtr>& FuncType::params() const {
    return params_;
}

bool FuncType::equals(const SemaType& other) const {
    if (other.getKind() != TypeKind::FUNC) {
        return false;
    }

    const auto& rhs = static_cast<const FuncType&>(other);

    if (ret_ != rhs.ret_) {
        if (!ret_ || !rhs.ret_) {
            return false;
        }
        if (!ret_->equals(*rhs.ret_)) {
            return false;
        }
    }

    if (params_.size() != rhs.params_.size()) {
        return false;
    }

    for (std::size_t i = 0; i < params_.size(); ++i) {
        const auto& lhsParam = params_[i];
        const auto& rhsParam = rhs.params_[i];

        if (lhsParam != rhsParam) {
            if (!lhsParam || !rhsParam) {
                return false;
            }
            if (!lhsParam->equals(*rhsParam)) {
                return false;
            }
        }
    }

    return true;
}

bool VoidType::equals(const SemaType& other) const {
    return other.getKind() == TypeKind::VOID;
}

namespace {
	template <class T, class... Args>
	SemaTypePtr makeType(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

SemaTypePtr makeIntType() {
	static SemaTypePtr instance = makeType<IntType>();
	return instance;
}

SemaTypePtr makeByteType() {
	static SemaTypePtr instance = makeType<ByteType>();
	return instance;
}

SemaTypePtr makeVoidType() {
	static SemaTypePtr instance = makeType<VoidType>();
	return instance;
}

SemaTypePtr makeArrayType(SemaTypePtr elementType, std::optional<std::size_t> size) {
	return makeType<ArrayType>(std::move(elementType), size);
}

SemaTypePtr makeFuncType(SemaTypePtr returnType, std::vector<SemaTypePtr> params) {
	return makeType<FuncType>(std::move(returnType), std::move(params));
}
