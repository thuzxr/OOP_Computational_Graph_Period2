#include "tensor.h"

std::mt19937 engine(100);
std::normal_distribution<double> gaussian;

const std::string Tensor::ErrMsg = "ERROR: shape of tensor incompatible";

int shape2size(const Shape &s) { //将shape转化成size
    int total_size = 1;
    for (auto it : s) total_size *= it;
        return total_size;
}

Shape rank2shape_rank(const int &rank, const Shape &shape) { //将循环量rank转化成每个维度上的分量
    int _dim = shape.size();
    Shape shape_rank(_dim);
    int tmp = rank;
    for (int d = _dim - 1; d >= 0; d--) { //得到对应在每个维度上的分量
        shape_rank[d] = tmp % shape[d];
        tmp /= shape[d];
    }
    return shape_rank;
}

int shape_rank2rank(const Shape &shape_rank, const Shape &shape) { //将每个维度的分量转化成循环量rank，加上取模操作方便broadcast操作，所以使用时需要额外注意
    int _dim = shape.size(), rank = 0;
    for (int d = 1; d < _dim; d++) {
        rank += (shape_rank[d - 1] % shape[d - 1]);
        rank *= shape[d];
    }
    rank += (shape_rank[_dim - 1] % shape[_dim - 1]);
    return rank;
}

int rank2count(const int &rank, const Shape &shape) {
    auto shape_rank = rank2shape_rank(rank, shape);
    int dim = shape_rank.size(), count = 0;
    for (int d = dim - 1; d >= 0; d--)
        if (shape_rank[d] == 0) count++;
        else break;
    return count;
}

bool Tensor::broadcast_cap(const Tensor &t) const {
    bool cap = true;
    for (int d = 0; d < _dim; d++)
        if (shape_size(d) != 1 && t.shape_size(d) != 1 && shape_size(d) != t.shape_size(d))
            return !cap;
    return cap;
}

//只针对二维Tensor
Tensor &Tensor::transpose() {
    if (_dim != 2) throw ErrMsg;
    if (shape_size(0) == 1 || shape_size(1) == 1) std::swap(_shape[0], _shape[1]);
    else {
        Elem new_elem(size());
        for (int row = 0; row < _shape[1]; row++)
            for (int col = 0; col < _shape[0]; col++)
                new_elem[row * _shape[0] + col] = elem(col * _shape[1] + row);
        _elem = new_elem;
        std::swap(_shape[0], _shape[1]);
    }
    return *this;
}

Tensor &Tensor::reshape(const Shape &new_shape){
    if (!size_cap(new_shape)) throw ErrMsg;
    _dim = new_shape.size(); //更新维度
    _shape.resize(_dim);
    for (int d = 0; d < _dim; d++)
        _shape[d] = new_shape[d]; //更新每一维尺寸
    return *this;
}

Tensor Tensor::broadcast_sum(const Tensor &t) const {
    if (!broadcast_cap(t)) throw ErrMsg;
    Shape new_shape(_dim), shape_rank(_dim);
    for (int d = 0; d < _dim; d++) new_shape[d] = std::max(shape_size(d), t.shape_size(d)); //判断可以broadcast后只需要取最大即可
    int new_size = shape2size(new_shape);
    Elem new_elem(new_size);
    for (int rank = 0; rank < new_size; rank++) {
        shape_rank = rank2shape_rank(rank, new_shape); //取出对应的每个维度的分量
        new_elem[rank] = elem(shape_rank) + t.elem(shape_rank); //这里按之前所说，模两个Tensor在每个维度的尺寸，即可做到broadcast，所以在shape_rank2rank中采用了取模操作
    }
    Tensor sum = Tensor(new_shape, new_elem);
    return sum;
}

Tensor Tensor::broadcast_min(const Tensor &t) const {
    return broadcast_sum(-t);
}

Tensor Tensor::broadcast_mul(const Tensor &t) const {
    if (!broadcast_cap(t)) throw ErrMsg;
    Shape new_shape(_dim), shape_rank(_dim);
    for (int d = 0; d < _dim; d++) new_shape[d] = std::max(shape_size(d), t.shape_size(d)); //判断可以broadcast后只需要取最大即可
    int new_size = shape2size(new_shape);
    Elem new_elem(new_size);
    for (int rank = 0; rank < new_size; rank++) {
        shape_rank = rank2shape_rank(rank, new_shape); //取出对应的每个维度的分量
        new_elem[rank] = elem(shape_rank) * t.elem(shape_rank); //这里按之前所说，模两个Tensor在每个维度的尺寸，即可做到broadcast，所以在shape_rank2rank中采用了取模操作
    }
    Tensor prod = Tensor(new_shape, new_elem);
    return prod;
}

Tensor Tensor::broadcast_div(const Tensor &t) const {
    if (!broadcast_cap(t)) throw ErrMsg;
    Shape new_shape(_dim), shape_rank(_dim);
    for (int d = 0; d < _dim; d++) new_shape[d] = std::max(shape_size(d), t.shape_size(d)); //判断可以broadcast后只需要取最大即可
    int new_size = shape2size(new_shape);
    Elem new_elem(new_size);
    for (int rank = 0; rank < new_size; rank++) {
        shape_rank = rank2shape_rank(rank, new_shape); //取出对应的每个维度的分量
        new_elem[rank] = elem(shape_rank) / t.elem(shape_rank); //这里按之前所说，模两个Tensor在每个维度的尺寸，即可做到broadcast，所以在shape_rank2rank中采用了取模操作
    }
    Tensor prod = Tensor(new_shape, new_elem);
    return prod;
}

