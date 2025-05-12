#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <random>
#include <fstream>
#include <filesystem>
#include <functional>


using namespace std;

struct Process {
    int pid; //proc id
    int arrival_time; //when a proc arrives in the ready queue
    int burst_time; //cpu burst time
    int remaining_time; //the remaining time of the proc to finish execution
    int start_time; //the time when the proc starts executing
    int completion_time; //the time when the proc finish execution
    bool started; //flag to check if a proc started execution
};

struct ProcessStats {
    int pid;
    vector<int> tat_values;
    vector<int> wt_values;
    vector<int> rt_values;
};


float average(const vector<int>& data) {
    float sum = 0;
    for (int val : data) sum += val;
    return sum / data.size();
}

float stddev(const vector<int>& data) {
    float avg = average(data);
    float sum_sq = 0;
    for (int val : data) {
        float diff = val - avg;
        sum_sq += diff * diff;
    }
    return sqrt(sum_sq / data.size());
}

float median(vector<int> data) {
    sort(data.begin(), data.end());
    int n = data.size();
    if (n % 2 == 0)
        return (data[n/2 - 1] + data[n/2]) / 2.0;
    else
        return data[n/2];
}

void printSummary(const string& algo_name, const vector<ProcessStats>& stats) {
    vector<int> all_tat, all_wt, all_rt;

    for (const auto& ps : stats) {
        all_tat.insert(all_tat.end(), ps.tat_values.begin(), ps.tat_values.end());
        all_wt.insert(all_wt.end(), ps.wt_values.begin(), ps.wt_values.end());
        all_rt.insert(all_rt.end(), ps.rt_values.begin(), ps.rt_values.end());
    }

    cout << "\n==== " << algo_name << " Summary Statistics over all runs ====\n";
    cout << "Metric      | Average   | Std Dev   | Median\n";
    cout << "------------+-----------+-----------+--------\n";
    cout << "Turnaround  | " << setw(9) << fixed << setprecision(2) << average(all_tat)
         << " | " << setw(9) << stddev(all_tat)
         << " | " << median(all_tat) << "\n";

    cout << "Waiting     | " << setw(9) << average(all_wt)
         << " | " << setw(9) << stddev(all_wt)
         << " | " << median(all_wt) << "\n";

    cout << "Response    | " << setw(9) << average(all_rt)
         << " | " << setw(9) << stddev(all_rt)
         << " | " << median(all_rt) << "\n";
}


void exportStatsToCSV(const string& filename, const vector<ProcessStats>& stats) {
    ofstream file(filename);
    file << "PID,Turnaround,Waiting,Response\n";
    for (const auto& ps : stats) {
        for (size_t i = 0; i < ps.tat_values.size(); ++i) {
            file << ps.pid << "," << ps.tat_values[i] << "," << ps.wt_values[i] << "," << ps.rt_values[i] << "\n";
        }
    }
    file.close();
}


void simulateRR(const vector<Process>& procs_input, int quantum, vector<ProcessStats>& stats){
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
         << "Avg TAT: " << total_tat/n //turnaround time = completion time - arrival time
         << ", Avg WT: "   << total_wt/n //waiting time = turnaround time - burst time
         << ", Avg RT: "   << total_rt/n << "\n"; //response time = start time - arrival time

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
    for (const auto &p : procs) {
        stats[p.pid].pid = p.pid;
        stats[p.pid].tat_values.push_back(p.completion_time - p.arrival_time);
        stats[p.pid].wt_values.push_back((p.completion_time - p.arrival_time) - p.burst_time);
        stats[p.pid].rt_values.push_back(p.start_time - p.arrival_time);
    }
    
    
}

void simulateSRTF(const vector<Process>& procs_input, vector<ProcessStats>& stats){
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

    for (const auto &p : procs) {
        stats[p.pid].pid = p.pid;
        stats[p.pid].tat_values.push_back(p.completion_time - p.arrival_time);
        stats[p.pid].wt_values.push_back((p.completion_time - p.arrival_time) - p.burst_time);
        stats[p.pid].rt_values.push_back(p.start_time - p.arrival_time);
    }
    
}



