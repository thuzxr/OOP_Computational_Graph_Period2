#ifndef COMPUTATIONAL_GRAPH_PHNODE_H
#define COMPUTATIONAL_GRAPH_PHNODE_H

#include <string>
#include "node.h"

template<typename _T>
class PHNode : public Node<_T>
{
private:
	static const std::string ErrMsg; //需要输出的错误信息，设定为静态
public:
	using Node<_T>::Result;
	using Node<_T>::DerResult;

	_T GetVal();
	_T GetDer(Node <_T> *operand);

	_T SetVal(_T);

	using Node<_T>::Clear;
};

template<typename _T>
const std::string PHNode<_T>::ErrMsg = "ERROR: Placeholder missing"; //类外定义

template<typename _T>
_T PHNode<_T>::GetVal()
{
	if (!Result) throw ErrMsg; //如果未被赋值则出错，抛出错误信息
	return *Result;
}

template<typename _T>
_T PHNode<_T>::SetVal(_T _Val) //在Eval的时候用来预先赋值
{
	if (Result) *Result = _Val;
	else Result = new _T(_Val);
	return *Result;
}

template<>
double PHNode<double>::GetDer(Node <double> *operand)
{
	double der = (operand == this) ? 1.0 : 0.0;
	DerResult = new double(der);
	return *DerResult;
}

template<>
Tensor PHNode<Tensor>::GetDer(Node <Tensor> *operand)
{
	int row_num = GetVal().size(), col_num = operand->GetVal().size();
	auto der = Tensor(Shape({row_num, col_num}), 0.0);
	if (this == operand)
		for (int i = 0; i < row_num; i++)
			der.elem(Shape({i, i})) = 1.0;
	DerResult = new Tensor(der);
	return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_PHNODE_H

