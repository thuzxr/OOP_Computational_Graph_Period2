#ifndef COMPUTATIONAL_GRAPH_MULCNODE_H
#define COMPUTATIONAL_GRAPH_MULCNODE_H

#include"../lib/calcnode.h"

template<typename _T>
class MulCNode : public CalcNode<_T>
{
protected:
    _T Calc(); //重载Calc，在这里进行计算
	_T DerCalc(Node <_T> *operand);
public:
	using CalcNode<_T>::Result;
    using CalcNode<_T>::DerResult;
	using CalcNode<_T>::OperandNum;
	using CalcNode<_T>::Operands;
	using CalcNode<_T>::CalcNode;
};

template<>
double MulCNode<double>::Calc()
{
	Result = new double(Operands[0]->GetVal() * Operands[1]->GetVal());
	return *Result;
}

template<>
Tensor MulCNode<Tensor>::Calc()
{	
	Result = new Tensor(Operands[0]->GetVal() * Operands[1]->GetVal());
	return *Result;
}

template<>
double MulCNode<double>::DerCalc(Node <double> *operand)
{
	double der = (this == operand) ? 1.0 : Operands[0]->GetDer(operand) * Operands[1]->GetVal() 
		+ Operands[1]->GetDer(operand) * Operands[0]->GetVal();
	DerResult = new double(der);
	return *DerResult;
}

template<>
Tensor MulCNode<Tensor>::DerCalc(Node <Tensor> *operand) //这里估计有bug
{
	//（m*n）矩阵W乘（n*l）矩阵x对（a*b）矩阵t求导，得到应该是（ml*ab）的矩阵
	int row_num = GetVal().size(), col_num = operand->GetVal().size(), iter_num = Operands[0]->GetVal().shape_size(1); // 分别是ml，ab， n
	auto first_shape = GetVal().shape(), second_shape = operand->GetVal().shape(); // 分别是(m, l)和(a, b)
	auto operand_1_val = Operands[0]->GetVal(), operand_2_val = Operands[1]->GetVal();
	auto operand_1_shape = operand_1_val.shape(), operand_2_shape = operand_2_val.shape(); // 分别是(m, n)和(n, l)
	auto der = Tensor(Shape({row_num, col_num}), 0.0);
    if (this != operand) {
		auto first_der = Operands[0]->GetDer(operand), second_der = Operands[1]->GetDer(operand); //分别计算W和x对t的导数，应当为(mn, ab)和(nl, ab)
		//将row看成(i, j)相对于(m, l)， col看成(p, q)相对于(a, b)
		for (int row = 0; row < row_num; row++)
			for (int col = 0; col < col_num; col++) {
				auto first_shape_rank = rank2shape_rank(row, first_shape); // (i, j)
				int row_in_operand_1 = first_shape_rank[0], col_in_operand_2 = first_shape_rank[1]; // 分别是i，j
				for (int iter = 0; iter < iter_num; iter++) { //就是k
					int rank_in_operand_1 = shape_rank2rank(Shape({row_in_operand_1, iter}), operand_1_shape); //(i, k)相对于(m, n)的rank
					int rank_in_operand_2 = shape_rank2rank(Shape({iter, col_in_operand_2}), operand_2_shape); //(k, j)相对于(n, l)的rank
					der.elem(Shape({row, col})) += (first_der.elem(Shape({rank_in_operand_1, col})) * operand_2_val.elem(rank_in_operand_2) 
						+ operand_1_val.elem(rank_in_operand_1) * second_der.elem(Shape({rank_in_operand_2, col})));
				}
			}
	}
	else 		
        for (int i = 0; i < row_num; i++)
			der.elem(Shape({i, i})) = 1.0;
	DerResult = new Tensor(der);
    return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_MULCNODE_H