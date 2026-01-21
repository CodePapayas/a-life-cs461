#include <cmath>
#include <vector>
#include <numeric>
#include "brain.hpp"

// Hyperbolic Tangent (tanh) Activation Function
// maps any real output to between [-1, 1]
double tanh_func(double x) {
    return std::tanh(x);
}

// Sigmoid Activation Function
// maps any real output to between [0, 1]
double sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

// Rectified Linear Unit (ReLU) Activation Function
// outputs input directly if it is positive; otherwise outputs zero.
double relu(double x) {
    return x > 0 ? x : 0;
}

// Dot Product Function
// computes the sum of element-wise products of two vectors.
double dot_product(std::vector<double> v1, std::vector<double> v2) {
    double result = 0;
    for (int i = 0; i < v1.size(); ++i) {
        result += v1[i] * v2[i];
    }
    return result;
}

double ActivationLayerRelu(double x) {
    return x;
}