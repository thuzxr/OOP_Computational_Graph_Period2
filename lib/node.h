#ifndef COMPUTATIONAL_GRAPH_NODE_H
#define COMPUTATIONAL_GRAPH_NODE_H

#include "tensor.h"

template<typename _T>
class Node
{
protected:
	_T *Result;
	_T *DerResult;
public:
	Node() : Result(nullptr), DerResult(nullptr) {};

	virtual _T GetVal() = 0; //获取节点的计算结果
	virtual void Clear();  //清除节点的计算结果

	virtual ~Node() //析构时必须释放Result
	{
		Clear();
	}

	virtual _T GetDer(Node <_T> *operand) = 0;
};

template<typename _T>
void Node<_T>::Clear()
{
	if (Result) delete Result;
	Result = nullptr;
	if (DerResult) delete DerResult;
	DerResult = nullptr;
}

#endif //COMPUTATIONAL_GRAPH_NODE_H


