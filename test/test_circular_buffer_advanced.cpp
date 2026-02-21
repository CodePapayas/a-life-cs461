/*
  Author: Kai Lindskog-Coffin
  Oregon State University
  CS 462 
  
  Advanced CircularBuffer Tests
  Coverage: Edge cases, exceptions, boundary conditions, move semantics
*/

#include "../include/circular_buffer.h"
#include "../include/simulation_state.h"
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <string>

using namespace std;

int totalTests = 0, passedTests = 0;

#define TEST(name) totalTests++; cout << "[TEST] " << name << "... "; bool ok = true;
#define CHECK(cond) if (!(cond)) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #cond; }
#define CHECK_THROWS(expr, exception) try { expr; ok = false; cout << "\n  FAIL at line " << __LINE__ << ": Expected exception not thrown"; } \
    catch (const exception&) { } catch (...) { ok = false; cout << "\n  FAIL: Wrong exception type"; }
#define END_TEST() if (ok) { cout << "PASS"; passedTests++; } else { cout << endl; } cout << endl;

// Test 1: Exception handling for invalid capacity
void testInvalidCapacity() {
    TEST("CircularBuffer - Invalid capacity throws exception")
    
    CHECK_THROWS(CircularBuffer<int>(0), invalid_argument);
    
    END_TEST()
}

// Test 2: Exception handling for empty buffer operations
void testEmptyBufferExceptions() {
    TEST("CircularBuffer - Empty buffer exceptions")
    
    CircularBuffer<int> buf(5);
    CHECK(buf.empty());
    
    // latest() should throw on empty buffer
    CHECK_THROWS(buf.latest(), runtime_error);
    
    // rewind() should throw on empty buffer
    CHECK_THROWS(buf.rewind(0), out_of_range);
    
    // get() should throw on out of range
    CHECK_THROWS(buf.get(0), out_of_range);
    
    END_TEST()
}

// Test 3: Boundary conditions for rewind
void testRewindBoundaries() {
    TEST("CircularBuffer - Rewind boundary conditions")
    
    CircularBuffer<int> buf(5);
    for (int i = 0; i < 3; i++) {
        buf.push(i * 10);
    }
    
    // Valid rewinds: 0, 1, 2 (for size=3)
    CHECK(buf.rewind(0) == 20);  // Most recent
    CHECK(buf.rewind(1) == 10);  // One back
    CHECK(buf.rewind(2) == 0);   // Oldest
    
    // Invalid: trying to rewind beyond history
    CHECK_THROWS(buf.rewind(3), out_of_range);
    CHECK_THROWS(buf.rewind(100), out_of_range);
    
    END_TEST()
}

// Test 4: Wraparound behavior with exactly capacity elements
void testExactCapacityWraparound() {
    TEST("CircularBuffer - Exact capacity wraparound")
    
    CircularBuffer<int> buf(4);
    
    // Fill exactly to capacity
    for (int i = 0; i < 4; i++) {
        buf.push(i);  // 0, 1, 2, 3
    }
    
    CHECK(buf.full());
    CHECK(buf.size() == 4);
    CHECK(buf.get(0) == 0);  // Oldest
    CHECK(buf.latest() == 3);  // Newest
    
    // Push one more - should overwrite index 0
    buf.push(4);
    CHECK(buf.full());
    CHECK(buf.size() == 4);
    CHECK(buf.get(0) == 1);  // 0 was overwritten
    CHECK(buf.latest() == 4);
    
    // Continue pushing
    buf.push(5);
    buf.push(6);
    CHECK(buf.get(0) == 3);  // Should now be 3,4,5,6
    CHECK(buf.get(1) == 4);
    CHECK(buf.get(2) == 5);
    CHECK(buf.get(3) == 6);
    
    END_TEST()
}

// Test 5: Multiple complete wraparounds
void testMultipleWraparounds() {
    TEST("CircularBuffer - Multiple complete wraparounds")
    
    CircularBuffer<int> buf(3);
    
    // Push 10 items through a buffer of size 3
    for (int i = 0; i < 10; i++) {
        buf.push(i);
    }
    
    // Should contain the last 3 items: 7, 8, 9
    CHECK(buf.size() == 3);
    CHECK(buf.get(0) == 7);
    CHECK(buf.get(1) == 8);
    CHECK(buf.get(2) == 9);
    CHECK(buf.latest() == 9);
    CHECK(buf.rewind(2) == 7);
    
    END_TEST()
}

// Test 6: Move semantics
void testMoveSemantics() {
    TEST("CircularBuffer - Move semantics")
    
    CircularBuffer<string> buf(3);
    
    // Use move semantics to push
    string s1 = "Hello";
    string s2 = "World";
    string s3 = "Test";
    
    buf.push(move(s1));
    buf.push(move(s2));
    buf.push(move(s3));
    
    CHECK(buf.get(0) == "Hello");
    CHECK(buf.get(1) == "World");
    CHECK(buf.get(2) == "Test");
    
    // Original strings should be moved from (empty or in valid but unspecified state)
    // We can't really test this reliably, but the push should work
    
    END_TEST()
}