void simulateFCFS(const vector<Process>& procs_input, vector<ProcessStats>& stats){
    //copy the input vector so that we do not modify the original processes
    vector<Process> procs = procs_input;
    //get the number of processes
    int n = procs.size();
    //vector to store the timeline of processes
    vector<int> timeline;

    //initialize the clock time, number of completed processes, and the index for the next arrival
    int time = 0, completed = 0, idx = 0;

    //queue to manage the processes in the order they arrive which is FCFS
    queue<int> fq;

    //initialize each process:
    //set remaining time to burst time (the total CPU time needed)
    //set start time to -1 to mark that the process hasn't started yet
    //set started flag to false, indicating the process hasn't begun execution
    for (auto &p : procs) {
        p.remaining_time = p.burst_time;  
        p.start_time = -1;                
        p.started = false;                
    }

    //sort processes by arrival time in tp ensures we always handle the earliest arriving process first
    sort(procs.begin(), procs.end(),
         [](auto &a, auto &b) { return a.arrival_time < b.arrival_time; });

    //main simulation loop to run until all processes are completed
    while (completed < n) { //this logic is to handle any processes that arrive while the current process is running
        //to add newly arrived processes into the queue
        while (idx < n && procs[idx].arrival_time <= time) {
            fq.push(idx);  
            idx++;     //move to the next process    
        }

        //here i handle the case where no processes are in the queue
        //so if no processes are in the queue increment time 
        if (fq.empty()) {
            time++;  
            continue;  //go back to the beginning of the loop
        }

        //pop the process at the front of the queue FCFS
        int i = fq.front(); fq.pop();

        //for each process that will start to execute set the start time as time and set the flag to true
        if (!procs[i].started) {
            procs[i].start_time = time;  
            procs[i].started = true;     
        }

        //since FCFS is nonpreemptive we have to make sure that any process executing must run until it finishes
        //so we record the process ID in the timeline vector and each cycle we decrement the remaining time and 
        //increment the time by 1 unit
        while (procs[i].remaining_time > 0) {
            timeline.push_back(procs[i].pid);  
            procs[i].remaining_time--;          
            time++;                            

            //this time to handle processes that arrive while a proc is executing
            while (idx < n && procs[idx].arrival_time <= time) {
                fq.push(idx);  
                idx++;        
            }
        }
        //when the proc executing is done update the time so the next proc in the queue starts
        //and increment the no of completed procs
        procs[i].completion_time = time;
        completed++;  
    }

    // After the simulation, print the results of the First-Come, First-Served scheduling
    cout << "\nFCFS results:\n";
    cout << "PID  Arrival  Burst  Start  Complete  Turnaround  Waiting  Response\n";
    
    // Initialize variables to calculate averages
    double total_tat = 0, total_wt = 0, total_rt = 0;

    // Loop through each process and compute its metrics
    for (auto &p : procs) {
        int tat = p.completion_time - p.arrival_time;  //turnaround time
        int wt = tat - p.burst_time;                    //waiting time
        int rt = p.start_time - p.arrival_time;         //response time 
        
        //calc the totals of each metric
        total_tat += tat;
        total_wt += wt;
        total_rt += rt;

        //just a nice way for printing the table of results
        cout << setw(3) << p.pid
             << setw(9) << p.arrival_time
             << setw(7) << p.burst_time
             << setw(7) << p.start_time
             << setw(11) << p.completion_time
             << setw(13) << tat
             << setw(8) << wt
             << setw(9) << rt << "\n";
    }

    //the averages for TAT WT RT
    cout << fixed << setprecision(2)  //fixed decimal precision to 2 for better readability
         << "Avg TAT: " << total_tat / n  
         << ", Avg WT: " << total_wt / n  
         << ", Avg RT: " << total_rt / n << "\n";  

   


    cout << "\nTime Interval | Running Process\n"; //header for the chart

    int start = 0, last_pid = timeline[0]; //start is first zero bec the first proc starts at time 0
    //and the last pid keeps track of the last proc running and its initialized to the first proc in the timeline
    //bec the first proc whill be the first to finish running
    for (int t = 1; t <= (int)timeline.size(); ++t) { //loop through the timeline
        
        //current proc= the time at t if its not the end of the timeline which will be -1
        int pid = (t < (int)timeline.size() ? timeline[t] : -1);
        
       //printing the time interval for the proc running and setting start and last pid 
        if (pid != last_pid) {
            cout << setw(2) << start << " - " << setw(2) << t << "       "
                 << 'P' << last_pid << "\n";  
            start = t;  
            last_pid = pid;  
        }
    }

    for (const auto &p : procs) {
        stats[p.pid].pid = p.pid;
        stats[p.pid].tat_values.push_back(p.completion_time - p.arrival_time);
        stats[p.pid].wt_values.push_back((p.completion_time - p.arrival_time) - p.burst_time);
        stats[p.pid].rt_values.push_back(p.start_time - p.arrival_time);
    }
    

}


