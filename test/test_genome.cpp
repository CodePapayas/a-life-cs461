/* Genome unit tests
   Simple in-repo test harness matching existing test style
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "../source/entity/decision_center/genome.hpp"

using namespace std;

int totalTests = 0, passedTests = 0;
#define TEST(name) totalTests++; cout << name << "... "; bool ok = true;
#define CHECK(cond) if (!(cond)) { ok = false; cout << "FAIL@" << __LINE__ << " "; }
#define END_TEST() if (ok) { cout << "PASS"; passedTests++; } cout << endl;

double approx_eq(double a, double b, double eps=0.02) { return std::abs(a-b) <= eps; }

void test_roundtrip()
{
    TEST("Genome encode/decode roundtrip")
    auto keys = Genome::trait_order();
    unordered_map<string,double> traits;
    for (size_t i = 0; i < keys.size(); ++i) {
        traits[keys[i]] = (i + 1) / static_cast<double>(keys.size());
    }
    auto bytes = Genome::encodeFromTraits(traits);
    auto decoded = Genome::decodeToTraits(bytes);
    for (const auto &k : keys) {
        CHECK( approx_eq(decoded[k], traits[k], 1.0/255.0 + 0.02) );
    }
    END_TEST()
}

void test_mutation()
{
    TEST("Genome mutation changes bytes")
    auto keys = Genome::trait_order();
    vector<uint8_t> bytes(keys.size(), 128);
    auto before = bytes;
    Genome::mutateBytes(bytes, 1.0 /*rate*/, 1.0 /*magnitude*/);
    bool any_diff = false;
    for (size_t i = 0; i < bytes.size(); ++i) if (bytes[i] != before[i]) { any_diff = true; break; }
    CHECK(any_diff);
    END_TEST()
}

void test_crossover()
{
    TEST("Genome crossover mixes parents")
    auto keys = Genome::trait_order();
    vector<uint8_t> a(keys.size(), 0);
    vector<uint8_t> b(keys.size(), 255);
    bool mixed_found = false;
    bool any_run_success = false;
    for (int i = 0; i < 8; ++i) {
        auto child = Genome::crossover(a,b);
        if (child.size() != keys.size()) continue;
        any_run_success = true;
        bool all_from_a = all_of(child.begin(), child.end(), [](uint8_t v){ return v == 0; });
        bool all_from_b = all_of(child.begin(), child.end(), [](uint8_t v){ return v == 255; });
        if (!all_from_a && !all_from_b) { mixed_found = true; break; }
    }
    CHECK(any_run_success && mixed_found);
    END_TEST()
}

int main() {
    cout << "=======================" << endl;
    cout << "Genome Unit Tests" << endl;
    cout << "=======================" << endl;

    test_roundtrip();
    test_mutation();
    test_crossover();

    cout << "=======================" << endl;
    cout << passedTests << "/" << totalTests << " tests passed";
    if (passedTests == totalTests) cout << " ✓";
    cout << endl << "=======================" << endl;

    return passedTests == totalTests ? 0 : 1;
}
