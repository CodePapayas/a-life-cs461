#include <cmath>
#include <vector>
#include <numeric>
#include <random>
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

// Dot Product Function [adapted heavily from https://www.educative.io/answers/dot-product-of-two-vectors-in-cpp]
// computes the sum of element-wise products of two vectors.
double dot_product(std::vector<double> v1, std::vector<double> v2) {
    double result = 0;
    for (int i = 0; i < v1.size(); ++i) {
        result += v1[i] * v2[i];
    }
    return result;
}

ActivationLayerReLU::ActivationLayerReLU(int input_size, int output_size) {
    n_in = input_size;
    n_out = output_size;
    weights.resize(input_size * output_size);
    biases.resize(n_out);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    // Initialize biases with 0's
    for (double& b : biases) {
        b = dist(gen);
    }
    // Initialize weights with random values between [-1, 1] inclusive
    for (double& w : weights) {
        w = dist(gen);
    }

}

const std::vector<double>& ActivationLayerReLU::get_biases() const{
    return biases;
}

const std::vector<double>& ActivationLayerReLU::get_weights() const{
    return weights;
}
