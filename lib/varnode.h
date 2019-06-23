#ifndef COMPUTATIONAL_GRAPH_VARNODE_H
#define COMPUTATIONAL_GRAPH_VARNODE_H

#include "phnode.h"

template<typename _T>
class VarNode : public PHNode<_T>
{
private:
	static const std::string ErrMsg;  //需要输出的错误信息，设定为静态
public:
	using PHNode<_T>::Result;
	using PHNode<_T>::DerResult;

	VarNode() { Result = nullptr; } //第一种构造方式，建一个空变量

	explicit VarNode(_T _InitVal) { Result = new _T(_InitVal); } //第二种构造方式，建立变量的同时赋初值

	_T GetVal(); //如果未被赋值或被强制清除过则抛出错误信息，否则返回*Result

	_T GetDer(Node <_T> *operand); //如果未被赋值或被强制清除过则抛出错误信息，否则返回*DerResult

	void Clear() {}

	void ForcedClear();

};

template<typename _T>
const std::string VarNode<_T>::ErrMsg = "ERROR: Variable missing";

template<typename _T>
void VarNode<_T>::ForcedClear()
{
	Node<_T>::Clear();
}

template<typename _T>
_T VarNode<_T>::GetVal()
{
	if (!Result) throw ErrMsg;
	return *Result;
}

template<>
double VarNode<double>::GetDer(Node <double> *operand)
{
	if (!Result) throw ErrMsg;
	double der = (operand == this) ? 1.0 : 0.0;
	DerResult = new double(der);
	return *DerResult;
}

template<>
Tensor VarNode<Tensor>::GetDer(Node <Tensor> *operand)
{
	if (!Result) throw ErrMsg;
	int row_num = GetVal().size(), col_num = operand->GetVal().size();
	auto der = Tensor(Shape({row_num, col_num}), 0.0);
	if (this == operand) {
		for (int i = 0; i < row_num; i++)
			der.elem(Shape({i, i})) = 1.0;
	}
	DerResult = new Tensor(der);
	return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_VARNODE_H

