/*
  Author: Kai Lindskog-Coffin
  Oregon State University
  CS 462
*/

#include "LiveVisualizer.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <csignal>

using namespace std;

// Global flag flipped by SIGINT (Ctrl+C) so the loop exits cleanly
static volatile sig_atomic_t g_running = 1;
static void handleSignal(int) { g_running = 0; }

LiveVisualizer::LiveVisualizer(shared_ptr<DBConnector> db, int pollIntervalSeconds, int historyLimit)
    : m_db(db)
    , m_viz(db)
    , m_pollInterval(pollIntervalSeconds)
    , m_historyLimit(historyLimit)
{}

void LiveVisualizer::run() {
    signal(SIGINT, handleSignal);

    cout << "\033[?25l" << flush;  // Hide cursor once.

    while (g_running) {
        vector<SnapshotData> data = m_viz.fetchHistory(m_historyLimit);

        // Build entire frame into a buffer so we can count lines before emitting.
        ostringstream buf;

        buf << "\033[1;36m"
            << "╔══════════════════════════════════════════════════════╗\n"
            << "║        A-LIFE SIMULATION LIVE STATS MONITOR         ║\n"
            << "║  Poll interval: " << m_pollInterval << "s  |  Showing last "
            << setw(3) << m_historyLimit << " ticks          ║\n"
            << "║  Press Ctrl+C to exit                               ║\n"
            << "╚══════════════════════════════════════════════════════╝\n"
            << "\033[0m";

        if (data.empty()) {
            buf << "\n  \033[33mWaiting for simulation data...\033[0m  (Is main_exe running?)\n";
        } else {
            drawSummaryTable(data, buf);
            drawCharts(data, buf);
        }

        const string frame = buf.str();

        // Count newlines to know how far to retreat on the next redraw.
        int lines = static_cast<int>(count(frame.begin(), frame.end(), '\n'));

        // Move cursor up by last frame's line count and erase to end — in-place redraw, no flash.
        if (m_lastFrameLines > 0)
            cout << "\033[" << m_lastFrameLines << "A\033[J";

        cout << frame << flush;
        m_lastFrameLines = lines;

        this_thread::sleep_for(chrono::seconds(m_pollInterval));
    }

    cout << "\033[?25h";   // Restore cursor.
    cout << "\n\033[32mMonitor stopped.\033[0m\n";
}

void LiveVisualizer::drawSummaryTable(const vector<SnapshotData>& data, ostream& out) const {
    const SnapshotData& latest = data.back();

    out << "\n\033[1mLatest Snapshot\033[0m\n";
    out << "  Tick           : " << latest.tick          << "\n";
    out << "  Agents Alive   : " << latest.agentCount    << "\n";
    out << "  Resources      : " << latest.resourceCount << "\n";
    out << fixed << setprecision(4);
    out << "  Total Energy   : " << latest.totalEnergy   << "\n";
    out << "  Avg Fitness    : " << latest.avgFitness    << "\n";

    if (data.size() >= 2) {
        double delta = data.back().totalEnergy - data[data.size()-2].totalEnergy;
        string arrow = (delta > 0) ? "\033[32m▲\033[0m" : (delta < 0) ? "\033[31m▼\033[0m" : "=";
        out << "  Energy Trend   : " << arrow << "\n";
    }
    out << "\n";
}

void LiveVisualizer::drawCharts(const vector<SnapshotData>& data, ostream& out) const {
    // DataVisualizer writes to cout; redirect it into our frame buffer.
    auto* prev = cout.rdbuf(out.rdbuf());
    m_viz.plotAgentCount(data);
    m_viz.plotTotalEnergy(data);
    cout.rdbuf(prev);
}