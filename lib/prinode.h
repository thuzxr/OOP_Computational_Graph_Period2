#ifndef COMPUTATIONAL_GRAPH_PRINODE_H
#define COMPUTATIONAL_GRAPH_PRINODE_H

#include <iostream>
#include "node.h"

template<typename _T>
class PriNode : public Node<_T>
{
private:
	static const std::string ErrMsg; //需要输出的错误信息，设定为静态
protected:
	std::string WatchName; //需要被输出的节点名称
	Node<_T> *WatchNode; //需要被输出的节点
	std::ostream &OUTPUT;

	_T Print();

public:
	using Node<_T>::Result;
	using Node<_T>::DerResult;

	explicit PriNode(std::string _NtoWName, Node<_T> *_NtoW, std::ostream &_OSTR)
		: WatchName(_NtoWName), WatchNode(_NtoW), OUTPUT(_OSTR) {};

	_T GetVal();
	_T GetDer(Node <_T> *operand);

	void Clear();
};

template <typename _T>
const std::string PriNode<_T>::ErrMsg = "ERROR: Cannot derivate with PRINT";

template<typename _T>
_T PriNode<_T>::Print()
{
	Result = new _T(WatchNode->GetVal());
	OUTPUT << "PRINT operator: " << WatchName << " = " << *Result << std::endl; //输出观察结果
	return *Result;
}

template<typename _T>
_T PriNode<_T>::GetVal()
{
	if (!Result) Print();
	return *Result;
}

template<typename _T>
void PriNode<_T>::Clear()
{
	WatchNode->Clear();
	Node<_T>::Clear();
}

template<typename _T>
_T PriNode<_T>::GetDer(Node <_T> *operand)
{
	DerResult = new _T(WatchNode->GetDer(operand));
	return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_PRINODE_H

