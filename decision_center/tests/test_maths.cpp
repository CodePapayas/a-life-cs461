#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../brain.hpp"

TEST_CASE("Activation Function Tests") {
    // ReLU Tests
    CHECK(relu(1) == 1);
    CHECK(relu(-1) == 0);

    // Sigmoid Tests
    double log_x = std::log(3);
    CHECK(sigmoid(0) == 0.5);
    CHECK(sigmoid(log_x) == 0.75);
    CHECK(sigmoid(-log_x) == 0.25);

    // Dot Product Tests
    std::vector<double> vector1 = {1.0, 4.0, 5.2};
    std::vector<double> vector2 = {1.5, 4.5, 5.5};
    std::vector<double> vector3 = {1.0, 4.0, 5.2, 6.9, 6.7, 0.0, 2.35, 3.25435};
    std::vector<double> vector4 = {1.5, 4.5, 5.5, 9.6, 7.6, 10.0, 3.52, 6.25435};
    std::vector<double> vectorNeg1 = {-1.0, 4.0, -5.2};
    std::vector<double> vectorNeg2 = {1.5, -4.5, 5.5};
    CHECK(dot_product(vector1, vector2) == 48.1);
    CHECK(dot_product(vectorNeg1, vectorNeg2) == -48.1);
    CHECK(dot_product(vector3, vector4) == doctest::Approx(193.886).epsilon(1e-6));

}