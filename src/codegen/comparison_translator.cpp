//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// comparison_translator.cpp
//
// Identification: src/codegen/comparison_translator.cpp
//
// Copyright (c) 2015-17, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "codegen/value_proxy.h"
#include "codegen/comparison_translator.h"

#include "expression/comparison_expression.h"
#include "codegen/value_proxy.h"

namespace peloton {
namespace codegen {

// Constructor
ComparisonTranslator::ComparisonTranslator(
    const expression::ComparisonExpression &comparison,
    CompilationContext &context)
    : ExpressionTranslator(comparison, context) {
  PL_ASSERT(comparison.GetChildrenSize() == 2);
}

codegen::Value ComparisonTranslator::DeriveValue(CodeGen &codegen,
                                                 RowBatch::Row &row) const {
  return DoTypeValueComp(codegen, row);
}

// Produce the result of performing the comparison of left and right values
codegen::Value ComparisonTranslator::DoTypeValueComp(CodeGen &codegen,
                                                 RowBatch::Row &row) const {
  const auto &comparison = GetExpressionAs<expression::ComparisonExpression>();

  codegen::Value left = row.DeriveTypeValue(codegen, *comparison.GetChild(0));
  codegen::Value right = row.DeriveTypeValue(codegen, *comparison.GetChild(1));
  std::vector<llvm::Value *> args =
        {left.GetValue(), right.GetValue()};

  switch (comparison.GetExpressionType()) {
    case ExpressionType::COMPARE_EQUAL:
      return codegen::Value{type::Type::BOOLEAN,
                            codegen.CallFunc(
                                    ValueProxy::_CmpEqual::GetFunction(codegen),
                                    args),
                            nullptr};
    case ExpressionType::COMPARE_NOTEQUAL:
      return codegen::Value{type::Type::BOOLEAN,
                            codegen.CallFunc(
                                    ValueProxy::_CmpNotEqual::GetFunction(codegen),
                                    args),
                            nullptr};
    case ExpressionType::COMPARE_LESSTHAN:
      return codegen::Value{type::Type::BOOLEAN,
                            codegen.CallFunc(
                                    ValueProxy::_CmpLess::GetFunction(codegen),
                                    args),
                            nullptr};
    case ExpressionType::COMPARE_LESSTHANOREQUALTO:
      return codegen::Value{type::Type::BOOLEAN,
                            codegen.CallFunc(
                                    ValueProxy::_CmpLessEqual::GetFunction(codegen),
                                    args),
                            nullptr};
    case ExpressionType::COMPARE_GREATERTHAN:
      return codegen::Value{type::Type::BOOLEAN,
                            codegen.CallFunc(
                                    ValueProxy::_CmpGreater::GetFunction(codegen),
                                    args),
                            nullptr};
    case ExpressionType::COMPARE_GREATERTHANOREQUALTO:
      return codegen::Value{type::Type::BOOLEAN,
                            codegen.CallFunc(
                                    ValueProxy::_CmpGreaterEqual::GetFunction(codegen),
                                    args),
                            nullptr};
    default: {
      throw Exception{"Invalid expression type for translation " +
                      ExpressionTypeToString(comparison.GetExpressionType())};
    }
  }
}

}  // namespace codegen
}  // namespace peloton