void simulateMLFQ(const vector<Process>& procs_input, const vector<int>& quanta, vector<ProcessStats>& stats){    vector<Process> procs = procs_input;
    int n = procs.size();
    int levels = quanta.size(); //numbers of levels in MLFQ is the size of quanta 
    vector<queue<int>> mlfq(levels); //create a queue for each level
    vector<function<void(vector<Process>&, int)>> schedulingFuncs(levels); //created a function pointers for scheduling algorithms
    vector<int> timeline;
    int time = 0, completed = 0, idx = 0;

    //initial procs like any sim
    for (auto &p : procs) {
        p.remaining_time = p.burst_time;
        p.start_time = -1;
        p.started = false;
    }
    //sort the procs by arrival time like any sim
    sort(procs.begin(), procs.end(), [](const Process &a, const Process &b) {
        return a.arrival_time < b.arrival_time;
    });

    // for each queue we point to a scheduling algo so here for level 0 i used RR and for lower levels i used FCFS
    schedulingFuncs[0] = [&](vector<Process>& procs, int quantum) { 
        vector<ProcessStats> stats(procs.size());
        simulateRR(procs, quantum, stats); 
    }; 
    for (int i = 1; i < levels; ++i) {
        schedulingFuncs[i] = [&](vector<Process>& procs, int) { 
            vector<ProcessStats> stats(procs.size());
            simulateFCFS(procs, stats); 
        }; 
    }

    //sim loop 
    while (completed < n) {
        //push newly arrived procs into the top level queue
        while (idx < n && procs[idx].arrival_time <= time) {
            mlfq[0].push(idx);
            idx++;
        }

        //find the first nonempty queue to allocate the new procs and if no queues are empty increment time
        int lvl = 0;
        while (lvl < levels && mlfq[lvl].empty()) lvl++;

        if (lvl == levels) { 
            time++;
            continue;
        }

        //since we sorted the procs by arrival time we can just pop the first proc in the queue
        int i = mlfq[lvl].front();
        mlfq[lvl].pop();

        //set the start time and started flag for the proc like any sim
        if (!procs[i].started) {
            procs[i].start_time = time;
            procs[i].started = true;
        }

        //execute the procs using the scheduling function for the current level
        int slice = (lvl == 0) ? min(quanta[lvl], procs[i].remaining_time) : procs[i].remaining_time;
        for (int t = 0; t < slice; ++t) {
            timeline.push_back(procs[i].pid);
            procs[i].remaining_time--;
            time++;
            while (idx < n && procs[idx].arrival_time <= time) {
                mlfq[0].push(idx);
                idx++;
            }
            if (procs[i].remaining_time == 0) break;
        }

        //after the time slice is over we have two options:
        //1 that the proc is done executing in which we save the completion time and ++ the no of completed procs
        //2 the proc is not done executing in which we need to "age" this proc to lower priority queue to avoid starving other procs
        if (procs[i].remaining_time == 0) {
            procs[i].completion_time = time;
            completed++;
        } else {
            
            if (lvl < levels - 1) {
                mlfq[lvl + 1].push(i);
            } else {
                mlfq[lvl].push(i); //while aging the proc we need to check if the next level is not out of bounds
            }
        }
        
    }

    
    cout << "\nMLFQ results:\n";
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
         << "Avg TAT: " << total_tat / n
         << ", Avg WT: " << total_wt / n
         << ", Avg RT: " << total_rt / n << "\n";

    
    cout << "\nTime Interval | Running Process\n";
    int start = 0, last_pid = timeline[0];
    for (int t = 1; t <= (int)timeline.size(); ++t) {
        int pid = (t < (int)timeline.size() ? timeline[t] : -1);
        if (pid != last_pid) {
            cout << setw(2) << start << " - " << setw(2) << t << "       "
                 << 'P' << last_pid << "\n";
            start = t;
            last_pid = pid;
        }
    }

    for (const auto &p : procs) {
        stats[p.pid].pid = p.pid;
        stats[p.pid].tat_values.push_back(p.completion_time - p.arrival_time);
        stats[p.pid].wt_values.push_back((p.completion_time - p.arrival_time) - p.burst_time);
        stats[p.pid].rt_values.push_back(p.start_time - p.arrival_time);
    }
    
}


