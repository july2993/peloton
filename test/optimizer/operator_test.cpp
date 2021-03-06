#include <algorithm>
#include "common/harness.h"

#define private public

#include "optimizer/operators.h"
#include "expression/comparison_expression.h"
#include "expression/tuple_value_expression.h"

namespace peloton {

namespace test {

using namespace optimizer;

class OperatorTests : public PelotonTest {};

// TODO: Test complex expressions
TEST_F(OperatorTests, OperatorHashAndEqualTest){
  //===--------------------------------------------------------------------===//
  // GroupBy
  //===--------------------------------------------------------------------===//
  const size_t num_exprs = 100;
  std::vector<std::shared_ptr<expression::AbstractExpression>> cols;
  for (size_t i = 0; i<num_exprs; i++) {
    auto tv_expr = std::make_shared<expression::TupleValueExpression>(std::to_string(i));
    auto oids = std::make_tuple<oid_t, oid_t ,oid_t>(0,0,i);
    tv_expr->SetBoundOid(oids);
    cols.push_back(tv_expr);
  }

  // Generate having
  expression::AbstractExpression* having =
      new expression::ComparisonExpression(ExpressionType::COMPARE_EQUAL);
  auto col1 = new expression::TupleValueExpression("a");
  col1->SetBoundOid(0,0,1);
  auto col2 = new expression::TupleValueExpression("b");
  col2->SetBoundOid(0,0,2);
  having->SetChild(0, col1);
  having->SetChild(1, col2);

  const size_t num_iter = 1000;
  // HashGroupBy
  Operator l_group_by =
      PhysicalHashGroupBy::make(cols, having);

  for (size_t i = 0; i<num_iter; i++) {
    std::random_shuffle(cols.begin(), cols.end());

    Operator r_group_by =
        PhysicalHashGroupBy::make(cols, having);

    EXPECT_EQ(l_group_by.Hash(), r_group_by.Hash());
    EXPECT_TRUE(l_group_by == r_group_by);
  }

  // SortGroupBy
  l_group_by =
      PhysicalSortGroupBy::make(cols, having);

  for (size_t i = 0; i<num_iter; i++) {
    std::random_shuffle(cols.begin(), cols.end());

    Operator r_group_by =
        PhysicalSortGroupBy::make(cols, having);

    EXPECT_EQ(l_group_by.Hash(), r_group_by.Hash());
    EXPECT_TRUE(l_group_by == r_group_by);
  }

  delete having;
}

} /* namespace test */
} /* namespace peloton */