Tensor Tensor::concat(const Tensor &t, const int &op_dim) const { //大致思路是：通过新Tensor当前rank对应的各维度分量，如果在op_dim分量小于this在op_dim的尺寸，就应该是this的元素，否则是t的
    for (int d = 0; d < _dim; d++)
        if (d != op_dim && !shape_cap(t, d)) //判断其余维度的相容性
            throw ErrMsg;
    Shape new_shape = _shape, shape_rank(_dim);
    new_shape[op_dim] += t.shape_size(op_dim);
    int new_size = shape2size(new_shape);
    Elem new_elem(new_size);
    for (int rank = 0; rank < new_size; rank++) {
        shape_rank = rank2shape_rank(rank, new_shape);
        if (shape_rank[op_dim] < shape_size(op_dim))
            new_elem[rank] = elem(shape_rank);
        else {
            shape_rank[op_dim] -= shape_size(op_dim);
            new_elem[rank] = t.elem(shape_rank);
        }
    }
    return Tensor(new_shape, new_elem);
}

Tensor Tensor::relu() const {
    int new_size = size();
    Elem new_elem(new_size);
    for (int rank = 0; rank < new_size; rank++)
        new_elem[rank] = std::max(0.0, _elem[rank]);
    return Tensor(shape(), new_elem);
}

Tensor Tensor::der_relu() const {
    int new_size = size();
    Elem new_elem(new_size);
    for (int rank = 0; rank < new_size; rank++)
        new_elem[rank] = (_elem[rank] > 0) ? 1.0 : 0.0;
    return Tensor(shape(), new_elem);
}

Tensor Tensor::softmax() const {
    int new_size = size();
    Elem new_elem(new_size);
    double sum = 0.0;
    for (auto it : _elem) sum += exp(it);
    for (int rank = 0; rank < new_size; rank++)
        new_elem[rank] = exp(_elem[rank]) / sum;
    return Tensor(shape(), new_elem);
}

Tensor Tensor::sqrt() const {
    int new_size = size();
    Elem new_elem(new_size);
    for (int rank = 0; rank < new_size; rank++)
        new_elem[rank] = std::sqrt(_elem[rank]);
    return Tensor(shape(), new_elem);
}

Tensor Tensor::reduce_sum(const int &op_dim) const {
    auto new_shape = _shape; new_shape[op_dim] = 1;
    auto old_size = size(), new_size = shape2size(new_shape);
    auto sum = Tensor(new_shape, 0.0);
    for (int rank = 0; rank < old_size; rank++) {
        auto shape_rank = rank2shape_rank(rank, _shape);
        sum.elem(shape_rank) += elem(rank); //这里实际上会做一次模的操作，参看shape_rank2rank
    }
    new_shape.erase(new_shape.begin() + op_dim);
    sum.reshape(new_shape);
    return sum;
}

Tensor Tensor::reduce_mul(const int &op_dim) const {
    auto new_shape = _shape; new_shape[op_dim] = 1;
    auto old_size = size(), new_size = shape2size(new_shape);
    auto prod = Tensor(new_shape, 1.0);
    for (int rank = 0; rank < old_size; rank++) {
        auto shape_rank = rank2shape_rank(rank, _shape);
        prod.elem(shape_rank) *= elem(rank); //这里实际上会做一次模的操作，参看shape_rank2rank
    }
    new_shape.erase(new_shape.begin() + op_dim);
    prod.reshape(new_shape);
    return prod;
}

Tensor Tensor::onehot(const int &size_) const {
    if (size() > 1) throw ErrMsg;
    auto new_shape = Shape({size_, 1});
    auto res = Tensor(new_shape, 0.);
    res.elem(int(elem(0))) = 1.;
    return res;
}

double Tensor::norm() const {
    double res = 0.0;
    for (auto it : _elem) res += it * it;
    return std::sqrt(res);
}

int Tensor::argmax() const {
    double max_elem = elem(0); int max_rank = 0;
    int total_size = size();
    for (int i = 1; i < total_size; i++) 
        if (elem(i) > max_elem) {
            max_elem = elem(i); max_rank = i;
        }
    return max_rank;
}

Tensor Tensor::operator+(const Tensor &t) { //按正常的矩阵加法，也可以调用broadcast_sum
    return broadcast_sum(t);
}

Tensor Tensor::operator-() const {
    return broadcast_mul(Tensor(shape(), -1.));
}

Tensor Tensor::operator-(const Tensor &t) {
    return *this + (-t);
}

Tensor Tensor::operator*(const Tensor &t) {
    if (dim() != 2 || t.dim() != 2 || shape_size(1) != t.shape_size(0)) throw ErrMsg;
    Shape new_shape(2); new_shape[0] = shape_size(0); new_shape[1] = t.shape_size(1); 
    int new_size = shape2size(new_shape);
    Elem new_elem(new_size);
    for (int row = 0; row < new_shape[0]; row++) {
        for (int col = 0; col < new_shape[1]; col++) {
            double val = 0.0;
            for (int i = 0; i < shape_size(1); i++)
                val += elem(row * shape_size(1) + i) * t.elem(i * t.shape_size(1) + col);
            new_elem[row * new_shape[1] + col] = val;
        }
    }
    Tensor prod = Tensor(new_shape, new_elem);
    return prod;
}

Tensor Tensor::operator*(const double &d) {
    Shape new_shape(dim());
    for (auto it = new_shape.begin(); it != new_shape.end(); it++) *it = 1;
    return broadcast_mul(Tensor(new_shape, d));
}

Tensor get_normal_tensor(const Shape &shape) {
    int new_size = shape2size(shape); Elem new_elem(new_size);
    for (int rank = 0; rank < new_size; rank++)
        new_elem[rank] = gaussian(engine);
    return Tensor(shape, new_elem);
}
