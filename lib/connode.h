#ifndef COMPUTATIONAL_GRAPH_CONNODE_H
#define COMPUTATIONAL_GRAPH_CONNODE_H

#include "node.h"

template<typename _T>
class ConNode : public Node<_T>
{
private:
	const _T *ConVal; //指向常量的指证，用来储存初始化的答案，不可更改
	_T DerCalc(Node <_T> *operand);
public:
	explicit ConNode(const _T &_InitData) { ConVal = new const _T(_InitData); } //从常量构造节点

	_T GetVal(); //求值

	_T GetDer(Node <_T> *operand);

	void Clear(); //清除（虽然能被调用，但实际上什么都不做）

	~ConNode()
	{
		delete ConVal;
		ConVal = nullptr;
	}
};

template<typename _T>
_T ConNode<_T>::GetVal() //直接返回常值
{
	return *ConVal;
}

template<typename _T>
void ConNode<_T>::Clear() {} //清除时什么都不做

template<>
double ConNode<double>::GetDer(Node <double> *operand)
{
	double der = (operand == this) ? 1.0 : 0.0;
	DerResult = new double(der);
	return *DerResult;
}

template<>
Tensor ConNode<Tensor>::GetDer(Node <Tensor> *operand)
{
	int row_num = GetVal().size(), col_num = operand->GetVal().size();
	auto der = Tensor(Shape({row_num, col_num}), 0.0);
	if (this == operand)
		for (int i = 0; i < row_num; i++)
			der.elem(Shape({i, i})) = 1.0;
	DerResult = new Tensor(der);
	return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_CONNODE_H