// Test 7: Clear and reuse
void testClearAndReuse() {
    TEST("CircularBuffer - Clear and reuse")
    
    CircularBuffer<int> buf(5);
    
    // Fill buffer
    for (int i = 0; i < 7; i++) {
        buf.push(i);
    }
    CHECK(buf.size() == 5);
    CHECK(buf.full());
    
    // Clear
    buf.clear();
    CHECK(buf.empty());
    CHECK(buf.size() == 0);
    CHECK(!buf.full());
    
    // Reuse after clear
    buf.push(100);
    buf.push(200);
    CHECK(buf.size() == 2);
    CHECK(buf.get(0) == 100);
    CHECK(buf.get(1) == 200);
    
    END_TEST()
}

// Test 8: Large capacity buffer
void testLargeCapacity() {
    TEST("CircularBuffer - Large capacity (1000 elements)")
    
    CircularBuffer<int> buf(1000);
    
    // Fill to capacity
    for (int i = 0; i < 1000; i++) {
        buf.push(i);
    }
    
    CHECK(buf.size() == 1000);
    CHECK(buf.full());
    CHECK(buf.get(0) == 0);
    CHECK(buf.get(999) == 999);
    CHECK(buf.latest() == 999);
    
    // Overflow
    buf.push(1000);
    CHECK(buf.get(0) == 1);  // 0 was overwritten
    CHECK(buf.latest() == 1000);
    
    END_TEST()
}

// Test 9: Single element buffer
void testSingleElementBuffer() {
    TEST("CircularBuffer - Single element capacity")
    
    CircularBuffer<int> buf(1);
    
    buf.push(42);
    CHECK(buf.size() == 1);
    CHECK(buf.full());
    CHECK(buf.latest() == 42);
    CHECK(buf.get(0) == 42);
    CHECK(buf.rewind(0) == 42);
    
    // Overflow - should replace the single element
    buf.push(99);
    CHECK(buf.size() == 1);
    CHECK(buf.get(0) == 99);
    CHECK(buf.latest() == 99);
    
    END_TEST()
}

// Test 10: Complex object storage (SimulationState)
void testComplexObjectStorage() {
    TEST("CircularBuffer - Complex object storage")
    
    CircularBuffer<SimulationState> buf(5);
    
    for (int i = 0; i < 7; i++) {
        SimulationState state;
        state.tick = i * 100;
        state.agentCount = i * 5;
        state.totalEnergy = i * 10.5;
        state.timestamp = i * 0.1;
        buf.push(state);
    }
    
    // Should have last 5 states (indices 2-6)
    CHECK(buf.size() == 5);
    CHECK(buf.get(0).tick == 200);
    CHECK(buf.latest().tick == 600);
    CHECK(buf.rewind(1).agentCount == 25);  // i=5
    CHECK(abs(buf.rewind(4).totalEnergy - 21.0) < 0.01);  // i=2
    
    END_TEST()
}

// Test 11: Index access patterns
void testIndexAccessPatterns() {
    TEST("CircularBuffer - Various index access patterns")
    
    CircularBuffer<int> buf(5);
    
    // Partial fill
    buf.push(10);
    buf.push(20);
    buf.push(30);
    
    CHECK(buf.get(0) == 10);
    CHECK(buf.get(1) == 20);
    CHECK(buf.get(2) == 30);
    
    // Out of range for current size
    CHECK_THROWS(buf.get(3), out_of_range);
    CHECK_THROWS(buf.get(5), out_of_range);
    CHECK_THROWS(buf.get(100), out_of_range);
    
    // Fill to capacity and overflow
    buf.push(40);
    buf.push(50);
    buf.push(60);  // Overwrites 10
    
    CHECK(buf.get(0) == 20);
    CHECK(buf.get(4) == 60);
    
    END_TEST()
}

// Test 12: Consistency after many operations
void testConsistencyAfterManyOperations() {
    TEST("CircularBuffer - Consistency after many operations")
    
    CircularBuffer<int> buf(10);
    
    // Push, clear, push pattern
    for (int i = 0; i < 5; i++) {
        buf.push(i);
    }
    buf.clear();
    
    for (int i = 0; i < 8; i++) {
        buf.push(i * 2);
    }
    
    CHECK(buf.size() == 8);
    CHECK(buf.get(0) == 0);
    CHECK(buf.latest() == 14);
    
    // Continue to overflow
    for (int i = 0; i < 5; i++) {
        buf.push(100 + i);
    }
    
    CHECK(buf.size() == 10);
    CHECK(buf.full());
    // Should have: 6, 8, 10, 12, 14, 100, 101, 102, 103, 104
    CHECK(buf.get(0) == 6);
    CHECK(buf.latest() == 104);
    
    END_TEST()
}

int main() {
    cout << "========================================" << endl;
    cout << "CIRCULAR BUFFER ADVANCED TEST SUITE" << endl;
    cout << "========================================" << endl << endl;
    
    testInvalidCapacity();
    testEmptyBufferExceptions();
    testRewindBoundaries();
    testExactCapacityWraparound();
    testMultipleWraparounds();
    testMoveSemantics();
    testClearAndReuse();
    testLargeCapacity();
    testSingleElementBuffer();
    testComplexObjectStorage();
    testIndexAccessPatterns();
    testConsistencyAfterManyOperations();
    
    cout << endl << "========================================" << endl;
    cout << "Results: " << passedTests << "/" << totalTests << " tests passed";
    if (passedTests == totalTests) {
        cout << " ✓" << endl;
    } else {
        cout << " ✗" << endl;
    }
    cout << "Coverage: Edge cases, exceptions, boundary conditions" << endl;
    cout << "========================================" << endl;
    
    return passedTests == totalTests ? 0 : 1;
}