void summarizeStats(const vector<vector<ProcessStats>>& all_stats, const string& algoname) {
    cout << "\nSummary Statistics for " << algoname << " (across runs):\n";
    cout << "PID  Avg_TAT  Med_TAT  Std_TAT  Avg_WT  Med_WT  Std_WT  Avg_RT  Med_RT  Std_RT\n";

    int num_procs = all_stats[0].size();

    for (int pid = 0; pid < num_procs; ++pid) {
        vector<int> tat_all, wt_all, rt_all;

        for (int run = 0; run < all_stats.size(); ++run) {
            tat_all.push_back(all_stats[run][pid].tat_values[0]);
            wt_all.push_back(all_stats[run][pid].wt_values[0]);
            rt_all.push_back(all_stats[run][pid].rt_values[0]);
        }

        cout << setw(3) << pid
             << setw(9) << average(tat_all)
             << setw(9) << median(tat_all)
             << setw(9) << stddev(tat_all)
             << setw(9) << average(wt_all)
             << setw(9) << median(wt_all)
             << setw(9) << stddev(wt_all)
             << setw(9) << average(rt_all)
             << setw(9) << median(rt_all)
             << setw(9) << stddev(rt_all)
             << "\n";
    }
}



int main() {
    const int runs = 5;
    const int num_processes = 100;
    // int quantum = 3;
    // vector<int> quanta = {8, 4, 2};



    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<> arrival_dist(0.5);
    std::exponential_distribution<> burst_dist(0.2);
    std::uniform_int_distribution<> q_dist(2, 6); // random quantum [2,6]
    std::uniform_int_distribution<> mlfq_dist(3, 10); // quanta range for MLFQ levels

    int quantum = q_dist(gen); // random RR quantum
    vector<int> quanta = {mlfq_dist(gen), mlfq_dist(gen), mlfq_dist(gen)}; // random MLFQ quanta

    cout << "=== Quantum for Round Robin: " << quantum << "\n";
    cout << "=== Quanta for MLFQ levels: ";
    for (int q : quanta) cout << q << " ";
    cout << "\n";

    // Stats containers
    vector<vector<ProcessStats>> stats_rr(runs, vector<ProcessStats>(num_processes));
    vector<vector<ProcessStats>> stats_srtf(runs, vector<ProcessStats>(num_processes));
    vector<vector<ProcessStats>> stats_fcfs(runs, vector<ProcessStats>(num_processes));
    vector<vector<ProcessStats>> stats_mlfq(runs, vector<ProcessStats>(num_processes));

    for (int r = 0; r < runs; ++r) {
        // Generate new processes
        double current_arrival = 0.0;
        vector<Process> procs;
        cout << "\n=== Process Queue for Run " << r + 1 << " ===\n";
        cout << "PID  Arrival  Burst\n";
        for (int i = 0; i < num_processes; ++i) {
            current_arrival += arrival_dist(gen);
            int burst = static_cast<int>(burst_dist(gen)) + 1;
            int arrival = static_cast<int>(current_arrival);
            procs.push_back({i, arrival, burst});
            cout << setw(3) << i << setw(9) << arrival << setw(7) << burst << "\n";
        }

        // Run all schedulers
        simulateSRTF(procs, stats_srtf[r]);
        simulateFCFS(procs, stats_fcfs[r]);
        simulateMLFQ(procs, quanta, stats_mlfq[r]);
        simulateRR(procs, quantum, stats_rr[r]);
    }

    // Aggregate statistics
    vector<ProcessStats> rr_combined(num_processes), srtf_combined(num_processes), fcfs_combined(num_processes), mlfq_combined(num_processes);
    for (int r = 0; r < runs; ++r) {
        for (int i = 0; i < num_processes; ++i) {
            auto combine = [](ProcessStats& out, const ProcessStats& in) {
                out.pid = in.pid;
                out.tat_values.insert(out.tat_values.end(), in.tat_values.begin(), in.tat_values.end());
                out.wt_values.insert(out.wt_values.end(), in.wt_values.begin(), in.wt_values.end());
                out.rt_values.insert(out.rt_values.end(), in.rt_values.begin(), in.rt_values.end());
            };
            combine(rr_combined[i], stats_rr[r][i]);
            combine(srtf_combined[i], stats_srtf[r][i]);
            combine(fcfs_combined[i], stats_fcfs[r][i]);
            combine(mlfq_combined[i], stats_mlfq[r][i]);
        }
    }

    printSummary("Round Robin", rr_combined);
    printSummary("SRTF", srtf_combined);
    printSummary("FCFS", fcfs_combined);
    printSummary("MLFQ", mlfq_combined);

    auto get_avg = [](const vector<ProcessStats>& stats, string name) {
        vector<int> all_wt, all_tat;
        for (const auto& ps : stats) {
            all_wt.insert(all_wt.end(), ps.wt_values.begin(), ps.wt_values.end());
            all_tat.insert(all_tat.end(), ps.tat_values.begin(), ps.tat_values.end());
        }
        cout << ">>> " << name << " AVG Waiting Time: " << average(all_wt)
             << ", AVG Turnaround Time: " << average(all_tat) << "\n";
    };

    // === Compute average WT and TAT for each algorithm ===
    vector<int> all_wt_rr, all_tat_rr, all_rt_rr;
    vector<int> all_wt_srtf, all_tat_srtf, all_rt_srtf;
    vector<int> all_wt_fcfs, all_tat_fcfs, all_rt_fcfs;
    vector<int> all_wt_mlfq, all_tat_mlfq, all_rt_mlfq;
    

    for (const auto& ps : rr_combined) {
        all_wt_rr.insert(all_wt_rr.end(), ps.wt_values.begin(), ps.wt_values.end());
        all_tat_rr.insert(all_tat_rr.end(), ps.tat_values.begin(), ps.tat_values.end());
        all_rt_rr.insert(all_rt_rr.end(), ps.rt_values.begin(), ps.rt_values.end());
    }
    
    for (const auto& ps : srtf_combined) {
        all_wt_srtf.insert(all_wt_srtf.end(), ps.wt_values.begin(), ps.wt_values.end());
        all_tat_srtf.insert(all_tat_srtf.end(), ps.tat_values.begin(), ps.tat_values.end());
        all_rt_srtf.insert(all_rt_srtf.end(), ps.rt_values.begin(), ps.rt_values.end());
    }
    
    for (const auto& ps : fcfs_combined) {
        all_wt_fcfs.insert(all_wt_fcfs.end(), ps.wt_values.begin(), ps.wt_values.end());
        all_tat_fcfs.insert(all_tat_fcfs.end(), ps.tat_values.begin(), ps.tat_values.end());
        all_rt_fcfs.insert(all_rt_fcfs.end(), ps.rt_values.begin(), ps.rt_values.end());
    }
    
    for (const auto& ps : mlfq_combined) {
        all_wt_mlfq.insert(all_wt_mlfq.end(), ps.wt_values.begin(), ps.wt_values.end());
        all_tat_mlfq.insert(all_tat_mlfq.end(), ps.tat_values.begin(), ps.tat_values.end());
        all_rt_mlfq.insert(all_rt_mlfq.end(), ps.rt_values.begin(), ps.rt_values.end());
    }
    

    // === Print the averages for report ===
    cout << "\n== Overall Averages ==\n";
    cout << "RR    : WT = " << average(all_wt_rr) << ", TAT = " << average(all_tat_rr) << "\n";
    cout << "SRTF  : WT = " << average(all_wt_srtf) << ", TAT = " << average(all_tat_srtf) << "\n";
    cout << "FCFS  : WT = " << average(all_wt_fcfs) << ", TAT = " << average(all_tat_fcfs) << "\n";
    cout << "MLFQ  : WT = " << average(all_wt_mlfq) << ", TAT = " << average(all_tat_mlfq) << "\n";

    /// === Export to summary.csv ===
    ofstream summary_csv("summary.csv");
    summary_csv << "Algorithm,AvgWaitingTime,AvgTurnaroundTime,AvgResponseTime\n";
    summary_csv << "RR," << average(all_wt_rr) << "," << average(all_tat_rr) << "," << average(all_rt_rr) << "\n";
    summary_csv << "SRTF," << average(all_wt_srtf) << "," << average(all_tat_srtf) << "," << average(all_rt_srtf) << "\n";
    summary_csv << "FCFS," << average(all_wt_fcfs) << "," << average(all_tat_fcfs) << "," << average(all_rt_fcfs) << "\n";
    summary_csv << "MLFQ," << average(all_wt_mlfq) << "," << average(all_tat_mlfq) << "," << average(all_rt_mlfq) << "\n";

    summary_csv.close();

    cout << "Saved summary: " << filesystem::absolute("summary.csv") << "\n";



    exportStatsToCSV("rr_stats.csv", rr_combined);
    cout << "Saved: " << filesystem::absolute("rr_stats.csv") << "\n";
    exportStatsToCSV("srtf_stats.csv", srtf_combined);
    cout << "Saved: " << filesystem::absolute("srtf_stats.csv") << "\n";
    exportStatsToCSV("fcfs_stats.csv", fcfs_combined);
    cout << "Saved: " << filesystem::absolute("fcfs_stats.csv") << "\n";
    exportStatsToCSV("mlfq_stats.csv", mlfq_combined);
    cout << "Saved: " << filesystem::absolute("mlfq_stats.csv") << "\n";


    return 0;
}
