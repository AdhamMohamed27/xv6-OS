#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
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
    //i decide the number of levels in the MLFQ based on the amount of time quantum i provide
    int levels = quanta.size();
    //input vector so that we don't modify the original data
    vector<Process> procs = procs_input;
    
    //num of procs and completed procs count and curr proc index and clock time initial
    int n = procs.size(), completed = 0, idx = 0, time = 0;
    
    //timeline vector to store the order of procs execution
    vector<int> timeline;
    
    //create a queue for each prio level and each queue holds proc idx and queue will be fifo
    vector<queue<int>> mlfq(levels);

    //initial all procs like all 3 sims
    for (auto &p : procs) {
        p.remaining_time = p.burst_time;
        p.start_time = -1;
        p.started = false;
    }

    //sort procs by arrival time first is at the front 
    sort(procs.begin(), procs.end(),
         [](auto &a, auto &b) { return a.arrival_time < b.arrival_time; });

    //sim loop continues until all procs are completed
    while (completed < n) {
        //enqueue all procs that have arrived by the current time into the top queue (level 0)
        while (idx < n && procs[idx].arrival_time <= time) {
            mlfq[0].push(idx); 
            idx++;  
        }

        //whenever a proc arrives we check the queue levels and find the first nonempty queue and place the proc in it
        int lvl = 0;
        while (lvl < levels && mlfq[lvl].empty()) lvl++;  

        //again same logic like any sim if no procs are in any queue ready to execute increment time
        if (lvl == levels) {
            time++;  
            continue; 
        }

        //get the proc at the front of the highest nonempty queue and pop it for execution
        int i = mlfq[lvl].front();
        mlfq[lvl].pop(); 

        //if the proc didnt start before mark its start time and set started flag to true 
        if (!procs[i].started) {
            procs[i].start_time = time; 
            procs[i].started = true;    
        }

        //here is the logic of MLFQ which we allow a proc in any level to execute for the time qunatum of that level OR 
        //if the proc finishes exedution before the time slice expires
        int slice = min(quanta[lvl], procs[i].remaining_time);  
        for (int t = 0; t < slice; ++t) {  
            timeline.push_back(procs[i].pid);  //save the proc ID in the timeline
            procs[i].remaining_time--;          
            time++;                            

            //same logic like any sim to ensure that any proc that arrives while the current proc is executing is added to the queue
            while (idx < n && procs[idx].arrival_time <= time) {
                mlfq[0].push(idx);  
                idx++;  
            }
        }

        //after the time slice is over we have two options:
        //1 that the proc is done executing in which we save the completion time and ++ the no of completed procs
        //2 the proc is not done executing in which we need to "age" this proc to lower priority queue to avoid starving other procs
        if (procs[i].remaining_time == 0) {  
            procs[i].completion_time = time; 
            completed++;  
        } else {  
            int next_lvl = min(lvl + 1, levels - 1); //while aging the proc we need to check if the next level is not out of bounds
            mlfq[next_lvl].push(i);  
        }
    }

    //same format as other sims
    cout << "\nMLFQ results:\n";
    cout << "PID  Arrival  Burst  Start  Complete  Turnaround  Waiting  Response\n";
    double total_tat=0, total_wt=0, total_rt=0;
    for (auto &p : procs) {
        int tat = p.completion_time - p.arrival_time;
        int wt  = tat - p.burst_time;
        int rt  = p.start_time  - p.arrival_time;
        total_tat += tat; total_wt  += wt; total_rt += rt;
        cout << setw(3)<<p.pid
             << setw(9)<<p.arrival_time
             << setw(7)<<p.burst_time
             << setw(7)<<p.start_time
             << setw(11)<<p.completion_time
             << setw(13)<<tat
             << setw(8)<<wt
             << setw(9)<<rt<<"\n";
    }
    cout << fixed<<setprecision(2)
         << "Avg TAT: "<< total_tat/n
         << ", Avg WT: "<< total_wt/n
         << ", Avg RT: "<< total_rt/n <<"\n";

   
    cout << "\nTime Interval | Running Process\n";
    int start=0, last=timeline[0];

    for (int t=1; t<=(int)timeline.size(); ++t) {
        int pid = (t<(int)timeline.size()? timeline[t]: -1);

        if (pid != last) {
            cout << setw(2)<<start<<" - "<<setw(2)<<t<<"       "
                 << 'P'<<last<<"\n";
            start = t; last = pid;
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
    vector<int> quanta = {4, 10, 20};  // example: 3 levels
    simulateMLFQ(procs, quanta);


    return 0;

}
