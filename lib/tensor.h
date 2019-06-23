#ifndef COMPUTATIONAL_GRAPH_TENSOR_H
#define COMPUTATIONAL_GRAPH_TENSOR_H

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <random>

extern std::mt19937 engine;
extern std::normal_distribution<double> gaussian;

typedef std::vector<int> Shape;
typedef std::vector<double> Elem;

int shape2size(const Shape &s); //将shape转化成size

Shape rank2shape_rank(const int &rank, const Shape &shape); //将循环量rank转化成每个维度上的分量

int shape_rank2rank(const Shape &shape_rank, const Shape &shape); //将每个维度的分量转化成循环量rank，加上取模操作方便broadcast操作，所以使用时需要额外注意

int rank2count(const int &rank, const Shape &shape);

class Tensor {
    Elem _elem;
    int _dim; //维数
    Shape _shape; //各个维度上的尺寸
    static const std::string ErrMsg;
public:
    Tensor() {}
    Tensor(const Shape &shape_, const Elem &elem_): _dim(shape_.size()), _shape(shape_), _elem(elem_) {} //通过shape和elem构造
    Tensor(const Shape &shape_, Elem &&elem_): _dim(shape_.size()), _shape(shape_) { _elem = std::move(elem_); }
    Tensor(const Shape &shape_, const double &val_):  _dim(shape_.size()), _shape(shape_) { //默认所有元素均为d，形状为shape（只在标量乘法用到）
        _elem = Elem(size());
        for (auto it = _elem.begin(); it != _elem.end(); it++) *it = val_;
    }
    Tensor(const Elem &elem_): _dim(1), _elem(elem_) { //默认构造一个1*n的Tensor
        _shape = Shape(1); _shape[0] = elem_.size();
    }
    Tensor(Elem &&elem_): _dim(1) { //默认构造一个1*n的Tensor
        _shape = Shape(1); _shape[0] = elem_.size();
        _elem = std::move(elem_);
    }
    Tensor(const Tensor &t) {
        int new_size = t.size();
        Elem new_elem(new_size); 
        for (int i = 0; i < new_size; i++) new_elem[i] = t.elem(i);
        _elem = new_elem; _shape = t.shape(); _dim = t.dim();
    }

    int size() const { return shape2size(_shape); } //返回元素个数
    int dim() const { return _dim; } 
    Shape shape() const { return _shape; }
    int shape_size(const int &dim) const { return _shape[dim]; } //返回dim维度的尺寸

    double &elem(const int &rank) { return _elem[rank]; }
    double elem(const int &rank) const { return _elem[rank]; }
    double &elem(const Shape &shape_rank) { return elem(shape_rank2rank(shape_rank, _shape)); }
    double elem(const Shape &shape_rank) const { return elem(shape_rank2rank(shape_rank, _shape)); }

    bool size_cap(const Shape &s) { return (size() == shape2size(s)); } //比较元素个数是否相容
    bool broadcast_cap(const Tensor &t) const; //比较是否能够broadcast
    bool shape_cap(const Tensor &t, const int &dim) const { return (shape_size(dim) == t.shape_size(dim)); } //比较在dim维度是否相容，用于concat

    Tensor &transpose();
    Tensor &reshape(const Shape &new_shape);
    Tensor broadcast_sum(const Tensor &t) const;
    Tensor broadcast_min(const Tensor &t) const;
    Tensor broadcast_mul(const Tensor &t) const; //broadcast版本的乘法
    Tensor broadcast_div(const Tensor &t) const;
    Tensor concat(const Tensor &t, const int &op_dim) const;
    Tensor relu() const;
    Tensor der_relu() const;
    Tensor softmax() const;
    Tensor sqrt() const;
    Tensor reduce_sum(const int &op_dim) const;
    Tensor reduce_mul(const int &op_dim) const;
    Tensor onehot(const int &size_) const;

    double norm() const;
    int argmax() const; //返回元素最大的rank

    Tensor operator+(const Tensor &t);
    Tensor operator-() const ;
    Tensor operator-(const Tensor &t);
    Tensor operator*(const Tensor &t); //矩阵乘法，默认计算2维
    Tensor operator*(const double &d); //每个元素乘一个标量

    friend std::ostream &operator<<(std::ostream &out, const Tensor &t) {
        int size_ = t.size(), dim_ = t.dim(); bool left_flag = true;
        for (int rank = 0; rank <= size_; rank++) {
            int count = rank2count(rank, t._shape);
            if (count == dim_) //需要输出左括号
                if (left_flag) {
                    while (count--) out << "[";
                    left_flag = false;
                    out << t.elem(rank);
                }
                else
                    while (count--) out << "]";
            else {
                int count_ = count;
                while (count_--) out << "]";
                if (count >= dim_ - 1 && count) out << "\n";
                else out << " ";
                while (count--) out << "[";
                out << t.elem(rank);
            }
        }
        out << "\n";
	return out;
    }

    void shape_display() {
        std::cout << "(";
        for (int i = 0; i < dim() - 1; i++) std::cout << shape_size(i) << ", ";
        std::cout << shape_size(dim() - 1) << ")" << std::endl;
    }
};

Tensor get_normal_tensor(const Shape &shape);

#endif
