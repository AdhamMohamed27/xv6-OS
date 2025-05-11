#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
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
         << ", Avg WT: " << total_wt/n
         << ", Avg RT: " << total_rt/n << "\n";

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

void simulateFCFS(const vector<Process>& procs_input) {
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
}

void simulateMLFQ(const vector<Process>& procs_input, const vector<int>& quanta) {
    vector<Process> procs = procs_input;
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

    //for each queue we point to a scheduling algo so here for level 0 i used RR and for lower levels i used FCFS
    schedulingFuncs[0] = [&](vector<Process>& procs, int quantum) { simulateRR(procs, quantum); }; 
    for (int i = 1; i < levels; ++i) {
        schedulingFuncs[i] = [&](vector<Process>& procs, int) { simulateFCFS(procs); }; 
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
}

int main() {

    vector<Process> procs = {
        {0, 0, 24},
        {1, 1, 3},
        {2, 2, 3},
        {3, 3, 5},
        {4, 4, 17}
    };
    int quantum = 4;

    simulateRR(procs, quantum);
    simulateSRTF(procs);
    simulateFCFS(procs);
    vector<int> quanta = {10, 10, 20};  // assign the quanta for each level from here 
    simulateMLFQ(procs, quanta);

    return 0;

}
