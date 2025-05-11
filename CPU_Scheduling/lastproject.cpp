#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int start_time;
    int completion_time;
    bool started;
};

void simulateRR(const vector<Process>& procs_input, int quantum) {
    vector<Process> procs = procs_input;
    int n = procs.size();
    vector<int> timeline;
    int time = 0, completed = 0, idx = 0;
    queue<int> rq;

    for (auto &p : procs) {
        p.remaining_time = p.burst_time;
        p.start_time = -1;
        p.started = false;
    }
    sort(procs.begin(), procs.end(), [](const Process &a, const Process &b) {
        return a.arrival_time < b.arrival_time;
    });

    // simulation loop
    while (completed < n) {
        while (idx < n && procs[idx].arrival_time <= time) {
            rq.push(idx);
            idx++;
        }
        if (rq.empty()) {
            time++;
            continue;
        }

        int i = rq.front(); rq.pop();
        if (!procs[i].started) {
            procs[i].start_time = time;
            procs[i].started = true;
        }

        int slice = min(quantum, procs[i].remaining_time);
        for (int t = 0; t < slice; ++t) {
            timeline.push_back(procs[i].pid);
            procs[i].remaining_time--;
            time++;
            while (idx < n && procs[idx].arrival_time <= time) {
                rq.push(idx);
                idx++;
            }
        }

        if (procs[i].remaining_time == 0) {
            procs[i].completion_time = time;
            completed++;
        } else {
            rq.push(i);
        }
    }

    // printing the results
    cout << "\nRound Robin (Q=" << quantum << ") results:\n";
    cout << "PID  Arrival  Burst  Start  Complete  Turnaround  Waiting  Response\n";
    double total_tat = 0, total_wt = 0, total_rt = 0;
    for (const auto &p : procs) {
        int tat = p.completion_time - p.arrival_time;
        int wt  = tat - p.burst_time;
        int rt  = p.start_time - p.arrival_time;
        total_tat += tat;
        total_wt  += wt;
        total_rt  += rt;
        cout << setw(3) << p.pid
             << setw(9) << p.arrival_time
             << setw(7) << p.burst_time
             << setw(7) << p.start_time
             << setw(11) << p.completion_time
             << setw(13) << tat
             << setw(8) << wt
             << setw(9) << rt << "\n";
    }
    cout << fixed << setprecision(2)
         << "Avg TAT: " << total_tat/n
         << ", Avg WT: "   << total_wt/n
         << ", Avg RT: "   << total_rt/n << "\n";

    cout << "\nTime Interval | Running Process\n";
    int start = 0;
    int last_pid = timeline[0];
    for (int t = 1; t <= (int)timeline.size(); ++t) {
        int pid = (t < (int)timeline.size() ? timeline[t] : -1);
        if (pid != last_pid) {
            cout << setw(2) << start << " - " << setw(2) << t << "       "
                 << 'P' << last_pid << "\n";
            start = t;
            last_pid = pid;
        }
    }
}

void simulateSRTF(const vector<Process>& procs_input) {
    vector<Process> procs = procs_input;
    int n = procs.size();
    vector<int> timeline;
    int time = 0, completed = 0, idx = 0;
    auto cmp = [&](int a, int b) {
        return procs[a].remaining_time > procs[b].remaining_time;
    };
    priority_queue<int, vector<int>, decltype(cmp)> pq(cmp);

    for (auto &p : procs) {
        p.remaining_time = p.burst_time;
        p.start_time = -1;
        p.started = false;
    }
    sort(procs.begin(), procs.end(), [](const Process &a, const Process &b) {
        return a.arrival_time < b.arrival_time;
    });

    // simulation loop
    while (completed < n) {
        while (idx < n && procs[idx].arrival_time <= time) {
            pq.push(idx);
            idx++;
        }
        if (pq.empty()) {
            time++;
            continue;
        }

        int i = pq.top(); pq.pop();
        if (!procs[i].started) {
            procs[i].start_time = time;
            procs[i].started = true;
        }

        timeline.push_back(procs[i].pid);
        procs[i].remaining_time--;
        time++;

        while (idx < n && procs[idx].arrival_time <= time) {
            pq.push(idx);
            idx++;
        }

        if (procs[i].remaining_time == 0) {
            procs[i].completion_time = time;
            completed++;
        } else {
            pq.push(i);
        }
    }

    // printing the results
    cout << "\nPreemptive SJF (SRTF) results:\n";
    cout << "PID  Arrival  Burst  Start  Complete  Turnaround  Waiting  Response\n";
    double total_tat = 0, total_wt = 0, total_rt = 0;
    for (const auto &p : procs) {
        int tat = p.completion_time - p.arrival_time;
        int wt  = tat - p.burst_time;
        int rt  = p.start_time - p.arrival_time;
        total_tat += tat;
        total_wt  += wt;
        total_rt  += rt;
        cout << setw(3) << p.pid
             << setw(9) << p.arrival_time
             << setw(7) << p.burst_time
             << setw(7) << p.start_time
             << setw(11) << p.completion_time
             << setw(13) << tat
             << setw(8) << wt
             << setw(9) << rt << "\n";
    }
    cout << fixed << setprecision(2)
         << "Avg TAT: " << total_tat/n
         << ", Avg WT: "   << total_wt/n
         << ", Avg RT: "   << total_rt/n << "\n";

    cout << "\nTime Interval | Running Process\n";
    int start2 = 0;
    int last_pid2 = timeline[0];
    for (int t = 1; t <= (int)timeline.size(); ++t) {
        int pid = (t < (int)timeline.size() ? timeline[t] : -1);
        if (pid != last_pid2) {
            cout << setw(2) << start2 << " - " << setw(2) << t << "       "
                 << 'P' << last_pid2 << "\n";
            start2 = t;
            last_pid2 = pid;
        }
    }
}

int main() {

    vector<Process> procs = {
        {1, 0, 24},
        {2, 1, 3},
        {3, 2, 3}
    };
    int quantum = 4;

    simulateRR(procs, quantum);
    simulateSRTF(procs);

    return 0;
}
